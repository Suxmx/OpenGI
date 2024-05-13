#version 330 core
layout (location = 0) in vec3 aPos;
out vec4 outPos;
uniform float xPos;
uniform float yPos;

void main()
{
    gl_Position = vec4(aPos.x + xPos, aPos.y+yPos, aPos.z, 1);
    outPos=gl_Position;
}