#include "App.h"
#include "vk/vulkan_common.h"
#include "vk/window.h"
#include <iostream>

static bool esc_was_pressed = false;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
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

void App::initVulkan()
{
    instance = std::make_unique<VkInstanceHolder>();
    debugMessenger = std::make_unique<DebugMessenger>(instance->get());
    window->initSurface(instance->get());
    physicalDevice = std::make_unique<PhysicalDevice>(instance->get(), window->getSurface());
    device = std::make_unique<LogicalDeviceHolder>(*physicalDevice);

    swapChain = std::make_unique<SwapChain>(device->handler(), *physicalDevice, *window);

    renderPass = std::make_unique<RenderPass>(device->handler(), swapChain->getImageFormat());
    swapChain->createFrameBuffers(renderPass->getHandler());

    PipelineInfo pipelineInfo(window->getResolution());
    graphicsPipeline = std::make_unique<GraphicsPipeline>(device->handler(), pipelineInfo, renderPass->getHandler());

    commandPool = std::make_unique<CommandPool>(
            physicalDevice->getQueueFamilyInds().graphicsFamily.value(),
            device->handler());
    commandBuffers.allocate(device->handler(), commandPool->getHandler(), swapChain->imgCount());
    commandBuffers.record(
            renderPass->getHandler(),
            window->getResolution(),
            swapChain->getVkFrameBuffers(),
            graphicsPipeline->getHandler());
    createSyncObjects();
}

void App::drawFrame()
{
    vkWaitForFences(device->handler(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(
            device->handler(), swapChain->getSwapChain(),
            UINT64_MAX/*timeout off*/, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

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

    VkResult result = vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]);
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

    vkQueuePresentKHR(device->getPresentQueue(), &presentInfo);
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void App::mainLoop()
{
    while (!glfwWindowShouldClose(window->getGLFWp()))
    {
        glfwPollEvents();
        if (isClosed())
        	break;
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
    commandPool.reset();
    graphicsPipeline.reset();
    swapChain->clearFrameBuffers();
    renderPass.reset();
    swapChain.reset();
    device.reset();
    physicalDevice.reset();
    std::cout << "6\n";
    window->closeSurface();
    std::cout << "7\n";
    debugMessenger.reset();
    instance.reset();
    window.reset();

    glfwTerminate();
}