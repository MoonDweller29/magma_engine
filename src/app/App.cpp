#include "App.h"
#include "vk/vulkan_common.h"
#include "vk/window.h"
#include <iostream>
#include <ctime>

const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
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

static bool esc_was_pressed = false;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    std::cout << "PRESSED\n";
    if ( ((key == GLFW_KEY_ESCAPE) || (key == GLFW_KEY_Q))  && action == GLFW_PRESS)
        esc_was_pressed = true;
}

bool App::isClosed()
{
	return esc_was_pressed;
}

void App::run()
{
    initWindow();
    initVulkan();
    mainLoop();
    cleanUp();
}

void App::initWindow()
{
    window = std::make_unique<Window>(WIN_WIDTH, WIN_HEIGHT);
	glfwSetKeyCallback(window->getGLFWp(), key_callback);
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

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void createBuffer(
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer& buffer,
        VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);
    vk_check_err(result, "failed to create vertex buffer!");

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

    result = vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory);
    vk_check_err(result, "failed to allocate vertex buffer memory!");

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void App::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool->getHandler();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device->handler(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    {
        VkBufferCopy copyRegion{}; //may be an array
        copyRegion.srcOffset = 0; // Optional
        copyRegion.dstOffset = 0; // Optional
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    }
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(device->getGraphicsQueue()); //TODO:: it's not optimal

    vkFreeCommandBuffers(device->handler(), commandPool->getHandler(), 1, &commandBuffer);
}

//TODO: move this functionality to logical device
void App::createVertexBuffer()
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(
            physicalDevice->device(), device->handler(),
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);

    //filling the staging buffer
    void* data;
    vkMapMemory(device->handler(), stagingBufferMemory, 0, bufferSize, 0, &data);
    {
        memcpy(data, vertices.data(), (size_t) bufferSize);
    }
    vkUnmapMemory(device->handler(), stagingBufferMemory);

    createBuffer(
            physicalDevice->device(), device->handler(),
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            vertexBuffer, vertexBufferMemory);

    copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(device->handler(), stagingBuffer, nullptr);
    vkFreeMemory(device->handler(), stagingBufferMemory, nullptr);
}

void App::createIndexBuffer() {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(
            physicalDevice->device(), device->handler(),
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(device->handler(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), (size_t) bufferSize);
    vkUnmapMemory(device->handler(), stagingBufferMemory);

    createBuffer(
            physicalDevice->device(), device->handler(),
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            indexBuffer, indexBufferMemory);

    copyBuffer(stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(device->handler(), stagingBuffer, nullptr);
    vkFreeMemory(device->handler(), stagingBufferMemory, nullptr);
}

void App::initVulkan()
{
    instance = std::make_unique<VkInstanceHolder>();
    debugMessenger = std::make_unique<DebugMessenger>(instance->get());
    window->initSurface(instance->get());
    physicalDevice = std::make_unique<PhysicalDevice>(instance->get(), window->getSurface());
    device = std::make_unique<LogicalDeviceHolder>(*physicalDevice);
    commandPool = std::make_unique<CommandPool>(
            physicalDevice->getQueueFamilyInds().graphicsFamily.value(),
            device->handler());
    createVertexBuffer();
    createIndexBuffer();

    swapChain = std::make_unique<SwapChain>(device->handler(), *physicalDevice, *window);

    renderPass = std::make_unique<RenderPass>(device->handler(), swapChain->getImageFormat());
    swapChain->createFrameBuffers(renderPass->getHandler());

    PipelineInfo pipelineInfo(window->getResolution());
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    pipelineInfo.setVertexInputInfo(bindingDescription, attributeDescriptions);
    graphicsPipeline = std::make_unique<GraphicsPipeline>(device->handler(), pipelineInfo, renderPass->getHandler());

    commandBuffers.allocate(device->handler(), commandPool->getHandler(), swapChain->imgCount());
    commandBuffers.record(
            indexBuffer,
            vertexBuffer,
            indices.size(),
            renderPass->getHandler(),
            window->getResolution(),
            swapChain->getVkFrameBuffers(),
            graphicsPipeline->getHandler());
    createSyncObjects();
}

void App::cleanupSwapChain()
{
    vkFreeCommandBuffers(
            device->handler(), commandPool->getHandler(),
            commandBuffers.size(), commandBuffers.data());

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

    renderPass = std::make_unique<RenderPass>(device->handler(), swapChain->getImageFormat());
    swapChain->createFrameBuffers(renderPass->getHandler());

    PipelineInfo pipelineInfo(window->getResolution());
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    pipelineInfo.setVertexInputInfo(bindingDescription, attributeDescriptions);
    graphicsPipeline = std::make_unique<GraphicsPipeline>(device->handler(), pipelineInfo, renderPass->getHandler());

    commandBuffers.allocate(device->handler(), commandPool->getHandler(), swapChain->imgCount());
    commandBuffers.record(
            indexBuffer,
            vertexBuffer,
            indices.size(),
            renderPass->getHandler(),
            window->getResolution(),
            swapChain->getVkFrameBuffers(),
            graphicsPipeline->getHandler());
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

void App::mainLoop()
{
    clock_t time, prev_time = 0;
    while (!glfwWindowShouldClose(window->getGLFWp()))
    {
        glfwPollEvents();
        if (isClosed())
        {
            break;
        }
        drawFrame();
        time = clock();
//        printf("%f \n",CLOCKS_PER_SEC/float(time - prev_time));
        prev_time = time;
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
    vkDestroyBuffer(device->handler(), indexBuffer, nullptr);
    vkFreeMemory(device->handler(), indexBufferMemory, nullptr);
    vkDestroyBuffer(device->handler(), vertexBuffer, nullptr);
    vkFreeMemory(device->handler(), vertexBufferMemory, nullptr);
    commandPool.reset();
    device.reset();
    physicalDevice.reset();
    window->closeSurface();
    debugMessenger.reset();
    instance.reset();
    window.reset();

    glfwTerminate();
}