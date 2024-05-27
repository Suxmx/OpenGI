#pragma once

#include <glm/glm.hpp>
#include <vector>
//#include "obj_loader.h"
#include <algorithm>
#include <string>

using namespace glm;
using namespace std;
struct material
{
    std::string name;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 transmittance;
    vec3 emission;
    float shininess;
    float ior;       // 折射率
    float dissolve;  //"1 == 不透明，0 == 完全透明"
    int illum;
};
struct Vertex
{
    vec3 position;
    vec3 normal;
};


struct triangle
{
    vec3 p1, p2, p3;    // 顶点坐标
    vec3 n1, n2, n3;    // 顶点法线
    material material;  // 材质
    std::vector<vec3> poses;
    std::vector<vec3> normals;

    void bindData()
    {
        p1 = poses[0];
        p2 = poses[1];
        p3 = poses[2];
        n1 = normals[0];
        n2 = normals[1];
        n3 = normals[2];
    }
};


struct BVHNode
{
    int left, right;    // 左右子树索引
    int n, index;       // 叶子节点信息
    vec3 AA, BB;        // 碰撞盒
};
struct triangle_encoded
{
    vec3 p1, p2, p3;    // 顶点坐标
    vec3 n1, n2, n3;    // 顶点法线
    vec3 emissive;      // 自发光参数
    vec3 baseColor;     // 颜色
//    vec3 param1;        // (subsurface, metallic, specular)
//    vec3 param2;        // (specularTint, roughness, anisotropic)
//    vec3 param3;        // (sheen, sheenTint, clearcoat)
//    vec3 param4;        // (clearcoatGloss, IOR, transmission)
};
struct BVHNode_encoded
{
    vec3 childs;        // (left, right, 保留)
    vec3 leafInfo;      // (n, index, 保留)
    vec3 AA, BB;
};
struct light_encoded
{
    vec3 p1,p2,p3;
    vec3 n1,n2,n3;
    vec3 emissive;
    vec3 area;// (area,0,0)
};
vector<triangle_encoded> encodeTriangles(vector<triangle> &triangles);
vector<light_encoded> encodeLights(vector<triangle> &triangles,float* totalArea);
vector<BVHNode_encoded> encodeBVHNodes(vector<BVHNode>& bvhs);
bool cmpx(const triangle &t1, const triangle &t2);

bool cmpy(const triangle &t1, const triangle &t2);

bool cmpz(const triangle &t1, const triangle &t2);

int buildBVHwithSAH(std::vector<triangle> &triangles, std::vector<BVHNode> &nodes, int l, int r, int n);