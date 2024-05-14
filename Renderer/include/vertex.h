#pragma once

#include <glm/glm.hpp>
#include "obj_loader.h"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 texcoord;

    bool operator==(const Vertex &other) const
    {
        return position == other.position && normal == other.normal &&
               texcoord == other.texcoord && color == other.color;
    }
};

struct Mesh
{
    unsigned int VAO;
    std::vector<unsigned int> indices;
    material_t material;
};

