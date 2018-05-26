#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "objects.h"

const uint8_t max_shells[] = {5, 1, 1, 1, 1, 3, 2, 5, 5, 2};
const uint8_t max_mines[] = {2, 0, 0, 0, 4, 0, 0, 2, 2, 2};

bool fire_shell(Tank* tank) {
	int i;
	for(i = max_shells[tank->type] - 1; i >= 0; i--) {
		Shell* shell = &tank->shells[i];
		float vector_x, vector_y;
		if(shell->alive) continue;
		shell->alive = true;
		vector_x = cos(tank->barrel_rot * ROT_UNITS_TO_RADIANS);
		vector_y = sin(tank->barrel_rot * ROT_UNITS_TO_RADIANS);
		shell->pos_x = tank->pos_x + TANK_SIZE / 2 + BARREL_LENGTH * vector_x;
		shell->pos_y = tank->pos_y + TANK_SIZE / 2 + BARREL_LENGTH * vector_y;
		if(tank->type == MISSILE || tank->type == IMMOB_MISSILE) {
			shell->vel_x = SHELL_SPEED_MISSILE * vector_x;
			shell->vel_y = SHELL_SPEED_MISSILE * vector_y;
		} else {
			shell->vel_x = SHELL_SPEED_STANDARD * vector_x;
			shell->vel_y = SHELL_SPEED_STANDARD * vector_y;
		}
		return true;
	}
	return false;
}

bool lay_mine(Tank* tank) {
	int i;
	if(!max_mines[tank->type]) return false;
	for(i = max_mines[tank->type] - 1; i >= 0; i--) {
		Mine* mine = &tank->mines[i];
		if(mine->alive) continue;
		mine->alive = true;
		mine->countdown = MINE_COUNTDOWN;
		mine->pos_x = tank->pos_x + (TANK_SIZE - MINE_SIZE) / 2;
		mine->pos_y = tank->pos_y + (TANK_SIZE - MINE_SIZE) / 2;
		return true;
	}
	return false;
}

void calc_bullet_spawn(Tank* tank) {
	tank->bullet_spawn_x = BARREL_LENGTH * cos(tank->barrel_rot * ROT_UNITS_TO_RADIANS);
	tank->bullet_spawn_y = BARREL_LENGTH * sin(tank->barrel_rot * ROT_UNITS_TO_RADIANS);
}
