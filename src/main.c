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

#include "constants.h"
#include "objects.h"
#include "collision.h"
#include "level.h"
#include "graphics.h"
#include "gfx/tiles_gfx.h"
#include "debug.h"
#include "util.h"

void startMission(bool initial); //Start a mission and reset various tank things.

void processTank(Tank* tank); //Process tank physics, along with that tank's shells and mines

void processShell(Shell* shell, Tank* tank); //Process a shell

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

	dbg_sprintf(dbgout, "\n\n[TANKS] Program started.\n");

	ti_CloseAll();
	
	createLevels(); //TODO: TEMP (you'll just download an appvar after I get one properly generated.)

	gfx_Begin(); //Set up draw bits
	gfx_SetPalette(tiles_gfx_pal, sizeof_tiles_gfx_pal, 0);
	gfx_SetDrawBuffer();
	gfx_SetTextFGColor(7);

	timer_1_MatchValue_1 = 32768 / TARGET_FPS; //Something with timers, I think.

	game.lives = 3;

	displayScores();

	appVar = ti_Open("TANKSLPK", "r");
	ti_Read(&lvl_pack, sizeof(LevelPack), 1, appVar);
	dbg_sprintf(dbgout, "Found %u levels.\n", lvl_pack.num_levels);

	game.status = NEXT_LEVEL;

	for(game.mission = 0; game.mission < lvl_pack.num_levels && game.status == NEXT_LEVEL; game.mission++) {
		//Level loop
		uint8_t* comp_tiles; //Compressed tile data
		SerializedTank* ser_tanks;

		dbg_sprintf(dbgout, "Loading level %u.\n", game.mission);
		
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
		zx7_Decompress(tiles, comp_tiles);

		gfx_FillScreen(gfx_white);
		
		//Display the mission start screen
		startMission(true);

		game.status = IN_PROGRESS;
		//Game loop
		while(game.status == IN_PROGRESS) {
			int i, alive_tanks = 0;
			if(!tanks[0].alive) {
				if(!--game.lives) {
					game.status = LOSE;
					break;
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
				game.status = NEXT_LEVEL;
			}

			render(tiles, &game.level, tanks);
		}

		free(ser_tanks); //Free memory so that we don't have issues
		free(tanks);    //(hopefully this does not cause issues)
		free(comp_tiles); 
		
	}

	if(game.status == LOSE) {
		displayScores();
	}

	gfx_End();

	ti_CloseAll();
}

void startMission(bool initial) {
	int i;
	int remaining_tanks = -1; //Don't count the player tank
	tanks[0].alive = true;
	//Initialize tanks
	for(i = 0; i < game.level.num_tanks; i++) {
		Tank* tank = &tanks[i];
		int j;
		if(initial) tank->alive = true;
		if(tank->alive) remaining_tanks++;
		tank->phys.position_x = to_ufix(tileToXPixel(tank->start_x));
		tank->phys.position_y = to_ufix(tileToYPixel(tank->start_y));
		tank->barrel_rot = 0;
		calc_bullet_spawn(tank);
		tank->tread_rot = 192;
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
	missionStart(game.mission, game.lives, remaining_tanks);
}

void resetFPSCounter() {
	//woo copypaste!
	/* Disable the timer so it doesn't run when we don't want it to be running */
	timer_Control = TIMER1_DISABLE;

	timer_1_Counter = 0;

	/* Enable the timer, set it to the 32768 kHz clock */
	timer_Control = TIMER1_ENABLE | TIMER1_32K | TIMER1_NOINT | TIMER1_UP;
}

//Process tank physics
void processTank(Tank* tank) {
	int i;
	struct reflection reflect;

	//Keep the tank inside the map
	if(tank->phys.position_x < to_ufix(MAP_OFFSET_X)) {
		tank->phys.position_x = to_ufix(MAP_OFFSET_X);
	} else if(tank->phys.position_x > to_ufix(MAP_OFFSET_X + TILE_SIZE * LEVEL_SIZE_X - TANK_SIZE - 1)) {
		tank->phys.position_x = to_ufix(MAP_OFFSET_X + TILE_SIZE * LEVEL_SIZE_X - TANK_SIZE - 1);
	}
	if(tank->phys.position_y > to_ufix(LCD_WIDTH + 20)) {
		tank->phys.position_y = to_ufix(0);
	} else if(tank->phys.position_y > to_ufix(TILE_SIZE * LEVEL_SIZE_Y - TANK_SIZE)) {
		tank->phys.position_y = to_ufix(TILE_SIZE * LEVEL_SIZE_Y - TANK_SIZE);
	}

	reflect = getTileReflect(&tank->phys, false, tiles);

	for(i = game.level.num_tanks - 1; i >= 0; i--) {
		if(tanks[i].alive)
			collideAndPush(&tank->phys, &tanks[i].phys);
	}

	//Loop through all shells
	for(i = max_shells[tank->type] - 1; i >= 0; i--) {
		processShell(&tank->shells[i], tank);
	}
	//Loop through mines
	if(max_mines[tank->type])
		for(i = max_mines[tank->type] - 1; i >= 0; i--) {
			Mine* mine = &tank->mines[i];
			int j;
			//Ignore mines which have already finished their countdowns
			if(!mine->countdown) continue;
			
			if(--mine->countdown == EXPLOSION_ANIM) {
				detonate(mine, tiles);
			}
			if(!mine->alive) continue;
			//mine belongs to enemy
			if(tank != &tanks[0])
				if(center_distance_lt(&mine->phys, &tanks[0].phys, float_to_ufix(MINE_EXPLOSION_RADIUS))) {
					detonate(mine, tiles);
					continue;
				}
			//mine belongs to our tank
			if(!center_distance_lt(&mine->phys, &tanks[0].phys, float_to_ufix(MINE_EXPLOSION_RADIUS)))
				for(j = 1; j < game.level.num_tanks; j++) {
					if(center_distance_lt(&mine->phys, &tanks[j].phys, float_to_ufix(MINE_EXPLOSION_RADIUS))) {
						detonate(mine, tiles);
						break;
					}
				}
		}

}

void processShell(Shell* shell, Tank* tank) {
	int j;
	struct reflection reflect;
	//Ignore dead shells
	if(!shell->alive) return;
	//Add velocity
	shell->phys.position_x += shell->phys.velocity_x;
	shell->phys.position_y += shell->phys.velocity_y;

	if(shell->left_tank_hitbox) {
		//This will eventually be part of the collision bit
		for(j = 0; j < game.level.num_tanks; j++) {
			int i;

			if(max_mines[tank->type])
				for(i = max_mines[tanks[j].type] - 1; i >= 0; i--) {
					Mine* mine = &tanks[j].mines[i];
					if(mine->alive && detectCollision(&shell->phys, &mine->phys)) {
						shell->alive = false;
						detonate(mine, tiles);
					}
				}

			for(i = max_shells[tanks[j].type] - 1; i >= 0; i--) {
				Shell* shell2 = &tanks[j].shells[i];
				if(shell != shell2 && shell2->alive && detectCollision(&shell->phys, &shell2->phys)) {
					shell->alive = false;
					shell2->alive = false;
				}
			}

			if(!tanks[j].alive) continue;

			if(detectCollision(&shell->phys, &tanks[j].phys)) {
				tanks[j].alive = false;
				shell->alive = false;
			}
		}
	} else if(!detectCollision(&shell->phys, &tank->phys)) {
		shell->left_tank_hitbox = true;
	}

	if(shell->phys.position_x < to_ufix(MAP_OFFSET_X)) {
		shell_ricochet(shell, LEFT);
	} else if(shell->phys.position_x > to_ufix(MAP_OFFSET_X + TILE_SIZE * LEVEL_SIZE_X - SHELL_SIZE)) {
		shell_ricochet(shell, RIGHT);
	}
	if(shell->phys.position_y > to_ufix(LCD_WIDTH + 50)) {
		shell_ricochet(shell, UP);
	} else if(shell->phys.position_y > to_ufix(TILE_SIZE * LEVEL_SIZE_Y - SHELL_SIZE)) {
		shell_ricochet(shell, DOWN);
	}

	reflect = getTileReflect(&shell->phys, false, tiles);

	if(reflect.colliding) {
		shell_ricochet(shell, reflect.dir);
	}	
}

void handleInput() {
	Tank* player = &tanks[0];
	bool moving = true;
	uint8_t target_rot = 0;
	uint8_t keys = 0;

	kb_Scan();

	if(kb_Data[7] & kb_Down)  keys |= DOWN;
	if(kb_Data[7] & kb_Left)  keys |= LEFT;
	if(kb_Data[7] & kb_Right) keys |= RIGHT;
	if(kb_Data[7] & kb_Up)    keys |= UP;

	//Right is 0 degrees

	switch(keys) {
		default:
			moving = false;
			break;
		case UP:
			target_rot = 192;
			break;
		case DOWN:
			target_rot = 64;
			break;
		case LEFT:
			target_rot = 128;
			break;
		case RIGHT:
			target_rot = 0;
			break;
		case UP|RIGHT:
			target_rot = 224;
			break;
		case DOWN|RIGHT:
			target_rot = 32;
			break;
		case UP|LEFT:
			target_rot = 160;
			break;
		case DOWN|LEFT:
			target_rot = 96;
	}

	if(moving) {
		int8_t diff = (int8_t)(player->tread_rot - target_rot);
		if(diff > 64 || diff < -64) {
			player->tread_rot += 128;
			diff = (int8_t)(player->tread_rot - target_rot);
		}
		if(diff < -PLAYER_TREAD_ROTATION) {
			player->tread_rot += PLAYER_TREAD_ROTATION;
		} else if(diff > PLAYER_TREAD_ROTATION) {
			player->tread_rot -= PLAYER_TREAD_ROTATION;
		} else {
			player->tread_rot = target_rot;
		}

		if(diff <= 32 && diff >=-32) {
			player->phys.position_x += TANK_SPEED_NORMAL * fast_cos(player->tread_rot);
			player->phys.position_y += TANK_SPEED_NORMAL * fast_sin(player->tread_rot);
		}
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
	if(kb_Data[1] & kb_Del) { // TODO: remove
		game.status = NEXT_LEVEL;
	}
	if(kb_Data[6] & kb_Clear) {
		game.status = QUIT;
	}
}

//TODO: compress sprites
// ^ Might not be totally necessary b/c program is already compressed
//TODO: tank sprites
//TODO: crosshair / direction indicator
//TODO: bonus tank every 5th level
