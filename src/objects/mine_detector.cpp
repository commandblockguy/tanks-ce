#include "mine_detector.h"

#include "mine.h"
#include "tank.h"

MineDetector::MineDetector(Mine *mine):
    PhysicsBody(MINE_EXPLOSION_RADIUS * 2, MINE_EXPLOSION_RADIUS * 2),
    primed(false) {
    tile_collisions = false;

    position_x = mine->center_x() - MINE_EXPLOSION_RADIUS;
    position_y = mine->center_y() - MINE_EXPLOSION_RADIUS;

    parent = mine;
}

void MineDetector::process() {
    if(!parent) active = false;

    if(!primed) {
        Tank *placer = (Tank*)parent->parent;
        if(!placer || !detect_collision(placer)) {
            primed = true;
        }
    }
}

void MineDetector::render(__attribute__((unused)) uint8_t layer) {
    // this is invisible, so do nothing
}

void MineDetector::handle_collision(PhysicsBody *other) {
    other->collide(this);
}

void MineDetector::collide(__attribute__((unused)) Tank *tank) {
    if(primed && center_distance_less_than(tank, MINE_EXPLOSION_RADIUS)) {
        if(parent) {
            ((Mine*)parent)->countdown = MINE_TRIGGERED;
        }
        active = false;
    }
}
