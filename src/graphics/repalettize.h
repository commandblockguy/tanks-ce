#ifndef TANKS_REPALETTIZE_H
#define TANKS_REPALETTIZE_H

#include "../objects/tank.h"
#include <graphx.h>

#define NUM_NON_DYNAMIC_COLORS 4
#define NUM_DYNAMIC_COLORS 5
#define PALETTE_MAP_SIZE (NUM_NON_DYNAMIC_COLORS + NUM_DYNAMIC_COLORS)
typedef uint8_t palette_map_t[PALETTE_MAP_SIZE];

void repalettize_sprite(gfx_sprite_t *out, const gfx_sprite_t *in, const uint8_t *map);

void get_sprite_shadow(gfx_sprite_t *out, gfx_sprite_t *in, uint8_t shadow_color);

void get_enemy_palette_map(uint8_t *out, tank_type_t type);

#endif //TANKS_REPALETTIZE_H
