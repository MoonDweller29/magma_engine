#include "App.h"
#include "vk/vulkan_common.h"
#include "vk/window.h"

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


void App::initVulkan()
{
    instance = std::make_unique<VkInstanceHolder>();
    debugMessenger = std::make_unique<DebugMessenger>(instance->get());
    window->initSurface(instance->get());
    physicalDevice = std::make_unique<PhysicalDevice>(instance->get(), window->getSurface());
    device = std::make_unique<LogicalDeviceHolder>(*physicalDevice);
    swapChain = std::make_unique<SwapChain>(device->handler(), *physicalDevice, *window);
    renderPass = std::make_unique<RenderPass>(device->handler(), swapChain->getImageFormat());
    PipelineInfo pipelineInfo(window->getResolution());
    graphicsPipeline = std::make_unique<GraphicsPipeline>(device->handler(), pipelineInfo, renderPass->getHandler());
}

void App::mainLoop()
{
    while (!glfwWindowShouldClose(window->getGLFWp())) {
        glfwPollEvents();
        if (isClosed())
        	break;
    }
}

void App::cleanUp()
{
    graphicsPipeline.reset();
    renderPass.reset();
    swapChain.reset();
    device.reset();
    physicalDevice.reset();
    window.reset();
    debugMessenger.reset();
    instance.reset();

    glfwTerminate();
}