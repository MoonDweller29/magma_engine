#include "magma/app/Keyboard.h"
#include "SFML/Window/Keyboard.hpp"
#include <iostream>

void Keyboard::addPressed(int key) {
    _keyWasPressed[key] = true;
}

void Keyboard::addReleased(int key) {
    _keyWasReleased[key] = true;
}

void Keyboard::flush()
{
    _keyWasPressed.fill(false);
    _keyWasReleased.fill(false);
}

bool Keyboard::isPressed(int key)
{
    return _keyboard.isKeyPressed((sf::Keyboard::Key)key);
}

bool Keyboard::wasPressed(int key)
{
    return _keyWasPressed.at(key);
}

bool Keyboard::wasReleased(int key)
{
    return _keyWasReleased.at(key);
}