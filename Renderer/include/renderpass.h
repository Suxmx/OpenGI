#pragma once

#include <vector>
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

using namespace glm;
using namespace std;

class RenderPass {
public:
    GLuint FBO = 0;
    GLuint vao, vbo;
    std::vector<GLuint> colorAttachments;
    GLuint program;
    int width = 512;
    int height = 512;
    void bindData();
    void draw(std::vector<GLuint> texPassArray = {}) const;
};