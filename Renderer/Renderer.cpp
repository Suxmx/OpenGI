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

clock_t lastT;

double fps()
{
    double t, fps, dt;
    t = clock();
    dt = (double) (t - lastT) / CLOCKS_PER_SEC;
    fps = 1.0 / dt;
    return fps;
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
    ImVec2 window_pos = ImGui::GetWindowPos();
    auto contentSize = ImVec2(content_max.x - content_min.x, content_max.y - content_min.y);

    ImGui::Image((void *) (intptr_t) pipeline.getRenderTexture(), contentSize, ImVec2(0, 1), ImVec2(1, 0));
    //显示帧率
    float frameRate = ImGui::GetIO().Framerate;
    char frameRateText[64];
    sprintf(frameRateText, "FPS: %.1f", frameRate);
    // 计算文本的位置，使其显示在图像上方
    ImVec2 text_pos = ImVec2(content_min.x + 20 + window_pos.x,
                             content_min.y + ImGui::GetTextLineHeight() + window_pos.y);


// 使用透明背景绘制文字浮在图像上方
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), frameRateText);

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


    //准备数据
    vector<triangle> triangles;
    loadObj("./Models/Stanford Bunny.obj", triangles,
            getTransformMatrix(vec3(0, 0, 0), vec3(0.3, -1.6, 0), vec3(1.5, 1.5, 1.5)),vec3(0.5,0.5,1));
    loadObj("./Models/quad.obj",triangles,getTransformMatrix(vec3(0, 0, 0), vec3(0, -1.4, 0), vec3(18.83, 0.01, 18.83)),vec3(1,1,1));
    //建立BVH
    BVHNode testNode;
    testNode.left = 255;
    testNode.right = 128;
    testNode.n = 30;
    testNode.AA = vec3(1, 1, 0);
    testNode.BB = vec3(0, 1, 0);
    vector<BVHNode> nodes{testNode};
    buildBVHwithSAH(triangles, nodes, 0, triangles.size() - 1, 8);
    vector<triangle_encoded> encode_triangles = encodeTriangles(triangles);
    vector<BVHNode_encoded> encode_bvh = encodeBVHNodes(nodes);
    //开始绑定texture buffer
    GLuint tbo0, tbo1, triangleTexBuffer, bvhTexBuffer;
    //将三角形数据传入texture buffer
    glGenBuffers(1, &tbo0);
    glBindBuffer(GL_TEXTURE_BUFFER, tbo0);
    glBufferData(GL_TEXTURE_BUFFER, encode_triangles.size() * sizeof(triangle_encoded), encode_triangles.data(),
                 GL_STATIC_DRAW);
    glGenTextures(1, &triangleTexBuffer);
    glBindTexture(GL_TEXTURE_BUFFER, triangleTexBuffer);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, tbo0);
    //将BVH数据传入texture buffer
    glGenBuffers(1, &tbo1);
    glBindBuffer(GL_TEXTURE_BUFFER, tbo1);
    glBufferData(GL_TEXTURE_BUFFER, encode_bvh.size() * sizeof(BVHNode_encoded), encode_bvh.data(), GL_STATIC_DRAW);
    glGenTextures(1, &bvhTexBuffer);
    glBindTexture(GL_TEXTURE_BUFFER, bvhTexBuffer);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, tbo1);

    //初始化渲染管线
    pipeline.init(1024, 1024);
    auto pass1 = make_shared<FirstPass>(pipeline.width, pipeline.height, testShader);
    pass1->nTriangle = encode_triangles.size();
    pass1->nBVHNode = nodes.size();
    pass1->bindTexes["triangles"] = triangleTexBuffer;
    pass1->bindTexes["bvh"] = bvhTexBuffer;
    pipeline.addRenderPass(pass1);
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
