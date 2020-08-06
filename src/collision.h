#ifndef H_COLLISION
#define H_COLLISION

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

typedef struct {
	int24_t x1;
    int24_t y1;
    int24_t x2;
    int24_t y2;
} lineSeg_t;

typedef enum {
	UP    = 1,
	DOWN  = 2,
	LEFT  = 4,
	RIGHT = 8
} direction_t;

enum {
	TankPhysics,
	ShellPhysics,
	MinePhysics
};

typedef uint8_t physicsType_t;

typedef struct {
	physicsType_t type;
	int24_t position_x;
	int24_t position_y;
	int24_t velocity_x;
	int24_t velocity_y;
	uint24_t width;
	uint24_t height;
	uint32_t updateTime;
} physicsBody_t;

uint24_t centerX(const physicsBody_t *p); //Get the center coords of a AABB
uint24_t centerY(const physicsBody_t *p);

//Determine if two bounding boxes are intersecting
bool detectCollision(physicsBody_t* p1, physicsBody_t* p2);

//Check if a point is inside a bounding box
bool pointInsideBody(physicsBody_t* p, uint24_t x, uint24_t y);

//Determine if a collision occurs with the tilemap
direction_t processReflection(physicsBody_t *p, bool respectHoles);

//if colliding, push bodies an equal distance apart and return true
bool collideAndPush(physicsBody_t* p1, physicsBody_t* p2);

bool centerDistanceLessThan(physicsBody_t* p1, physicsBody_t* p2, uint24_t dis);

bool segCollidesBody(lineSeg_t* ls, physicsBody_t* phys);
bool segCollidesSeg(lineSeg_t* l1, lineSeg_t* l2, int24_t* intercept_x, int24_t* intercept_y);

int24_t yIntercept(lineSeg_t* line, int24_t xPos);
int24_t xIntercept(lineSeg_t* line, int24_t yPos);

#endif /* H_COLLISION */
