#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>

#include <iostream>
#include <string>
#include <vector>

class OpenGL {
    inline static std::vector<unsigned> buffers;
    inline static float lastFrame;
public:
    static GLFWwindow* Init(const glm::ivec2& dimentions, const std::string& name) {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // glfw window creation
        // --------------------
        GLFWwindow* window = glfwCreateWindow(dimentions.x, dimentions.y, name.c_str(), NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }
        glfwMakeContextCurrent(window);
        // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        // glfwSetCursorPosCallback(window, mouse_callback);
        // glfwSetScrollCallback(window, scroll_callback);

        // tell GLFW to capture our mouse
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // glew: load all OpenGL function pointers
        // ---------------------------------------
        glewInit();

        // configure global opengl state
        // -----------------------------
        glEnable(GL_DEPTH_TEST);
        return window;
    }

    static void CleanUp() {
        for (int i = 0; i < buffers.size(); i += 2) {
            unsigned& b1 = buffers[i];
            unsigned& b2 = buffers[i + 1];
            glDeleteBuffers(1, &b1); // vbo
            glDeleteVertexArrays(1, &b2); // vao
        }
        glfwTerminate();
    }

    static unsigned& CreateBuffer(const std::vector<float>& data, std::vector<int> floatAttributs = {}) {
        unsigned vbo, vao;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), &data[0], GL_STATIC_DRAW);

        int size = 0;
        int i = 0;
        int jump = 0;

        for (int& atrr : floatAttributs) {
            size += atrr;
        }
        for (int& atrr : floatAttributs) {
            glVertexAttribPointer(i, atrr, GL_FLOAT, GL_FALSE, size * sizeof(float), (void*)(jump * sizeof(float)));
            glEnableVertexAttribArray(i++);
            jump += atrr;
        }

        glBindVertexArray(0);

        buffers.push_back(vbo);
        buffers.push_back(vao);
        return vao;
    }

    static float GetFPS() {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        return 1.0f / deltaTime;
    }
    static float GetDT() {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        return deltaTime;
    }
};