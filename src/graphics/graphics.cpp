#include "graphics.h"

#include <sys/lcd.h>

#include "../util/profiler.h"
#include "partial_redraw.h"
#include "dynamic_sprites.h"
#include "../data/gfx/enemy_pal.h"
#include "tiles.h"
#include "../game.h"
#include "../gui/banner.h"
#include "../gui/aim_indicator.h"

bool needs_redraw;

void init_graphics() {
    gfx_Begin(); //Set up draw bits
    gfx_SetPalette(palette, sizeof_palette, 0);
    gfx_SetPalette(enemy_pal, sizeof_enemy_pal, 256 - sizeof_enemy_pal / 2);
    gfx_SetDrawBuffer();
    gfx_SetTextFGColor(COL_RIB_SHADOW); // todo: verify

    for(uint8_t i = 1; i < 8; i++) {
        gfx_FlipSpriteY(tank_bases[PLAYER][i], tank_bases[PLAYER][16 - i]);
        gfx_FlipSpriteY(tank_turrets[PLAYER][i], tank_turrets[PLAYER][16 - i]);
        gfx_FlipSpriteY(tread_sprites[i], tread_sprites[16 - i]);
        gfx_FlipSpriteY(shell_sprites[i], shell_sprites[16 - i]);
    }
}

void get_sprite_footprint(gfx_region_t *out, const PhysicsBody *phys, gfx_sprite_t **sprites, const uint8_t *offsets_x,
                          const uint8_t *offsets_y, uint8_t anim) {
    profiler_add(sprite_footprint);
    uint base_x = SCREEN_X(phys->center_x()) - SPRITE_OFFSET_X;
    uint8_t base_y = SCREEN_Y(phys->center_y()) - SPRITE_OFFSET_Y;
    gfx_sprite_t *sprite = sprites[anim];
    out->xmin = base_x + offsets_x[anim];
    out->ymin = base_y + offsets_y[anim];
    out->xmax = out->xmin + sprite->width;
    out->ymax = out->ymin + sprite->height;

    profiler_end(sprite_footprint);
}

void render() {
    profiler_start(graphics);

    profiler_start(gfx_wait);
    gfx_Wait();
    profiler_end(gfx_wait);

    if(needs_redraw) {
        profiler_start(tilemap);
        generate_bg_tilemap();
        draw_tilemap();
        display_game_banner(game.mission + 1, game.lives);
        display_game_kill_counter();
        gfx_BlitBuffer();
        pdraw_FreeAll();
        update_game_kill_counter(game.total_kills, true);
        needs_redraw = false;
        profiler_end(tilemap);
    }

    update_game_kill_counter(game.total_kills, false);

    profiler_start(undraw);
    pdraw_RemoveSprites();
    profiler_end(undraw);

    profiler_start(render_pbs);
    // restrict drawing to only the play area, to prevent the banners from being overwritten
    gfx_SetClipRegion(SCREEN_X_CONST(0), SCREEN_Y_CONST(-TILE_SIZE), SCREEN_X_CONST(LEVEL_SIZE_X * TILE_SIZE),
                      SCREEN_Y_CONST((LEVEL_SIZE_Y - 2) * TILE_SIZE));
    for(uint8_t layer = 0; layer < 3; layer++) {
        for(auto it: PhysicsBody::objects) {
            it->render(layer);
        }
    }
    gfx_SetClipRegion(0, 0, LCD_WIDTH, LCD_HEIGHT);
    profiler_end(render_pbs);

    draw_aim_dots();

    profiler_start(swapdraw);
    gfx_SwapDraw();
    profiler_end(swapdraw);

    profiler_end(graphics);
}
