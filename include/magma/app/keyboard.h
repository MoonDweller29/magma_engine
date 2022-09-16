#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <array>


class Keyboard
{
    GLFWwindow *window;

    std::array<int, GLFW_KEY_LAST> keyWasPressed;
    std::array<int, GLFW_KEY_LAST> keyWasReleased;

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
public:
    Keyboard(GLFWwindow *window);
    void flush();

    bool isPressed(int key);
    bool wasPressed(int key);
    bool wasReleased(int key);

    ~Keyboard();
};
