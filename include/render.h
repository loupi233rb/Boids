#pragma once


#include "bird.h"
#include "shader.h"
#include "shape.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class birdInstance;

class Renderer {
private:
    GLuint VAO[2];
    GLuint shapeVBO, shapeEBO;
    GLuint instanceVBO[2];
    std::vector<birdInstance> instances;
public:
    Shader shader;
    bool start;
    int curWrite, curDraw;
    Renderer(){
        curWrite = 0;
        curDraw = 1;
        start = true;}
    ~Renderer();
    void initShader(const char* vertexPath, const char* fragmentPath);
    void initBuffers();
    void updateInstances();
    void bindDoubleBufferVBO(GLuint vao, GLuint shapeVBO, GLuint shapeEBO, GLuint instanceVBO);
    void render(GLFWwindow *window);
};

namespace BoidDebugWindow{
    void render();
}