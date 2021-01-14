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
#include "globals.h"
#include "util/profiler.h"
#include "graphics/gui.h"
#include "game.h"

int main() {
    dbg_printf("\n\n[TANKS] Program started.\n");

    ti_CloseAll();
    kb_SetMode(MODE_3_CONTINUOUS);

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
        dbg_printf("Failed to open level pack\n");
        goto exit;
    }

    level_pack_t lvl_pack;
    ti_Read(&lvl_pack, sizeof(level_pack_t), 1, appVar);
    dbg_printf("Found %u levels.\n", lvl_pack.num_levels);

    for(game.mission = 0; game.mission < lvl_pack.num_levels; game.mission++) {
        //Level loop
        const uint8_t *comp_tiles; //Compressed tile data
        const serialized_tank_t *ser_tanks;

        dbg_printf("Loading level %u.\n", game.mission);

        //Read level from appvar
        ti_Read(&game.level, sizeof(level_t), 1, appVar);
        comp_tiles = (const uint8_t *)ti_GetDataPtr(appVar);
        ti_Seek(game.level.compressed_tile_size, SEEK_CUR, appVar);
        ser_tanks = (const serialized_tank_t *)ti_GetDataPtr(appVar);
        ti_Seek(sizeof(serialized_tank_t) * game.level.num_tanks, SEEK_CUR, appVar);

        if(game.level.num_tanks > MAX_NUM_TANKS) {
            dbg_printf("Too many tanks in level (%u)\n", game.level.num_tanks);
            continue;
        }

        uint8_t status = play_level(comp_tiles, ser_tanks);

        switch(status) {
            case NEXT_LEVEL:
                break;
            case LOSE: {
                display_kill_counts();
                display_scores();
                goto exit;
            }
            case QUIT:
            case ERROR: {
                goto exit;
            }
            default: {
                dbg_printf("Got a weird status code from a mission: %u\n", status);
                goto exit;
            }
        }

        if(game.mission % 5 == 4 && game.mission != lvl_pack.num_levels - 1) {
            //TODO: display lives++ screen
            game.lives++;
        }
    }

    // todo: win

    exit:

    gfx_End();

    ti_CloseAll();
    return 0;
}
