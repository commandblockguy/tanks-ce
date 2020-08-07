#include "mine.h"
#include "level.h"
#include "globals.h"
#include "tank.h"
#include "graphics.h"


void detonate(mine_t *mine) {

    mine->alive = false;
    mine->countdown = EXPLOSION_ANIM - 1;

    //The original game uses a radius, not a square
    //Don't tell anyone.

    for(uint8_t j = COORD_TO_X_TILE(center_x(&mine->phys) - MINE_EXPLOSION_RADIUS);
        j <= COORD_TO_X_TILE(center_x(&mine->phys) + MINE_EXPLOSION_RADIUS); j++) {
        if(j < 0 || j >= LEVEL_SIZE_X) continue;
        for(uint8_t k = COORD_TO_Y_TILE(center_y(&mine->phys) - MINE_EXPLOSION_RADIUS);
            k <= COORD_TO_Y_TILE(center_y(&mine->phys) + MINE_EXPLOSION_RADIUS); k++) {
            if(k < 0 || k >= LEVEL_SIZE_Y) continue;
            if(TILE_TYPE(tiles[k][j]) == DESTRUCTIBLE) tiles[k][j] |= DESTROYED;
            needs_redraw = true;
        }
    }

    for(uint8_t j = 0; j < game.level.num_tanks; j++) {
        tank_t *tank = &tanks[j];
        if(tank->alive && center_distance_less_than(&mine->phys, &tank->phys, MINE_EXPLOSION_RADIUS)) {
            tank->alive = false;
            game.kills[tanks[j].type]++;
            if(tanks[j].type != PLAYER) game.total_kills++;
        }
        for(uint8_t k = 0; k < max_shells[tank->type]; k++) {
            shell_t *shell = &tank->shells[k];
            if(shell->alive && center_distance_less_than(&mine->phys, &shell->phys, MINE_EXPLOSION_RADIUS)) {
                shell->alive = false;
            }
        }
        for(uint8_t k = 0; k < max_mines[tank->type]; k++) {
            mine_t *mine2 = &tank->mines[k];
            if(mine2->alive && center_distance_less_than(&mine->phys, &mine2->phys, MINE_EXPLOSION_RADIUS)) {
                detonate(mine2);
            }
        }
    }

    generate_bg_tilemap();
}

void process_mine(mine_t *mine, tank_t *tank) {
    //Ignore mines which have already finished their countdowns
    if(!mine->countdown) return;

    if(--mine->countdown == EXPLOSION_ANIM) {
        detonate(mine);
    }
    if(!mine->alive) return;

    //mine belongs to enemy
    if(tank != &tanks[0]) {
        if(center_distance_less_than(&mine->phys, &tanks[0].phys, MINE_EXPLOSION_RADIUS)) {
            detonate(mine);
            return;
        }
    }

    //mine belongs to our tank
    if(!center_distance_less_than(&mine->phys, &tanks[0].phys, MINE_EXPLOSION_RADIUS)) {
        for(uint8_t j = 1; j < game.level.num_tanks; j++) {
            if(center_distance_less_than(&mine->phys, &tanks[j].phys, MINE_EXPLOSION_RADIUS)) {
                detonate(mine);
                break;
            }
        }
    }
}
