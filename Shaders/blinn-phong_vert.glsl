#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

struct material_t
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform material_t material;
out vec3 viewspace_normal;
out vec3 viewspace_pos;
out vec3 color;

uniform mat4 mv;
uniform mat4 mvp;
uniform mat3 normal_mv;

void main()
{
    vec4 pos4 = vec4(pos.x, pos.y, pos.z, 1);
    viewspace_normal = normal_mv * normal;
    viewspace_pos = vec3(mv * pos4);
    gl_Position = mvp * pos4;
    color=material.diffuse;
}