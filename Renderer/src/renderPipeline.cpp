#include "renderPipeline.h"

void RenderPipeline::addRenderPass(GLuint shader)
{
    passes.emplace_back(width, height, shader);
}

void RenderPipeline::render()
{
    for (size_t i = 0; i < passes.size(); ++i)
    {
        passes[i].begin();
        if (i > 0)
        {
            bindLastPassTexture(passes[i].getShader(), passes[i-1].getColorTexture());
        }
        passes[i].draw();
        passes[i].end();
    }
}

void RenderPipeline::bindLastPassTexture(GLuint shader, GLuint texture)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glUniform1i(glGetUniformLocation(shader, "lastpass"), 0);
}

GLuint RenderPipeline::getRenderTexture()
{
    return passes[passes.size() - 1].getColorTexture();
}

void RenderPipeline::init(int width, int height)
{
    this->width = width;
    this->height = height;
}
