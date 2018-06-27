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
	uint8_t distance;
	Direction dir;
};

typedef struct {
    //Axis-aligned bounding box
    uint24_t x1;
    uint24_t x2; 
    uint8_t y1;
    uint8_t y2;
} AABB;

typedef struct {
	PhysicsType type;
	uint8_t rotation;
	uint16_t position_x; //this is ufix, but gives compiler errors for some reason
	uint16_t position_y;
	float velocity_x;
	float velocity_y;
	uint8_t width;
	uint8_t height;
	uint32_t updateTime;
} PhysicsBody;

AABB getAABB(PhysicsBody* phys);
AABB getBlockAABB(uint8_t x, uint8_t y); //Get the AABB of a tile

uint24_t center_x(AABB bb); //Get the center coords of a AABB
uint8_t center_y(AABB bb);

//Determine if two bounding boxes are intersecting
bool detectCollision(AABB bb1, AABB bb2); 

//Determine if a collision occurs with the tilemap
struct reflection getTileReflect(PhysicsBody* state1, PhysicsBody* state2, bool respectHoles, uint8_t* tiles);

//This shouldn't need to handle cases where one bounding box is fully inside the other because of the low speed of bullets and tanks.
struct reflection getReflection(AABB bb1, AABB bb2); 

bool center_distance(AABB bb1, AABB bb2);

#endif /* H_COLLISION */
