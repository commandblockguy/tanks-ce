/*
 *--------------------------------------
 * Program Name: Tanks! CE
 * Author: commandblockguy
 * License:
 * Description: Tanks! from Wii Play
 *--------------------------------------
*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEVEL_SIZE_X 22
#define LEVEL_SIZE_Y 18

typedef enum {
	EMPTY, //Empty space
	BLOCK, //Block that can be neither shot nor moved through
	DESTRUCTIBLE, //Blocks that can be exploded with mines
	DESTROYED, //DESTRUCTIBLE blocks that have been removed using mines
	HOLE //Hole that can be shot over but not moved through
} Tile;

typedef enum {
	PLAYER, //blue
	IMMOBILE, //gray
	BASIC, //brown
	MISSILE, //green
	MINE, //yellow
	RED,
	IMMOB_MISSILE, //turquoise
	FAST, //purple
	INVISIBLE, //white
	BLACK
} TankType;

typedef struct {
	//A tank as stored in the level file
	TankType type;
	uint8_t start_x; //Tile the tank starts on
	uint8_t start_y;
} SerializedTank;

typedef struct {
	Tile tiles[LEVEL_SIZE_X * LEVEL_SIZE_Y]; //Array of all tiles
	uint8_t num_tanks; //Number of tanks in the level
} Level;

typedef struct {
	uint16_t pos_x; //Pixel of the top-left pixel of the sprite and AABB
	uint8_t pos_y;
	uint8_t vel_x; //Velocity of the bullets (units?)
	uint8_t vel_y;
	uint8_t bounces; //Number of times the bullet can bounce off a wall without exploding
} Shell;

typedef struct {
	uint8_t pos_x; //Tile the mine is occupying (should this be in pixels?)
	uint8_t pos_y;
	uint8_t countdown; //Number of physics loops until explosions occur
} Mine;

typedef struct {
	//A tank, used while gameplay occurs
	TankType type;
	bool alive; //Whether this tank is alive or exploded.
	uint16_t pos_x; //Pixel of the top-left pixel of the sprite and AABB
	uint8_t pos_y;
	uint8_t start_x; //Tile the tank starts on
	uint8_t start_y;
	uint8_t tread_rot; //Rotation of tank treads. Determines the direction of the tank.
	uint8_t barrel_rot; //Rotation of the barrel. Determines the direction shots are fired in;
	Shell shells[5]; //Shells that belong to this tank. Players can shoot up to 5, and each type of tank is limited to a different number.
	Mine mines[2]; //Mines that belong to this tank. Players and some tanks can lay up to two.
} Tank;

typedef struct {
	uint8_t mission; //The mission number, always displayed 1 higher than stored. Also used as an index for levels.
	uint8_t lives; //Number of remaining tanks. This includes the tank that is currently in use, so a value of 1 means that the game will end the next time the tank is hit.
	uint8_t kills; //Number of enemy tanks destroyed.
	uint24_t timer; //Game time, probably used for physics stuff.
	uint16_t cursor_x; //If I decide to implement a cursor mode, this will represent the position of the crosshairs on the screen.
	uint8_t cursor_y;
} Game;
Game game; //Game global, so I can reuse those names elsewhere

Level levels[]; //Array of all the levels, possibly loaded from an appvar.
Level* level; //Pointer to the level currently being played.
//Should I modify this while the game is running, or use a seperate method to determine whether tiles have been destroyed?
Tank tanks[]; //List of all active tanks. 

void main(void) {
    //Remember to implement this function.
}


