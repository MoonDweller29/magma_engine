#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

VkSurfaceKHR createSurface(const VkInstance &instance, GLFWwindow* window);

class Window
{
    uint32_t width, height;
    GLFWwindow* window;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkInstance instance = VK_NULL_HANDLE;
    bool _wasResized = false;

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
public:
    Window(uint32_t width, uint32_t height);

    VkExtent2D getResolution() const { return {width, height}; }
    void updateResolution();

    void initSurface(const VkInstance &instance);
    void closeSurface();
    GLFWwindow* getGLFWp() { return window; }
    VkSurfaceKHR getSurface() const { return surface; }
    bool wasResized() const { return _wasResized; }

    ~Window();
};