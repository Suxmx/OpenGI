#pragma once

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>
#include "renderpass.h"

using namespace glm;

class RenderPipeline
{
public:
    void addRenderPass(GLuint shader);

    void render();
    void init(int width,int height);

    GLuint getRenderTexture();

    std::vector<RenderPass> passes;
private:
    void bindLastPassTexture(GLuint shader, GLuint texture);


    int width, height;
    GLuint vao = 0, vbo = 0;
};