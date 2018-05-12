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
#include "collision.h"

AABB getShellAABB(Shell* shell) {
	AABB result;
	result.x1 = shell->pos_x;
	result.y1 = shell->pos_y;
	result.x2 = result.x1 + SHELL_SIZE;
	result.y2 = result.y1 + SHELL_SIZE;
	return result;
}

AABB getTankAABB(Tank* tank) {
	AABB result;
	result.x1 = tank->pos_x;
	result.y1 = tank->pos_y;
	result.x2 = tank->pos_x + TANK_SIZE;
	result.y2 = tank->pos_y + TANK_SIZE;
	return result;
}
