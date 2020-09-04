#include "App.h"
#include "vk/vulkan_common.h"
#include "vk/window.h"
#include <iostream>
#include "glm_inc.h"
#include "image.h"
#include <chrono>

const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
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
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2, VkVertexInputAttributeDescription{});

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);


    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

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
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1; //this can be used to create an array of uniform values
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    descriptorSetLayouts.resize(1);
    VkResult result = vkCreateDescriptorSetLayout(device->handler(), &layoutInfo, nullptr, &descriptorSetLayouts[0]);
    vk_check_err(result, "failed to create descriptor set layout!");
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

void App::createTextureImage()
{
    Image img("../logo.png", 4);
    img.save("../logo1.png");
}

void App::createDescriptorPool()
{
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(swapChain->imgCount());

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(swapChain->imgCount());
    poolInfo.flags = 0; //Optional

    VkResult result = vkCreateDescriptorPool(device->handler(), &poolInfo, nullptr, &descriptorPool);
    vk_check_err(result, "failed to create descriptor pool!");
}

void App::createDescriptorSets()
{
    size_t volatile size = descriptorSets.size();
    std::vector<VkDescriptorSetLayout> layouts(swapChain->imgCount(), descriptorSetLayouts[0]);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(layouts.size());
    VkResult result = vkAllocateDescriptorSets(device->handler(), &allocInfo, descriptorSets.data());
    vk_check_err(result, "failed to allocate descriptor sets!");

    for (size_t i = 0; i < layouts.size(); i++)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i].buf;
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0; //like in shader
        descriptorWrite.dstArrayElement = 0; //first index in array
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr; // Optional
        descriptorWrite.pTexelBufferView = nullptr; // Optional

        vkUpdateDescriptorSets(device->handler(), 1, &descriptorWrite, 0, nullptr);
    }
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

    swapChain = std::make_unique<SwapChain>(device->handler(), *physicalDevice, *window);
    createDescriptorSetLayout();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
    createTextureImage();

    renderPass = std::make_unique<RenderPass>(device->handler(), swapChain->getImageFormat());
    swapChain->createFrameBuffers(renderPass->getHandler());

    PipelineInfo pipelineInfo(window->getResolution());
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    pipelineInfo.setVertexInputInfo(bindingDescription, attributeDescriptions);
    pipelineInfo.setLayouts(descriptorSetLayouts);
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

    vkDestroyDescriptorPool(device->handler(), descriptorPool, nullptr);
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

    swapChain = std::make_unique<SwapChain>(device->handler(), *physicalDevice, *window);
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();

    renderPass = std::make_unique<RenderPass>(device->handler(), swapChain->getImageFormat());
    swapChain->createFrameBuffers(renderPass->getHandler());

    PipelineInfo pipelineInfo(window->getResolution());
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    pipelineInfo.setVertexInputInfo(bindingDescription, attributeDescriptions);
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
    vkDestroyDescriptorSetLayout(device->handler(), descriptorSetLayouts[0], nullptr);
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