#include "funcThread.h"
#include "render.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

void render_tick()
{
    Rfrc.init();
    std::cout<<"Render thread started."<<std::endl;
    glfwMakeContextCurrent(window);
    while(RUNNING[1]){
        renderer.updateInstances();
        renderer.render(window);
        // 帧数控制器等待
        Rfrc.wait();
    }
}

void tick()
{
    Lfrc.init();
    std::cout<<"Tick thread started."<<std::endl;
    while(RUNNING[0]){
        for(bird* i:birds){
            i->tick_v2_0();
        }
        Lfrc.wait();
    }
}
void ai_tick(CellGrid &cellgrid)
{
    Afrc.init();
    std::cout<<"AI thread started."<<std::endl;
    while(RUNNING[2]){
        cellgrid.refresh();
        for(bird* i:birds){
            i->update(cellgrid);
        }
        Afrc.wait();
    }
}