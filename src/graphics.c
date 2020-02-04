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
#include "util.h"
#include "keypadc.h"


void displayScores(void) {

}

//Screen is 700 (240) pixels tall
//Background is (228,230,173)
//140 (48) pixels between the top and banner
//294 (?) pixel tall red banner
//Red banner has checkerboard with 38x52 (12x15) pixel rhombi
//Colors of rhombi are (174,49,48) and (164,45,46)
//16 (5) pixel shadow underneath (183,185,139)
//10 (3) pixel gap on top, 12 (4) on bottom
//12 (4) pixel tall gold (193,162,43) band on top and bottom
//40 (14) (12) pixels between band and text
//60 (21) (24) pixel tall mission number text w/ shadow
//50 (17) (16) pixels between
//44 (15) (16) pixel tall Enemies Remaining text w/ shadow
//50 (17) (16) pixels between text and bottom band
//Text shadow (134,36,37) has 8px (3px) offset
//# of lives text (70,127,111) - centered between bottom or ribbon and bottom of screen
//todo: fix right side
void missionStart(uint8_t mission, uint8_t lives, uint8_t num_tanks) {
	int x, y;
	gfx_FillScreen(COL_BG);

	gfx_SetColor(COL_RHOM_1);
	
	gfx_FillRectangle_NoClip(0, 48, LCD_WIDTH, 100);
	gfx_SetColor(COL_RHOM_2);
	for(x = 0; x < LCD_WIDTH / 12; x++) {
		for(y = 0; y < 6; y++) {
			gfx_FillTriangle_NoClip(x * 12 + 6, 55 + y * 15, x * 12, y * 15 + 55 + 7, x * 12 + 12, y * 15 + 55 + 7);
			gfx_FillTriangle_NoClip(x * 12 + 6, 55 + y * 15 + 15, x * 12, y * 15 + 55 + 7, x * 12 + 12, y * 15 + 55 + 7);
		}
	}

	gfx_SetColor(COL_RIB_SHADOW);
	gfx_FillRectangle_NoClip(0, 148, LCD_WIDTH, 4);

	gfx_SetColor(COL_GOLD);
	gfx_FillRectangle_NoClip(0, 51, LCD_WIDTH, 4);
	gfx_FillRectangle_NoClip(0, 140, LCD_WIDTH, 4);

	//Print mission number
	gfx_SetTextScale(MISSION_NUMBER_TEXT, MISSION_NUMBER_TEXT);
	gfx_SetTextFGColor(COL_TXT_SHADOW);
	gfx_SetTextXY((LCD_WIDTH - 60 * MISSION_NUMBER_TEXT) / 2 + 3, 70 + 3);
	gfx_PrintString("Mission ");
	gfx_PrintUInt(mission + 1, 1 + (mission >= 9) + (mission >= 99));

	gfx_SetTextFGColor(COL_BG);
	gfx_SetTextXY((LCD_WIDTH - 60 * MISSION_NUMBER_TEXT) / 2, 70);
	gfx_PrintString("Mission ");
	gfx_PrintUInt(mission + 1, 1 + (mission >= 9) + (mission >= 99));


	gfx_SetTextScale(ENEMY_TANK_TEXT, ENEMY_TANK_TEXT);
	gfx_SetTextFGColor(COL_TXT_SHADOW);
	gfx_SetTextXY((LCD_WIDTH - 97 * ENEMY_TANK_TEXT) / 2 + 3, 110 + 3);
	gfx_PrintString("Enemy Tanks: ");
	gfx_PrintUInt(num_tanks, 1);
	gfx_SetTextXY((LCD_WIDTH - 8 * MISSION_NUMBER_TEXT) / 2, 150);

	gfx_SetTextFGColor(COL_BG);
	gfx_SetTextXY((LCD_WIDTH - 97 * ENEMY_TANK_TEXT) / 2, 110);
	gfx_PrintString("Enemy Tanks: ");
	gfx_PrintUInt(num_tanks, 1);


	gfx_SetTextXY((LCD_WIDTH - 8 * MISSION_NUMBER_TEXT) / 2 + 2, 186 + 2);
	gfx_SetTextFGColor(COL_RIB_SHADOW);
	gfx_PrintString("x   ");
	gfx_PrintUInt(lives, 1);

	gfx_SetTextXY((LCD_WIDTH - 8 * MISSION_NUMBER_TEXT) / 2, 186);
	gfx_SetTextFGColor(COL_LIVES_TXT);
	gfx_PrintString("x   ");
	gfx_PrintUInt(lives, 1);


	gfx_SetTextFGColor(COL_BLACK);
	gfx_SetTextScale(1, 1);

	//TODO: tank sprite

	gfx_BlitBuffer();

	//Delay for some time, or wait for a keypress
    timer_Control = TIMER1_DISABLE;
    timer_1_ReloadValue = timer_1_Counter = 33 * MISSION_START_TIME;
	timer_Control = TIMER1_ENABLE | TIMER1_32K | TIMER1_0INT | TIMER1_DOWN;

	while(true) {
		if(timer_IntStatus & TIMER1_RELOADED) {
			timer_IntAcknowledge = TIMER1_RELOADED;
			break;
		}
		kb_Scan();
		if(kb_Data[1] & kb_2nd || kb_Data[1] & kb_Del || kb_Data[6] & kb_Clear) {
			while(kb_Data[1] || kb_Data[6]) kb_Scan();
			break;
		}
	}
}

void redraw(uint8_t* tiles, Level* level) {
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

	gfx_FillScreen(COL_WHITE);

	//Render level tiles
	gfx_Tilemap_NoClip(&tilemap, 0, 0);
}

void render(uint8_t* tiles, Level* level, Tank* tanks) {
	int i = 0;
	
	//Eventually, this will only be called once
	//redraw(tiles, level);

	for(i = 0; i < level->num_tanks; i++) {
		//Render tanks
		int j;
		AABB bb;
		Tank* tank = &tanks[i];
		if(tank->alive) {
			bb = getAABB(&tank->phys);
			gfx_SetColor(COL_BLACK);
			renderAABB(bb);
			gfx_Line(
				from_ufix(center_x(&tank->phys)),
				from_ufix(center_y(&tank->phys)),
				from_ufix(center_x(&tank->phys) + fast_cos(tank->barrel_rot) * BARREL_LENGTH),
				from_ufix(center_y(&tank->phys) + fast_sin(tank->barrel_rot) * BARREL_LENGTH));
			gfx_SetTextFGColor(COL_RED);
			gfx_SetTextXY(from_ufix(tank->phys.position_x) + 1, from_ufix(tank->phys.position_y) + 1);
			gfx_PrintUInt(tank->type, 1);
		}

		//draw shell hitboxes until I can get sprites
		for(j = max_shells[tank->type] - 1; j >= 0; j--) {
			Shell* shell = &tank->shells[j];
			AABB bb;
			if(!(shell->alive)) continue;
			bb = getAABB(&shell->phys);
			gfx_SetColor(COL_BLACK);
			renderAABB(bb);
		}
		//draw mine hitboxes
		for(j = max_mines[tank->type] - 1; j >= 0; j--) {
			Mine* mine = &tank->mines[j];
			AABB bb;
			if(!mine->countdown) continue;
			gfx_SetColor(COL_RED);
			if(mine->alive) gfx_SetColor(COL_BLACK);
			bb = getAABB(&mine->phys);
			renderAABB(bb);
		}
	}

	gfx_SetTextXY(0,0);
	gfx_PrintUInt(fpsCounter(), 4);

	gfx_BlitBuffer();

	redraw(tiles, level); //todo: move back?

}

void renderAABB(AABB bb) {
	gfx_Rectangle(bb.x1, bb.y1, bb.x2 - bb.x1, bb.y2 - bb.y1);
}

void draw_line(LineSeg* ls) {
	gfx_Line(
		from_fix(ls->x1),
		from_fix(ls->y1),
		from_fix(ls->x2),
		from_fix(ls->y2)
	);
}

//TODO: UI stuff
