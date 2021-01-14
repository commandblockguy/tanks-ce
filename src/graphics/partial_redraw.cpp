#include "partial_redraw.h"

#include <debug.h>
#include <TINYSTL/vector.h>

#include "../util/profiler.h"

bool pdraw_current_buffer = false;
tinystl::vector<pdraw_sprite_t> pdraw_sprites[2];

bool pdraw_RectRegion(uint x, uint8_t y, uint8_t width, uint8_t height) {
    profiler_add(store_bg);
    gfx_sprite_t *bg_sprite = gfx_MallocSprite(width, height);
    if(bg_sprite == nullptr) {
        dbg_printf("Failed to allocate memory for redraw sprite\n");
        profiler_end(store_bg);
        return false;
    }

    gfx_GetSprite(bg_sprite, x, y);
    pdraw_sprites[pdraw_current_buffer].push_back({x, y, bg_sprite});

    profiler_end(store_bg);
    return true;
}

bool pdraw_RectRegion(const gfx_region_t *region) {
    return pdraw_RectRegion(region->xmin, region->ymin, region->xmax - region->xmin, region->ymax - region->ymin);
}

void pdraw_Sprite_NoClip(gfx_sprite_t *sprite, uint x, uint8_t y) {
    if(pdraw_RectRegion(x, y, sprite->width, sprite->height))
        gfx_Sprite_NoClip(sprite, x, y);
}

void pdraw_TransparentSprite(const gfx_sprite_t *sprite, uint x, uint8_t y) {
    gfx_region_t region = {.xmin = (int)x, .ymin = y, .xmax = (int)(x + sprite->width), .ymax = y + sprite->height};
    if(gfx_GetClipRegion(&region))
        if(pdraw_RectRegion(region.xmin, region.ymin, region.xmax - region.xmin, region.ymax - region.ymin))
            gfx_TransparentSprite(sprite, x, y);
}

void pdraw_TransparentSprite_NoClip(gfx_sprite_t *sprite, uint x, uint8_t y) {
    if(pdraw_RectRegion(x, y, sprite->width, sprite->height))
        gfx_TransparentSprite_NoClip(sprite, x, y);
}

void pdraw_RemoveSprites() {
    pdraw_current_buffer = !pdraw_current_buffer;
    while(!pdraw_sprites[pdraw_current_buffer].empty()) {
        pdraw_sprite_t & psprite = pdraw_sprites[pdraw_current_buffer].back();
        gfx_Sprite_NoClip(psprite.sprite, psprite.x, psprite.y);
        free(psprite.sprite);
        pdraw_sprites[pdraw_current_buffer].pop_back();
    }
}

void pdraw_FreeAll() {
    for(auto & pdraw_sprite : pdraw_sprites) {
        for(pdraw_sprite_t & psprite : pdraw_sprite) {
            free(psprite.sprite);
        }
        pdraw_sprite.clear();
    }
}
