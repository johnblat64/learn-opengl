#pragma once

#include <SDL2/SDL.h>

#define MAX_DELTA_TIME 0.0167


typedef struct TimeStep TimeStep;
struct TimeStep {
    float secondsPerFrame;
    float startTicks;
};

extern TimeStep gTimeStep;

TimeStep ts_TimeStep_init(float FPS);
void ts_TimeStep_start_ticks_set_to_current_ticks(TimeStep &ts);
void ts_TimeStep_delay_remaining_time(TimeStep &ts);