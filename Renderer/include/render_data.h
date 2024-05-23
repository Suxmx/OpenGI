#pragma once

#include <glm/glm.hpp>
#include "obj_loader.h"
#include <vector>
using namespace glm;

struct Vertex
{
    vec3 position;
    vec3 normal;
};

//struct material {
//    vec3 emissive = vec3(0, 0, 0);  // 作为光源时的发光颜色
//    vec3 baseColor = vec3(1, 1, 1);
//    float subsurface = 0.0;
//    float metallic = 0.0;
//    float specular = 0.0;
//    float specularTint = 0.0;
//    float roughness = 0.0;
//    float anisotropic = 0.0;
//    float sheen = 0.0;
//    float sheenTint = 0.0;
//    float clearcoat = 0.0;
//    float clearcoatGloss = 0.0;
//    float IOR = 1.0;
//    float transmission = 0.0;
//};

struct triangle {
    vec3 p1, p2, p3;    // 顶点坐标
    vec3 n1, n2, n3;    // 顶点法线
    material_t material;  // 材质
    std::vector<vec3> poses;
    std::vector<vec3> normals;
    void bindData()
    {
        p1=poses[0];
        p2=poses[1];
        p3=poses[2];
        n1=normals[0];
        n2=normals[1];
        n3=normals[2];
    }
};
struct triangle_encoded {
    vec3 p1, p2, p3;    // 顶点坐标
    vec3 n1, n2, n3;    // 顶点法线
    vec3 emissive;      // 自发光参数
    vec3 baseColor;     // 颜色
//    vec3 param1;        // (subsurface, metallic, specular)
//    vec3 param2;        // (specularTint, roughness, anisotropic)
//    vec3 param3;        // (sheen, sheenTint, clearcoat)
//    vec3 param4;        // (clearcoatGloss, IOR, transmission)
};