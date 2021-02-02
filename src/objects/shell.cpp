#include "shell.h"
#include "tank.h"
#include "../graphics/dynamic_sprites.h"
#include "../graphics/graphics.h"
#include "../util/profiler.h"
#include "../graphics/partial_redraw.h"
#include "../graphics/tiles.h"

Shell::Shell(Tank *tank):
    PhysicsBody(SHELL_SIZE, SHELL_SIZE),
    bounces(tank->max_bounces()) {
    tile_collisions = true;
    respect_holes = false;

    int vector_x = fast_cos(tank->barrel_rot);
    int vector_y = fast_sin(tank->barrel_rot);

    position_x = tank->center_x() + BARREL_LENGTH * vector_x / TRIG_SCALE;
    position_y = tank->center_y() + BARREL_LENGTH * vector_y / TRIG_SCALE;

    if(tank->type == MISSILE || tank->type == IMMOB_MISSILE) {
        velocity_x = SHELL_SPEED_MISSILE * vector_x / TRIG_SCALE;
        velocity_y = SHELL_SPEED_MISSILE * vector_y / TRIG_SCALE;
    } else {
        velocity_x = SHELL_SPEED_STANDARD * vector_x / TRIG_SCALE;
        velocity_y = SHELL_SPEED_STANDARD * vector_y / TRIG_SCALE;
    }
    direction = angle_to_shell_direction(tank->barrel_rot);

    left_tank_hitbox = false;
    parent = tank;
};

Shell::~Shell() {
    if(parent) {
        ((Tank*)parent)->num_shells--;
    }
}

void Shell::process() {
    profiler_add(shells);

    if(!parent || (!left_tank_hitbox && !detect_collision(parent))) {
        left_tank_hitbox = true;
    }

    profiler_end(shells);
}

void Shell::render(uint8_t layer) {
    if(layer != 2) return;
    profiler_add(render_shells);

    uint8_t sprite = direction;
    gfx_region_t region;
    get_sprite_footprint(&region, this, shell_sprites, shell_x_offsets, shell_y_offsets, sprite);
    if(pdraw_RectRegion(&region)) {
        gfx_TransparentSprite(shell_sprites[sprite], region.xmin, region.ymin);
        redraw_tiles(&region, 0);
    }

    profiler_end(render_shells);
}

void Shell::update_direction() {
    angle_t angle = fast_atan2(velocity_y, velocity_x);
    direction = angle_to_shell_direction(angle);
}

void Shell::handle_explosion() {
    kill();
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
    mine->detonate();
}

void Shell::handle_tile_collision(direction_t dir) {
    if(!dir) return;

    if(!bounces) {
        kill();
        return;
    }

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
}

void Shell::kill() {
    active = false;
}
