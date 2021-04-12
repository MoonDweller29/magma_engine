#pragma once
#include "magma/glm_inc.h"

class Keyboard;
class Mouse;

class Camera {
public:
    Camera(float zNear, float zFar, int width, int height, float FOV);

    void updateScreenSize(int width, int height);
    void update(Keyboard& keyboard, Mouse& mouse, float elapsedTime);
    void setPos(glm::vec3 newPos);
    void lookAt(glm::vec3 point);

    const glm::mat4 &getProjMat() const { return _proj;  }
    const glm::mat4 &getViewMat() const { return _view;  }
    const glm::vec3 &getPos()     const { return _pos;   }
    float            getZNear()   const { return _zNear; }
    float            getZFar()    const { return _zFar;  }

private:
    glm::mat4 _proj;
    glm::mat4 _view;

    glm::vec3 _pos;

    glm::vec3 _forward;
    glm::vec3 _up;
    glm::vec3 _right;
    float     _pitch;
    float     _yaw;

    float _zNear;
    float _zFar;
    int   _width;
    int   _height;
    float _FOV;

    float _sensitivity;
    float _speed;


    void updateProjMat();
    void updateViewMat();
};
