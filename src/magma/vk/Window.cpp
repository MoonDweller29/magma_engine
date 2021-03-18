#include "magma/vk/Window.h"

#include <iostream>

#include "magma/vk/vulkan_common.h"
#include "magma/app/log.hpp"

Window::Window(uint32_t width, uint32_t height, vk::Instance instance)
        :_width(width),
        _height(height),
        _instance(instance),
        _wasResized(false),
        _window(sf::VideoMode(width, height), "Magma Engine", sf::Style::Default)
{
    _keyboard = std::make_unique<Keyboard>();

    _mouse = std::make_unique<Mouse>(_window);

    _surface = createSurface(_instance, _window);
}

Window::~Window() {
    _instance.destroySurfaceKHR(_surface);
    _keyboard.reset();
    _mouse.reset();
}

void Window::setTitle(const std::string &title) {
    _window.setTitle(title.c_str());
}

void Window::updateEvents() {
    sf::Event event;
    _keyboard->flush();

    while(_window.pollEvent(event)) {

        switch (event.type) {
            case sf::Event::Closed:
                _window.close();
                break;
            case sf::Event::Resized:
                _wasResized = true;
                break;

            case sf::Event::KeyPressed:
                _keyboard->addPressed(event.key.code);
                break;
            case sf::Event::KeyReleased:
                _keyboard->addReleased(event.key.code);
                break;
            default:
                break;
        }

    }
    // Closed,                 //!< The window requested to be closed (no data)
    // Resized,                //!< The window was resized (data in event.size)
    // LostFocus,              //!< The window lost the focus (no data)
    // GainedFocus,            //!< The window gained the focus (no data)
    // TextEntered,            //!< A character was entered (data in event.text)
    // KeyPressed,             //!< A key was pressed (data in event.key)
    // KeyReleased,            //!< A key was released (data in event.key)
    // MouseWheelMoved,        //!< The mouse wheel was scrolled (data in event.mouseWheel) (deprecated)
    // MouseWheelScrolled,     //!< The mouse wheel was scrolled (data in event.mouseWheelScroll)
    // MouseButtonPressed,     //!< A mouse button was pressed (data in event.mouseButton)
    // MouseButtonReleased,    //!< A mouse button was released (data in event.mouseButton)
    // MouseMoved,             //!< The mouse cursor moved (data in event.mouseMove)
    // MouseEntered,           //!< The mouse cursor entered the area of the window (no data)
    // MouseLeft,              //!< The mouse cursor left the area of the window (no data)
    // JoystickButtonPressed,  //!< A joystick button was pressed (data in event.joystickButton)
    // JoystickButtonReleased, //!< A joystick button was released (data in event.joystickButton)
    // JoystickMoved,          //!< The joystick moved along an axis (data in event.joystickMove)
    // JoystickConnected,      //!< A joystick was connected (data in event.joystickConnect)
    // JoystickDisconnected,   //!< A joystick was disconnected (data in event.joystickConnect)
    // TouchBegan,             //!< A touch event began (data in event.touch)
    // TouchMoved,             //!< A touch moved (data in event.touch)
    // TouchEnded,             //!< A touch event ended (data in event.touch)
    // SensorChanged,          //!< A sensor value changed (data in event.sensor)
    // Count                   //!< Keep last -- the total number of event types
}

std::vector<const char*> Window::getRequiredVkExtensions() {
    std::vector<const char*> extensions = sf::Vulkan::getGraphicsRequiredInstanceExtensions();
    return extensions;
}

void Window::updateResolution() {
    int new_w = 0, new_h = 0;

    new_w = _window.getSize().x;
    new_h = _window.getSize().y;
    while (new_w == 0 || new_h == 0) {
        new_w = _window.getSize().x;
        new_h = _window.getSize().y;
    }
    _width = new_w;
    _height = new_h;

    _wasResized = false;
}

vk::SurfaceKHR Window::createSurface(vk::Instance hpp_instance, sf::WindowBase &window) {
    VkInstance instance(hpp_instance);
    VkSurfaceKHR surface;
    if (!window.createVulkanSurface(instance, surface))
        LOG_WARNING("Failed to create window surface!");

    return surface;
}
