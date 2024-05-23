#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>
#include "renderpass.h"

using namespace glm;

class RenderPipeline
{
public:
    int width, height;
    void addRenderPass(const shared_ptr<RenderPass>& pass);

    void render();
    void init(int width,int height);
    void bindTriangleTexBuffer(GLuint buffer);
    GLuint getRenderTexture();

    std::vector<shared_ptr<RenderPass>> passes;
private:
    void bindLastPassTexture(shared_ptr<RenderPass> pass, GLuint texture);



    GLuint vao = 0, vbo = 0,triangleTexBuffer;
};