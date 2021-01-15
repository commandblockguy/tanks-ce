#ifndef H_GRAPHICS
#define H_GRAPHICS

#include <stdbool.h>
#include <cstddef>
#include <cstdint>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include <string.h>

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

#define NUM_NON_DYNAMIC_COLORS 4
#define NUM_DYNAMIC_COLORS 5
#define PALETTE_MAP_SIZE (NUM_NON_DYNAMIC_COLORS + NUM_DYNAMIC_COLORS)
typedef uint8_t palette_map_t[PALETTE_MAP_SIZE];

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

typedef struct {
    uint8_t base_0_data[en_base_0_size];
    uint8_t base_1_data[en_base_1_size];
    uint8_t base_2_data[en_base_2_size];
    uint8_t base_3_data[en_base_3_size];
    uint8_t base_4_data[en_base_4_size];
    uint8_t base_5_data[en_base_5_size];
    uint8_t base_6_data[en_base_6_size];
    uint8_t base_7_data[en_base_7_size];
    uint8_t base_8_data[en_base_8_size];
    uint8_t base_9_data[en_base_7_size];
    uint8_t base_10_data[en_base_6_size];
    uint8_t base_11_data[en_base_5_size];
    uint8_t base_12_data[en_base_4_size];
    uint8_t base_13_data[en_base_3_size];
    uint8_t base_14_data[en_base_2_size];
    uint8_t base_15_data[en_base_1_size];

    uint8_t turret_0_data[en_turret_0_size];
    uint8_t turret_1_data[en_turret_1_size];
    uint8_t turret_2_data[en_turret_2_size];
    uint8_t turret_3_data[en_turret_3_size];
    uint8_t turret_4_data[en_turret_4_size];
    uint8_t turret_5_data[en_turret_5_size];
    uint8_t turret_6_data[en_turret_6_size];
    uint8_t turret_7_data[en_turret_7_size];
    uint8_t turret_8_data[en_turret_8_size];
    uint8_t turret_9_data[en_turret_7_size];
    uint8_t turret_10_data[en_turret_6_size];
    uint8_t turret_11_data[en_turret_5_size];
    uint8_t turret_12_data[en_turret_4_size];
    uint8_t turret_13_data[en_turret_3_size];
    uint8_t turret_14_data[en_turret_2_size];
    uint8_t turret_15_data[en_turret_1_size];
} spriteset_t;

#define TILE_PIXEL_SIZE_X 18
#define TILE_PIXEL_SIZE_Y 12

/* Calculate the screen-space distance for a given world-space distance */
#define SCREEN_DELTA_X_CONST(x) ((x) * TILE_PIXEL_SIZE_X / TILE_SIZE)
#define SCREEN_DELTA_Y_CONST(y) ((y) * TILE_PIXEL_SIZE_Y / TILE_SIZE)

#define SCREEN_DELTA_X(x) (div256_24((x) * TILE_PIXEL_SIZE_X))
#define SCREEN_DELTA_Y(y) (div256_24((y) * TILE_PIXEL_SIZE_Y))

#define HALF_TILE_PIXEL_HEIGHT (TILE_PIXEL_SIZE_Y / 2)

//Offset from sides of screen
#define MAP_OFFSET_X ((LCD_WIDTH - SCREEN_DELTA_X_CONST(LEVEL_SIZE_X * TILE_SIZE)) / 2)
// Offset from top of the screen
#define MAP_OFFSET_Y 12

/* Calculate the screen-space position for a given world-space point */
#define SCREEN_X_CONST(x) (SCREEN_DELTA_X_CONST(x) + MAP_OFFSET_X)
#define SCREEN_Y_CONST(y) (SCREEN_DELTA_Y_CONST(y) + MAP_OFFSET_Y)

#define SCREEN_X(x) (SCREEN_DELTA_X(x) + MAP_OFFSET_X)
#define SCREEN_Y(y) (SCREEN_DELTA_Y(y) + MAP_OFFSET_Y)

// todo: wat
#define TILEMAP_OFFSET 2
#define TILEMAP_HEIGHT (2 * LEVEL_SIZE_Y + TILEMAP_OFFSET)
#define TILEMAP_WIDTH LEVEL_SIZE_X
#define TILEMAP_BASE_Y (SCREEN_Y_CONST(0) - HALF_TILE_PIXEL_HEIGHT * TILEMAP_OFFSET)

#define SPRITE_OFFSET_X 20
#define SPRITE_OFFSET_Y 22

// todo: get an actual value?
#define AIM_INDICATOR_DOT_DISTANCE (2 * TILE_SIZE)
#define AIM_INDICATOR_RADIUS 2

void init_graphics();

void render(); //Render tilemap, tanks, and UI during the game loop

void draw_line(line_seg_t *ls);

void generate_bg_tilemap();

void init_tank_sprites(tank_type_t type);

void free_tank_sprites(tank_type_t type);

void get_sprite_footprint(gfx_region_t *out, const PhysicsBody *phys, gfx_sprite_t **sprites, const uint8_t *offsets_x,
                          const uint8_t *offsets_y, uint8_t anim);

void redraw_tiles(const gfx_region_t *region, uint8_t height);

void repalettize_sprite(gfx_sprite_t *out, const gfx_sprite_t *in, const uint8_t *map);

void get_sprite_shadow(gfx_sprite_t *out, gfx_sprite_t *in, uint8_t shadow_color);

void get_enemy_palette_map(uint8_t *out, tank_type_t type);

extern bool needs_redraw; // set if the entire map should be redrawn

#endif
