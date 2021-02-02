#include "../game.h"
#include <tice.h>
#include <keypadc.h>
#include <graphx.h>
#include "banner.h"
#include "../graphics/graphics.h"

#define KILL_COUNTER_END_X (SCREEN_DELTA_X_CONST(2.75 * TILE_SIZE))
#define KILL_COUNTER_INNER_END_X (SCREEN_DELTA_X_CONST(2.4 * TILE_SIZE))
#define KILL_COUNTER_RADIUS ((uint)SCREEN_DELTA_Y_CONST(TILE_SIZE) - 1)
#define KILL_COUNTER_INNER_RADIUS ((uint)SCREEN_DELTA_Y_CONST(0.75 * TILE_SIZE))
#define KILL_COUNTER_HEIGHT (2 * KILL_COUNTER_RADIUS + 1)
#define KILL_COUNTER_INNER_HEIGHT (2 * KILL_COUNTER_INNER_RADIUS + 1)
#define KILL_COUNTER_Y (SCREEN_Y_CONST((LEVEL_SIZE_Y - 2) * TILE_SIZE))
#define KILL_COUNTER_INNER_Y (KILL_COUNTER_Y + KILL_COUNTER_RADIUS - KILL_COUNTER_INNER_RADIUS)

void update_game_kill_counter_current_buffer(uint8_t kills) {
    uint8_t digits = 1 + (kills > 9) + (kills > 99);
    uint8_t width = gfx_GetCharWidth('1') * digits;
    uint8_t x = KILL_COUNTER_INNER_END_X - KILL_COUNTER_INNER_RADIUS - width;
    gfx_SetColor(COL_WHITE);
    gfx_FillRectangle_NoClip(x, KILL_COUNTER_INNER_Y, width, KILL_COUNTER_INNER_HEIGHT);
    gfx_SetTextFGColor(COL_LIVES_TXT);
    gfx_SetTextXY(x, KILL_COUNTER_INNER_Y + 5);
    gfx_PrintUInt(kills, digits);
}

// todo: ??????
void update_game_kill_counter(uint8_t kills, bool force) {
    static uint8_t last = -1;
    if(!force && last == kills) return;
    last = kills;
    gfx_SetDrawScreen();
    update_game_kill_counter_current_buffer(kills);
    gfx_SetDrawBuffer();
    update_game_kill_counter_current_buffer(kills);
}

void display_game_kill_counter() {
    gfx_SetColor(COL_LIVES_TXT); // todo: add a bluer color
    gfx_FillCircle_NoClip(KILL_COUNTER_END_X - KILL_COUNTER_RADIUS, KILL_COUNTER_Y + KILL_COUNTER_RADIUS,
                          KILL_COUNTER_RADIUS);
    gfx_FillRectangle_NoClip(0, KILL_COUNTER_Y, KILL_COUNTER_END_X - KILL_COUNTER_RADIUS, KILL_COUNTER_HEIGHT);

    gfx_SetColor(COL_WHITE);
    gfx_FillCircle_NoClip(KILL_COUNTER_INNER_END_X - KILL_COUNTER_INNER_RADIUS - 1,
                          KILL_COUNTER_INNER_Y + KILL_COUNTER_INNER_RADIUS, KILL_COUNTER_INNER_RADIUS);
    gfx_FillRectangle_NoClip(0, KILL_COUNTER_INNER_Y, KILL_COUNTER_INNER_END_X - KILL_COUNTER_INNER_RADIUS - 1,
                             KILL_COUNTER_INNER_HEIGHT);
}

// todo: apparently this shows the number of alive tanks, not the number of lives
void display_game_banner(uint8_t mission, uint8_t lives) {
    // todo: check if the compiler optimizes these properly
    const uint8_t banner_width = SCREEN_DELTA_X_CONST(10.5 * TILE_SIZE);
    const uint8_t banner_height = 14;
    const uint base_x = (LCD_WIDTH - banner_width) / 2;
    const uint8_t base_y = SCREEN_Y_CONST((LEVEL_SIZE_Y - 2) * TILE_SIZE) + 3;
    const uint8_t text_x = base_x + 18;
    const uint text2_x = base_x + 122;
    const uint8_t text_y = base_y + 3;
    const uint8_t rhomb_width = 7;

    gfx_SetColor(COL_RHOM_1);
    gfx_HorizLine(base_x + 1, base_y, banner_width - 2);
    gfx_HorizLine(base_x + 1, base_y + banner_height - 1, banner_width - 2);
    gfx_FillRectangle_NoClip(base_x, base_y + 1, banner_width, banner_height - 2);

    gfx_SetColor(COL_RHOM_2);
    for(uint x = base_x; x < base_x + banner_width; x += rhomb_width) {
        gfx_FillTriangle_NoClip(x, base_y + banner_height / 2 - 1, x + rhomb_width - 1, base_y + banner_height / 2 - 1,
                                x + rhomb_width / 2, base_y);
        gfx_FillTriangle_NoClip(x, base_y + banner_height / 2 - 1, x + rhomb_width - 1, base_y + banner_height / 2 - 1,
                                x + rhomb_width / 2, base_y + banner_height - 1);
    }

    gfx_SetTextXY(text_x + 1, text_y + 1);
    gfx_SetTextFGColor(COL_TXT_SHADOW);
    gfx_PrintString("Mission ");
    gfx_PrintUInt(mission, 1);
    gfx_SetTextXY(text2_x + 1, text_y + 1);
    gfx_PrintString("x ");
    gfx_PrintUInt(lives, 1);

    gfx_SetTextXY(text_x, text_y);
    gfx_SetTextFGColor(COL_BG);
    gfx_PrintString("Mission ");
    gfx_PrintUInt(mission, 1);
    gfx_SetTextXY(text2_x, text_y);
    gfx_PrintString("x ");
    gfx_PrintUInt(lives, 1);
}