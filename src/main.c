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

#include <graphx.h>

#include "gfx/tiles_gfx.h"
#include "tiles/tilemaps.h"

//Size of the gameplay area in tiles
#define LEVEL_SIZE_X 24
#define LEVEL_SIZE_Y 19

//Pixel size of each square tile
#define TILE_SIZE 12

#define MAP_OFFSET_X 16 //Offset from sides of screen

//Speed of bullets (units?)
#define SHELL_SPEED_STANDARD 1
#define SHELL_SPEED_MISSILE 1

//Target frame rate
#define TARGET_FPS 60

enum {
	EMPTY, //Empty space
	BLOCK, //Block that can be neither shot nor moved through
	DESTRUCTIBLE, //Blocks that can be exploded with mines
	HOLE, //Hole that can be shot over but not moved through
	DESTROYED //DESTRUCTIBLE blocks that have been removed using mines
};

typedef enum {
	PLAYER, //blue
	IMMOBILE, //brown
	BASIC, //grey
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
	uint8_t compressed_tile_size; //Compressed size of tile data
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
	uint16_t pos_x; //Top left pixel of the mine
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
    Level level; //level currently being played
	uint8_t mission; //The mission number, always displayed 1 higher than stored. Also used as an index for levels.
	uint8_t lives; //Number of remaining tanks. This includes the tank that is currently in use, so a value of 1 means that the game will end the next time the tank is hit.
	uint8_t kills; //Number of enemy tanks destroyed.
	uint24_t timer; //Game time, probably used for physics stuff.
	uint16_t cursor_x; //If I decide to implement a cursor mode, this will represent the position of the crosshairs on the screen.
	uint8_t cursor_y;
	uint24_t lastCycle; //Time the last physics cycle started
	bool inProgress; //Whether a mission is in progress
} Game;

void createLevels(void); //Temporary function to make a level pack

void displayScores(void); //Display high scores

void restartMission(void); //Restart a mission.

void missionStart(uint8_t mission, uint8_t lives, uint8_t num_tanks); //Display the mission start screen

void render(void); //Render tilemap, tanks, and UI

void displayUI(void); //Display UI during a mission

void processCollisions(void); //Process collisions between all entities that could reasonably intersect.

void stablizeFPS(uint);

uint16_t tileToXPixel(uint8_t tile_x);
uint8_t tileToYPixel(uint8_t tile_y);

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

Tank deserializeTank(SerializedTank ser_tank);

Game game; //Game global, so I can reuse those names elsewhere

Tank* tanks; //List of all active tanks. 
uint8_t tiles[LEVEL_SIZE_X * LEVEL_SIZE_Y]; //Currently active tilemap data

void main(void) {
	int i;
	LevelPack lvl_pack;
	ti_var_t appVar;
	
	createLevels(); //TEMP (you'll just download an appvar after I get one properly generated.)

	gfx_Begin();

	gfx_SetPalette(tiles_gfx_pal, sizeof_tiles_gfx_pal, 0);

	gfx_SetDrawBuffer();

    displayScores();
    appVar = ti_Open("TANKSLPK", "r");
    ti_Read(&lvl_pack, sizeof(LevelPack), 1, appVar);
    for(game.mission = 0; game.mission < lvl_pack.num_levels; game.mission++) {
        //Level loop
        uint8_t* comp_tiles; //Compressed tile data
        SerializedTank* ser_tanks;
        
        //Read level from appvar
        ti_Read(&game.level, sizeof(Level), 1, appVar);
        comp_tiles = malloc(game.level.compressed_tile_size);
        ti_Read(comp_tiles, sizeof(uint8_t), game.level.compressed_tile_size, appVar); //Load tiles
        ser_tanks = malloc(game.level.num_tanks * sizeof(SerializedTank));
        tanks = malloc(game.level.num_tanks * sizeof(Tank));
        ti_Read(ser_tanks, sizeof(SerializedTank), game.level.num_tanks, appVar);
        for(i = 0; i < game.level.num_tanks; i++) {
        	tanks[i] = deserializeTank(ser_tanks[i]);
        }

        //Decompress tile data
        dzx7_Turbo(comp_tiles, tiles);
        
        missionStart(game.mission, game.lives, game.level.num_tanks);
        
        game.inProgress = true;
        //Game loop
        while(game.inProgress) {
            //process physics
            
            
            render();
            game.inProgress = !os_GetCSC(); //TEMP exit game loop if key pressed
        }

        free(ser_tanks); //Free memory so that we don't have issues
        free(tanks); //(hopefully this does not cause issues)
        free(comp_tiles); 
        
    }

    gfx_End();

	ti_CloseAll();
}

void restartMission() {
	int i;
	int remaining_tanks = 0;
	for(i = 0; i < game.level.num_tanks; i++) {
		int j;
		if(tanks[i].alive) remaining_tanks++;
		tanks[i].pos_x = (float)(tileToXPixel(tanks[i].start_x));
		tanks[i].pos_y = (float)(tileToYPixel(tanks[i].start_y));
		for(j = 0; j < 5; j++) {
			tanks[i].shells[j].alive = false;
			if(j < 2) tanks[i].mines[j].alive = false;
		}
	}
	for(i = 0; i < LEVEL_SIZE_X * LEVEL_SIZE_Y; i++) {
		if(tiles[i] == DESTROYED)
			tiles[i] = DESTRUCTIBLE;
	}
	game.lives--;
	missionStart(game.mission, game.lives, remaining_tanks);
}

uint16_t tileToXPixel(uint8_t tile_x) {
	return MAP_OFFSET_X + tile_x * TILE_SIZE;
}

uint8_t tileToYPixel(uint8_t tile_y) {
	return tile_y * TILE_SIZE;
}

void createLevels(void) {
	LevelPack lvl_pack = {"TANKS!", 3, {0, 0, 0, 0, 0}};
	Level lvls[] = {{30, 1}, {32, 1}, {37, 1}};
	SerializedTank ser_tanks[] = {{PLAYER, 0, 0}};
	ti_var_t appVar;
	int i;

	ti_CloseAll();

	appVar = ti_Open("TANKSLPK", "w");

	ti_Write(&lvl_pack, sizeof(LevelPack), 1, appVar);
	for(i = 0; i < lvl_pack.num_levels; i++) {
		uint8_t* comp_tiles;
		ti_Write(lvls + i, sizeof(Level), 1, appVar);
		switch(i) {
			case 0:
			default:
				comp_tiles = lvl1_compressed;
				break;
			case 1:
				comp_tiles = lvl2_compressed;
				break;
			case 2:
				comp_tiles = lvl3_compressed;
				break;
		}
		ti_Write(comp_tiles, sizeof(uint8_t), lvls[i].compressed_tile_size, appVar);
		ti_Write(ser_tanks, sizeof(SerializedTank), lvls[i].num_tanks, appVar);
	}

	ti_CloseAll();
}

void displayScores(void) {

}

void missionStart(uint8_t mission, uint8_t lives, uint8_t num_tanks) {

}

void render(void) {
	int i = 0;
	gfx_tilemap_t tilemap;

	tilemap.map 		= tiles;
	tilemap.tiles 		= tileset_tiles;
	tilemap.type_width 	= gfx_tile_no_pow2;
	tilemap.type_height = gfx_tile_no_pow2;
	tilemap.tile_height = TILE_SIZE;
	tilemap.tile_width	= TILE_SIZE;
	tilemap.draw_height	= LEVEL_SIZE_Y;
	tilemap.draw_width 	= LEVEL_SIZE_X;
	tilemap.height 		= LEVEL_SIZE_Y;
	tilemap.width		= LEVEL_SIZE_X;
	tilemap.y_loc		= 0;
	tilemap.x_loc		= MAP_OFFSET_X;

	gfx_Tilemap(&tilemap, 0, 0);

	gfx_BlitBuffer();

	while(!os_GetCSC);

}

Tank deserializeTank(SerializedTank ser_tank) {
	Tank result;
	result.type = ser_tank.type;
	//TODO: rest of deserialization
	return result;
}

/*
1: blue:(3,7), brown(20,7)
2: blue:(3,14), grey:(20,4)
3: blue:(3,9), grey:(6,2), grey:(19,16), brown:(20,9)
*/
