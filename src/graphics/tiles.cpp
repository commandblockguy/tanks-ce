#include "tiles.h"
#include "graphics.h"
#include "../globals.h"
#include "../util/profiler.h"

uint8_t tilemap[TILEMAP_HEIGHT][TILEMAP_WIDTH];
// For each tilemap tile, the level Y of the block that it's representing
uint8_t heightmap[TILEMAP_HEIGHT][TILEMAP_WIDTH];

void generate_bg_tilemap() {
    const bool bottom_is_tall[] = {false, true, false, false, true, false, false, true};
    const bool bottom_is_alt[] = {false, false, true, false, false, true, false, false};

    for(int8_t y = 0; y < TILEMAP_HEIGHT; y++) {
        uint8_t x;
        for(x = 0; x < TILEMAP_WIDTH; x++) {
            tilemap[y][x] = TS_NONE;
            heightmap[y][x] = 0;
        }
    }

    for(int8_t y = 0; y < LEVEL_SIZE_Y; y++) {
        uint8_t x;
        for(x = 0; x < TILEMAP_WIDTH; x++) {
            tile_t tile = tiles[y][x];
            uint8_t height = TILE_HEIGHT(tile);
            uint8_t type = TILE_TYPE(tile);
            bool tall = bottom_is_tall[height];
            bool alt = bottom_is_alt[height];
            uint8_t tm_y = 2 * y + TILEMAP_OFFSET;

            switch(type) {
                default:
                    break;
                case DESTROYED:
                    tilemap[tm_y][x] = TS_NONE;
                    tilemap[tm_y + 1][x] = TS_NONE;
                    break;
                case HOLE:
                    tilemap[tm_y][x] = TS_HOLE_TOP;
                    tilemap[tm_y + 1][x] = TS_HOLE_BOT;
                    break;
                case BLOCK: {
                    int8_t z;
                    if(!TILE_HEIGHT(tile)) {
                        tilemap[tm_y][x] = TS_NONE;
                        tilemap[tm_y + 1][x] = TS_NONE;
                        break;
                    }

                    for(z = -1; z < height; z++) {
                        if(tall) {
                            if(alt) {
                                tilemap[tm_y - z - 1][x] = TS_SIDE_ALT_TOP;
                                tilemap[tm_y - z][x] = TS_SIDE_ALT_BOT;
                                heightmap[tm_y - z - 1][x] = z + 2;
                                heightmap[tm_y - z][x] = z + 1;
                            } else {
                                tilemap[tm_y - z - 1][x] = TS_SIDE_TOP;
                                tilemap[tm_y - z][x] = TS_SIDE_BOT;
                                heightmap[tm_y - z - 1][x] = z + 2;
                                heightmap[tm_y - z][x] = z + 1;
                            }
                            z++;
                        } else {
                            if(alt) {
                                tilemap[tm_y - z][x] = TS_SIDE_ALT_HALF;
                                heightmap[tm_y - z][x] = z + 1;
                            } else {
                                tilemap[tm_y - z][x] = TS_SIDE_HALF;
                                heightmap[tm_y - z][x] = z + 1;
                            }
                        }
                        alt = !alt;
                        tall = !tall;
                    }

                    tilemap[tm_y - z - 1][x] = TS_TOP_TOP;
                    tilemap[tm_y - z][x] = TS_TOP_BOT;
                    heightmap[tm_y - z - 1][x] = z + 1;
                    heightmap[tm_y - z][x] = z + 1;

                    break;
                }
                case DESTRUCTIBLE: {
                    int8_t z;
                    if(!TILE_HEIGHT(tile)) {
                        tilemap[tm_y][x] = TS_NONE;
                        tilemap[tm_y + 1][x] = TS_NONE;
                        break;
                    }

                    for(z = -1; z < height; z++) {
                        if(tall) {
                            tilemap[tm_y - z - 1][x] = TS_DEST_SIDE_TOP;
                            tilemap[tm_y - z][x] = TS_DEST_SIDE_BOT;
                            heightmap[tm_y - z - 1][x] = z + 2;
                            heightmap[tm_y - z][x] = z + 1;
                            z++;
                        } else {
                            tilemap[tm_y - z][x] = TS_DEST_SIDE_HALF;
                            heightmap[tm_y - z][x] = z + 1;
                        }
                        tall = !tall;
                    }

                    tilemap[tm_y - z - 1][x] = TS_DEST_TOP_TOP;
                    tilemap[tm_y - z][x] = TS_DEST_TOP_BOT;
                    heightmap[tm_y - z - 1][x] = z + 1;
                    heightmap[tm_y - z][x] = z + 1;

                    break;
                }
            }
        }
    }
}

const gfx_tilemap_t tilemap_config = {(uint8_t *) tilemap, tileset_tiles, HALF_TILE_PIXEL_HEIGHT, TILE_PIXEL_SIZE_X,
                                      TILEMAP_HEIGHT, TILEMAP_WIDTH, gfx_tile_no_pow2, gfx_tile_no_pow2, TILEMAP_HEIGHT,
                                      TILEMAP_WIDTH, TILEMAP_BASE_Y, (uint)SCREEN_X_CONST(0)};

void draw_tilemap() {
    gfx_Tilemap(&tilemap_config, 0, 0);
}

void redraw_tile(uint8_t x, uint8_t y) {
    gfx_sprite_t *tile = tileset_tiles[tilemap[y][x]];
    uint screen_x = SCREEN_X(TILE_SIZE * x);
    uint8_t screen_y = TILEMAP_BASE_Y + HALF_TILE_PIXEL_HEIGHT * y;
    gfx_Sprite(tile, screen_x, screen_y);
}

// Convert a screenspace coordinate to a redraw tile
uint8_t inline screen_to_tm_x(uint screen_x) {
    return fast_div(screen_x - SCREEN_X_CONST(0), SCREEN_DELTA_X_CONST(TILE_SIZE));
}

uint8_t inline screen_to_tm_y(uint screen_y) {
    return fast_div(screen_y - TILEMAP_BASE_Y, HALF_TILE_PIXEL_HEIGHT);
}

void redraw_tiles(const gfx_region_t *region, uint8_t height) {
    profiler_add(redraw_tile);
    uint8_t tile_start_x = screen_to_tm_x(region->xmin);
    uint8_t tile_start_y = screen_to_tm_y(region->ymin);
    uint8_t tile_end_x = screen_to_tm_x(region->xmax);
    uint8_t tile_end_y = screen_to_tm_y(region->ymax);
    uint8_t *hm_base_ptr = heightmap[tile_start_y];
    for(uint8_t tile_x = tile_start_x; tile_x <= tile_end_x; tile_x++) {
        uint8_t *hm_ptr = hm_base_ptr + tile_x;
        for(uint8_t tile_y = tile_start_y; tile_y <= tile_end_y; tile_y++, hm_ptr += TILEMAP_WIDTH) {
            if(*hm_ptr > height) {
                redraw_tile(tile_x, tile_y);
            }
        }
    }
    profiler_end(redraw_tile);
}
