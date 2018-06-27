#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "objects.h"
#include "level.h"
#ifdef CREATE_LEVEL_APPVAR
#include "tiles/tilemaps.h"
#endif

uint16_t tileToXPixel(uint8_t tile_x) {
	return MAP_OFFSET_X + tile_x * TILE_SIZE;
}

uint8_t tileToYPixel(uint8_t tile_y) {
	return tile_y * TILE_SIZE;
}

uint8_t pixelToXTile(uint24_t pix_x) {
	return (pix_x - MAP_OFFSET_X) / TILE_SIZE;
}

uint8_t pixelToYTile(uint8_t pix_y) {
	return pix_y / TILE_SIZE;
}

void createLevels(void) {
	#ifdef CREATE_LEVEL_APPVAR
	LevelPack lvl_pack = {"TANKS!", 14, {0, 0, 0, 0, 0}};
	Level lvls[] = {{25, 2}, {22, 2}, {29, 4}, {37, 5}, {22, 3}, {32, 5}, {23, 5}, {47, 6}, {31, 7}, {32, 3}, {46, 7}, {41, 5}, {26, 7}, {41, 7}};
	SerializedTank* ser_tanks;
	SerializedTank ser_tanks1[]  = {{PLAYER, 2, 6}, {IMMOBILE, 19, 6}};
	SerializedTank ser_tanks2[]  = {{PLAYER, 2, 13}, {BASIC, 19, 3}};
	SerializedTank ser_tanks3[]  = {{PLAYER, 2, 8}, {BASIC, 5, 1}, {BASIC, 18, 15}, {IMMOBILE, 19, 8}};
	SerializedTank ser_tanks4[]  = {{PLAYER, 3, 14}, {BASIC, 10, 8}, {BASIC, 18, 2}, {IMMOBILE, 18, 8}, {IMMOBILE, 11, 2}};
	SerializedTank ser_tanks5[]  = {{PLAYER, 2, 13}, {MISSILE, 14, 1}, {MISSILE, 20, 9}};
	SerializedTank ser_tanks6[]  = {{PLAYER, 2, 8}, {BASIC, 17, 4}, {BASIC, 17, 15}, {MISSILE, 19, 8}, {MISSILE, 20, 13}};
	SerializedTank ser_tanks7[]  = {{PLAYER, 2, 14}, {MISSILE, 1, 1}, {MISSILE, 19, 2}, {MISSILE, 2, 7}, {MISSILE, 19, 15}};
	SerializedTank ser_tanks8[]  = {{PLAYER, 1, 8}, {MISSILE, 21, 2}, {MISSILE, 21, 14}, {MINE, 15, 4}, {MINE, 18, 8}, {MINE, 14, 14}};
	SerializedTank ser_tanks9[]  = {{PLAYER, 2, 13}, {MINE, 3, 3}, {MINE, 19, 13}, {BASIC, 7, 1}, {BASIC, 13, 5}, {BASIC, 14, 15}, {BASIC, 19, 3}};
	SerializedTank ser_tanks10[] = {{PLAYER, 1, 12}, {RED, 11, 1}, {RED, 20, 4}};
	SerializedTank ser_tanks11[] = {{PLAYER, 2, 2}, {BASIC, 3, 13}, {BASIC, 14, 1}, {MISSILE, 7, 0}, {MISSILE, 19, 14}, {RED, 19, 4}, {RED, 10, 8}};
	SerializedTank ser_tanks12[] = {{PLAYER, 2, 7}, {RED, 12, 1}, {RED, 10, 14}, {IMMOB_MISSILE, 17, 15}, {IMMOB_MISSILE, 19, 4}};
	SerializedTank ser_tanks13[] = {{PLAYER, 0, 8}, {MINE, 5, 15}, {MINE, 8, 1}, {MINE, 12, 10}, {MISSILE, 20, 1}, {MISSILE, 20, 15}, {MISSILE, 21, 8}};
	SerializedTank ser_tanks14[] = {{PLAYER, 1, 15}, {RED, 1, 7}, {RED, 15, 1}, {RED, 20, 11}, {IMMOB_MISSILE, 6, 6}, {IMMOB_MISSILE, 15, 11}, {IMMOB_MISSILE, 20, 1}};
	ti_var_t appVar;
	int i;

	ti_CloseAll();

	appVar = ti_Open("TANKSLPK", "w");

	ti_Write(&lvl_pack, sizeof(LevelPack), 1, appVar);
	for(i = 0; i < lvl_pack.num_levels; i++) {
		uint8_t* comp_tiles;
		ti_Write(lvls + i, sizeof(Level), 1, appVar);
		switch(i) {
			case 0:
			default:
				comp_tiles = lvl1_compressed;
				ser_tanks = ser_tanks1;
				break;
			case 1:
				comp_tiles = lvl2_compressed;
				ser_tanks = ser_tanks2;
				break;
			case 2:
				comp_tiles = lvl3_compressed;
				ser_tanks = ser_tanks3;
				break;
			case 3:
				comp_tiles = lvl4_compressed;
				ser_tanks = ser_tanks4;
				break;
			case 4:
				comp_tiles = lvl5_compressed;
				ser_tanks = ser_tanks5;
				break;
			case 5:
				comp_tiles = lvl6_compressed;
				ser_tanks = ser_tanks6;
				break;
			case 6:
				comp_tiles = lvl7_compressed;
				ser_tanks = ser_tanks7;
				break;
			case 7:
				comp_tiles = lvl8_compressed;
				ser_tanks = ser_tanks8;
				break;
			case 8:
				comp_tiles = lvl9_compressed;
				ser_tanks = ser_tanks9;
				break;
			case 9:
				comp_tiles = lvl10_compressed;
				ser_tanks = ser_tanks10;
				break;
			case 10:
				comp_tiles = lvl11_compressed;
				ser_tanks = ser_tanks11;
				break;
			case 11:
				comp_tiles = lvl12_compressed;
				ser_tanks = ser_tanks12;
				break;
			case 12:
				comp_tiles = lvl13_compressed;
				ser_tanks = ser_tanks13;
				break;
			case 13:
				comp_tiles = lvl14_compressed;
				ser_tanks = ser_tanks14;
				break;
		}
		ti_Write(comp_tiles, sizeof(uint8_t), lvls[i].compressed_tile_size, appVar);
		ti_Write(ser_tanks, sizeof(SerializedTank), lvls[i].num_tanks, appVar);
	}

	ti_CloseAll();

	/*
	1: blue:(3,7), brown(20,7)
	2: blue:(3,14), grey:(20,4)
	3: blue:(3,9), grey:(6,2), grey:(19,16), brown:(20,9)
	*/
	#endif
}

Tank deserializeTank(SerializedTank ser_tank) {
	Tank result = {0};
	result.type = ser_tank.type;
	result.start_x = ser_tank.start_x;
	result.start_y = ser_tank.start_y;
	result.phys.position_x = tileToXPixel(ser_tank.start_x) << SHIFT_AMOUNT;
	result.phys.position_y = tileToYPixel(ser_tank.start_y) << SHIFT_AMOUNT;
	result.phys.height = TANK_SIZE;
	result.phys.width = TANK_SIZE;
	result.barrel_rot = 0;
	result.tread_rot = 0;
	calc_bullet_spawn(&result);
	return result;
}
