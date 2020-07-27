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

#include "tank.h"
#include "constants.h"

typedef uint8_t tile_t;
//TODO: change formatting to the following:
//Bits 2-0: Block height in half-blocks, skipping height 0.5
//0 for no tile, 1 for full block, 3 for 1.5 blocks, 4 for 2 blocks
//Bits 4-3:
enum {
	BLOCK        = 0 << 3, //Block that can be neither shot nor moved through
	DESTRUCTIBLE = 1 << 3, //Blocks that can be exploded with mines
	HOLE         = 2 << 3, //Hole that can be shot over but not moved through
	DESTROYED    = 3 << 3, //DESTRUCTIBLE blocks that have been removed using mines
	TYPE_MASK    = 3 << 3
};

//0 = no block
//1 = full block, as above
#define TILE_HEIGHT(tile) (tile & 7)
#define TILE_TYPE(tile) (tile & TYPE_MASK)

typedef struct {
	//A tank as stored in the level file
	tankType_t type;
	uint8_t start_x; //Tile the tank starts on
	uint8_t start_y;
} SerializedTank;

typedef struct {
	uint8_t compressed_tile_size; //Compressed size of tile data
	uint8_t num_tanks; //Number of tanks in the level
} level_t;

typedef struct {
	char name[15];
	uint8_t num_levels;
	uint8_t scores[5];
} LevelPack;

void createLevels(void); //Temporary function to make a level pack

#define tileToXPt(x) ((x) * TILE_SIZE)
#define tileToYPt(y) ((y) * TILE_SIZE)

// we always want to round down, rather than towards 0, hence the -(x < 0)
#define ptToXTile(x) ((x) / TILE_SIZE - (x < 0))
#define ptToYTile(y) ((y) / TILE_SIZE - (y < 0))

void deserializeTank(tank_t *tank, const SerializedTank *ser_tank); //Convert a serialized tank into an actual one

#endif
