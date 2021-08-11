/*
 *--------------------------------------
 * Program Name: Tanks! CE
 * Author: commandblockguy
 * License: MIT
 * Description: Tanks! from Wii Play
 *--------------------------------------
*/
#include <cstdint>

#include <debug.h>
#include <graphx.h>
#include <fileioc.h>

#include <string.h>
#include <keypadc.h>

#include "level.h"
#include "graphics/graphics.h"
#include "util/profiler.h"
#include "game.h"
#include "gui/error.h"
#include "gui/kill_counts.h"
#include "gui/transition.h"

int main() {
    dbg_printf("\n\n[TANKS] Program started.\n");

    kb_SetMode(MODE_3_CONTINUOUS);

    srand(rtc_Time());

    init_graphics();

    dbg_printf("Initted graphics\n");

    profiler_init();

    dbg_printf("Started profiler\n");

    gen_lookups();

    dbg_printf("Generated lookups\n");

    game.lives = 3;
    game.total_kills = 0;
    memset(game.kills, 0, sizeof(game.kills));

    ti_var_t appVar = ti_Open("TANKSLPK", "r");
    if(!appVar) {
        // todo: present this better
        ERROR("TANKSLPK not found");
    }

    struct level_pack lvl_pack;
    ti_Read(&lvl_pack, sizeof(struct level_pack), 1, appVar);
    dbg_printf("Found %u levels.\n", lvl_pack.num_levels);

    for(game.mission = 0; game.mission < lvl_pack.num_levels; game.mission++) {
        //Level loop
        const uint8_t *comp_tiles; //Compressed tile data
        const struct serialized_tank *ser_tanks;

        dbg_printf("Loading level %u.\n", game.mission);

        //Read level from appvar
        ti_Read(&game.level, sizeof(struct level), 1, appVar);
        comp_tiles = (const uint8_t *)ti_GetDataPtr(appVar);
        ti_Seek(game.level.compressed_tile_size, SEEK_CUR, appVar);
        ser_tanks = (const struct serialized_tank *)ti_GetDataPtr(appVar);
        ti_Seek(sizeof(struct serialized_tank) * game.level.num_tanks, SEEK_CUR, appVar);

        if(game.level.num_tanks > MAX_NUM_TANKS) {
            dbg_printf("Too many tanks in level (%u)\n", game.level.num_tanks);
            ERROR("Too many tanks in level.");
        }

        uint8_t status = play_level(comp_tiles, ser_tanks);

        switch(status) {
            case NEXT_LEVEL:
                break;
            case LOSE: {
                display_kill_counts();
                // todo: display high scores
                goto exit;
            }
            case QUIT:
            case ERROR: {
                goto exit;
            }
            default: {
                dbg_printf("Status: %u\n", status);
                ERROR("Unknown game status");
            }
        }

        if(game.mission % 5 == 4 && game.mission != lvl_pack.num_levels - 1) {
            extra_life_screen(game.lives);
            game.lives++;
        }
    }

    // todo: win

    exit:

    gfx_End();
    ti_Close(appVar);

    return 0;
}
