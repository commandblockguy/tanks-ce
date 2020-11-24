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

#include <stdlib.h>

bool start_mission(const serialized_tank_t *ser_tanks); //Start a mission and reset various tank things.
uint8_t play_level(const void *comp_tiles, const serialized_tank_t *ser_tanks);
uint8_t play_mission(const serialized_tank_t *ser_tanks);

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

bool start_mission(const serialized_tank_t *ser_tanks) {
    bool tank_type_used[NUM_TANK_TYPES] = {false};

    while(!PhysicsBody::objects.empty()) {
        delete PhysicsBody::objects[0];
    }

    game.num_tanks = 0;

    // todo: don't recreate destroyed tanks
    for(uint8_t i = 0; i < game.level.num_tanks; i++) {
        new Tank(&ser_tanks[i], i);
        tank_type_used[ser_tanks[i].type] = true;
    }

    for(uint8_t x = 1; x < LEVEL_SIZE_X - 1; x++) {
        for(uint8_t y = 1; y < LEVEL_SIZE_Y - 1; y++) {
            if(tiles[y][x] == DESTROYED)
                tiles[y][x] = DESTRUCTIBLE;
        }
    }

    draw_mission_start_screen(game.mission, game.lives, game.num_tanks - 1);
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

uint8_t play_mission(const serialized_tank_t *ser_tanks) {
    start_mission(ser_tanks);
    while(true) {
        profiler_start(total);

        //handle player input
        uint8_t status = handle_input();
        if(status) return status;


        profiler_start(physics);
        for(auto it: PhysicsBody::objects) {
            it->process();
        }
        process_collisions();
        profiler_end(physics);

        if(!game.player_alive) {
            game.lives--;
            if(!game.lives) {
                profiler_end(total);
                return LOSE;
            } else {
                return RETRY;
            }
        }

        if(game.num_tanks == 1) {
            profiler_end(total);
            return NEXT_LEVEL;
        }

        PhysicsBody::sort();

        render();

        profiler_end(total);
        profiler_start(frame_wait);
        limit_framerate();
        profiler_end(frame_wait);
        profiler_tick();
    }
}

uint8_t play_level(const void *comp_tiles, const serialized_tank_t *ser_tanks) {
    decompress_tiles(comp_tiles);

    uint8_t status;
    do status = play_mission(ser_tanks);
    while(status == RETRY);

    return status;
}
