#include "magma/app/mouse.h"
#include "magma/vk/Window.h"

#include <iostream>

Mouse::Mouse(Window *window):
    window(window),
    prev_pos(0,0),
    shift(0,0),
    locked(false)
{
    double x_pos, y_pos;
    glfwGetCursorPos(window->getGlfwWindow(), &x_pos, &y_pos);
    pos = glm::ivec2(x_pos, y_pos);

//    glfwSetCursorPosCallback(window, cursor_position_callback);
}

void Mouse::update()
{
    prev_pos = pos;
    double x_pos, y_pos;
    glfwGetCursorPos(window->getGlfwWindow(), &x_pos, &y_pos);
    pos = glm::ivec2(x_pos, y_pos);

    if (!locked &&
        ((pos.x > window->getResolution().width) || (pos.x < 0) ||
        (pos.y > window->getResolution().height) || (pos.y < 0)))
    {
        shift = glm::ivec2(0, 0);
    }
    else
    {
        shift = pos - prev_pos;
    }

//        std::cout << shift.x << "_" << shift.y <<std::endl;
}

const glm::ivec2 &Mouse::getShift() const
{
    return shift;
}

void Mouse::lock()
{
    auto resolution = window->getResolution();
    glfwSetCursorPos(window->getGlfwWindow(), resolution.width/2, resolution.height/2);
    glfwSetInputMode(window->getGlfwWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    update();
    locked = true;
}

void Mouse::unlock()
{
    glfwSetInputMode(window->getGlfwWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    locked = false;
}
