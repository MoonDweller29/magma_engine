#include "window.h"

#include "vulkan_common.h"
#include <iostream>


VkSurfaceKHR createSurface(
        const VkInstance &instance,
        GLFWwindow* window)
{
    VkSurfaceKHR surface;
    VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
    vk_check_err(result, "failed to create window surface!");

    return surface;
}

Window::Window(uint32_t width, uint32_t height)
{
    this->width = width;
    this->height = height;
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //call that turns off OpenGL context
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); //potential problem

    window = glfwCreateWindow(width, height, "Triangle", nullptr, nullptr);
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


Window::~Window()
{
    closeSurface();
    glfwDestroyWindow(window);
}