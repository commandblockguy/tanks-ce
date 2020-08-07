#ifndef TANKS_DYNAMIC_SPRITES_H
#define TANKS_DYNAMIC_SPRITES_H

extern gfx_sprite_t *tank_bases[NUM_TANK_TYPES][16];
extern gfx_sprite_t *tank_turrets[NUM_TANK_TYPES][16];

extern gfx_sprite_t *shell_sprites[16];

extern gfx_sprite_t *const enemy_bases_unconv[9];
extern gfx_sprite_t *const enemy_turrets_unconv[9];

extern const uint8_t base_x_offsets[16];
extern const uint8_t base_y_offsets[16];
extern const uint8_t turret_x_offsets[16];
extern const uint8_t turret_y_offsets[16];

extern const uint8_t shell_x_offsets[16];
extern const uint8_t shell_y_offsets[16];

#endif //TANKS_DYNAMIC_SPRITES_H
