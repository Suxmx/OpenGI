#version 100
in vec3 pix;
//out vec4 color;
void main() {
    gl_FragData[0]=vec4(pix,1);
}