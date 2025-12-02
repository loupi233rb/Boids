#ifndef FUNCTHREAD_CPP
#define FUNCTHREAD_CPP

#include "bird.h"
#include "timer.h"
#include "crossList.h"
#include <vector>
void logic_tick_v2_0(std::vector<bird*> &birds, HighPrecisionTimer &timer, const EnvSetting &eset, const BirdSetting &bset, bool* RUNNING);

void render_tick_v2_0(std::vector<bird*> &birds, HighPrecisionTimer &timer, const EnvSetting &eset, const BirdSetting &bset, bool* RUNNING);

// 更好性能
void logic_tick_v3_0(std::vector<bird*> &birds
    , HighPrecisionTimer &timer
    , const EnvSetting &eset
    , const BirdSetting &bset
    , std::vector<CrossList<grid*>*>* gridSet
    , bool* RUNNING);

// 优化时钟性能
void render_tick_v3_0(std::vector<bird*> &birds
    , FrameRateController &Lfrc
    , const EnvSetting &eset
    , const BirdSetting &bset
    , TCHAR fpsInfo[][7]
    , bool* RUNNING);

void logic_tick_v4_0(std::vector<bird*> &birds
    , FrameRateController &Lfrc
    , const EnvSetting &eset
    , const BirdSetting &bset
    , std::vector<CrossList<grid*>*>* gridSet
    , bool* RUNNING);

#endif