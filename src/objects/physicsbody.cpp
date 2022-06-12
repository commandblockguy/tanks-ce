#include "physicsbody.h"

#include "../level.h"
#include "../graphics/graphics.h"
#include "../graphics/partial_redraw.h"
#include "../util/profiler.h"

ezSTL::vector<PhysicsBody*> PhysicsBody::objects;

uint PhysicsBody::center_x() const {
    return position_x + width / 2;
}

uint PhysicsBody::center_y() const {
    return position_y + height / 2;
}

PhysicsBody::PhysicsBody(uint width, uint height):
    width(width),
    height(height) {
    objects.push_back(this);
}

PhysicsBody::~PhysicsBody() = default;

void PhysicsBody::sort() {
    // Wikipedia Insertion Sort
    for(size_t i = 1; i < objects.size(); i++) {
        PhysicsBody *x = objects[i];
        int y = x->position_y;
        int j;
        for(j = i - 1; j >= 0 && objects[j]->position_y > y; j--) {
            objects[j + 1] = objects[j];
        }
        objects[j + 1] = x;
    }
}

void PhysicsBody::remove_all() {
    for(auto & obj : PhysicsBody::objects) {
        obj->active = false;
    }
    PhysicsBody::remove_inactive();
}

void PhysicsBody::remove_inactive() {
    for(auto & obj : objects) {
        if(!obj->parent->active) obj->parent = nullptr;
    }
    for(auto it = objects.begin(); it != objects.end();) {
        if(!(*it)->active) {
            delete *it;
            it = objects.erase(it);
        } else it++;
    }
}

bool PhysicsBody::detect_collision(PhysicsBody *other) const {
    return position_x < other->position_x + (int)other->width &&
           position_x + (int)width > other->position_x &&
           position_y < other->position_y + (int)other->height &&
           position_y + (int)height > other->position_y;
}

bool PhysicsBody::is_point_inside(int x, int y) const {
    return position_x <= x && position_y <= y &&
           position_x + (int)width >= x &&
           position_y + (int)height >= y;
}

direction_t PhysicsBody::process_tile_collision() {
    profiler_start(tile_collision);

    // Figure out if the four corners are colliding
    bool top_right = check_tile_collision(position_x + width, position_y, respect_holes);
    bool bottom_right = check_tile_collision(position_x + width, position_y + height, respect_holes);
    bool top_left = check_tile_collision(position_x, position_y, respect_holes);
    bool bottom_left = check_tile_collision(position_x, position_y + height, respect_holes);

    bool double_x = (bottom_left && top_left) || (top_right && bottom_right);
    bool double_y = (top_right && top_left) || (bottom_right && bottom_left);

    direction_t dir = 0;

    if((top_right || bottom_right) && (!double_y || double_x)) {
        int dis_right = position_x + width - TILE_TO_X_COORD(COORD_TO_X_TILE(position_x + width));
        if(dis_right <= velocity_x) {
            dir |= RIGHT;
            position_x -= dis_right;
        }
    }
    if((top_left || bottom_left) && (!double_y || double_x)) {
        int dis_left = TILE_TO_X_COORD(COORD_TO_X_TILE(position_x) + 1) - position_x;
        if(dis_left <= -velocity_x) {
            dir |= LEFT;
            position_x += dis_left;
        }
    }
    if((top_left || top_right) && (!double_x || double_y)) {
        int dis_up = TILE_TO_Y_COORD(COORD_TO_Y_TILE(position_y) + 1) - position_y;
        if(dis_up <= -velocity_y) {
            dir |= UP;
            position_y += dis_up;
        }
    }
    if((bottom_left || bottom_right) && (!double_x || double_y)) {
        int dis_down = position_y + height - TILE_TO_Y_COORD(COORD_TO_Y_TILE(position_y + height));
        if(dis_down <= velocity_y) {
            dir |= DOWN;
            position_y -= dis_down;
        }
    }

    profiler_end(tile_collision);
    return dir;
}

bool PhysicsBody::center_distance_less_than(PhysicsBody *other, uint dis) const {
    if((uint)abs((int) other->center_x() - (int) center_x()) > dis) return false;
    if((uint)abs((int) other->center_y() - (int) center_y()) > dis) return false;

    int delta_x = ((int)other->center_x() - (int)center_x());
    int delta_y = ((int)other->center_y() - (int)center_y());

    return (uint)(delta_x * delta_x + delta_y * delta_y) < dis * dis;
}

// todo: optimize
bool PhysicsBody::collides_line(struct line_seg *ls) const {
    struct line_seg border;
    //top
    border.x1 = position_x;
    border.x2 = position_x + width;
    border.y1 = position_y;
    border.y2 = position_y;
    if(seg_collides_seg(&border, ls)) return true;
    //bottom
    border.y1 += height;
    border.y2 += height;
    if(seg_collides_seg(&border, ls)) return true;
    //left
    border.x2 = position_x;
    border.y1 = position_y;
    if(seg_collides_seg(&border, ls)) return true;
    //right
    border.x1 += width;
    border.x2 += width;
    if(seg_collides_seg(&border, ls)) return true;
    return false;
}

void PhysicsBody::render(uint8_t layer) {
    if(layer != 0) return;

    uint x = SCREEN_X(position_x);
    uint8_t y = SCREEN_Y(position_y);
    uint8_t width = SCREEN_DELTA_X(this->width);
    uint8_t height = SCREEN_DELTA_Y(this->height);
    if(pdraw_RectRegion(x, y, width, height)) {
        gfx_Rectangle(x, y, width, height);
    }
}

void PhysicsBody::tick() {
    position_x += velocity_x;
    position_y += velocity_y;

    if(tile_collisions) {
        direction_t dir = process_tile_collision();
        handle_tile_collision(dir);
    }
}

void PhysicsBody::handle_tile_collision(__attribute__((unused)) direction_t dir) {

}

void PhysicsBody::handle_explosion() {
    // Do nothing, by default
}

void PhysicsBody::collide([[maybe_unused]] Tank *tank) {}
void PhysicsBody::collide([[maybe_unused]] Shell *shell) {}
void PhysicsBody::collide([[maybe_unused]] Mine *mine) {}
void PhysicsBody::collide([[maybe_unused]] MineDetector *detector) {}
