#include "Renderer.h"


using namespace glm;
using namespace std;

const int screenWidth = 1920, screenHeight = 1080;


vector<string> models;
unsigned int VAO, currentModel = 0;
GLuint framebuffer, textureColorbuffer, depthBuffer;
mat4 model = mat4(1), view = mat4(1), proj = mat4(1), mvp;
bool setFrameBuffer = false;

void SetupFramebuffer(int framebufferWidth, int framebufferHeight)
{
    setFrameBuffer = true;
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

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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

void loadObj(const char *filename, vector<Batch> &meshes)
{
    vector<Vertex> vertices;
    meshes.clear();
    curModel = Model(filename, "Models/");
    curModel.Load(nullptr, nullptr);
    tinyobj::ObjReader rd;
    rd.ParseFromFile(filename);
    map<int, Batch> batches;
    for (const auto &shape: curModel.shapes)
    {
        bool needCalcNormal = false;
        int numIndex = 0;
        for (const auto &index: shape.mesh.indices)
        {
            int numFace = numIndex++ / 3;
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
            else needCalcNormal = true;
            // uv
            if (index.texcoord_index >= 0)
            {
                v.texcoord[0] = curModel.vertexAttribute.texcoords[2 * index.texcoord_index];
                v.texcoord[1] = curModel.vertexAttribute.texcoords[2 * index.texcoord_index + 1];
            }


            int materialId = numFace < shape.mesh.material_ids.size() ? shape.mesh.material_ids[numFace] : -1;
            if (!batches.contains(materialId))
            {
                Batch b{};
                b.materialId = materialId;
                if (materialId >= 0)
                    b.material = curModel.materials[materialId];
                batches[materialId] = b;
            }

            batches[materialId].indices.push_back(batches[materialId].v.size());
            batches[materialId].v.push_back(v);
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
    }
    //Bind
    for (auto &batch: batches)
    {
        unsigned int VBOid, VAOid, EBOid;
        glGenVertexArrays(1, &VAOid);
        glBindVertexArray(VAOid);
        //VAOBegin
        glGenBuffers(1, &VBOid);
        glBindBuffer(GL_ARRAY_BUFFER, VBOid);
        //vertices
        glBufferData(GL_ARRAY_BUFFER, batch.second.v.size() * sizeof(Vertex), batch.second.v.data(), GL_STATIC_DRAW);
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
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, batch.second.indices.size() * sizeof(int), batch.second.indices.data(),
                     GL_STATIC_DRAW);
        batch.second.VAO = VAOid;
        meshes.push_back(batch.second);
    }


}


typedef pair<int, bool> KeyPair;
vec3 cameraPos = glm::vec3(-0.5f, 0.f, 0.f);
vec3 cameraTarget = glm::vec3(0, 0, -0);


float yaww = -90.0f; // 初始值指向负Z方向
float pitchh = 0.0f;
float lastX = 400, lastY = 300; // 初始鼠标位置
bool firstMouse = true;
bool dragging = false;
vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        dragging = true;
        firstMouse = true; // 初始化鼠标位置
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        dragging = false;
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (!dragging) return;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // 注意这里是相反的，因为y坐标是从底部往顶部数的
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaww += xoffset;
    pitchh += yoffset;

    // 限制俯仰角度，避免翻转
    if (pitchh > 89.0f)
        pitchh = 89.0f;
    if (pitchh < -89.0f)
        pitchh = -89.0f;

    // 计算前向量
    glm::vec3 front;
    front.x = cos(glm::radians(yaww)) * cos(glm::radians(pitchh));
    front.y = sin(glm::radians(pitchh));
    front.z = sin(glm::radians(yaww)) * cos(glm::radians(pitchh));
    cameraFront = glm::normalize(front);
    cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
}

void processInput(GLFWwindow *window, vector<Batch> &meshes)
{
    static map<int, bool> keyStayMap{KeyPair(GLFW_KEY_ESCAPE, false), KeyPair(GLFW_KEY_ENTER, false)};
    map<int, bool> keyDownMap;
    map<int, bool> keyUpMap;
    static float lastT = 0;
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


    float speed = 0.8f;
    if (keyStayMap[GLFW_KEY_W])
    {
        cameraPos += cameraFront * speed * ((float) glfwGetTime() - lastT);
    }
    if (keyStayMap[GLFW_KEY_S])
    {
        cameraPos -= cameraFront * speed * ((float) glfwGetTime() - lastT);
    }
    if (keyStayMap[GLFW_KEY_A])
    {
        cameraPos -= cameraRight * speed * ((float) glfwGetTime() - lastT);
    }

    if (keyStayMap[GLFW_KEY_D])
    {
        cameraPos += cameraRight * speed * ((float) glfwGetTime() - lastT);
    }
    if (keyStayMap[GLFW_KEY_E])
    {
        cameraPos += cameraUp * speed * ((float) glfwGetTime() - lastT);
    }

    if (keyStayMap[GLFW_KEY_Q])
    {
        cameraPos -= cameraUp * speed * ((float) glfwGetTime() - lastT);
    }

    cameraTarget = cameraPos + cameraFront * 0.3f;
    view = lookAt(cameraPos, cameraTarget, cameraUp);
    lastT = glfwGetTime();
    mvp = proj * view * model;
}

ImVec2 gl_viewport_size;
ImVec2 gl_viewport_content_pos;

bool checkViewportChange(ImVec2 viewportSize)
{
    return ((int) viewportSize.x != (int) gl_viewport_size.x || ((int) viewportSize.y != (int) gl_viewport_size.y));
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
    ImVec2 viewport_pos = ImGui::GetWindowPos();
    ImVec2 content_min = ImGui::GetWindowContentRegionMin();
    ImVec2 content_max = ImGui::GetWindowContentRegionMax();

    auto new_gl_viewport_size = ImVec2(content_max.x - content_min.x, content_max.y - content_min.y);
    auto new_gl_viewport_content_pos = ImVec2(viewport_pos.x + content_min.x, viewport_pos.y + content_min.y);
    if (checkViewportChange(new_gl_viewport_size))
    {
        SetupFramebuffer((int) new_gl_viewport_size.x, (int) new_gl_viewport_size.y);
        gl_viewport_size = new_gl_viewport_size;
        gl_viewport_content_pos = new_gl_viewport_content_pos;
        proj = perspective(radians(45.0f), gl_viewport_size.x / std::max(0.1f, gl_viewport_size.y), 0.1f, 1500.0f);
        mvp = proj * view * model;
    }
    ImGui::Image((void *) (intptr_t) textureColorbuffer, new_gl_viewport_size, ImVec2(0, 1), ImVec2(1, 0));

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

void drawOpengl(ImVec2 position, ImVec2 size, vector<Batch> &meshes, shader myShader)
{
    glViewport(0, 0, (int) size.x, (int) size.y);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClearColor(0.f, 0.f, 0.f, 1.0f);
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
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int main()
{
    GLFWwindow *window = initAll();
    if (window == nullptr)
    {
        return -1;
    }
    shader myShader("Shaders/blinn-phong_vert.glsl", "Shaders/blinn-phong_frag.glsl");
    vector<Batch> meshes;
    // load objs
    models.push_back("Models/hm2.obj");
    models.push_back("Models/cornell_box.obj");
    models.push_back("Models/helmet.obj");
    models.push_back("Models/sword2.obj");
    loadObj(models[0].c_str(), meshes);
    // use shader
    myShader.use();
    // init mvp matrix


    vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    cameraTarget = vec3(0, 0, 0);
    view = lookAt(cameraPos, cameraTarget, up);
    model = scale(model, vec3(1, 1, 1));
//    view = mat4(1);
    proj = perspective(radians(45.0f), gl_viewport_size.x / std::max(0.1f, gl_viewport_size.y), 0.1f, 1500.0f);
    // normal transform matrix
    mat3 normal_mv = transpose(inverse(view * model));
    // light pos
    vec4 vs_lightPos = view * vec4(0, 1.f, 0, 1);
    vs_lightPos = vs_lightPos / vs_lightPos.w;
    // set uniforms
    myShader.setVec3("viewspace_lightPos", vs_lightPos.x, vs_lightPos.y, vs_lightPos.z);


    mvp = proj * view * model;
    //Render Loop
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        processInput(window, meshes);
        normal_mv = transpose(inverse(view * model));
        vs_lightPos = view * vec4(0, 1.f, 0, 1);
        vs_lightPos = vs_lightPos / vs_lightPos.w;

        myShader.setVec3("viewspace_lightPos", vs_lightPos.x, vs_lightPos.y, vs_lightPos.z);
        myShader.setMat4("mv", view * model);
        myShader.setMat3("normal_mv", normal_mv);
        myShader.setMat4("mvp", mvp);
        //Render Command

        drawOpengl(gl_viewport_content_pos, gl_viewport_size, meshes, myShader);
        drawImgui();

        //Render Command End
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}
