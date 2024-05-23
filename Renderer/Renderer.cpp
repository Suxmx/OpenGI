#include "Renderer.h"

using namespace glm;
using namespace std;

const int screenWidth = 1920, screenHeight = 1080;
RenderPipeline pipeline;
mat4 model = mat4(1), view = mat4(1), proj = mat4(1), mvp;

GLFWwindow *initAll()
{
    //Init Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "OpenGI", NULL, NULL);
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

bool checkViewportChange(ImVec2 viewportSize)
{
    return false;
//    return ((int) viewportSize.x != (int) gl_viewport_size.x || ((int) viewportSize.y != (int) gl_viewport_size.y));
}

void drawImgui()
{

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

    auto contentSize = ImVec2(content_max.x - content_min.x, content_max.y - content_min.y);

    ImGui::Image((void *) (intptr_t) pipeline.getRenderTexture(), contentSize, ImVec2(0, 1), ImVec2(1, 0));

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


void drawOpengl()
{
    pipeline.render();
//    pipeline.passes[0].draw();

}

int main()
{
    GLFWwindow *window = initAll();
    if (window == nullptr)
    {
        return -1;
    }
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    shader testShader("./Shaders/tvert.glsl", "./Shaders/tfrag.glsl");
    shader testPassShader("./Shaders/pass2v.glsl", "./Shaders/pass2f.glsl");
    pipeline.init(512, 512);
    pipeline.addRenderPass(testShader.ID);
    pipeline.addRenderPass(testPassShader.ID);
    //Render Loop
    while (!glfwWindowShouldClose(window))
    {

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
