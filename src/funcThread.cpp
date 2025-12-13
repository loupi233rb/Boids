#include "funcThread.h"

void render_tick(std::vector<bird*> &birds
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
    std::cout<<"Render thread started."<<std::endl;
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

void logic_tick(std::vector<bird*> &birds
    , FrameRateController &Lfrc
    , const EnvSetting &eset
    , const BirdSetting &bset
    , std::vector<bird_crossList> gridSet
    , bool* RUNNING)
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