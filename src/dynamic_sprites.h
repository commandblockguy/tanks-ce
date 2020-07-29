#ifndef TANKS_DYNAMIC_SPRITES_H
#define TANKS_DYNAMIC_SPRITES_H

extern gfx_sprite_t * const tank_bases[NUM_TANK_TYPES][16];
extern gfx_sprite_t * const tank_turrets[NUM_TANK_TYPES][16];

gfx_sprite_t * const enemy_bases_unconv[9];
gfx_sprite_t * const enemy_turrets_unconv[9];

extern const uint8_t base_x_offsets[16];
extern const uint8_t base_y_offsets[16];
extern const uint8_t turret_x_offsets[16];
extern const uint8_t turret_y_offsets[16];

#endif //TANKS_DYNAMIC_SPRITES_H
