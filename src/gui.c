#include <graphx.h>
#include <stdint.h>
#include <tice.h>
#include <keypadc.h>


#include "gui.h"
#include "graphics.h"
#include "globals.h"

void displayScores(void) {

}

// todo: improve
void displayKillCounts(void) {
    uint8_t i;
    const uint24_t bg_width = 120;
    const uint24_t base_x = (LCD_WIDTH - bg_width) / 2;

    const uint8_t bands_base_y = 17;
    const uint8_t band_height = 3;
    const uint8_t num_bands = 4;
    const uint8_t bands_total_height = num_bands * 2*band_height - band_height;

    const uint8_t text_base_y = bands_base_y + bands_total_height + 20;
    const uint8_t line_spacing = 18;
    const uint24_t text_center_point = LCD_WIDTH / 2 + 19;
    const uint8_t char_width = 8;

    const uint8_t bottom_band_y = LCD_HEIGHT - 36;

    const uint24_t final_box_width = 38;
    const uint8_t final_box_height = 19;
    const uint24_t final_box_x = text_center_point - final_box_width / 2;
    const uint8_t final_box_y = bottom_band_y + 2 * band_height;
    const uint8_t box_text_y = final_box_y + (final_box_height - 6 * 2) / 2;

    const char results[] = "Results";

    gfx_SetColor(COL_BG);
    gfx_FillRectangle(base_x, 0,
                      bg_width, LCD_HEIGHT);

    gfx_SetColor(COL_OLIVE_BANDS);
    for(i = 0; i < num_bands; i++)
        gfx_FillRectangle(base_x, bands_base_y + i * 2 * band_height,
                          bg_width, band_height);

    gfx_FillRectangle(base_x, bottom_band_y, bg_width, band_height);

    gfx_SetColor(COL_WHITE);
    gfx_FillRectangle(final_box_x, final_box_y, final_box_width, final_box_height);
    gfx_FillCircle(final_box_x, final_box_y + final_box_height / 2, final_box_height / 2);
    gfx_FillCircle(final_box_x + final_box_width, final_box_y + final_box_height / 2, final_box_height / 2);

    gfx_SetTextFGColor(COL_BLACK);
    gfx_SetTextScale(2,2);
    gfx_PrintStringXY(results, (LCD_WIDTH - gfx_GetStringWidth(results)) / 2, bands_base_y + band_height);


    gfx_SetTextScale(1, 1);
    for(i = 0; i < NUM_TANK_TYPES - 1; i++) {
        uint8_t num_kills = game.kills[i + 1];
        if(!num_kills) continue;

        /* TODO: tank sprite */
        if(num_kills >= 10) {
            gfx_SetTextXY(text_center_point - char_width,
                          text_base_y + line_spacing * i);
            gfx_PrintUInt(num_kills, 2);
        } else {
            gfx_SetTextXY(text_center_point - char_width / 2,
                          text_base_y + line_spacing * i);
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

    while(!kb_IsDown(kb_KeyEnter) && !kb_IsDown(kb_KeyClear)) kb_Scan();
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
void missionStartScreen(uint8_t mission, uint8_t lives, uint8_t num_tanks) {
    int x, y;
    gfx_FillScreen(COL_BG);

    gfx_SetColor(COL_RHOM_1);

    gfx_FillRectangle_NoClip(0, 48, LCD_WIDTH, 100);
    gfx_SetColor(COL_RHOM_2);
    for(x = 0; x <= LCD_WIDTH / 12; x++) {
        for(y = 0; y < 6; y++) {
            gfx_FillTriangle(x * 12 + 6, 55 + y * 15, x * 12, y * 15 + 55 + 7, x * 12 + 12, y * 15 + 55 + 7);
            gfx_FillTriangle(x * 12 + 6, 55 + y * 15 + 15, x * 12, y * 15 + 55 + 7, x * 12 + 12, y * 15 + 55 + 7);
        }
    }

    gfx_SetColor(COL_RIB_SHADOW);
    gfx_FillRectangle_NoClip(0, 148, LCD_WIDTH, 4);

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


    gfx_SetTextXY((LCD_WIDTH - 8 * MISSION_NUMBER_TEXT) / 2 + 2, 186 + 2);
    gfx_SetTextFGColor(COL_RIB_SHADOW);
    gfx_PrintString("x   ");
    gfx_PrintUInt(lives, 1);

    gfx_SetTextXY((LCD_WIDTH - 8 * MISSION_NUMBER_TEXT) / 2, 186);
    gfx_SetTextFGColor(COL_LIVES_TXT);
    gfx_PrintString("x   ");
    gfx_PrintUInt(lives, 1);


    gfx_SetTextFGColor(COL_BLACK);
    gfx_SetTextScale(1, 1);

    //TODO: tank sprite

    gfx_BlitBuffer();

    //Delay for some time, or wait for a keypress
    timer_Control &= ~TIMER1_ENABLE;
    timer_1_ReloadValue = timer_1_Counter = 33 * MISSION_START_TIME;
    timer_Control |= TIMER1_ENABLE | TIMER1_32K | TIMER1_0INT | TIMER1_DOWN;

    while(true) {
        if(timer_IntStatus & TIMER1_RELOADED) {
            timer_IntAcknowledge = TIMER1_RELOADED;
            break;
        }
        kb_Scan();
        if(kb_Data[1] & kb_2nd || kb_Data[1] & kb_Del || kb_Data[6] & kb_Clear) {
            while(kb_Data[1] || kb_Data[6]) kb_Scan();
            break;
        }
    }
}
