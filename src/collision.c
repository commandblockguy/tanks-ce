#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "collision.h"
#include "level.h"
#include "util.h"
#include "globals.h"

#undef NDEBUG
#include <debug.h>

uint24_t centerX(physicsBody_t* p) {
	return p->position_x + p->width / 2;
}

uint24_t centerY(physicsBody_t* p) {
	return p->position_y + p->height / 2;
}

bool detectCollision(physicsBody_t *p1, physicsBody_t *p2) {
	return
		p1->position_x < p2->position_x + p2->width &&
		p1->position_x + p1->width > p2->position_x &&
		p1->position_y < p2->position_y + p2->height &&
		p1->position_y + p1->height > p2->position_y;
}

bool centerDistanceLessThan(physicsBody_t* p1, physicsBody_t* p2, uint24_t dis) {
	int24_t delta_x;
	int24_t delta_y;

	if(abs((int24_t) centerX(p1) - (int24_t) centerX(p2)) > dis) return false;
	if(abs((int24_t) centerY(p1) - (int24_t) centerY(p2)) > dis) return false;

	delta_x = (centerX(p1) - centerX(p2)) >> 8;
	delta_y = (centerY(p1) - centerY(p2)) >> 8;

	return sqrt(delta_x * delta_x + delta_y * delta_y) < dis >> 8;
}

//Check if a point is colliding with a tile
bool checkTileCollision(uint24_t x, uint24_t y, bool respectHoles) {
	tile_t tile = tiles[ptToXTile(x) + LEVEL_SIZE_X * ptToYTile(y)];
	bool colliding = (TILE_HEIGHT(tile) && TILE_TYPE(tile) != DESTROYED) || (respectHoles && TILE_TYPE(tile) == HOLE);
	return colliding;
}

//TODO: if three corners are hit, move diagonally out
void processReflection(reflection_t *result, physicsBody_t *p, bool respectHoles) {

	//Figure out if the four corners are colliding
	bool	topRight = checkTileCollision(p->position_x + p->width,
                                          p->position_y, respectHoles);
	bool bottomRight = checkTileCollision(p->position_x + p->width,
                                          p->position_y + p->height,
                                          respectHoles);
	bool	topLeft  = checkTileCollision(p->position_x, p->position_y,
                                          respectHoles);
	bool bottomLeft  = checkTileCollision(p->position_x,
                                          p->position_y + p->height,
                                          respectHoles);

	bool double_x;
	bool double_y;

    if(p->position_y < 0) topRight =    topLeft = true;
    if(p->position_x < 0) topLeft  = bottomLeft = true;
    if(p->position_x + p->width > LEVEL_SIZE_X * TILE_SIZE)
        topRight = bottomRight = true;
    if(p->position_y + p->height > LEVEL_SIZE_Y * TILE_SIZE)
        bottomLeft = bottomRight = true;

	double_x = (bottomLeft && topLeft) || (topRight && bottomRight);
	double_y = (topRight && topLeft) || (bottomRight && bottomLeft);

	uint24_t disUp	   = -1;
	uint24_t disDown  = -1;
	uint24_t disLeft  = -1;
	uint24_t disRight = -1;

	result->colliding = (topRight || bottomRight || topLeft || bottomLeft);

	result->dir = 0;

	if(!result->colliding) return;

	if((topRight || bottomRight) && !double_y) {
        disRight = p->position_x + p->width - tileToXPt(ptToXTile(p->position_x + p->width));
	}
	if((topLeft || bottomLeft) && !double_y) {
        disLeft = tileToXPt(ptToXTile(p->position_x) + 1) - p->position_x;
	}
	if((topLeft || topRight) && !double_x) {
        disUp = tileToYPt(ptToYTile(p->position_y) + 1) - p->position_y;
	}
	if((bottomLeft || bottomRight) && !double_x) {
        disDown = p->position_y + p->height - tileToYPt(ptToYTile(p->position_y + p->height));
	}

	//pick the direction with the smallest distance
	if(disUp <= disLeft && disUp <= disRight) {
		result->dir = UP;
		p->position_y += disUp;
	}
	if(disLeft < disUp && disLeft < disDown) {
		result->dir = LEFT;
		p->position_x += disLeft;
	}
	if(disDown <= disLeft && disDown <= disRight) {
		result->dir = DOWN;
		p->position_y -= disDown;
	}
	if(disRight < disUp && disRight < disDown) {
		result->dir = RIGHT;
		p->position_x -= disRight;
	}
}

bool pointInsideBody(physicsBody_t* p, uint24_t x, uint24_t y) {
	return
		p->position_x <= x &&
		p->position_y <= y &&
		p->position_x + p->width >= x &&
		p->position_y + p->height >= y;
}

bool collideAndPush(physicsBody_t* p1, physicsBody_t* p2) {
	//Figure out if the four corners are colliding
	bool	topRight = pointInsideBody(p2, p1->position_x + p1->width, p1->position_y);
	bool bottomRight = pointInsideBody(p2, p1->position_x + p1->width, p1->position_y + p1->height);
	bool	topLeft  = pointInsideBody(p2, p1->position_x, p1->position_y);
	bool bottomLeft  = pointInsideBody(p2, p1->position_x, p1->position_y + p1->height);

	uint24_t disUp    = -1;
	uint24_t disDown  = -1;
	uint24_t disLeft  = -1;
	uint24_t disRight = -1;

	if(!(topRight || bottomRight || topLeft || bottomLeft)) return false;

	if((topRight || bottomRight)) {
        disRight = p1->position_x + p1->width - p2->position_x;
	}
	if((topLeft || bottomLeft)) {
        disLeft = p2->position_x + p2->width - p1->position_x;
	}
	if((topLeft || topRight)) {
        disUp = p2->position_y + p2->height - p1->position_y;
	}
	if((bottomLeft || bottomRight)) {
        disDown = p1->position_y + p1->height - p2->position_y;
	}

	//pick the direction with the smallest distance
	if(disUp < disLeft && disUp < disRight) {
		p1->position_y += disUp / 2;
		p2->position_y -= disUp / 2;
	}
	if(disLeft < disUp && disLeft < disDown) {
		p1->position_x += disLeft / 2;
		p2->position_x -= disLeft / 2;
	}
	if(disDown < disLeft && disDown < disRight) {
		p1->position_y -= disDown / 2;
		p2->position_y += disDown / 2;
	}
	if(disRight < disUp && disRight < disDown) {
		p1->position_x -= disRight / 2;
		p2->position_x += disRight / 2;
	}

	return true;
}

//todo: optimize?
bool segCollidesSeg(lineSeg_t* l1, lineSeg_t* l2, int24_t* intercept_x, int24_t* intercept_y) {
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
bool segCollidesBody(lineSeg_t* ls, physicsBody_t* phys) {
	lineSeg_t border;
	//top
	border.x1 = phys->position_x;
	border.x2 = phys->position_x + phys->width;
	border.y1 = phys->position_y;
	border.y2 = phys->position_y;
	if(segCollidesSeg(&border, ls, NULL, NULL)) return true;
	//bottom
	border.y1 += phys->height;
	border.y2 += phys->height;
	if(segCollidesSeg(&border, ls, NULL, NULL)) return true;
	//left
	border.x2 = phys->position_x;
	border.y1 = phys->position_y;
	if(segCollidesSeg(&border, ls, NULL, NULL)) return true;
	//right
	border.x1 += phys->width;
	border.x2 += phys->width;
	if(segCollidesSeg(&border, ls, NULL, NULL)) return true;
	return false;
}

int24_t yIntercept(lineSeg_t* line, int24_t xPos) {
	return line->y1 + (line->y2 - line->y1) * (xPos - line->x1) / (line->x2 - line->x1);
}

int24_t xIntercept(lineSeg_t* line, int24_t yPos) {
	return line->x1 + (line->x2 - line->x1) * (yPos - line->y1) / (line->y2 - line->y1);
}
