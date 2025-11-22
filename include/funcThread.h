#ifndef FUNCTHREAD_CPP
#define FUNCTHREAD_CPP

#include "bird.h"
#include "timer.h"
#include <vector>
void logic_tick_v2_0(std::vector<bird*> &birds, HighPrecisionTimer &timer, const EnvSetting &eset, const BirdSetting &bset, bool* RUNNING);

void render_tick_v2_0(std::vector<bird*> &birds, HighPrecisionTimer &timer, const EnvSetting &eset, const BirdSetting &bset, bool* RUNNING);

#endif