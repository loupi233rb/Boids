#include "bird.h"
#include "timer.h"
#include "funcThread.h"
#include "crossList.h"

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include <vector>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <conio.h>
#include <fstream>
#include <thread>

#include <easyx.h>
#include <json.hpp>



// 读取配置文件
void ReadSetting(EnvSetting &eset, BirdSetting &bset){
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
    bset.shape.push_back(vector2(3,0));
    bset.shape.push_back(vector2(-2.796,-1.553));
    bset.shape.push_back(vector2(-2.796,1.553));
    std::cout << "config file is loaded SUCCESSFULLY!" << std::endl;
}


bool RUNNING[2] = {false,false};

std::vector<bird_crossList> gridSet;

vector2 shape[4] = {vector2(3,0),vector2(-2.796,-1.553),vector2(-2.796,1.553)};

TCHAR fpsInfo[2][7];


int main()
{;
    EnvSetting eset;
    BirdSetting bset;
    ReadSetting(eset, bset);


    srand((unsigned int)time(NULL));
    initgraph(eset.MX + 200, eset.MY);
    setbkcolor(DARKGRAY);
    setlinestyle(PS_SOLID);


    // 创建鸟群
    std::vector<bird*> birds;
    for(int i=0;i<eset.BIRD_NUM;i++){
        int rx = rand() % eset.MX;
        int ry = rand() % eset.MY;
        float rvx = 20 - rand() % 41;
        float rvy = 20 - rand() % 41;
        vector2 p(rx,ry),v(rvx,rvy);
        bird* b;
        b = new bird(WHITE,3, v, p);
        birds.push_back(b);
    }

    bool running = 1;
    ExMessage m;

    // 计时器
     FrameRateController Rfrc(eset.RENDER_FPS, eset.RENDER_FPS_UPDATE_SPEED);
     FrameRateController Lfrc(eset.LOGIC_FPS, eset.LOGIC_FPS_UPDATE_SPEED);
    
    gridSet = DivideGrid(eset, bset);

    std::thread logic_thread(logic_tick, std::ref(birds), std::ref(Lfrc), std::ref(eset), std::ref(bset), gridSet, RUNNING);
    std::thread render_thread(render_tick, std::ref(birds), std::ref(Rfrc), std::ref(eset), std::ref(bset), fpsInfo, RUNNING);

    logic_thread.detach();
    render_thread.detach();

    RUNNING[0] = true;
    RUNNING[1] = true;

    
    while(running){
        if(peekmessage(&m, EX_MOUSE, true)){
            eset.MOUSE_POSITION.x = m.x;
            eset.MOUSE_POSITION.y = m.y;
            if(m.message == WM_RBUTTONDOWN){
                running = 0;
                RUNNING[0] = false;
                RUNNING[1] = false;
                continue;
            }
            if(m.message == WM_LBUTTONDOWN){
                eset.IS_LEFTBUTTON_DOWN = 1;
            }
            if(m.message == WM_LBUTTONUP){
                eset.IS_LEFTBUTTON_DOWN = 0;
            } 
        }
        _stprintf_s(fpsInfo[0], 7, _T("%3.2f"), Rfrc.getFps());
        _stprintf_s(fpsInfo[1], 7, _T("%3.2f"), Lfrc.getFps());
    }

    Sleep(1000);
    closegraph();
    // timeEndPeriod(1);

    for(auto *i:birds){
        delete i;
    }
    DeleteGrids(gridSet);
    
    return 0;
}