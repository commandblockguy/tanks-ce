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
#include <keypadc.h>
#include <compression.h>
#include <fileioc.h>

#include "collision.h"
#include "level.h"
#include "graphics.h"
#include "util.h"
#include "tank.h"
#include "globals.h"
#include "profiler.h"
#include "gui.h"

//Player action cooldown
#define SHOT_COOLDOWN 5
#define MINE_COOLDOWN 10

#define PLAYER_BARREL_ROTATION DEGREES_TO_ANGLE(5)
//1/3 of a second for 90 degree rotation
#define PLAYER_TREAD_ROTATION (DEGREES_TO_ANGLE(90) / (TARGET_FPS / 3))

// Game status
enum {
    IN_PROGRESS, QUIT, NEXT_LEVEL, WIN, LOSE
};

bool start_mission(bool initial); //Start a mission and reset various tank things.

void handle_input(void); //Handles inputs from the keypad

void main(void) {
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

    display_scores();

    appVar = ti_Open("TANKSLPK", "r");
    if(!appVar) goto exit;
    ti_Read(&lvl_pack, sizeof(level_pack_t), 1, appVar);
    printf_("Found %u levels.\n", lvl_pack.num_levels);

    game.status = NEXT_LEVEL;

    for(game.mission = 0; game.mission < lvl_pack.num_levels && game.status == NEXT_LEVEL; game.mission++) {
        //Level loop
        uint8_t *comp_tiles; //Compressed tile data
        serialized_tank_t *ser_tanks;

        printf_("Loading level %u.\n", game.mission);

        //Read level from appvar
        ti_Read(&game.level, sizeof(level_t), 1, appVar);
        comp_tiles = malloc(game.level.compressed_tile_size);
        ti_Read(comp_tiles, sizeof(uint8_t), game.level.compressed_tile_size, appVar); //Load tiles
        ser_tanks = malloc(game.level.num_tanks * sizeof(serialized_tank_t));
        tanks = malloc(game.level.num_tanks * sizeof(tank_t));
        ti_Read(ser_tanks, sizeof(serialized_tank_t), game.level.num_tanks, appVar);
        for(uint8_t i = 0; i < game.level.num_tanks; i++) {
            deserialize_tank(&tanks[i], &ser_tanks[i]);
        }
        //todo: check if null

        //Decompress tile data
        zx7_Decompress(tiles, comp_tiles);

        //Display the mission start screen
        if(!start_mission(true)) {
            // todo: display an error to the user
            printf_("error occurred on mission start\n");
            return;
        }

        needs_redraw = true;

        game.status = IN_PROGRESS;
        //Game loop
        while(game.status == IN_PROGRESS) {
            profiler_start(total);
            int alive_tanks = 0;
            if(!tanks[0].alive) {
                game.lives--;
                if(!game.lives) {
                    game.status = LOSE;
                    break;
                }
                if(!start_mission(false)) {
                    // todo: display an error to the user
                    printf_("error occurred on mission start\n");
                    return;
                }
                needs_redraw = true;
            }

            if(game.shotCooldown) {
                game.shotCooldown--;
            }
            if(game.mineCooldown) {
                game.mineCooldown--;
            }

            //handle player input
            handle_input();
            //process physics

            profiler_start(physics);
            for(uint8_t i = 0; i < game.level.num_tanks; i++) {
                process_tank(&tanks[i]);
                if(i && tanks[i].alive) {
                    alive_tanks++;
                }
            }
            if(!alive_tanks) {
                game.status = NEXT_LEVEL;
            }
            profiler_end(physics);

            render(&game.level);

            profiler_end(total);
            profiler_start(frame_wait);
            limit_framerate();
            profiler_end(frame_wait);
            profiler_tick();
        }

        if(game.mission % 5 == 4 && game.mission != lvl_pack.num_levels - 1) {
            //TODO: display lives++ screen
            game.lives++;
        }

        free(ser_tanks); //Free memory so that we don't have issues
        free(tanks);    //(hopefully this does not cause issues)
        free(comp_tiles);

    }

    if(game.status == LOSE) {
        display_kill_counts();
        display_scores();
    }

    exit:

    gfx_End();

    ti_CloseAll();
}

bool start_mission(bool initial) {
    int remaining_tanks = -1; //Don't count the player tank
    bool tank_type_used[NUM_TANK_TYPES] = {false};
    tanks[0].alive = true;
    //Initialize tanks
    for(uint8_t i = 0; i < game.level.num_tanks; i++) {
        tank_t *tank = &tanks[i];
        int j;
        if(initial) tank->alive = true;
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
    for(uint8_t x = 0; x < LEVEL_SIZE_X; x++) {
        for(uint8_t y = 0; y < LEVEL_SIZE_Y; y++) {
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
    return true;
}

void handle_input() {
    profiler_start(input);
	tank_t* player = &tanks[0];
	bool moving = true;
	angle_t target_rot = 0;
	uint8_t keys = 0;

    kb_Scan();

    if(kb_IsDown(kb_KeyDown)) keys |= DOWN;
    if(kb_IsDown(kb_KeyLeft)) keys |= LEFT;
    if(kb_IsDown(kb_KeyRight)) keys |= RIGHT;
    if(kb_IsDown(kb_KeyUp)) keys |= UP;

    switch(keys) {
        default:
            moving = false;
            break;
        case UP:
            target_rot = DEGREES_TO_ANGLE(270);
            break;
        case DOWN:
            target_rot = DEGREES_TO_ANGLE(90);
            break;
        case LEFT:
            target_rot = DEGREES_TO_ANGLE(180);
            break;
        case RIGHT:
            target_rot = DEGREES_TO_ANGLE(0);
            break;
        case UP | RIGHT:
            target_rot = DEGREES_TO_ANGLE(315);
            break;
        case DOWN | RIGHT:
            target_rot = DEGREES_TO_ANGLE(45);
            break;
        case UP | LEFT:
            target_rot = DEGREES_TO_ANGLE(225);
            break;
        case DOWN | LEFT:
            target_rot = DEGREES_TO_ANGLE(135);
    }

    if(moving) {
        int24_t diff = player->tread_rot - target_rot;
        if(abs(diff) > DEGREES_TO_ANGLE(90)) {
            player->tread_rot += DEGREES_TO_ANGLE(180);
            diff = (int24_t) (player->tread_rot - target_rot);
        }
        if(diff < -(int24_t) PLAYER_TREAD_ROTATION) {
            player->tread_rot += PLAYER_TREAD_ROTATION;
        } else if(diff > (int24_t) PLAYER_TREAD_ROTATION) {
            player->tread_rot -= PLAYER_TREAD_ROTATION;
        } else {
            player->tread_rot = target_rot;
        }

        if(abs(diff) <= DEGREES_TO_ANGLE(45)) {
            set_velocity(player, TANK_SPEED_NORMAL);
        } else {
            set_velocity(player, 0);
        }
    } else {
        set_velocity(player, 0);
    }

    if(kb_IsDown(kb_Key2nd) && !game.shotCooldown) {
        fire_shell(player);
        game.shotCooldown = SHOT_COOLDOWN;
    }
    if(kb_IsDown(kb_KeyAlpha) && !game.mineCooldown) {
        lay_mine(player);
        game.mineCooldown = MINE_COOLDOWN;
    }
    if(kb_IsDown(kb_KeyMode)) {
        player->barrel_rot -= PLAYER_BARREL_ROTATION;
    }
    if(kb_IsDown(kb_KeyGraphVar)) {
        player->barrel_rot += PLAYER_BARREL_ROTATION;
    }
    if(kb_IsDown(kb_KeyDel)) { // TODO: remove
        game.status = NEXT_LEVEL;
    }
    if(kb_IsDown(kb_KeyClear)) {
        game.status = QUIT;
    }
    if(kb_IsDown(kb_KeyYequ)) {
        profiler_print();
    }
    profiler_end(input);
}
