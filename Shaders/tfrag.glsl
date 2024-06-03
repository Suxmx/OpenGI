#version 330 core
#define PI 3.1415926
#define INF 999999
#define SIZE_TRIANGLE 8
#define SIZE_BVH 4
in vec3 pix;
out vec4 color;
uniform samplerBuffer triangles;
uniform samplerBuffer bvh;
uniform samplerBuffer lights;
uniform int nTriangle;
uniform int nNodes;
uniform int width;
uniform int height;
uniform uint frameCount;
uniform int nLight;
//随机数种子
uint seed = uint(
    uint((pix.x * 0.5 + 0.5) * width) * uint(1973) +
    uint((pix.y * 0.5 + 0.5) * height) * uint(9277) +
    uint(frameCount) * uint(26699)) | uint(1);
uint wang_hash(inout uint seed)
{
    seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> 4);
    seed *= uint(0x27d4eb2d);
    seed = seed ^ (seed >> 15);
    return seed;
}
//返回0-1均匀随机分布的数
float rand()
{
    return float(wang_hash(seed)) / 4294967296.0;
}
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
struct BVHNode
{
    int left, right;    // 左右子树索引
    int n, index;       // 叶子节点信息
    vec3 minPoint, maxPoint;        // 碰撞盒
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
// 从texture buffer中解析出bvh节点
BVHNode getNode(int i)
{
    BVHNode node;
    int offset = i * SIZE_BVH;
    ivec3 childs = ivec3(texelFetch(bvh, offset).xyz);
    ivec3 leafInfo = ivec3(texelFetch(bvh, offset + 1).xyz);
    node.left = childs.x;
    node.right = childs.y;
    node.n = leafInfo.x;
    node.index = leafInfo.y;
    node.minPoint = texelFetch(bvh, offset + 2).xyz;
    node.maxPoint = texelFetch(bvh, offset + 3).xyz;

    return node;
}
//功能部分
Hit hitTriangle(Triangle tri, Ray ray)
{
    Hit ret;
    ret.dist = INF;
    ret.isHit = false;
    ret.isInside = false;
    vec3 N = normalize(cross(tri.p2 - tri.p1, tri.p3 - tri.p1));
    float dotND = dot(N, ray.dir);
    // 光线从内部打到三角形
    if (dot(N, ray.dir) > 0.0f)
    {
        ret.isInside = true;
        N = -N;
    }
    // 视线和三角形平行
    if (abs(dot(N, ray.dir)) < 0.00001f)
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
    mollerRet = 1.0 / dot(s1, e1) * vec3(dot(s2, e2), dot(s1, s), dot(s2, ray.dir));
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
    vec3 smoothNormal = normalize(b1 * tri.n1 + b2 * tri.n2 + (1 - b1 - b2) * tri.n3);
//    ret.normal = !ret.isInside ? smoothNormal : -smoothNormal;
    ret.normal=N;
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

// 和 aabb 盒子求交，没有交点则返回 -1
float hitAABB(Ray r, vec3 AA, vec3 BB)
{
    vec3 invdir = 1.0 / r.dir;
    vec3 far = (BB - r.o) * invdir;
    vec3 near = (AA - r.o) * invdir;
    vec3 tmax = max(far, near);
    vec3 tmin = min(far, near);
    float t1 = min(tmax.x, min(tmax.y, tmax.z));
    float t0 = max(tmin.x, max(tmin.y, tmin.z));
    return (t1 >= t0) ? ((t0 > 0.0) ? (t0) : (t1)) : (-1);
}
Hit hitBVH(Ray ray)
{
    Hit ret;
    ret.isHit = false;
    ret.dist = INF;
    //手模栈
    int stack[256];
    int sp = 0;
    //放入根节点
    stack[sp++] = 1;
    while (sp > 0)
    {
        int top = stack[--sp];
        BVHNode node = getNode(top);
        //若是叶子节点则和一系列三角形求交
        if (node.n > 0)
        {
            Hit hitTriangles;
            hitTriangles = hitArray(node.index, node.index + node.n - 1, ray);
            if (hitTriangles.isHit && hitTriangles.dist < ret.dist)
            {
                ret = hitTriangles;
            }
            continue;
        }
        // 分别到两个碰撞盒的距离，用于提前裁剪
        float dist2L = INF, dist2R = INF;
        if (node.left > 0)
        {
            BVHNode leftNode = getNode(node.left);
            dist2L = hitAABB(ray, leftNode.minPoint, leftNode.maxPoint);
        }
        if (node.right > 0)
        {
            BVHNode rightNode = getNode(node.right);
            dist2R = hitAABB(ray, rightNode.minPoint, rightNode.maxPoint);
        }
        if (dist2L > 0 && dist2R > 0)
        {
            if (dist2L < dist2R)
            {
                //左边距离近则先左再右,先入栈右是因为栈的特性先进后出
                stack[sp++] = node.right;
                stack[sp++] = node.left;
            }
            else
            {
                stack[sp++] = node.left;
                stack[sp++] = node.right;
            }
        }
        else if (dist2L > 0)
        {
            stack[sp++] = node.left;
        }
        else if (dist2R > 0)
        {
            stack[sp++] = node.right;
        }
    }
    return ret;
}
// 半球均匀采样
vec3 SampleHemisphereInZ()
{
    float z = rand();
    float r = max(0, sqrt(1.0 - z * z));
    float phi = 2.0 * PI * rand();
    return vec3(r * cos(phi), r * sin(phi), z);
}
// 将向量 v 投影到 N 的法向半球
vec3 toNormalHemisphere(vec3 v, vec3 N) {
    vec3 helper = vec3(1, 0, 0);
    if (abs(N.x) > 0.999) helper = vec3(0, 0, 1);
    vec3 tangent = normalize(cross(N, helper));
    vec3 bitangent = normalize(cross(N, tangent));
    return v.x * tangent + v.y * bitangent + v.z * N;
}
vec3 SampleHemisphere(vec3 N)
{
    return toNormalHemisphere(SampleHemisphereInZ(), N);
}
//用循环模拟递归
vec3 pathTracing(Hit hit, int maxBounce)
{
    vec3 Lo = vec3(0), bounceFactor = vec3(1);
    for (int i = 1;i <= maxBounce; i++)
    {
        vec3 randomDir = SampleHemisphere(hit.normal);
        Ray randomRay;
        randomRay.o = hit.hitPoint;
        randomRay.dir = randomDir;
        Hit randomHit = hitBVH(randomRay);
        float pdf = 1 / (2.0 * PI);
        float cos_o = max(0, dot(-hit.lightDir, hit.normal));
        float cos_i = max(0, dot(randomDir, hit.normal));
        vec3 f_r = hit.m.baseColor / PI;//BRDF
        if (!randomHit.isHit)
        {
//            Lo += bounceFactor * f_r * cos_i / pdf;
            continue;
        }

        Lo += bounceFactor * randomHit.m.emissive * f_r * cos_i / pdf;

        hit = randomHit;
        bounceFactor *= f_r * cos_i / pdf;
    }
    return Lo;
}

void main()
{
    Ray ray;
    ray.o = vec3(0, 0, 4);
    ray.dir = normalize(vec3(pix.xy, 2) - ray.o);
    Hit firstHit = hitBVH(ray);
    if (!firstHit.isHit)
    {
        color = vec4(0);
    }
    else
    {
        vec3 L = firstHit.m.emissive + pathTracing(firstHit, 10);
        color = vec4(L, 1);
    }

}
