#include "kill_counts.h"

#include <graphx.h>
#include <keypadc.h>
#include <sys/timers.h>

#include "../game.h"
#include "../graphics/repalettize.h"
#include "../graphics/partial_redraw.h"
#include "../graphics/graphics.h"

// todo: improve
void display_kill_counts() {
    const uint bg_width = 120;
    const uint base_x = (LCD_WIDTH - bg_width) / 2;

    const uint8_t bands_base_y = 17;
    const uint8_t band_height = 3;
    const uint8_t num_bands = 4;
    const uint8_t bands_total_height = num_bands * 2 * band_height - band_height;

    const uint8_t text_base_y = bands_base_y + bands_total_height + 20;
    const uint8_t line_spacing = 18;
    const uint text_center_point = LCD_WIDTH / 2 + 19;
    const uint8_t char_width = 8;

    const uint sprite_center = LCD_WIDTH / 2 - 19;
    const uint sprite_base_y = text_base_y - 2;

    const uint8_t bottom_band_y = LCD_HEIGHT - 36;

    const uint final_box_width = 38;
    const uint8_t final_box_height = 19;
    const uint final_box_x = text_center_point - final_box_width / 2;
    const uint8_t final_box_y = bottom_band_y + 2 * band_height;
    const uint8_t box_text_y = final_box_y + (final_box_height - 6 * 2) / 2;

    const char results[] = "Results";

    gfx_SetColor(COL_BG);
    gfx_FillRectangle(base_x, 0, bg_width, LCD_HEIGHT);

    gfx_SetColor(COL_OLIVE_BANDS);
    for(uint8_t i = 0; i < num_bands; i++)
        gfx_FillRectangle(base_x, bands_base_y + i * 2 * band_height, bg_width, band_height);

    gfx_FillRectangle(base_x, bottom_band_y, bg_width, band_height);

    gfx_SetColor(COL_WHITE);
    gfx_FillRectangle(final_box_x, final_box_y, final_box_width, final_box_height);
    gfx_FillCircle(final_box_x, final_box_y + final_box_height / 2, final_box_height / 2);
    gfx_FillCircle(final_box_x + final_box_width, final_box_y + final_box_height / 2, final_box_height / 2);

    gfx_SetTextFGColor(COL_BLACK);
    gfx_SetTextScale(2, 2);
    gfx_PrintStringXY(results, (LCD_WIDTH - gfx_GetStringWidth(results)) / 2, bands_base_y + band_height);

    // todo: P1 text

    gfx_UninitedSprite(en_tank_shadow, en_tank_width, en_tank_height);
    get_sprite_shadow(en_tank_shadow, en_tank, COL_RIB_SHADOW);

    gfx_SetTextScale(1, 1);
    for(uint8_t i = 0; i < NUM_TANK_TYPES - 1; i++) {
        uint8_t num_kills = game.kills[i + 1];
        if(!num_kills) continue;

        uint8_t palette_map[9];
        get_enemy_palette_map(palette_map, i + 1);
        gfx_UninitedSprite(en_tank_palettized, en_tank_width, en_tank_height);
        repalettize_sprite(en_tank_palettized, en_tank, palette_map);

        gfx_TransparentSprite_NoClip(en_tank_shadow, sprite_center - en_tank_width / 2 + 2, sprite_base_y + line_spacing * i + 2);
        gfx_TransparentSprite_NoClip(en_tank_palettized, sprite_center - en_tank_width / 2, sprite_base_y + line_spacing * i);

        if(num_kills >= 10) {
            gfx_SetTextXY(text_center_point - char_width, text_base_y + line_spacing * i);
            gfx_PrintUInt(num_kills, 2);
        } else {
            gfx_SetTextXY(text_center_point - char_width / 2, text_base_y + line_spacing * i);
            gfx_PrintUInt(num_kills, 1);
        }
        gfx_BlitBuffer();
        delay(500);
    }

    gfx_SetTextScale(2, 2);
    if(game.total_kills > 100) {
        gfx_SetTextXY(text_center_point - char_width * 3 / 2, box_text_y);
        gfx_PrintUInt(game.total_kills, 3);
    } else if(game.total_kills > 10) {
        gfx_SetTextXY(text_center_point - char_width, box_text_y);
        gfx_PrintUInt(game.total_kills, 2);
    } else {
        gfx_SetTextXY(text_center_point - char_width / 2, box_text_y);
        gfx_PrintUInt(game.total_kills, 1);
    }

    gfx_BlitBuffer();

    while(!kb_IsDown(kb_KeyEnter) && !kb_IsDown(kb_KeyClear));
}