#pragma once
#include "vk3d/glm_inc.h"
#include "mesh.h"

#include <vector>
#include <string>

class MeshReader
{
    std::vector<Mesh> meshes;
public:
    std::vector<Mesh> &load_scene(const std::string &filename);
};