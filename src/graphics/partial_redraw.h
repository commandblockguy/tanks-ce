#ifndef TANKS_PARTIAL_REDRAW_H
#define TANKS_PARTIAL_REDRAW_H

#include <graphx.h>

#include "../util/util.h"

struct pdraw_sprite {
    uint x;
    uint8_t y;
    gfx_sprite_t *sprite;
};

// Store a rectangular region for redraw on the next relevant frame
bool pdraw_RectRegion(uint x, uint8_t y, uint8_t width, uint8_t height);

bool pdraw_RectRegion(const gfx_region_t *region);

// Draw a sprite, storing the background sprite for later redraw
void pdraw_Sprite_NoClip(gfx_sprite_t *sprite, uint x, uint8_t y);

void pdraw_TransparentSprite(const gfx_sprite_t *sprite, uint x, uint8_t y);

void pdraw_TransparentSprite_NoClip(gfx_sprite_t *sprite, uint x, uint8_t y);

// Remove sprites previously drawn with draw_Sprite from the screen
void pdraw_RemoveSprites();

// Free all sprites on both buffers without undrawing them
void pdraw_FreeAll();

#endif //TANKS_PARTIAL_REDRAW_H
