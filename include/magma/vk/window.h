#pragma once
#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>

#include "magma/app/keyboard.h"
#include "magma/app/mouse.h"
#include "magma/glm_inc.h"

VkSurfaceKHR createSurface(const VkInstance &instance, GLFWwindow* window);

class Window {
public:
    Window(uint32_t width, uint32_t height);
    ~Window();

    static void initContext();
    static std::vector<const char*> getRequiredVkExtensions();

    VkExtent2D getResolution() const { return {width, height}; }
    void       updateResolution();

    void initSurface(const VkInstance &instance);
    void closeSurface();

    // Meh? Is this good?
    GLFWwindow  *getGLFWp()         { return window;         }
    VkSurfaceKHR getSurface() const { return surface;        }
    bool         wasResized() const { return _wasResized;    }
    Keyboard    *getKeyboard()      { return keyboard.get(); }
    Mouse       *getMouse()         { return mouse.get();    }

private:
    uint32_t width, height;
    GLFWwindow* window;
    std::unique_ptr<Keyboard> keyboard;
    std::unique_ptr<Mouse> mouse;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkInstance instance = VK_NULL_HANDLE;
    bool _wasResized = false;

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
};
