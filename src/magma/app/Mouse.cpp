#include "magma/app/Mouse.h"
#include "SFML/System/Vector2.hpp"

Mouse::Mouse(sf::WindowBase &window)
        : _window(window),
        _prev_pos(0,0),
        _shift(0,0),
        _locked(false)
{
    _window.setMouseCursorGrabbed(false);
    double x_pos, y_pos;
    sf::Vector2i win_pos = _mouse.getPosition(_window);
    _pos = glm::ivec2(win_pos.x, win_pos.y);
}

void Mouse::update()
{
    _prev_pos = _pos;
    double x_pos, y_pos;
    sf::Vector2i win_pos = _mouse.getPosition(_window);
    sf::Vector2u win_size = _window.getSize();
    _pos = glm::ivec2(win_pos.x, win_pos.y);

    if (!_locked && ((_pos.x > win_size.x) || (_pos.x < 0) ||
            (_pos.y > win_size.y) || (_pos.y < 0))) {
        _shift = glm::ivec2(0, 0);
    } else {
        _shift = _pos - _prev_pos;
    }
}

const glm::ivec2 &Mouse::getShift() const {
    return _shift;
}

void Mouse::lock() {
    sf::Vector2u win_size = _window.getSize();
    sf::Vector2i newPosition(win_size.x, win_size.y);
    newPosition /= 2;
    _mouse.setPosition(newPosition ,_window);
    _window.setMouseCursorVisible(false);
    update();
    _locked = true;
}

void Mouse::unlock() {
    _window.setMouseCursorVisible(true);
    _locked = false;
}
