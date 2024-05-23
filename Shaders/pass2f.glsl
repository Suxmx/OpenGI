#version 330 core

// 声明 uniform 变量，用于接收之前pass的颜色附件
uniform sampler2D lastpass;

out vec4 FragColor;

void main() {
    // 从之前pass的颜色附件中采样颜色
    vec2 texCoord = gl_FragCoord.xy / vec2(800, 600); // 假设屏幕尺寸是 800x600
    vec4 previousColor = texture(lastpass, vec2(1)-texCoord);

    FragColor = previousColor;
}
