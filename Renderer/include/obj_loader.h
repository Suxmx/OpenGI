//
// Created by 42450 on 2024/5/8.
//
#pragma once

//Macro
#define IS_SPACE(x) (((x) == ' ') || ((x) == '\t'))
#define IS_DIGIT(x) (static_cast<unsigned int>((x) - '0') < static_cast<unsigned int>(10))
#define IS_NEW_LINE(x) (((x) == '\r') || ((x) == '\n') || ((x) == '\0'))
#define LAST_OF(x) (x[x.size()-1])
//Header
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <cassert>

class Model;

bool LoadObj(const std::string &filename, Model *model, std::string *err, std::string *warn, bool colorFallback);

//定义各类数据结构
struct warning_context
{
    std::string *warn;
    size_t line_number;
};
struct index_t
{
    int vertex_index;
    int normal_index;
    int texcoord_index;
};
struct mesh_t
{
    std::vector<index_t> indices;
    std::vector<unsigned int> num_face_vertices;// 每个面的顶点数
    std::vector<int> material_ids;  // 每个面的材质ID

    std::vector<unsigned int> smoothingGroupId;
};
struct shape_t
{
    std::string name;
    mesh_t mesh;
};
typedef enum
{
    TEXTURE_TYPE_NONE,  // default
    TEXTURE_TYPE_SPHERE,
    TEXTURE_TYPE_CUBE_TOP,
    TEXTURE_TYPE_CUBE_BOTTOM,
    TEXTURE_TYPE_CUBE_FRONT,
    TEXTURE_TYPE_CUBE_BACK,
    TEXTURE_TYPE_CUBE_LEFT,
    TEXTURE_TYPE_CUBE_RIGHT
} texture_type_t;
struct texture_option_t
{
    texture_type_t type;      // 贴图种类
    float sharpness;         // 默认 1.0
    float brightness;        // 默认 0
    float contrast;          // 默认 1
    float origin_offset[3];  // 默认 0 0 0
    float scale[3];          // 默认 1 1 1
    float turbulence[3];     // 默认 0 0 0
    int texture_resolution;
    bool clamp;               // 默认 false
    char imfchan;  // bump贴图默认值为l，其他贴图默认值为m
    bool blendu;   // 默认 true
    bool blendv;   // 默认 true
    float bump_multiplier;  //  默认 1.0

    // 拓展
    std::string colorspace;  // 色彩空间，通常是sRGB或者线性，默认为空
};
struct material_t
{
    std::string name;
    float ambient[3];
    float diffuse[3];
    float specular[3];
    float transmittance[3];
    float emission[3];
    float shininess;
    float ior;       // 折射率
    float dissolve;  //"1 == 不透明，0 == 完全透明"
    int illum;

    int dummy;
    // 贴图名称
    std::string ambient_texname;   // map_Ka，用于环境或环境遮蔽
    std::string diffuse_texname;   // map_Kd
    std::string specular_texname;  // map_Ks
    std::string specular_highlight_texname;  // map_Ns
    std::string bump_texname;                // map_bump, map_Bump, bump
    std::string displacement_texname;        // disp
    std::string alpha_texname;               // map_d
    std::string reflection_texname;          // refl

    texture_option_t ambient_texopt;
    texture_option_t diffuse_texopt;
    texture_option_t specular_texopt;
    texture_option_t specular_highlight_texopt;
    texture_option_t bump_texopt;
    texture_option_t displacement_texopt;
    texture_option_t alpha_texopt;
    texture_option_t reflection_texopt;

    // PBR 拓展
    float roughness;            // [0, 1] 默认 0
    float metallic;             // [0, 1] 默认 0
    float sheen;                // [0, 1] 默认 0
    float clearcoat_thickness;  // [0, 1] 默认 0
    float clearcoat_roughness;  // [0, 1] 默认 0
    float anisotropy;           // aniso. [0, 1] 默认 0
    float anisotropy_rotation;  // anisor. [0, 1] 默认 0
    float pad0;
    std::string roughness_texname;  // map_Pr
    std::string metallic_texname;   // map_Pm
    std::string sheen_texname;      // map_Ps
    std::string emissive_texname;   // map_Ke
    std::string normal_texname;     // norm. 法线贴图用

    texture_option_t roughness_texopt;
    texture_option_t metallic_texopt;
    texture_option_t sheen_texopt;
    texture_option_t emissive_texopt;
    texture_option_t normal_texopt;

    int pad2;
    std::map<std::string, std::string> unknown_parameter;

};

struct vertex_attribute_t
{
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texcoords;//uv
    std::vector<float> colors; // 额外的顶点颜色拓展
    void Free()
    {
        vertices.clear();
        normals.clear();
        texcoords.clear();
        colors.clear();
    }
};

struct vertex_index_t
{
    int vIndex, vtIndex, vnIndex;

    vertex_index_t() : vIndex(-1), vtIndex(-1), vnIndex(-1)
    {}

    explicit vertex_index_t(int idx) : vIndex(idx), vtIndex(idx), vnIndex(idx)
    {}

    vertex_index_t(int vidx, int vtidx, int vnidx)
            : vIndex(vidx), vtIndex(vtidx), vnIndex(vnidx)
    {}
};

struct face_t
{
    std::vector<vertex_index_t> vertexIndices;
    unsigned int smoothingGroupId;
};
struct primitive_group_t
{
    std::vector<face_t> faces;
};

//综合模型类
class Model
{
public:
    // 模型名称
    std::string name;
    // 材质文件夹
    std::string mtlBaseDir;
    // 各个属性的数量
    unsigned long long num_vertices{};
    unsigned int num_shapes{};
    unsigned int num_materials{};
    // 顶点的基本属性
    vertex_attribute_t vertexAttribute;
    // shape和material
    std::vector<shape_t> shapes;
    std::vector<material_t> materials;

    //利用文件名或路径来读取模型
    Model(std::string name, std::string mtlBaseDir);

    bool Load(std::string *err, std::string *warn)
    {
        return LoadObj(name, this, err, warn, true);
    }

    void Free()
    {
        num_shapes = num_vertices = num_materials = 0;
        vertexAttribute.Free();
        shapes.clear();
        materials.clear();
    }
};

Model::Model(std::string name, std::string mtlBaseDir = "")
{
    this->name = std::move(name);
    this->mtlBaseDir = std::move(mtlBaseDir);
}

// 初始化材质
static void InitMaterial(material_t *material)
{
    material->name = "";
    material->ambient_texname = "";
    material->diffuse_texname = "";
    material->specular_texname = "";
    material->specular_highlight_texname = "";
    material->bump_texname = "";
    material->displacement_texname = "";
    material->reflection_texname = "";
    material->alpha_texname = "";
    for (int i = 0; i < 3; i++)
    {
        material->ambient[i] = static_cast<float>(0.0);
        material->diffuse[i] = static_cast<float>(0.0);
        material->specular[i] = static_cast<float>(0.0);
        material->transmittance[i] = static_cast<float>(0.0);
        material->emission[i] = static_cast<float>(0.0);
    }
    material->illum = 0;
    material->dissolve = static_cast<float>(1.0);
    material->shininess = static_cast<float>(1.0);
    material->ior = static_cast<float>(1.0);

    material->roughness = static_cast<float>(0.0);
    material->metallic = static_cast<float>(0.0);
    material->sheen = static_cast<float>(0.0);
    material->clearcoat_thickness = static_cast<float>(0.0);
    material->clearcoat_roughness = static_cast<float>(0.0);
    material->anisotropy_rotation = static_cast<float>(0.0);
    material->anisotropy = static_cast<float>(0.0);
    material->roughness_texname = "";
    material->metallic_texname = "";
    material->sheen_texname = "";
    material->emissive_texname = "";
    material->normal_texname = "";

    material->unknown_parameter.clear();
}

// 用于将各种类型直接转换成string
template<typename T>
static inline std::string toString(const T &t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}

//用于在CRLF和LF两种情况下来读取流中的一行
static std::istream &safeGetLine(std::string &str, std::istream &stream)
{
    //清除文本缓冲区
    str.clear();

    //检查流的可用性
    std::istream::sentry sentry(stream, true);
    //利用streambuf读取会比istream读取更快，但是同时也需要受sentry保护
    std::streambuf *streambuf = stream.rdbuf();
    if (!sentry)return stream;
    //循环读取整行中的字符
    while (true)
    {
        int ch = streambuf->sbumpc();
        switch (ch)
        {
            case '\n':
                return stream;
            case '\r':
                //若是\r\n的结构则读取\r后将指针再向后挪一位
                if (streambuf->sgetc() == '\n')streambuf->sbumpc();
                return stream;
            case EOF:
                if (str.empty()) stream.setstate(std::ios::eofbit);
                return stream;
            default:
                str += static_cast<char>(ch);
                break;
        }
    }
    return stream;
}


//   可以接受的数据实例:
//   -0  +3.1417e+2  -0.0E-3  1.0324  -1.41   11e2
static bool tryParseDouble(const char *s, const char *sEnd, double *result)
{
    if (s >= sEnd)
    {
        return false;
    }

    double mantissa = 0.0;

    int exponent = 0;
    char sign = '+';
    char exp_sign = '+';
    char const *curr = s;

    // 读取了多少个字符
    int read = 0;
    // 判断读取循环是否结束
    bool endNotReached = false;
    bool leading_decimal_dots = false;

    //----------开始读取----------
    // 判断符号
    if (*curr == '+' || *curr == '-')
    {
        sign = *curr;
        curr++;
        if ((curr != sEnd) && (*curr == '.'))
        {
            // 判断诸如 .7e+2, -.5234等数字
            leading_decimal_dots = true;
        }
    }
    else if (IS_DIGIT(*curr))
    { //若是数字则直接跳过
    }
    else if (*curr == '.')
    {
        // 判断诸如 .7e+2, -.5234等数字
        leading_decimal_dots = true;
    }
    else
    {
        goto fail;
    }

    // 读取整数部分
    endNotReached = (curr != sEnd);
    if (!leading_decimal_dots)
    {
        while (endNotReached && IS_DIGIT(*curr))
        {
            mantissa *= 10;
            mantissa += static_cast<int>(*curr - 0x30);
            curr++, read++;
            endNotReached = (curr != sEnd);
        }

        // 必须保证读取到了整数
        if (read == 0) goto fail;
    }

    // 如果仅有整数部分
    if (!endNotReached) goto assemble;

    // 读取小数部分
    if (*curr == '.')
    {
        curr++;
        read = 1;
        endNotReached = (curr != sEnd);
        while (endNotReached && IS_DIGIT(*curr))
        {
            static const double pow_lut[] = {
                    1.0, 0.1, 0.01, 0.001, 0.0001, 0.00001, 0.000001, 0.0000001,
            };
            const int lutEntries = sizeof pow_lut / sizeof pow_lut[0];

            mantissa += static_cast<int>(*curr - 0x30) *
                        (read < lutEntries ? pow_lut[read] : std::pow(10.0, -read));
            read++, curr++;
            endNotReached = (curr != sEnd);
        }
    }
    else if (*curr == 'e' || *curr == 'E')
    {
    }
    else
    {
        goto assemble;
    }

    if (!endNotReached) goto assemble;

    // 读取指数部分
    if (*curr == 'e' || *curr == 'E')
    {
        curr++;
        // 判断是否是符号，是什么符号
        endNotReached = (curr != sEnd);
        if (endNotReached && (*curr == '+' || *curr == '-'))
        {
            exp_sign = *curr;
            curr++;
        }
        else if (IS_DIGIT(*curr))
        { // 跳过
        }
        else
        {
            // 单独的E没有意义
            goto fail;
        }

        read = 0;
        endNotReached = (curr != sEnd);
        while (endNotReached && IS_DIGIT(*curr))
        {
            // 防止溢出
            if (exponent > (2147483647 / 10))
            {
                goto fail;
            }
            exponent *= 10;
            exponent += static_cast<int>(*curr - 0x30);
            curr++;
            read++;
            endNotReached = (curr != sEnd);
        }
        exponent *= (exp_sign == '+' ? 1 : -1);
        if (read == 0) goto fail;
    }

    assemble:
    *result = (sign == '+' ? 1 : -1) *
              (exponent ? std::ldexp(mantissa * std::pow(5.0, exponent), exponent)
                        : mantissa);
    return true;
    fail:
    return false;
}

// 用于将f中的序号标准化为顶点数组中的下标
static inline bool fixIndex(int idx, int n, int *ret, bool allow_zero,
                            const warning_context &context)
{
    if (!ret)
    {
        return false;
    }
    // 若大于0则直接转化成下标返回
    if (idx > 0)
    {
        (*ret) = idx - 1;
        return true;
    }

    if (idx == 0)
    {
        // 序号不得为0
        if (context.warn)
        {
            (*context.warn) +=
                    "在第" +
                    toString(context.line_number)
                    + "行中发现0值序号，f中0值序号实际会指向数组中下标为-1的数据 ";
        }

        (*ret) = idx - 1;
        return allow_zero;
    }

    if (idx < 0)
    {   // 负值则从后往前数
        (*ret) = n + idx;
        // 转换后不能为负值
        if ((*ret) < 0)
        {

            return false;
        }
        return true;
    }
    // 防止IDE警告
    return false;
}

static inline float parseFloat(const char **token, double defaultValue = 0.0)
{
    //跳过所有空格与制表符
    (*token) += strspn((*token), " \t");
    const char *end = (*token) + strcspn((*token), " \t\r");
    double ret = defaultValue;
    tryParseDouble((*token), end, &ret);
    (*token) = end;
    return static_cast<float>(ret);
}

// 判断是否读取成功的parseFloat
static inline bool parseFloat(const char **token, float *out)
{
    (*token) += strspn((*token), " \t");
    const char *end = (*token) + strcspn((*token), " \t\r");
    double val;
    bool ret = tryParseDouble((*token), end, &val);
    if (ret)
    {
        (*out) = static_cast<float>(val);
    }
    (*token) = end;
    return ret;
}

static inline int parseInt(const char **token)
{
    (*token) += strspn((*token), " \t");
    int i = atoi((*token));
    (*token) += strcspn((*token), " \t\r");
    return i;
}

static inline bool parseBool(const char **token, bool default_value = true)
{
    (*token) += strspn((*token), " \t");
    const char *end = (*token) + strcspn((*token), " \t\r");

    bool ret = default_value;
    if ((0 == strncmp((*token), "on", 2)))
    {
        ret = true;
    }
    else if ((0 == strncmp((*token), "off", 3)))
    {
        ret = false;
    }

    (*token) = end;
    return ret;
}

// 用于读取f项中的i/j/k、i//k、i/j几种情况的序号
static bool parseTriple(const char **token, int vsize, int vnsize, int vtsize,
                        vertex_index_t *ret, const warning_context &context)
{
    if (!ret)
    {
        return false;
    }

    vertex_index_t vi(-1);

    if (!fixIndex(atoi((*token)), vsize, &vi.vIndex, false, context))
    {
        return false;
    }

    (*token) += strcspn((*token), "/ \t\r");
    if ((*token)[0] != '/')
    {
        (*ret) = vi;
        return true;
    }
    (*token)++;

    // i//k
    if ((*token)[0] == '/')
    {
        (*token)++;
        if (!fixIndex(atoi((*token)), vnsize, &vi.vnIndex, true, context))
        {
            return false;
        }
        (*token) += strcspn((*token), "/ \t\r");
        (*ret) = vi;
        return true;
    }

    // i/j/k 或者 i/j
    if (!fixIndex(atoi((*token)), vtsize, &vi.vtIndex, true, context))
    {
        return false;
    }

    (*token) += strcspn((*token), "/ \t\r");
    if ((*token)[0] != '/')
    {
        (*ret) = vi;
        return true;
    }

    // i/j/k
    (*token)++;  // skip '/'
    if (!fixIndex(atoi((*token)), vnsize, &vi.vnIndex, true, context))
    {
        return false;
    }
    (*token) += strcspn((*token), "/ \t\r");

    (*ret) = vi;

    return true;
}

static bool exportGroupsToShape(shape_t *shape, const primitive_group_t &primitiveGroup, const std::string &name,
                                const std::vector<float> &v, int materialId, std::string *warn);

static std::string JoinPath(const std::string &dir,
                            const std::string &filename)
{
    if (dir.empty())
    {
        return filename;
    }
    else
    {
        // check '/'
        char lastChar = *dir.rbegin();
        if (lastChar != '/')
        {
            return dir + std::string("/") + filename;
        }
        else
        {
            return dir + filename;
        }
    }
}

static void SplitString(const std::string &s, char delim, char escape,
                        std::vector<std::string> &elems)
{
    std::string token;

    bool escaping = false;
    for (size_t i = 0; i < s.size(); ++i)
    {
        char ch = s[i];
        if (escaping)
        {
            escaping = false;
        }
        else if (ch == escape)
        {
            escaping = true;
            continue;
        }
        else if (ch == delim)
        {
            if (!token.empty())
            {
                elems.push_back(token);
            }
            token.clear();
            continue;
        }
        token += ch;
    }

    elems.push_back(token);
}

// 用于解释贴图种类
static inline texture_type_t parseTextureType(
        const char **token, texture_type_t default_value = TEXTURE_TYPE_NONE)
{
    (*token) += strspn((*token), " \t");
    const char *end = (*token) + strcspn((*token), " \t\r");
    texture_type_t ty = default_value;

    if ((0 == strncmp((*token), "cube_top", strlen("cube_top"))))
    {
        ty = TEXTURE_TYPE_CUBE_TOP;
    }
    else if ((0 == strncmp((*token), "cube_bottom", strlen("cube_bottom"))))
    {
        ty = TEXTURE_TYPE_CUBE_BOTTOM;
    }
    else if ((0 == strncmp((*token), "cube_left", strlen("cube_left"))))
    {
        ty = TEXTURE_TYPE_CUBE_LEFT;
    }
    else if ((0 == strncmp((*token), "cube_right", strlen("cube_right"))))
    {
        ty = TEXTURE_TYPE_CUBE_RIGHT;
    }
    else if ((0 == strncmp((*token), "cube_front", strlen("cube_front"))))
    {
        ty = TEXTURE_TYPE_CUBE_FRONT;
    }
    else if ((0 == strncmp((*token), "cube_back", strlen("cube_back"))))
    {
        ty = TEXTURE_TYPE_CUBE_BACK;
    }
    else if ((0 == strncmp((*token), "sphere", strlen("sphere"))))
    {
        ty = TEXTURE_TYPE_SPHERE;
    }

    (*token) = end;
    return ty;
}

// 解释贴图以及其设置项
bool ParseTextureNameAndOption(std::string *texname, texture_option_t *texopt,
                               const char *linebuf)
{
    bool found_texname = false;
    std::string texture_name;

    const char *token = linebuf;

    while (!IS_NEW_LINE((*token)))
    {
        token += strspn(token, " \t");  // 跳过空格
        if ((0 == strncmp(token, "-blendu", 7)) && IS_SPACE((token[7])))
        {
            token += 8;
            texopt->blendu = parseBool(&token, true);
        }
        else if ((0 == strncmp(token, "-blendv", 7)) && IS_SPACE((token[7])))
        {
            token += 8;
            texopt->blendv = parseBool(&token, true);
        }
        else if ((0 == strncmp(token, "-clamp", 6)) && IS_SPACE((token[6])))
        {
            token += 7;
            texopt->clamp = parseBool(&token, true);
        }
        else if ((0 == strncmp(token, "-boost", 6)) && IS_SPACE((token[6])))
        {
            token += 7;
            texopt->sharpness = parseFloat(&token, 1.0);
        }
        else if ((0 == strncmp(token, "-bm", 3)) && IS_SPACE((token[3])))
        {
            token += 4;
            texopt->bump_multiplier = parseFloat(&token, 1.0);
        }
        else if ((0 == strncmp(token, "-o", 2)) && IS_SPACE((token[2])))
        {
            token += 3;
            texopt->origin_offset[0] = parseFloat(&token);
            texopt->origin_offset[1] = parseFloat(&token);
            texopt->origin_offset[2] = parseFloat(&token);
        }
        else if ((0 == strncmp(token, "-s", 2)) && IS_SPACE((token[2])))
        {
            token += 3;
            texopt->scale[0] = parseFloat(&token, 1.0);
            texopt->scale[1] = parseFloat(&token, 1.0);
            texopt->scale[2] = parseFloat(&token, 1.0);
        }
        else if ((0 == strncmp(token, "-t", 2)) && IS_SPACE((token[2])))
        {
            token += 3;
            texopt->turbulence[0] = parseFloat(&token);
            texopt->turbulence[1] = parseFloat(&token);
            texopt->turbulence[2] = parseFloat(&token);
        }
        else if ((0 == strncmp(token, "-type", 5)) && IS_SPACE((token[5])))
        {
            token += 5;
            texopt->type = parseTextureType((&token), TEXTURE_TYPE_NONE);
        }
        else if ((0 == strncmp(token, "-texres", 7)) && IS_SPACE((token[7])))
        {
            token += 7;
            texopt->texture_resolution = parseInt(&token);
        }
        else if ((0 == strncmp(token, "-imfchan", 8)) && IS_SPACE((token[8])))
        {
            token += 9;
            token += strspn(token, " \t");
            const char *end = token + strcspn(token, " \t\r");
            if ((end - token) == 1)
            {
                texopt->imfchan = (*token);
            }
            token = end;
        }
        else if ((0 == strncmp(token, "-mm", 3)) && IS_SPACE((token[3])))
        {
            token += 4;
            texopt->brightness = parseFloat(&token, 0.0);
            texopt->contrast = parseFloat(&token, 1.0);
        }
        else if ((0 == strncmp(token, "-colorspace", 11)) &&
                 IS_SPACE((token[11])))
        {
            token += 12;
            texopt->colorspace = std::string(token, &token[strcspn(token, " \t\r")]);
        }
        else
        {
            // 设置贴图名称
            texture_name = std::string(token);
            token += texture_name.length();


            found_texname = true;
        }
    }

    if (found_texname)
    {
        (*texname) = texture_name;
        return true;
    }
    else
    {
        return false;
    }
}

static bool LoadMtl(const std::string &matId, const std::string &mtlBaseDir, std::vector<material_t> *materials,
                    std::map<std::string, int> *matMap,
                    std::string *warn, std::string *err)
{
    if (!mtlBaseDir.empty() || std::ifstream(matId))
    {
        // 根据不同系统切换分隔符
#ifdef _WIN32
        char sep = ';';
#else
        char sep = ':';
#endif
        std::vector<std::string> paths;
        std::istringstream f(mtlBaseDir);
        std::string buf;
        std::ifstream matStream;
        // 将待搜索的文件夹加入
        while (std::getline(f, buf, sep))
        {
            paths.push_back(buf);
        }
        // 在提供的各个文件夹中寻找材质文件
        for (const auto &path: paths)
        {
            std::string filePath = JoinPath(path, matId);
            matStream = std::ifstream(filePath.c_str());
            if (matStream)break;
        }
        //如果未提供文件夹则尝试直接使用材质名称在当前文件夹下进行寻找
        if (!matStream)matStream = std::ifstream(matId);
        if (!matStream)
        {
            if (warn)
                (*warn) += "未能在 " + mtlBaseDir + " 中找到材质文件 [ " + matId + " ] ! \n ";
            return false;
        }
        material_t material;
        size_t lineNum = 0;
        std::string lineBuffer;
        // d项和Tr项不应该同时出现，因为他们都会覆盖dissolve项，所以这里应该做出提醒
        bool hasd = false, hasTr;
        while (matStream.peek() != -1)
        {
            safeGetLine(lineBuffer, matStream);
            lineNum++;
            // 去除尾部空白
            if (!lineBuffer.empty())
                lineBuffer = lineBuffer.substr(0, lineBuffer.find_last_not_of(" \t") + 1);
            // 去除尾部换行符和回车符
            if (!lineBuffer.empty() && LAST_OF(lineBuffer) == '\n')
                lineBuffer.erase(lineBuffer.size() - 1);
            if (!lineBuffer.empty() && LAST_OF(lineBuffer) == '\r')
                lineBuffer.erase(lineBuffer.size() - 1);
            if (lineBuffer.empty())
                continue;
            const char *token = lineBuffer.c_str();
            // 去除前缀空白
            token += strspn(token, " \t");
            assert(token);
            if (token[0] == '\0' || token[0] == '#')continue;

            // 创建新材质项
            if (strncmp(token, "newmtl", 6) == 0 && IS_SPACE(token[6]))
            {
                // 若先前已经读取了材质则将其添加至map
                if (!material.name.empty())
                {
                    matMap->insert(std::pair<std::string, int>(material.name, static_cast<int>(materials->size())));
                    materials->push_back(material);
                }
                // 重新初始化材质
                InitMaterial(&material);
                hasd = hasTr = false;
                token += 7;
                // 获取名字
                std::string nameBuffer = std::string(token, &token[strcspn(token, " \t\r")]);
                material.name = nameBuffer;
                continue;
            }
            // 环境光
            if (token[0] == 'K' && token[1] == 'a' && IS_SPACE((token[2])))
            {
                token += 2;
                float r, g, b;
                r = parseFloat(&token, 0.);
                g = parseFloat(&token, 0.);
                b = parseFloat(&token, 0.);
                material.ambient[0] = r;
                material.ambient[1] = g;
                material.ambient[2] = b;
                continue;
            }

            // 漫反射
            if (token[0] == 'K' && token[1] == 'd' && IS_SPACE((token[2])))
            {
                token += 2;
                float r, g, b;
                r = parseFloat(&token, 0.);
                g = parseFloat(&token, 0.);
                b = parseFloat(&token, 0.);
                material.diffuse[0] = r;
                material.diffuse[1] = g;
                material.diffuse[2] = b;
                hasd = true;
                continue;
            }

            // 高光
            if (token[0] == 'K' && token[1] == 's' && IS_SPACE((token[2])))
            {
                token += 2;
                float r, g, b;
                r = parseFloat(&token, 0.);
                g = parseFloat(&token, 0.);
                b = parseFloat(&token, 0.);
                material.specular[0] = r;
                material.specular[1] = g;
                material.specular[2] = b;
                continue;
            }

            // transmittance
            if ((token[0] == 'K' && token[1] == 't' && IS_SPACE((token[2]))) ||
                (token[0] == 'T' && token[1] == 'f' && IS_SPACE((token[2]))))
            {
                token += 2;
                float r, g, b;
                r = parseFloat(&token, 0.);
                g = parseFloat(&token, 0.);
                b = parseFloat(&token, 0.);
                material.transmittance[0] = r;
                material.transmittance[1] = g;
                material.transmittance[2] = b;
                continue;
            }
            // 折射率
            if (token[0] == 'N' && token[1] == 'i' && IS_SPACE((token[2])))
            {
                token += 2;
                material.ior = parseFloat(&token);
                continue;
            }

            // 自发光
            if (token[0] == 'K' && token[1] == 'e' && IS_SPACE(token[2]))
            {
                token += 2;
                float r, g, b;
                r = parseFloat(&token, 0.);
                g = parseFloat(&token, 0.);
                b = parseFloat(&token, 0.);
                material.emission[0] = r;
                material.emission[1] = g;
                material.emission[2] = b;
                continue;
            }

            // shininess
            if (token[0] == 'N' && token[1] == 's' && IS_SPACE(token[2]))
            {
                token += 2;
                material.shininess = parseFloat(&token);
                continue;
            }

            // 光照模型
            if (0 == strncmp(token, "illum", 5) && IS_SPACE(token[5]))
            {
                token += 6;
                material.illum = parseInt(&token);
                continue;
            }
            // dissolve
            if ((token[0] == 'd' && IS_SPACE(token[1])))
            {
                token += 1;
                material.dissolve = parseFloat(&token);

                if (hasTr)
                {
                    std::stringstream warn_ss;
                    warn_ss << "在材质: \""
                            << material.name
                            << "\"中同时使用了d和Tr。请用d来定义 dissolve 的值 (line " << lineNum
                            << " \n";
                    (*warn) += warn_ss.str();
                }
                hasd = true;
                continue;
            }
            if (token[0] == 'T' && token[1] == 'r' && IS_SPACE(token[2]))
            {
                token += 2;
                if (hasd)
                {
                    // 如果d和Tr的值都有则默认使用d的
                    std::stringstream warn_ss;
                    warn_ss << "在材质: \""
                            << material.name
                            << "\"中同时使用了d和Tr。请用d来定义 dissolve 的值 (line " << lineNum
                            << " \n";
                    (*warn) += warn_ss.str();
                }
                else
                {
                    // 取反Tr
                    material.dissolve = static_cast<float>(1.0) - parseFloat(&token);
                }
                hasTr = true;
                continue;
            }
            //-----PBR-----
            // PBR: 粗糙度
            if (token[0] == 'P' && token[1] == 'r' && IS_SPACE(token[2]))
            {
                token += 2;
                material.roughness = parseFloat(&token);
                continue;
            }

            // PBR: 金属度
            if (token[0] == 'P' && token[1] == 'm' && IS_SPACE(token[2]))
            {
                token += 2;
                material.metallic = parseFloat(&token);
                continue;
            }

            // PBR: sheen
            if (token[0] == 'P' && token[1] == 's' && IS_SPACE(token[2]))
            {
                token += 2;
                material.sheen = parseFloat(&token);
                continue;
            }

            // PBR: clearcoat thickness
            if (token[0] == 'P' && token[1] == 'c' && IS_SPACE(token[2]))
            {
                token += 2;
                material.clearcoat_thickness = parseFloat(&token);
                continue;
            }

            // PBR: clearcoat roughness
            if ((0 == strncmp(token, "Pcr", 3)) && IS_SPACE(token[3]))
            {
                token += 4;
                material.clearcoat_roughness = parseFloat(&token);
                continue;
            }

            // PBR: anisotropy
            if ((0 == strncmp(token, "aniso", 5)) && IS_SPACE(token[5]))
            {
                token += 6;
                material.anisotropy = parseFloat(&token);
                continue;
            }

            // PBR: anisotropy rotation
            if ((0 == strncmp(token, "anisor", 6)) && IS_SPACE(token[6]))
            {
                token += 7;
                material.anisotropy_rotation = parseFloat(&token);
                continue;
            }
            // -----PBR-END-----
            // AO贴图
            if ((0 == strncmp(token, "map_Ka", 6)) && IS_SPACE(token[6]))
            {
                token += 7;
                ParseTextureNameAndOption(&(material.ambient_texname),
                                          &(material.ambient_texopt), token);
                continue;
            }

            // 漫反射贴图
            if ((0 == strncmp(token, "map_Kd", 6)) && IS_SPACE(token[6]))
            {
                token += 7;
                ParseTextureNameAndOption(&(material.diffuse_texname),
                                          &(material.diffuse_texopt), token);

                if (!hasd)
                {
                    material.diffuse[0] = static_cast<float>(0.6);
                    material.diffuse[1] = static_cast<float>(0.6);
                    material.diffuse[2] = static_cast<float>(0.6);
                }

                continue;
            }

            // 高光贴图
            if ((0 == strncmp(token, "map_Ks", 6)) && IS_SPACE(token[6]))
            {
                token += 7;
                ParseTextureNameAndOption(&(material.specular_texname),
                                          &(material.specular_texopt), token);
                continue;
            }

            // 高光贴图
            if ((0 == strncmp(token, "map_Ns", 6)) && IS_SPACE(token[6]))
            {
                token += 7;
                ParseTextureNameAndOption(&(material.specular_highlight_texname),
                                          &(material.specular_highlight_texopt), token);
                continue;
            }

            // bump 贴图
            if (((0 == strncmp(token, "map_bump", 8)) ||
                 (0 == strncmp(token, "map_Bump", 8))) &&
                IS_SPACE(token[8]))
            {
                token += 9;
                ParseTextureNameAndOption(&(material.bump_texname),
                                          &(material.bump_texopt), token);
                continue;
            }

            // bump 贴图
            if ((0 == strncmp(token, "bump", 4)) && IS_SPACE(token[4]))
            {
                token += 5;
                ParseTextureNameAndOption(&(material.bump_texname),
                                          &(material.bump_texopt), token);
                continue;
            }

            // alpha 贴图
            if ((0 == strncmp(token, "map_d", 5)) && IS_SPACE(token[5]))
            {
                token += 6;
                material.alpha_texname = token;
                ParseTextureNameAndOption(&(material.alpha_texname),
                                          &(material.alpha_texopt), token);
                continue;
            }

            // displacement 贴图
            if (((0 == strncmp(token, "map_disp", 8)) ||
                 (0 == strncmp(token, "map_Disp", 8))) &&
                IS_SPACE(token[8]))
            {
                token += 9;
                ParseTextureNameAndOption(&(material.displacement_texname),
                                          &(material.displacement_texopt), token);
                continue;
            }

            // displacement 贴图
            if ((0 == strncmp(token, "disp", 4)) && IS_SPACE(token[4]))
            {
                token += 5;
                ParseTextureNameAndOption(&(material.displacement_texname),
                                          &(material.displacement_texopt), token);
                continue;
            }

            // 反射度贴图
            if ((0 == strncmp(token, "refl", 4)) && IS_SPACE(token[4]))
            {
                token += 5;
                ParseTextureNameAndOption(&(material.reflection_texname),
                                          &(material.reflection_texopt), token);
                continue;
            }

            // PBR: 粗糙度贴图
            if ((0 == strncmp(token, "map_Pr", 6)) && IS_SPACE(token[6]))
            {
                token += 7;
                ParseTextureNameAndOption(&(material.roughness_texname),
                                          &(material.roughness_texopt), token);
                continue;
            }

            // PBR: 金属度贴图
            if ((0 == strncmp(token, "map_Pm", 6)) && IS_SPACE(token[6]))
            {
                token += 7;
                ParseTextureNameAndOption(&(material.metallic_texname),
                                          &(material.metallic_texopt), token);
                continue;
            }

            // PBR: sheen 贴图
            if ((0 == strncmp(token, "map_Ps", 6)) && IS_SPACE(token[6]))
            {
                token += 7;
                ParseTextureNameAndOption(&(material.sheen_texname),
                                          &(material.sheen_texopt), token);
                continue;
            }

            // PBR: 自发光贴图
            if ((0 == strncmp(token, "map_Ke", 6)) && IS_SPACE(token[6]))
            {
                token += 7;
                ParseTextureNameAndOption(&(material.emissive_texname),
                                          &(material.emissive_texopt), token);
                continue;
            }

            // PBR: 法线贴图
            if ((0 == strncmp(token, "norm", 4)) && IS_SPACE(token[4]))
            {
                token += 5;
                ParseTextureNameAndOption(&(material.normal_texname),
                                          &(material.normal_texopt), token);
                continue;
            }

            // 未知参数
            const char *_space = strchr(token, ' ');
            if (!_space)
            {
                _space = strchr(token, '\t');
            }
            if (_space)
            {
                std::ptrdiff_t len = _space - token;
                std::string key(token, static_cast<size_t>(len));
                std::string value = _space + 1;
                material.unknown_parameter.insert(std::pair<std::string, std::string>(key, value));
            }

        }
        // 加入最后一个材质
        matMap->insert(std::pair<std::string, int>(
                material.name, static_cast<int>(materials->size())));
        materials->push_back(material);
        return true;
    }
    else
    {
        if (warn)
            (*warn) += "未能找到材质文件 [ " + matId + " ] !\n";
        return false;
    }

}

bool LoadObj(const std::string &filename, Model *model, std::string *err, std::string *warn, bool colorFallback = true)
{
    std::ifstream fileStream(filename);
    std::stringstream errss;
    if (!fileStream)
    {
        errss << "Cannot open file [" << filename << "]\n";
        if (err)
        {
            (*err) = errss.str();
        }
        return false;
    }
    std::string name;
    primitive_group_t primitiveGroup;
    std::vector<shape_t> shapes;
    std::vector<material_t> materials;
    // 顶点属性
    std::vector<float> v;
    std::vector<float> vn;
    std::vector<float> vt;
    std::vector<float> vc;
    // 材质相关
    std::set<std::string> materialFilenames;
    std::map<std::string, int> materialMap;
    int material = -1;
    // 平滑着色组 0代表没有
    unsigned int current_smoothing_id = 0;

    shape_t shape;
    // 用于从流中读取一行string的缓冲区
    size_t lineNum = 0;
    std::string lineBuffer;

    //f中最大的v、vt、vn序号，用于判断是否有超出范围的序号
    int vMaxIndex = -1, vtMaxIndex = -1, vnMaxIndex = -1;
    while (fileStream.peek() != -1)
    {
        safeGetLine(lineBuffer, fileStream);
        lineNum++;
        // 去除这一行末尾的回车与换行符
        if (!lineBuffer.empty() && LAST_OF(lineBuffer) == '\n')
            lineBuffer.erase(lineBuffer.size() - 1);
        if (!lineBuffer.empty() && LAST_OF(lineBuffer) == '\n')
            lineBuffer.erase(lineBuffer.size() - 1);
        // 若去除之后该行为空则读取下一行
        if (lineBuffer.empty())
            continue;
        // 去除行首的空格等
        const char *token = lineBuffer.c_str();
        token += strspn(token, " \t");
        assert(token);
        // 去除空行和注释
        if (token[0] == '\0' || token[0] == '#')continue;
        // 顶点
        if (token[0] == 'v' && IS_SPACE(token[1]))
        {
            token += 2;
            float x, y, z, r, g, b;
            bool hasR, hasG, hasB;
            x = parseFloat(&token, 0.);
            y = parseFloat(&token, 0.);
            z = parseFloat(&token, 0.);

            int vertexDataNum = 0;
            hasR = parseFloat(&token, &r);
            // 没有r则只有xyz，给一个fallback颜色111
            if (!hasR)
            {
                r = g = b = 1.0;
                vertexDataNum = 3;
                goto vertexParseEnd;
            }
            hasG = parseFloat(&token, &g);
            // 当做x y z w处理
            if (!hasG)
            {
                g = b = 1.0;
                vertexDataNum = 4;
                goto vertexParseEnd;
            }
            hasB = parseFloat(&token, &b);
            // 若有rg没有b则当做只有xyz处理
            if (!hasB)
            {
                r = g = b = 1.0;
                vertexDataNum = 3;
                goto vertexParseEnd;
            }
            // 若xyz rgb都读取到了
            vertexDataNum = 6;
            // 若没有读取到颜色值则直接goto来减少不必要的性能消耗
            vertexParseEnd:
            v.push_back(x);
            v.push_back(y);
            v.push_back(z);
            //如果rgb都有或者开启了color_fallback选项则向vc里面添加数据
            if (vertexDataNum == 6 || colorFallback)
            {
                vc.push_back(r);
                vc.push_back(g);
                vc.push_back(b);
            }
            continue;
        }
        // 法线
        if (token[0] == 'v' && token[1] == 'n' && IS_SPACE(token[2]))
        {
            token += 3;
            float x, y, z;
            x = parseFloat(&token, 0.0);
            y = parseFloat(&token, 0.0);
            z = parseFloat(&token, 0.0);
            vn.push_back(x);
            vn.push_back(y);
            vn.push_back(z);
            continue;

        }
        // UV
        if (token[0] == 'v' && token[1] == 't' && IS_SPACE(token[2]))
        {
            token += 3;
            float x, y;
            x = parseFloat(&token, 0.0);
            y = parseFloat(&token, 0.0);
            vt.push_back(x);
            vt.push_back(y);
            continue;
        }
        warning_context context;
        // 面
        if (token[0] == 'f' && IS_SPACE(token[1]))
        {
            token += 2;
            token += strspn(token, " \t");
            face_t face;
            face.smoothingGroupId = current_smoothing_id;
            face.vertexIndices.reserve(3);
            // 循环读取各个顶点中的v、vn、vt数据
            while (!IS_NEW_LINE(token[0]))
            {
                vertex_index_t vi;
                // 读取出错处理
                if (!parseTriple(&token, static_cast<int>(v.size() / 3),
                                 static_cast<int>(vn.size() / 3),
                                 static_cast<int>(vt.size() / 2), &vi, context))
                {
                    if (err)
                    {
                        (*err) += "读取第 " + toString(lineNum) + "行的f数据失败\n ";
                    }
                    return false;
                }
                face.vertexIndices.push_back(vi);
                token += strspn(token, " \t\r");
                vMaxIndex = std::max(vMaxIndex, vi.vIndex);
                vtMaxIndex = std::max(vtMaxIndex, vi.vtIndex);
                vnMaxIndex = std::max(vnMaxIndex, vi.vnIndex);
            }

            primitiveGroup.faces.push_back(face);
            continue;
        }
        // 模型
        if (token[0] == 'o' && IS_SPACE(token[1]))
        {
            bool ret = exportGroupsToShape(&shape, primitiveGroup, name, v, material, warn);
            if (shape.mesh.indices.size() > 0)
            {
                shapes.push_back(shape);
            }
            primitiveGroup.faces.clear();//TODO:如果增加点和线的读取的话要替换这里的clear
            shape = shape_t();
            token += 2;
            std::stringstream ss;
            ss << token;
            name = ss.str();

            continue;
        }
        if (token[0] == 'g' && IS_SPACE(token[1]))
        {
            bool ret = exportGroupsToShape(&shape, primitiveGroup, name, v, material, warn);
            (void) ret;
            if (shape.mesh.indices.size() > 0)
            {
                shapes.push_back(shape);
            }
            shape = shape_t();

            primitiveGroup.faces.clear();
            token += 2;
            std::stringstream ss;
            ss << token;
            name = ss.str();

            continue;
        }
        // 平滑着色组
        if (token[0] == 's' && IS_SPACE(token[1]))
        {
            // smoothing group id
            token += 2;

            // skip space.
            token += strspn(token, " \t");  // skip space

            if (token[0] == '\0')
            {
                continue;
            }

            if (token[0] == '\r' || token[1] == '\n')
            {
                continue;
            }

            if (strlen(token) >= 3 && token[0] == 'o' && token[1] == 'f' &&
                token[2] == 'f')
            {
                current_smoothing_id = 0;
            }
            else
            {
                // assume number
                int smGroupId = parseInt(&token);
                if (smGroupId < 0)
                {
                    // parse error. force set to 0.
                    // FIXME(syoyo): Report warning.
                    current_smoothing_id = 0;
                }
                else
                {
                    current_smoothing_id = static_cast<unsigned int>(smGroupId);
                }
            }

            continue;
        }  // smoothing group id
        // 使用材质
        if (strncmp(token, "usemtl", 6) == 0)
        {
            token += 6;
            token += strspn(token, " \t");
            // 获取名字
            std::string materialName = std::string(token, &token[strcspn(token, " \t\r")]);
            auto it = materialMap.find(materialName);
            int newMaterialId = -1;
            if (it != materialMap.end())
            {
                newMaterialId = it->second;
            }
            else
            {
                if (warn)
                    (*warn) += "未能找到材质 [ " + materialName + " ] \n";
            }
            if (newMaterialId != material)
            {
                // 材质是针对每个面的，所以这里替换了材质之后我们不需要创建一个新的shape而是将之前读取到的材质id和顶点数据先加入当前
                // shape，然后清空当前的顶点数据即可
                exportGroupsToShape(&shape, primitiveGroup, name, v, material, warn);
                primitiveGroup.faces.clear();
                material = newMaterialId;
            }
            continue;

        }
        // 加载材质
        if (strncmp(token, "mtllib", 6) == 0 && IS_SPACE(token[6]))
        {
            token += 7;
            std::vector<std::string> materialNames;
            SplitString(std::string(token), ' ', '\\', materialNames);
            if (materialNames.empty())
            {
                if (warn)
                    (*warn) += "不得加载空名称材质 (line " + toString(lineNum) + "\n";
            }

            else
            {
                for (const auto &materialName: materialNames)
                {
                    if (materialFilenames.count(materialName) > 0)
                    {
                        continue;
                    }
                    std::string loadMtlWarn;
                    std::string loadMtlErr;
                    bool loadSuccess = LoadMtl(materialName, model->mtlBaseDir, &materials, &materialMap, warn, err);
                    if (loadSuccess)
                    {
                        materialFilenames.insert(materialName);
                        continue;//continue or break?
                    }
                    else
                    {
                        if (warn)
                            (*warn) += "加载材质:" + materialName + "失败！\n";
                    }
                }
            }
        }

    }
    // 判断是否有序号超出范围了，除以三是因为一个顶点的xyz数据在vector中占据了三个位置
    if (vMaxIndex >= static_cast<int>(v.size() / 3))
    {
        if (warn)
            (*warn) +=
                    "v的序号超出范围，读取v数量:" + toString(v.size() / 3) + "，f中最大序号:" + toString(vMaxIndex) +
                    ".\n";
    }
    if (vnMaxIndex >= static_cast<int>(vn.size() / 3))
    {
        if (warn)
            (*warn) += "vn的序号超出范围，读取vn数量:" + toString(vn.size() / 3) + "，f中最大序号:" +
                       toString(vnMaxIndex) +
                       ".\n";
    }
    if (vtMaxIndex >= static_cast<int>(vt.size() / 2))
    {
        if (warn)
            (*warn) += "vt的序号超出范围，读取vt数量:" + toString(vt.size() / 2) + "，f中最大序号:" +
                       toString(vtMaxIndex) +
                       ".\n";
    }
    // 将primitiveGroup中的数据打包进shape里
    bool ret = exportGroupsToShape(&shape, primitiveGroup, name, v, material, warn);
    if (ret)
    {
        shapes.push_back(shape);
    }
    // 向模型类中打包数据
    model->vertexAttribute.vertices.swap(v);
    model->vertexAttribute.normals.swap(vn);
    model->vertexAttribute.texcoords.swap(vt);
    model->vertexAttribute.colors.swap(vc);
    model->shapes.swap(shapes);
    model->materials.swap(materials);
    model->num_vertices = model->vertexAttribute.vertices.size() / 3;
    model->num_shapes = model->shapes.size();
    return true;
}


template<typename T>
static int pnpoly(int nvert, T *vertx, T *verty, T testx, T testy)
{
    int i, j, c = 0;
    for (i = 0, j = nvert - 1; i < nvert; j = i++)
    {
        if (((verty[i] > testy) != (verty[j] > testy)) &&
            (testx <
             (vertx[j] - vertx[i]) * (testy - verty[i]) / (verty[j] - verty[i]) +
             vertx[i]))
            c = !c;
    }
    return c;
}
static bool exportGroupsToShape(shape_t *shape, const primitive_group_t &primitiveGroup, const std::string &name,
                                const std::vector<float> &v, const int materialId, std::string *warn)
{
    if (primitiveGroup.faces.empty())
    {
        return false;
    }
    shape->name = name;
    for (const auto &face: primitiveGroup.faces)
    {
        // 若某个面片的顶点数量小于3则说明不完整，需要跳过
        int faceVertexNum = face.vertexIndices.size();
        if (faceVertexNum < 3)
        {
            if (warn)
                (*warn) += "存在不完整的三角形面!\n";
            continue;
        }
        //先只考虑三角形情况
        if (faceVertexNum == 4)
        {
            vertex_index_t i0 = face.vertexIndices[0];
            vertex_index_t i1 = face.vertexIndices[1];
            vertex_index_t i2 = face.vertexIndices[2];
            vertex_index_t i3 = face.vertexIndices[3];

            size_t vi0 = size_t(i0.vIndex);
            size_t vi1 = size_t(i1.vIndex);
            size_t vi2 = size_t(i2.vIndex);
            size_t vi3 = size_t(i3.vIndex);

            // 三角形序号超出范围
            if (((3 * vi0 + 2) >= v.size()) || ((3 * vi1 + 2) >= v.size()) ||
                ((3 * vi2 + 2) >= v.size()) || ((3 * vi3 + 2) >= v.size()))
            {
                // FIXME(syoyo): Is it ok to simply skip this invalid triangle?
                if (warn)
                {
                    (*warn) += "面片顶点序号超出范围!\n";
                }
                continue;
            }
            float v0x = v[vi0 * 3 + 0], v0y = v[vi0 * 3 + 1], v0z = v[vi0 * 3 + 2];
            float v1x = v[vi1 * 3 + 0], v1y = v[vi1 * 3 + 1], v1z = v[vi1 * 3 + 2];
            float v2x = v[vi2 * 3 + 0], v2y = v[vi2 * 3 + 1], v2z = v[vi2 * 3 + 2];
            float v3x = v[vi3 * 3 + 0], v3y = v[vi3 * 3 + 1], v3z = v[vi3 * 3 + 2];

            float e02x = v2x - v0x, e02y = v2y - v0y, e02z = v2z - v0z;
            float e13x = v3x - v1x, e13y = v3y - v1y, e13z = v3z - v1z;

            float sqr02 = e02x * e02x + e02y * e02y + e02z * e02z;
            float sqr13 = e13x * e13x + e13y * e13y + e13z * e13z;

            index_t idx0, idx1, idx2, idx3;

            idx0.vertex_index = i0.vIndex;
            idx0.normal_index = i0.vnIndex;
            idx0.texcoord_index = i0.vtIndex;
            idx1.vertex_index = i1.vIndex;
            idx1.normal_index = i1.vnIndex;
            idx1.texcoord_index = i1.vtIndex;
            idx2.vertex_index = i2.vIndex;
            idx2.normal_index = i2.vnIndex;
            idx2.texcoord_index = i2.vtIndex;
            idx3.vertex_index = i3.vIndex;
            idx3.normal_index = i3.vnIndex;
            idx3.texcoord_index = i3.vtIndex;

            // 选择斜边短的一组三角形
            //
            // +---+
            // |\  |
            // | \ |
            // |  \|
            // +---+
            //
            // +---+
            // |  /|
            // | / |
            // |/  |
            // +---+
            if (sqr02 < sqr13)
            {
                // [0, 1, 2], [0, 2, 3]
                shape->mesh.indices.push_back(idx0);
                shape->mesh.indices.push_back(idx1);
                shape->mesh.indices.push_back(idx2);

                shape->mesh.indices.push_back(idx0);
                shape->mesh.indices.push_back(idx2);
                shape->mesh.indices.push_back(idx3);
            }
            else
            {
                // [0, 1, 3], [1, 2, 3]
                shape->mesh.indices.push_back(idx0);
                shape->mesh.indices.push_back(idx1);
                shape->mesh.indices.push_back(idx3);

                shape->mesh.indices.push_back(idx1);
                shape->mesh.indices.push_back(idx2);
                shape->mesh.indices.push_back(idx3);
            }
            // 变成两个三角形
            shape->mesh.num_face_vertices.push_back(3);
            shape->mesh.num_face_vertices.push_back(3);
            shape->mesh.material_ids.push_back(materialId);
            shape->mesh.material_ids.push_back(materialId);
            // 平滑着色组
            shape->mesh.smoothingGroupId.push_back(face.smoothingGroupId);
            shape->mesh.smoothingGroupId.push_back(face.smoothingGroupId);
        }
        else if (faceVertexNum == 3)
        {
            for (size_t k = 0; k < faceVertexNum; k++)
            {
                index_t index;
                index.vertex_index = face.vertexIndices[k].vIndex;
                index.normal_index = face.vertexIndices[k].vnIndex;
                index.texcoord_index = face.vertexIndices[k].vtIndex;
                shape->mesh.indices.push_back(index);
            }
            shape->mesh.num_face_vertices.push_back(3);
            shape->mesh.material_ids.push_back(materialId);
            shape->mesh.smoothingGroupId.push_back(face.smoothingGroupId);
        }
            //n边形
        else
        {
            vertex_index_t i0 = face.vertexIndices[0];
            vertex_index_t i1(-1);
            vertex_index_t i2 = face.vertexIndices[1];

            // find the two axes to work in
            size_t axes[2] = {1, 2};
            for (size_t k = 0; k < faceVertexNum; ++k)
            {
                i0 = face.vertexIndices[(k + 0) % faceVertexNum];
                i1 = face.vertexIndices[(k + 1) % faceVertexNum];
                i2 = face.vertexIndices[(k + 2) % faceVertexNum];
                size_t vi0 = size_t(i0.vIndex);
                size_t vi1 = size_t(i1.vIndex);
                size_t vi2 = size_t(i2.vIndex);

                if (((3 * vi0 + 2) >= v.size()) || ((3 * vi1 + 2) >= v.size()) ||
                    ((3 * vi2 + 2) >= v.size()))
                {
                    // Invalid triangle.
                    // FIXME(syoyo): Is it ok to simply skip this invalid triangle?
                    continue;
                }
                float v0x = v[vi0 * 3 + 0];
                float v0y = v[vi0 * 3 + 1];
                float v0z = v[vi0 * 3 + 2];
                float v1x = v[vi1 * 3 + 0];
                float v1y = v[vi1 * 3 + 1];
                float v1z = v[vi1 * 3 + 2];
                float v2x = v[vi2 * 3 + 0];
                float v2y = v[vi2 * 3 + 1];
                float v2z = v[vi2 * 3 + 2];
                float e0x = v1x - v0x;
                float e0y = v1y - v0y;
                float e0z = v1z - v0z;
                float e1x = v2x - v1x;
                float e1y = v2y - v1y;
                float e1z = v2z - v1z;
                float cx = std::fabs(e0y * e1z - e0z * e1y);
                float cy = std::fabs(e0z * e1x - e0x * e1z);
                float cz = std::fabs(e0x * e1y - e0y * e1x);
                const float epsilon = std::numeric_limits<float>::epsilon();
                // std::cout << "cx " << cx << ", cy " << cy << ", cz " << cz <<
                // "\n";
                if (cx > epsilon || cy > epsilon || cz > epsilon)
                {
                    // std::cout << "corner\n";
                    // found a corner
                    if (cx > cy && cx > cz)
                    {
                        // std::cout << "pattern0\n";
                    }
                    else
                    {
                        // std::cout << "axes[0] = 0\n";
                        axes[0] = 0;
                        if (cz > cx && cz > cy)
                        {
                            // std::cout << "axes[1] = 1\n";
                            axes[1] = 1;
                        }
                    }
                    break;
                }
            }

            face_t remainingFace = face;  // copy
            size_t guess_vert = 0;
            vertex_index_t ind[3];
            float vx[3];
            float vy[3];

            // How many iterations can we do without decreasing the remaining
            // vertices.
            size_t remainingIterations = face.vertexIndices.size();
            size_t previousRemainingVertices =
                    remainingFace.vertexIndices.size();

            while (remainingFace.vertexIndices.size() > 3 &&
                   remainingIterations > 0)
            {
                // std::cout << "remainingIterations " << remainingIterations <<
                // "\n";

                faceVertexNum = remainingFace.vertexIndices.size();
                if (guess_vert >= faceVertexNum)
                {
                    guess_vert -= faceVertexNum;
                }

                if (previousRemainingVertices != faceVertexNum)
                {
                    // The number of remaining vertices decreased. Reset counters.
                    previousRemainingVertices = faceVertexNum;
                    remainingIterations = faceVertexNum;
                }
                else
                {
                    // We didn't consume a vertex on previous iteration, reduce the
                    // available iterations.
                    remainingIterations--;
                }

                for (size_t k = 0; k < 3; k++)
                {
                    ind[k] = remainingFace.vertexIndices[(guess_vert + k) % faceVertexNum];
                    size_t vi = size_t(ind[k].vIndex);
                    if (((vi * 3 + axes[0]) >= v.size()) ||
                        ((vi * 3 + axes[1]) >= v.size()))
                    {
                        vx[k] = static_cast<float>(0.0);
                        vy[k] = static_cast<float>(0.0);
                    }
                    else
                    {
                        vx[k] = v[vi * 3 + axes[0]];
                        vy[k] = v[vi * 3 + axes[1]];
                    }
                }

                //
                // area is calculated per face
                //
                float e0x = vx[1] - vx[0];
                float e0y = vy[1] - vy[0];
                float e1x = vx[2] - vx[1];
                float e1y = vy[2] - vy[1];
                float cross = e0x * e1y - e0y * e1x;
                // std::cout << "axes = " << axes[0] << ", " << axes[1] << "\n";
                // std::cout << "e0x, e0y, e1x, e1y " << e0x << ", " << e0y << ", "
                // << e1x << ", " << e1y << "\n";

                float area =
                        (vx[0] * vy[1] - vy[0] * vx[1]) * static_cast<float>(0.5);
                // std::cout << "cross " << cross << ", area " << area << "\n";
                // if an internal angle
                if (cross * area < static_cast<float>(0.0))
                {
                    // std::cout << "internal \n";
                    guess_vert += 1;
                    // std::cout << "guess vert : " << guess_vert << "\n";
                    continue;
                }

                // check all other verts in case they are inside this triangle
                bool overlap = false;
                for (size_t otherVert = 3; otherVert < faceVertexNum; ++otherVert)
                {
                    size_t idx = (guess_vert + otherVert) % faceVertexNum;

                    if (idx >= remainingFace.vertexIndices.size())
                    {
                        // std::cout << "???0\n";
                        // ???
                        continue;
                    }

                    size_t ovi = size_t(remainingFace.vertexIndices[idx].vIndex);

                    if (((ovi * 3 + axes[0]) >= v.size()) ||
                        ((ovi * 3 + axes[1]) >= v.size()))
                    {
                        // std::cout << "???1\n";
                        // ???
                        continue;
                    }
                    float tx = v[ovi * 3 + axes[0]];
                    float ty = v[ovi * 3 + axes[1]];
                    if (pnpoly(3, vx, vy, tx, ty))
                    {
                        // std::cout << "overlap\n";
                        overlap = true;
                        break;
                    }
                }

                if (overlap)
                {
                    // std::cout << "overlap2\n";
                    guess_vert += 1;
                    continue;
                }

                // this triangle is an ear
                {
                    index_t idx0, idx1, idx2;
                    idx0.vertex_index = ind[0].vIndex;
                    idx0.normal_index = ind[0].vnIndex;
                    idx0.texcoord_index = ind[0].vtIndex;
                    idx1.vertex_index = ind[1].vIndex;
                    idx1.normal_index = ind[1].vnIndex;
                    idx1.texcoord_index = ind[1].vtIndex;
                    idx2.vertex_index = ind[2].vIndex;
                    idx2.normal_index = ind[2].vnIndex;
                    idx2.texcoord_index = ind[2].vtIndex;

                    shape->mesh.indices.push_back(idx0);
                    shape->mesh.indices.push_back(idx1);
                    shape->mesh.indices.push_back(idx2);

                    shape->mesh.num_face_vertices.push_back(3);
                    shape->mesh.material_ids.push_back(materialId);
                    shape->mesh.smoothingGroupId.push_back(
                            face.smoothingGroupId);
                }

                // remove v1 from the list
                size_t removed_vert_index = (guess_vert + 1) % faceVertexNum;
                while (removed_vert_index + 1 < faceVertexNum)
                {
                    remainingFace.vertexIndices[removed_vert_index] =
                            remainingFace.vertexIndices[removed_vert_index + 1];
                    removed_vert_index += 1;
                }
                remainingFace.vertexIndices.pop_back();
            }

            // std::cout << "remainingFace.vi.size = " <<
            // remainingFace.vertexIndices.size() << "\n";
            if (remainingFace.vertexIndices.size() == 3)
            {
                i0 = remainingFace.vertexIndices[0];
                i1 = remainingFace.vertexIndices[1];
                i2 = remainingFace.vertexIndices[2];
                {
                    index_t idx0, idx1, idx2;
                    idx0.vertex_index = i0.vIndex;
                    idx0.normal_index = i0.vnIndex;
                    idx0.texcoord_index = i0.vtIndex;
                    idx1.vertex_index = i1.vIndex;
                    idx1.normal_index = i1.vnIndex;
                    idx1.texcoord_index = i1.vtIndex;
                    idx2.vertex_index = i2.vIndex;
                    idx2.normal_index = i2.vnIndex;
                    idx2.texcoord_index = i2.vtIndex;

                    shape->mesh.indices.push_back(idx0);
                    shape->mesh.indices.push_back(idx1);
                    shape->mesh.indices.push_back(idx2);

                    shape->mesh.num_face_vertices.push_back(3);
                    shape->mesh.material_ids.push_back(materialId);
                    shape->mesh.smoothingGroupId.push_back(
                            face.smoothingGroupId);
                }
            }
        }  // faceVertexNum
    }
    return true;
}