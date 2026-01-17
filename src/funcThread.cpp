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
        Rfrc.wait();
    }
}

void tick()
{
    Lfrc.init();
    Lfrc.addTask([](){
        for(bird*i:birds) {i->tick_v2_0();}
    });
    std::cout<<"Tick thread started."<<std::endl;
    while(RUNNING[0]){
        Lfrc.step();
    }
}
void ai_tick()
{
    Afrc.init();
    Afrc.addTask([](){
        cellgrid.refresh();
    });
    Afrc.addTask([](){
        for(bird* i:birds){i->update(cellgrid);}
    });
    std::cout<<"AI thread started."<<std::endl;
    while(RUNNING[2]){
        Afrc.step();
    }
}