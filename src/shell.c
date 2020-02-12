#include "shell.h"
#include "tank.h"
#include "mine.h"
#include "globals.h"

void processShell(shell_t* shell, tank_t* tank) {
    int j;
    reflection_t reflect;
    //Ignore dead shells
    if(!shell->alive) return;
    //Add velocity
    shell->phys.position_x += shell->phys.velocity_x;
    shell->phys.position_y += shell->phys.velocity_y;

    if(shell->left_tank_hitbox) {
        //This will eventually be part of the collision bit
        for(j = 0; j < game.level.num_tanks; j++) {
            int i;

            for(i = max_mines[tanks[j].type] - 1; i >= 0; i--) {
                mine_t* mine = &tanks[j].mines[i];
                if(mine->alive && detectCollision(&shell->phys, &mine->phys)) {
                    shell->alive = false;
                    detonate(mine);
                }
            }

            for(i = max_shells[tanks[j].type] - 1; i >= 0; i--) {
                shell_t* shell2 = &tanks[j].shells[i];
                if(shell != shell2 && shell2->alive && detectCollision(&shell->phys, &shell2->phys)) {
                    shell->alive = false;
                    shell2->alive = false;
                }
            }

            if(!tanks[j].alive) continue;

            if(detectCollision(&shell->phys, &tanks[j].phys)) {
                game.total_kills++;
                game.kills[tanks[j].type]++;
                tanks[j].alive = false;
                shell->alive = false;
            }
        }
    } else if(!detectCollision(&shell->phys, &tank->phys)) {
        shell->left_tank_hitbox = true;
    }

    processReflection(&reflect, &shell->phys, false);

    if(reflect.colliding) {
        shellRicochet(shell, reflect.dir);
    }
}

bool shellRicochet(shell_t* shell, direction_t dir) {
    //Determine if shell explodes here, and subtracts 1 from the bounces left
    if(!shell->bounces--) {
        shell->alive = false;
        return false;
    }
    //shell_t is still alive
    if(dir & UP || dir & DOWN)
        shell->phys.velocity_y *= -1;
    if(dir & LEFT || dir & RIGHT)
        shell->phys.velocity_x *= -1;
    return true;
}
