#ifndef H_LEVEL
#define H_LEVEL

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "objects.h"

typedef uint8_t tile_t;
//TODO: change formatting to the following:
//Bits 2-0: Block height in half-blocks, skipping height 0.5
//0 for no tile, 1 for full block, 3 for 1.5 blocks, 4 for 2 blocks
//Bits 4-3:
enum {
	BLOCK = 0, //Block that can be neither shot nor moved through
	DESTRUCTIBLE = 8, //Blocks that can be exploded with mines
	HOLE = 16, //Hole that can be shot over but not moved through
	DESTROYED = 24//DESTRUCTIBLE blocks that have been removed using mines
};
/*
enum {
	EMPTY = 0,
	BLOCK,
	DESTRUCTIBLE,
	HOLE,
	DESTROYED
};*/

//0 = no block
//1 = full block, as above
#define tileHeight(tile) (tile & 7)
#define tileType(tile) (tile & DESTROYED)

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

void createLevels(void); //Temporary function to make a level pack

uint16_t tileToXPixel(uint8_t tile_x);
uint8_t tileToYPixel(uint8_t tile_y);

ufix_t tileToXPt(uint8_t x);
ufix_t tileToYPt(uint8_t y);

uint8_t pixelToXTile(uint24_t pix_x);
uint8_t pixelToYTile(uint8_t pix_x);

uint8_t ptToXTile(ufix_t x);
uint8_t ptToYTile(ufix_t y);

Tank deserializeTank(SerializedTank ser_tank); //Convert a serialized tank into an actual one

#endif
