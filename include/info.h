#pragma once

#include "render.h"
#include "timer.h"
#include "bird.h"
#include "camera.h"
#include "key.h"
#include "mouse.h"
#include <vector>
#include <cmath>
#include <shared_mutex>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <json.hpp>

class FrameRateController;
class GTimer;
class KeyManager;
class Mouse;
// 环境设置
struct EnvSetting{
    int MX = 1000;        // window width
    int MY = 800;        // window height
    int BOUNDARY = 15;
    int BOUNDARY_FORCE = 140;
    double DT = 0.03;
    int LOGIC_FPS = 5;
    int RENDER_FPS = 60;
    int AI_FPS = 10;
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
    float s_r = 50.0;                 // separation range
    float a_r = 200.0;                // alignment range
    float c_r = 80.0;                 // cohesion range
    float s_r_sq = 250.0;
    float a_r_sq = 40000.0;
    float c_r_sq = 6400.0;
    double v_max = 120;           // max velocity
    double f_max = 200;           // max force
    double f_min = 5;             // min force
    double mouse_r = 250;         // mouse react range
    double noise_w = 1.0;         // noise weight
    BirdSetting() = default;
};

void ReadSetting();
inline void SetMXY();
void InitialSetting();
static void glfw_error_callback(int error, const char* description);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
GLFWwindow *InitGLFW(int width, int height);


extern EnvSetting eset;
extern BirdSetting bset;

extern bool RUNNING[3];

extern std::vector<bird*> birds;

extern Renderer renderer;

extern KeyManager keyManager;
extern Mouse mouse;

extern GLFWwindow* window;
extern int windowWidth;
extern int windowHeight;

extern FrameRateController Rfrc;
extern GTimer Lfrc;
extern GTimer Afrc;

extern glm::mat4 view;
extern glm::mat4 projection;

extern CellGrid cellgrid;

extern glm::vec2 mousePosition;
extern glm::vec2 lastMousePosition;
extern Camera2D camera;
