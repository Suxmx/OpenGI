#version 330 core
#define SIZE_TRIANGLE 8
layout (location = 0) in vec3 vPosition;  // cpu传入的顶点坐标
out vec3 pix;


void main() {
    gl_Position = vec4(vPosition, 1.0);
    pix = vec3(vPosition.x,vPosition.y,vPosition.z);
}