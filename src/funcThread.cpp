#include "funcThread.h"
#include "render.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

void render_tick()
{
    Rfrc.init();
    Rfrc.addTask([](){renderer.updateInstances();});
    Rfrc.addTask([&](){renderer.render(window);});
    std::cout<<"Render thread started."<<std::endl;
    glfwMakeContextCurrent(window);
    while(RUNNING[1]){
        Rfrc.step();
    }
}

void tick()
{
    Lfrc.init();
    Lfrc.addTask([](){for(bird*i:birds) {i->tick_v2_0();}});
    Lfrc.addTask([](){cellgrid.refresh();});
    Lfrc.addTask([](){for(bird* i:birds){i->update(cellgrid);}});
    std::cout<<"Tick thread started."<<std::endl;
    while(RUNNING[0]){
        Lfrc.step();
    }
}
// void ai_tick(CellGrid &cellgrid)
// {
//     Afrc.init();
    
//     std::cout<<"AI thread started."<<std::endl;
//     while(RUNNING[2]){
//         Afrc.step();
//     }
// }