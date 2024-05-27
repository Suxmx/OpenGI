#include "renderPipeline.h"

void RenderPipeline::addRenderPass(const shared_ptr<RenderPass> &pass)
{
    passes.emplace_back(pass);
}

void RenderPipeline::render()
{
    for (size_t i = 0; i < passes.size(); ++i)
    {
        passes[i]->begin();
        if (i > 0)
        {
            bindLastPassTexture(passes[i], passes[i - 1]->getColorTexture());
        }
        passes[i]->draw();
        passes[i]->end();
    }
}

void RenderPipeline::bindLastPassTexture(shared_ptr<RenderPass> pass, GLuint texture)
{
    glActiveTexture(GL_TEXTURE0 + pass->bindTexes.size());
    glBindTexture(GL_TEXTURE_2D, texture);

    glUniform1i(glGetUniformLocation(pass->getShaderId(), "lastpass"), pass->bindTexes.size());
}

GLuint RenderPipeline::getRenderTexture()
{
    return passes[passes.size() - 1]->getColorTexture();
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

shared_ptr<RenderPass> RenderPipeline::getRenderpass(int i)
{
    return passes[i];
}
