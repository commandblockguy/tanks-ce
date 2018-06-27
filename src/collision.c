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

#include <graphx.h>

AABB getAABB(PhysicsBody* phys) {
	AABB result;
	result.x1 = phys->position_x >> SHIFT_AMOUNT;
	result.y1 = phys->position_y >> SHIFT_AMOUNT;
	result.x2 = (phys->position_x >> SHIFT_AMOUNT) + phys->width;
	result.y2 = (phys->position_y >> SHIFT_AMOUNT) + phys->height;
	return result;
}

AABB getDetectionAABB(Mine* mine) {
	AABB result;

	uint24_t center_x = (mine->phys.position_x >> SHIFT_AMOUNT) + MINE_SIZE / 2;
	uint24_t center_y = (mine->phys.position_y >> SHIFT_AMOUNT) + MINE_SIZE / 2;

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

uint24_t center_x(AABB bb) {
	return (bb.x1 + bb.x2) / 2;
}

uint8_t center_y(AABB bb) {
	return (bb.y1 + bb.y2) / 2;
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

bool center_distance(AABB bb1, AABB bb2) {
	int24_t delta_x = center_x(bb1) - center_x(bb2);
	int24_t delta_y  = center_y(bb1) - center_y(bb2);
	return sqrt(delta_x * delta_x + delta_y * delta_y);
}

struct reflection getTileReflect(PhysicsBody* state1, PhysicsBody* state2, bool respectHoles, uint8_t* tiles) {
	struct reflection result = {false};
	uint8_t initial_corners[] = {0,0,0,0};
	uint8_t final_corners[] = {0,0,0,0};
	AABB bb1 = getAABB(state1);
	AABB bb2 = getAABB(state2);

	int x, y;

	//Loop through the 4 corners
	for(x = 0; x < 2; x++) {
		for(y = 0; y < 2; y++) {
			uint24_t final_corner_x = (&bb2.x1)[x]; //I'm pretty sure this is poor practice.
			uint8_t  final_corner_y = (&bb2.y1)[y];
			//The ID of the tile that the corner is occupying
			uint8_t final_tile = tiles[pixelToXTile(final_corner_x) + LEVEL_SIZE_X * pixelToYTile(final_corner_y)];
			//Check if we need to collide here
			if(final_tile == BLOCK || final_tile == DESTRUCTIBLE || (respectHoles && final_tile == HOLE)) {
				//We have collided
				result.colliding = true;
				//Determine direction by seeing which axis has changed tiles
				//This means if it hits a corner it will always prefer to reflect on the X axis
				//(don't tell anyone though)
				if(pixelToXTile(final_corner_x) > pixelToXTile((&bb1.x1)[x])) {
					result.dir = RIGHT;
				} else if(pixelToXTile(final_corner_x) < pixelToXTile((&bb1.x1)[x])) {
					result.dir = LEFT;
				} else if(pixelToYTile(final_corner_y) > pixelToYTile((&bb1.y1)[y])) {
					result.dir = DOWN;
				} else if(pixelToYTile(final_corner_y) < pixelToYTile((&bb1.y1)[y])) {
					result.dir = UP;
				} else {
					//This should be unreachable.
					result.dir = 0;
				}
			}
		}
	}
	return result;
}
