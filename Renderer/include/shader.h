#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class shader
{
public:
    unsigned int ID;

    void use() const;
    void unuse() const;

    // uniform¹¤¾ßº¯Êý
    void setBool(const std::string &name, bool value) const;

    void setInt(const std::string &name, int value) const;

    void setFloat(const std::string &name, float value) const;

    void setVec3(const std::string &name, float a, float b, float c) const;

    void setVec4(const std::string &name, float a, float b, float c, float d) const;

    void setMat4(const std::string &name, glm::mat4 mat) const;

    void setMat3(const std::string &name, glm::mat3 mat) const;

    shader(const char *vPath, const char *fPath);
};