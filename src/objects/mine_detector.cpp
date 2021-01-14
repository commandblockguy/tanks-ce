#include "mine_detector.h"

#include "mine.h"
#include "tank.h"

MineDetector::MineDetector(Mine *mine) {
    primed = false;

    width = MINE_EXPLOSION_RADIUS * 2;
    height = MINE_EXPLOSION_RADIUS * 2;
    tile_collisions = false;

    position_x = mine->center_x() - MINE_EXPLOSION_RADIUS;
    position_y = mine->center_y() - MINE_EXPLOSION_RADIUS;

    velocity_x = 0;
    velocity_y = 0;

    parent = mine;
}

void MineDetector::process() {
    if(!parent) kill();

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
        kill();
    }
}

void MineDetector::collide(__attribute__((unused)) Shell *shell) {
    // don't do anything
}

void MineDetector::collide(__attribute__((unused)) Mine *mine) {
    // don't do anything
}

void MineDetector::collide(__attribute__((unused)) MineDetector *detector) {
    // don't do anything
}
