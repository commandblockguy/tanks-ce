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

typedef enum {
	PLAYER = 0, //blue
	IMMOBILE, //brown
	BASIC, //grey
	MISSILE, //turquoise
	MINE, //yellow
	RED,
	IMMOB_MISSILE, //green
	FAST, //purple
	INVISIBLE, //white
	BLACK
} TankType;

typedef struct {
	bool alive; //Whether this bullet is processed
	float pos_x; //Pixel of the top-left pixel of the sprite and AABB
	float pos_y;
	float vel_x; //Velocity of the bullets, in pixels per tick
	float vel_y;
	uint8_t bounces; //Number of times the bullet can bounce off a wall without exploding
	bool left_tank_hitbox; //Whether the shell has exited the tank hitbox yet. Used to stop bullets from blowing up the tank that fired them.
} Shell;

//Numer of shots each type of tank can have on-screen at any one time
extern const uint8_t max_shells[];

typedef struct {
	bool alive; //Whether this mine is processed
	uint16_t pos_x; //Top left pixel of the mine
	uint8_t pos_y;
	uint8_t countdown; //Number of physics loops until explosions occur
} Mine;

//Numer of mines each type of tank can have on-screen at any one time
extern const uint8_t max_mines[];

typedef struct {
	//A tank, used while gameplay occurs
	TankType type;
	bool alive; //Whether this tank is alive or exploded.
	float pos_x; //Pixel of the top-left pixel of the sprite and AABB
	float pos_y;
	uint8_t start_x; //Tile the tank starts on
	uint8_t start_y;
	uint8_t tread_rot; //Rotation of tank treads. Determines the direction of the tank.
	uint8_t barrel_rot; //Rotation of the barrel. Determines the direction shots are fired in
	int8_t bullet_spawn_x; //Position relative to center that bullets will spawn from
	int8_t bullet_spawn_y;
	Shell shells[5]; //Shells that belong to this tank. Players can shoot up to 5, and each type of tank is limited to a different number.
	Mine mines[4]; //Mines that belong to this tank. Players and some tanks can lay up to two.
} Tank;

uint8_t fire_shell(Tank* tank); //PEW PEW PEW

uint8_t lay_mine(Tank* tank); //Lay a mine under the tank

void calc_bullet_spawn(Tank* tank);

#endif /* H_OBJECTS */
