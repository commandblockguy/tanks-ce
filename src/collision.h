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

#include "constants.h"
#include "util.h"

typedef struct {
	int24_t x1;
    int24_t y1;
    int24_t x2;
    int24_t y2;
} LineSeg;

typedef enum {
	UP    = 1,
	DOWN  = 2,
	LEFT  = 4,
	RIGHT = 8
} Direction;

enum {
	TankPhysics,
	ShellPhysics,
	MinePhysics
};

typedef uint8_t PhysicsType;

struct reflection {
	bool colliding;
	Direction dir;
};

typedef struct {
	PhysicsType type;
	uint24_t position_x; //this is ufix, but gives compiler errors for some reason
	uint24_t position_y;
	uint24_t velocity_x;
	uint24_t velocity_y;
	uint24_t width;
	uint24_t height;
	uint32_t updateTime;
} PhysicsBody;

uint24_t center_x(PhysicsBody* p); //Get the center coords of a AABB
uint24_t center_y(PhysicsBody* p);

//Determine if two bounding boxes are intersecting
bool detectCollision(PhysicsBody* p1, PhysicsBody* p2); 

//Check if a point is inside a bounding box
bool pointInsideBody(PhysicsBody* p, uint24_t x, uint24_t y);

//Determine if a collision occurs with the tilemap
void processReflection(struct reflection *result, PhysicsBody *p, bool respectHoles);

//if colliding, push bodies an equal distance apart and return true
bool collideAndPush(PhysicsBody* p1, PhysicsBody* p2);

bool center_distance_lt(PhysicsBody* p1, PhysicsBody* p2, uint24_t dis);

bool seg_collides_bb(LineSeg* ls, PhysicsBody* phys);
bool seg_collides_seg(LineSeg* l1, LineSeg* l2, int24_t* intercept_x, int24_t* intercept_y);

int24_t y_intercept(LineSeg* line, int24_t xPos);
int24_t x_intercept(LineSeg* line, int24_t yPos);

#endif /* H_COLLISION */
