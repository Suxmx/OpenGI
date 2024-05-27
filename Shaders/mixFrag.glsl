#version 330 core

in vec3 pix;
out highp vec4 fragColor;

uniform sampler2D lastpass;
uniform sampler2D framebuffer;
uniform int frameCount;
uniform float fac;

void main() {
    highp vec3 lastColor = texture(lastpass, pix.xy * 0.5 + 0.5).rgb;
    highp vec3 frameColor = texture(framebuffer, pix.xy * 0.5 + 0.5).rgb;
    highp float factor = 1.0 / float(frameCount + 1);

    highp vec3 finalColor = mix(frameColor,lastColor,  factor);

    fragColor =  vec4(finalColor, 1.0);
}
