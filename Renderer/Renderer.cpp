#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "obj_loader.h"
#include "shader.h"

using namespace std;

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
    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    return window;
}

Model curModel("");

unsigned int loadObj(const char *filename)
{
    float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f, 0.5f, 0.0f
    };
    curModel = Model(filename);
    curModel.Load(nullptr, nullptr);
    vector<int> indices;
    for (const auto &shape: curModel.shapes)
    {
        for (const auto &index: shape.mesh.indices)
        {
            indices.push_back(index.vertex_index);
            curModel.num_indices++;
        }

    }
    unsigned int VBOid, VAOid;
    glGenVertexArrays(1, &VAOid);
    glBindVertexArray(VAOid);
    //VAOBegin
    glGenBuffers(1, &VBOid);
    glBindBuffer(GL_ARRAY_BUFFER, VBOid);
    glBufferData(GL_ARRAY_BUFFER, curModel.num_vertices * 3 * sizeof(float),
                 curModel.vertexAttribute.vertices.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    unsigned int EBOid;
    glGenBuffers(1, &EBOid);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOid);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    //VAOEnd
    glBindVertexArray(VAOid);
    return VAOid;

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
    if (keyDownMap[GLFW_KEY_ENTER])
    {
        currentModel = currentModel + 1 >= models.size() ? 0 : currentModel + 1;
        VAO = loadObj(models[currentModel].c_str());
    }
}

int main()
{
    GLFWwindow *window = initAll();
    if (window == nullptr)
    {
        return -1;
    }
    shader myShader("Shaders/learnGL1Vertex.glsl", "Shaders/learnGL1Frag.glsl");

    models.push_back("Models/helmet.obj");
    models.push_back("Models/sword2.obj");
    VAO = loadObj(models[currentModel].c_str());
    myShader.use();

    //Render Loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        //Render Command
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        float timeValue = glfwGetTime();
        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        float xPos=(sin(2*timeValue))*0.5f,yPos=cos(2*timeValue)*0.5f;
        myShader.setVec4("outColor", greenValue, greenValue, 0, 1.0f);
        myShader.setFloat("xPos",xPos);
        myShader.setFloat("yPos",yPos);
        glBindVertexArray(VAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, curModel.num_indices, GL_UNSIGNED_INT, 0);
        //Render Command End
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}
