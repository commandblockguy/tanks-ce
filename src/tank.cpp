#include <keypadc.h>
#include "ai.h"
#include "collision.h"
#include "globals.h"
#include "level.h"
#include "mine.h"
#include "profiler.h"
#include "shell.h"
#include "tank.h"

const uint8_t max_shells[] = {5, 1, 1, 1, 1, 3, 2, 5, 5, 2};
const uint8_t max_mines[] = {2, 0, 0, 0, 4, 0, 0, 2, 2, 2};
const uint8_t max_bounces[] = {1, 1, 1, 0, 1, 1, 2, 1, 1, 0};
const uint8_t tank_velocities[] = {(uint8_t)TANK_SPEED_NORMAL,
                                   (uint8_t)0,
                                   (uint8_t)TANK_SPEED_SLOW,
                                   (uint8_t)TANK_SPEED_SLOW,
                                   (uint8_t)TANK_SPEED_HIGH,
                                   (uint8_t)TANK_SPEED_NORMAL,
                                   (uint8_t)0,
                                   (uint8_t)TANK_SPEED_HIGH,
                                   (uint8_t)TANK_SPEED_NORMAL,
                                   (uint8_t)TANK_SPEED_BLACK};

void init_tank(tank_t *tank) {
    tank->phys.type = PHYS_TANK;
    tank->phys.position_x = TILE_TO_X_COORD(tank->start_x);
    tank->phys.position_y = TILE_TO_Y_COORD(tank->start_y);
    tank->phys.velocity_x = 0;
    tank->phys.velocity_y = 0;
    tank->phys.height = TANK_SIZE;
    tank->phys.width = TANK_SIZE;
    tank->barrel_rot = 0;
    tank->tread_rot = 0;
}

//Process tank physics
void process_tank(tank_t *tank) {
    if(tank->alive) {
        profiler_add(ai);
        ai_process_move(tank);
        ai_process_fire(tank);
        profiler_end(ai);

        tank->phys.position_x += tank->phys.velocity_x;
        tank->phys.position_y += tank->phys.velocity_y;

        profiler_add(tank_collision);
        if(!(kb_IsDown(kb_Key1) && tank == tanks))
            process_reflection(&tank->phys, true);

        for(int8_t i = game.level.num_tanks - 1; i >= 0; i--) {
            if(tanks[i].alive)
                collide_and_push(&tank->phys, &tanks[i].phys);
        }
        profiler_end(tank_collision);
    }

    //Loop through all shells
    profiler_add(shells);
    for(int8_t i = max_shells[tank->type] - 1; i >= 0; i--) {
        process_shell(&tank->shells[i], tank);
    }
    profiler_end(shells);
    //Loop through mines
    profiler_add(mines);
    if(max_mines[tank->type]) {
        for(int8_t i = max_mines[tank->type] - 1; i >= 0; i--) {
            process_mine(&tank->mines[i], tank);
        }
    }
    profiler_end(mines);
}

bool fire_shell(tank_t *tank) {
    for(int8_t i = max_shells[tank->type] - 1; i >= 0; i--) {
        shell_t *shell = &tank->shells[i];
        int24_t vector_x, vector_y;

        if(shell->alive) continue;

        shell->alive = true;
        shell->left_tank_hitbox = false;
        shell->bounces = max_bounces[tank->type];

        vector_x = fast_cos(tank->barrel_rot);
        vector_y = fast_sin(tank->barrel_rot);

        shell->phys.position_x = center_x(&tank->phys) + BARREL_LENGTH * vector_x / TRIG_SCALE;
        shell->phys.position_y = center_y(&tank->phys) + BARREL_LENGTH * vector_y / TRIG_SCALE;

        shell->phys.type = PHYS_SHELL;
        shell->phys.width = shell->phys.height = SHELL_SIZE;
        if(tank->type == MISSILE || tank->type == IMMOB_MISSILE) {
            shell->phys.velocity_x = SHELL_SPEED_MISSILE * vector_x / TRIG_SCALE;
            shell->phys.velocity_y = SHELL_SPEED_MISSILE * vector_y / TRIG_SCALE;
        } else {
            shell->phys.velocity_x = SHELL_SPEED_STANDARD * vector_x / TRIG_SCALE;
            shell->phys.velocity_y = SHELL_SPEED_STANDARD * vector_y / TRIG_SCALE;
        }
        shell->direction = angle_to_shell_direction(tank->barrel_rot);
        return true;
    }
    return false;
}

bool can_shoot(tank_t *tank) {
    for(int8_t i = max_shells[tank->type] - 1; i >= 0; i--) {
        if(!tank->shells[i].alive) {
            return true;
        }
    }
    return false;
}

bool lay_mine(tank_t *tank) {
    if(!max_mines[tank->type]) return false;
    for(int8_t i = max_mines[tank->type] - 1; i >= 0; i--) {
        mine_t *mine = &tank->mines[i];
        if(mine->alive) continue;
        mine->alive = true;
        mine->countdown = MINE_COUNTDOWN;
        mine->phys.type = PHYS_MINE;
        mine->phys.position_x = tank->phys.position_x + (TANK_SIZE - MINE_SIZE) / 2;
        mine->phys.position_y = tank->phys.position_y + (TANK_SIZE - MINE_SIZE) / 2;
        mine->phys.width = mine->phys.height = MINE_SIZE;
        return true;
    }
    return false;
}

void set_velocity(tank_t *tank, int24_t velocity) {
    if(velocity == 0) {
        tank->phys.velocity_x = 0;
        tank->phys.velocity_y = 0;
    } else {
        tank->phys.velocity_x = (int24_t) velocity * fast_cos(tank->tread_rot) / TRIG_SCALE;
        tank->phys.velocity_y = (int24_t) velocity * fast_sin(tank->tread_rot) / TRIG_SCALE;
    }
}
