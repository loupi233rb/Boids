#include "funcThread.h"
#include "render.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

void render_tick(FrameRateController &Rfrc)
{
    Rfrc.init();
    std::cout<<"Render thread started."<<std::endl;
    glfwMakeContextCurrent(window);
    while(RUNNING[1]){
        renderer.updateInstances(birds);
        renderer.render(window);
        for(bird* b : birds){
            b->tick_v2_0(eset, bset);
        }
        // 帧数控制器等待
        Rfrc.wait();
    }
}

void logic_tick(FrameRateController &Lfrc)
{
    Lfrc.init();
    std::cout<<"Logic thread started."<<std::endl;
    while(RUNNING[0]){

        GridUpdate(gridSet, birds, bset);

        for(bird* i:birds){
            i->update(birds, eset, bset, gridSet);
        }

        // 帧数控制器等待
        Lfrc.wait();
    }
}