#include <graphx.h>
#include "gfx/gfx.h"
#include "gfx/offsets/offsets.h"
#include "tank.h"

#define tank_sprite_total_width 40
#define tank_sprite_total_height 30

gfx_UninitedSprite(pl_base_9, pl_base_7_width, pl_base_7_height);
gfx_UninitedSprite(pl_base_10, pl_base_6_width, pl_base_6_height);
gfx_UninitedSprite(pl_base_11, pl_base_5_width, pl_base_5_height);
gfx_UninitedSprite(pl_base_12, pl_base_4_width, pl_base_4_height);
gfx_UninitedSprite(pl_base_13, pl_base_3_width, pl_base_3_height);
gfx_UninitedSprite(pl_base_14, pl_base_2_width, pl_base_2_height);
gfx_UninitedSprite(pl_base_15, pl_base_1_width, pl_base_1_height);

gfx_UninitedSprite(pl_turret_9, pl_turret_7_width, pl_turret_7_height);
gfx_UninitedSprite(pl_turret_10, pl_turret_6_width, pl_turret_6_height);
gfx_UninitedSprite(pl_turret_11, pl_turret_5_width, pl_turret_5_height);
gfx_UninitedSprite(pl_turret_12, pl_turret_4_width, pl_turret_4_height);
gfx_UninitedSprite(pl_turret_13, pl_turret_3_width, pl_turret_3_height);
gfx_UninitedSprite(pl_turret_14, pl_turret_2_width, pl_turret_2_height);
gfx_UninitedSprite(pl_turret_15, pl_turret_1_width, pl_turret_1_height);

gfx_sprite_t *tank_bases[NUM_TANK_TYPES][16] = {
        {
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
        }
};

gfx_sprite_t *tank_turrets[NUM_TANK_TYPES][16] = {
        {
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
        }
};

const uint8_t base_x_offsets[16] =  {
        pl_base_0_offset_x,
        pl_base_1_offset_x,
        pl_base_2_offset_x,
        pl_base_3_offset_x,
        pl_base_4_offset_x,
        pl_base_5_offset_x,
        pl_base_6_offset_x,
        pl_base_7_offset_x,
        pl_base_8_offset_x,
        tank_sprite_total_width - pl_base_7_offset_x - pl_base_7_width,
        tank_sprite_total_width - pl_base_6_offset_x - pl_base_6_width,
        tank_sprite_total_width - pl_base_5_offset_x - pl_base_5_width,
        tank_sprite_total_width - pl_base_4_offset_x - pl_base_4_width,
        tank_sprite_total_width - pl_base_3_offset_x - pl_base_3_width,
        tank_sprite_total_width - pl_base_2_offset_x - pl_base_2_width,
        tank_sprite_total_width - pl_base_1_offset_x - pl_base_1_width,
};

const uint8_t base_y_offsets[16] =  {
        pl_base_0_offset_y,
        pl_base_1_offset_y,
        pl_base_2_offset_y,
        pl_base_3_offset_y,
        pl_base_4_offset_y,
        pl_base_5_offset_y,
        pl_base_6_offset_y,
        pl_base_7_offset_y,
        pl_base_8_offset_y,
        pl_base_7_offset_y,
        pl_base_6_offset_y,
        pl_base_5_offset_y,
        pl_base_4_offset_y,
        pl_base_3_offset_y,
        pl_base_2_offset_y,
        pl_base_1_offset_y,
};

const uint8_t turret_x_offsets[16] =  {
        pl_turret_0_offset_x,
        pl_turret_1_offset_x,
        pl_turret_2_offset_x,
        pl_turret_3_offset_x,
        pl_turret_4_offset_x,
        pl_turret_5_offset_x,
        pl_turret_6_offset_x,
        pl_turret_7_offset_x,
        pl_turret_8_offset_x,
        tank_sprite_total_width - pl_turret_7_offset_x - pl_turret_7_width,
        tank_sprite_total_width - pl_turret_6_offset_x - pl_turret_6_width,
        tank_sprite_total_width - pl_turret_5_offset_x - pl_turret_5_width,
        tank_sprite_total_width - pl_turret_4_offset_x - pl_turret_4_width,
        tank_sprite_total_width - pl_turret_3_offset_x - pl_turret_3_width,
        tank_sprite_total_width - pl_turret_2_offset_x - pl_turret_2_width,
        tank_sprite_total_width - pl_turret_1_offset_x - pl_turret_1_width,
};

const uint8_t turret_y_offsets[16] =  {
        pl_turret_0_offset_y,
        pl_turret_1_offset_y,
        pl_turret_2_offset_y,
        pl_turret_3_offset_y,
        pl_turret_4_offset_y,
        pl_turret_5_offset_y,
        pl_turret_6_offset_y,
        pl_turret_7_offset_y,
        pl_turret_8_offset_y,
        pl_turret_7_offset_y,
        pl_turret_6_offset_y,
        pl_turret_5_offset_y,
        pl_turret_4_offset_y,
        pl_turret_3_offset_y,
        pl_turret_2_offset_y,
        pl_turret_1_offset_y,
};
