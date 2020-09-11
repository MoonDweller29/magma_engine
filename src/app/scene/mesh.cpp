#include "mesh.h"

Mesh::Mesh(std::string name,
           std::vector<Vertex> vertices,
           std::vector<uint32_t> indices):
    name(std::move(name)),
    vertices(std::move(vertices)),
    indices(std::move(indices))
{}