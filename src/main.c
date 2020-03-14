/*
 *--------------------------------------
 * Program Name: Tanks! CE
 * Author: commandblockguy
 * License:
 * Description: Tanks! from Wii Play
 *--------------------------------------
*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <graphx.h>
#include <keypadc.h>
#include <compression.h>
#include <fileioc.h>
#undef NDEBUG
#include <debug.h>

#include "constants.h"
#include "collision.h"
#include "level.h"
#include "graphics.h"
#include "gfx/gfx.h"
#include "util.h"
#include "constants.h"
#include "ai.h"
#include "tank.h"
#include "shell.h"
#include "globals.h"

void startMission(bool initial); //Start a mission and reset various tank things.

void handleInput(void); //Handles inputs from the keypad

void main(void) {
	int i;
	LevelPack lvl_pack;
	ti_var_t appVar;

	dbg_sprintf(dbgout, "\n\n[TANKS] Program started.\n");

	ti_CloseAll();
	
	createLevels(); //TODO: TEMP (you'll just download an appvar after I get one properly generated.)

	gfx_Begin(); //Set up draw bits
	gfx_SetPalette(palette, sizeof_palette, 0);
	gfx_SetDrawBuffer();
	gfx_SetTextFGColor(7);

	timer_1_MatchValue_1 = 32768 / TARGET_FPS;

	game.lives = 3;
	game.total_kills = 0;
	memset(game.kills, 0, sizeof(game.kills));

	displayScores();

	appVar = ti_Open("TANKSLPK", "r");
	if(!appVar) goto exit;
	ti_Read(&lvl_pack, sizeof(LevelPack), 1, appVar);
	dbg_sprintf(dbgout, "Found %u levels.\n", lvl_pack.num_levels);

	game.status = NEXT_LEVEL;

	for(game.mission = 0; game.mission < lvl_pack.num_levels && game.status == NEXT_LEVEL; game.mission++) {
		//Level loop
		uint8_t* comp_tiles; //Compressed tile data
		SerializedTank* ser_tanks;

		dbg_sprintf(dbgout, "Loading level %u.\n", game.mission);
		
		//Read level from appvar
		ti_Read(&game.level, sizeof(level_t), 1, appVar);
		comp_tiles = malloc(game.level.compressed_tile_size);
		ti_Read(comp_tiles, sizeof(uint8_t), game.level.compressed_tile_size, appVar); //Load tiles
		ser_tanks = malloc(game.level.num_tanks * sizeof(SerializedTank));
		tanks = malloc(game.level.num_tanks * sizeof(tank_t));
		ti_Read(ser_tanks, sizeof(SerializedTank), game.level.num_tanks, appVar);
		for(i = 0; i < game.level.num_tanks; i++) {
			deserializeTank(&tanks[i], &ser_tanks[i]);
		}

		//Decompress tile data
		zx7_Decompress(tiles, comp_tiles);
		
		//Display the mission start screen
		startMission(true);

		game.status = IN_PROGRESS;
		//Game loop
		while(game.status == IN_PROGRESS) {
			int alive_tanks = 0;
			if(!tanks[0].alive) {
                game.lives--;
				if(!game.lives) {
					game.status = LOSE;
					break;
				}
				startMission(false);
			}

			if(game.shotCooldown) {
				game.shotCooldown--;
			}
			if(game.mineCooldown) {
				game.mineCooldown--;
			}

			//handle player input
			handleInput();
			//process physics

			for(i = 0; i < game.level.num_tanks; i++) {
				processTank(&tanks[i]);
				if(i && tanks[i].alive) {
					alive_tanks++;
				}
			}
			if(!alive_tanks) {
				game.status = NEXT_LEVEL;
			}

            render(&game.level);
		}

		if(game.mission % 5 == 4 && game.mission != lvl_pack.num_levels - 1) {
			//TODO: display lives++ screen
			game.lives++;
		}

		for(i = 0; i < game.level.num_tanks; i++) {
			free(tanks[i].ai_move);
			free(tanks[i].ai_fire);
		}

		free(ser_tanks); //Free memory so that we don't have issues
		free(tanks);    //(hopefully this does not cause issues)
		free(comp_tiles); 
		
	}

	if(game.status == LOSE) {
        displayKillCounts();
		displayScores();
	}

    exit:

	gfx_End();

	ti_CloseAll();
}

void startMission(bool initial) {
	int i;
	int remaining_tanks = -1; //Don't count the player tank
	tanks[0].alive = true;
	//Initialize tanks
	for(i = 0; i < game.level.num_tanks; i++) {
		tank_t* tank = &tanks[i];
		int j;
		if(initial) tank->alive = true;
		if(tank->alive) remaining_tanks++;
		tank->phys.position_x = tileToXPt(tank->start_x);
		tank->phys.position_y = tileToYPt(tank->start_y);
		tank->barrel_rot = 0;
		tank->tread_rot = DEGREES_TO_ANGLE(270);
		for(j = max_shells[tank->type] - 1; j >= 0; j--) {
			tank->shells[j].alive = false;
		}
		for(j = max_mines[tank->type] - 1; j >= 0; j--) {
			tank->mines[j].countdown = 0;
			tank->mines[j].alive = false;
		}
	}
	for(i = 0; i < LEVEL_SIZE_X * LEVEL_SIZE_Y; i++) {
		if(tiles[i] == DESTROYED)
			tiles[i] = DESTRUCTIBLE;
	}
    missionStartScreen(game.mission, game.lives, remaining_tanks);
}

void handleInput() {
	tank_t* player = &tanks[0];
	bool moving = true;
	angle_t target_rot = 0;
	uint8_t keys = 0;

	kb_Scan();

	if(kb_IsDown(kb_KeyDown))  keys |= DOWN;
	if(kb_IsDown(kb_KeyLeft))  keys |= LEFT;
	if(kb_IsDown(kb_KeyRight)) keys |= RIGHT;
	if(kb_IsDown(kb_KeyUp))    keys |= UP;

	switch(keys) {
		default:
            player->phys.velocity_x = 0;
            player->phys.velocity_y = 0;
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
		case UP|RIGHT:
			target_rot = DEGREES_TO_ANGLE(315);
			break;
		case DOWN|RIGHT:
			target_rot = DEGREES_TO_ANGLE(45);
			break;
		case UP|LEFT:
			target_rot = DEGREES_TO_ANGLE(225);
			break;
		case DOWN|LEFT:
			target_rot = DEGREES_TO_ANGLE(135);
	}

	if(moving) {
		int24_t diff = player->tread_rot - target_rot;
		if(abs(diff) > DEGREES_TO_ANGLE(90)) {
			player->tread_rot += DEGREES_TO_ANGLE(180);
			diff = (int24_t)(player->tread_rot - target_rot);
		}
		if(diff < -(int24_t)PLAYER_TREAD_ROTATION) {
			player->tread_rot += PLAYER_TREAD_ROTATION;
		} else if(diff > (int24_t)PLAYER_TREAD_ROTATION) {
			player->tread_rot -= PLAYER_TREAD_ROTATION;
		} else {
			player->tread_rot = target_rot;
		}

		if(abs(diff) <= DEGREES_TO_ANGLE(45)) {
			player->phys.velocity_x = (int24_t)TANK_SPEED_NORMAL * fast_cos(player->tread_rot) / TRIG_SCALE;
			player->phys.velocity_y = (int24_t)TANK_SPEED_NORMAL * fast_sin(player->tread_rot) / TRIG_SCALE;
		}
	}

	if(kb_IsDown(kb_Key2nd) && !game.shotCooldown) {
        fireShell(player);
		game.shotCooldown = SHOT_COOLDOWN;
	}
	if(kb_IsDown(kb_KeyAlpha) && !game.mineCooldown) {
        layMine(player);
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
}

//TODO: compress sprites
// ^ Might not be totally necessary b/c program is already compressed
//TODO: tank sprites
//TODO: crosshair / direction indicator
