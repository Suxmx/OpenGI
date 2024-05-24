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


bool checkViewportChange(ImVec2 viewportSize)
{
    return false;
//    return ((int) viewportSize.x != (int) gl_viewport_size.x || ((int) viewportSize.y != (int) gl_viewport_size.y));
}
clock_t lastT;
double fps()
{
    double t,fps,dt;
    t = clock();
    dt = (double)(t - lastT) / CLOCKS_PER_SEC;
    fps = 1.0 / dt;
    return fps;
}
void drawImgui()
{

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // ����������
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
    // ����DockSpace
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
    //��ʾ֡��
    float frameRate = ImGui::GetIO().Framerate;
    char frameRateText[64];
    sprintf(frameRateText, "FPS: %.1f", frameRate);
    // �����ı���λ�ã�ʹ����ʾ��ͼ���Ϸ�
    ImVec2 text_pos = ImVec2(content_min.x+20, content_min.y + ImGui::GetTextLineHeight()*3);

// ʹ��͸�������������ָ���ͼ���Ϸ�
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), frameRateText);

    //opengl window end
    ImGui::End();
    ImGui::PopStyleColor(); // �ָ�Ĭ�ϴ��ڱ���ɫ
    // ��ȾOpenGL����

    // main window
    ImGui::End();
    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    ImGui::Begin("Test1");
    ImGui::End();


    // ��ȾImGui
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


    //׼������
    vector<triangle> triangles;
    loadObj("./Models/Stanford Bunny.obj", triangles,
            getTransformMatrix(vec3(0, 0, 0), vec3(0.3, -1.6, 0), vec3(1.5, 1.5, 1.5)));
    vector<triangle_encoded> encode_triangles = encodeTriangles(triangles);
    GLuint tbo0, triangleTexBuffer;
    glGenBuffers(1, &tbo0);
    glBindBuffer(GL_TEXTURE_BUFFER, tbo0);
    glBufferData(GL_TEXTURE_BUFFER, encode_triangles.size() * sizeof(triangle_encoded), encode_triangles.data(),
                 GL_STATIC_DRAW);
    glGenTextures(1, &triangleTexBuffer);
    glBindTexture(GL_TEXTURE_BUFFER, triangleTexBuffer);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, tbo0);

    pipeline.init(512, 512);
    auto pass1 = make_shared<FirstPass>(pipeline.width, pipeline.height, testShader);
    pass1->nTriangle=encode_triangles.size();
    pass1->bindTexes["triangles"] = triangleTexBuffer;
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
