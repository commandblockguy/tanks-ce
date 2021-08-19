#include "collision.h"

#include <limits.h>

#include "../level.h"
#include "../util/profiler.h"
#include "../game.h"

//Check if a point is colliding with a tile
bool check_tile_collision(uint x, uint y, bool respect_holes) {
    tile_t tile = game.tiles[COORD_TO_Y_TILE(y)][COORD_TO_X_TILE(x)];
    bool colliding = (TILE_HEIGHT(tile) && TILE_TYPE(tile) != DESTROYED) || (respect_holes && TILE_TYPE(tile) == HOLE);
    return colliding;
}


//credit: https://theshoemaker.de/2016/02/ray-casting-in-2d-grids/
//though I've rewritten a lot of it
//returns 0 if hits across x axis, non-zero if y axis
uint8_t raycast(uint startX, uint startY, angle_t angle, const tile_t tiles[][18], struct line_seg *result) {
    int dirX = fast_sec(angle);
    int dirY = fast_csc(angle);

    int8_t dirSignX = dirX >= 0 ? 1 : -1;
    int8_t dirSignY = dirY >= 0 ? 1 : -1;
    int8_t row_offset = dirY >= 0 ? LEVEL_SIZE_X : -LEVEL_SIZE_X;

    int8_t tileX = COORD_TO_X_TILE(startX);
    int8_t tileY = COORD_TO_Y_TILE(startY);
    const tile_t *tile_ptr = &tiles[tileY][tileX];
    int t = 0;

    int dtX = (TILE_TO_X_COORD(tileX + (dirX >= 0)) - startX) * dirX;
    int dtY = (TILE_TO_Y_COORD(tileY + (dirY >= 0)) - startY) * dirY;

    int dtXr = TILE_SIZE * dirSignX * dirX;
    int dtYr = TILE_SIZE * dirSignY * dirY;

    if(dirX == INT_MAX || dirX == INT_MIN) {
        dtXr = INT_MAX;
        dtX = INT_MAX;
    }

    if(dirY == INT_MAX || dirY == INT_MIN) {
        dtYr = INT_MAX;
        dtY = INT_MAX;
    }

    while(true) {
        tile_t tile = *tile_ptr;

        if(TILE_HEIGHT(tile) && TILE_TYPE(tile) != DESTROYED) {
            break;
        }

        //not entirely sure how this works
        if(dtX < dtY) {
            t += dtX;
            tile_ptr += dirSignX;
            dtY -= dtX;
            dtX = dtXr;
        } else {
            t += dtY;
            tile_ptr += row_offset;
            dtX -= dtY;
            dtY = dtYr;
        }

    }
    //store into result if it exists
    if(result != nullptr) {
        result->x1 = startX;
        result->y1 = startY;
        result->x2 = t / dirX + startX;
        result->y2 = t / dirY + startY;
    }
    if(angle == 0 || angle == 128) return AXIS_X; //return 0 if angle is horizontal - not sure why
    if(angle == 64 || angle == 192) return AXIS_Y;
    return dtX == dtXr ? AXIS_X : AXIS_Y; //if dtX == dtXr, last movement was X
}

//todo: optimize?
bool seg_collides_seg_(struct line_seg *l1, struct line_seg *l2, int *intercept_x, int *intercept_y) {
    int p0_x = l1->x1, p1_x = l1->x2, p2_x = l2->x1, p3_x = l2->x2;
    int p0_y = l1->y1, p1_y = l1->y2, p2_y = l2->y1, p3_y = l2->y2;
    int s1_x = p1_x - p0_x;
    int s1_y = p1_y - p0_y;
    int s2_x = p3_x - p2_x;
    int s2_y = p3_y - p2_y;

    int d = -s2_x * s1_y + s1_x * s2_y;
    int s = -s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y);
    int t = s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x);

    if((s >= 0) == (d >= 0) && abs(s) <= abs(d) && (t >= 0) == (d >= 0) && abs(t) <= abs(d)) {
        //  Collision detected
        if(intercept_x != nullptr)
            *intercept_x = p0_x + ((float) t * s1_x / d);
        if(intercept_y != nullptr)
            *intercept_y = p0_y + ((float) t * s1_y / d);
        return true;
    }

    return false; // No collision
}

bool seg_collides_seg(struct line_seg *l1, struct line_seg *l2) {
    return seg_collides_seg(l1, l2, nullptr, nullptr);
}

bool seg_collides_seg(struct line_seg *l1, struct line_seg *l2, int *intercept_x, int *intercept_y) {
    profiler_add(seg_collision);
    bool x = seg_collides_seg_(l1, l2, intercept_x, intercept_y);
    profiler_end(seg_collision);
    return x;
}

int y_intercept(struct line_seg *line, int x_pos) {
    return line->y1 + (line->y2 - line->y1) * (x_pos - line->x1) / (line->x2 - line->x1);
}

int x_intercept(struct line_seg *line, int y_pos) {
    return line->x1 + (line->x2 - line->x1) * (y_pos - line->y1) / (line->y2 - line->y1);
}

void process_collisions() {
    for(auto & it : PhysicsBody::objects) {
        int bottom_y = it->position_y + (int)it->height;
        for(auto other = &it + 1; other != PhysicsBody::objects.end() && (**other).position_y <= bottom_y; other++) {
            if((**other).position_x < it->position_x + (int)it->width &&
               it->position_x < (**other).position_x + (int)(**other).width) {
                it->handle_collision(*other);
            }
        }
    }
}
