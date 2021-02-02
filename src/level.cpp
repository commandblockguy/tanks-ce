#include "level.h"
#include "game.h"

#include <compression.h>

void decompress_tiles(const void *comp_tiles) {
    //Decompress tile data
    zx7_Decompress(game.tiles, comp_tiles);
    for(uint8_t row = LEVEL_SIZE_Y - 2; row > 0; row--) {
        auto orig_tiles = (tile_t(*)[LEVEL_SIZE_X - 2])game.tiles;
        memmove(&game.tiles[row][1], orig_tiles[row - 1], LEVEL_SIZE_X - 2);
        game.tiles[row][0] = game.tiles[row][LEVEL_SIZE_X - 1] = 1;
    }
    memset(game.tiles[0], 1, LEVEL_SIZE_X);
    memset(game.tiles[LEVEL_SIZE_Y - 1], 1, LEVEL_SIZE_X);
}
