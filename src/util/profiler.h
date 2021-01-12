#ifndef TANKS_PROFILER_H
#define TANKS_PROFILER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <tice.h>

#define USE_PROFILER 1

#if USE_PROFILER

typedef union {
    struct {
        unsigned int total;
        unsigned int graphics;
        unsigned int gfx_wait;
        unsigned int tilemap;
        unsigned int render_tanks;
        unsigned int render_pbs;
        unsigned int render_shells;
        unsigned int render_mines;
        unsigned int sprite_footprint;
        unsigned int redraw_tile;
        unsigned int aim_indicator;
        unsigned int swapdraw;
        unsigned int store_bg;
        unsigned int undraw;
        unsigned int physics;
        unsigned int ai;
        unsigned int ai_move;
        unsigned int ai_move_random;
        unsigned int ai_aim;
        unsigned int ai_aim_random;
        unsigned int ai_aim_reflect;
        unsigned int raycast;
        unsigned int seg_collision;
        unsigned int tile_collision;
        unsigned int pb_collision;
        unsigned int tanks;
        unsigned int shells;
        unsigned int mines;
        unsigned int input;
        unsigned int frame_wait;
        unsigned int temp;
        unsigned int temp2;
        unsigned int temp3;
    };
    unsigned int array[0];
} profiler_set_t;

#define NUM_PROFILER_FIELDS (sizeof(profiler_set_t) / sizeof(unsigned int))

#define profiler_start(name) current_profiler.name = (unsigned int)timer_2_Counter
#define profiler_add(name) (current_profiler.name = (unsigned int)timer_2_Counter - current_profiler.name)
#define profiler_end(name) (current_profiler.name = (unsigned int)timer_2_Counter - current_profiler.name)

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

#ifdef __cplusplus
};
#endif

#endif //TANKS_PROFILER_H
