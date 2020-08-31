#include "keyboard.h"
#include <iostream>
#include "vk/window.h"

void Keyboard::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
//    std::cout << "PRESSED\n";
    Keyboard *keyBoard = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window))->getKeyboard();
    if (action == GLFW_PRESS)
        keyBoard->keyWasPressed[key] = true;
    else if (action == GLFW_RELEASE)
        keyBoard->keyWasReleased[key] = true;
}

Keyboard::Keyboard(GLFWwindow* window): window(window)
{
    glfwSetKeyCallback(window, key_callback);
    flush();
}

void Keyboard::flush()
{
    keyWasPressed.fill(false);
    keyWasReleased.fill(false);
}

bool Keyboard::isPressed(int key)
{
    return glfwGetKey(window, key) == GLFW_PRESS;
}

bool Keyboard::wasPressed(int key)
{
    return keyWasPressed.at(key);
}

bool Keyboard::wasReleased(int key)
{
    return keyWasReleased.at(key);
}

Keyboard::~Keyboard()
{
    glfwSetKeyCallback(window, (GLFWkeyfun) nullptr);
}