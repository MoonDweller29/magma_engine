#include "magma/vk/window.h"

#include "magma/vk/vulkan_common.h"
#include <iostream>

void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
//    bool volatile loop = true;
    Window *app_window = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
    app_window->_wasResized = true;
//    while (loop);
//    std::cout << "RESIZED\n";
}

VkSurfaceKHR createSurface(
        const VkInstance &instance,
        GLFWwindow* window)
{
    VkSurfaceKHR surface;
    VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
    VK_CHECK_ERR(result, "failed to create window surface!");

    return surface;
}

Window::Window(uint32_t width, uint32_t height)
{
    this->width = width;
    this->height = height;
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //call that turns off OpenGL context
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); //potential problem. Resizable window is unstable

    window = glfwCreateWindow(width, height, "Vulkan 3D", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    keyboard = std::make_unique<Keyboard>(window);
    mouse = std::make_unique<Mouse>(this);
}

void Window::initSurface(const VkInstance &instance)
{
    this->instance = instance;
    surface = createSurface(instance, window);
}

void Window::closeSurface()
{
    if (surface != VK_NULL_HANDLE)
        vkDestroySurfaceKHR(instance, surface, nullptr);
    surface = VK_NULL_HANDLE;
}

void Window::updateResolution()
{
    int new_w = 0, new_h = 0;

    glfwGetFramebufferSize(window, &new_w, &new_h);
    while (new_w == 0 || new_h == 0)
    {
        glfwWaitEvents();
        glfwGetFramebufferSize(window, &new_w, &new_h);
    }
    width = new_w;
    height = new_h;

    _wasResized = false;
}

Window::~Window()
{
    closeSurface();
    keyboard.reset();
    mouse.reset();
    glfwDestroyWindow(window);
}
