#version 330 core

layout (location = 0) in vec3 vPosition;  // cpu传入的顶点坐标
uniform samplerBuffer triangles;
out vec3 pix;
out float test;

void main() {
    gl_Position = vec4(vPosition, 1.0);
    pix = vec3(vPosition.z,vPosition.y,vPosition.x);
    test=texelFetch(triangles,1).x;
}

