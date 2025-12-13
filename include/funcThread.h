#ifndef FUNCTHREAD_CPP
#define FUNCTHREAD_CPP

#include "bird.h"
#include "timer.h"
#include "crossList.h"
#include <vector>

// 优化时钟性能
void render_tick(std::vector<bird*> &birds
    , FrameRateController &Lfrc
    , const EnvSetting &eset
    , const BirdSetting &bset
    , TCHAR fpsInfo[][7]
    , bool* RUNNING);

void logic_tick(std::vector<bird*> &birds
    , FrameRateController &Lfrc
    , const EnvSetting &eset
    , const BirdSetting &bset
    , std::vector<bird_crossList> gridSet
    , bool* RUNNING);

#endif