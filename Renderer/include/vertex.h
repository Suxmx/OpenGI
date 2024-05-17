#pragma once

#include <glm/glm.hpp>
#include "obj_loader.h"
#include "tinyobjloader.h"

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



struct Batch
{
    int materialId;
    unsigned int VAO;
    std::vector<unsigned int> indices;
    std::vector<Vertex> v;
    material_t material;
};

