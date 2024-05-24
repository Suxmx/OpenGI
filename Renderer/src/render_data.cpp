#include "render_data.h"
#define INF 9999999

vector<triangle_encoded> encodeTriangles(vector<triangle> &triangles)
{
    vector<triangle_encoded> ets;
    ets.reserve(triangles.size());
    for (const auto &t: triangles)
    {
        triangle_encoded et{};
        et.p1 = t.p1;
        et.p2 = t.p2;
        et.p3 = t.p3;
        et.n1 = t.n1;
        et.n2 = t.n2;
        et.n3 = t.n3;
        for (int i = 0; i < 3; i++)
        {
            et.emissive[i] = t.material.emission[i];
            et.baseColor[i] = t.material.diffuse[i];
        }
        //for test
//        et.baseColor = vec3(1, 1, 0);
        ets.emplace_back(et);
    }
    return ets;
}

vector<BVHNode_encoded> encodeBVHNodes(vector<BVHNode> &bvhs)
{
    vector<BVHNode_encoded> ebs;
    for(const auto& node:bvhs)
    {
        BVHNode_encoded eb{};
        eb.AA=node.AA;
        eb.BB=node.BB;
        eb.childs=vec3(node.left,node.right,0);
        eb.leafInfo=vec3(node.n,node.index,0);
        ebs.emplace_back(eb);
    }
    return ebs;
}

bool cmpx(const triangle &t1, const triangle &t2)
{
    vec3 center1 = (t1.p1 + t1.p2 + t1.p3) / vec3(3, 3, 3);
    vec3 center2 = (t2.p1 + t2.p2 + t2.p3) / vec3(3, 3, 3);
    return center1.x < center2.x;
}

bool cmpy(const triangle &t1, const triangle &t2)
{
    vec3 center1 = (t1.p1 + t1.p2 + t1.p3) / vec3(3, 3, 3);
    vec3 center2 = (t2.p1 + t2.p2 + t2.p3) / vec3(3, 3, 3);
    return center1.y < center2.y;
}

bool cmpz(const triangle &t1, const triangle &t2)
{
    vec3 center1 = (t1.p1 + t1.p2 + t1.p3) / vec3(3, 3, 3);
    vec3 center2 = (t2.p1 + t2.p2 + t2.p3) / vec3(3, 3, 3);
    return center1.z < center2.z;
}

// SAH 优化构建 BVH
int buildBVHwithSAH(std::vector<triangle> &triangles, std::vector<BVHNode> &nodes, int l, int r, int n)
{
    if (l > r) return 0;

    nodes.push_back(BVHNode());
    int id = nodes.size() - 1;
    nodes[id].left = nodes[id].right = nodes[id].n = nodes[id].index = 0;
    nodes[id].AA = vec3(1145141919, 1145141919, 1145141919);
    nodes[id].BB = vec3(-1145141919, -1145141919, -1145141919);

    // 计算 AABB
    for (int i = l; i <= r; i++)
    {
        // 最小点 AA
        float minx = glm::min(triangles[i].p1.x, glm::min(triangles[i].p2.x, triangles[i].p3.x));
        float miny = glm::min(triangles[i].p1.y, glm::min(triangles[i].p2.y, triangles[i].p3.y));
        float minz = glm::min(triangles[i].p1.z, glm::min(triangles[i].p2.z, triangles[i].p3.z));
        nodes[id].AA.x = glm::min(nodes[id].AA.x, minx);
        nodes[id].AA.y = glm::min(nodes[id].AA.y, miny);
        nodes[id].AA.z = glm::min(nodes[id].AA.z, minz);
        // 最大点 BB
        float maxx = glm::max(triangles[i].p1.x, glm::max(triangles[i].p2.x, triangles[i].p3.x));
        float maxy = glm::max(triangles[i].p1.y, glm::max(triangles[i].p2.y, triangles[i].p3.y));
        float maxz = glm::max(triangles[i].p1.z, glm::max(triangles[i].p2.z, triangles[i].p3.z));
        nodes[id].BB.x = glm::max(nodes[id].BB.x, maxx);
        nodes[id].BB.y = glm::max(nodes[id].BB.y, maxy);
        nodes[id].BB.z = glm::max(nodes[id].BB.z, maxz);
    }

    // 不多于 n 个三角形 返回叶子节点
    if ((r - l + 1) <= n)
    {
        nodes[id].n = r - l + 1;
        nodes[id].index = l;
        return id;
    }

    // 否则递归建树
    float Cost = INF;
    int Axis = 0;
    int Split = (l + r) / 2;
    for (int axis = 0; axis < 3; axis++)
    {
        // 分别按 x，y，z 轴排序
        if (axis == 0) std::sort(&triangles[0] + l, &triangles[0] + r + 1, cmpx);
        if (axis == 1) std::sort(&triangles[0] + l, &triangles[0] + r + 1, cmpy);
        if (axis == 2) std::sort(&triangles[0] + l, &triangles[0] + r + 1, cmpz);

        // leftMax[i]: [l, i] 中最大的 xyz 值
        // leftMin[i]: [l, i] 中最小的 xyz 值
        std::vector<vec3> leftMax(r - l + 1, vec3(-INF, -INF, -INF));
        std::vector<vec3> leftMin(r - l + 1, vec3(INF, INF, INF));
        // 计算前缀 注意 i-l 以对齐到下标 0
        for (int i = l; i <= r; i++)
        {
            triangle &t = triangles[i];
            int bias = (i == l) ? 0 : 1;  // 第一个元素特殊处理

            leftMax[i - l].x = glm::max(leftMax[i - l - bias].x, glm::max(t.p1.x, glm::max(t.p2.x, t.p3.x)));
            leftMax[i - l].y = glm::max(leftMax[i - l - bias].y, glm::max(t.p1.y, glm::max(t.p2.y, t.p3.y)));
            leftMax[i - l].z = glm::max(leftMax[i - l - bias].z, glm::max(t.p1.z, glm::max(t.p2.z, t.p3.z)));

            leftMin[i - l].x = glm::min(leftMin[i - l - bias].x, glm::min(t.p1.x, glm::min(t.p2.x, t.p3.x)));
            leftMin[i - l].y = glm::min(leftMin[i - l - bias].y, glm::min(t.p1.y, glm::min(t.p2.y, t.p3.y)));
            leftMin[i - l].z = glm::min(leftMin[i - l - bias].z, glm::min(t.p1.z, glm::min(t.p2.z, t.p3.z)));
        }

        // rightMax[i]: [i, r] 中最大的 xyz 值
        // rightMin[i]: [i, r] 中最小的 xyz 值
        std::vector<vec3> rightMax(r - l + 1, vec3(-INF, -INF, -INF));
        std::vector<vec3> rightMin(r - l + 1, vec3(INF, INF, INF));
        // 计算后缀 注意 i-l 以对齐到下标 0
        for (int i = r; i >= l; i--)
        {
            triangle &t = triangles[i];
            int bias = (i == r) ? 0 : 1;  // 第一个元素特殊处理

            rightMax[i - l].x = glm::max(rightMax[i - l + bias].x, glm::max(t.p1.x, glm::max(t.p2.x, t.p3.x)));
            rightMax[i - l].y = glm::max(rightMax[i - l + bias].y, glm::max(t.p1.y, glm::max(t.p2.y, t.p3.y)));
            rightMax[i - l].z = glm::max(rightMax[i - l + bias].z, glm::max(t.p1.z, glm::max(t.p2.z, t.p3.z)));

            rightMin[i - l].x = glm::min(rightMin[i - l + bias].x, glm::min(t.p1.x, glm::min(t.p2.x, t.p3.x)));
            rightMin[i - l].y = glm::min(rightMin[i - l + bias].y, glm::min(t.p1.y, glm::min(t.p2.y, t.p3.y)));
            rightMin[i - l].z = glm::min(rightMin[i - l + bias].z, glm::min(t.p1.z, glm::min(t.p2.z, t.p3.z)));
        }

        // 遍历寻找分割
        float cost = INF;
        int split = l;
        for (int i = l; i <= r - 1; i++)
        {
            float lenx, leny, lenz;
            // 左侧 [l, i]
            vec3 leftAA = leftMin[i - l];
            vec3 leftBB = leftMax[i - l];
            lenx = leftBB.x - leftAA.x;
            leny = leftBB.y - leftAA.y;
            lenz = leftBB.z - leftAA.z;
            float leftS = 2.0 * ((lenx * leny) + (lenx * lenz) + (leny * lenz));
            float leftCost = leftS * (i - l + 1);

            // 右侧 [i+1, r]
            vec3 rightAA = rightMin[i + 1 - l];
            vec3 rightBB = rightMax[i + 1 - l];
            lenx = rightBB.x - rightAA.x;
            leny = rightBB.y - rightAA.y;
            lenz = rightBB.z - rightAA.z;
            float rightS = 2.0 * ((lenx * leny) + (lenx * lenz) + (leny * lenz));
            float rightCost = rightS * (r - i);

            // 记录每个分割的最小答案
            float totalCost = leftCost + rightCost;
            if (totalCost < cost)
            {
                cost = totalCost;
                split = i;
            }
        }
        // 记录每个轴的最佳答案
        if (cost < Cost)
        {
            Cost = cost;
            Axis = axis;
            Split = split;
        }
    }

    // 按最佳轴分割
    if (Axis == 0) std::sort(&triangles[0] + l, &triangles[0] + r + 1, cmpx);
    if (Axis == 1) std::sort(&triangles[0] + l, &triangles[0] + r + 1, cmpy);
    if (Axis == 2) std::sort(&triangles[0] + l, &triangles[0] + r + 1, cmpz);

    // 递归
    int left = buildBVHwithSAH(triangles, nodes, l, Split, n);
    int right = buildBVHwithSAH(triangles, nodes, Split + 1, r, n);

    nodes[id].left = left;
    nodes[id].right = right;

    return id;
}