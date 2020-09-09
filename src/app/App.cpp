#include "App.h"
#include "vk/vulkan_common.h"
#include "vk/window.h"
#include <iostream>
#include "glm_inc.h"
#include "image.h"
#include <chrono>

const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4
};

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

std::vector<VkVertexInputBindingDescription> Vertex::getBindingDescription()
{
    std::vector<VkVertexInputBindingDescription>  bindingDescription(1, VkVertexInputBindingDescription{});
    bindingDescription[0].binding = 0;
    bindingDescription[0].stride = sizeof(Vertex);
    bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

std::vector<VkVertexInputAttributeDescription> Vertex::getAttributeDescriptions()
{
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3, VkVertexInputAttributeDescription{});

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

    return attributeDescriptions;
}

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
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight.resize(swapChain->imgCount(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        VkResult result = vkCreateSemaphore(device->handler(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]);
        vk_check_err(result, "failed to create semaphores!");
        result = vkCreateSemaphore(device->handler(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]);
        vk_check_err(result, "failed to create semaphores!");
        result = vkCreateFence(device->handler(), &fenceInfo, nullptr, &inFlightFences[i]);
        vk_check_err(result, "failed to create fences!");
    }
}

void App::createDescriptorSetLayout()
{
    descriptorSetLayout.addUniformBuffer(1, VK_SHADER_STAGE_VERTEX_BIT);
    descriptorSetLayout.addCombinedImageSampler(VK_SHADER_STAGE_FRAGMENT_BIT);
    descriptorSetLayout.createLayout(device->handler());
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
}

void App::createTexture()
{
    Image img("../logo.png", 4);
    img.save("../logo1.png");

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

void App::createDescriptorSets()
{
    uint32_t descriptorSetCount = swapChain->imgCount();
    descriptorSetLayout.allocateSets(descriptorSetCount);
    for (uint32_t i = 0; i < descriptorSetCount; ++i)
    {
        descriptorSetLayout.beginSet(i);
        descriptorSetLayout.bindUniformBuffer(0, uniformBuffers[i].buf, 0, sizeof(UniformBufferObject));
        descriptorSetLayout.bindCombinedImageSampler(1, texture.view(), textureSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
    descriptorSets = descriptorSetLayout.recordAndReturnSets();
}

void App::initVulkan()
{
    instance = std::make_unique<VkInstanceHolder>();
    debugMessenger = std::make_unique<DebugMessenger>(instance->get());
    window->initSurface(instance->get());
    physicalDevice = std::make_unique<PhysicalDevice>(instance->get(), window->getSurface());
    device = std::make_unique<LogicalDevice>(*physicalDevice);
    vertexBuffer = device->createVertexBuffer(vertices);
    indexBuffer = device->createIndexBuffer(indices);

    swapChain = std::make_unique<SwapChain>(*device, *physicalDevice, *window);
    createDescriptorSetLayout();
    createUniformBuffers();
    createTexture();
    createDepthResources();
    createTextureSampler();
    createDescriptorSets();

    renderPass = std::make_unique<RenderPass>(device->handler(), physicalDevice->device(), swapChain->getImageFormat());
    swapChain->createFrameBuffers(renderPass->getHandler(), depthTex.view());

    PipelineInfo pipelineInfo(window->getResolution());
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    pipelineInfo.setVertexInputInfo(bindingDescription, attributeDescriptions);
    pipelineInfo.setLayout(descriptorSetLayout.getLayout());
    graphicsPipeline = std::make_unique<GraphicsPipeline>(device->handler(), pipelineInfo, renderPass->getHandler());

    commandBuffers.allocate(device->handler(), device->getGraphicsCmdPool(), swapChain->imgCount());
    commandBuffers.record(
            indexBuffer.buf,
            vertexBuffer.buf,
            indices.size(),
            descriptorSets,
            renderPass->getHandler(),
            window->getResolution(),
            swapChain->getVkFrameBuffers(),
            *graphicsPipeline);
    createSyncObjects();
}

void App::cleanupSwapChain()
{
    for (size_t i = 0; i < uniformBuffers.size(); i++)
    {
        device->deleteBuffer(uniformBuffers[i]);
    }
    vkFreeCommandBuffers(
            device->handler(), device->getGraphicsCmdPool(),
            commandBuffers.size(), commandBuffers.data());

    device->deleteTexture(depthTex);

    descriptorSetLayout.freePool();
    graphicsPipeline.reset();
    swapChain->clearFrameBuffers();
    renderPass.reset();
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
    createDescriptorSets();

    renderPass = std::make_unique<RenderPass>(device->handler(), physicalDevice->device(), swapChain->getImageFormat());
    swapChain->createFrameBuffers(renderPass->getHandler(), depthTex.view());

    PipelineInfo pipelineInfo(window->getResolution());
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    pipelineInfo.setVertexInputInfo(bindingDescription, attributeDescriptions);
    pipelineInfo.setLayout(descriptorSetLayout.getLayout());
    graphicsPipeline = std::make_unique<GraphicsPipeline>(device->handler(), pipelineInfo, renderPass->getHandler());

    commandBuffers.allocate(device->handler(), device->getGraphicsCmdPool(), swapChain->imgCount());
    commandBuffers.record(
            indexBuffer.buf,
            vertexBuffer.buf,
            indices.size(),
            descriptorSets,
            renderPass->getHandler(),
            window->getResolution(),
            swapChain->getVkFrameBuffers(),
            *graphicsPipeline);
}

void App::drawFrame()
{
    vkWaitForFences(device->handler(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    if (window->wasResized())
        recreateSwapChain();

    uint32_t imageIndex;
    VkResult result;
    result = vkAcquireNextImageKHR(
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

    // Check if a previous frame is using this image (i.e. there is its fence to wait on)
    if (imagesInFlight[imageIndex] != VK_NULL_HANDLE)
    {
        vkWaitForFences(device->handler(), 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }
    // Mark the image as now being in use by this frame
    imagesInFlight[imageIndex] = inFlightFences[currentFrame];

    updateUniformBuffer(imageIndex);

    //Submitting the command buffer
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(device->handler(), 1, &inFlightFences[currentFrame]);

    result = vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]);
    vk_check_err(result, "failed to submit draw command buffer!");

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

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
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = mainCamera->getViewMat();
    ubo.proj = mainCamera->getProjMat();

    void* data_p;
    vkMapMemory(device->handler(), uniformBuffers[currentImage].mem, 0, sizeof(ubo), 0, &data_p);
    memcpy(data_p, &ubo, sizeof(ubo));
    vkUnmapMemory(device->handler(), uniformBuffers[currentImage].mem);
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
//        std::cout << 1 / (time - prev_time) << std::endl;
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
        vkDestroySemaphore(device->handler(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device->handler(), imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device->handler(), inFlightFences[i], nullptr);
    }

    cleanupSwapChain();
    descriptorSetLayout.clear();
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