#include "shell.h"
#include "tank.h"
#include "mine.h"
#include "globals.h"
#include "dynamic_sprites.h"

Shell::Shell(Tank *tank) {
    width = SHELL_SIZE;
    height = SHELL_SIZE;
    respect_holes = false;

    left_tank_hitbox = false;
    parent = tank;
};

Shell::~Shell() {
    if(parent) {
        // todo: see if there's a way to do this without an ugly cast
        ((Tank*)parent)->num_shells--;
    }
}

void Shell::process() {
    direction_t collide_dir;
    //Add velocity
    position_x += velocity_x;
    position_y += velocity_y;

    if(!left_tank_hitbox && !detect_collision(parent)) {
        left_tank_hitbox = true;
    }

    collide_dir = process_reflection();

    if(collide_dir) {
        ricochet(collide_dir);
    }
}

void Shell::render() {
    uint8_t sprite = direction;
    render_obscured_object(shell_sprites, shell_x_offsets, shell_y_offsets, this, sprite, 0);
}

bool Shell::ricochet(direction_t dir) {
    if(!bounces) {
        kill();
        return false;
    }

    //shell_t is still alive
    if(dir & UP || dir & DOWN) {
        velocity_y *= -1;
        update_direction();
        bounces--;
    }
    if(dir & LEFT || dir & RIGHT) {
        velocity_x *= -1;
        update_direction();
        bounces--;
    }

    return true;
}

void Shell::update_direction() {
    angle_t angle = fast_atan2(velocity_y, velocity_x);
    direction = angle_to_shell_direction(angle);
}

void Shell::handle_collision(PhysicsBody *other) {
    other->collide(this);
}

void Shell::collide(Tank *tank) {
    if(left_tank_hitbox) {
        tank->kill();
        kill();
    }
}

void Shell::collide(Shell *shell) {
    kill();
    shell->kill();
}

void Shell::collide(Mine *mine) {
    kill();
    mine->kill();
}
