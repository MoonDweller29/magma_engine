#include "App.h"
#include "vk/vulkan_common.h"
#include "vk/window.h"
#include <iostream>
#include "glm_inc.h"
#include "image.h"
#include <chrono>
#include <unistd.h> //for sleep

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

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

struct FragmentUniform {
    alignas(16) glm::vec3 cameraPos;
    alignas(16) glm::vec3 lightPos;
};


bool App::isClosed()
{
	return keyBoard->wasPressed(GLFW_KEY_ESCAPE) ||
	    keyBoard->wasPressed(GLFW_KEY_Q);
}

void App::run()
{
    initWindow();
    initVulkan();
    mainCamera = std::make_unique<Camera>(0.1, 100, WIN_WIDTH, WIN_HEIGHT, 90.0f);
    mainLoop();
    cleanUp();
}

void App::initWindow()
{
    window = std::make_unique<Window>(WIN_WIDTH, WIN_HEIGHT);
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
        vk_check_err(result, "failed to create semaphores!");
    }
}

void App::createUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    uint32_t imgCount = swapChain->imgCount();

    uniformBuffers.resize(imgCount);
    for (size_t i = 0; i < imgCount; i++)
    {
        uniformBuffers[i] = device->createUniformBuffer(bufferSize);
    }

    fragmentUniforms.resize(imgCount);
    for (size_t i = 0; i < imgCount; i++)
    {
        fragmentUniforms[i] = device->createUniformBuffer(bufferSize);
    }
}

void App::loadScene()
{
    scene = meshReader.load_scene("../models/viking_room.obj");
    vertices = scene[0].getVertices();
    indices = scene[0].getIndices();
}


void App::createTexture()
{
    Image img(TEXTURE_PATH.c_str(), 4);

    int imageSize = img.size();

    Buffer stagingBuffer = device->createBuffer(
            imageSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    void* data;
    vkMapMemory(device->handler(), stagingBuffer.mem, 0, imageSize, 0, &data);
        memcpy(data, img.data(), static_cast<size_t>(imageSize));
    vkUnmapMemory(device->handler(), stagingBuffer.mem);

    texture = device->createTexture2D(
            img.getWidth(), img.getHeight(),
            VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            VK_IMAGE_ASPECT_COLOR_BIT
    );
    device->transitionImageLayout(
            texture.img(), VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    device->copyBufferToImage(
            stagingBuffer.buf, texture.img(),
            static_cast<uint32_t>(img.getWidth()), static_cast<uint32_t>(img.getHeight()));
    device->transitionImageLayout(
            texture.img(), VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    device->deleteBuffer(stagingBuffer);
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
    vk_check_err(result, "failed to create texture sampler!");
}

void App::createDepthResources()
{
    VkFormat depthFormat = findDepthFormat(physicalDevice->device());
    depthTex = device->createTexture2D(
            WIN_WIDTH, WIN_HEIGHT, depthFormat,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            VK_IMAGE_ASPECT_DEPTH_BIT);
}


void App::initVulkan()
{
    instance = std::make_unique<VkInstanceHolder>();
    debugMessenger = std::make_unique<DebugMessenger>(instance->get());
    window->initSurface(instance->get());
    physicalDevice = std::make_unique<PhysicalDevice>(instance->get(), window->getSurface());
    device = std::make_unique<LogicalDevice>(*physicalDevice);
    loadScene();
    vertexBuffer = device->createVertexBuffer(vertices);
    indexBuffer = device->createIndexBuffer(indices);

    swapChain = std::make_unique<SwapChain>(*device, *physicalDevice, *window);
    createUniformBuffers();
    createTexture();
    createDepthResources();
    createTextureSampler();

    colorPass = std::make_unique<ColorPass>(*device, *swapChain);
    colorPass->writeDescriptorSets(uniformBuffers, sizeof(UniformBufferObject),
                                   fragmentUniforms, sizeof(FragmentUniform),
                                   texture.view(), textureSampler);
    swapChain->createFrameBuffers(colorPass->getRenderPass(), depthTex.view());
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
    for (size_t i = 0; i < uniformBuffers.size(); i++)
    {
        device->deleteBuffer(uniformBuffers[i]);
        device->deleteBuffer(fragmentUniforms[i]);
    }

    device->deleteTexture(depthTex);

    swapChain->clearFrameBuffers();
    colorPass.reset();
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

    colorPass = std::make_unique<ColorPass>(*device, *swapChain);
    colorPass->writeDescriptorSets(uniformBuffers, sizeof(UniformBufferObject),
                                   fragmentUniforms, sizeof(FragmentUniform),
                                   texture.view(), textureSampler);
    swapChain->createFrameBuffers(colorPass->getRenderPass(), depthTex.view());
    colorPass->recordCmdBuffers(
            indexBuffer.buf, vertexBuffer.buf, indices.size(),
            swapChain->getVkFrameBuffers()
    );
    mainCamera->updateScreenSize(WIN_WIDTH, WIN_HEIGHT);
}

void App::drawFrame()
{
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

    std::vector<VkFence> waitFences = { colorPass->getSync().fence };
    std::vector<VkSemaphore> waitSemaphores = { imageAvailableSemaphores[currentFrame] };
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

    UniformBufferObject ubo{};
//    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.model = glm::mat4x4( 1.0f );
    ubo.view = mainCamera->getViewMat();
    ubo.proj = mainCamera->getProjMat();

    void* data_p;
    vkMapMemory(device->handler(), uniformBuffers[currentImage].mem, 0, sizeof(ubo), 0, &data_p);
    memcpy(data_p, &ubo, sizeof(ubo));
    vkUnmapMemory(device->handler(), uniformBuffers[currentImage].mem);

    FragmentUniform fu{};
    fu.cameraPos = mainCamera->getPos();
    fu.lightPos = glm::vec3(sin(time), 0.4f, cos(time));
    vkMapMemory(device->handler(), fragmentUniforms[currentImage].mem, 0, sizeof(ubo), 0, &data_p);
    memcpy(data_p, &fu, sizeof(ubo));
    vkUnmapMemory(device->handler(), fragmentUniforms[currentImage].mem);
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
//            std::cout << 1 / frameTime << std::endl;
        }
        frames_count++;

        keyBoard->flush();
        glfwPollEvents();
        mouse->update();
        mainCamera->update(*keyBoard, *mouse, frameTime);

        if (keyBoard->wasPressed(GLFW_KEY_1))
        {
            if (mouse->isLocked())
                mouse->unlock();
            else
                mouse->lock();
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
    vkDestroySampler(device->handler(), textureSampler, nullptr);
    device->deleteTexture(texture);
    device->deleteBuffer(indexBuffer);
    device->deleteBuffer(vertexBuffer);
    device.reset();
    physicalDevice.reset();
    window->closeSurface();
    debugMessenger.reset();
    instance.reset();
    window.reset();

    glfwTerminate();
}