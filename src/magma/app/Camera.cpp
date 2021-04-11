#include "magma/app/Camera.h"

#include <algorithm>
#include <unistd.h>

#include "glm/fwd.hpp"
#include "magma/app/keyboard.h"
#include "magma/app/log.hpp"
#include "magma/app/mouse.h"

Camera::Camera(float zNear, float zFar, int width, int height, float FOV):
    _zNear(zNear),
    _zFar(zFar),
    _width(width),
    _height(height),
    _FOV(std::clamp(FOV, 1.f, 179.f)),
    _pos(glm::vec3(0.0f, 0.0f, 3.0f)),
    _forward(glm::vec3(0.0f, 0.0f, -1.0f)),
    _right(glm::vec3(1.0f, 0.0f, 0.0f)),
    _up(glm::vec3(0.0f, 1.0f, 0.0f)),
    _pitch(0),
    _yaw(180),
    _sensitivity(0.2f),
    _speed(5.f),
    _pixelSize(0.2 / 960, 0.2 / 540),
    _jitterShift( {
        glm::vec2( 0.25, -0.25) * _pixelSize,
        glm::vec2(-0.25, -0.25) * _pixelSize,
        glm::vec2(-0.25,  0.25) * _pixelSize,
        glm::vec2( 0.25,  0.25) * _pixelSize
    } )
{
    updateViewMat();
    updateProjMat();
}

void Camera::updateScreenSize(int width, int height) {
    _width = width;
    _height = height;
    updateProjMat();
}

void Camera::update(Keyboard& keyboard, Mouse& mouse, float elapsedTime) {
    glm::vec2 cursor_shift = mouse.getShift();

    if (mouse.isLocked()) {
        _pitch -= cursor_shift.y*_sensitivity;
        _yaw -= cursor_shift.x*_sensitivity;
    }

    // std::cout << pitch << "_" << cursor_shift.y << std::endl;

    _forward = glm::vec3(
            glm::cos(glm::radians(_pitch)) * glm::sin(glm::radians(_yaw)),
            glm::sin(glm::radians(_pitch)),
            glm::cos(glm::radians(_pitch)) * glm::cos(glm::radians(_yaw))
    );
    _right = glm::normalize(glm::cross(_forward, glm::vec3(0,1,0)));
    _up = glm::normalize(glm::cross(_right, _forward));

    float curr_speed = _speed;
    if (keyboard.isPressed(GLFW_KEY_LEFT_SHIFT)) {
        curr_speed = _speed*4;
    } else if (keyboard.isPressed(GLFW_KEY_LEFT_CONTROL)) {
        curr_speed = _speed/20;
    }

    if (keyboard.isPressed(GLFW_KEY_W)) {
        _pos += elapsedTime*curr_speed*_forward;
    }
    if (keyboard.isPressed(GLFW_KEY_S)) {
        _pos -= elapsedTime*curr_speed*_forward;
    }

    if (keyboard.isPressed(GLFW_KEY_D)) {
        _pos += elapsedTime*curr_speed*_right;
    }
    if (keyboard.isPressed(GLFW_KEY_A)) {
        _pos -= elapsedTime*curr_speed*_right;
    }

    if (keyboard.isPressed(GLFW_KEY_E)) {
        _pos += elapsedTime*curr_speed*_up;
    }
    if (keyboard.isPressed(GLFW_KEY_Q)) {
        _pos -= elapsedTime*curr_speed*_up;
    }

    updateViewMat();
}

void Camera::setPos(glm::vec3 newPos) {
    _pos = newPos;
    updateViewMat();
}

void Camera::lookAt(glm::vec3 point) {
    _forward = glm::normalize(point - _pos);
    _right = glm::normalize(glm::cross(_forward, glm::vec3(0,1,0)));
    _up = glm::normalize(glm::cross(_right, _forward));

    //calculating pitch
    float angle = glm::acos(dot(_forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    angle = glm::pi<float>()/2 - angle;
    _pitch = glm::degrees(angle);

    //calculating yaw
    angle = glm::acos(dot(_right, glm::vec3(-1, 0, 0)));
    if (_right.z < 0) {
        angle = -angle;
    }
    _yaw = glm::degrees(angle);

    updateViewMat();
}

void Camera::updateProjMat() {
    _proj = glm::perspective(
            glm::radians(_FOV),
            float(_width)/float(_height),
            _zNear, _zFar
    );
    _proj[1][1] *= -1; //OpenGL legacy in glm
}

//! @todo noraml calculate _jitterShift
void Camera::updateViewMat() {
    // return glm::translate(glm::mat4(), -pos);
    glm::vec3 newPos = _pos + _right * _jitterShift[_ind % 4].x + _up * _jitterShift[_ind % 4].y;
    _view = glm::lookAt(newPos, newPos + _forward, _up);
    _ind = (_ind + 1) % 4;
}
