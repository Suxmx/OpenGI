#version 330 core
#define INF 999999
#define SIZE_TRIANGLE 8
in vec3 pix;

uniform samplerBuffer triangles;
uniform int nTriangle;
struct Material
{
    vec3 emissive;
    vec3 baseColor;
};
struct Triangle
{
    vec3 p1, p2, p3;
    vec3 n1, n2, n3;
    Material m;
};
struct Ray
{
    vec3 o;
    vec3 dir;
};
struct Hit
{
    bool isHit;
    bool isInside;//是否从内部命中
    float dist;// 距离交点的距离(因为对于每条光线来说方向一样起点一样所以这里仅记录t)
    vec3 hitPoint;
    vec3 normal;
    vec3 lightDir;//击中该点的光线的方向
    Material m;//命中点的材质
};

// 从texture buffer中解析出三角形
Triangle getTriangle(int i)
{
    Triangle ret;
    int offset = SIZE_TRIANGLE * i;
    // 顶点
    ret.p1 = texelFetch(triangles, offset + 0).xyz;
    ret.p2 = texelFetch(triangles, offset + 1).xyz;
    ret.p3 = texelFetch(triangles, offset + 2).xyz;
    // 法线
    ret.n1 = texelFetch(triangles, offset + 3).xyz;
    ret.n2 = texelFetch(triangles, offset + 4).xyz;
    ret.n3 = texelFetch(triangles, offset + 5).xyz;
    // 读取材质
    Material m;
    m.emissive = texelFetch(triangles, offset + 6).xyz;
    m.baseColor = texelFetch(triangles, offset + 7).xyz;
    ret.m = m;
    return ret;
}
Hit hitTriangle(Triangle tri, Ray ray)
{
    Hit ret;
    ret.dist = INF;
    ret.isHit = false;
    ret.isInside = false;
    vec3 N = normalize(cross(tri.p2 - tri.p1, tri.p3 - tri.p1));
    float dotND = dot(N, ray.dir);
    // 光线从内部打到三角形
    if (dotND > 0.0f)
    {
        ret.isInside = true;
        N = -N;
    }
    // 视线和三角形平行
    if (abs(dotND) < 0.00001f)
    {
        return ret;
    }
    //moller algorithm
    vec3 mollerRet;
    vec3 e1 = tri.p2 - tri.p1;
    vec3 e2 = tri.p3 - tri.p1;
    vec3 s = ray.o - tri.p1;
    vec3 s1 = cross(ray.dir, e2);
    vec3 s2 = cross(s, e1);
    mollerRet = 1 / dot(s1, e1) * vec3(dot(s2, e2), dot(s1, s), dot(s2, ray.dir));
    float t = mollerRet.x, b1 = mollerRet.y, b2 = mollerRet.z;

    if (t < 0.00001f || b1 < 0.0f || b2 < 0.0f || (b1 + b2) > 1.0f)
    {
        return ret;
    }
    ret.hitPoint = ray.o + t * ray.dir;
    ret.isHit = true;
    ret.dist = t;
    ret.lightDir = ray.dir;
    ret.m = tri.m;
    vec3 smoothNormal = (b1 * tri.n1 + b2 * tri.n2 + (1 - b1 - b2) * tri.n3);
    ret.normal = !ret.isInside ? smoothNormal : -smoothNormal;
    return ret;
}
// 取得[l,r]中最近的交点的结果
Hit hitArray(int l, int r, Ray ray)
{
    Hit ret;
    ret.isHit = false;
    ret.dist = INF;
    for (int i = l;i <= r; i++)
    {
        Triangle tri = getTriangle(i);
        Hit tmp = hitTriangle(tri, ray);
        if (tmp.isHit && tmp.dist < ret.dist)
        {
            ret = tmp;
            ret.m = tri.m;
        }
    }
    return ret;
}

void main()
{
    Ray ray;
    ray.o = vec3(0, 0, 4);
    ray.dir = normalize(vec3(pix.xy, 2) - ray.o);
    Hit hit = hitArray(0, nTriangle-1, ray);
    if (hit.isHit)gl_FragData[0] = vec4(hit.m.baseColor,1);
}
