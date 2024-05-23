#version 330 core
in vec3 pix;
uniform samplerBuffer triangles;
//out vec4 color;
void main() {
    gl_FragData[0]=vec4(pix,1);
}