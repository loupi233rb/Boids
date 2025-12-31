#include "funcThread.h"
#include "render.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

void render_tick(FrameRateController &Rfrc, CellGrid &cellgrid)
{
    Rfrc.init();
    std::cout<<"Render thread started."<<std::endl;
    glfwMakeContextCurrent(window);
    while(RUNNING[1]){
        renderer.updateInstances(birds);
        renderer.render(window);
        // 帧数控制器等待
        Rfrc.wait();
    }
}

void logic_tick(FrameRateController &Lfrc, CellGrid &cellgrid)
{
    Lfrc.init();
    std::cout<<"Logic thread started."<<std::endl;
    while(RUNNING[0]){

        // GridUpdate(gridSet, birds, bset);
        cellgrid.refresh();
        for(bird* i:birds){
            i->tick_v2_0(eset, bset);
            i->update(cellgrid);
        }

        // 帧数控制器等待
        Lfrc.wait();
    }
}