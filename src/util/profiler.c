#include "profiler.h"

#include <debug.h>
#include <string.h>

#if USE_PROFILER

profiler_set_t current_profiler;
profiler_set_t profiler_sum;
profiler_set_t profiler_frames[256];

uint8_t profiler_frame_index;

void profiler_init() {
    timer_Set(2, 0);
    timer_Enable(2, TIMER_32K, TIMER_NOINT, TIMER_UP);
    profiler_frame_index = 0;
    memset(profiler_frames, 0, sizeof(profiler_frames));
    memset(&profiler_sum, 0, sizeof(profiler_sum));
}

void profiler_tick() {
    for(uint8_t i = 0; i < NUM_PROFILER_FIELDS; i++) {
        profiler_sum.array[i] -= profiler_frames[profiler_frame_index].array[i];
        profiler_sum.array[i] += current_profiler.array[i];
        profiler_frames[profiler_frame_index].array[i] = current_profiler.array[i];
        current_profiler.array[i] = 0;
    }
    profiler_frame_index++;
    timer_Set(2, 0);
}

#define as_decimal(x) (unsigned int)(x),((unsigned int)((x)*1000) % 1000)
#define profiler_field_last(name, depth) dbg_printf("%.*s%s: %u.%03u ms\n", 2*(1+(depth)), "                    ", #name, as_decimal(profiler_frames[profiler_frame_index - 1].name / 33.0))
#define profiler_field_average(name, depth) dbg_printf("%.*s%s: %u.%03u ms\n", 2*(1+(depth)), "                    ", #name, as_decimal(profiler_sum.name / 8388.608))

void profiler_print() {
    dbg_printf("Last frame (%u): %u FPS\n", profiler_frame_index - 1, 32768 / profiler_frames[profiler_frame_index - 1].total);
    profiler_field_last(total, 0);
    profiler_field_last(  graphics, 1);
    profiler_field_last(    gfx_wait, 2);
    profiler_field_last(    tilemap, 2);
    profiler_field_last(    render_tanks, 2);
    profiler_field_last(      render_shells, 3);
    profiler_field_last(      render_mines, 3);
    profiler_field_last(      render_obscured, 3);
    profiler_field_last(        redraw_tile, 4);
    profiler_field_last(    aim_indicator, 2);
    profiler_field_last(    swapdraw, 2);
    profiler_field_last(    undraw, 2);
    profiler_field_last(    store_bg, 2);
    profiler_field_last(  physics, 1);
    profiler_field_last(    ai, 2);
    profiler_field_last(      ai_move, 3);
    profiler_field_last(        ai_move_random, 4);
    profiler_field_last(      ai_aim, 3);
    profiler_field_last(        ai_aim_random, 4);
    profiler_field_last(        ai_aim_reflect, 4);
    profiler_field_last(        raycast, 4);
    profiler_field_last(        seg_collision, 4);
    profiler_field_last(    tank_collision, 2);
    profiler_field_last(    shells, 2);
    profiler_field_last(    mines, 2);
    profiler_field_last(  input, 1);
    profiler_field_last(frame_wait, 0);
    dbg_printf("Average of last 256 frames: %u FPS\n", (unsigned int)8388608 / profiler_sum.total);
    profiler_field_average(total, 0);
    profiler_field_average(  graphics, 1);
    profiler_field_average(    gfx_wait, 2);
    profiler_field_average(    tilemap, 2);
    profiler_field_average(    render_tanks, 2);
    profiler_field_average(      render_shells, 3);
    profiler_field_average(      render_mines, 3);
    profiler_field_average(      render_obscured, 3);
    profiler_field_average(        redraw_tile, 4);
    profiler_field_average(    aim_indicator, 2);
    profiler_field_average(    swapdraw, 2);
    profiler_field_average(    undraw, 2);
    profiler_field_average(    store_bg, 2);
    profiler_field_average(  physics, 1);
    profiler_field_average(    ai, 2);
    profiler_field_average(      ai_move, 3);
    profiler_field_average(        ai_move_random, 4);
    profiler_field_average(      ai_aim, 3);
    profiler_field_average(        ai_aim_random, 4);
    profiler_field_average(        ai_aim_reflect, 4);
    profiler_field_average(        raycast, 4);
    profiler_field_average(        seg_collision, 4);
    profiler_field_average(    tank_collision, 2);
    profiler_field_average(    shells, 2);
    profiler_field_average(    mines, 2);
    profiler_field_average(  input, 1);
    profiler_field_average(frame_wait, 0);
}

#endif
