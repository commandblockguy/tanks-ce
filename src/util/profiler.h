#ifndef TANKS_PROFILER_H
#define TANKS_PROFILER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

#ifndef NDEBUG
#define USE_PROFILER 1
#endif

#if USE_PROFILER

#define PROFILER_ENTRIES(f) \
    f(total, 0) \
    f(  graphics, 1) \
    f(    gfx_wait, 2) \
    f(    tilemap, 2) \
    f(    render_pbs, 2) \
    f(      render_tanks, 3) \
    f(      render_shells, 3) \
    f(      render_mines, 3) \
    f(    sprite_footprint, 2) \
    f(    redraw_tile, 2) \
    f(    aim_indicator, 2) \
    f(    swapdraw, 2) \
    f(    undraw, 2) \
    f(    store_bg, 2) \
    f(  physics, 1) \
    f(    ai, 2) \
    f(      ai_move, 3) \
    f(        ai_move_random, 4) \
    f(      ai_aim, 3) \
    f(        ai_aim_random, 4) \
    f(        ai_aim_reflect, 4) \
    f(        raycast, 4) \
    f(        seg_collision, 4) \
    f(    tile_collision, 2) \
    f(    pb_collision, 2) \
    f(    tanks, 2) \
    f(    shells, 2) \
    f(    mines, 2) \
    f(  input, 1) \
    f(frame_wait, 0) \

#define PROFILER_STRUCT_MEMBER(name, depth) clock_t name;
union profiler_set {
    struct {
        PROFILER_ENTRIES(PROFILER_STRUCT_MEMBER)
    };
    clock_t array[0];
};

#define NUM_PROFILER_FIELDS (sizeof(union profiler_set) / sizeof(clock_t))

#define profiler_start(name) current_profiler.name = clock()
#define profiler_add(name) (current_profiler.name = clock() - current_profiler.name)
#define profiler_end(name) (current_profiler.name = clock() - current_profiler.name)

void profiler_init(void);

void profiler_tick(void);

void profiler_print(void);

extern union profiler_set current_profiler;

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
