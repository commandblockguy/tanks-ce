#include "profiler.h"

#include <debug.h>
#include <string.h>
#include <stdint.h>

#if USE_PROFILER

union profiler_set current_profiler;
union profiler_set profiler_sum;
union profiler_set profiler_frames[256];

uint8_t profiler_frame_index;
int considered_frames = 0;

void profiler_init() {
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
    if(considered_frames < 256) {
        considered_frames++;
    }
}

#define profiler_field_last(name, depth) \
    dbg_printf("%.*s%s: %.3f ms\n",   \
               2*(1+(depth)), "                    ", \
               #name, profiler_frames[last_frame].name / (CLOCKS_PER_SEC / 1000.0));

#define profiler_field_average(name, depth) \
    dbg_printf("%.*s%s: %.3f ms\n",      \
               2*(1+(depth)), "                    ", \
               #name, profiler_sum.name / (CLOCKS_PER_SEC / 1000.0) / considered_frames);

void profiler_print() {
    uint8_t last_frame = profiler_frame_index - 1;
    dbg_printf("Last frame (%u): %u FPS\n", last_frame, (uint24_t) (CLOCKS_PER_SEC / profiler_frames[last_frame].total));
    PROFILER_ENTRIES(profiler_field_last)
    dbg_printf("Average of last %u frames: %u FPS\n", considered_frames, (uint24_t) (CLOCKS_PER_SEC * considered_frames / profiler_sum.total));
    PROFILER_ENTRIES(profiler_field_average)
}

#endif
