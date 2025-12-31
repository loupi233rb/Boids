#pragma once


#include "bird.h"
#include "shader.h"
#include "shape.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class birdInstance;

class Renderer {
private:
    GLuint VAO;
    GLuint shapeVBO, shapeEBO;
    GLuint instanceVBO[2];
    std::vector<birdInstance> instances;
public:
    Shader shader;
    int curWrite, curDraw;
    Renderer(){
        curWrite = 0;
        curDraw = 1;}
    ~Renderer();
    void initShader(const char* vertexPath, const char* fragmentPath);
    void initBuffers();
    void updateInstances();

    void render(GLFWwindow *window);
};