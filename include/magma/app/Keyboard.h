#pragma once
#include "SFML/Window/Keyboard.hpp"
#include <array>

class Keyboard
{
public:
    Keyboard() = default;
    ~Keyboard() = default;

    void addPressed(int key);
    void addReleased(int key);
    void flush();

    bool isPressed(int key);
    bool wasPressed(int key);
    bool wasReleased(int key);

private:
    sf::Keyboard _keyboard;
    std::array<int, sf::Keyboard::KeyCount> _keyWasPressed;
    std::array<int, sf::Keyboard::KeyCount> _keyWasReleased;
};
