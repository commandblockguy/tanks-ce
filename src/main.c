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

//Size of the gameplay area in tiles
#define LEVEL_SIZE_X 22
#define LEVEL_SIZE_Y 18

//Pixel size of each square tile
#define TILE_SIZE 13

#define MAP_OFFSET_X 17 //Offset from sides of screen
#define MAP_OFFSET_BOTTOM 14 //Space under the map left for UI elements

//Speed of bullets (units?)
#define SHELL_SPEED_STANDARD 1
#define SHELL_SPEED_MISSILE 1

//Target frame rate
#define TARGET_FPS 60

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
	char name[15];
	uint8_t num_levels;
	uint8_t scores[5];
} LevelPack;

typedef struct {
	bool alive; //Whether this bullet is processed
	float pos_x; //Pixel of the top-left pixel of the sprite and AABB
	float pos_y;
	uint8_t vel_x; //Velocity of the bullets (units?)
	uint8_t vel_y;
	uint8_t bounces; //Number of times the bullet can bounce off a wall without exploding
} Shell;

typedef struct {
	bool alive; //Whether this mine is processed
	uint8_t pos_x; //Tile the mine is occupying (should this be in pixels?)
	uint8_t pos_y;
	uint8_t countdown; //Number of physics loops until explosions occur
} Mine;

typedef struct {
	//A tank, used while gameplay occurs
	TankType type;
	bool alive; //Whether this tank is alive or exploded.
	float pos_x; //Pixel of the top-left pixel of the sprite and AABB
	float pos_y;
	uint8_t start_x; //Tile the tank starts on
	uint8_t start_y;
	uint8_t tread_rot; //Rotation of tank treads. Determines the direction of the tank.
	uint8_t barrel_rot; //Rotation of the barrel. Determines the direction shots are fired in;
	Shell shells[5]; //Shells that belong to this tank. Players can shoot up to 5, and each type of tank is limited to a different number.
	Mine mines[2]; //Mines that belong to this tank. Players and some tanks can lay up to two.
} Tank;

typedef struct {
    //Axis-aligned bounding box
    uint16_t x1;
    uint16_t x2; 
    uint8_t y1;
    uint8_t y2;
} AABB;

typedef struct {
	uint8_t mission; //The mission number, always displayed 1 higher than stored. Also used as an index for levels.
	uint8_t lives; //Number of remaining tanks. This includes the tank that is currently in use, so a value of 1 means that the game will end the next time the tank is hit.
	uint8_t kills; //Number of enemy tanks destroyed.
	uint24_t timer; //Game time, probably used for physics stuff.
	uint16_t cursor_x; //If I decide to implement a cursor mode, this will represent the position of the crosshairs on the screen.
	uint8_t cursor_y;
	uint24_t lastCycle; //Time the last physics cycle started
} Game;

void createLevels(void); //Temporary function to make a level pack

void displayScores(void); //Display high scores

void restartMission(); //Restart a mission.

void missionStart(uint8_t mission, uint8_t lives, uint8_t num_tanks); //Display the mission start screen

void render(void); //Render tilemap, tanks, and UI

void displayUI(void); //Display UI during a mission

void processCollisions(void); //Process collisions between all entities that could reasonably intersect.

uint16_t tileXPixel(uint8_t tile_x);
uint8_t tileYPixel(uint8_t tile_y);

AABB getTankAABB(Tank* tank); //Get the AABB of a tank
AABB getShellAABB(Shell* shell); //Get the AABB of a shell
AABB getMineAABB(Mine* mine); //Get the AABB of a mine
AABB getDetectionAABB(Mine* mine); //Get the AABB that a mine uses to detect enemy tanks. A seperate circular deterction radius will be used if the AABB detects a tank.
AABB getBlockAABB(uint8_t x, uint8_t y); //Get the AABB of a tile

//Determine if two bounding boxes are intersecting
bool detectCollision(AABB bb1, AABB bb2); 

//Distance "into" one BB the other is.
//This shouldn't need to handle cases where one bounding box is fully inside the other because of the low speed of bullets and tanks.
uint8_t reflectionDistance(AABB bb1, AABB bb2); 

Game game; //Game global, so I can reuse those names elsewhere

Level* levels; //Array of all the levels, possibly loaded from an appvar.
Level* level; //Pointer to the level currently being played.
//Should I modify this while the game is running, or use a seperate method to determine whether tiles have been destroyed?
Tank* tanks; //List of all active tanks. 

void main(void) {
	LevelPack lvl_pack;
	ti_var_t appVar;
	createLevels(); //TEMP (you'll just download an appvar after I get one properly generated.)
    //Display scores
    appVar = ti_Open("TANKSLPK", "r");
    ti_Read(&lvl_pack, sizeof(LevelPack), 1, appVar);

	ti_CloseAll();
    while (!os_GetCSC());
}

void restartMission() {
	int i;
	for(i = 0; i < level->num_tanks; i++) {
		int j;
		tanks[i].pos_x = (float)(tileXPixel(tanks[i].start_x));
		tanks[i].pos_y = (float)(tileYPixel(tanks[i].start_y));
		for(j = 0; j < 5; j++) {
			tanks[i].shells[j].alive = false;
			if(j < 2) tanks[i].mines[j].alive = false;
		}
	}
	for(i = 0; i < LEVEL_SIZE_X * LEVEL_SIZE_Y; i++) {
		if(level->tiles[i] == DESTROYED)
			level->tiles[i] = DESTRUCTIBLE;
	}
}

uint16_t tileXPixel(uint8_t tile_x) {
	return MAP_OFFSET_X + tile_x * TILE_SIZE;
}

uint8_t tileYPixel(uint8_t tile_y) {
	return tile_y * TILE_SIZE;
}

void createLevels(void) {
	LevelPack lvl_pack = {"TANKS!", 1, {0, 0, 0, 0, 0}};
	Level lvls[] = {{{0}, 1}};
	SerializedTank ser_tanks[] = {{PLAYER, 0, 0}};
	ti_var_t appVar;
	int i;

	ti_CloseAll();

	appVar = ti_Open("TANKSLPK", "w");

	ti_Write(&lvl_pack, sizeof(LevelPack), 1, appVar);
	for(i = 0; i < lvl_pack.num_levels; i++) {
		ti_Write(lvls + 1, sizeof(Level), 1, appVar);
		ti_Write(ser_tanks, sizeof(SerializedTank), lvls[i].num_tanks, appVar);
	}

	ti_CloseAll();
}
