#include "pause.h"

#include "../graphics/graphics.h"
#include <graphx.h>
#include <keypadc.h>
#include <tice.h>

void rounded_rectangle(uint24_t x, uint8_t y, uint24_t width, uint8_t height, uint8_t radius) {
    gfx_FillCircle_NoClip(x + radius + 1, y + radius + 1, radius);
    gfx_FillCircle_NoClip(x + radius + 1, y + height - radius - 1, radius);
    gfx_FillCircle_NoClip(x + width - radius - 1, y + radius + 1, radius);
    gfx_FillCircle_NoClip(x + width - radius - 1, y + height - radius - 1, radius);

    gfx_FillRectangle_NoClip(x + radius + 1, y, x + width - x - 2 * radius - 2, radius + 1);
    gfx_FillRectangle_NoClip(x, y + radius + 1, x + width - x, y + height - y - 2 * radius - 2);
    gfx_FillRectangle_NoClip(x + radius + 1, y + height - radius - 1, x + width - x - 2 * radius - 2, radius + 1);
}

uint8_t pause_menu() {
    const uint8_t OUTER_BORDER_WIDTH = 2;
    const uint8_t INNER_BORDER_WIDTH = 2;
    const uint8_t BORDER_CURVE_RADIUS = 7;
    const uint24_t BUTTON_WIDTH = 244;
    //const uint24_t BUTTON_WIDTH_SELECTED = 269;
    const uint8_t BUTTON_HEIGHT = 58;
    //const uint8_t BUTTON_HEIGHT_SELECTED = 63;
    const uint8_t BUTTON_GAP = 7;

    const uint8_t TOP_BUTTON_CENTER_Y = LCD_HEIGHT / 2 - BUTTON_GAP - BUTTON_HEIGHT;

    needs_redraw = true;

    int8_t selection = 0;

    gfx_SetTextFGColor(COL_BLACK);
    gfx_SetTextScale(2, 2);

    while(true) {
        if(kb_IsDown(kb_KeyClear)) {
            gfx_SetTextScale(1, 1);
            return 0;
        }
        if(kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyClear)) {
            gfx_SetTextScale(1, 1);
            return selection;
        }
        if(kb_IsDown(kb_KeyEnter)) return selection;

        if(kb_IsDown(kb_KeyUp)) selection--;
        if(kb_IsDown(kb_KeyDown)) selection++;
        if(selection < 0) selection = 2;
        if(selection >= 3) selection = 0;

        while(kb_IsDown(kb_KeyUp) || kb_IsDown(kb_KeyDown));

        for(int8_t button = 0; button < 3; button++) {
            const char *strings[3] = {"Continue", "Start Over", "Quit"};
            uint8_t center_y = TOP_BUTTON_CENTER_Y + button * (BUTTON_HEIGHT + BUTTON_GAP);
            uint24_t x = LCD_WIDTH / 2 - BUTTON_WIDTH / 2;
            uint8_t y = center_y - BUTTON_HEIGHT / 2;

            gfx_SetColor(COL_WHITE);
            rounded_rectangle(x, y, BUTTON_WIDTH, BUTTON_HEIGHT, BORDER_CURVE_RADIUS);

            gfx_SetColor(COL_LIVES_TXT);
            rounded_rectangle(x + OUTER_BORDER_WIDTH, y + OUTER_BORDER_WIDTH,
                              BUTTON_WIDTH - 2 * OUTER_BORDER_WIDTH, BUTTON_HEIGHT - 2 * OUTER_BORDER_WIDTH,
                              BORDER_CURVE_RADIUS - OUTER_BORDER_WIDTH);

            gfx_SetColor(button == selection ? COL_RIB_SHADOW : COL_WHITE);
            rounded_rectangle(x + OUTER_BORDER_WIDTH + INNER_BORDER_WIDTH,
                              y + OUTER_BORDER_WIDTH + INNER_BORDER_WIDTH,
                              BUTTON_WIDTH - 2 * (OUTER_BORDER_WIDTH + INNER_BORDER_WIDTH),
                              BUTTON_HEIGHT - 2 * (OUTER_BORDER_WIDTH + INNER_BORDER_WIDTH),
                              BORDER_CURVE_RADIUS - OUTER_BORDER_WIDTH - INNER_BORDER_WIDTH);

            gfx_SetTextXY(LCD_WIDTH / 2 - gfx_GetStringWidth(strings[button]) / 2, y + BUTTON_HEIGHT / 2 - 8);
            gfx_PrintString(strings[button]);
        }
        gfx_BlitBuffer();
    };
}