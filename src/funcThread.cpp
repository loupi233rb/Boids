#include "funcThread.h"


void render_tick_v2_0(std::vector<bird*> &birds, HighPrecisionTimer &timer, const EnvSetting &eset, const BirdSetting &bset, bool* RUNNING){
    while(RUNNING[1]){

        double start_time = timer.getElapsedMicroseconds();

        BeginBatchDraw();
        cleardevice();
        setbkcolor(DARKGRAY);

        for(bird* i:birds){
            i->tick_v2_0(eset, bset);
            i->render(bset.shape, bset.shape.size());
        }
        FlushBatchDraw();

        double oriented_time = 1000000.0 / eset.RENDER_FPS;
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

void logic_tick_v3_0(std::vector<bird*> &birds
    , HighPrecisionTimer &timer
    , const EnvSetting &eset
    , const BirdSetting &bset
    , std::vector<CrossList<grid*>*>* gridSet
    , bool* RUNNING)
{
    while(RUNNING[0]){
        double start_time = timer.getElapsedMicroseconds();

        GridUpdate(gridSet, birds, bset);

        for(bird* i:birds){
            i->update(birds, eset, bset, gridSet);
        }

        double oriented_time = 1000000.0 / eset.LOGIC_FPS;
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

void render_tick_v3_0(std::vector<bird*> &birds
    , FrameRateController &Rfrc
    , const EnvSetting &eset
    , const BirdSetting &bset
    , TCHAR fpsInfo[][7]
    , bool* RUNNING)
{
    TCHAR c1[] = _T("Render");
    TCHAR c2[] = _T("Logic");

    setbkmode(TRANSPARENT);
    settextcolor(RED);
    setlinecolor(BLACK);
    settextstyle(50, 0, _T("Consolas"));
    Rfrc.init();
    while(RUNNING[1]){

        BeginBatchDraw();
        cleardevice();
        setbkcolor(DARKGRAY);
        
        for(bird* i:birds){
            i->tick_v2_0(eset, bset);
            i->render(bset.shape, bset.shape.size());
        }

        line(eset.MX, 0, eset.MX, eset.MY);

        outtextxy(eset.MX + 5, 5, c1);
        outtextxy(eset.MX + 5, 5 + 50*1, fpsInfo[0]);
        outtextxy(eset.MX + 5, 5 + 50*2, c2);
        outtextxy(eset.MX + 5, 5 + 50*3, fpsInfo[1]);

        FlushBatchDraw();

        // 帧数控制器等待
        Rfrc.wait();
    }
}

void logic_tick_v4_0(std::vector<bird*> &birds
    , FrameRateController &Lfrc
    , const EnvSetting &eset
    , const BirdSetting &bset
    , std::vector<CrossList<grid*>*>* gridSet
    , bool* RUNNING)
{
    Lfrc.init();
    while(RUNNING[0]){

        GridUpdate(gridSet, birds, bset);

        for(bird* i:birds){
            i->update(birds, eset, bset, gridSet);
        }

        // 帧数控制器等待
        Lfrc.wait();
    }
}