#pragma once

#include <glm/glm.hpp>
#include "render_data.h"
#include "obj_loader.h"
#include <vector>

using namespace std;
using namespace glm;

vec3 calculateNormal(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2)
{
    glm::vec3 e1 = v1 - v0;  // 边向量 1
    glm::vec3 e2 = v2 - v0;  // 边向量 2
    glm::vec3 normal = glm::cross(e1, e2);  // 计算叉积得到法线向量
    normal = glm::normalize(normal);  // 归一化法线向量
    return normal;
}

void loadObj(const char *filename, vector<triangle> &triangles, mat4 modelMat = mat4(1))
{
    Model m = Model(filename, "Models/");
    m.Load(nullptr, nullptr);
    float minx = 9999999, miny = 9999999, minz = 9999999;
    float maxx = -9999999, maxy = -9999999, maxz = -9999999;

    for (int i = 0; i < m.vertexAttribute.vertices.size(); i += 3)
    {
        vec3 v = vec3(m.vertexAttribute.vertices[i], m.vertexAttribute.vertices[i + 1],
                      m.vertexAttribute.vertices[i + 2]);
        minx = std::min(v.x, minx);
        miny = std::min(v.y, miny);
        minz = std::min(v.z, minz);

        maxx = std::max(v.x, maxx);
        maxy = std::max(v.y, maxy);
        maxz = std::max(v.z, maxz);
    }
    float boundx = maxx - minx, boundy = maxy - miny, boundz = maxz - minz;
    float maxbound = std::max(std::max(boundx, boundy), boundz);
    for (auto &v: m.vertexAttribute.vertices)
    {
        v /= maxbound;
    }
    // 计算法线
    vector<vec3> calcNormals(m.vertexAttribute.vertices.size(), vec3(0));
    for (const auto &shape: m.shapes)
    {
        for (int i = 0; i < shape.mesh.indices.size(); i += 3)
        {
            vector<vec3> ps;
            vector<int> indices;
            for (int j = 0; j < 3; j++)
            {
                index_t index = shape.mesh.indices[i + j];
                indices.push_back(index.vertex_index);
                vec3 p;
                p[0] = m.vertexAttribute.vertices[3 * index.vertex_index];
                p[1] = m.vertexAttribute.vertices[3 * index.vertex_index + 1];
                p[2] = m.vertexAttribute.vertices[3 * index.vertex_index + 2];
                ps.emplace_back(p);
            }
            vec3 n = normalize(cross(ps[1] - ps[0], ps[2] - ps[0]));
            calcNormals[indices[0]] += n;
            calcNormals[indices[1]] += n;
            calcNormals[indices[2]] += n;
        }
    }
    for (const auto &shape: m.shapes)
    {
        for (int i = 0; i < shape.mesh.indices.size(); i += 3)
        {
            triangle t;
            for (int j = 0; j < 3; j++)
            {
                Vertex v{};
                index_t index = shape.mesh.indices[i + j];
                // pos
                v.position[0] = m.vertexAttribute.vertices[3 * index.vertex_index];
                v.position[1] = m.vertexAttribute.vertices[3 * index.vertex_index + 1];
                v.position[2] = m.vertexAttribute.vertices[3 * index.vertex_index + 2];
                // normal
                if (index.normal_index >= 0)
                {
                    v.normal[0] = m.vertexAttribute.normals[3 * index.normal_index];
                    v.normal[1] = m.vertexAttribute.normals[3 * index.normal_index + 1];
                    v.normal[2] = m.vertexAttribute.normals[3 * index.normal_index + 2];
                }
                else
                {
                    v.normal = normalize(calcNormals[index.vertex_index]);
                }
                //进行矩阵变换
                vec4 vw = vec4(v.position, 1);
                vw = modelMat * vw;
                vw / vw.w;
                t.poses.emplace_back(vw.x, vw.y, vw.z);
                t.normals.push_back(v.normal);
            }
            t.bindData();

            int numFace = i / 3;
            int materialId = numFace < shape.mesh.material_ids.size() ? shape.mesh.material_ids[numFace] : -1;
            t.material = materialId >= 0 ? m.materials[materialId] : material_t();
//            t.bindData();
            triangles.emplace_back(t);
        }
    }

}

vector<triangle_encoded> encodeTriangles(vector<triangle> &triangles)
{
    vector<triangle_encoded> ets;
    ets.reserve(triangles.size());
    for (const auto &t: triangles)
    {
        triangle_encoded et;
        et.p1 = t.p1;
        et.p2 = t.p2;
        et.p3 = t.p3;
        et.n1 = t.n1;
        et.n2 = t.n2;
        et.n3 = t.n3;
        for (int i = 0; i < 3; i++)
        {
            et.emissive[i] = t.material.emission[i];
            et.baseColor[i] = t.material.diffuse[i];
        }
        //for test
        et.baseColor=vec3(1,1,0);
        ets.emplace_back(et);
    }
    return ets;
}

mat4 getTransformMatrix(vec3 rotateCtrl, vec3 translateCtrl, vec3 scaleCtrl)
{
    glm::mat4 unit(    // 单位矩阵
            glm::vec4(1, 0, 0, 0),
            glm::vec4(0, 1, 0, 0),
            glm::vec4(0, 0, 1, 0),
            glm::vec4(0, 0, 0, 1)
    );
    mat4 scale = glm::scale(unit, scaleCtrl);
    mat4 translate = glm::translate(unit, translateCtrl);
    mat4 rotate = unit;
    rotate = glm::rotate(rotate, glm::radians(rotateCtrl.x), glm::vec3(1, 0, 0));
    rotate = glm::rotate(rotate, glm::radians(rotateCtrl.y), glm::vec3(0, 1, 0));
    rotate = glm::rotate(rotate, glm::radians(rotateCtrl.z), glm::vec3(0, 0, 1));

    mat4 model = translate * rotate * scale;
    return model;
}