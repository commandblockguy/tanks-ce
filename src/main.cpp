/*
 *--------------------------------------
 * Program Name: Tanks! CE
 * Author: commandblockguy
 * License:
 * Description: Tanks! from Wii Play
 *--------------------------------------
*/
#include <stdbool.h>
#include <stdint.h>
#include <tice.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <graphx.h>
#include <fileioc.h>

#include "level.h"
#include "graphics.h"
#include "util.h"
#include "tank.h"
#include "globals.h"
#include "profiler.h"
#include "gui.h"
#include "input.h"

bool start_mission(void); //Start a mission and reset various tank things.
uint8_t play_level(const void *comp_tiles, const serialized_tank_t *ser_tanks);
uint8_t play_mission(void);

int main() {
    level_pack_t lvl_pack;
    ti_var_t appVar;

    printf_("\n\n[TANKS] Program started.\n");

    ti_CloseAll();

    create_levels(); //TODO: TEMP (you'll just download an appvar after I get one properly generated.)

    init_graphics();

    timer_Control = 0;

    profiler_init();

    gen_lookups();

    game.lives = 3;
    game.total_kills = 0;
    memset(game.kills, 0, sizeof(game.kills));

    appVar = ti_Open("TANKSLPK", "r");
    if(!appVar) {
        printf_("Failed to open level pack\n");
        goto exit;
    }
    ti_Read(&lvl_pack, sizeof(level_pack_t), 1, appVar);
    printf_("Found %u levels.\n", lvl_pack.num_levels);

    for(game.mission = 0; game.mission < lvl_pack.num_levels; game.mission++) {
        //Level loop
        const uint8_t *comp_tiles; //Compressed tile data
        const serialized_tank_t *ser_tanks;

        printf_("Loading level %u.\n", game.mission);

        //Read level from appvar
        ti_Read(&game.level, sizeof(level_t), 1, appVar);
        comp_tiles = (const uint8_t *)ti_GetDataPtr(appVar);
        ti_Seek(game.level.compressed_tile_size, SEEK_CUR, appVar);
        ser_tanks = (const serialized_tank_t *)ti_GetDataPtr(appVar);
        ti_Seek(sizeof(serialized_tank_t) * game.level.num_tanks, SEEK_CUR, appVar);

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
                printf_("Got a weird status code from a mission: %u\n", status);
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

bool start_mission() {
    int remaining_tanks = -1; //Don't count the player tank
    bool tank_type_used[NUM_TANK_TYPES] = {false};
    tanks[0].alive = true;
    //Initialize tanks
    for(uint8_t i = 0; i < game.level.num_tanks; i++) {
        tank_t *tank = &tanks[i];
        int j;
        if(tank->alive) {
            remaining_tanks++;
            tank->phys.position_x = TILE_TO_X_COORD(tank->start_x);
            tank->phys.position_y = TILE_TO_Y_COORD(tank->start_y);
            tank->barrel_rot = 0;
            tank->tread_rot = DEGREES_TO_ANGLE(270);
            tank_type_used[tank->type] = true;
        }
        for(j = max_shells[tank->type] - 1; j >= 0; j--) {
            tank->shells[j].alive = false;
        }
        for(j = max_mines[tank->type] - 1; j >= 0; j--) {
            tank->mines[j].countdown = 0;
            tank->mines[j].alive = false;
        }
    }
    for(uint8_t x = 1; x < LEVEL_SIZE_X - 1; x++) {
        for(uint8_t y = 1; y < LEVEL_SIZE_Y - 1; y++) {
            if(tiles[y][x] == DESTROYED)
                tiles[y][x] = DESTRUCTIBLE;
        }
    }

    draw_mission_start_screen(game.mission, game.lives, remaining_tanks);
    for(uint8_t type = 1; type < NUM_TANK_TYPES; type++) {
        if(tank_type_used[type]) {
            if(!init_tank_sprites(type)) {
                dbg_sprintf((char*)dbgerr, "Ran out of memory when allocating tank sprites\n");
                return false;
            }
        } else {
            free_tank_sprites(type);
        }
    }
    wait_ms_or_keypress(MISSION_START_TIME);
    init_timer();
    needs_redraw = true;
    return true;
}

uint8_t play_mission(void) {
    start_mission();
    while(true) {
        profiler_start(total);
        int alive_tanks = 0;
        if(!tanks[0].alive) {
            game.lives--;
            if(!game.lives) {
                profiler_end(total);
                return LOSE;
            } else {
                return RETRY;
            }
        }

        //handle player input
        uint8_t status = handle_input();
        if(status) return status;

        profiler_start(physics);
        for(uint8_t i = 0; i < game.level.num_tanks; i++) {
            process_tank(&tanks[i]);
            if(i && tanks[i].alive) {
                alive_tanks++;
            }
        }
        profiler_end(physics);

        if(!alive_tanks) {
            profiler_end(total);
            return NEXT_LEVEL;
        }

        render();

        profiler_end(total);
        profiler_start(frame_wait);
        limit_framerate();
        profiler_end(frame_wait);
        profiler_tick();
    }
}

uint8_t play_level(const void *comp_tiles, const serialized_tank_t *ser_tanks) {
    tanks = (tank_t*)malloc(game.level.num_tanks * sizeof(tank_t));
    if(!tanks) {
        printf_("Failed to allocate tanks array\n");
        return ERROR;
    }
    for(uint8_t i = 0; i < game.level.num_tanks; i++) {
        deserialize_tank(&tanks[i], &ser_tanks[i]);
        tanks[i].alive = true;
    }

    decompress_tiles(comp_tiles);

    uint8_t status;
    do status = play_mission();
    while(status == RETRY);

    free(tanks);
    return status;
}
