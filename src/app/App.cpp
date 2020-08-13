#include "App.h"
#include "vk/vulkan_common.h"

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
    glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //call that turns off OpenGL context
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); //potential problem

	window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Triangle", nullptr, nullptr);
	glfwSetKeyCallback(window, key_callback);
}


void App::initVulkan()
{
    instance = std::make_unique<VkInstanceHolder>();
    debugMessenger = std::make_unique<DebugMessenger>(instance->get());
}

void App::mainLoop()
{
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (isClosed())
        	break;
    }
}

void App::cleanUp()
{
    debugMessenger.reset();
    instance.reset();
    glfwDestroyWindow(window);

    glfwTerminate();
}