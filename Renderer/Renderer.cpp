#include "Renderer.h"

using namespace glm;
using namespace std;

const int screenWidth = 800, screenHeight = 600;

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

vector<string> models;
unsigned int VAO, currentModel = 0;


GLFWwindow *initAll()
{
    //Init Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    //Init Glad
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }
    //Init opengl
    glViewport(0, 0, 800, 600);
    glEnable(GL_DEPTH_TEST);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    return window;
}

Model curModel("");

vec3 calculateNormal(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2)
{
    glm::vec3 e1 = v1 - v0;  // 边向量 1
    glm::vec3 e2 = v2 - v0;  // 边向量 2
    glm::vec3 normal = glm::cross(e1, e2);  // 计算叉积得到法线向量
    normal = glm::normalize(normal);  // 归一化法线向量
    return normal;
}

void loadObj(const char *filename, vector<Mesh> &meshes)
{
    vector<Vertex> vertices;
    meshes.clear();
    curModel = Model(filename, "Models/");
    curModel.Load(nullptr, nullptr);
    for (const auto &shape: curModel.shapes)
    {
        Mesh mesh{};
        bool needCalcNormal = false;
        if (!shape.mesh.material_ids.empty())
        {
            if (shape.mesh.material_ids[0] >= 0)
                mesh.material = curModel.materials[shape.mesh.material_ids[0]];
        }
        for (const auto &index: shape.mesh.indices)
        {
            Vertex v{};
            // pos
            vec3 v1, v2, v3;
            v.position[0] = curModel.vertexAttribute.vertices[3 * index.vertex_index];
            v.position[1] = curModel.vertexAttribute.vertices[3 * index.vertex_index + 1];
            v.position[2] = curModel.vertexAttribute.vertices[3 * index.vertex_index + 2];
            // normal
            if (index.normal_index >= 0)
            {
                v.normal[0] = curModel.vertexAttribute.normals[3 * index.normal_index];
                v.normal[1] = curModel.vertexAttribute.normals[3 * index.normal_index + 1];
                v.normal[2] = curModel.vertexAttribute.normals[3 * index.normal_index + 2];
            }
            // uv
            if (index.texcoord_index >= 0)
            {
                v.texcoord[0] = curModel.vertexAttribute.texcoords[2 * index.texcoord_index];
                v.texcoord[1] = curModel.vertexAttribute.texcoords[2 * index.texcoord_index + 1];
            }
            else needCalcNormal = true;
            mesh.indices.push_back(mesh.indices.size());
            vertices.push_back(v);
        }
        //若无法向量则重新计算
        if (needCalcNormal)
        {
            for (int i = 0; i < vertices.size(); i += 3)
            {
                if (i + 2 >= vertices.size())break;
                vec3 v1, v2, v3;
                v1 = vertices[i].position;
                v2 = vertices[i + 1].position;
                v3 = vertices[i + 2].position;
                vertices[i].normal = vertices[i + 1].normal = vertices[i + 2].normal = calculateNormal(v1, v2, v3);
            }
        }

        unsigned int VBOid, VAOid, EBOid;
        glGenVertexArrays(1, &VAOid);
        glBindVertexArray(VAOid);
        //VAOBegin
        glGenBuffers(1, &VBOid);
        glBindBuffer(GL_ARRAY_BUFFER, VBOid);
        //vertices
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
        //position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, position));
        //normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
        //uv
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texcoord));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glGenBuffers(1, &EBOid);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOid);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(int), mesh.indices.data(), GL_STATIC_DRAW);

        mesh.VAO = VAOid;
        meshes.push_back(mesh);
        vertices.clear();
    }

}


typedef pair<int, bool> KeyPair;

void processInput(GLFWwindow *window, vector<Mesh> &meshes)
{
    static map<int, bool> keyStayMap{KeyPair(GLFW_KEY_ESCAPE, false), KeyPair(GLFW_KEY_ENTER, false)};
    map<int, bool> keyDownMap;
    map<int, bool> keyUpMap;
    for (auto &p: keyStayMap)
    {
        int state = glfwGetKey(window, p.first);
        keyDownMap[p.first] = state == GLFW_PRESS && !keyStayMap[p.first];
        keyUpMap[p.first] = state == GLFW_RELEASE && keyStayMap[p.first];
        keyStayMap[p.first] = state == GLFW_PRESS;
    }

    if (keyDownMap[GLFW_KEY_ESCAPE])
    {
        glfwSetWindowShouldClose(window, true);
    }
    if (keyDownMap[GLFW_KEY_ENTER])
    {
        currentModel = currentModel + 1 >= models.size() ? 0 : currentModel + 1;
        loadObj(models[currentModel].c_str(), meshes);
    }
}

int main()
{
    GLFWwindow *window = initAll();
    if (window == nullptr)
    {
        return -1;
    }
    shader myShader("Shaders/blinn-phong_vert.glsl", "Shaders/blinn-phong_frag.glsl");
    vector<Mesh> meshes;
    // load objs
    models.push_back("Models/cornell_box.obj");
    models.push_back("Models/helmet.obj");
    models.push_back("Models/sword2.obj");
    loadObj(models[0].c_str(), meshes);
    // use shader
    myShader.use();
    // init mvp matrix
    mat4 model = mat4(1), view = mat4(1), proj = mat4(1);
    vec3 cameraPos = glm::vec3(278.0f, 273.0f, -660.0f);
    vec3 cameraTarget = glm::vec3(278.0f, 273.0f, 279.0f);
    vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    view = lookAt(cameraPos, cameraTarget, up);
    proj = perspective(radians(45.0f), (float) screenWidth / (float) screenHeight, 0.1f, 1500.0f);
    // mvp matrix
    mat4 mvp = proj * view * model;
    // normal transform matrix
    mat3 normal_mv = transpose(inverse(view * model));
    // light pos
    vec4 vs_lightPos = view * model * vec4((343.0 + 213.0) / 2, 548, (227.0 + 332.0) / 2, 1);
    vs_lightPos = vs_lightPos / vs_lightPos.w;
    // set uniforms
    myShader.setVec3("viewspace_lightPos", vs_lightPos.x, vs_lightPos.y, vs_lightPos.z);
    myShader.setMat4("mvp", mvp);
    myShader.setMat4("mv", view * model);
    myShader.setMat3("normal_mv", normal_mv);

    //Render Loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window, meshes);
        //Render Command
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (const auto &mesh: meshes)
        {
            glBindVertexArray(mesh.VAO);
            //Set uniforms
            material_t m = mesh.material;
            myShader.setVec3("material.ambient", m.ambient[0], m.ambient[1], m.ambient[2]);
            myShader.setVec3("material.diffuse", m.diffuse[0], m.diffuse[1], m.diffuse[2]);
            myShader.setVec3("material.specular", m.specular[0], m.specular[1], m.specular[2]);

            glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
        }


        //Render Command End
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}

#pragma clang diagnostic pop