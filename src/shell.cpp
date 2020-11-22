#include "shell.h"
#include "tank.h"
#include "mine.h"
#include "globals.h"

Shell::Shell() {
    this->width = SHELL_SIZE;
    this->height = SHELL_SIZE;
    this->respect_holes = false;
};

void Shell::process() {
    direction_t collide_dir;
    //Ignore dead shells
    if(!this->alive) return;
    //Add velocity
    this->position_x += this->velocity_x;
    this->position_y += this->velocity_y;

    if(this->left_tank_hitbox) {
        //This will eventually be part of the collision bit
        for(uint8_t j = 0; j < game.level.num_tanks; j++) {
            int i;

            for(i = Tank::max_mines[tanks[j].type] - 1; i >= 0; i--) {
                Mine *mine = &tanks[j].mines[i];
                if(mine->alive && detect_collision(mine)) {
                    this->alive = false;
                    mine->detonate();
                }
            }

            for(i = Tank::max_shells[tanks[j].type] - 1; i >= 0; i--) {
                Shell *shell2 = &tanks[j].shells[i];
                if(this != shell2 && shell2->alive && detect_collision(shell2)) {
                    this->alive = false;
                    shell2->alive = false;
                }
            }

            if(!tanks[j].alive) continue;

            if(detect_collision(&tanks[j])) {
                if(tanks[j].type != PLAYER) game.total_kills++;
                game.kills[tanks[j].type]++;
                tanks[j].alive = false;
                this->alive = false;
            }
        }
    } else if(!detect_collision(tank)) {
        this->left_tank_hitbox = true;
    }

    collide_dir = process_reflection();

    if(collide_dir) {
        ricochet(collide_dir);
    }
}

bool Shell::ricochet(direction_t dir) {
    if(!this->bounces) {
        this->alive = false;
        return false;
    }

    //shell_t is still alive
    if(dir & UP || dir & DOWN) {
        this->velocity_y *= -1;
        update_direction();
        this->bounces--;
    }
    if(dir & LEFT || dir & RIGHT) {
        this->velocity_x *= -1;
        update_direction();
        this->bounces--;
    }

    return true;
}

void Shell::update_direction() {
    angle_t angle = fast_atan2(this->velocity_y, this->velocity_x);
    this->direction = angle_to_shell_direction(angle);
}
