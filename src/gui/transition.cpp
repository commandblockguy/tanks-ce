#include "transition.h"

#include "../graphics/repalettize.h"
#include "../graphics/partial_redraw.h"
#include "../graphics/graphics.h"
#include <graphx.h>
#include <keypadc.h>
#include <sys/timers.h>

#define MISSION_START_TIME 3000
#define MISSION_NUMBER_TEXT 3
#define ENEMY_TANK_TEXT 2

void draw_tank_background(uint8_t min_y, uint8_t max_y, uint8_t lives, uint8_t life_color, const gfx_sprite_t *tank_shadow) {
    static uint8_t frame = 0, offset = 0;
    gfx_SetColor(COL_BG);
    gfx_FillRectangle_NoClip(0, 0, LCD_WIDTH, min_y);
    gfx_FillRectangle_NoClip(0, max_y, LCD_WIDTH, LCD_HEIGHT - max_y);

    const uint TANK_TILE_SIZE = 34;
    const uint8_t FRAMES_PER_PX = 3;

    frame++;
    if(frame == FRAMES_PER_PX) {
        frame = 0;
        offset++;
        if(offset == TANK_TILE_SIZE) {
            offset = 0;
        }
    }

    // Draw the background tank sprite
    for(int x = offset - TANK_TILE_SIZE; x < LCD_WIDTH; x += TANK_TILE_SIZE) {
        gfx_SetClipRegion(0, 0, LCD_WIDTH, min_y);
        for(int y = -offset; y < min_y; y += TANK_TILE_SIZE) {
            gfx_TransparentSprite(bg_tank, x, y);
        }
        gfx_SetClipRegion(0, max_y, LCD_WIDTH, LCD_HEIGHT);
        for(int y = -offset; y < LCD_WIDTH; y += TANK_TILE_SIZE) {
            gfx_TransparentSprite(bg_tank, x, y);
        }
    }
    gfx_SetClipRegion(0, 0, LCD_WIDTH, LCD_HEIGHT);

    gfx_SetTextXY((LCD_WIDTH - 8 * MISSION_NUMBER_TEXT) / 2 + 2, 186 + 2);
    gfx_SetTextFGColor(COL_RIB_SHADOW);
    gfx_PrintString("x   ");
    gfx_PrintUInt(lives, 1);

    gfx_SetTextXY((LCD_WIDTH - 8 * MISSION_NUMBER_TEXT) / 2, 186);
    gfx_SetTextFGColor(life_color);
    gfx_PrintString("x   ");
    gfx_PrintUInt(lives, 1);

    gfx_TransparentSprite_NoClip(tank_shadow, LCD_WIDTH / 2 - fg_tank_width - 30, 189);
    gfx_TransparentSprite_NoClip(fg_tank, LCD_WIDTH / 2 - fg_tank_width - 32, 187);
}

//Screen is 700 (240) pixels tall
//Background is (228,230,173)
//140 (48) pixels between the top and banner
//294 (?) pixel tall red banner
//Red banner has checkerboard with 38x52 (12x15) pixel rhombi
//Colors of rhombi are (174,49,48) and (164,45,46)
//16 (5) pixel shadow underneath (183,185,139)
//10 (3) pixel gap on top, 12 (4) on bottom
//12 (4) pixel tall gold (193,162,43) band on top and bottom
//40 (14) (12) pixels between band and text
//60 (21) (24) pixel tall mission number text w/ shadow
//50 (17) (16) pixels between
//44 (15) (16) pixel tall Enemies Remaining text w/ shadow
//50 (17) (16) pixels between text and bottom band
//Text shadow (134,36,37) has 8px (3px) offset
//# of lives text (70,127,111) - centered between bottom or ribbon and bottom of screen
void mission_start_screen(uint8_t mission, uint8_t lives, uint8_t num_tanks) {
    timer_Disable(1);
    timer_Set(1, 33 * MISSION_START_TIME);
    timer_SetReload(1, 33 * MISSION_START_TIME);
    timer_AckInterrupt(1, TIMER_RELOADED);
    timer_Enable(1, TIMER_32K, TIMER_0INT, TIMER_DOWN);

    gfx_FillScreen(COL_BG);

    gfx_SetColor(COL_RHOM_1);

    const int BANNER_TOP = 48;
    const int BANNER_BOTTOM = 148;
    const int SHADOW_BOTTOM = BANNER_BOTTOM + 4;

    gfx_FillRectangle_NoClip(0, BANNER_TOP, LCD_WIDTH, BANNER_BOTTOM - BANNER_TOP);
    gfx_SetColor(COL_RHOM_2);
    for(int x = 0; x <= LCD_WIDTH / 12; x++) {
        for(int y = 0; y < 6; y++) {
            gfx_FillTriangle(x * 12 + 6, 55 + y * 15, x * 12, y * 15 + 55 + 7, x * 12 + 12, y * 15 + 55 + 7);
            gfx_FillTriangle(x * 12 + 6, 55 + y * 15 + 15, x * 12, y * 15 + 55 + 7, x * 12 + 12, y * 15 + 55 + 7);
        }
    }

    gfx_SetColor(COL_RIB_SHADOW);
    gfx_FillRectangle_NoClip(0, BANNER_BOTTOM, LCD_WIDTH, SHADOW_BOTTOM - BANNER_BOTTOM);

    gfx_SetColor(COL_GOLD);
    gfx_FillRectangle_NoClip(0, 51, LCD_WIDTH, 4);
    gfx_FillRectangle_NoClip(0, 140, LCD_WIDTH, 4);

    //Print mission number
    gfx_SetTextScale(MISSION_NUMBER_TEXT, MISSION_NUMBER_TEXT);
    gfx_SetTextFGColor(COL_TXT_SHADOW);
    gfx_SetTextXY((LCD_WIDTH - 60 * MISSION_NUMBER_TEXT) / 2 + 3, 70 + 3);
    gfx_PrintString("Mission ");
    gfx_PrintUInt(mission + 1, 1 + (mission >= 9) + (mission >= 99));

    gfx_SetTextFGColor(COL_BG);
    gfx_SetTextXY((LCD_WIDTH - 60 * MISSION_NUMBER_TEXT) / 2, 70);
    gfx_PrintString("Mission ");
    gfx_PrintUInt(mission + 1, 1 + (mission >= 9) + (mission >= 99));


    gfx_SetTextScale(ENEMY_TANK_TEXT, ENEMY_TANK_TEXT);
    gfx_SetTextFGColor(COL_TXT_SHADOW);
    gfx_SetTextXY((LCD_WIDTH - 97 * ENEMY_TANK_TEXT) / 2 + 3, 110 + 3);
    gfx_PrintString("Enemy Tanks: ");
    gfx_PrintUInt(num_tanks, 1);
    gfx_SetTextXY((LCD_WIDTH - 8 * MISSION_NUMBER_TEXT) / 2, 150);

    gfx_SetTextFGColor(COL_BG);
    gfx_SetTextXY((LCD_WIDTH - 97 * ENEMY_TANK_TEXT) / 2, 110);
    gfx_PrintString("Enemy Tanks: ");
    gfx_PrintUInt(num_tanks, 1);

    gfx_BlitBuffer();

    gfx_UninitedSprite(fg_tank_shadow, fg_tank_width, fg_tank_height);
    get_sprite_shadow(fg_tank_shadow, fg_tank, COL_RIB_SHADOW);

    while(true) {
        if(timer_ChkInterrupt(1, TIMER_RELOADED)) {
            timer_AckInterrupt(1, TIMER_RELOADED);
            break;
        }
        if(kb_Data[1] & kb_2nd || kb_Data[1] & kb_Del || kb_Data[6] & kb_Clear) {
            while(kb_Data[1] || kb_Data[6]);
            break;
        }

        draw_tank_background(BANNER_TOP, SHADOW_BOTTOM, lives, COL_LIVES_TXT, fg_tank_shadow);

        gfx_SwapDraw();
    }

    gfx_SetTextFGColor(COL_BLACK);
    gfx_SetTextScale(1, 1);
}

void extra_life_screen(uint8_t old_lives) {
    const uint8_t BANNER_START_Y = 86;
    const uint8_t BANNER_HEIGHT = 35;
    const uint BANNER_WIDTH = 306;
    const uint8_t NOTCH_WIDTH = 8;
    const uint8_t SHADOW_HEIGHT = 5;
    const uint8_t STAR_X_OFFSET = 10;

    const uint BANNER_START_X = LCD_WIDTH / 2 - BANNER_WIDTH / 2;
    const uint BANNER_END_X = BANNER_START_X + BANNER_WIDTH;
    const uint8_t BANNER_END_Y = BANNER_START_Y + BANNER_HEIGHT;

    const uint WAIT_TIME = 33 * MISSION_START_TIME;

    timer_Disable(1);
    timer_Set(1, WAIT_TIME);
    timer_SetReload(1, WAIT_TIME);
    timer_AckInterrupt(1, TIMER_RELOADED);
    timer_Enable(1, TIMER_32K, TIMER_0INT, TIMER_DOWN);

    gfx_FillScreen(COL_BG);

    gfx_SetColor(COL_LIGHT_GREEN);
    gfx_FillRectangle_NoClip(BANNER_START_X, BANNER_START_Y, BANNER_WIDTH, BANNER_HEIGHT);

    gfx_SetColor(COL_DARK_GREEN);
    gfx_FillRectangle_NoClip(BANNER_START_X, BANNER_START_Y, BANNER_WIDTH, 2);
    gfx_FillRectangle_NoClip(BANNER_START_X, BANNER_END_Y - 2, BANNER_WIDTH, 2);
    gfx_HorizLine_NoClip(BANNER_START_X, BANNER_START_Y + 3, BANNER_WIDTH);
    gfx_HorizLine_NoClip(BANNER_START_X, BANNER_END_Y - 4, BANNER_WIDTH);

    gfx_SetColor(COL_BG);
    gfx_FillTriangle_NoClip(BANNER_START_X, BANNER_START_Y,
                            BANNER_START_X, BANNER_END_Y,
                            BANNER_START_X + NOTCH_WIDTH, BANNER_START_Y + BANNER_HEIGHT / 2);
    gfx_FillTriangle_NoClip(BANNER_END_X, BANNER_START_Y,
                            BANNER_END_X, BANNER_END_Y,
                            BANNER_END_X - NOTCH_WIDTH, BANNER_START_Y + BANNER_HEIGHT / 2);

    gfx_SetTextScale(2, 2);
    uint text_x = LCD_WIDTH / 2 - gfx_GetStringWidth("Bonus Tank!") / 2;
    uint text_y = BANNER_START_Y + BANNER_HEIGHT / 2 - 8;

    gfx_SetTextFGColor(COL_DARK_GREEN);
    gfx_PrintStringXY("Bonus Tank!", text_x + 2, text_y + 2);

    gfx_SetTextFGColor(COL_GOLD);
    gfx_PrintStringXY("Bonus Tank!", text_x, text_y);

    gfx_TransparentSprite_NoClip(star, BANNER_START_X + STAR_X_OFFSET, BANNER_START_Y + BANNER_HEIGHT / 2 - star_height / 2);
    gfx_TransparentSprite_NoClip(star, BANNER_END_X - STAR_X_OFFSET - star_width, BANNER_START_Y + BANNER_HEIGHT / 2 - star_height / 2);

    gfx_BlitBuffer();

    gfx_UninitedSprite(fg_tank_shadow, fg_tank_width, fg_tank_height);
    get_sprite_shadow(fg_tank_shadow, fg_tank, COL_RIB_SHADOW);

    while(true) {
        if(timer_ChkInterrupt(1, TIMER_RELOADED)) {
            timer_AckInterrupt(1, TIMER_RELOADED);
            break;
        }
        if(kb_Data[1] & kb_2nd || kb_Data[1] & kb_Del || kb_Data[6] & kb_Clear) {
            while(kb_Data[1] || kb_Data[6]);
            break;
        }

        if(timer_Get(1) > WAIT_TIME * 2 / 3) {
            draw_tank_background(BANNER_START_Y, BANNER_START_Y + BANNER_HEIGHT + SHADOW_HEIGHT,
                                 old_lives, COL_LIVES_TXT, fg_tank_shadow);
        } else if(timer_Get(1) > WAIT_TIME / 3) {
            draw_tank_background(BANNER_START_Y, BANNER_START_Y + BANNER_HEIGHT + SHADOW_HEIGHT,
                                 old_lives + 1, COL_GOLD, fg_tank_shadow);
        } else {
            draw_tank_background(BANNER_START_Y, BANNER_START_Y + BANNER_HEIGHT + SHADOW_HEIGHT,
                                 old_lives + 1, COL_LIVES_TXT, fg_tank_shadow);
        }

        gfx_SwapDraw();
    }

    gfx_SetTextFGColor(COL_BLACK);
    gfx_SetTextScale(1, 1);
}