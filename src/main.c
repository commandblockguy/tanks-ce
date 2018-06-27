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
#include <decompress.h>

#include "constants.h"
#include "objects.h"
#include "collision.h"
#include "level.h"
#include "graphics.h"
#include "gfx/tiles_gfx.h"

void startMission(bool initial); //Start a mission and reset various tank things.

void processTank(Tank* tank); //Process tank physics, along with that tank's shells and mines

void processCollisions(void); //Process collisions between all entities that could reasonably intersect.

void stablizeFPS(uint8_t target);

void resetFPSCounter(void); //Start the tick and reset the tick time counter

void handleInput(void); //Handles inputs from the keypad

Game game; //Game global, so I can reuse those names elsewhere if needed

Tank* tanks; //List of all active tanks. 
uint8_t tiles[LEVEL_SIZE_X * LEVEL_SIZE_Y]; //Currently active tilemap data

void main(void) {
	int i;
	LevelPack lvl_pack;
	ti_var_t appVar;

	ti_CloseAll();
	
	createLevels(); //TEMP (you'll just download an appvar after I get one properly generated.)

	gfx_Begin(); //Set up draw bits
	gfx_SetPalette(tiles_gfx_pal, sizeof_tiles_gfx_pal, 0);
	gfx_SetDrawBuffer();
	gfx_SetTextFGColor(7);

	timer_1_MatchValue_1 = 32768 / TARGET_FPS; //Something with timers, I think.

	game.lives = 3;

	displayScores();
	appVar = ti_Open("TANKSLPK", "r");
	ti_Read(&lvl_pack, sizeof(LevelPack), 1, appVar);
	for(game.mission = 0; game.mission < lvl_pack.num_levels; game.mission++) {
		//Level loop
		uint8_t* comp_tiles; //Compressed tile data
		SerializedTank* ser_tanks;
		
		//Read level from appvar
		ti_Read(&game.level, sizeof(Level), 1, appVar);
		comp_tiles = malloc(game.level.compressed_tile_size);
		ti_Read(comp_tiles, sizeof(uint8_t), game.level.compressed_tile_size, appVar); //Load tiles
		ser_tanks = malloc(game.level.num_tanks * sizeof(SerializedTank));
		tanks = malloc(game.level.num_tanks * sizeof(Tank));
		ti_Read(ser_tanks, sizeof(SerializedTank), game.level.num_tanks, appVar);
		for(i = 0; i < game.level.num_tanks; i++) {
			tanks[i] = deserializeTank(ser_tanks[i]);
		}

		//Decompress tile data
		dzx7_Turbo(comp_tiles, tiles);

		gfx_FillScreen(gfx_white);
		
		//Display the mission start screen
		startMission(true);

		game.inProgress = true;
		//Game loop
		while(game.inProgress) {
			int i, alive_tanks = 0;
			if(!tanks[0].alive) {
				if(!--game.lives) {
					displayScores();
					gfx_End();
					return;
				}
				startMission(false);
			}
			resetFPSCounter();

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
				game.inProgress = 0;
			}

			processCollisions();

			render(tiles, &game.level, tanks);
		}

		free(ser_tanks); //Free memory so that we don't have issues
		free(tanks);    //(hopefully this does not cause issues)
		free(comp_tiles); 
		
	}

	gfx_End();

	ti_CloseAll();
}

void startMission(bool initial) {
	int i;
	int remaining_tanks = -1; //Don't count the player tank
	tanks[0].alive = true;
	for(i = 0; i < game.level.num_tanks; i++) {
		Tank* tank = &tanks[i];
		int j;
		if(initial) tank->alive = true;
		if(tank->alive) remaining_tanks++;
		tank->phys.position_x = tileToXPixel(tank->start_x) << SHIFT_AMOUNT;
		tank->phys.position_y = tileToYPixel(tank->start_y) << SHIFT_AMOUNT;
		tank->barrel_rot = 0;
		tank->tread_rot = 191;
		for(j = 0; j < 5; j++) {
			tank->shells[j].alive = false;
			if(j < 2) tank->mines[j].alive = false;
		}
	}
	for(i = 0; i < LEVEL_SIZE_X * LEVEL_SIZE_Y; i++) {
		if(tiles[i] == DESTROYED)
			tiles[i] = DESTRUCTIBLE;
	}
	missionStart(game.mission, game.lives, remaining_tanks);
}

void resetFPSCounter() {
	//woo copypaste!
	/* Disable the timer so it doesn't run when we don't want it to be running */
	timer_Control = TIMER1_DISABLE;

	timer_1_Counter = 0;

	/* Enable the timer, set it to the 32768 kHz clock, enable an interrupt once it reaches 0, and make it count down */
	timer_Control = TIMER1_ENABLE | TIMER1_32K | TIMER1_NOINT | TIMER1_UP;
}

//Process tank physics
void processTank(Tank* tank) {
	int i;
	//Loop through all shells
	for(i = max_shells[tank->type] - 1; i >= 0; i--) {
		int j;
		Shell* shell = &tank->shells[i];
		struct reflection reflect;
		//Ignore dead shells
		if(!shell->alive) continue;
		//Add velocity
		shell->phys.position_x += (uint24_t)(shell->phys.velocity_x * (1 << SHIFT_AMOUNT));
		shell->phys.position_y += (uint24_t)(shell->phys.velocity_y * (1 << SHIFT_AMOUNT));

		//This will eventually be part of the collision bit
		for(j = 0; j < game.level.num_tanks; j++) {
			AABB bb1 = getAABB(&shell->phys);
			AABB bb2 = getAABB(&tanks[j].phys);
			if(!tanks[j].alive) continue;
			if(shell->left_tank_hitbox) {
				if(detectCollision(bb1, bb2)) {
					tanks[j].alive = false;
					shell->alive = false;
				}
			} else {
				if(j == 0 && !detectCollision(bb1, bb2)) {
					shell->left_tank_hitbox = true;
				}
			}
		}

		if(shell->phys.position_x >> SHIFT_AMOUNT < MAP_OFFSET_X) {
			shell_ricochet(shell, LEFT, MAP_OFFSET_X - (shell->phys.position_x >> SHIFT_AMOUNT));
		} else if(shell->phys.position_x >> SHIFT_AMOUNT > MAP_OFFSET_X + TILE_SIZE * LEVEL_SIZE_X) {
			shell_ricochet(shell, RIGHT, (shell->phys.position_x >> SHIFT_AMOUNT) - MAP_OFFSET_X - TILE_SIZE * LEVEL_SIZE_X);
		}
		if((shell->phys.position_y >> SHIFT_AMOUNT) + 30 < 30) {
			shell_ricochet(shell, UP, 0 - (shell->phys.position_y >> SHIFT_AMOUNT));
		} else if(shell->phys.position_y >> SHIFT_AMOUNT > TILE_SIZE * LEVEL_SIZE_Y) {
			shell_ricochet(shell, DOWN, (shell->phys.position_y >> SHIFT_AMOUNT) - TILE_SIZE * LEVEL_SIZE_Y);
		}
		reflect = getTileReflect(&shell->oldPhys, &shell->phys, false, tiles);
		if(reflect.colliding) {
			shell_ricochet(shell, reflect.dir, 0);
		}

		shell->oldPhys = shell->phys;
		
	}
	if(!max_mines[tank->type]) return;
	for(i = max_mines[tank->type] - 1; i >= 0; i--) {
		Mine* mine = &tank->mines[i];
		//Ignore dead shells
		if(!mine->alive) continue;
		//TODO: check for nearby enemy tanks and set counter if necessary
		if(--mine->countdown == 0) {
			detonate(mine, tiles);
		}
		mine->oldPhys = mine->phys;
	}

	tank->oldPhys = tank->phys;
}

void handleInput() {
	Tank* player = &tanks[0];

	kb_Scan();

	//TODO: Replace with fancy roatation stuff if necessary
	if(kb_Data[7] & kb_Down) {
		player->phys.position_y += TANK_SPEED_NORMAL << SHIFT_AMOUNT;
	}
	if(kb_Data[7] & kb_Left) {
		player->phys.position_x -= TANK_SPEED_NORMAL << SHIFT_AMOUNT;
	}
	if(kb_Data[7] & kb_Right) {
		player->phys.position_x += TANK_SPEED_NORMAL << SHIFT_AMOUNT;
	}
	if(kb_Data[7] & kb_Up) {
		player->phys.position_y -= TANK_SPEED_NORMAL << SHIFT_AMOUNT;
	}
	if(kb_Data[1] & kb_2nd && !game.shotCooldown) {
		fire_shell(player);
		game.shotCooldown = SHOT_COOLDOWN;
	}
	if(kb_Data[2] & kb_Alpha && !game.mineCooldown) {
		lay_mine(player);
		game.mineCooldown = MINE_COOLDOWN;
	}
	if(kb_Data[1] & kb_Mode) {
		player->barrel_rot -= PLAYER_BARREL_ROTATION;
		calc_bullet_spawn(player);
	}
	if(kb_Data[3] & kb_GraphVar) {
		player->barrel_rot += PLAYER_BARREL_ROTATION;
		calc_bullet_spawn(player);
	}
	if(kb_Data[1] & kb_Del) {
		game.inProgress = false;
	}
}

//TODO: make this work
//TODO: make this work faster
void processCollisions() {
	
}

//TODO: moar levels
//TODO: compress sprites
//TODO: basic physics
//TODO: tank sprites
//TODO: crosshair / direction indicator
