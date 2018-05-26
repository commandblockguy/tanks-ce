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
#include "level.h"

AABB getShellAABB(Shell* shell) {
	AABB result;
	result.x1 = shell->pos_x;
	result.y1 = shell->pos_y;
	result.x2 = result.x1 + SHELL_SIZE;
	result.y2 = result.y1 + SHELL_SIZE;
	return result;
}

AABB getMineAABB(Mine* mine) {
	AABB result;
	result.x1 = mine->pos_x;
	result.y1 = mine->pos_y;
	result.x2 = result.x1 + MINE_SIZE;
	result.y2 = result.y1 + MINE_SIZE;
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

AABB getDetectionAABB(Mine* mine) {
	AABB result;

	uint24_t center_x = mine->pos_x + MINE_SIZE / 2;
	uint24_t center_y = mine->pos_y + MINE_SIZE / 2;

	result.x1 = center_x - MINE_DETECT_RANGE;
	result.y1 = center_y - MINE_DETECT_RANGE;
	result.x2 = center_x + MINE_DETECT_RANGE;
	result.y2 = center_y + MINE_DETECT_RANGE;

	return result;
}

AABB getBlockAABB(uint8_t x, uint8_t y) {
	AABB result;
	result.x1 = tileToXPixel(x);
	result.y1 = tileToYPixel(y);
	result.x2 = tileToXPixel(x + 1) - 1;
	result.y2 = tileToYPixel(y + 1) - 1;
	return result;
}

bool detectCollision(AABB bb1, AABB bb2) {
	return
		bb1.x1 < bb2.x2 &&
		bb1.x2 > bb2.x1 &&
		bb1.y1 < bb2.y2 &&
		bb1.y2 > bb2.y1;
}

//TODO: this bit
struct reflection getReflection(AABB bb1, AABB bb2) {
	struct reflection value = {0};
	return value;
}
