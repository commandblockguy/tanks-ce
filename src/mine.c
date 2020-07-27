#include "mine.h"
#include "level.h"
#include "globals.h"
#include "tank.h"
#include "graphics.h"


void detonate(mine_t *mine) {
    int j, k;

    mine->alive = false;
    mine->countdown = EXPLOSION_ANIM - 1;

    //The original game uses a radius, not a square
    //Don't tell anyone.

    for(j  = ptToXTile(centerX(&mine->phys) - MINE_EXPLOSION_RADIUS);
        j <= ptToXTile(centerX(&mine->phys) + MINE_EXPLOSION_RADIUS); j++) {
        if(j < 0 || j >= LEVEL_SIZE_X) continue;
        for(k  = ptToYTile(centerY(&mine->phys) - MINE_EXPLOSION_RADIUS);
            k <= ptToYTile(centerY(&mine->phys) + MINE_EXPLOSION_RADIUS); k++) {
            if(k < 0 || k >= LEVEL_SIZE_Y) continue;
            if(TILE_TYPE(tiles[k][j]) == DESTRUCTIBLE) tiles[k][j] |= DESTROYED;
            needs_redraw = true;
        }
    }

    for(j = 0; j < game.level.num_tanks; j++) {
        tank_t* tank = &tanks[j];
        if(tank->alive && centerDistanceLessThan(&mine->phys, &tank->phys,
                                                 MINE_EXPLOSION_RADIUS)) {
            tank->alive = false;
            game.kills[tanks[j].type]++;
            game.total_kills++;
        }
        for(k = 0; k < max_shells[tank->type]; k++) {
            shell_t* shell = &tank->shells[k];
            if(shell->alive && centerDistanceLessThan(&mine->phys, &shell->phys,
                                                      MINE_EXPLOSION_RADIUS)) {
                shell->alive = false;
            }
        }
        for(k = 0; k < max_mines[tank->type]; k++) {
            mine_t* mine2 = &tank->mines[k];
            if(mine2->alive && centerDistanceLessThan(&mine->phys, &mine2->phys,
                                                      MINE_EXPLOSION_RADIUS)) {
                detonate(mine2);
            }
        }
    }

    generate_bg_tilemap();
}

void processMine(mine_t *mine, tank_t *tank) {
    //Ignore mines which have already finished their countdowns
    if(!mine->countdown) return;

    if(--mine->countdown == EXPLOSION_ANIM) {
        detonate(mine);
    }
    if(!mine->alive) return;

    //mine belongs to enemy
    if(tank != &tanks[0]) {
        if(centerDistanceLessThan(&mine->phys, &tanks[0].phys,
                                  MINE_EXPLOSION_RADIUS)) {
            detonate(mine);
            return;
        }
    }

    //mine belongs to our tank
    if(!centerDistanceLessThan(&mine->phys, &tanks[0].phys,
                               MINE_EXPLOSION_RADIUS)) {
        uint8_t j;
        for(j = 1; j < game.level.num_tanks; j++) {
            if(centerDistanceLessThan(&mine->phys, &tanks[j].phys,
                                      MINE_EXPLOSION_RADIUS)) {
                detonate(mine);
                break;
            }
        }
    }
}
