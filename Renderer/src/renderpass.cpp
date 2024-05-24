#include "renderpass.h"

void RenderPass::begin() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);
    int cnt = 0;

    for (const auto &pair: bindTexes)
    {
        glActiveTexture(GL_TEXTURE0 + cnt);
        glBindTexture(GL_TEXTURE_BUFFER, pair.second);
        glUniform1i(glGetUniformLocation(shaderProgram, pair.first.c_str()), cnt++);
    }

}

void RenderPass::end()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
}

GLuint RenderPass::getColorTexture() const
{
    return colorTexture;
}

void RenderPass::draw()
{
//    begin();
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
//    end();
}

void RenderPass::setupFramebuffer()
{
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass::setupCanvas()
{
    std::vector<vec3> square = {
            vec3(-1, -1, 0), vec3(1, -1, 0), vec3(-1, 1, 0),
            vec3(1, 1, 0), vec3(-1, 1, 0), vec3(1, -1, 0)
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, square.size() * sizeof(vec3), &square[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *) 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

GLuint RenderPass::getShaderId() const
{
    return shaderProgram;
}

shader *RenderPass::getShader()
{
    return &passShader;
}

void FirstPass::draw()
{
    passShader.setInt("nTriangle",nTriangle);
    RenderPass::draw();
}

FirstPass::FirstPass(int width, int height, const shader &s) : RenderPass(width, height, s)
{}
