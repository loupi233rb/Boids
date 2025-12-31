#pragma once

#include "bird.h"
#include "timer.h"
#include "info.h"
#include <vector>

void render_tick(FrameRateController &Lfrc, CellGrid &cellgrid);

void logic_tick(FrameRateController &Lfrc, CellGrid &cellgrid);