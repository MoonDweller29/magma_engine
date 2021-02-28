#pragma once
#include "magma/glm_inc.h"

class Keyboard;
class Mouse;

class Camera {
public:
    Camera(float z_near, float z_far, int width, int height, float FOV);

    void updateScreenSize(int width, int height);
    void update(Keyboard& keyboard, Mouse& mouse, float elapsed_time);

    const glm::mat4 &getProjMat() const;
    const glm::mat4 &getViewMat() const;
    const glm::vec3 &getPos() const;

private:
    glm::mat4 proj;
    glm::mat4 view;

    glm::vec3 pos;

    glm::vec3 forward;
    glm::vec3 up;
    glm::vec3 right;

    float z_near;
    float z_far;
    int   width;
    int   height;
    float FOV;

    float sensitivity;
    float speed;

    float pitch;
    float yaw;

    void updateProjMat();
    void updateViewMat();
};
