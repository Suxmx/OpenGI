#version 330 core

in vec3 pix;
out highp vec4 fragColor;
uniform sampler2D lastpass;

const mat3 POST_TONEMAPPING_TRANSFORM =
mat3(1.666954300, -0.601741150, -0.065202855,
     -0.106835220, 1.237778600, -0.130948950,
     -0.004142626, -0.087411870, 1.091555000);

vec3 ACESFilm(vec3 LinearColor, float a, float b, float c, float d, float e)
{
    const float ExposureMultiplier = 0.6f;                  // 曝光值

    const mat3 PRE_TONEMAPPING_TRANSFORM = // 色调映射矩阵
    mat3(
        0.575961650, 0.344143820, 0.079952030,
        0.070806820, 0.827392350, 0.101774690,
        0.028035252, 0.131523770, 0.840242300
    );

    const mat3 EXPOSED_PRE_TONEMAPPING_TRANSFORM = ExposureMultiplier * PRE_TONEMAPPING_TRANSFORM;    // key  场景颜色乘 曝光度 整个场景的色调。

    const mat3 POST_TONEMAPPING_TRANSFORM =
    mat3(
        1.666954300, -0.601741150, -0.065202855,
        -0.106835220, 1.237778600, -0.130948950,
        -0.004142626, -0.087411870, 1.091555000
    );

/*
    float a; // 2.51
    float b; // 0.03
    float c; // 2.43
    float d; // 0.59
    float e; // 0.14
    */

    vec3 Color = EXPOSED_PRE_TONEMAPPING_TRANSFORM * LinearColor;                     // 线性颜色转换
    Color = clamp((Color * (a * Color + b)) / (Color * (c * Color + d) + e), 0.f, 1.f);               // 应用到ACES 颜色校正

    return clamp(POST_TONEMAPPING_TRANSFORM * Color, 0.0f, 1.0f);
}
void main()
{
    vec4 col = texture(lastpass, pix.xy * 0.5 + 0.5);
    col.xyz = ACESFilm(col.xyz, 2.51f, 0.03f, 2.43f, 0.59f, 0.14f);
    fragColor = col;
}