#ifndef TANKS_PROFILER_H
#define TANKS_PROFILER_H

#include <stdint.h>
#include <tice.h>
#include "util.h"

#define USE_PROFILER 1

#if USE_PROFILER

typedef union {
    struct {
        uint total;
        uint graphics;
        uint gfx_wait;
        uint tilemap;
        uint render_tanks;
        uint render_shells;
        uint render_mines;
        uint render_obscured;
        uint redraw_tile;
        uint aim_indicator;
        uint swapdraw;
        uint store_bg;
        uint undraw;
        uint physics;
        uint ai;
        uint ai_move;
        uint ai_move_random;
        uint ai_aim;
        uint ai_aim_random;
        uint ai_aim_reflect;
        uint raycast;
        uint seg_collision;
        uint tank_collision;
        uint shells;
        uint mines;
        uint input;
        uint frame_wait;
    };
    uint array[0];
} profiler_set_t;

#define NUM_PROFILER_FIELDS (sizeof(profiler_set_t) / sizeof(uint))

#define profiler_start(name) current_profiler.name = (uint)timer_2_Counter
#define profiler_add(name) current_profiler.name = (uint)timer_2_Counter - current_profiler.name
#define profiler_end(name) current_profiler.name = (uint)timer_2_Counter - current_profiler.name

void profiler_init(void);

void profiler_tick(void);

void profiler_print(void);

extern profiler_set_t current_profiler;

#else

#define profiler_start(name)
#define profiler_add(name)
#define profiler_end(name)

#define profiler_init()
#define profiler_tick()
#define profiler_print()

#endif

#endif //TANKS_PROFILER_H
