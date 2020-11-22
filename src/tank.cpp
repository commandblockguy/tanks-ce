#include <keypadc.h>
#include "ai.h"
#include "collision.h"
#include "globals.h"
#include "level.h"
#include "mine.h"
#include "profiler.h"
#include "shell.h"
#include "tank.h"

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

Tank::Tank(const serialized_tank_t *ser_tank) {
    this->width = TANK_SIZE;
    this->height = TANK_SIZE;
    this->respect_holes = true;

    this->alive = true;
    this->type = ser_tank->type;
    // add 1 because the level system uses coordinates from the first non-border block
    this->start_x = ser_tank->start_x + 1;
    this->start_y = ser_tank->start_y + 1;
    this->position_x = TILE_TO_X_COORD(this->start_x);
    this->position_y = TILE_TO_Y_COORD(this->start_y);
    this->velocity_x = 0;
    this->velocity_y = 0;
    this->barrel_rot = 0;
    this->tread_rot = 0;
}

void Tank::process() {
    if(this->alive) {
        profiler_add(ai);
        ai_process_move(this);
        ai_process_fire(this);
        profiler_end(ai);

        this->position_x += this->velocity_x;
        this->position_y += this->velocity_y;

        profiler_add(tank_collision);
        if(!(kb_IsDown(kb_Key1) && this == tanks))
            this->process_reflection();

        for(int8_t i = game.level.num_tanks - 1; i >= 0; i--) {
            if(tanks[i].alive)
                collide_and_push(&tanks[i]);
        }
        profiler_end(tank_collision);
    }

    //Loop through all shells
    profiler_add(shells);
    for(int8_t i = max_shells[this->type] - 1; i >= 0; i--) {
        this->shells[i].process();
    }
    profiler_end(shells);
    //Loop through mines
    profiler_add(mines);
    if(max_mines[this->type]) {
        for(int8_t i = max_mines[this->type] - 1; i >= 0; i--) {
            this->mines[i].process();
        }
    }
    profiler_end(mines);
}

bool Tank::fire_shell() {
    for(int8_t i = max_shells[this->type] - 1; i >= 0; i--) {
        Shell *shell = &this->shells[i];
        int24_t vector_x, vector_y;

        if(shell->alive) continue;

        shell->tank = this;
        shell->alive = true;
        shell->left_tank_hitbox = false;
        shell->bounces = max_bounces[this->type];

        vector_x = fast_cos(this->barrel_rot);
        vector_y = fast_sin(this->barrel_rot);

        shell->position_x = this->center_x() + BARREL_LENGTH * vector_x / TRIG_SCALE;
        shell->position_y = this->center_y() + BARREL_LENGTH * vector_y / TRIG_SCALE;

        shell->width = shell->height = SHELL_SIZE;
        if(this->type == MISSILE || this->type == IMMOB_MISSILE) {
            shell->velocity_x = SHELL_SPEED_MISSILE * vector_x / TRIG_SCALE;
            shell->velocity_y = SHELL_SPEED_MISSILE * vector_y / TRIG_SCALE;
        } else {
            shell->velocity_x = SHELL_SPEED_STANDARD * vector_x / TRIG_SCALE;
            shell->velocity_y = SHELL_SPEED_STANDARD * vector_y / TRIG_SCALE;
        }
        shell->direction = Shell::angle_to_shell_direction(this->barrel_rot);
        return true;
    }
    return false;
}

bool Tank::can_shoot() {
    for(int8_t i = max_shells[this->type] - 1; i >= 0; i--) {
        if(!this->shells[i].alive) {
            return true;
        }
    }
    return false;
}

bool Tank::lay_mine() {
    if(!max_mines[this->type]) return false;
    for(int8_t i = max_mines[this->type] - 1; i >= 0; i--) {
        Mine *mine = &this->mines[i];
        if(mine->alive) continue;
        mine->tank = this;
        mine->alive = true;
        mine->countdown = MINE_COUNTDOWN;
        mine->position_x = this->position_x + (TANK_SIZE - MINE_SIZE) / 2;
        mine->position_y = this->position_y + (TANK_SIZE - MINE_SIZE) / 2;
        mine->width = mine->height = MINE_SIZE;
        return true;
    }
    return false;
}

void Tank::set_velocity(int24_t velocity) {
    if(velocity == 0) {
        this->velocity_x = 0;
        this->velocity_y = 0;
    } else {
        this->velocity_x = (int24_t) velocity * fast_cos(this->tread_rot) / TRIG_SCALE;
        this->velocity_y = (int24_t) velocity * fast_sin(this->tread_rot) / TRIG_SCALE;
    }
}

bool Tank::collide_and_push(Tank *other) {
    //Figure out if the four corners are colliding
    bool top_right = other->is_point_inside(this->position_x + this->width, this->position_y);
    bool bottom_right = other->is_point_inside(this->position_x + this->width, this->position_y + this->height);
    bool top_left = other->is_point_inside(this->position_x, this->position_y);
    bool bottom_left = other->is_point_inside(this->position_x, this->position_y + this->height);

    uint24_t dis_up = -1;
    uint24_t dis_down = -1;
    uint24_t dis_left = -1;
    uint24_t dis_right = -1;

    if(!(top_right || bottom_right || top_left || bottom_left)) return false;

    if((top_right || bottom_right)) {
        dis_right = this->position_x + this->width - other->position_x;
    }
    if((top_left || bottom_left)) {
        dis_left = other->position_x + other->width - this->position_x;
    }
    if((top_left || top_right)) {
        dis_up = other->position_y + other->height - this->position_y;
    }
    if((bottom_left || bottom_right)) {
        dis_down = this->position_y + this->height - other->position_y;
    }

    //pick the direction with the smallest distance
    if(dis_up < dis_left && dis_up < dis_right) {
        this->position_y += dis_up / 2;
        other->position_y -= dis_up / 2;
    }
    if(dis_left < dis_up && dis_left < dis_down) {
        this->position_x += dis_left / 2;
        other->position_x -= dis_left / 2;
    }
    if(dis_down < dis_left && dis_down < dis_right) {
        this->position_y -= dis_down / 2;
        other->position_y += dis_down / 2;
    }
    if(dis_right < dis_up && dis_right < dis_down) {
        this->position_x -= dis_right / 2;
        other->position_x += dis_right / 2;
    }

    return true;
}
