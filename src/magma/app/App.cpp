#include <chrono>
#include <iostream>
#include <sstream>

#include "magma/app/App.h"
#include "magma/vk/buffers/BufferManager.h"
#include "magma/vk/textures/TextureManager.h"
#include "magma/vk/vulkan_common.h"
#include "magma/vk/physicalDevice/HardwareManager.h"
#include "magma/glm_inc.h"
#include "magma/app/image.h"
#include "magma/app/config/JSON.h"


struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

struct FragmentUniform {
    alignas(16) glm::vec3 cameraPos;
    alignas(16) glm::vec3 lightPos;
};

struct LightSpaceUniform {
    glm::mat4 lightSpaceMat;
};

static std::string joinPath(const std::string &s1, const std::string &s2) {
    if (s1[s1.size() - 1] == '/') {
        return s1 + s2;
    } else {
        return s1 + "/" + s2;
    }
}


bool App::isClosed() {
	return _keyBoard->wasPressed(GLFW_KEY_ESCAPE);
}

int App::run() {
    try {
        initFromConfig();
        initVulkan();
        _mainCamera = std::make_unique<Camera>(0.1, 100, WIN_WIDTH, WIN_HEIGHT, 90.0f);
        mainLoop();
        cleanUp();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


void App::initFromConfig() {
    JSON buildInfo = json::load<JSON>(_buildInfoFilename);
    _dataPath = joinPath(buildInfo["build_info"]["src_dir"], "data");
    _texturePath = joinPath(_dataPath, "textures/viking_room.png");
    _modelPath = joinPath(_dataPath, "models/viking_room.obj");
    JSON main_config = json::load<JSON>(joinPath(_dataPath, "config.json"));

    Log::Config log_config = main_config["logger"].get<Log::Config>();
    Log::initFromConfig(log_config);
    DebugMessenger::Config dbg_config = main_config["validation_layers"].get<DebugMessenger::Config>();
    DebugMessenger::initConfig(dbg_config);

    WIN_WIDTH = main_config["win_width"];
    WIN_HEIGHT = main_config["win_height"];

    LOG_INFO("Inited from config file");
}

void App::initWindow() {
    _window = std::make_unique<Window>(WIN_WIDTH, WIN_HEIGHT, _instance->instance());
    _keyBoard = _window->getKeyboard();
    _mouse = _window->getMouse();
}

void App::createSyncObjects() {
    _imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkResult result = vkCreateSemaphore(_device->c_getDevice(), &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]);
        VK_CHECK_ERR(result, "failed to create semaphores!");
    }
}

void App::createUniformBuffers() {
    uint32_t imgCount = _swapChain->imgCount();

    BufferManager& bufferManager = _device->getBufferManager();
    _uniformBuffer = bufferManager.createUniformBuffer("uniformBuffer", sizeof(UniformBufferObject));
    _fragmentUniform = bufferManager.createUniformBuffer("fragmentUniform", sizeof(FragmentUniform));
}

void App::loadScene() {
    _texture = _device->getTextureManager().loadTexture("input_texture", _texturePath);
    _scene = _meshReader.load_scene(_modelPath);
    _vertices = _scene[0].getVertices();
    _indices = _scene[0].getIndices();

    _light = std::make_unique<DirectLight>(
            glm::vec3(1,1,0), glm::vec3(0,0,0) - glm::vec3(1,1,0),
            0.1f, 20.f);
}

vk::Sampler App::createDefaultTextureSampler(vk::Filter minFilter, vk::Filter magFilter) {
    vk::SamplerCreateInfo samplerInfo;
    samplerInfo.magFilter = magFilter;
    samplerInfo.minFilter = minFilter;
    samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
    samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
    samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16.0f;
    samplerInfo.borderColor = vk::BorderColor::eFloatOpaqueBlack;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = vk::CompareOp::eAlways;

    samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    auto[result, sampler] = _device->getDevice().createSampler(samplerInfo);
    VK_CHECK_ERR(result, "failed to create texture sampler!");

    return sampler;
}

void App::createShadowMapTex() {
    _shadowMap = _device->getTextureManager().createTexture2D("shadowMap_texture",
        vk::Format::eD32Sfloat,
        vk::Extent2D{2048, 2048},
        vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled,
        vk::ImageAspectFlagBits::eDepth);
    _shadowMapSampler = createDefaultTextureSampler(vk::Filter::eNearest, vk::Filter::eNearest);
}

void App::createShadowMapResources() {
    createShadowMapTex();

    BufferManager& bufferManager = _device->getBufferManager();
    _shadowUniform = bufferManager.createUniformBuffer("shadowUniform", sizeof(UniformBufferObject));
    _lightSpaceUniform = bufferManager.createUniformBuffer("lightSpaceUniform", sizeof(LightSpaceUniform));

    _renderShadow = std::make_unique<DepthPass>(_device->getDevice(), _shadowMap,
                                               vk::ImageLayout::eShaderReadOnlyOptimal,
                                               _device->getGraphicsQueue());
    _renderShadow->writeDescriptorSets(_shadowUniform, sizeof(UniformBufferObject));
    _renderShadow->recordCmdBuffers(_indexBuffer.getBuf(), _vertexBuffer.getBuf(), _indices.size());
}

void App::createMainRenderTarget() {
    _mainRenderTarget = _device->getTextureManager().createTexture2D("main_render_target",
        vk::Format::eR16G16B16A16Unorm, _gBuffer->getExtent(),
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
        vk::ImageAspectFlagBits::eColor);
}


void App::initDevice() {
    HardwareManager hardwareMGR(_instance->instance());

    DeviceRequirements deviceRequirements;
    deviceRequirements.surface.require(_window->getSurface());
    deviceRequirements.deviceType.recommend(vk::PhysicalDeviceType::eDiscreteGpu);
    deviceRequirements.graphicsSupport.require(true);
    deviceRequirements.computeSupport.require(true);

    vk::PhysicalDeviceFeatures physicalDeviceFeatures;
    physicalDeviceFeatures.samplerAnisotropy = true;
    deviceRequirements.features.require(physicalDeviceFeatures);

    auto physDevice = hardwareMGR.selectBestSuitableDevice(deviceRequirements);

    _device = std::make_unique<LogicalDevice>(
            physDevice,
            deviceRequirements.features.getValue(),
            deviceRequirements.requiredExtensions());
}

void App::initVulkan() {
    _instance = std::make_unique<Context>();
    _debugMessenger = std::make_unique<DebugMessenger>(_instance->c_instance());
    initWindow();
    initDevice();

    loadScene();

    BufferManager& bufferManager = _device->getBufferManager();
    _vertexBuffer = bufferManager.createVertexBuffer("vertexBuffer", _vertices);
    _indexBuffer = bufferManager.createIndexBuffer("indexBuffer", _indices);

    _swapChain = std::make_unique<SwapChain>(*_device, *_window);
    createUniformBuffers();
    _gBuffer = std::make_unique<GBuffer>(_device->getTextureManager(), _window->getResolution());
    createMainRenderTarget();
    _textureSampler = createDefaultTextureSampler(vk::Filter::eLinear, vk::Filter::eLinear);

    createShadowMapResources();

    _depthPass = std::make_unique<DepthPass>(_device->getDevice(), _gBuffer->getDepth(),
                                            vk::ImageLayout::eDepthStencilAttachmentOptimal,
                                            _device->getGraphicsQueue());
    _depthPass->writeDescriptorSets(_uniformBuffer, sizeof(UniformBufferObject));
    _depthPass->recordCmdBuffers(_indexBuffer.getBuf(), _vertexBuffer.getBuf(), _indices.size());


    _mainColorPass = std::make_unique<MainColorPass>(_device->getDevice(), *_gBuffer, _device->getGraphicsQueue());
    _mainColorPass->writeDescriptorSets(_uniformBuffer, sizeof(UniformBufferObject),
                                       _texture.getView(), _textureSampler);
    _mainColorPass->recordCmdBuffers(_indexBuffer.getBuf(), _vertexBuffer.getBuf(), _indices.size());
    _gBufferResolve = std::make_unique<GBufferResolve>(_device->getDevice(), _mainRenderTarget, _device->getGraphicsQueue());
    _gBufferResolve->writeDescriptorSets(*_gBuffer, _shadowMap.getView(), _shadowMapSampler,
                                        _fragmentUniform, sizeof(FragmentUniform),
                                        _lightSpaceUniform, sizeof(LightSpaceUniform));
    _gBufferResolve->recordCmdBuffers();
    _swapChainImageSupplier = std::make_unique<SwapChainImageSupplier>(
            _device->getDevice(), _mainRenderTarget.getView(), *_swapChain, _device->getGraphicsQueue()
    );
    _swapChainImageSupplier->recordCmdBuffers();

    createSyncObjects();
}

void App::cleanupSwapChain() {
    BufferManager& bufferManager = _device->getBufferManager();
    bufferManager.deleteBuffer(_uniformBuffer);
    bufferManager.deleteBuffer(_fragmentUniform);

    _gBuffer.reset();
    _device->getTextureManager().deleteTexture(_mainRenderTarget);

    _mainColorPass.reset();
    _gBufferResolve.reset();
    _swapChainImageSupplier.reset();
    _depthPass.reset();
    _swapChain.reset();
}

void App::recreateSwapChain() {
    _device->waitIdle();
    cleanupSwapChain();

    _window->updateResolution();
    VkExtent2D res = _window->getResolution();
    WIN_WIDTH = res.width;
    WIN_HEIGHT = res.height;

    _swapChain = std::make_unique<SwapChain>(*_device, *_window);
    createUniformBuffers();
    _gBuffer = std::make_unique<GBuffer>(_device->getTextureManager(), _window->getResolution());
    createMainRenderTarget();

    _depthPass = std::make_unique<DepthPass>(_device->getDevice(), _gBuffer->getDepth(),
                                            vk::ImageLayout::eDepthStencilAttachmentOptimal,
                                            _device->getGraphicsQueue());
    _depthPass->writeDescriptorSets(_uniformBuffer, sizeof(UniformBufferObject));
    _depthPass->recordCmdBuffers(_indexBuffer.getBuf(), _vertexBuffer.getBuf(), _indices.size());


    _mainColorPass = std::make_unique<MainColorPass>(_device->getDevice(), *_gBuffer, _device->getGraphicsQueue());
    _mainColorPass->writeDescriptorSets(_uniformBuffer, sizeof(UniformBufferObject),
                                       _texture.getView(), _textureSampler);
    _mainColorPass->recordCmdBuffers(_indexBuffer.getBuf(), _vertexBuffer.getBuf(), _indices.size());
    _gBufferResolve = std::make_unique<GBufferResolve>(_device->getDevice(), _mainRenderTarget, _device->getGraphicsQueue());
    _gBufferResolve->writeDescriptorSets(*_gBuffer, _shadowMap.getView(), _shadowMapSampler,
                                        _fragmentUniform, sizeof(FragmentUniform),
                                        _lightSpaceUniform, sizeof(LightSpaceUniform));
    _gBufferResolve->recordCmdBuffers();
    _swapChainImageSupplier = std::make_unique<SwapChainImageSupplier>(
            _device->getDevice(), _mainRenderTarget.getView(), *_swapChain, _device->getGraphicsQueue()
    );
    _swapChainImageSupplier->recordCmdBuffers();
    _mainCamera->updateScreenSize(WIN_WIDTH, WIN_HEIGHT);
}

void App::drawFrame() {
    _swapChainImageSupplier->getSync().waitForFence();

    if (_window->wasResized()) {
        recreateSwapChain();
    }

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
            _device->c_getDevice(), _swapChain->getSwapChain(),
            UINT64_MAX/*timeout off*/, _imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        LOG_AND_THROW std::runtime_error("failed to acquire swap chain image!");
    }

    updateUniformBuffer(imageIndex);
    updateShadowUniform();

    CmdSync depthPassSync = _depthPass->draw({}, {});
    CmdSync shadowPassSync = _renderShadow->draw({}, {});
    const CmdSync &mainColorPassSync = _mainColorPass->draw(
            { depthPassSync.getSemaphore() }, {}
    );
    const CmdSync &gBufferResolveSync = _gBufferResolve->draw(
            { mainColorPassSync.getSemaphore(), shadowPassSync.getSemaphore() }, {}
    );
    const CmdSync &swapChainImageSupplierSync = _swapChainImageSupplier->draw(
            imageIndex, { _imageAvailableSemaphores[_currentFrame], gBufferResolveSync.getSemaphore() }, {}
    );


    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    VkSemaphore colorPassSemaphore = swapChainImageSupplierSync.getSemaphore();
    presentInfo.pWaitSemaphores = &colorPassSemaphore;

    VkSwapchainKHR swapChains[] = {_swapChain->getSwapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    result = vkQueuePresentKHR(_device->getPresentQueue().queue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _window->wasResized()) {
        recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        LOG_AND_THROW std::runtime_error("failed to present swap chain image!");
    }

    _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}


void App::updateUniformBuffer(uint32_t currentImage) {
    float time = _global_clock.getTime();
    static bool light_view = false;

    if (_keyBoard->wasPressed(GLFW_KEY_2))
        light_view = !light_view;

    UniformBufferObject ubo{};
//    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.model = glm::mat4x4( 1.0f );
    ubo.view = _mainCamera->getViewMat();
    ubo.proj = _mainCamera->getProjMat();
    if (light_view) {
        ubo.view = _light->getView();
        ubo.proj = _light->getProj();
    }


    FragmentUniform fu{};
    fu.cameraPos = _mainCamera->getPos();
    fu.lightPos = _light->getPos();

    BufferManager& bufferManager = _device->getBufferManager();
    bufferManager.copyDataToBuffer(_uniformBuffer, &ubo, sizeof(ubo));
    bufferManager.copyDataToBuffer(_fragmentUniform, &fu, sizeof(fu));
}

void App::updateShadowUniform() {
    UniformBufferObject ubo{};
    ubo.model = glm::mat4x4( 1.0f );
    ubo.view = _light->getView();
    ubo.proj = _light->getProj();

    LightSpaceUniform lu{};
    lu.lightSpaceMat = ubo.proj * ubo.view;

    BufferManager& bufferManager = _device->getBufferManager();
    bufferManager.copyDataToBuffer(_shadowUniform, &ubo, sizeof(ubo));
    bufferManager.copyDataToBuffer(_lightSpaceUniform, &lu, sizeof(lu));
}

void App::mainLoop() {
    float prev_time = _global_clock.restart();
    int frames_count = 0;

    _mainCamera->setPos({1, 1, 1});
    _mainCamera->lookAt({0, 0, 0});

    while (!glfwWindowShouldClose(_window->getGlfwWindow())) {
        float time = _global_clock.getTime();
        float frameTime = time-prev_time;
        prev_time = time;

        if(frames_count % 100 == 0) {
            std::stringstream ss;
            ss << "MAGMA ENGINE | FPS: " << 1 / frameTime;
            _window->setTitle(ss.str());
        }
        frames_count++;

        _keyBoard->flush();
        glfwPollEvents();
        _mouse->update();
        _mainCamera->update(*_keyBoard, *_mouse, frameTime);
//        light->lookAt(glm::vec3(0,0,0), glm::vec3(sin(time), 0.4f, cos(time)));
        _light->lookAt(glm::vec3(0,0,0), glm::vec3(5.0f*sin(time), 5.0f, 5.0f*cos(time)));

        // @TODO Move this to input class
        bool isLeftMouseButtonPressed  = glfwGetMouseButton(_window->getGlfwWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
        bool isLeftMouseButtonReleased = glfwGetMouseButton(_window->getGlfwWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE;

        // @TODO Add window focus handling
        if (_mouse->isLocked()) {
            if (isLeftMouseButtonReleased) {
                _mouse->unlock();
            }
        } else {
            if (isLeftMouseButtonPressed) {
                _mouse->lock();
            }
        }

        if (isClosed()) {
          break;
        }

        drawFrame();
    }

    _device->waitIdle();
}

void App::cleanUp() {
    std::cout << "CLEAN UP\n";
    BufferManager& bufferManager = _device->getBufferManager();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(_device->c_getDevice(), _imageAvailableSemaphores[i], nullptr);
    }

    cleanupSwapChain();
    bufferManager.deleteBuffer(_shadowUniform);
    bufferManager.deleteBuffer(_lightSpaceUniform);
    _renderShadow.reset();
    vkDestroySampler(_device->c_getDevice(), _textureSampler, nullptr);
    vkDestroySampler(_device->c_getDevice(), _shadowMapSampler, nullptr);
    _device->getTextureManager().deleteTexture(_shadowMap);
    _device->getTextureManager().deleteTexture(_texture);
    bufferManager.deleteBuffer(_indexBuffer);
    bufferManager.deleteBuffer(_vertexBuffer);
    _device.reset();
    _window.reset();
    _debugMessenger.reset();
    _instance.reset();

    Window::closeContext();
}
