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
#include "physics.h"

typedef struct {
    int24_t x1;
    int24_t y1;
    int24_t x2;
    int24_t y2;
} line_seg_t;

//Determine if two bounding boxes are intersecting
bool detect_collision(physics_body_t *p1, physics_body_t *p2);

//Check if a point is inside a bounding box
bool is_point_inside_body(physics_body_t *p, int24_t x, int24_t y);

//Determine if a collision occurs with the tilemap
direction_t process_reflection(physics_body_t *p, bool respect_holes);

//if colliding, push bodies an equal distance apart and return true
bool collide_and_push(physics_body_t *p1, physics_body_t *p2);

bool center_distance_less_than(physics_body_t *p1, physics_body_t *p2, uint24_t dis);

bool raycast(uint24_t startX, uint24_t startY, angle_t angle, line_seg_t *result);

bool seg_collides_body(line_seg_t *ls, physics_body_t *phys);

bool seg_collides_seg(line_seg_t *l1, line_seg_t *l2, int24_t *intercept_x, int24_t *intercept_y);

int24_t y_intercept(line_seg_t *line, int24_t x_pos);

int24_t x_intercept(line_seg_t *line, int24_t y_pos);

#endif /* H_COLLISION */
