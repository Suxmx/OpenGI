#include "renderPipeline.h"

void RenderPipeline::addRenderPass(GLuint shader)
{
    if (passes.size() + 1 >= 10)
    {
        std::cout << "Error:Renderpass count larger than 10!\n";
        return;
    }
    passes.emplace_back(width, height, shader);
    passes[passes.size() - 1].bindTexes["triangles"] = triangleTexBuffer;
}

void RenderPipeline::render()
{
    for (size_t i = 0; i < passes.size(); ++i)
    {
        passes[i].begin();
        if (i > 0)
        {
            bindLastPassTexture(passes[i], passes[i - 1].getColorTexture());
        }
        passes[i].draw();
        passes[i].end();
    }
}

void RenderPipeline::bindLastPassTexture(RenderPass &pass, GLuint texture)
{
    glActiveTexture(GL_TEXTURE0 + pass.bindTexes.size());
    glBindTexture(GL_TEXTURE_2D, texture);

    glUniform1i(glGetUniformLocation(pass.getShader(), "lastpass"), +pass.bindTexes.size());
}

GLuint RenderPipeline::getRenderTexture()
{
    return passes[passes.size() - 1].getColorTexture();
}

void RenderPipeline::init(int width, int height)
{
    this->width = width;
    this->height = height;
    passes.reserve(10);
}

void RenderPipeline::bindTriangleTexBuffer(GLuint buffer)
{
    triangleTexBuffer = buffer;
}
