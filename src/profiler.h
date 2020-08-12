#ifndef TANKS_PROFILER_H
#define TANKS_PROFILER_H

#include <stdint.h>
#include <tice.h>

#define USE_PROFILER 1

#if USE_PROFILER

typedef union {
    struct {
        uint24_t total;
        uint24_t graphics;
        uint24_t gfx_wait;
        uint24_t tilemap;
        uint24_t render_tanks;
        uint24_t render_shells;
        uint24_t render_mines;
        uint24_t render_obscured;
        uint24_t redraw_tile;
        uint24_t aim_indicator;
        uint24_t swapdraw;
        uint24_t store_bg;
        uint24_t undraw;
        uint24_t physics;
        uint24_t ai;
        uint24_t ai_move;
        uint24_t ai_move_random;
        uint24_t ai_aim;
        uint24_t ai_aim_random;
        uint24_t ai_aim_reflect;
        uint24_t raycast;
        uint24_t seg_collision;
        uint24_t tank_collision;
        uint24_t shells;
        uint24_t mines;
        uint24_t input;
        uint24_t frame_wait;
    };
    uint24_t array[0];
} profiler_set_t;

#define NUM_PROFILER_FIELDS (sizeof(profiler_set_t) / sizeof(uint24_t))

#define profiler_start(name) current_profiler.name = (uint24_t)timer_2_Counter
#define profiler_add(name) current_profiler.name = (uint24_t)timer_2_Counter - current_profiler.name
#define profiler_end(name) current_profiler.name = (uint24_t)timer_2_Counter - current_profiler.name

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
