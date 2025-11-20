#include <easyx.h>
#include <conio.h>
#include <bird.h>
#include <vector>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <timer.cpp>
#include <thread>
#include <mmsystem.h>
#include <json.hpp>
#include <fstream>
#pragma comment(lib, "winmm.lib")

// #define SEP_WEIGHT 1.2
// #define PAR_WEIGHT 0.8
// #define AGG_WEIGHT 2.0
// #define SEP_RANGE 50
// #define PAR_RANGE 50
// #define AGG_RANGE 100
// #define MAX_SPEED 90
// #define MAX_FORCE 25
// #define MIN_FORCE 5
// #define MX 1000
// #define MY 800
// #define DT 0.02f
// #define BIRD_NUM 50
// #define LOGIC_FPS 60
// #define RENDER_FPS 90
// #define RANDOM_STEP 10

// 读取配置文件

std::ifstream config_file("config.json");
nlohmann::json config = nlohmann::json::parse(config_file);

float SEP_WEIGHT = config.value("sep_weight",1.2);
float PAR_WEIGHT = config.value("par_weight",0.8);
float AGG_WEIGHT = config.value("agg_weight",2.0);
int SEP_RANGE = config.value("sep_range",50);
int PAR_RANGE = config.value("par_range",50);
int AGG_RANGE = config.value("agg_range",100);
int MAX_SPEED = config.value("max_speed",90);
int MAX_FORCE = config.value("max_force",25);
int MIN_FORCE = config.value("min_force",5);
int MX = config.value("window_width",800);
int MY = config.value("window_height",600);
double DT = config.value("delta_t",0.02);
int BIRD_NUM = config.value("bird_num",50);
int LOGIC_FPS = config.value("logic_fps",60);
int RENDER_FPS = config.value("render_fps",120);
int RANDOM_STEP = config.value("random_step",5);
int MOUSE_RANGE = config.value("mouse_range", 200);
int CHASE_SPEED = config.value("chase_speed", 10);

bool RUNNING[2] = {false,false};

vector2 shape[4] = {vector2(3,0),vector2(-2.796,-1.553),vector2(-2.796,1.553)};

bool is_left_pressed = 0;
vector2 mouse_position;

void logic_tick(std::vector<bird*> &birds, HighPrecisionTimer &timer, int logic_rate){
    while(RUNNING[0]){    
        double start_time = timer.getElapsedMicroseconds();
        // 思考间隔
        int think_delta = 0;
        for(bird* i:birds){
            // c
            if(think_delta == 1){
                i->think(birds,SEP_RANGE,PAR_RANGE,AGG_RANGE,SEP_WEIGHT,PAR_WEIGHT,AGG_WEIGHT,MAX_FORCE,MIN_FORCE, is_left_pressed,mouse_position,MOUSE_RANGE,CHASE_SPEED);
                think_delta = 0;
            }
            // 调整DT，适应逻辑帧和图形帧速率
            i->tick(DT*120/LOGIC_FPS*60/RENDER_FPS, MAX_SPEED, MX, MY, RANDOM_STEP);
            think_delta++ ;
        }

        double oriented_time = 1000000.0 / logic_rate;
        double elapsed_time = timer.getElapsedMicroseconds() - start_time;
        double remaining_time = oriented_time - elapsed_time;
        if(remaining_time > 0){
            if(remaining_time > 1000){
                Sleep(static_cast<DWORD>((remaining_time-500)/1000));
            }
            do {
                elapsed_time = timer.getElapsedMicroseconds();
            } while(elapsed_time < oriented_time);
        }
    }
}

void render_tick(std::vector<bird*> &birds, HighPrecisionTimer &timer, int render_rate){
    while(RUNNING[1]){

        double start_time = timer.getElapsedMicroseconds();

        BeginBatchDraw();
        cleardevice();
        setbkcolor(DARKGRAY);

        for(bird* i:birds){
            i->render(shape,3);
        }
        FlushBatchDraw();

        double oriented_time = 1000000.0 / render_rate;
        double elapsed_time = timer.getElapsedMicroseconds() - start_time;
        double remaining_time = oriented_time - elapsed_time;
        if(remaining_time > 0){
            if(remaining_time > 1000){
                Sleep(static_cast<DWORD>((remaining_time-500)/1000));
            }
            do {
                elapsed_time = timer.getElapsedMicroseconds();
            } while(elapsed_time < oriented_time);
        }
    }
}


int main()
{
    srand((unsigned int)time(NULL));
    initgraph(MX, MY);
    setbkcolor(DARKGRAY);
    setlinestyle(PS_SOLID);


    // 创建鸟群
    std::vector<bird*> birds;
    for(int i=0;i<BIRD_NUM;i++){
        int rx = rand() % MX;
        int ry = rand() % MY;
        float rvx = 20 - rand() % 41;
        float rvy = 20 - rand() % 41;
        vector2 p(rx,ry),v(rvx,rvy);
        bird* b;
        b = new bird(WHITE,5, v, p);
        birds.push_back(b);
    }

    bool running = 1;
    ExMessage m;

    HighPrecisionTimer timer;

    // 提高Sleep精度
    timeBeginPeriod(1);


    std::thread logic_thread(logic_tick,std::ref(birds),std::ref(timer),LOGIC_FPS);
    std::thread render_thread(render_tick,std::ref(birds),std::ref(timer),RENDER_FPS);

    logic_thread.detach();
    render_thread.detach();

    RUNNING[0] = true;
    RUNNING[1] = true;

    
    while(running){
        if(peekmessage(&m, EX_MOUSE, true)){
            mouse_position.x = m.x;
            mouse_position.y = m.y;
            if(m.message == WM_RBUTTONDOWN){
                running = 0;
                RUNNING[0] = false;
                RUNNING[1] = false;
                continue;
            }
            if(m.message == WM_LBUTTONDOWN){
                is_left_pressed = 1;
            }
            if(m.message == WM_LBUTTONUP){
                is_left_pressed = 0;
            } 
        }
        std::cout<<is_left_pressed;

    }

    // 
    Sleep(1000);
    closegraph();
    timeEndPeriod(1);
    return 0;
}