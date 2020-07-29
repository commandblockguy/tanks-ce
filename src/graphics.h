#ifndef H_GRAPHICS
#define H_GRAPHICS

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "level.h"

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
    COL_ENEMY_TANK_WOOD_2 = 13
};

#define NUM_DYNAMIC_COLORS 5

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

#define TILE_PIXEL_SIZE_X 14
#define TILE_PIXEL_SIZE_Y 10

/* Calculate the screen-space distance for a given world-space distance */
#define SCREEN_DELTA_X(x) ((x) * TILE_PIXEL_SIZE_X / 256)
#define SCREEN_DELTA_Y(y) ((y) * TILE_PIXEL_SIZE_Y / 256)

#define TILE_PIXEL_WIDTH SCREEN_DELTA_X(TILE_SIZE)
#define TILE_PIXEL_HEIGHT 10
#define HALF_TILE_PIXEL_HEIGHT (TILE_PIXEL_HEIGHT / 2)

#define TILEMAP_OFFSET 4
#define TILEMAP_HEIGHT (2 * LEVEL_SIZE_Y + TILEMAP_OFFSET)
#define TILEMAP_BASE_Y (SCREEN_Y(0) - HALF_TILE_PIXEL_HEIGHT * TILEMAP_OFFSET)

//Offset from sides of screen
#define MAP_OFFSET_X ((LCD_WIDTH  - SCREEN_DELTA_X(LEVEL_SIZE_X * TILE_SIZE)) / 2)
// Offset from top of the screen
#define MAP_OFFSET_Y ((LCD_HEIGHT - SCREEN_DELTA_Y(LEVEL_SIZE_Y * TILE_SIZE)) / 2)

/* Calculate the screen-space position for a given world-space point */
#define SCREEN_X(x) (SCREEN_DELTA_X(x) + MAP_OFFSET_X)
#define SCREEN_Y(y) (SCREEN_DELTA_Y(y) + MAP_OFFSET_Y)

#define TANK_SPRITE_OFFSET_X 8
#define TANK_SPRITE_OFFSET_Y 11
#define TANK_SPRITE_SIZE_X 28
#define TANK_SPRITE_SIZE_Y 24

void initGraphics(void);

void render(level_t *level); //Render tilemap, tanks, and UI during the game loop

void renderPhysicsBody(physicsBody_t *phys);

void displayUI(void); //Display UI during a mission

void drawLine(lineSeg_t* ls);

void generate_bg_tilemap(void);

extern bool needs_redraw; // set if the entire map should be redrawn

#endif
