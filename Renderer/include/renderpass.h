#pragma once

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>
#include <map>

using namespace glm;
using namespace std;

class RenderPass {
public:
    RenderPass(int width, int height,GLuint shader) : width(width), height(height),shaderProgram(shader)
    {
        setupFramebuffer();
        setupCanvas();
    }

    ~RenderPass() {
        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &colorTexture);
    }
    void init(int width, int height);

    virtual void begin() const;

    virtual void end();

    virtual void draw();
    GLuint getColorTexture() const;
    GLuint getShader()const;
    map<string,GLuint> bindTexes;


private:
    void setupFramebuffer();
    void setupCanvas();

    GLuint fbo,vao,vbo;
    GLuint colorTexture;
    int width, height;
    GLuint shaderProgram;
};
