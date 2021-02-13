#include <chrono>
#include <iostream>
#include <sstream>

#include "magma/app/App.h"
#include "magma/vk/textures/TextureManager.h"
#include "magma/vk/vulkan_common.h"
#include "magma/glm_inc.h"
#include "magma/app/image.h"
#include "magma/app/config/JSON.h"


//const std::vector<Vertex> vertices = {
//        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
//        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
//        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
//        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
//
//        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
//        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
//        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
//        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
//};
//
//const std::vector<uint32_t> indices = {
//        0, 1, 2, 2, 3, 0,
//        4, 5, 6, 6, 7, 4
//};

// Why is this still here? Just to suffer?
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


bool App::isClosed()
{
	return keyBoard->wasPressed(GLFW_KEY_ESCAPE);
}

int App::run() {
    try {
        initFromConfig();
        initVulkan();
        mainCamera = std::make_unique<Camera>(0.1, 100, WIN_WIDTH, WIN_HEIGHT, 90.0f);
        mainLoop();
        cleanUp();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


void App::initFromConfig() {
    JSON buildInfo = json::load<JSON>(buildInfoFilename);
    dataPath = joinPath(buildInfo["build_info"]["src_dir"], "data");
    texturePath = joinPath(dataPath, "textures/viking_room.png");
    modelPath = joinPath(dataPath, "models/viking_room.obj");
    JSON main_config = json::load<JSON>(joinPath(dataPath, "config.json"));

    Log::Config log_config = main_config["logger"].get<Log::Config>();
    Log::initFromConfig(log_config);
    DebugMessenger::Config dbg_config = main_config["validation_layers"].get<DebugMessenger::Config>();
    DebugMessenger::initConfig(dbg_config);

    WIN_WIDTH = main_config["win_width"];
    WIN_HEIGHT = main_config["win_height"];

    LOG_INFO("Inited from config file");
}

void App::initWindow()
{
    window = std::make_unique<Window>(WIN_WIDTH, WIN_HEIGHT, instance->instance());
    keyBoard = window->getKeyboard();
    mouse = window->getMouse();
}

void App::createSyncObjects()
{
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        VkResult result = vkCreateSemaphore(device->handler(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]);
        VK_CHECK_ERR(result, "failed to create semaphores!");
    }
}

void App::createUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    uint32_t imgCount = swapChain->imgCount();

    uniformBuffer = device->createUniformBuffer(bufferSize);

    fragmentUniform = device->createUniformBuffer(bufferSize);
}

void App::loadScene()
{
    scene = meshReader.load_scene(modelPath);
    vertices = scene[0].getVertices();
    indices = scene[0].getIndices();

    light = std::make_unique<DirectLight>(
            glm::vec3(1,1,0), glm::vec3(0,0,0) - glm::vec3(1,1,0),
            0.1f, 20.f);
}


void App::createTexture()
{
    texture = device->getTextureManager().loadTexture("input_texture", texturePath);
}

void App::createTextureSampler()
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    VkResult result = vkCreateSampler(device->handler(), &samplerInfo, nullptr, &textureSampler);
    VK_CHECK_ERR(result, "failed to create texture sampler!");
}

void App::createShadowMapSampler()
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_NEAREST;
    samplerInfo.minFilter = VK_FILTER_NEAREST;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    VkResult result = vkCreateSampler(device->handler(), &samplerInfo, nullptr, &shadowMapSampler);
    VK_CHECK_ERR(result, "failed to create texture sampler!");
}


void App::createShadowMapTex()
{
    shadowMap = device->getTextureManager().createTexture2D("shadowMap_texture", 
        VK_FORMAT_D32_SFLOAT,
        VkExtent2D{2048, 2048},
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_IMAGE_ASPECT_DEPTH_BIT);
    createShadowMapSampler();
}

void App::createShadowMapResources()
{
    createShadowMapTex();
    shadowUniform = device->createUniformBuffer(sizeof(UniformBufferObject));
    lightSpaceUniform = device->createUniformBuffer(sizeof(LightSpaceUniform));
    renderShadow = std::make_unique<DepthPass>(*device, shadowMap, VkExtent2D{2048, 2048},
                                               VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    renderShadow->writeDescriptorSets(shadowUniform, sizeof(UniformBufferObject));
    renderShadow->recordCmdBuffers(indexBuffer.buf, vertexBuffer.buf, indices.size());
}


void App::createDepthResources()
{
    VkFormat depthFormat = findDepthFormat(physicalDevice->c_device());
    depthTex = device->getTextureManager().createTexture2D("depth_texture", 
        depthFormat, 
        VkExtent2D{WIN_WIDTH, WIN_HEIGHT}, 
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
        VK_IMAGE_ASPECT_DEPTH_BIT);
}


void App::initVulkan() {
    instance = std::make_unique<Context>();
    debugMessenger = std::make_unique<DebugMessenger>(instance->c_instance());
    initWindow();
    physicalDevice = std::make_unique<PhysicalDevice>(instance->c_instance(), window->getSurface());
    device = std::make_unique<LogicalDevice>(*physicalDevice);
    loadScene();
    vertexBuffer = device->createVertexBuffer(vertices);
    indexBuffer = device->createIndexBuffer(indices);

    swapChain = std::make_unique<SwapChain>(*device, *physicalDevice, *window);
    createUniformBuffers();
    createTexture();
    createDepthResources();
    createTextureSampler();
    createShadowMapResources();

    depthPass = std::make_unique<DepthPass>(*device, depthTex, VkExtent2D{WIN_WIDTH, WIN_HEIGHT},
                                            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    depthPass->writeDescriptorSets(uniformBuffer, sizeof(UniformBufferObject));
    depthPass->recordCmdBuffers(indexBuffer.buf, vertexBuffer.buf, indices.size());

    colorPass = std::make_unique<ColorPass>(*device, *swapChain);
    colorPass->writeDescriptorSets(uniformBuffer, sizeof(UniformBufferObject),
                                   fragmentUniform, sizeof(FragmentUniform),
                                   texture.getView(), textureSampler,
                                   lightSpaceUniform, sizeof(LightSpaceUniform),
                                   shadowMap.getView(), shadowMapSampler);
    swapChain->createFrameBuffers(colorPass->getRenderPass(), depthTex.getView());
    colorPass->recordCmdBuffers(
            indexBuffer.buf,
            vertexBuffer.buf,
            indices.size(),
            swapChain->getVkFrameBuffers()
    );

    createSyncObjects();
}

void App::cleanupSwapChain()
{
    device->deleteBuffer(uniformBuffer);
    device->deleteBuffer(fragmentUniform);

    device->getTextureManager().deleteTexture(depthTex);

    swapChain->clearFrameBuffers();
    colorPass.reset();
    depthPass.reset();
    swapChain.reset();
}

void App::recreateSwapChain()
{
    vkDeviceWaitIdle(device->handler());

    cleanupSwapChain();

    window->updateResolution();
    VkExtent2D res = window->getResolution();
    WIN_WIDTH = res.width;
    WIN_HEIGHT = res.height;

    swapChain = std::make_unique<SwapChain>(*device, *physicalDevice, *window);
    createUniformBuffers();
    createDepthResources();

    depthPass = std::make_unique<DepthPass>(*device, depthTex, VkExtent2D{WIN_WIDTH, WIN_HEIGHT},
                                            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    depthPass->writeDescriptorSets(uniformBuffer, sizeof(UniformBufferObject));
    depthPass->recordCmdBuffers(indexBuffer.buf, vertexBuffer.buf, indices.size());

    colorPass = std::make_unique<ColorPass>(*device, *swapChain);
    colorPass->writeDescriptorSets(uniformBuffer, sizeof(UniformBufferObject),
                                   fragmentUniform, sizeof(FragmentUniform),
                                   texture.getView(), textureSampler,
                                   lightSpaceUniform, sizeof(LightSpaceUniform),
                                   shadowMap.getView(), shadowMapSampler);
    swapChain->createFrameBuffers(colorPass->getRenderPass(), depthTex.getView());
    colorPass->recordCmdBuffers(
            indexBuffer.buf, vertexBuffer.buf, indices.size(),
            swapChain->getVkFrameBuffers()
    );
    mainCamera->updateScreenSize(WIN_WIDTH, WIN_HEIGHT);
}

void App::drawFrame()
{
    vkWaitForFences(device->handler(), 1, &colorPass->getSync().fence, VK_TRUE, UINT64_MAX);

    if (window->wasResized())
        recreateSwapChain();

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
            device->handler(), swapChain->getSwapChain(),
            UINT64_MAX/*timeout off*/, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    updateUniformBuffer(imageIndex);
    updateShadowUniform();

    std::vector<VkFence> waitFences = { colorPass->getSync().fence };
    std::vector<VkSemaphore> waitSemaphores;
    CmdSync depthPassSync = depthPass->draw(waitSemaphores, waitFences);
    CmdSync shadowPassSync = renderShadow->draw(waitSemaphores, waitFences);
    waitFences = { depthPassSync.fence, shadowPassSync.fence};
    waitSemaphores = { imageAvailableSemaphores[currentFrame], depthPassSync.semaphore, shadowPassSync.semaphore};
    CmdSync colorPassSync = colorPass->draw(imageIndex, waitSemaphores, waitFences);


    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &colorPassSync.semaphore;

    VkSwapchainKHR swapChains[] = {swapChain->getSwapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    result = vkQueuePresentKHR(device->getPresentQueue(), &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window->wasResized())
    {
        recreateSwapChain();
    } else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}


void App::updateUniformBuffer(uint32_t currentImage)
{
    float time = global_clock.getTime();
    static bool light_view = false;

    if (keyBoard->wasPressed(GLFW_KEY_2))
        light_view = !light_view;

    UniformBufferObject ubo{};
//    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.model = glm::mat4x4( 1.0f );
    ubo.view = mainCamera->getViewMat();
    ubo.proj = mainCamera->getProjMat();
    if (light_view)
    {
        ubo.view = light->getView();
        ubo.proj = light->getProj();
    }

    void* data_p;
    vkMapMemory(device->handler(), uniformBuffer.mem, 0, sizeof(ubo), 0, &data_p);
    memcpy(data_p, &ubo, sizeof(ubo));
    vkUnmapMemory(device->handler(), uniformBuffer.mem);

    FragmentUniform fu{};
    fu.cameraPos = mainCamera->getPos();
    fu.lightPos = light->getPos();
    vkMapMemory(device->handler(), fragmentUniform.mem, 0, sizeof(ubo), 0, &data_p);
    memcpy(data_p, &fu, sizeof(fu));
    vkUnmapMemory(device->handler(), fragmentUniform.mem);
}

void App::updateShadowUniform()
{
    UniformBufferObject ubo{};
    ubo.model = glm::mat4x4( 1.0f );
    ubo.view = light->getView();
    ubo.proj = light->getProj();

    void* data_p;
    vkMapMemory(device->handler(), shadowUniform.mem, 0, sizeof(ubo), 0, &data_p);
    memcpy(data_p, &ubo, sizeof(ubo));
    vkUnmapMemory(device->handler(), shadowUniform.mem);

    LightSpaceUniform lu{};
    lu.lightSpaceMat = ubo.proj * ubo.view;
    vkMapMemory(device->handler(), lightSpaceUniform.mem, 0, sizeof(lu), 0, &data_p);
    memcpy(data_p, &lu, sizeof(lu));
    vkUnmapMemory(device->handler(), lightSpaceUniform.mem);
}

void App::mainLoop()
{
    float prev_time = global_clock.restart();
    int frames_count = 0;

    while (!glfwWindowShouldClose(window->getGLFWp()))
    {
        float time = global_clock.getTime();
        float frameTime = time-prev_time;
        prev_time = time;

        if(frames_count % 100 == 0)
        {
            std::stringstream ss;

            ss << "VULKAN ENGINE | FPS: " << 1 / frameTime << std::endl;

            glfwSetWindowTitle(window->getGLFWp(), ss.str().c_str());
        }
        frames_count++;

        keyBoard->flush();
        glfwPollEvents();
        mouse->update();
        mainCamera->update(*keyBoard, *mouse, frameTime);
//        light->lookAt(glm::vec3(0,0,0), glm::vec3(sin(time), 0.4f, cos(time)));
        light->lookAt(glm::vec3(0,0,0), glm::vec3(5.0f*sin(time), 5.0f, 5.0f*cos(time)));

        // @TODO Move this to input class
        bool isLeftMouseButtonPressed  = glfwGetMouseButton(window->getGLFWp(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
        bool isLeftMouseButtonReleased = glfwGetMouseButton(window->getGLFWp(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE;

        // @TODO Add window focus handling
        if (mouse->isLocked()) {
          if (isLeftMouseButtonReleased)
          {
            mouse->unlock();
          }
        } else {
          if (isLeftMouseButtonPressed)
          {
            mouse->lock();
          }
        }

        if (isClosed())
        {
          break;
        }

        drawFrame();
    }

    vkDeviceWaitIdle(device->handler());
}

void App::cleanUp()
{
    std::cout << "CLEAN UP\n";

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(device->handler(), imageAvailableSemaphores[i], nullptr);
    }

    cleanupSwapChain();
    device->deleteBuffer(shadowUniform);
    device->deleteBuffer(lightSpaceUniform);
    renderShadow.reset();
    vkDestroySampler(device->handler(), textureSampler, nullptr);
    vkDestroySampler(device->handler(), shadowMapSampler, nullptr);
    device->getTextureManager().deleteTexture(shadowMap);
    device->getTextureManager().deleteTexture(texture);
    device->deleteBuffer(indexBuffer);
    device->deleteBuffer(vertexBuffer);
    device.reset();
    physicalDevice.reset();
    window.reset();
    debugMessenger.reset();
    instance.reset();

    Window::closeContext();
}
