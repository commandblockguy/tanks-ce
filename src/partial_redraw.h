#ifndef TANKS_PARTIAL_REDRAW_H
#define TANKS_PARTIAL_REDRAW_H

typedef struct {
    uint24_t x;
    uint8_t y;
    gfx_sprite_t *sprite;
} pdraw_sprite_t;

// Store a rectangular region for redraw on the next relevant frame
pdraw_sprite_t *pdraw_RectRegion(uint24_t x, uint8_t y, uint8_t width, uint8_t height);
// Draw a sprite, storing the background sprite for later redraw
void pdraw_Sprite_NoClip(gfx_sprite_t *sprite, uint24_t x, uint8_t y);
void pdraw_TransparentSprite_NoClip(gfx_sprite_t *sprite, uint24_t x, uint8_t y);
// Remove sprites previously drawn with draw_Sprite from the screen
void pdraw_RemoveSprites(void);
// Free all sprites on both buffers without undrawing them
void pdraw_FreeAll(void);

#endif //TANKS_PARTIAL_REDRAW_H
