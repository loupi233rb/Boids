#pragma once

#include "bird.h"
#include "time.h"
#include "info.h"
#include <vector>

void render_tick();

void tick();

void ai_tick(CellGrid &cellgrid);