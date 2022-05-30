#ifndef H_COLLISION
#define H_COLLISION

#include <stdbool.h>
#include <cstddef>
#include <cstdint>

#include "../util/trig.h"
#include "../level.h"

struct line_seg {
    int x1;
    int y1;
    int x2;
    int y2;
};

void process_collisions();

uint8_t raycast(uint startX, uint startY, angle_t angle, const tile_t tiles[][18], struct line_seg *result);

bool check_tile_collision(uint x, uint y, bool respect_holes);

bool seg_collides_seg(struct line_seg *l1, struct line_seg *l2);
bool seg_collides_seg(struct line_seg *l1, struct line_seg *l2, int *intercept_x, int *intercept_y);

int y_intercept(struct line_seg *line, int x_pos);

int x_intercept(struct line_seg *line, int y_pos);

#endif /* H_COLLISION */
