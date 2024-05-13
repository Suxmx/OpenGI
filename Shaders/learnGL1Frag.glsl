#version 330 core
out vec4 color;
in vec4 outPos;
uniform vec4 outColor;
void main() {
    color = outPos;
}



//stest