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

void createLevels(void) {
	#ifdef CREATE_LEVEL_APPVAR
	LevelPack lvl_pack = {"TANKS!", 3, {0, 0, 0, 0, 0}};
	Level lvls[] = {{25, 2}, {22, 2}, {29, 4}};
	SerializedTank* ser_tanks;
	SerializedTank ser_tanks1[] = {{PLAYER, 2, 6}, {IMMOBILE, 19, 6}};
	SerializedTank ser_tanks2[] = {{MISSILE, 2, 13}, {BASIC, 19, 3}};
	SerializedTank ser_tanks3[] = {{PLAYER, 2, 8}, {BASIC, 5, 1}, {BASIC, 18, 15}, {IMMOBILE, 19, 8}};
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
	Tank result;
	result.type = ser_tank.type;
	result.start_x = ser_tank.start_x;
	result.start_y = ser_tank.start_y;
	result.pos_x = (float)tileToXPixel(ser_tank.start_x);
	result.pos_y = (float)tileToYPixel(ser_tank.start_y);
	return result;
}
