#include "level.h"

#include <compression.h>

#include "globals.h"

void decompress_tiles(const void *comp_tiles) {
    //Decompress tile data
    zx7_Decompress(tiles, comp_tiles);
    for(uint8_t row = LEVEL_SIZE_Y - 2; row > 0; row--) {
        auto *orig_tiles = (tile_t(*)[LEVEL_SIZE_X - 2])tiles;
        memmove(&tiles[row][1], orig_tiles[row - 1], LEVEL_SIZE_X - 2);
        tiles[row][0] = tiles[row][LEVEL_SIZE_X - 1] = 1;
    }
    memset(tiles[0], 1, LEVEL_SIZE_X);
    memset(tiles[LEVEL_SIZE_Y - 1], 1, LEVEL_SIZE_X);
}
