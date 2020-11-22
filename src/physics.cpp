#include <string.h>

#include "physics.h"
#include "level.h"

PhysicsBody *PhysicsBody::objects[MAX_OBJECTS];
uint8_t PhysicsBody::num_objects;

uint24_t PhysicsBody::center_x() const {
    return this->position_x + this->width / 2;
}

uint24_t PhysicsBody::center_y() const {
    return this->position_y + this->height / 2;
}

bool PhysicsBody::add() {
    if(num_objects == 255) return false;
    for(uint8_t i = 0; i < num_objects; i++) {
        if(this->position_y > objects[i]->position_y) {
            memmove(&objects[i + 1], &objects[i], (num_objects - i) * sizeof(void*));
            objects[i] = this;
            num_objects++;
            return true;
        }
    }
    objects[num_objects] = this;
    num_objects++;
    return true;
}

void PhysicsBody::remove() {
    for(uint8_t i = 0; i < num_objects; i++) {
        if(this == objects[i]) {
            memmove(&objects[i], &objects[i + 1], (num_objects - 1 - i) * sizeof(void*));
            num_objects--;
            break;
        }
    }
}

void PhysicsBody::sort() {
    // Wikipedia Insertion Sort
    for(uint8_t i = 1; i < num_objects; i++) {
        PhysicsBody *x = objects[i];
        int24_t y = x->position_y;
        int8_t j;
        for(j = i - 1; j >= 0 && objects[j]->position_y > y; j--) {
            objects[j + 1] = objects[j];
        }
        objects[j + 1] = x;
    }
}

bool PhysicsBody::detect_collision(PhysicsBody *other) const {
    return this->position_x < other->position_x + (int24_t)other->width &&
           this->position_x + (int24_t)this->width > other->position_x &&
           this->position_y < other->position_y + (int24_t)other->height &&
           this->position_y + (int24_t)this->height > other->position_y;
}

bool PhysicsBody::is_point_inside(int24_t x, int24_t y) const {
    return this->position_x <= x && this->position_y <= y &&
           this->position_x + (int24_t)this->width >= x &&
           this->position_y + (int24_t)this->height >= y;
}

// todo: remove duplicate code from this and collide_and_push
direction_t PhysicsBody::process_reflection() {
    // Figure out if the four corners are colliding
    bool top_right = check_tile_collision(this->position_x + this->width, this->position_y, respect_holes);
    bool bottom_right = check_tile_collision(this->position_x + this->width, this->position_y + this->height, respect_holes);
    bool top_left = check_tile_collision(this->position_x, this->position_y, respect_holes);
    bool bottom_left = check_tile_collision(this->position_x, this->position_y + this->height, respect_holes);

    bool double_x, double_y;

    double_x = (bottom_left && top_left) || (top_right && bottom_right);
    double_y = (top_right && top_left) || (bottom_right && bottom_left);

    direction_t dir = 0;

    if((top_right || bottom_right) && (!double_y || double_x)) {
        int24_t dis_right = this->position_x + this->width - TILE_TO_X_COORD(COORD_TO_X_TILE(this->position_x + this->width));
        if(dis_right <= this->velocity_x) {
            dir |= RIGHT;
            this->position_x -= dis_right;
        }
    }
    if((top_left || bottom_left) && (!double_y || double_x)) {
        int24_t dis_left = TILE_TO_X_COORD(COORD_TO_X_TILE(this->position_x) + 1) - this->position_x;
        if(dis_left <= -this->velocity_x) {
            dir |= LEFT;
            this->position_x += dis_left;
        }
    }
    if((top_left || top_right) && (!double_x || double_y)) {
        int24_t dis_up = TILE_TO_Y_COORD(COORD_TO_Y_TILE(this->position_y) + 1) - this->position_y;
        if(dis_up <= -this->velocity_y) {
            dir |= UP;
            this->position_y += dis_up;
        }
    }
    if((bottom_left || bottom_right) && (!double_x || double_y)) {
        int24_t dis_down = this->position_y + this->height - TILE_TO_Y_COORD(COORD_TO_Y_TILE(this->position_y + this->height));
        if(dis_down <= this->velocity_y) {
            dir |= DOWN;
            this->position_y -= dis_down;
        }
    }

    return dir;
}

bool PhysicsBody::center_distance_less_than(PhysicsBody *other, uint24_t dis) const {
    int24_t delta_x;
    int24_t delta_y;

    if((uint24_t)abs((int24_t) other->center_x() - (int24_t) other->center_x()) > dis) return false;
    if((uint24_t)abs((int24_t) other->center_y() - (int24_t) other->center_y()) > dis) return false;

    delta_x = (other->center_x() - other->center_x()) >> 8;
    delta_y = (other->center_y() - other->center_y()) >> 8;

    return (uint24_t)(delta_x * delta_x + delta_y * delta_y) < (dis >> 8) * (dis >> 8);
}

// todo: optimize
bool PhysicsBody::collides_line(line_seg_t *ls) const {
    line_seg_t border;
    //top
    border.x1 = this->position_x;
    border.x2 = this->position_x + this->width;
    border.y1 = this->position_y;
    border.y2 = this->position_y;
    if(seg_collides_seg(&border, ls)) return true;
    //bottom
    border.y1 += this->height;
    border.y2 += this->height;
    if(seg_collides_seg(&border, ls)) return true;
    //left
    border.x2 = this->position_x;
    border.y1 = this->position_y;
    if(seg_collides_seg(&border, ls)) return true;
    //right
    border.x1 += this->width;
    border.x2 += this->width;
    if(seg_collides_seg(&border, ls)) return true;
    return false;
}
