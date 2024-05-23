#include "renderpass.h"

void RenderPass::draw(std::vector<GLuint> texPassArray) const
{
    glUseProgram(program);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glBindVertexArray(vao);

    // 传上一帧的帧缓冲颜色附件
    for (int i = 0; i < texPassArray.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, texPassArray[i]);
        std::string uName = "texPass" + std::to_string(i);
        glUniform1i(glGetUniformLocation(program, uName.c_str()), i);
    }

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 确保绑定了EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
}

void RenderPass::bindData()
{
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    std::vector<vec3> square = {
            vec3(-1, -1, 0), vec3(1, -1, 0), vec3(-1, 1, 0),
            vec3(1, 1, 0), vec3(-1, 1, 0), vec3(1, -1, 0)
    };

    // 上传顶点数据
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * square.size(), &square[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);   // layout (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    // 生成并绑定EBO
    vector<int> indices{0, 1, 2, 3, 4, 5};
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    std::vector<GLuint> attachments;
    for (int i = 0; i < colorAttachments.size(); i++)
    {
        glBindTexture(GL_TEXTURE_2D, colorAttachments[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorAttachments[i], 0);
        attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
    }
    glDrawBuffers(attachments.size(), &attachments[0]);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
