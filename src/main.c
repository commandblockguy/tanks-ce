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
#include "gfx/tiles_gfx.h"

typedef struct {
    Level level; //level currently being played
	uint8_t mission; //The mission number, always displayed 1 higher than stored. Also used as an index for levels.
	uint8_t lives; //Number of remaining tanks. This includes the tank that is currently in use, so a value of 1 means that the game will end the next time the tank is hit.
	uint8_t kills; //Number of enemy tanks destroyed.
	uint24_t timer; //Game time, probably used for physics stuff.
	uint16_t cursor_x; //If I decide to implement a cursor mode, this will represent the position of the crosshairs on the screen.
	uint8_t cursor_y;  //Otherwise, this will be removed
	uint24_t lastCycle; //Time the last physics cycle started
	bool inProgress; //Whether a mission is in progress
} Game;

void displayScores(void); //Display high scores

void startMission(bool initial); //Start a mission and reset various tank things.

void missionStart(uint8_t mission, uint8_t lives, uint8_t num_tanks); //Display the mission start screen

void processTank(Tank* tank); //Process tank physics, along with that tank's shells and mines

void render(void); //Render tilemap, tanks, and UI

void renderAABB(AABB bb);

void displayUI(void); //Display UI during a mission

void processCollisions(void); //Process collisions between all entities that could reasonably intersect.

void stablizeFPS(uint8_t target);

void resetFPSCounter(void); //Start the tick and reset the tick time counter

void handleInput(); //Handles inputs from the keypad

void memView(uint8_t* pointer);

Game game; //Game global, so I can reuse those names elsewhere

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
        	int i;
        	resetFPSCounter();
        	//handle player input
            //process physics
            for(i = 0; i < game.level.num_tanks; i++) {
            	processTank(&tanks[i]);
            }

            handleInput();

            processCollisions();
            
            render();
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
	int remaining_tanks = 0;
	for(i = 0; i < game.level.num_tanks; i++) {
		Tank* tank = &tanks[i];
		int j;
		if(initial) tank->alive = true;
		if(tank->alive) remaining_tanks++;
		tank->pos_x = (float)(tileToXPixel(tank->start_x));
		tank->pos_y = (float)(tileToYPixel(tank->start_y));
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

void displayScores(void) {

}

void missionStart(uint8_t mission, uint8_t lives, uint8_t num_tanks) {

}

void render(void) {
	int i = 0;
	gfx_tilemap_t tilemap; //Tilemap config struct

	tilemap.map 		= tiles;
	tilemap.tiles 		= tileset_tiles;
	tilemap.type_width 	= gfx_tile_no_pow2;
	tilemap.type_height = gfx_tile_no_pow2;
	tilemap.tile_height = TILE_SIZE;
	tilemap.tile_width	= TILE_SIZE;
	tilemap.draw_height	= LEVEL_SIZE_Y;
	tilemap.draw_width 	= LEVEL_SIZE_X;
	tilemap.height 		= LEVEL_SIZE_Y;
	tilemap.width		= LEVEL_SIZE_X;
	tilemap.y_loc		= 0;
	tilemap.x_loc		= MAP_OFFSET_X;

	gfx_FillScreen(gfx_white);

	//Render level tiles
	gfx_Tilemap(&tilemap, 0, 0);

	for(i = 0; i < game.level.num_tanks; i++) {
		//Render tanks
		int j;
		uint16_t center_x;
		uint16_t center_y;
		AABB bb;
		Tank* tank = &tanks[i];
		gfx_SetTextXY((uint16_t)tank->pos_x, (uint8_t)tank->pos_y);
		gfx_PrintUInt(tank->type, 1);
		bb = getTankAABB(tank);
		renderAABB(bb);
		center_x = tank->pos_x + TANK_SIZE / 2;
		center_y = tank->pos_y + TANK_SIZE / 2;
		gfx_Line(center_x, center_y, center_x + tank->bullet_spawn_x, center_y + tank->bullet_spawn_y);

		//draw shell hitboxes until I can get sprites
		for(j = max_shells[tank->type] - 1; j >= 0; j--) {
			Shell* shell = &tank->shells[j];
			AABB bb;
			if(!(shell->alive)) continue;
			bb = getShellAABB(shell);
			renderAABB(bb);
		}
		for(j = max_mines[tank->type] - 1; j >= 0; j--) {
			Mine* mine = &tank->mines[j];
			AABB bb;
			if(!(mine->alive)) continue;
			bb = getMineAABB(mine);
			renderAABB(bb);
		}
	}

	gfx_SetTextXY(0,0);
	gfx_PrintUInt(timer_1_Counter / 32.768 , 4);

	gfx_BlitBuffer();

	while(!os_GetCSC);

}

void resetFPSCounter() {
	//woo copypaste!
	/* Disable the timer so it doesn't run when we don't want it to be running */
    timer_Control = TIMER1_DISABLE;

    timer_1_Counter = 0;

    /* Enable the timer, set it to the 32768 kHz clock, enable an interrupt once it reaches 0, and make it count down */
	timer_Control = TIMER1_ENABLE | TIMER1_32K | TIMER1_NOINT | TIMER1_UP;
}

void memView(uint8_t* pointer) {
	int i, j;
	for(i = 0; i < 12; i++) {
		for(j = 0; j < 8; j++) {
			gfx_SetTextXY(40 * j, 20 * i);
			gfx_PrintUInt(pointer[8 * i + j], 3);
		}
	}
	gfx_BlitBuffer();
}

void renderAABB(AABB bb) {
	gfx_SetColor(7);
	gfx_Rectangle(bb.x1, bb.y1, bb.x2 - bb.x1, bb.y2 - bb.y1);
}

//Process tank physics
void processTank(Tank* tank) {
	int i;
	//Loop through all shells
	for(i = max_shells[tank->type] - 1; i >= 0; i--) {
		Shell* shell = &tank->shells[i];
		//Ignore dead shells
		if(!shell->alive) continue;
		//Add velocity
		shell->pos_x += shell->vel_x;
		shell->pos_y += shell->vel_y;
		//This will eventually be part of the collision bit
		if(	shell->pos_x < MAP_OFFSET_X ||
			shell->pos_x > MAP_OFFSET_X + TILE_SIZE * LEVEL_SIZE_X ||
			shell->pos_y < 0 ||
			shell->pos_y > TILE_SIZE * LEVEL_SIZE_Y ) shell->alive = false;
	}
	if(!max_mines[tank->type]) return;
	for(i = max_mines[tank->type] - 1; i >= 0; i--) {
		Mine* mine = &tank->mines[i];
		//Ignore dead shells
		if(!mine->alive) continue;
		//TODO: check for nearby enemy tanks and set counter if necessary
		if(--mine->countdown == 0) {
			//TODO: kill things
			//TODO: explosions
			//TODO: sound effects for explosions
			//TODO: USB drivers for sound effects for explosions
			//TODO: be realistic abouot USB drivers for sound effects for explosions
			mine->alive = false;
		}
	}
}

void handleInput() {
	Tank* player = &tanks[0];

	kb_Scan();

	//TODO: Replace with fancy roatation stuff if necessary
	if(kb_Data[7] & kb_Down) {
		player->pos_y += TANK_SPEED_NORMAL;
	}
	if(kb_Data[7] & kb_Left) {
		player->pos_x -= TANK_SPEED_NORMAL;
	}
	if(kb_Data[7] & kb_Right) {
		player->pos_x += TANK_SPEED_NORMAL;
	}
	if(kb_Data[7] & kb_Up) {
		player->pos_y -= TANK_SPEED_NORMAL;
	}
	if(kb_Data[1] & kb_2nd) {
		fire_shell(player);
	}
	if(kb_Data[2] & kb_Alpha) {
		lay_mine(player);
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

//TODO: compress sprites
//TODO: basic physics
//TODO: tank sprites
