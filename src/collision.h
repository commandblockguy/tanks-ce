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
	fix_t x1;
	fix_t y1;
	fix_t x2;
	fix_t y2;
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
    //Axis-aligned bounding box
    uint24_t x1;
    uint24_t x2; 
    uint8_t y1;
    uint8_t y2;
} AABB;

typedef uint24_t ufix_t;
typedef int24_t fix_t;

typedef struct {
	PhysicsType type;
	ufix_t position_x; //this is ufix, but gives compiler errors for some reason
	ufix_t position_y;
	fix_t velocity_x;
	fix_t velocity_y;
	uint8_t width;
	uint8_t height;
	uint32_t updateTime;
} PhysicsBody;

AABB getAABB(PhysicsBody* phys);
AABB getBlockAABB(uint8_t x, uint8_t y); //Get the AABB of a tile

ufix_t center_x(PhysicsBody* p); //Get the center coords of a AABB
ufix_t center_y(PhysicsBody* p);

//Determine if two bounding boxes are intersecting
bool detectCollision(PhysicsBody* p1, PhysicsBody* p2); 

//Check if a point is inside a bounding box
bool pointInsideBody(PhysicsBody* p, ufix_t x, ufix_t y);

//Determine if a collision occurs with the tilemap
struct reflection getTileReflect(PhysicsBody* state2, bool respectHoles, uint8_t* tiles);

//if colliding, push bodies an equal distance apart and return true
bool collideAndPush(PhysicsBody* p1, PhysicsBody* p2);

bool center_distance_lt(PhysicsBody* p1, PhysicsBody* p2, ufix_t dis);

bool seg_collides_bb(LineSeg* ls, PhysicsBody* phys);
bool seg_collides_seg(LineSeg* line1, LineSeg* line2, fix_t* i_x, fix_t* i_y);

fix_t y_intercept(LineSeg* line, fix_t xPos);
fix_t x_intercept(LineSeg* line, fix_t yPos);

#endif /* H_COLLISION */
