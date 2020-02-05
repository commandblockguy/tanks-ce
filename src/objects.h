#ifndef H_OBJECTS
#define H_OBJECTS

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "collision.h"
#include "ai_data.h"

enum {
	PLAYER = 0, //blue
	IMMOBILE = 1, //brown
	BASIC = 2, //grey
	MISSILE = 3, //turquoise
	MINE = 4, //yellow
	RED = 5,
	IMMOB_MISSILE = 6, //green
	FAST = 7, //purple
	INVISIBLE = 8, //white
	BLACK = 9
};

typedef uint8_t TankType;

typedef struct {
	bool alive; //Whether this bullet is processed
	PhysicsBody phys;
	uint8_t bounces; //Number of times the bullet can bounce off a wall without exploding
	bool left_tank_hitbox; //Whether the shell has exited the tank hitbox yet. Used to stop bullets from blowing up the tank that fired them.
} Shell;

//Numer of shots each type of tank can have on-screen at any one time
extern const uint8_t max_shells[];
//Number of times a shell can bounce off a wall
extern const uint8_t max_bounces[];

typedef struct {
	bool alive; //Whether this mine is processed
	PhysicsBody phys;
	uint24_t countdown; //Number of physics loops until explosions occur
} Mine;

//Numer of mines each type of tank can have on-screen at any one time
extern const uint8_t max_mines[];

typedef struct {
	//A tank, used while gameplay occurs
	TankType type;
	bool alive; //Whether this tank is alive or exploded.
	uint8_t start_x;
	uint8_t start_y;
	PhysicsBody phys;
	angle_t tread_rot; //Rotation of tank treads. Determines the direction of the tank.
	angle_t barrel_rot; //Rotation of the barrel. Determines the direction shots are fired in
	Shell shells[5]; //Shells that belong to this tank. Players can shoot up to 5, and each type of tank is limited to a different number.
	Mine mines[4]; //Mines that belong to this tank. Players and some tanks can lay up to two.
	union ai_move* ai_move;
	union ai_fire* ai_fire;
} Tank;

bool fire_shell(Tank* tank); //PEW PEW PEW

bool lay_mine(Tank* tank); //Lay a mine under the tank

void detonate(Mine* mine, uint8_t* tiles);

//Bounce a shell off a wall
//Returns whether or not the shell is still alive
bool shell_ricochet(Shell* shell, Direction dir);

//true if a tank has a slot for a bullet
bool canShoot(Tank* tank);

#endif /* H_OBJECTS */
