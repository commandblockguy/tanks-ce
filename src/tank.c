#include "tank.h"
#include "collision.h"
#include "ai.h"
#include "shell.h"
#include "mine.h"
#include "globals.h"
#include "profiler.h"

//todo: fix headers
void processShell(shell_t *shell, tank_t *tank);
void processMine(mine_t *mine, tank_t *tank);

const uint8_t max_shells[]  = 	{5, 1, 1, 1, 1, 3, 2, 5, 5, 2};
const uint8_t max_mines[]   = 	{2, 0, 0, 0, 4, 0, 0, 2, 2, 2};
const uint8_t max_bounces[] = 	{1, 1, 1, 0, 1, 1, 2, 1, 1, 0};

//Process tank physics
void processTank(tank_t* tank) {
    int i;

    if(tank->alive) {
        profiler_add(ai);
        ai_process_move(tank);
        ai_process_fire(tank);
        profiler_end(ai);

        tank->phys.position_x += tank->phys.velocity_x;
        tank->phys.position_y += tank->phys.velocity_y;

        profiler_add(tank_collision);
        processReflection(&tank->phys, true);

        for(i = game.level.num_tanks - 1; i >= 0; i--) {
            if(tanks[i].alive)
                collideAndPush(&tank->phys, &tanks[i].phys);
        }
        profiler_end(tank_collision);
    }

    //Loop through all shells
    profiler_add(shells);
    for(i = max_shells[tank->type] - 1; i >= 0; i--) {
        processShell(&tank->shells[i], tank);
    }
    profiler_end(shells);
    //Loop through mines
    profiler_add(mines);
    if(max_mines[tank->type]) {
        for(i = max_mines[tank->type] - 1; i >= 0; i--) {
            processMine(&tank->mines[i], tank);
        }
    }
    profiler_end(mines);
}

bool fireShell(tank_t* tank) {
    int i;
    for(i = max_shells[tank->type] - 1; i >= 0; i--) {
        shell_t* shell = &tank->shells[i];
        int24_t vector_x, vector_y;

        if(shell->alive) continue;

        shell->alive = true;
        shell->left_tank_hitbox = false;
        shell->bounces = max_bounces[tank->type];

        vector_x = fast_cos(tank->barrel_rot);
        vector_y = fast_sin(tank->barrel_rot);

        shell->phys.position_x =
                centerX(&tank->phys) + BARREL_LENGTH * vector_x / TRIG_SCALE;
        shell->phys.position_y =
                centerY(&tank->phys) + BARREL_LENGTH * vector_y / TRIG_SCALE;

        shell->phys.width = shell->phys.height = SHELL_SIZE;
        shell->phys.type = ShellPhysics;
        if(tank->type == MISSILE || tank->type == IMMOB_MISSILE) {
            shell->phys.velocity_x = SHELL_SPEED_MISSILE * vector_x / TRIG_SCALE;
            shell->phys.velocity_y = SHELL_SPEED_MISSILE * vector_y / TRIG_SCALE;
        } else {
            shell->phys.velocity_x = SHELL_SPEED_STANDARD * vector_x / TRIG_SCALE;
            shell->phys.velocity_y = SHELL_SPEED_STANDARD * vector_y / TRIG_SCALE;
        }
        return true;
    }
    return false;
}

bool canShoot(tank_t* tank) {
    int i;
    for(i = max_shells[tank->type] - 1; i >= 0; i--) {
        if(!tank->shells[i].alive) {
            return true;
        }
    }
    return false;
}

bool layMine(tank_t* tank) {
    int i;
    if(!max_mines[tank->type]) return false;
    for(i = max_mines[tank->type] - 1; i >= 0; i--) {
        mine_t* mine = &tank->mines[i];
        if(mine->alive) continue;
        mine->alive = true;
        mine->countdown = MINE_COUNTDOWN;
        mine->phys.position_x = tank->phys.position_x + (TANK_SIZE - MINE_SIZE) / 2;
        mine->phys.position_y = tank->phys.position_y + (TANK_SIZE - MINE_SIZE) / 2;
        mine->phys.width = mine->phys.height = MINE_SIZE;
        return true;
    }
    return false;
}
