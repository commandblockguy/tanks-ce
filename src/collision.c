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
#include "debug.h"

#include <graphx.h>

AABB getAABB(PhysicsBody* phys) {
	AABB result;
	result.x1 = from_ufix(phys->position_x);
	result.y1 = from_ufix(phys->position_y);
	result.x2 = from_ufix(phys->position_x) + phys->width;
	result.y2 = from_ufix(phys->position_y) + phys->height;
	return result;
}

AABB getDetectionAABB(Mine* mine) {
	AABB result;

	uint24_t center_x = from_ufix(mine->phys.position_x) + MINE_SIZE / 2;
	uint24_t center_y = from_ufix(mine->phys.position_y) + MINE_SIZE / 2;

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

ufix_t center_x(PhysicsBody* p) {
	return p->position_x + to_ufix(p->width / 2);
}

ufix_t center_y(PhysicsBody* p) {
	return p->position_y + to_ufix(p->height / 2);
}

bool detectCollision(PhysicsBody *p1, PhysicsBody *p2) {
	return
		p1->position_x < p2->position_x + to_ufix(p2->width) &&
		p1->position_x + to_ufix(p1->width) > p2->position_x &&
		p1->position_y < p2->position_y + to_ufix(p2->height) &&
		p1->position_y + to_ufix(p1->height) > p2->position_y;
}

bool center_distance_lt(PhysicsBody* p1, PhysicsBody* p2, ufix_t dis) {
	int24_t delta_x;
	int24_t delta_y;

	if(abs((fix_t)center_x(p1) - (fix_t)center_x(p2)) > dis) return false;
	if(abs((fix_t)center_y(p1) - (fix_t)center_y(p2)) > dis) return false;

	delta_x = from_ufix(center_x(p1) - center_x(p2));
	delta_y = from_ufix(center_y(p1) - center_y(p2));

	return sqrt(delta_x * delta_x + delta_y * delta_y) < from_ufix(dis);
}

//Check if a point is colliding with a tile
bool checkTileCollision(ufix_t x, ufix_t y, bool respectHoles, tile_t* tiles) {
	tile_t tile = tiles[ptToXTile(x) + LEVEL_SIZE_X * ptToYTile(y)];
	return tile == BLOCK || tile == DESTRUCTIBLE || (respectHoles && tile == HOLE);
}

struct reflection getTileReflect(PhysicsBody* p, bool respectHoles, uint8_t* tiles) {
	struct reflection result = {false};

	//Figure out if the four corners are colliding
	bool    topRight = checkTileCollision(p->position_x + to_ufix(p->width), p->position_y, respectHoles, tiles);
	bool bottomRight = checkTileCollision(p->position_x + to_ufix(p->width), p->position_y + to_ufix(p->height), respectHoles, tiles);
	bool    topLeft  = checkTileCollision(p->position_x, p->position_y, respectHoles, tiles);
	bool bottomLeft  = checkTileCollision(p->position_x, p->position_y + to_ufix(p->height), respectHoles, tiles);

	bool double_x = (bottomLeft && topLeft) || (topRight && bottomRight);
	bool double_y = (topRight && topLeft) || (bottomRight && bottomLeft);

	ufix_t dis_up    = -1;
	ufix_t dis_down  = -1;
	ufix_t dis_left  = -1;
	ufix_t dis_right = -1;

	result.colliding = (topRight || bottomRight || topLeft || bottomLeft);

	result.dir = 0;

	if(!result.colliding) return result;

	if((topRight || bottomRight) && !double_y) {
		dis_right = p->position_x + to_ufix(p->width) - tileToXPt(ptToXTile(p->position_x + from_ufix(p->width)) + 1);
	}
	if((topLeft || bottomLeft) && !double_y) {
		dis_left = tileToXPt(ptToXTile(p->position_x) + 1) - p->position_x;
	}
	if((topLeft || topRight) && !double_x) {
		dis_up = tileToYPt(ptToYTile(p->position_y) + 1) - p->position_y;
	}
	if((bottomLeft || bottomRight) && !double_x) {
		dis_down = p->position_y + to_ufix(p->height) - tileToYPt(ptToYTile(p->position_y + from_ufix(p->height)) + 1);
	}

	//pick the direction with the smallest distance
	if(dis_up <= dis_left && dis_up <= dis_right) {
		result.dir = UP;
		p->position_y += dis_up;
	}
	if(dis_left < dis_up && dis_left < dis_down) {
		result.dir = LEFT;
		p->position_x += dis_left;
	}
	if(dis_down <= dis_left && dis_down <= dis_right) {
		result.dir = DOWN;
		p->position_y -= dis_down;
	}
	if(dis_right < dis_up && dis_right < dis_down) {
		result.dir = RIGHT;
		p->position_x -= dis_right;
	}

	return result;
}

bool pointInsideBody(PhysicsBody* p, ufix_t x, ufix_t y) {
	return
		p->position_x <= x &&
		p->position_y <= y &&
		p->position_x + to_ufix(p->width) >= x &&
		p->position_y + to_ufix(p->height) >= y;
}

bool collideAndPush(PhysicsBody* p1, PhysicsBody* p2) {
	//Figure out if the four corners are colliding
	bool    topRight = pointInsideBody(p2, p1->position_x + to_ufix(p1->width), p1->position_y);
	bool bottomRight = pointInsideBody(p2, p1->position_x + to_ufix(p1->width), p1->position_y + to_ufix(p1->height));
	bool    topLeft  = pointInsideBody(p2, p1->position_x, p1->position_y);
	bool bottomLeft  = pointInsideBody(p2, p1->position_x, p1->position_y + to_ufix(p1->height));

	ufix_t dis_up    = -1;
	ufix_t dis_down  = -1;
	ufix_t dis_left  = -1;
	ufix_t dis_right = -1;

	if(!(topRight || bottomRight || topLeft || bottomLeft)) return false;

	if((topRight || bottomRight)) {
		dis_right = p1->position_x + to_ufix(p1->width) - p2->position_x;
	}
	if((topLeft || bottomLeft)) {
		dis_left = p2->position_x + to_ufix(p2->width) - p1->position_x;
	}
	if((topLeft || topRight)) {
		dis_up = p2->position_y + to_ufix(p2->height) - p1->position_y;
	}
	if((bottomLeft || bottomRight)) {
		dis_down = p1->position_y + to_ufix(p1->height) - p2->position_y;
	}

	//pick the direction with the smallest distance
	if(dis_up < dis_left && dis_up < dis_right) {
		p1->position_y += dis_up / 2;
		p2->position_y -= dis_up / 2;
	}
	if(dis_left < dis_up && dis_left < dis_down) {
		p1->position_x += dis_left / 2;
		p2->position_x -= dis_left / 2;
	}
	if(dis_down < dis_left && dis_down < dis_right) {
		p1->position_y -= dis_down / 2;
		p2->position_y += dis_down / 2;
	}
	if(dis_right < dis_up && dis_right < dis_down) {
		p1->position_x -= dis_right / 2;
		p2->position_x += dis_right / 2;
	}

	return true;
}
