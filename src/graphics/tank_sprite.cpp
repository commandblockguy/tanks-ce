#include <graphx.h>
#include "tank_sprite.h"
#include "dynamic_sprites.h"
#include "graphics.h"
#include "repalettize.h"
#include "../game.h"
#include "../gui/error.h"

#define MAX_SPRITES 30

// this is supposed to go in dynamic_sprites.c, but a weird (linker?) issue causes this array to be filled with NULL if I do that
gfx_sprite_t *const enemy_bases_unconv[9] = {en_base_0, en_base_1, en_base_2, en_base_3, en_base_4, en_base_5,
                                             en_base_6, en_base_7, en_base_8};

gfx_sprite_t *const enemy_turrets_unconv[9] = {en_turret_0, en_turret_1, en_turret_2, en_turret_3, en_turret_4,
                                               en_turret_5, en_turret_6, en_turret_7, en_turret_8};

TankSpriteCache base_cache = TankSpriteCache(enemy_bases_unconv, tank_bases);
TankSpriteCache turret_cache = TankSpriteCache(enemy_turrets_unconv, tank_turrets);

TankSpriteCache::TankSpriteCache(gfx_sprite_t * const *unconvs, gfx_sprite_t *(*sprites)[16]) {
    this->unconvs = unconvs;
    this->sprites = sprites;
    this->allocated = 0;
}

gfx_sprite_t *TankSpriteCache::get(tank_type_t type, uint8_t n) {
    if(type != PLAYER) last_used[type - 1][n] = game.tick;
    if(sprites[type][n]) {
        return sprites[type][n];
    }

    // Remove the sprite that was displayed the longest time ago if there are too many
    if(allocated >= MAX_SPRITES) {
        remove_oldest();
    }

    if(n > 8) {
        const gfx_sprite_t *mirror = get(type, 16 - n);
        gfx_sprite_t *sprite = gfx_MallocSprite(mirror->width, mirror->height);
        if(!sprite) ERROR("Failed to allocate tank sprite");
        gfx_FlipSpriteY(mirror, sprite);
        sprites[type][n] = sprite;
        allocated++;
        return sprite;
    } else {
        const gfx_sprite_t *unconv = unconvs[n];
        gfx_sprite_t *sprite = gfx_MallocSprite(unconv->width, unconv->height);
        if(!sprite) ERROR("Failed to allocate tank sprite");

        palette_map_t palette_map;
        get_enemy_palette_map(palette_map, type);
        repalettize_sprite(sprite, unconv, palette_map);
        sprites[type][n] = sprite;
        allocated++;
        return sprite;
    }
}

void TankSpriteCache::free_all() {
    for(tank_type_t type = 1; type < NUM_TANK_TYPES; type++) {
        for(uint8_t n = 0; n < 16; n++) {
            if(sprites[type][n]) {
                free(sprites[type][n]);
                sprites[type][n] = nullptr;
                last_used[type][n] = 0;
            }
        }
    }
    allocated = 0;
}

void TankSpriteCache::remove_oldest() {
    uint24_t min = UINT24_MAX;
    gfx_sprite_t **ptr = nullptr;

    for(tank_type_t type = 1; type < NUM_TANK_TYPES; type++) {
        for(uint8_t n = 0; n < 16; n++) {
            if(sprites[type][n] && game.tick - last_used[type - 1][n] < min) {
                min = game.tick - last_used[type - 1][n];
                ptr = &sprites[type][n];
            }
        }
    }
    free(*ptr);
    *ptr = nullptr;
    allocated--;
}

void free_tank_sprites() {
    base_cache.free_all();
    turret_cache.free_all();
}
