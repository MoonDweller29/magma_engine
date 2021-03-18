#pragma once
#include "SFML/Window/WindowBase.hpp"
#include "SFML/Window/Mouse.hpp"
#include "SFML/System/Vector2.hpp"

#include "magma/glm_inc.h"

class Mouse
{
public:
    Mouse(sf::WindowBase &window);
    ~Mouse() = default;

    void update();

    void lock();
    void unlock();

    const glm::ivec2 &getShift() const;
    bool isLocked() { return _locked; }
    bool isButtonPressed(sf::Mouse::Button button) { return _mouse.isButtonPressed(button); }

private:
    sf::WindowBase& _window;
    sf::Mouse _mouse;

    glm::ivec2 _pos;
    glm::ivec2 _prev_pos;
    glm::ivec2 _shift;

    bool _locked;
};
