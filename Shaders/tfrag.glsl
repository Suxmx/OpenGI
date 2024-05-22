#version 100
in vec3 pix;
out vec4 color;
void main() {
    color=vec4(pix,1);
}