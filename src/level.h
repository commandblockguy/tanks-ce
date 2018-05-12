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

//Size of the gameplay area in tiles
#define LEVEL_SIZE_X 22
#define LEVEL_SIZE_Y 17

enum {
	EMPTY = 0, //Empty space
	BLOCK, //Block that can be neither shot nor moved through
	DESTRUCTIBLE, //Blocks that can be exploded with mines
	HOLE, //Hole that can be shot over but not moved through
	DESTROYED //DESTRUCTIBLE blocks that have been removed using mines
};

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

Tank deserializeTank(SerializedTank ser_tank); //Convert a serialized tank into an actual one

#endif
