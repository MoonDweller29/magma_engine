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
    static void closeContext();
    static std::vector<const char*> getRequiredVkExtensions();

    VkExtent2D getResolution() const { return {_width, _height}; }
    void       updateResolution();

    void initSurface(const VkInstance &instance);
    void closeSurface();

    // Meh? Is this good?
    GLFWwindow * getGLFWp()         { return _window;         }
    VkSurfaceKHR getSurface() const { return _surface;        }
    bool         wasResized() const { return _wasResized;     }
    Keyboard *   getKeyboard()      { return _keyboard.get(); }
    Mouse *      getMouse()         { return _mouse.get();    }

private:
    uint32_t                   _width, _height;
    bool                       _wasResized = false;
    GLFWwindow                *_window;
    std::unique_ptr<Keyboard>  _keyboard;
    std::unique_ptr<Mouse>     _mouse;
    VkSurfaceKHR               _surface = VK_NULL_HANDLE;
    VkInstance                 _instance = VK_NULL_HANDLE;

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
};
