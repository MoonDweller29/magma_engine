#include "magma/app/scene/mesh.h"
#include <vulkan/vulkan.hpp>

std::vector<vk::VertexInputBindingDescription> Vertex::getBindingDescription()
{
    std::vector<vk::VertexInputBindingDescription>  bindingDescription(1, vk::VertexInputBindingDescription());
    bindingDescription[0].binding = 0;
    bindingDescription[0].stride = sizeof(Vertex);
    bindingDescription[0].inputRate = vk::VertexInputRate::eVertex;

    return bindingDescription;
}

std::vector<vk::VertexInputAttributeDescription> Vertex::getAttributeDescriptions()
{
    std::vector<vk::VertexInputAttributeDescription> attributeDescriptions(3, vk::VertexInputAttributeDescription());
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
    attributeDescriptions[1].offset = offsetof(Vertex, normal);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = vk::Format::eR32G32B32Sfloat;
    attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

    return attributeDescriptions;
}


Mesh::Mesh(std::string name,
           std::vector<Vertex> vertices,
           std::vector<uint32_t> indices):
    name(std::move(name)),
    vertices(std::move(vertices)),
    indices(std::move(indices))
{}
