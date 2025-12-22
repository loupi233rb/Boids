#pragma once

#include "render.h"
#include <vector>
#include <cmath>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <json.hpp>

// 环境设置
struct EnvSetting{
    int MX = 1000;        // window width
    int MY = 800;        // window height
    int BOUNDARY = 15;
    int BOUNDARY_FORCE = 140;
    double DT = 0.03;
    int LOGIC_FPS = 5;
    int RENDER_FPS = 60;
    double LOGIC_FPS_UPDATE_SPEED = 0.5;
    double RENDER_FPS_UPDATE_SPEED = 0.2;
    int BIRD_NUM = 800;
    bool IS_LEFTBUTTON_DOWN = 0;
    glm::vec2 MOUSE_POSITION = glm::vec2(0.0f, 0.0f);
    EnvSetting() = default;
};

// 鸟设置
struct BirdSetting{
    float s_w = 1.0;              // separation weight
    float a_w = 0.8;              // alignment weight
    float c_w = 0.8;              // cohesion weight
    int s_r = 50;                 // separation range
    int a_r = 200;                // alignment range
    int c_r = 80;                 // cohesion range
    double v_max = 120;           // max velocity
    double f_max = 200;           // max force
    double f_min = 5;             // min force
    double mouse_r = 250;         // mouse react range
    double noise_w = 1.0;         // noise weight
    BirdSetting() = default;
};

void ReadSetting();
void InitialSetting();
static void glfw_error_callback(int error, const char* description);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
GLFWwindow *InitGLFW(int width, int height);

extern int screenWidth;
extern int screenHeight;

extern EnvSetting eset;
extern BirdSetting bset;

extern bool RUNNING[2];

extern std::vector<bird_crossList> gridSet;

extern std::vector<bird*> birds;

extern Renderer renderer;

extern GLFWwindow* window;

extern glm::mat4 view;
extern glm::mat4 projection;
