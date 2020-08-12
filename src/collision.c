#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdlib.h>

#include "collision.h"
#include "level.h"
#include "util.h"
#include "globals.h"
#include "profiler.h"


bool detect_collision(physics_body_t *p1, physics_body_t *p2) {
    return p1->position_x < p2->position_x + p2->width && p1->position_x + p1->width > p2->position_x &&
           p1->position_y < p2->position_y + p2->height && p1->position_y + p1->height > p2->position_y;
}

bool center_distance_less_than(physics_body_t *p1, physics_body_t *p2, uint24_t dis) {
    int24_t delta_x;
    int24_t delta_y;

    if(abs((int24_t) center_x(p1) - (int24_t) center_x(p2)) > dis) return false;
    if(abs((int24_t) center_y(p1) - (int24_t) center_y(p2)) > dis) return false;

    delta_x = (center_x(p1) - center_x(p2)) >> 8;
    delta_y = (center_y(p1) - center_y(p2)) >> 8;

    return delta_x * delta_x + delta_y * delta_y < (dis >> 8) * (dis >> 8);
}

//Check if a point is colliding with a tile
bool check_tile_collision(uint24_t x, uint24_t y, bool respect_holes) {
    tile_t tile = tiles[COORD_TO_Y_TILE(y)][COORD_TO_X_TILE(x)];
    bool colliding = (TILE_HEIGHT(tile) && TILE_TYPE(tile) != DESTROYED) || (respect_holes && TILE_TYPE(tile) == HOLE);
    return colliding;
}

// todo: remove duplicate code from this and collide_and_push
direction_t process_reflection(physics_body_t *p, bool respect_holes) {
    // Figure out if the four corners are colliding
    bool top_right = check_tile_collision(p->position_x + p->width, p->position_y, respect_holes);
    bool bottom_right = check_tile_collision(p->position_x + p->width, p->position_y + p->height, respect_holes);
    bool top_left = check_tile_collision(p->position_x, p->position_y, respect_holes);
    bool bottom_left = check_tile_collision(p->position_x, p->position_y + p->height, respect_holes);

    bool double_x, double_y;

    if(p->position_y < 0) top_right = top_left = true;
    if(p->position_x < 0) top_left = bottom_left = true;
    if(p->position_x + p->width > LEVEL_SIZE_X * TILE_SIZE)
        top_right = bottom_right = true;
    if(p->position_y + p->height > LEVEL_SIZE_Y * TILE_SIZE)
        bottom_left = bottom_right = true;

    double_x = (bottom_left && top_left) || (top_right && bottom_right);
    double_y = (top_right && top_left) || (bottom_right && bottom_left);

    direction_t dir = 0;

    if((top_right || bottom_right) && (!double_y || double_x)) {
        int24_t dis_right = p->position_x + p->width - TILE_TO_X_COORD(COORD_TO_X_TILE(p->position_x + p->width));
        if(dis_right <= p->velocity_x) {
            dir |= RIGHT;
            p->position_x -= dis_right;
        }
    }
    if((top_left || bottom_left) && (!double_y || double_x)) {
        int24_t dis_left = TILE_TO_X_COORD(COORD_TO_X_TILE(p->position_x) + 1) - p->position_x;
        if(dis_left <= -p->velocity_x) {
            dir |= LEFT;
            p->position_x += dis_left;
        }
    }
    if((top_left || top_right) && (!double_x || double_y)) {
        int24_t dis_up = TILE_TO_Y_COORD(COORD_TO_Y_TILE(p->position_y) + 1) - p->position_y;
        if(dis_up <= -p->velocity_y) {
            dir |= UP;
            p->position_y += dis_up;
        }
    }
    if((bottom_left || bottom_right) && (!double_x || double_y)) {
        int24_t dis_down = p->position_y + p->height - TILE_TO_Y_COORD(COORD_TO_Y_TILE(p->position_y + p->height));
        if(dis_down <= p->velocity_y) {
            dir |= DOWN;
            p->position_y -= dis_down;
        }
    }

    return dir;
}

bool is_point_inside_body(physics_body_t *p, uint24_t x, uint24_t y) {
    return p->position_x <= x && p->position_y <= y && p->position_x + p->width >= x && p->position_y + p->height >= y;
}

bool collide_and_push(physics_body_t *p1, physics_body_t *p2) {
    //Figure out if the four corners are colliding
    bool top_right = is_point_inside_body(p2, p1->position_x + p1->width, p1->position_y);
    bool bottom_right = is_point_inside_body(p2, p1->position_x + p1->width, p1->position_y + p1->height);
    bool top_left = is_point_inside_body(p2, p1->position_x, p1->position_y);
    bool bottom_left = is_point_inside_body(p2, p1->position_x, p1->position_y + p1->height);

    uint24_t dis_up = -1;
    uint24_t dis_down = -1;
    uint24_t dis_left = -1;
    uint24_t dis_right = -1;

    if(!(top_right || bottom_right || top_left || bottom_left)) return false;

    if((top_right || bottom_right)) {
        dis_right = p1->position_x + p1->width - p2->position_x;
    }
    if((top_left || bottom_left)) {
        dis_left = p2->position_x + p2->width - p1->position_x;
    }
    if((top_left || top_right)) {
        dis_up = p2->position_y + p2->height - p1->position_y;
    }
    if((bottom_left || bottom_right)) {
        dis_down = p1->position_y + p1->height - p2->position_y;
    }

    //pick the direction with the smallest distance
    if(dis_up < dis_left && dis_up < dis_right) {
        p1->position_y += dis_up / 2;
        p2->position_y -= dis_up / 2;
    }
    if(dis_left < dis_up && dis_left < dis_down) {
        p1->position_x += dis_left / 2;
        p2->position_x -= dis_left / 2;
    }
    if(dis_down < dis_left && dis_down < dis_right) {
        p1->position_y -= dis_down / 2;
        p2->position_y += dis_down / 2;
    }
    if(dis_right < dis_up && dis_right < dis_down) {
        p1->position_x -= dis_right / 2;
        p2->position_x += dis_right / 2;
    }

    return true;
}

//todo: optimize?
bool seg_collides_seg_(line_seg_t *l1, line_seg_t *l2, int24_t *intercept_x, int24_t *intercept_y) {
    int24_t p0_x = l1->x1, p1_x = l1->x2, p2_x = l2->x1, p3_x = l2->x2;
    int24_t p0_y = l1->y1, p1_y = l1->y2, p2_y = l2->y1, p3_y = l2->y2;
    int24_t s1_x, s1_y, s2_x, s2_y;
    int24_t d, s, t;
    s1_x = p1_x - p0_x;
    s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;
    s2_y = p3_y - p2_y;

    d = -s2_x * s1_y + s1_x * s2_y;
    s = -s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y);
    t = s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x);

    if((s >= 0) == (d >= 0) && abs(s) <= abs(d) && (t >= 0) == (d >= 0) && abs(t) <= abs(d)) {
        //  Collision detected
        if(intercept_x != NULL)
            *intercept_x = p0_x + ((float) t * s1_x / d);
        if(intercept_y != NULL)
            *intercept_y = p0_y + ((float) t * s1_y / d);
        return 1;
    }

    return 0; // No collision
}

bool seg_collides_seg(line_seg_t *l1, line_seg_t *l2, int24_t *intercept_x, int24_t *intercept_y) {
    profiler_add(seg_collision);
    bool x = seg_collides_seg_(l1, l2, intercept_x, intercept_y);
    profiler_end(seg_collision);
    return x;
}

//TODO: optimize
bool seg_collides_body(line_seg_t *ls, physics_body_t *phys) {
    line_seg_t border;
    //top
    border.x1 = phys->position_x;
    border.x2 = phys->position_x + phys->width;
    border.y1 = phys->position_y;
    border.y2 = phys->position_y;
    if(seg_collides_seg(&border, ls, NULL, NULL)) return true;
    //bottom
    border.y1 += phys->height;
    border.y2 += phys->height;
    if(seg_collides_seg(&border, ls, NULL, NULL)) return true;
    //left
    border.x2 = phys->position_x;
    border.y1 = phys->position_y;
    if(seg_collides_seg(&border, ls, NULL, NULL)) return true;
    //right
    border.x1 += phys->width;
    border.x2 += phys->width;
    if(seg_collides_seg(&border, ls, NULL, NULL)) return true;
    return false;
}

int24_t y_intercept(line_seg_t *line, int24_t x_pos) {
    return line->y1 + (line->y2 - line->y1) * (x_pos - line->x1) / (line->x2 - line->x1);
}

int24_t x_intercept(line_seg_t *line, int24_t y_pos) {
    return line->x1 + (line->x2 - line->x1) * (y_pos - line->y1) / (line->y2 - line->y1);
}
