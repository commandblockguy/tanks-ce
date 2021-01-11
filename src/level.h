#ifndef H_LEVEL
#define H_LEVEL

#include <stdbool.h>
#include <cstddef>
#include <cstdint>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include <string.h>

typedef uint8_t tile_t;
//Bits 2-0: Block height in half-blocks, skipping height 0.5
//0 for no tile, 1 for full block, 3 for 1.5 blocks, 4 for 2 blocks
//Bits 4-3:
enum {
    BLOCK = 0 << 3, //Block that can be neither shot nor moved through
    DESTRUCTIBLE = 1 << 3, //Blocks that can be exploded with mines
    HOLE = 2 << 3, //Hole that can be shot over but not moved through
    DESTROYED = 3 << 3, //DESTRUCTIBLE blocks that have been removed using mines
    TYPE_MASK = 3 << 3
};

//0 = no block
//1 = full block, as above
#define TILE_HEIGHT(tile) ((tile) & 7)
#define TILE_TYPE(tile) ((tile) & TYPE_MASK)

typedef struct {
    uint8_t compressed_tile_size; //Compressed size of tile data
    uint8_t num_tanks; //Number of tanks in the level
} level_t;

typedef struct {
    char name[15];
    uint8_t num_levels;
    uint8_t scores[5];
} level_pack_t;

void decompress_tiles(const void *comp_tiles);

#define TILE_SIZE 256

#define TILE_TO_X_COORD(x) ((x) * TILE_SIZE)
#define TILE_TO_Y_COORD(y) ((y) * TILE_SIZE)

// we always want to round down, rather than towards 0, hence the -(x < 0)
#define COORD_TO_X_TILE(x) ((int8_t)((x) / TILE_SIZE - ((x) < 0)))
#define COORD_TO_Y_TILE(y) ((int8_t)((y) / TILE_SIZE - ((y) < 0)))

//Size of the gameplay area in tiles
#define LEVEL_SIZE_X 18
#define LEVEL_SIZE_Y 19

#endif
