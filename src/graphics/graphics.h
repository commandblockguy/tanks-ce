#ifndef H_GRAPHICS
#define H_GRAPHICS

#include <stdbool.h>
#include <cstddef>
#include <cstdint>

#include <sys/lcd.h>

#include <graphx.h>

#include "../physics/collision.h"
#include "../level.h"
#include "../data/gfx/gfx.h"
#include "../objects/tank.h"
#include "../fwd.h"

enum colors {
    COL_WHITE = 1,
    COL_BLACK = 2,
    COL_RED = 3,
    COL_BG = 4,
    COL_RHOM_1 = 5,
    COL_RHOM_2 = 6,
    COL_RIB_SHADOW = 7,
    COL_GOLD = 8,
    COL_TXT_SHADOW = 9,
    COL_LIVES_TXT = 10,
    COL_OLIVE_BANDS = 11,
    COL_ENEMY_TANK_WOOD_1 = 12,
    COL_ENEMY_TANK_WOOD_2 = 13,
    COL_ENEMY_TANK_WOOD_3 = 14,
    COL_LIGHT_GREEN = 15,
    COL_DARK_GREEN = 16
};

enum tileset {
    TS_TOP_TOP,
    TS_SIDE_TOP,
    TS_SIDE_ALT_TOP,
    TS_SIDE_HALF,
    TS_DEST_TOP_TOP,
    TS_DEST_SIDE_TOP,
    TS_DEST_SIDE_HALF,
    TS_HOLE_TOP,
    TS_TOP_BOT,
    TS_SIDE_BOT,
    TS_SIDE_ALT_BOT,
    TS_SIDE_ALT_HALF,
    TS_DEST_TOP_BOT,
    TS_DEST_SIDE_BOT,
    TS_NONE,
    TS_HOLE_BOT
};

#define TILE_PIXEL_SIZE_X 18
#define TILE_PIXEL_SIZE_Y 12

/* Calculate the screen-space distance for a given world-space distance */
#define SCREEN_DELTA_X_CONST(x) ((x) * TILE_PIXEL_SIZE_X / TILE_SIZE)
#define SCREEN_DELTA_Y_CONST(y) ((y) * TILE_PIXEL_SIZE_Y / TILE_SIZE)

#define SCREEN_DELTA_X(x) (div256_24((x) * TILE_PIXEL_SIZE_X))
#define SCREEN_DELTA_Y(y) (div256_24((y) * TILE_PIXEL_SIZE_Y))

//Offset from sides of screen
#define MAP_OFFSET_X ((LCD_WIDTH - SCREEN_DELTA_X_CONST(LEVEL_SIZE_X * TILE_SIZE)) / 2)
// Offset from top of the screen
#define MAP_OFFSET_Y 12

/* Calculate the screen-space position for a given world-space point */
#define SCREEN_X_CONST(x) (SCREEN_DELTA_X_CONST(x) + MAP_OFFSET_X)
#define SCREEN_Y_CONST(y) (SCREEN_DELTA_Y_CONST(y) + MAP_OFFSET_Y)

#define SCREEN_X(x) (SCREEN_DELTA_X(x) + MAP_OFFSET_X)
#define SCREEN_Y(y) (SCREEN_DELTA_Y(y) + MAP_OFFSET_Y)

#define SPRITE_OFFSET_X 20
#define SPRITE_OFFSET_Y 22

void init_graphics();

void render(); //Render tilemap, tanks, and UI during the game loop

void get_sprite_footprint(gfx_region_t *out, const PhysicsBody *phys, gfx_sprite_t **sprites, const uint8_t *offsets_x,
                          const uint8_t *offsets_y, uint8_t anim);

extern bool needs_redraw; // set if the entire map should be redrawn

#endif
