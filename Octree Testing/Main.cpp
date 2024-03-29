#include "OpenGL/OpenGL.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <iostream>

#include <FastNoiseLight.h>

#include "Utils/Timer.h"
#include "Octree/Octree.h"
#include "OpenGL/Shader.h"
#include "OpenGL/Camera.h"

GLFWwindow* window;

#define PI 3.14
#define RAD(a) a * PI / 180
#define DEG(a) a * 180 / PI

#define MAX_OBJECTS 100
#define MIN_OBJECTS 1
#define MAX_CHILDREN 8
#define WORLD_SPACE_X MAX_OBJECTS / 2
#define WORLD_SPACE_Y 5
#define WORLD_SPACE_Z MAX_OBJECTS / 2

#define MAX_ITTERATIONS 0

glm::ivec2 DIMENTIONS = { 800, 800 };
constexpr glm::fvec3 BG_COLOUR = { 0.5, 0.5, 0.5 };

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float deltaTime = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

void InitOpenGL() {
    window = OpenGL::Init(DIMENTIONS, "Jimbo mc Fimbo");
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glPointSize(10);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void PreDraw() {
    deltaTime = OpenGL::GetDT();
    processInput(window);

    glClearColor(BG_COLOUR.x, BG_COLOUR.y, BG_COLOUR.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void PostDraw() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

float randRange(float min, float max) {
    return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

int main()
{
    srand(0);
    InitOpenGL();
    Shader objectShader("ShaderObj/vert.glsl", "ShaderObj/frag.glsl");
    Shader boxShader("ShaderBox/vert.glsl", "ShaderBox/frag.glsl");
    Shader rayShader("ShaderRay/vert.glsl", "ShaderRay/frag.glsl");
    Shader quadShader("ShaderQuad/vert.glsl", "ShaderQuad/frag.glsl");

    std::vector<float> vertices = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };

    vertices = {
       0.5, 0.5, 0.5,
        -0.5, 0.5, 0.5,

       0.5, 0.5, 0.5,
        0.5, 0.5, -0.5,

        0.5, 0.5, 0.5,
        0.5, -0.5, 0.5,

        -0.5, -0.5, -0.5,
        0.5, -0.5, -0.5,

        -0.5, -0.5, -0.5,
        -0.5, -0.5, 0.5,

        -0.5, -0.5, -0.5,
        -0.5, 0.5, -0.5,


        -0.5, 0.5, -0.5,
        0.5, 0.5, -0.5,

        -0.5, 0.5, -0.5,
        -0.5, 0.5, 0.5,

        0.5, -0.5, 0.5,
        -0.5, -0.5, 0.5,

        0.5, -0.5, 0.5,
        0.5, -0.5, -0.5,


        0.5, 0.5, -0.5,
        0.5, -0.5, -0.5,


        -0.5, 0.5, 0.5,
        -0.5, -0.5, 0.5
    };

    unsigned box_VAO = OpenGL::CreateBuffer(vertices, { 3 });
    vertices.clear();
    vertices = {
        -1, -1, 0,
        -1, 1, 0,
        1, 1, 0,
        -1, -1, 0,
        1, 1, 0,
        1, -1, 0
    };
    unsigned quad_VAO = OpenGL::CreateBuffer(vertices, { 3 });
    vertices.clear();

    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2);

    std::vector<glm::vec3> objects;
    objects.reserve(MAX_OBJECTS * MAX_OBJECTS);
    // generate random objects within the world space
    for (int i = 0; i < MAX_OBJECTS; i++) {
        for (int j = 0; j < MAX_OBJECTS; j++) {
            auto obj = glm::vec3(
                randRange(-WORLD_SPACE_X, WORLD_SPACE_X),
                randRange(-WORLD_SPACE_Y, WORLD_SPACE_Y),
                randRange(-WORLD_SPACE_Z, WORLD_SPACE_Z));

            obj.x = i - MAX_OBJECTS / 2 + 0.5;
            obj.y = noise.GetNoise((float) i, (float) j);
            obj.z = j - MAX_OBJECTS / 2 + 0.5;
            objects.push_back(obj);

            vertices.push_back(obj.x);
            vertices.push_back(obj.y);
            vertices.push_back(obj.z);
        }
    }


    unsigned obj_VAO = OpenGL::CreateBuffer(vertices, { 3 });

    glm::vec3 worldMax(WORLD_SPACE_X, WORLD_SPACE_Y, WORLD_SPACE_Z);
    glm::vec3 worldMin(-WORLD_SPACE_X, -WORLD_SPACE_Y, -WORLD_SPACE_Z);

    BoundingBox worldBox(worldMax, worldMin);

    Octree tree{};

    Node* root = tree.CreateTree(worldBox);

    for (auto& obj : objects) {
        root->objects.push_back(&obj);
    }
    tree.BuildTreeItterative(root);

    std::vector<Node*> over;

    for (Node& n : tree.nodes) {
        if (n.objects.size() > 1) {
            //n.box.hit = true;
            over.push_back(&n);
        }
    }

    std::cout << over.size();

    float total = 0;

    Timer timer;
    timer.Start();
    for (int i = 0; i < MAX_ITTERATIONS; i++)
    {

        tree.BuildTreeItterative(root);

        tree.Reset();
        root = tree.CreateTree(worldBox);
        for (auto& obj : objects) {
            root->objects.push_back(&obj);
        }

    }
    timer.Stop();
    total = timer.GetDuration(0);
    total /= (float)MAX_ITTERATIONS;
    // std::cout << std::to_string(total);

    Ray ray({ 20, 20, 20 }, { -2, -2, -1 });

    timer.Start("itt");
    // auto it = tree.GetIntersectionItterative(ray, root);
    //timer.Log();
    timer.Stop();

    auto bbs = tree.GetBBs();


    ComputeShader computeShader("ComputeShader.glsl", DIMENTIONS);

    unsigned tex = OpenGL::CreateTexture(DIMENTIONS);
    glBindImageTexture(0, tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    float s = rand();
    while (!glfwWindowShouldClose(window))
    {
        // std::cout << std::to_string(1.0f / deltaTime) << "\r";
        computeShader.Reload();

        PreDraw();

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)DIMENTIONS.x / (float)DIMENTIONS.y, 0.001f, 1000.0f);
        boxShader.SetMat4("projection", projection);

        glm::mat4 view = camera.GetViewMatrix();
        boxShader.SetMat4("view", view);

        objectShader.Use();
        objectShader.SetMat4("projection", projection);
        objectShader.SetMat4("view", view);

        glBindVertexArray(obj_VAO);
        for (int i = 0; i < MAX_OBJECTS * MAX_OBJECTS; i++)
        {
            glm::mat4 model(1);
            model = glm::translate(model, objects[i]);
            objectShader.SetMat4("model", model);

            glDrawArrays(GL_POINTS, 0, 1);
        }

        glBindVertexArray(box_VAO);

        rayShader.Use();
        rayShader.SetMat4("projection", projection);
        rayShader.SetMat4("view", view);

        glm::mat4 m0(1), m1(1);
        m0 = glm::translate(m0, ray.origin);
        m1 = glm::translate(m1, ray.origin + ray.dir * 100.0f);
        rayShader.SetMat4("models[0]", m0);
        rayShader.SetMat4("models[1]", m1);
        glDrawArrays(GL_LINES, 0, 2);


        boxShader.Use();
        boxShader.SetMat4("projection", projection);
        boxShader.SetMat4("view", view);

        for (int i = 0; i < bbs.size(); i++)
        {
            auto& bb = bbs[i];

            glm::mat4 model(1);
            model = glm::translate(model, bb.Center());
            model = glm::scale(model, bb.Length());
            boxShader.SetMat4("model", model);

            boxShader.SetFloat("mixValue", bb.hit ? 1.0f : 0.0f);//(float)i / (float)bbs.size()

            glDrawArrays(GL_LINES, 0, 24);
        }
        /*
        
        // compute shader
        computeShader.Use(false);
        computeShader.SetFloat("seed", s);
        auto w_p = ((float)-DIMENTIONS.x * 0.5f) * camera.Position.x + 
            ((float)DIMENTIONS.y * 0.5f) * camera.Position.y - 
            (((float)DIMENTIONS.y * 0.5f) / tanf(RAD(45.0f * 0.5f))) * camera.Position.z;
        computeShader.SetVec2("dim", { DIMENTIONS.x, DIMENTIONS.y });
        computeShader.Use();
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        // compute shader

        quadShader.Use();
        quadShader.SetInt("tex", 0);
        quadShader.SetFloat("seed", s);

        glBindVertexArray(quad_VAO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);

        glDrawArrays(GL_TRIANGLES, 0, 6);*/

        glBindVertexArray(0);

        PostDraw();
    }
    OpenGL::CleanUp();
}

float lastX = DIMENTIONS.x / 2.0f;
float lastY = DIMENTIONS.y / 2.0f;
bool firstMouse = true;

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float dt = deltaTime * 2;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::FORWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::BACKWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::LEFT, dt);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::RIGHT, dt);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::UP, dt);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::DOWN, dt);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    DIMENTIONS = { width, height };
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}