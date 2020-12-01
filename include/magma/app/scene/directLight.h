#pragma once
#include "magma/glm_inc.h"

class DirectLight
{
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 up;
    glm::mat4 view;
    glm::mat4 proj;

    float zNear, zFar;
    void updateView();
public:
    DirectLight(
            const glm::vec3 start_pos,
            const glm::vec3 start_dir,
            float zNear, float zFar);

    void setPos(glm::vec3 new_pos);
    void setDir(glm::vec3 new_dir);
    void lookAt(const glm::vec3 &look_point, const glm::vec3 &pos);

    const glm::vec3 &getPos()    const { return pos;  }
    const glm::vec3 &getDir()    const { return dir;  }
    const glm::mat4 &getView() const { return view; }
    const glm::mat4 &getProj() const { return proj; }
};
