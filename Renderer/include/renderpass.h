#pragma once

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>
#include <map>

#include "shader.h"

using namespace glm;
using namespace std;

class RenderPass {
public:
    RenderPass(int width, int height,shader s) : width(width), height(height),passShader(s),shaderProgram(s.ID)
    {
        setupFramebuffer();
        setupCanvas();
    }

    ~RenderPass() {
        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &colorTexture);
    }

    virtual void begin() const;

    virtual void end();

    virtual void draw();

    GLuint getColorTexture() const;
    GLuint getShaderId()const;
    shader* getShader();
    map<string,GLuint> bindTexes;


protected:
    void setupFramebuffer();
    void setupCanvas();

    GLuint fbo,vao,vbo;
    GLuint colorTexture;
    int width, height;
    GLuint shaderProgram;
    shader passShader;
};
class FirstPass : public RenderPass
{
public:
    FirstPass(int width, int height, const shader &s);

    int nTriangle;
    void draw() override;
};
