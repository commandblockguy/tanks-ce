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
#include "util.h"

#include <graphx.h>

#undef NDEBUG
#include <debug.h>

uint24_t center_x(PhysicsBody* p) {
	return p->position_x + p->width / 2;
}

uint24_t center_y(PhysicsBody* p) {
	return p->position_y + p->height / 2;
}

bool detectCollision(PhysicsBody *p1, PhysicsBody *p2) {
	return
		p1->position_x < p2->position_x + p2->width &&
		p1->position_x + p1->width > p2->position_x &&
		p1->position_y < p2->position_y + p2->height &&
		p1->position_y + p1->height > p2->position_y;
}

bool center_distance_lt(PhysicsBody* p1, PhysicsBody* p2, uint24_t dis) {
	int24_t delta_x;
	int24_t delta_y;

	if(abs((int24_t)center_x(p1) - (int24_t)center_x(p2)) > dis) return false;
	if(abs((int24_t)center_y(p1) - (int24_t)center_y(p2)) > dis) return false;

	delta_x = (center_x(p1) - center_x(p2)) >> 8;
	delta_y = (center_y(p1) - center_y(p2)) >> 8;

	return sqrt(delta_x * delta_x + delta_y * delta_y) < dis >> 8;
}

//Check if a point is colliding with a tile
bool checkTileCollision(uint24_t x, uint24_t y, bool respectHoles, tile_t* tiles) {
	tile_t tile = tiles[ptToXTile(x) + LEVEL_SIZE_X * ptToYTile(y)];
	bool colliding = (tileHeight(tile) && tileType(tile) != DESTROYED) || (respectHoles && tileType(tile) == HOLE);
	return colliding;
}

//TODO: if three corners are hit, move diagonally out
void processReflection(struct reflection *result, PhysicsBody *p, bool respectHoles) {

	//Figure out if the four corners are colliding
	bool	topRight = checkTileCollision(p->position_x + p->width, p->position_y, respectHoles, tiles);
	bool bottomRight = checkTileCollision(p->position_x + p->width, p->position_y + p->height, respectHoles, tiles);
	bool	topLeft  = checkTileCollision(p->position_x, p->position_y, respectHoles, tiles);
	bool bottomLeft  = checkTileCollision(p->position_x, p->position_y + p->height, respectHoles, tiles);

	bool double_x = (bottomLeft && topLeft) || (topRight && bottomRight);
	bool double_y = (topRight && topLeft) || (bottomRight && bottomLeft);

	uint24_t dis_up	   = -1;
	uint24_t dis_down  = -1;
	uint24_t dis_left  = -1;
	uint24_t dis_right = -1;

	result->colliding = (topRight || bottomRight || topLeft || bottomLeft);

	result->dir = 0;

	if(!result->colliding) return;

	if((topRight || bottomRight) && !double_y) {
		dis_right = p->position_x + p->width - tileToXPt(ptToXTile(p->position_x + p->width));
	}
	if((topLeft || bottomLeft) && !double_y) {
		dis_left = tileToXPt(ptToXTile(p->position_x) + 1) - p->position_x;
	}
	if((topLeft || topRight) && !double_x) {
		dis_up = tileToYPt(ptToYTile(p->position_y) + 1) - p->position_y;
	}
	if((bottomLeft || bottomRight) && !double_x) {
		dis_down = p->position_y + p->height - tileToYPt(ptToYTile(p->position_y + p->height));
	}

	//pick the direction with the smallest distance
	if(dis_up <= dis_left && dis_up <= dis_right) {
		result->dir = UP;
		p->position_y += dis_up;
	}
	if(dis_left < dis_up && dis_left < dis_down) {
		result->dir = LEFT;
		p->position_x += dis_left;
	}
	if(dis_down <= dis_left && dis_down <= dis_right) {
		result->dir = DOWN;
		p->position_y -= dis_down;
	}
	if(dis_right < dis_up && dis_right < dis_down) {
		result->dir = RIGHT;
		p->position_x -= dis_right;
	}
}

bool pointInsideBody(PhysicsBody* p, uint24_t x, uint24_t y) {
	return
		p->position_x <= x &&
		p->position_y <= y &&
		p->position_x + p->width >= x &&
		p->position_y + p->height >= y;
}

bool collideAndPush(PhysicsBody* p1, PhysicsBody* p2) {
	//Figure out if the four corners are colliding
	bool	topRight = pointInsideBody(p2, p1->position_x + p1->width, p1->position_y);
	bool bottomRight = pointInsideBody(p2, p1->position_x + p1->width, p1->position_y + p1->height);
	bool	topLeft  = pointInsideBody(p2, p1->position_x, p1->position_y);
	bool bottomLeft  = pointInsideBody(p2, p1->position_x, p1->position_y + p1->height);

	uint24_t dis_up    = -1;
	uint24_t dis_down  = -1;
	uint24_t dis_left  = -1;
	uint24_t dis_right = -1;

	if(!(topRight || bottomRight || topLeft || bottomLeft)) return false;

	if((topRight || bottomRight)) {
		dis_right = p1->position_x + p1->width - p2->position_x;
	}
	if((topLeft || bottomLeft)) {
		dis_left = p2->position_x + p2->width - p1->position_x;
	}
	if((topLeft || topRight)) {
		dis_up = p2->position_y + p2->height - p1->position_y;
	}
	if((bottomLeft || bottomRight)) {
		dis_down = p1->position_y + p1->height - p2->position_y;
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

//todo: optimize?
bool seg_collides_seg(LineSeg* l1, LineSeg* l2, int24_t* intercept_x, int24_t* intercept_y) {
	int24_t p0_x = l1->x1 >> 4, p1_x = l1->x2 >> 4, p2_x = l2->x1 >> 4, p3_x = l2->x2 >> 4;
	int24_t p0_y = l1->y1 >> 4, p1_y = l1->y2 >> 4, p2_y = l2->y1 >> 4, p3_y = l2->y2 >> 4;
	int24_t s1_x, s1_y, s2_x, s2_y;
	int24_t d, s, t;
	s1_x = p1_x - p0_x;
	s1_y = p1_y - p0_y;
	s2_x = p3_x - p2_x;
	s2_y = p3_y - p2_y;

	d = -s2_x * s1_y + s1_x * s2_y;
	s = -s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y);
	t =  s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x);

	if ((s >= 0) == (d >= 0) && abs(s) <= abs(d) && (t >= 0) == (d >= 0) && abs(t) <= abs(d)) {
		//  Collision detected
		if (intercept_x != NULL)
			*intercept_x = p0_x + ((float)t * s1_x / d);
		if (intercept_y != NULL)
			*intercept_y = p0_y + ((float)t * s1_y / d);
		return 1;
	}

	return 0; // No collision
}

//TODO: optimize
bool seg_collides_bb(LineSeg* ls, PhysicsBody* phys) {
	LineSeg border;
	//top
	border.x1 = phys->position_x;
	border.x2 = phys->position_x + phys->width;
	border.y1 = phys->position_y;
	border.y2 = phys->position_y;
	if(seg_collides_seg(&border, ls, NULL, NULL)) return true;
	//bottom
	border.y1 += phys->height;
	border.y2 += phys->height;
	if(seg_collides_seg(&border, ls, NULL, NULL)) return true;
	//left
	border.x2 = phys->position_x;
	border.y1 = phys->position_y;
	if(seg_collides_seg(&border, ls, NULL, NULL)) return true;
	//right
	border.x1 += phys->width;
	border.x2 += phys->width;
	if(seg_collides_seg(&border, ls, NULL, NULL)) return true;
	return false;
}

int24_t y_intercept(LineSeg* line, int24_t xPos) {
	return line->y1 + (line->y2 - line->y1) * (xPos - line->x1) / (line->x2 - line->x1);
}

int24_t x_intercept(LineSeg* line, int24_t yPos) {
	return line->x1 + (line->x2 - line->x1) * (yPos - line->y1) / (line->y2 - line->y1);
}
