#include "magma/vk/Window.h"

#include "magma/vk/vulkan_common.h"
#include <iostream>

void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
//    bool volatile loop = true;
    Window *app_window = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
    app_window->_wasResized = true;
//    while (loop);
//    std::cout << "RESIZED\n";
}

VkSurfaceKHR createSurface(
        const VkInstance &instance,
        GLFWwindow* window
) {
    VkSurfaceKHR surface;
    VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
    VK_CHECK_ERR(result, "failed to create window surface!");

    return surface;
}

Window::Window(uint32_t width, uint32_t height) :
    _width(width), _height(height)
{
    initContext();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //call that turns off OpenGL context
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); //potential problem. Resizable window is unstable

    _window = glfwCreateWindow(width, height, "Vulkan 3D", nullptr, nullptr);
    glfwSetWindowUserPointer(_window, this);
    glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);
    _keyboard = std::make_unique<Keyboard>(_window);
    _mouse = std::make_unique<Mouse>(this);
}

void Window::initContext() {
    glfwInit();
}
void Window::closeContext() {
    glfwTerminate();
}


std::vector<const char*> Window::getRequiredVkExtensions() {
    initContext();

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    return extensions;
}


void Window::initSurface(const VkInstance &instance) {
    this->_instance = instance;
    _surface = createSurface(instance, _window);
}

void Window::closeSurface() {
    if (_surface != VK_NULL_HANDLE)
        vkDestroySurfaceKHR(_instance, _surface, nullptr);
    _surface = VK_NULL_HANDLE;
}

void Window::updateResolution() {
    int new_w = 0, new_h = 0;

    glfwGetFramebufferSize(_window, &new_w, &new_h);
    while (new_w == 0 || new_h == 0) {
        glfwWaitEvents();
        glfwGetFramebufferSize(_window, &new_w, &new_h);
    }
    _width = new_w;
    _height = new_h;

    _wasResized = false;
}

Window::~Window() {
    closeSurface();
    _keyboard.reset();
    _mouse.reset();
    glfwDestroyWindow(_window);
}
