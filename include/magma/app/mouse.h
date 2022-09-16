#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "magma/glm_inc.h"

class Window;

class Mouse
{
    Window *window;
    int winWidth, winHeight;

    glm::ivec2 pos;
    glm::ivec2 prev_pos;
    glm::ivec2 shift;

    bool locked;
public:
    Mouse(Window *window);
    void update();

    void lock();
    void unlock();

    const glm::ivec2 &getShift() const;
    bool isLocked() { return locked; }
};
