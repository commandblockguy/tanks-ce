#include <keypadc.h>
#include "ai.h"
#include "collision.h"
#include "globals.h"
#include "level.h"
#include "mine.h"
#include "profiler.h"
#include "shell.h"
#include "tank.h"
#include "dynamic_sprites.h"

const uint8_t Tank::max_shells[] = {5, 1, 1, 1, 1, 3, 2, 5, 5, 2};
const uint8_t Tank::max_mines[] = {2, 0, 0, 0, 4, 0, 0, 2, 2, 2};
const uint8_t Tank::max_bounces[] = {1, 1, 1, 0, 1, 1, 2, 1, 1, 0};
const uint8_t Tank::velocities[] = {(uint8_t)TANK_SPEED_NORMAL,
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
    if(this == game.player) {
        game.player_alive = false;
        game.player = nullptr;
    } else {
        game.total_kills++;
        game.kills[type]++;
    }
    game.num_tanks--;
}

void Tank::process() {
    profiler_add(ai);
    ai_process_move(this);
    ai_process_fire(this);
    profiler_end(ai);

    position_x += velocity_x;
    position_y += velocity_y;

    profiler_add(tank_collision);
    if(!(kb_IsDown(kb_Key1) && this == game.player))
        process_reflection();
    profiler_end(tank_collision);
}

void Tank::render() {
    uint8_t base_sprite = (((uint8_t) -((tread_rot >> 16) - 64)) >> 3) & 0xF;
    uint8_t turret_sprite = ((uint8_t) -((barrel_rot >> 16) - 64)) >> 4;

    // todo: a lot of this seems to be running twice as often as it needs to
    if(type == PLAYER) {
        render_obscured_object(tank_bases[type], pl_base_x_offsets, pl_base_y_offsets, this,
                               base_sprite, 0);
        render_obscured_object(tank_turrets[type], pl_turret_x_offsets, pl_turret_y_offsets, this,
                               turret_sprite, 0);
    } else {
        render_obscured_object(tank_bases[type], en_base_x_offsets, en_base_y_offsets, this,
                               base_sprite, 0);
        render_obscured_object(tank_turrets[type], en_turret_x_offsets, en_turret_y_offsets, this,
                               turret_sprite, 0);
    }
}

void Tank::fire_shell() {
    if(!can_shoot()) return;
    Shell *shell = new Shell(this);
    int24_t vector_x, vector_y;

    shell->bounces = max_bounces[type];

    vector_x = fast_cos(barrel_rot);
    vector_y = fast_sin(barrel_rot);

    shell->position_x = center_x() + BARREL_LENGTH * vector_x / TRIG_SCALE;
    shell->position_y = center_y() + BARREL_LENGTH * vector_y / TRIG_SCALE;

    if(type == MISSILE || type == IMMOB_MISSILE) {
        shell->velocity_x = SHELL_SPEED_MISSILE * vector_x / TRIG_SCALE;
        shell->velocity_y = SHELL_SPEED_MISSILE * vector_y / TRIG_SCALE;
    } else {
        shell->velocity_x = SHELL_SPEED_STANDARD * vector_x / TRIG_SCALE;
        shell->velocity_y = SHELL_SPEED_STANDARD * vector_y / TRIG_SCALE;
    }
    shell->direction = Shell::angle_to_shell_direction(barrel_rot);

    num_shells++;
}

bool Tank::can_shoot() const {
    return num_shells < max_shells[type];
}

void Tank::lay_mine() {
    if(!can_lay_mine()) return;
    Mine *mine = new Mine(this);
    mine->position_x = position_x + (TANK_SIZE - MINE_SIZE) / 2;
    mine->position_y = position_y + (TANK_SIZE - MINE_SIZE) / 2;

    num_mines++;
}

bool Tank::can_lay_mine() const {
    return num_mines < max_mines[type];
}

void Tank::set_velocity(int24_t velocity) {
    if(velocity == 0) {
        velocity_x = 0;
        velocity_y = 0;
    } else {
        velocity_x = (int24_t) velocity * fast_cos(tread_rot) / TRIG_SCALE;
        velocity_y = (int24_t) velocity * fast_sin(tread_rot) / TRIG_SCALE;
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

    uint24_t dis_up = -1;
    uint24_t dis_down = -1;
    uint24_t dis_left = -1;
    uint24_t dis_right = -1;

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
        position_y += dis_up / 2;
        tank->position_y -= dis_up / 2;
    }
    if(dis_left < dis_up && dis_left < dis_down) {
        position_x += dis_left / 2;
        tank->position_x -= dis_left / 2;
    }
    if(dis_down < dis_left && dis_down < dis_right) {
        position_y -= dis_down / 2;
        tank->position_y += dis_down / 2;
    }
    if(dis_right < dis_up && dis_right < dis_down) {
        position_x -= dis_right / 2;
        tank->position_x += dis_right / 2;
    }
}

void Tank::collide(Shell *shell) {
    shell->collide(this);
}

void Tank::collide(Mine *mine) {
    // don't do anything
}
