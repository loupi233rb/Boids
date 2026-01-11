#include "info.h"
#include "bird.h"
#include "timer.h"

#include <iostream>
#include <fstream>

int screenWidth;
int screenHeight;

EnvSetting eset;
BirdSetting bset;

bool RUNNING[3];

std::vector<bird*> birds;

Renderer renderer;

GLFWwindow* window;

glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection;

GTimer Rfrc;
GTimer Lfrc;
GTimer Afrc;

CellGrid cellgrid;

// 读取配置文件
void ReadSetting(){
    std::ifstream config_file("config.json");
    nlohmann::json config = nlohmann::json::parse(config_file);
    nlohmann::json temp;
    try
    {
        temp = config.at("EnvironmentSetting");
        try
        {
            eset.MX = temp.at("window_w");
            eset.MY = temp.at("window_h");
            eset.BOUNDARY = temp.at("boundary_range");
            eset.BOUNDARY_FORCE = temp.at("boundary_force");
            eset.DT = temp.at("dt");
            eset.LOGIC_FPS = temp.at("logic_fps");
            eset.RENDER_FPS = temp.at("render_fps");
            eset.AI_FPS = temp.at("ai_fps");
            eset.BIRD_NUM = temp.at("bird_num");
            eset.LOGIC_FPS_UPDATE_SPEED = temp.at("logic_fps_update_speed");
            eset.RENDER_FPS_UPDATE_SPEED = temp.at("render_fps_update_speed");
        }
        catch(nlohmann::json::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        temp = config.at("BirdSetting");
        try
        {
            bset.s_w = temp.at("separation_weight");
            bset.a_w = temp.at("alignment_weight");
            bset.c_w = temp.at("cohesion_weight");
            bset.s_r = temp.at("separation_range");
            bset.a_r = temp.at("alignment_range");
            bset.c_r = temp.at("cohesion_range");
            bset.v_max = temp.at("v_max");
            bset.f_max = temp.at("force_max");
            bset.f_min = temp.at("force_min");
            bset.mouse_r = temp.at("mouse_range");
            bset.noise_w = temp.at("noise_weight");
        }
        catch(nlohmann::json::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
    catch(nlohmann::json::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    screenWidth = eset.MX;
    screenHeight = eset.MY;
    std::cout << "config file is loaded SUCCESSFULLY!" << std::endl;
}

static void glfw_error_callback(int error, const char* description)
{
    std::cerr << "GLFW error " << error << ": " << description << std::endl;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // keep  screen ratio
    glViewport(0, 0, width, height);
    // projection = glm::ortho(0.0f, float(width), 0.0f, float(height), -1.0f, 1.0f);
    screenWidth = width;
    screenHeight = height;
}

GLFWwindow *InitGLFW(int screenWidth, int screenHeight){
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        std::cerr << "Failed to init GLFW" << std::endl;
        return nullptr;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    glfwSwapInterval(1);

    // 回调函数部分
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glDisable(GL_DEPTH_TEST);
    glfwSwapInterval(0); // 0 = no vsync

    return window;
}

// 全局共享变量统一由info.cpp管理和初始化，仅需调用一次
void InitialSetting(){
    ReadSetting();

    window = InitGLFW(eset.MX, eset.MY);

    Lfrc = GTimer(eset.LOGIC_FPS, 5);
    Rfrc = GTimer(eset.RENDER_FPS, 5);
    Afrc = GTimer(eset.AI_FPS, 1);

    cellgrid.Initialize();

    projection = glm::ortho(0.0f, float(eset.MX), 0.0f, float(eset.MY), -1.0f, 1.0f);

    framebuffer_size_callback(window, eset.MX, eset.MY);

    RUNNING[0] = false;
    RUNNING[1] = false;
    RUNNING[2] = false;

    srand((unsigned int)time(NULL));
    // 创建鸟群
    for(int i=0;i<eset.BIRD_NUM;i++){
        int rx = rand() % eset.MX;
        int ry = rand() % eset.MY;
        float rvx = 20 - rand() % 41;
        float rvy = 20 - rand() % 41;
        vector2 p(rx,ry),v(rvx,rvy);
        bird* b;
        b = new bird(3, v, p);
        birds.push_back(b);
    }
    renderer.initShader("./../shader/boids.vs", "./../shader/boids.fs");
    renderer.initBuffers();
    renderer.shader.setMat4("projection", projection);
    renderer.shader.setMat4("view", view);
}