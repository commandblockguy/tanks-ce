#ifndef TANKS_TILES_H
#define TANKS_TILES_H

#include <graphx.h>
#include <cstdint>

#define HALF_TILE_PIXEL_HEIGHT (TILE_PIXEL_SIZE_Y / 2)

// todo: wat
#define TILEMAP_OFFSET 2
#define TILEMAP_HEIGHT (2 * LEVEL_SIZE_Y + TILEMAP_OFFSET)
#define TILEMAP_WIDTH LEVEL_SIZE_X
#define TILEMAP_BASE_Y (SCREEN_Y_CONST(0) - HALF_TILE_PIXEL_HEIGHT * TILEMAP_OFFSET)

void generate_bg_tilemap();
void draw_tilemap();
void redraw_tiles(const gfx_region_t *region, uint8_t height);

#endif //TANKS_TILES_H
