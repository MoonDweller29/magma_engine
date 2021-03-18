#pragma once
#include <vulkan/vulkan.hpp>
#include "SFML/Window/WindowBase.hpp"
#include "SFML/Window/Event.hpp"
#include <vector>
#include <string>
#include <memory>

#include "magma/app/Keyboard.h"
#include "magma/app/Mouse.h"
#include "magma/glm_inc.h"

class Window {
public:
    Window(uint32_t width, uint32_t height, vk::Instance instance);
    ~Window();

    sf::WindowBase &getSfmlWindow()         { return _window;           }
    vk::SurfaceKHR  getSurface()    const   { return _surface;          }
    bool            wasResized()    const   { return _wasResized;       }
    Keyboard *      getKeyboard()           { return _keyboard.get();   }
    Mouse *         getMouse()              { return _mouse.get();      }
    VkExtent2D      getResolution() const   { return {_width, _height}; }

    void setTitle(const std::string &title);

    void updateEvents();
    void updateResolution();

    static std::vector<const char*> getRequiredVkExtensions();

private:
    uint32_t                   _width, _height;
    bool                       _wasResized;
    sf::WindowBase             _window;
    std::unique_ptr<Keyboard>  _keyboard;
    std::unique_ptr<Mouse>     _mouse;
    vk::SurfaceKHR             _surface;
    vk::Instance               _instance;

    static vk::SurfaceKHR createSurface(vk::Instance hpp_instance, sf::WindowBase &window);
};
