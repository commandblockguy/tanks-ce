#include "error.h"

#include <debug.h>
#include <fileioc.h>
#include <graphx.h>
#include <keypadc.h>
#include <tice.h>

#ifndef COMMIT
#define COMMIT "non-git"
#endif

[[noreturn]] void error_screen(const char *error, const char *file, uint24_t line) {
    uint24_t *sp;
    asm("\tld\thl,0\n"
        "\tadd\thl,sp"
        : "=l" (sp));
#ifndef NDEBUG
    dbg_sprintf(dbgerr, "Error: \"%s\" at %s:%u\n", error, file, line);
    *(char*)-1 = 2; // Open CEmu debugger, if the other call fails
    dbg_Debugger();
#endif
    gfx_palette[0] = gfx_RGBTo1555(0, 120, 215);
    gfx_palette[255] = gfx_RGBTo1555(255, 255, 255);
    gfx_SetDrawScreen();
    gfx_ZeroScreen();
    gfx_SetTextFGColor(255);
    gfx_SetTextBGColor(0);
    gfx_SetTextTransparentColor(0);

    gfx_SetTextScale(4, 4);
    const char *messages[] = {":(", "bruh", "wat", ">_<"};
    gfx_PrintStringXY(messages[randInt(0,3)], 16, 16);

    gfx_SetTextScale(1, 1);
    gfx_PrintStringXY("Tanks ran into a problem.", 16, 60);
    gfx_PrintStringXY("Press clear to return to TI-OS.", 16, 72);

    gfx_PrintStringXY("If you're using the latest", 16, 96);
    gfx_PrintStringXY("version of Tanks,", 16, 108);
    gfx_PrintStringXY("please submit a bug report:", 16, 120);
    gfx_PrintStringXY("https://git.io/JtJpA", 16, 132);

    gfx_PrintStringXY("Include this info:", 16, 156);
    gfx_PrintStringXY(error, 16, 168);

    gfx_PrintStringXY(file, 16, 180);
    gfx_PrintChar(':');
    gfx_PrintUInt(line, 1);
    gfx_PrintString(" @ " COMMIT);

    gfx_PrintStringXY("Stack trace:", LCD_WIDTH - 16 - gfx_GetStringWidth("Stack trace:"), 16);
    uint8_t width = gfx_GetCharWidth('0') * 8;
    for(uint8_t y = 28; y < LCD_HEIGHT - 10; y += 10, sp++) {
        gfx_SetTextXY(LCD_WIDTH - 16 - width, y);
        gfx_PrintUInt(*sp, 8);
    }

    asm("ei"); // For screenshots
    while(kb_IsDown(kb_KeyClear));
    while(!kb_IsDown(kb_KeyClear));
    asm("di");

    gfx_End();
    ti_CloseAll();
    exit(1);
}