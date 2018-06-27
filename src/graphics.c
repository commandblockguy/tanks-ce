#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <graphx.h>

#include "constants.h"
#include "objects.h"
//Hopefully we can remove this when we remove AABB rendering
//Hopefully we can now stop referring to myself in the plural
#include "collision.h"
#include "level.h"
#include "graphics.h"
#include "gfx/tiles_gfx.h"


void displayScores(void) {

}

void missionStart(uint8_t mission, uint8_t lives, uint8_t num_tanks) {
	gfx_FillScreen(gfx_white);

	gfx_SetColor(gfx_red);
	//TODO: get proper values for this
	gfx_FillRectangle_NoClip(0, 60, LCD_WIDTH, 80);

	//Print mission number
	gfx_SetTextScale(MISSION_NUMBER_TEXT, MISSION_NUMBER_TEXT);
	gfx_SetTextFGColor(WHITE);
	gfx_SetTextXY((LCD_WIDTH - 60 * MISSION_NUMBER_TEXT) / 2, 70);
	gfx_PrintString("Mission ");
	gfx_PrintUInt(mission + 1, 1 + (mission >= 9) + (mission >= 99));
	gfx_SetTextScale(ENEMY_TANK_TEXT, ENEMY_TANK_TEXT);
	gfx_SetTextXY((LCD_WIDTH - 97 * ENEMY_TANK_TEXT) / 2, 110);
	gfx_PrintString("Enemy Tanks: ");
	gfx_PrintUInt(num_tanks, 1);
	gfx_SetTextXY((LCD_WIDTH - 8 * MISSION_NUMBER_TEXT) / 2, 150);
	gfx_SetTextFGColor(BLACK); // Will be blue once I get pallettes working
	gfx_PrintString("x   ");
	gfx_PrintUInt(lives, 1);
	gfx_SetTextFGColor(BLACK);
	gfx_SetTextScale(1, 1);
	//Print number of tanks
	//Print number of lives

	gfx_BlitBuffer();

	delay(MISSION_START_TIME);
}

void render(uint8_t* tiles, Level* level, Tank* tanks) {
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
	gfx_Tilemap_NoClip(&tilemap, 0, 0);

	for(i = 0; i < level->num_tanks; i++) {
		//Render tanks
		int j;
		AABB bb;
		Tank* tank = &tanks[i];
		if(tank->alive) {
			gfx_SetTextXY(tank->phys.position_x >> SHIFT_AMOUNT, tank->phys.position_y >> SHIFT_AMOUNT);
			gfx_PrintUInt(tank->type, 1);
			bb = getAABB(&tank->phys);
			renderAABB(bb);
			gfx_Line(center_x(bb), center_y(bb), center_x(bb) + tank->bullet_spawn_x, center_y(bb) + tank->bullet_spawn_y);
		}

		//draw shell hitboxes until I can get sprites
		for(j = max_shells[tank->type] - 1; j >= 0; j--) {
			Shell* shell = &tank->shells[j];
			AABB bb;
			if(!(shell->alive)) continue;
			bb = getAABB(&shell->phys);
			renderAABB(bb);
		}
		for(j = max_mines[tank->type] - 1; j >= 0; j--) {
			Mine* mine = &tank->mines[j];
			AABB bb;
			if(!(mine->alive)) continue;
			bb = getAABB(&mine->phys);
			renderAABB(bb);
		}
	}

	gfx_SetTextXY(0,0);
	gfx_PrintUInt(timer_1_Counter / 32.768 , 4);

	gfx_BlitBuffer();

	while(!os_GetCSC);

}

void renderAABB(AABB bb) {
	gfx_SetColor(7);
	gfx_Rectangle(bb.x1, bb.y1, bb.x2 - bb.x1, bb.y2 - bb.y1);
	gfx_SetPixel(center_x(bb), center_y(bb));
}
