#include "tank.h"

#include <new>

#include <debug.h>
#include <keypadc.h>

#include "../ai/ai.h"
#include "../globals.h"
#include "../util/profiler.h"
#include "../graphics/dynamic_sprites.h"
#include "../graphics/graphics.h"
#include "../graphics/partial_redraw.h"
#include "mine_detector.h"

const uint8_t Tank::max_shells[] = {5, 1, 1, 1, 1, 3, 2, 5, 5, 2};
const uint8_t Tank::max_mines[] = {2, 0, 0, 0, 4, 0, 0, 2, 2, 2};
const uint8_t Tank::max_bounces[] = {1, 1, 1, 0, 1, 1, 2, 1, 1, 0};
const uint8_t Tank::velocities[] = {(uint8_t)TANK_SPEED_HIGH,
                                    (uint8_t)0,
                                    (uint8_t)TANK_SPEED_SLOW,
                                    (uint8_t)TANK_SPEED_SLOW,
                                    (uint8_t)TANK_SPEED_HIGH,
                                    (uint8_t)TANK_SPEED_NORMAL,
                                    (uint8_t)0,
                                    (uint8_t)TANK_SPEED_HIGH,
                                    (uint8_t)TANK_SPEED_NORMAL,
                                    (uint8_t)TANK_SPEED_BLACK};

Tank::Tank(const serialized_tank_t *ser_tank, uint8_t id) {
    width = TANK_SIZE;
    height = TANK_SIZE;
    tile_collisions = true;
    respect_holes = true;

    type = ser_tank->type;
    this->id = id;
    // add 1 because the level system uses coordinates from the first non-border block
    start_x = ser_tank->start_x + 1;
    start_y = ser_tank->start_y + 1;
    position_x = TILE_TO_X_COORD(start_x);
    position_y = TILE_TO_Y_COORD(start_y);
    velocity_x = 0;
    velocity_y = 0;
    barrel_rot = 0;
    tread_rot = 0;

    if(id == 0) {
        game.player = this;
        game.player_alive = true;
    }

    game.num_tanks++;
}

Tank::~Tank() {
    game.num_tanks--;
    if(this == game.player) {
        game.player_alive = false;
        game.player = nullptr;
    }
}

void Tank::kill() {
    if(this != game.player) {
        game.total_kills++;
        game.kills[type]++;
        game.alive_tanks[id] = false;
    }

    delete this;
}

void Tank::process() {
    profiler_add(tanks);

    profiler_add(ai);
    ai_process_move(this);
    ai_process_fire(this);
    profiler_end(ai);

    profiler_end(tanks);
}

void Tank::render(uint8_t layer) {
    if(layer != 1) return;
    profiler_add(render_tanks);

    uint8_t base_index = (((uint8_t) -((tread_rot >> (INT_BITS - 8)) - 64)) >> 3) & 0xF;
    uint8_t turret_index = ((uint8_t) -((barrel_rot >> (INT_BITS - 8)) - 68)) >> 4;

    gfx_sprite_t *base_sprite = tank_bases[type][base_index];
    gfx_sprite_t *turret_sprite = tank_turrets[type][turret_index];
    gfx_region_t base_region, turret_region, combined_region;

    if(type == PLAYER) {
        get_sprite_footprint(&base_region, this, tank_bases[type], pl_base_x_offsets, pl_base_y_offsets, base_index);
        get_sprite_footprint(&turret_region, this, tank_turrets[type], pl_turret_x_offsets, pl_turret_y_offsets, turret_index);
    } else {
        get_sprite_footprint(&base_region, this, tank_bases[type], en_base_x_offsets, en_base_y_offsets, base_index);
        get_sprite_footprint(&turret_region, this, tank_turrets[type], en_turret_x_offsets, en_turret_y_offsets, turret_index);
    }

    combined_region.xmin = min(base_region.xmin, turret_region.xmin);
    combined_region.xmax = max(base_region.xmax, turret_region.xmax);
    combined_region.ymin = min(base_region.ymin, turret_region.ymin);
    combined_region.ymax = max(base_region.ymax, turret_region.ymax);

    if(pdraw_RectRegion(&combined_region)) {
        gfx_TransparentSprite(base_sprite, base_region.xmin, base_region.ymin);
        gfx_TransparentSprite(turret_sprite, turret_region.xmin, turret_region.ymin);
        redraw_tiles(&combined_region, 0);
    }

    profiler_end(render_tanks);
}

void Tank::fire_shell() {
    if(!can_shoot()) return;
    Shell *shell = new (std::nothrow) Shell(this);
    if(!shell) {
        dbg_printf("Failed to allocate fired shell\n");
        return;
    }

    num_shells++;
}

bool Tank::can_shoot() const {
    return num_shells < max_shells[type];
}

void Tank::lay_mine() {
    if(!can_lay_mine()) return;
    Mine *mine = new (std::nothrow) Mine(this);
    if(!mine) {
        dbg_printf("Failed to allocate mine\n");
        return;
    }

    num_mines++;
}

bool Tank::can_lay_mine() const {
    return num_mines < max_mines[type];
}

void Tank::set_velocity(int velocity) {
    if(velocity == 0) {
        velocity_x = 0;
        velocity_y = 0;
    } else {
        velocity_x = velocity * fast_cos(tread_rot) / TRIG_SCALE;
        velocity_y = velocity * fast_sin(tread_rot) / TRIG_SCALE;
    }
}

void Tank::handle_collision(PhysicsBody *other) {
    other->collide(this);
}

void Tank::collide(Tank *tank) {
    //Figure out if the four corners are colliding
    bool top_right = tank->is_point_inside(position_x + width, position_y);
    bool bottom_right = tank->is_point_inside(position_x + width, position_y + height);
    bool top_left = tank->is_point_inside(position_x, position_y);
    bool bottom_left = tank->is_point_inside(position_x, position_y + height);

    uint dis_up = -1;
    uint dis_down = -1;
    uint dis_left = -1;
    uint dis_right = -1;

    if(!(top_right || bottom_right || top_left || bottom_left)) return;

    if((top_right || bottom_right)) {
        dis_right = position_x + width - tank->position_x;
    }
    if((top_left || bottom_left)) {
        dis_left = tank->position_x + tank->width - position_x;
    }
    if((top_left || top_right)) {
        dis_up = tank->position_y + tank->height - position_y;
    }
    if((bottom_left || bottom_right)) {
        dis_down = position_y + height - tank->position_y;
    }

    //pick the direction with the smallest distance
    if(dis_up < dis_left && dis_up < dis_right) {
        velocity_y += dis_up / 2;
        tank->velocity_y -= dis_up / 2;
    }
    if(dis_left < dis_up && dis_left < dis_down) {
        velocity_x += dis_left / 2;
        tank->velocity_x -= dis_left / 2;
    }
    if(dis_down < dis_left && dis_down < dis_right) {
        velocity_y -= dis_down / 2;
        tank->velocity_y += dis_down / 2;
    }
    if(dis_right < dis_up && dis_right < dis_down) {
        velocity_x -= dis_right / 2;
        tank->velocity_x += dis_right / 2;
    }
}

void Tank::collide(Shell *shell) {
    shell->collide(this);
}

void Tank::collide(__attribute__((unused)) Mine *mine) {
    // don't do anything
}

void Tank::collide(MineDetector *detector) {
    detector->collide(this);
}
