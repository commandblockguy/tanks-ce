#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <graphx.h>
#undef NDEBUG
#include <debug.h>

#include "constants.h"
#include "level.h"
#include "graphics.h"
#include "gfx/gfx.h"
#include "util.h"
#include "keypadc.h"
#include "globals.h"
#include "profiler.h"
#include "partial_redraw.h"
#include "dynamic_sprites.h"

uint8_t tilemap[TILEMAP_HEIGHT][LEVEL_SIZE_X];
// For each tilemap tile, the level Y of the block that it's representing
uint8_t depthmap[TILEMAP_HEIGHT][LEVEL_SIZE_X];

bool needs_redraw;

void repalettize_sprite(gfx_sprite_t *out, const gfx_sprite_t *in, const uint8_t *map) {
    out->width = in->width;
    out->height = in->height;

    size_t size = in->width * in->height;

    for(uint24_t i = 0; i < size; i++) {
        out->data[i] = map[in->data[i]];
    }
}

// this is supposed to go in dynamic_sprites.c, but a weird (linker?) issue causes this array to be filled with NULL if I do that
gfx_sprite_t * const enemy_bases_unconv[9] = {
        en_base_0,
        en_base_1,
        en_base_2,
        en_base_3,
        en_base_4,
        en_base_5,
        en_base_6,
        en_base_7,
        en_base_8
};

gfx_sprite_t * const enemy_turrets_unconv[9] = {
        en_turret_0,
        en_turret_1,
        en_turret_2,
        en_turret_3,
        en_turret_4,
        en_turret_5,
        en_turret_6,
        en_turret_7,
        en_turret_8
};

void initGraphics(void) {
    gfx_Begin(); //Set up draw bits
    gfx_SetPalette(palette, sizeof_palette, 0);
    gfx_SetDrawBuffer();
    gfx_SetTextFGColor(COL_RIB_SHADOW); // todo: verify

    for(uint8_t i = 1; i < 8; i++) {
        gfx_FlipSpriteY(tank_bases[PLAYER][i], tank_bases[PLAYER][16 - i]);
        gfx_FlipSpriteY(tank_turrets[PLAYER][i], tank_turrets[PLAYER][16 - i]);
    }
    for(uint8_t type = PLAYER + 1; type < NUM_TANK_TYPES; type++) {
        uint8_t palette_table[6] = {0, COL_ENEMY_TANK_WOOD_1, COL_ENEMY_TANK_WOOD_2};
        for(uint8_t i = 0; i < 3; i++) palette_table[3 + i] = 256 - NUM_DYNAMIC_COLORS * NUM_TANK_TYPES + NUM_DYNAMIC_COLORS * type;

        for(uint8_t rot = 0; rot < 9; rot++) {
            repalettize_sprite(tank_bases[type][rot], enemy_bases_unconv[rot], palette_table);
            repalettize_sprite(tank_turrets[type][rot], enemy_turrets_unconv[rot], palette_table);
        }
        for(uint8_t i = 1; i < 8; i++) {
            gfx_FlipSpriteY(tank_bases[type][i], tank_bases[type][16 - i]);
            gfx_FlipSpriteY(tank_turrets[type][i], tank_turrets[type][16 - i]);
        }
    }
}

void generate_bg_tilemap(void) {
    uint8_t y;
    const bool bottom_is_tall[] = {0, 1, 0, 0, 1, 0, 0, 1};
    const bool bottom_is_alt[] = {0, 0, 1, 0, 0, 1, 0, 0};

    for(y = 0; y < TILEMAP_HEIGHT; y++) {
        uint8_t x;
        for(x = 0; x < LEVEL_SIZE_X; x++) {
            tilemap[y][x] = TS_NONE;
            depthmap[y][x] = y;
        }
    }

    for(y = 0; y < LEVEL_SIZE_Y; y++) {
        uint8_t x;
        for(x = 0; x < LEVEL_SIZE_X; x++) {
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
                    depthmap[tm_y][x] = y;
                    depthmap[tm_y + 1][x] = y;
                    break;
                case HOLE:
                    tilemap[tm_y][x] = TS_HOLE_TOP;
                    tilemap[tm_y + 1][x] = TS_HOLE_BOT;
                    depthmap[tm_y][x] = y;
                    depthmap[tm_y + 1][x] = y;
                    break;
                case BLOCK: {
                    int8_t z;
                    if(!TILE_HEIGHT(tile)) {
                        tilemap[tm_y][x] = TS_NONE;
                        tilemap[tm_y + 1][x] = TS_NONE;
                        depthmap[tm_y][x] = y;
                        depthmap[tm_y + 1][x] = y;
                        break;
                    }

                    for(z = -1; z < height; z++) {
                        if(tall) {
                            if(alt) {
                                tilemap[tm_y - z - 1][x] = TS_SIDE_ALT_TOP;
                                tilemap[tm_y - z][x] = TS_SIDE_ALT_BOT;
                                depthmap[tm_y - z - 1][x] = y;
                                depthmap[tm_y - z][x] = y;
                            } else {
                                tilemap[tm_y - z - 1][x] = TS_SIDE_TOP;
                                tilemap[tm_y - z][x] = TS_SIDE_BOT;
                                depthmap[tm_y - z - 1][x] = y;
                                depthmap[tm_y - z][x] = y;
                            }
                            z++;
                        } else {
                            if(alt) {
                                tilemap[tm_y - z][x] = TS_SIDE_ALT_HALF;
                                depthmap[tm_y - z][x] = y;
                            } else {
                                tilemap[tm_y - z][x] = TS_SIDE_HALF;
                                depthmap[tm_y - z][x] = y;
                            }
                        }
                        alt  = !alt;
                        tall = !tall;
                    }

                    tilemap[tm_y - z - 1][x] = TS_TOP_TOP;
                    tilemap[tm_y - z][x] = TS_TOP_BOT;
                    depthmap[tm_y - z - 1][x] = y;
                    depthmap[tm_y - z][x] = y;

                    break;
                }
                case DESTRUCTIBLE: {
                    int8_t z;
                    if(!TILE_HEIGHT(tile)) {
                        tilemap[tm_y][x] = TS_NONE;
                        tilemap[tm_y + 1][x] = TS_NONE;
                        depthmap[tm_y][x] = y;
                        depthmap[tm_y + 1][x] = y;
                        break;
                    }

                    for(z = -1; z < height; z++) {
                        if(tall) {
                            tilemap[tm_y - z - 1][x] = TS_DEST_SIDE_TOP;
                            tilemap[tm_y - z][x] = TS_DEST_SIDE_BOT;
                            depthmap[tm_y - z - 1][x] = y;
                            depthmap[tm_y - z][x] = y;
                            z++;
                        } else {
                            tilemap[tm_y - z][x] = TS_DEST_SIDE_HALF;
                            depthmap[tm_y - z][x] = y;
                        }
                        tall = !tall;
                    }

                    tilemap[tm_y - z - 1][x] = TS_DEST_TOP_TOP;
                    tilemap[tm_y - z][x] = TS_DEST_TOP_BOT;
                    depthmap[tm_y - z - 1][x] = y;
                    depthmap[tm_y - z][x] = y;

                    break;
                }
            }
        }
    }
}

const gfx_tilemap_t tilemap_config = {
        (uint8_t*)tilemap,
        tileset_tiles,
        HALF_TILE_PIXEL_HEIGHT,
        14,
        TILEMAP_HEIGHT,
        LEVEL_SIZE_X,
        gfx_tile_no_pow2,
        gfx_tile_no_pow2,
        TILEMAP_HEIGHT,
        LEVEL_SIZE_X,
        TILEMAP_BASE_Y,
        SCREEN_X(0)
};

void redraw_tile(uint8_t x, uint8_t y) {
    gfx_sprite_t *tile = tileset_tiles[tilemap[y][x]];
    uint8_t screen_y = TILEMAP_BASE_Y + HALF_TILE_PIXEL_HEIGHT * y;
    gfx_Sprite_NoClip(tile, SCREEN_X(TILE_SIZE * x), screen_y);
}

void full_redraw(void) {
    generate_bg_tilemap();

	gfx_FillScreen(COL_WHITE);
	gfx_Tilemap(&tilemap_config, 0, 0);

	pdraw_FreeAll();
    needs_redraw = false;
}

// Convert a screenspace coordinate to a redraw tile
uint8_t screen_to_tm_x(uint24_t screen_x) {
    return (screen_x - SCREEN_X(0)) / SCREEN_DELTA_X(TILE_SIZE);
}

uint8_t screen_to_tm_y(uint24_t screen_y) {
    return (screen_y - TILEMAP_BASE_Y) / HALF_TILE_PIXEL_HEIGHT;
}

void render_tank(tank_t *tank) {
    int j;

    if(tank->alive) {
        uint24_t base_x = SCREEN_X(tank->phys.position_x) - TANK_SPRITE_OFFSET_X;
        uint8_t base_y = SCREEN_Y(tank->phys.position_y) - TANK_SPRITE_OFFSET_Y;
        uint8_t base_sprite = (((uint8_t)-((tank->tread_rot >> 16) - 64)) >> 3) & 0xF;
        uint8_t turret_sprite = ((uint8_t)-((tank->barrel_rot >> 16) - 64)) >> 4;
        uint8_t tile_x, tile_y;
        uint8_t end_x = screen_to_tm_x(base_x + TANK_SPRITE_SIZE_X);
        uint8_t end_y = screen_to_tm_y(base_y + TANK_SPRITE_SIZE_Y);
        uint8_t tank_y = ptToYTile(tank->phys.position_y + TANK_SIZE - 1); // -1 is to round down if exactly on the edge

        pdraw_TransparentSprite_NoClip(tank_bases[tank->type][base_sprite],
                base_x + base_x_offsets[base_sprite],
                base_y + base_y_offsets[base_sprite]);
        pdraw_TransparentSprite_NoClip(tank_turrets[tank->type][turret_sprite],
                base_x + turret_x_offsets[turret_sprite],
                base_y + turret_y_offsets[turret_sprite]);

        for(tile_x = screen_to_tm_x(base_x); tile_x <= end_x; tile_x++) {
            for(tile_y = screen_to_tm_y(base_y); tile_y <= end_y; tile_y++) {
                uint8_t world_tile_y = depthmap[tile_y][tile_x];
                if(world_tile_y > tank_y && TILE_HEIGHT(tiles[world_tile_y][tile_x])) { // this is a hackfix, but whatever
                    redraw_tile(tile_x, tile_y);
                }
            }
        }
    }

    //draw shell hitboxes until I can get sprites
    for(j = max_shells[tank->type] - 1; j >= 0; j--) {
        shell_t* shell = &tank->shells[j];
        if(!(shell->alive)) continue;
        gfx_SetColor(COL_BLACK);
        renderPhysicsBody(&shell->phys);
    }
    //draw mine hitboxes
    for(j = max_mines[tank->type] - 1; j >= 0; j--) {
        mine_t* mine = &tank->mines[j];
        if(!mine->countdown) continue;
        gfx_SetColor(COL_RED);
        if(mine->alive) gfx_SetColor(COL_BLACK);
        renderPhysicsBody(&mine->phys);
    }
}

void render(level_t *level) {
	profiler_start(graphics);
    int i;

    profiler_start(gfx_wait);
    gfx_Wait();
    profiler_end(gfx_wait);
	
	if(needs_redraw) {
        profiler_start(tilemap);
	    gfx_SetDrawScreen();
	    full_redraw();
	    gfx_SetDrawBuffer();
	    full_redraw();
        profiler_end(tilemap);
	}

	profiler_start(undraw);
    pdraw_RemoveSprites();
    profiler_end(undraw);

	profiler_start(render_tanks);
    // todo: z-sorting
	for(i = 0; i < level->num_tanks; i++) {
		render_tank(&tanks[i]);
	}
	profiler_end(render_tanks);

	gfx_SetColor(COL_WHITE);
	gfx_FillRectangle_NoClip(0, 0, 32, 8);
	gfx_SetTextXY(0,0);
	gfx_PrintUInt(fpsCounter(), 4);

	profiler_start(swapdraw);
	gfx_SwapDraw();
	profiler_end(swapdraw);

    profiler_end(graphics);
}

void renderPhysicsBody(physicsBody_t *phys) {
    uint24_t x = SCREEN_X(phys->position_x);
    uint8_t y = SCREEN_Y(phys->position_y);
    uint8_t width = SCREEN_DELTA_X(phys->width);
    uint8_t height = SCREEN_DELTA_Y(phys->height);
    pdraw_RectRegion(x, y, width, height);
	gfx_Rectangle(x, y, width, height);
}

void drawLine(lineSeg_t* ls) {
	gfx_Line(
		SCREEN_X(ls->x1),
		SCREEN_Y(ls->y1),
		SCREEN_X(ls->x2),
		SCREEN_Y(ls->y2)
	);
}
