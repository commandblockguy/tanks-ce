#include "repalettize.h"

#include <cstdint>
#include <graphx.h>

#include "../util/util.h"
#include "graphics.h"

void repalettize_sprite(gfx_sprite_t *out, const gfx_sprite_t *in, const uint8_t *map) {
    out->width = in->width;
    out->height = in->height;

    size_t size = in->width * in->height;

    for(uint i = 0; i < size; i++) {
        out->data[i] = map[in->data[i]];
    }
}

void get_sprite_shadow(gfx_sprite_t *out, gfx_sprite_t *in, uint8_t shadow_color) {
    out->width = in->width;
    out->height = in->height;
    for(uint i = 0; i < in->width * in->height; i++) {
        uint8_t px = in->data[i];
        out->data[i] = px ? shadow_color : 0;
    }
}

void get_enemy_palette_map(uint8_t *out, tank_type_t type) {
    out[0] = 0;
    out[1] = COL_ENEMY_TANK_WOOD_1;
    out[2] = COL_ENEMY_TANK_WOOD_2;
    out[3] = COL_ENEMY_TANK_WOOD_3;
    for(uint8_t i = 0; i < NUM_DYNAMIC_COLORS; i++)
        out[NUM_NON_DYNAMIC_COLORS + i] = 256 - NUM_DYNAMIC_COLORS * NUM_TANK_TYPES + NUM_DYNAMIC_COLORS * type + i;
}
