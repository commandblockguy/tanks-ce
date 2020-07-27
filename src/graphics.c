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

uint8_t tilemap[TILEMAP_HEIGHT][LEVEL_SIZE_X];
// For each tilemap tile, the level Y of the block that it's representing
uint8_t depthmap[TILEMAP_HEIGHT][LEVEL_SIZE_X];

gfx_UninitedSprite(pl_base_9, 28, 24);
gfx_UninitedSprite(pl_base_10, 28, 24);
gfx_UninitedSprite(pl_base_11, 28, 24);
gfx_UninitedSprite(pl_base_12, 28, 24);
gfx_UninitedSprite(pl_base_13, 28, 24);
gfx_UninitedSprite(pl_base_14, 28, 24);
gfx_UninitedSprite(pl_base_15, 28, 24);

gfx_UninitedSprite(pl_turret_9, 28, 24);
gfx_UninitedSprite(pl_turret_10, 28, 24);
gfx_UninitedSprite(pl_turret_11, 28, 24);
gfx_UninitedSprite(pl_turret_12, 28, 24);
gfx_UninitedSprite(pl_turret_13, 28, 24);
gfx_UninitedSprite(pl_turret_14, 28, 24);
gfx_UninitedSprite(pl_turret_15, 28, 24);

gfx_sprite_t *player_bases[16] = {
    pl_base_0,
    pl_base_1,
    pl_base_2,
    pl_base_3,
    pl_base_4,
    pl_base_5,
    pl_base_6,
    pl_base_7,
    pl_base_8,
    (gfx_sprite_t*)pl_base_9_data,
    (gfx_sprite_t*)pl_base_10_data,
    (gfx_sprite_t*)pl_base_11_data,
    (gfx_sprite_t*)pl_base_12_data,
    (gfx_sprite_t*)pl_base_13_data,
    (gfx_sprite_t*)pl_base_14_data,
    (gfx_sprite_t*)pl_base_15_data,
};

gfx_sprite_t *player_turrets[16] = {
    pl_turret_0,
    pl_turret_1,
    pl_turret_2,
    pl_turret_3,
    pl_turret_4,
    pl_turret_5,
    pl_turret_6,
    pl_turret_7,
    pl_turret_8,
    (gfx_sprite_t*)pl_turret_9_data,
    (gfx_sprite_t*)pl_turret_10_data,
    (gfx_sprite_t*)pl_turret_11_data,
    (gfx_sprite_t*)pl_turret_12_data,
    (gfx_sprite_t*)pl_turret_13_data,
    (gfx_sprite_t*)pl_turret_14_data,
    (gfx_sprite_t*)pl_turret_15_data,
};

void initGraphics(void) {
    int8_t i;
    gfx_Begin(); //Set up draw bits
    gfx_SetPalette(palette, sizeof_palette, 0);
    gfx_SetDrawBuffer();
    gfx_SetTextFGColor(7);

    for(i = 1; i < 8; i++) {
        player_bases[i]->width  = player_turrets[i]->width  = 28;
        player_bases[i]->height = player_turrets[i]->height = 24;
        gfx_FlipSpriteY(player_bases[i], player_bases[16 - i]);
        gfx_FlipSpriteY(player_turrets[i], player_turrets[16 - i]);
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

void redraw(void) {
    profiler_start(tilemap);
	gfx_FillScreen(COL_WHITE);

	gfx_Tilemap(&tilemap_config, 0, 0);
    profiler_end(tilemap);
}

// Convert a screenspace coordinate to a redraw tile
uint8_t screen_to_tm_x(uint24_t screen_x) {
    uint24_t base_x = SCREEN_X(0);
    return (screen_x - base_x) / SCREEN_DELTA_X(TILE_SIZE);
}

uint8_t screen_to_tm_y(uint24_t screen_y) {
    return (screen_y - TILEMAP_BASE_Y) / HALF_TILE_PIXEL_HEIGHT;
}

void render_tank(tank_t *tank) {
    int j;

    if(tank == &tanks[0]) {
        uint24_t base_x = SCREEN_X(tank->phys.position_x) - TANK_SPRITE_OFFSET_X;
        uint8_t base_y = SCREEN_Y(tank->phys.position_y) - TANK_SPRITE_OFFSET_Y;
        uint8_t base_sprite = (((uint8_t)-((tank->tread_rot >> 16) - 64)) >> 3) & 0xF;
        uint8_t turret_sprite = ((uint8_t)-((tank->barrel_rot >> 16) - 64)) >> 4;
        uint8_t tile_x, tile_y;
        uint8_t end_x = screen_to_tm_x(base_x + TANK_SPRITE_SIZE_X);
        uint8_t end_y = screen_to_tm_y(base_y + TANK_SPRITE_SIZE_Y);

        gfx_TransparentSprite(player_bases[base_sprite], base_x, base_y);
        gfx_TransparentSprite(player_turrets[turret_sprite], base_x, base_y);

        for(tile_x = screen_to_tm_x(base_x); tile_x <= end_x; tile_x++) {
            for(tile_y = screen_to_tm_y(base_y); tile_y <= end_y; tile_y++) {
                uint8_t tank_y = ptToYTile(tank->phys.position_y + TANK_SIZE - 1); // doesn't work without the -1
                if(depthmap[tile_y][tile_x] > tank_y) {
                    redraw_tile(tile_x, tile_y);
                }
            }
        }
    }
    else if(tank->alive) {
        gfx_SetColor(COL_BLACK);
        renderPhysicsBody(&tank->phys);
        gfx_Line(
                SCREEN_X(centerX(&tank->phys)),
                SCREEN_Y(centerY(&tank->phys)),
                SCREEN_X(centerX(&tank->phys) + fast_cos(tank->barrel_rot) * BARREL_LENGTH / TRIG_SCALE),
                SCREEN_Y(centerY(&tank->phys) + fast_sin(tank->barrel_rot) * BARREL_LENGTH / TRIG_SCALE));
        gfx_SetTextFGColor(COL_RED);
        gfx_SetTextXY(SCREEN_X(tank->phys.position_x) + 1, SCREEN_Y(tank->phys.position_y) + 1);
        gfx_PrintUInt(tank->type, 1);
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
	
	//Eventually, this will only be called once
	//redraw(tiles, level);

	profiler_start(render_tanks);
    // todo: z-sorting
	for(i = 0; i < level->num_tanks; i++) {
		//Render tanks
		render_tank(&tanks[i]);
	}
	profiler_end(render_tanks);

	gfx_SetTextXY(0,0);
	gfx_PrintUInt(fpsCounter(), 4);

	profiler_start(blit);
	gfx_BlitBuffer();
	profiler_end(blit);

    redraw(); //todo: move back?
    profiler_end(graphics);
}

void renderPhysicsBody(physicsBody_t *phys) {
	gfx_Rectangle(SCREEN_X(phys->position_x), SCREEN_Y(phys->position_y),
	        SCREEN_DELTA_X(phys->width), SCREEN_DELTA_Y(phys->height));
}

void drawLine(lineSeg_t* ls) {
	gfx_Line(
		SCREEN_X(ls->x1),
		SCREEN_Y(ls->y1),
		SCREEN_X(ls->x2),
		SCREEN_Y(ls->y2)
	);
}
