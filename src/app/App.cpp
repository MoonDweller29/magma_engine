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

void App::createSemaphores()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkResult result = vkCreateSemaphore(device->handler(), &semaphoreInfo, nullptr, &imageAvailableSemaphore);
    vk_check_err(result, "failed to create semaphores!");
    result = vkCreateSemaphore(device->handler(), &semaphoreInfo, nullptr, &renderFinishedSemaphore);
    vk_check_err(result, "failed to create semaphores!");
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
    createSemaphores();
}

void App::drawFrame()
{
    uint32_t imageIndex;
    vkAcquireNextImageKHR(
            device->handler(), swapChain->getSwapChain(),
            UINT64_MAX/*timeout off*/, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    //Submitting the command buffer
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VkResult result = vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
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
    vkDestroySemaphore(device->handler(), renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(device->handler(), imageAvailableSemaphore, nullptr);
    commandPool.reset();
    graphicsPipeline.reset();
    swapChain->clearFrameBuffers();
    renderPass.reset();
    swapChain.reset();
    device.reset();
    physicalDevice.reset();
    window.reset();
    debugMessenger.reset();
    instance.reset();

    glfwTerminate();
}