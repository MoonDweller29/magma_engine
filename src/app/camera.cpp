#include "app/camera.h"
#include "app/keyboard.h"
#include "app/mouse.h"

#include <algorithm>
#include <iostream>

Camera::Camera(float z_near, float z_far, int width, int height, float FOV):
    z_near(z_near),
    z_far(z_far),
    width(width),
    height(height),
    FOV(std::clamp(FOV, 1.f, 179.f)),
    pos(glm::vec3(0.0f, 0.0f, 3.0f)),
    forward(glm::vec3(0.0f, 0.0f, -1.0f)),
    right(glm::vec3(1.0f, 0.0f, 0.0f)),
    up(glm::vec3(0.0f, 1.0f, 0.0f)),
    pitch(0),
    yaw(180),
    sensitivity(0.2f),
    speed(5.f)
{}

void Camera::updateScreenSize(int width, int height)
{
    this->width = width;
    this->height = height;
}

void Camera::update(Keyboard& keyboard, Mouse& mouse, float elapsed_time)
{
    glm::vec2 cursor_shift = mouse.getShift();

    if (mouse.isLocked())
    {
        pitch -= cursor_shift.y*sensitivity;
        yaw -= cursor_shift.x*sensitivity;
    }

    // std::cout << pitch << "_" << cursor_shift.y << std::endl;

    forward = glm::vec3(
            glm::cos(glm::radians(pitch)) * glm::sin(glm::radians(yaw)),
            glm::sin(glm::radians(pitch)),
            glm::cos(glm::radians(pitch)) * glm::cos(glm::radians(yaw))
    );
    right = glm::normalize(glm::cross(forward, glm::vec3(0,1,0)));
    up = glm::normalize(glm::cross(right, forward));

    float curr_speed = speed;
    if (keyboard.isPressed(GLFW_KEY_LEFT_SHIFT))
        curr_speed = speed*4;
    if (keyboard.isPressed(GLFW_KEY_LEFT_CONTROL))
        curr_speed = speed/20;

    if (keyboard.isPressed(GLFW_KEY_W))
    {
        pos += elapsed_time*curr_speed*forward;
    }
    if (keyboard.isPressed(GLFW_KEY_S))
    {
        pos -= elapsed_time*curr_speed*forward;
    }

    if (keyboard.isPressed(GLFW_KEY_D))
    {
        pos += elapsed_time*curr_speed*right;
    }
    if (keyboard.isPressed(GLFW_KEY_A))
    {
        pos -= elapsed_time*curr_speed*right;
    }

    if (keyboard.isPressed(GLFW_KEY_E))
    {
        pos += elapsed_time*curr_speed*up;
    }
    if (keyboard.isPressed(GLFW_KEY_Q))
    {
        pos -= elapsed_time*curr_speed*up;
    }

    updateProjMat();
    updateViewMat();
}

void Camera::updateProjMat()
{
    proj = glm::perspective(
            glm::radians(this->FOV),
            float(this->width)/float(this->height),
            this->z_near, this->z_far
    );
    proj[1][1] *= -1; //OpenGL legacy in glm
}

void Camera::updateViewMat()
{
    // return glm::translate(glm::mat4(), -pos);
    view = glm::lookAt(pos, pos + forward, up);
}

const glm::mat4 &Camera::getProjMat() const
{
    return proj;
}

const glm::mat4 &Camera::getViewMat() const
{
    return view;
}

const glm::vec3 &Camera::getPos() const
{
    return pos;
}