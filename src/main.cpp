#include "bird.h"
#include "timer.h"
#include "funcThread.h"
#include "render.h"
#include "shader.h"
#include "info.h"

#include <vector>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <conio.h>
#include <thread>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <json.hpp>


void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


int main()
{
    InitialSetting();
    if(window == nullptr){
        return -1;
    }
    // 主线程释放OpenGL上下文
    glfwMakeContextCurrent(nullptr);
    
    RUNNING[0] = true;
    RUNNING[1] = true;
    RUNNING[2] = true;

    std::thread tick_thread(tick);
    std::thread render_thread(render_tick);
    std::thread ai_thread(ai_tick);

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);
        glfwPollEvents();
    }
    
    RUNNING[0] = false;
    RUNNING[1] = false;
    RUNNING[2] = false;

    tick_thread.join();
    render_thread.join();
    ai_thread.join();

    for(auto *i:birds){
        delete i;
    }
    
    return 0;
}