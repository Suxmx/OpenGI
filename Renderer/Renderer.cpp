#include "Renderer.h"

using namespace glm;
using namespace std;

const int screenWidth = 1920, screenHeight = 1080;


vector<string> models;
unsigned int VAO, currentModel = 0;
GLuint framebuffer, textureColorbuffer, depthBuffer, imguiViewportTexture;
mat4 model = mat4(1), view = mat4(1), proj = mat4(1), mvp;
RenderPass renderPass;

GLuint getTextureRGB32F(int width, int height)
{
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return tex;
}

void SetupFramebuffer(int framebufferWidth, int framebufferHeight)
{
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    {
        // 删除纹理附件
        glDeleteTextures(1, &textureColorbuffer);

        // 删除深度缓冲区
        glDeleteRenderbuffers(1, &depthBuffer);

        // 删除帧缓冲对象
        glDeleteFramebuffers(1, &framebuffer);
    }

    // 创建帧缓冲对象
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // 创建颜色附件纹理
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, framebufferWidth, framebufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, framebufferWidth, framebufferHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    // 检查帧缓冲完整性
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
//    SetupFramebuffer(width,height);
}

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
    glViewport(0, 0, screenHeight, screenHeight);
    glEnable(GL_DEPTH_TEST);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    //Init imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
//    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

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

//void loadObj(const char *filename, vector<Mesh> &meshes)
//{
//    vector<Vertex> vertices;
//    meshes.clear();
//    curModel = Model(filename, "Models/");
//    curModel.Load(nullptr, nullptr);
//    for (const auto &shape: curModel.shapes)
//    {
//        Mesh mesh{};
//        bool needCalcNormal = false;
//        if (!shape.mesh.material_ids.empty())
//        {
//            if (shape.mesh.material_ids[0] >= 0)
//                mesh.material = curModel.materials[shape.mesh.material_ids[0]];
//        }
//        for (const auto &index: shape.mesh.indices)
//        {
//            Vertex v{};
//            // pos
//            vec3 v1, v2, v3;
//            v.position[0] = curModel.vertexAttribute.vertices[3 * index.vertex_index];
//            v.position[1] = curModel.vertexAttribute.vertices[3 * index.vertex_index + 1];
//            v.position[2] = curModel.vertexAttribute.vertices[3 * index.vertex_index + 2];
//            // normal
//            if (index.normal_index >= 0)
//            {
//                v.normal[0] = curModel.vertexAttribute.normals[3 * index.normal_index];
//                v.normal[1] = curModel.vertexAttribute.normals[3 * index.normal_index + 1];
//                v.normal[2] = curModel.vertexAttribute.normals[3 * index.normal_index + 2];
//            }
//            // uv
//            if (index.texcoord_index >= 0)
//            {
//                v.texcoord[0] = curModel.vertexAttribute.texcoords[2 * index.texcoord_index];
//                v.texcoord[1] = curModel.vertexAttribute.texcoords[2 * index.texcoord_index + 1];
//            }
//            else needCalcNormal = true;
//            mesh.indices.push_back(mesh.indices.size());
//            vertices.push_back(v);
//        }
//        //若无法向量则重新计算
//        if (needCalcNormal)
//        {
//            for (int i = 0; i < vertices.size(); i += 3)
//            {
//                if (i + 2 >= vertices.size())break;
//                vec3 v1, v2, v3;
//                v1 = vertices[i].position;
//                v2 = vertices[i + 1].position;
//                v3 = vertices[i + 2].position;
//                vertices[i].normal = vertices[i + 1].normal = vertices[i + 2].normal = calculateNormal(v1, v2, v3);
//            }
//        }
//
//        unsigned int VBOid, VAOid, EBOid;
//        glGenVertexArrays(1, &VAOid);
//        glBindVertexArray(VAOid);
//        //VAOBegin
//        glGenBuffers(1, &VBOid);
//        glBindBuffer(GL_ARRAY_BUFFER, VBOid);
//        //vertices
//        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
//        //position
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, position));
//        //normal
//        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
//        //uv
//        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texcoord));
//        glEnableVertexAttribArray(0);
//        glEnableVertexAttribArray(1);
//        glEnableVertexAttribArray(2);
//        glGenBuffers(1, &EBOid);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOid);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(int), mesh.indices.data(), GL_STATIC_DRAW);
//
//        mesh.VAO = VAOid;
//        meshes.push_back(mesh);
//        vertices.clear();
//    }
//
//}


typedef pair<int, bool> KeyPair;

void processInput(GLFWwindow *window)
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
}

ImVec2 gl_viewport_size;

bool checkViewportChange(ImVec2 viewportSize)
{
    return ((int) viewportSize.x != (int) gl_viewport_size.x || ((int) viewportSize.y != (int) gl_viewport_size.y));
}

bool firstFrame = true;

void drawImgui()
{
    firstFrame = false;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // 创建主窗口
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Main Window", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar);
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::MenuItem("Open", "Ctrl+O");
            ImGui::MenuItem("Save", "Ctrl+S");
            ImGui::MenuItem("Close", "Ctrl+W");
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    // 创建DockSpace
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    // opengl window

    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::Begin("OpenGL Viewport", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImVec2 content_min = ImGui::GetWindowContentRegionMin();
    ImVec2 content_max = ImGui::GetWindowContentRegionMax();

    auto new_gl_viewport_size = ImVec2(content_max.x - content_min.x, content_max.y - content_min.y);
    if (checkViewportChange(new_gl_viewport_size))
    {
        gl_viewport_size = new_gl_viewport_size;

        renderPass.width = (int) new_gl_viewport_size.x;
        renderPass.height = (int) new_gl_viewport_size.y;
        renderPass.colorAttachments.clear();
        renderPass.colorAttachments.push_back(getTextureRGB32F(renderPass.width, renderPass.height));
        renderPass.bindData();
    }
    ImGui::Image((void *) (intptr_t) renderPass.colorAttachments[0], new_gl_viewport_size, ImVec2(0, 1), ImVec2(1, 0));

    //opengl window end
    ImGui::End();
    ImGui::PopStyleColor(); // 恢复默认窗口背景色
    // 渲染OpenGL内容

    // main window
    ImGui::End();
    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    ImGui::Begin("Test1");
    ImGui::End();


    // 渲染ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void drawOpengl()
{
    if (firstFrame)return;

    renderPass.draw();
}

int main()
{
    GLFWwindow *window = initAll();
    if (window == nullptr)
    {
        return -1;
    }
    shader testShader("./Shaders/tvert.glsl", "./Shaders/tfrag.glsl");
    renderPass.program = testShader.ID;
    //Render Loop
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Render Command

        drawOpengl();
        drawImgui();

        //Render Command End
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}
