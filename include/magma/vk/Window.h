#pragma once
#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <memory>

#include "magma/app/keyboard.h"
#include "magma/app/mouse.h"
#include "magma/glm_inc.h"
#include "magma/vk/vulkan_common.h"

class Window {
public:
    Window(uint32_t width, uint32_t height, vk::Instance instance);
    NON_COPYABLE(Window);
    ~Window();

    static void initContext();
    static void closeContext();
    static std::vector<const char*> getRequiredVkExtensions();

    VkExtent2D getResolution() const { return {_width, _height}; }
    void       updateResolution();

    void setTitle(const std::string &title);

    GLFWwindow *   getGlfwWindow()    { return _window;         }
    vk::SurfaceKHR getSurface() const { return _surface;        }
    bool           wasResized() const { return _wasResized;     }
    Keyboard *     getKeyboard()      { return _keyboard.get(); }
    Mouse *        getMouse()         { return _mouse.get();    }

private:
    uint32_t                   _width, _height;
    bool                       _wasResized;
    GLFWwindow               * _window;
    std::unique_ptr<Keyboard>  _keyboard;
    std::unique_ptr<Mouse>     _mouse;
    vk::SurfaceKHR             _surface;
    vk::Instance               _instance;

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    static vk::SurfaceKHR createSurface(vk::Instance hpp_instance, GLFWwindow* window);
};
