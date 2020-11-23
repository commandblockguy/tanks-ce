#ifndef H_COLLISION
#define H_COLLISION

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

typedef struct {
    int24_t x1;
    int24_t y1;
    int24_t x2;
    int24_t y2;
} line_seg_t;

void process_collisions();

bool raycast(uint24_t startX, uint24_t startY, angle_t angle, line_seg_t *result);

bool check_tile_collision(uint24_t x, uint24_t y, bool respect_holes);

bool seg_collides_seg(line_seg_t *l1, line_seg_t *l2);
bool seg_collides_seg(line_seg_t *l1, line_seg_t *l2, int24_t *intercept_x, int24_t *intercept_y);

int24_t y_intercept(line_seg_t *line, int24_t x_pos);

int24_t x_intercept(line_seg_t *line, int24_t y_pos);

#endif /* H_COLLISION */
