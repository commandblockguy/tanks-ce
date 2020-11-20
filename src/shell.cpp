#include "shell.h"
#include "tank.h"
#include "mine.h"
#include "globals.h"

void process_shell(shell_t *shell, tank_t *tank) {
    direction_t collide_dir;
    //Ignore dead shells
    if(!shell->alive) return;
    //Add velocity
    shell->phys.position_x += shell->phys.velocity_x;
    shell->phys.position_y += shell->phys.velocity_y;

    if(shell->left_tank_hitbox) {
        //This will eventually be part of the collision bit
        for(uint8_t j = 0; j < game.level.num_tanks; j++) {
            int i;

            for(i = max_mines[tanks[j].type] - 1; i >= 0; i--) {
                mine_t *mine = &tanks[j].mines[i];
                if(mine->alive && detect_collision(&shell->phys, &mine->phys)) {
                    shell->alive = false;
                    detonate(mine);
                }
            }

            for(i = max_shells[tanks[j].type] - 1; i >= 0; i--) {
                shell_t *shell2 = &tanks[j].shells[i];
                if(shell != shell2 && shell2->alive && detect_collision(&shell->phys, &shell2->phys)) {
                    shell->alive = false;
                    shell2->alive = false;
                }
            }

            if(!tanks[j].alive) continue;

            if(detect_collision(&shell->phys, &tanks[j].phys)) {
                if(tanks[j].type != PLAYER) game.total_kills++;
                game.kills[tanks[j].type]++;
                tanks[j].alive = false;
                shell->alive = false;
            }
        }
    } else if(!detect_collision(&shell->phys, &tank->phys)) {
        shell->left_tank_hitbox = true;
    }

    collide_dir = process_reflection(&shell->phys, false);

    if(collide_dir) {
        shell_ricochet(shell, collide_dir);
    }
}

bool shell_ricochet(shell_t *shell, direction_t dir) {
    if(!shell->bounces) {
        shell->alive = false;
        return false;
    }

    //shell_t is still alive
    if(dir & UP || dir & DOWN) {
        shell->phys.velocity_y *= -1;
        update_shell_direction(shell);
        shell->bounces--;
    }
    if(dir & LEFT || dir & RIGHT) {
        shell->phys.velocity_x *= -1;
        update_shell_direction(shell);
        shell->bounces--;
    }

    return true;
}

void update_shell_direction(shell_t *shell) {
    angle_t angle = fast_atan2(shell->phys.velocity_y, shell->phys.velocity_x);
    shell->direction = angle_to_shell_direction(angle);
}
