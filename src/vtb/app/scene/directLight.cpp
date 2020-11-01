#include "vtb/app/scene/directLight.h"

DirectLight::DirectLight(
        const glm::vec3 start_pos, const glm::vec3 start_dir,
        float zNear, float zFar):
    pos(start_pos), dir(start_dir),
    zNear(zNear), zFar(zFar),
    up(glm::vec3(0,1,0))
{
    updateView();
//    proj = glm::perspective(
//            glm::radians(45.f),
//            float(1)/float(1),
//            zNear, zFar
//    );
    proj = glm::ortho(
            -2.0f, 2.0f, -2.0f, 2.0f,
            zNear, zFar
    );
    proj[1][1] *= -1;
}

void DirectLight::setPos(glm::vec3 new_pos)
{
    pos = new_pos;
    updateView();
}

void DirectLight::setDir(glm::vec3 new_dir)
{
    dir = new_dir;
    updateView();
}

void DirectLight::lookAt(const glm::vec3 &look_point, const glm::vec3 &pos)
{
    this->pos = pos;
    dir = look_point - pos;

    glm::vec3 right = glm::normalize(glm::cross(dir, glm::vec3(0,1,0)));
    up = glm::normalize(glm::cross(right, dir));
    updateView();
}


void DirectLight::updateView()
{
//    view = glm::translate(glm::mat4(), -pos);
    view = glm::lookAt(pos, pos + dir, up);
}
