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

typedef enum {
	UP,
	DOWN,
	LEFT,
	RIGHT
} Direction;

struct reflection {
	bool colliding;
	uint8_t distance;
	Direction dir;
};

typedef struct {
    //Axis-aligned bounding box
    uint16_t x1;
    uint16_t x2; 
    uint8_t y1;
    uint8_t y2;
} AABB;

AABB getTankAABB(Tank* tank); //Get the AABB of a tank
AABB getShellAABB(Shell* shell); //Get the AABB of a shell
AABB getMineAABB(Mine* mine); //Get the AABB of a mine
AABB getDetectionAABB(Mine* mine); //Get the AABB that a mine uses to detect enemy tanks. A seperate circular deterction radius will be used if the AABB detects a tank.
AABB getBlockAABB(uint8_t x, uint8_t y); //Get the AABB of a tile

//Determine if two bounding boxes are intersecting
bool detectCollision(AABB bb1, AABB bb2); 

//Distance "into" one BB the other is.
//This shouldn't need to handle cases where one bounding box is fully inside the other because of the low speed of bullets and tanks.
struct reflection getReflection(AABB bb1, AABB bb2); 

#endif /* H_COLLISION */
