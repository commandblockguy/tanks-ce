#ifndef H_COLLISION
#define H_COLLISION

#include <stdbool.h>
#include <cstddef>
#include <cstdint>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include <string.h>

#include "../util/trig.h"

typedef struct {
    int x1;
    int y1;
    int x2;
    int y2;
} line_seg_t;

void process_collisions();

uint8_t raycast(uint startX, uint startY, angle_t angle, line_seg_t *result);

bool check_tile_collision(uint x, uint y, bool respect_holes);

bool seg_collides_seg(line_seg_t *l1, line_seg_t *l2);
bool seg_collides_seg(line_seg_t *l1, line_seg_t *l2, int *intercept_x, int *intercept_y);

int y_intercept(line_seg_t *line, int x_pos);

int x_intercept(line_seg_t *line, int y_pos);

#endif /* H_COLLISION */
