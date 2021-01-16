#ifndef TANKS_TANK_SPRITE_H
#define TANKS_TANK_SPRITE_H

#include "graphics.h"

class TankSpriteCache {
public:
    TankSpriteCache(gfx_sprite_t * const *unconvs, gfx_sprite_t *(*sprites)[16]);
    gfx_sprite_t *get(tank_type_t type, uint8_t n);
    void free_all();
private:
    void remove_oldest();
    uint24_t last_used[NUM_TANK_TYPES - 1][16];
    gfx_sprite_t * const *unconvs;
    gfx_sprite_t *(*sprites)[16];
    uint8_t allocated;
};

extern TankSpriteCache base_cache;
extern TankSpriteCache turret_cache;

void free_tank_sprites();

#endif //TANKS_TANK_SPRITE_H
