#include <stddef.h>
#include <stdint.h>

#include <stdlib.h>
#include <fileioc.h>
#include <compression.h>

#include "level.h"
#include "util.h"
//Comment this out to stop the levels from being bundled with the program
#define CREATE_LEVEL_APPVAR
#ifdef CREATE_LEVEL_APPVAR

#include "tiles/lvlpack.h"

#endif

#include "tank.h"
#include "globals.h"


// todo: maybe just invoke the level creator once that's finished instead of doing this
void create_levels(void) {
#ifdef CREATE_LEVEL_APPVAR
#define s(n) {sizeof(lvl##n##_compressed), sizeof(ser_tanks##n)/sizeof(ser_tanks##n[0])}
    const serialized_tank_t *ser_tanks; //TODO: change to a 2D array somehow?
    const serialized_tank_t ser_tanks1[] = {{PLAYER,   1,  6},
                                            {IMMOBILE, 14, 6}};
    const serialized_tank_t ser_tanks2[] = {{PLAYER, 1,  13},
                                            {BASIC,  14, 3}};
    const serialized_tank_t ser_tanks3[] = {{PLAYER,   1,  8},
                                            {BASIC,    3,  1},
                                            {BASIC,    13, 15},
                                            {IMMOBILE, 14, 8}};
    const serialized_tank_t ser_tanks4[] = {{PLAYER,   2,  14},
                                            {BASIC,    7,  8},
                                            {BASIC,    13, 2},
                                            {IMMOBILE, 13, 8},
                                            {IMMOBILE, 8,  2}};
    const serialized_tank_t ser_tanks5[] = {{PLAYER,  2,  14},
                                            {MISSILE, 12, 1},
                                            {MISSILE, 15, 9}};
    const serialized_tank_t ser_tanks6[] = {{PLAYER,  1,  8},
                                            {BASIC,   12, 4},
                                            {BASIC,   12, 15},
                                            {MISSILE, 14, 8},
                                            {MISSILE, 14, 13}};
    const serialized_tank_t ser_tanks7[] = {{PLAYER,  1,  14},
                                            {MISSILE, 1,  1},
                                            {MISSILE, 14, 2},
                                            {MISSILE, 1,  7},
                                            {MISSILE, 14, 15}};
    const serialized_tank_t ser_tanks8[] = {{PLAYER,  0,  8},
                                            {MISSILE, 15, 2},
                                            {MISSILE, 15, 14},
                                            {MINE,    11, 4},
                                            {MINE,    15, 8},
                                            {MINE,    10, 14}};
    const serialized_tank_t ser_tanks9[] = {{PLAYER, 2,  13},
                                            {MINE,   2,  3},
                                            {MINE,   14, 13},
                                            {BASIC,  5,  1},
                                            {BASIC,  10, 5},
                                            {BASIC,  11, 15},
                                            {BASIC,  14, 3}};
    const serialized_tank_t ser_tanks10[] = {{PLAYER, 0,  12},
                                             {RED,    8,  1},
                                             {RED,    15, 4}};
    const serialized_tank_t ser_tanks11[] = {{PLAYER,  1,  2},
                                             {BASIC,   2,  13},
                                             {BASIC,   10, 1},
                                             {MISSILE, 5,  0},
                                             {MISSILE, 14, 14},
                                             {RED,     14, 4},
                                             {RED,     7,  8}};
    const serialized_tank_t ser_tanks12[] = {{PLAYER,        1,  7},
                                             {RED,           10, 1},
                                             {RED,           6,  14},
                                             {IMMOB_MISSILE, 13, 15},
                                             {IMMOB_MISSILE, 14, 4}};
    const serialized_tank_t ser_tanks13[] = {{PLAYER,  0,  8},
                                             {MINE,    4,  15},
                                             {MINE,    7,  1},
                                             {MINE,    9,  10},
                                             {MISSILE, 14, 1},
                                             {MISSILE, 14, 15},
                                             {MISSILE, 15, 8}};
    const serialized_tank_t ser_tanks14[] = {{PLAYER,        1,  15},
                                             {RED,           1,  7},
                                             {RED,           10, 1},
                                             {RED,           14, 11},
                                             {IMMOB_MISSILE, 5,  6},
                                             {IMMOB_MISSILE, 10, 11},
                                             {IMMOB_MISSILE, 14, 1}};
    const serialized_tank_t ser_tanks15[] = {{PLAYER, 1,  15},
                                             {FAST,   4,  5},
                                             {FAST,   14, 2},
                                             {FAST,   14, 15}};
    const serialized_tank_t ser_tanks16[] = {{PLAYER,        1,  15},
                                             {FAST,          7,  1},
                                             {IMMOB_MISSILE, 14, 1},
                                             {FAST,          10, 8},
                                             {IMMOB_MISSILE, 6,  11},
                                             {FAST,          15, 11}};
    const serialized_tank_t ser_tanks17[] = {{PLAYER,        1,  13},
                                             {IMMOB_MISSILE, 2,  2},
                                             {IMMOB_MISSILE, 14, 3},
                                             {IMMOB_MISSILE, 1,  8},
                                             {IMMOB_MISSILE, 7,  8},
                                             {IMMOB_MISSILE, 14, 9},
                                             {IMMOB_MISSILE, 14, 14}};
    const serialized_tank_t ser_tanks18[] = {{PLAYER,        2,  13},
                                             {MISSILE,       3,  2},
                                             {FAST,          10, 2},
                                             {RED,           5,  4},
                                             {IMMOB_MISSILE, 8,  8},
                                             {MISSILE,       13, 14},
                                             {FAST,          10, 15}};
    const serialized_tank_t ser_tanks19[] = {{PLAYER, 1,  15},
                                             {FAST,   1,  1},
                                             {FAST,   7,  1},
                                             {FAST,   12, 1},
                                             {FAST,   8,  5},
                                             {FAST,   14, 6},
                                             {FAST,   1,  8},
                                             {FAST,   14, 11},
                                             {FAST,   12, 15}};
    const serialized_tank_t ser_tanks20[] = {{PLAYER,    2,  8},
                                             {INVISIBLE, 11, 6},
                                             {INVISIBLE, 13, 8}};
    level_t lvls[] = {s(1), s(2), s(3), s(4), s(5), s(6), s(7), s(8), s(9), s(10), s(11), s(12), s(13), s(14), s(15),
                      s(16), s(17), s(18), s(19), s(20)};
    level_pack_t lvl_pack = {"TANKS!", sizeof(lvls) / sizeof(lvls[0]), {0, 0, 0, 0, 0}};
    ti_var_t appVar;
    int i;

    ti_CloseAll();

    appVar = ti_Open("TANKSLPK", "w");

    ti_Write(&lvl_pack, sizeof(level_pack_t), 1, appVar);
    for(i = 0; i < lvl_pack.num_levels; i++) {
        uint8_t *comp_tiles;
        ti_Write(lvls + i, sizeof(level_t), 1, appVar);
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
            case 14:
                comp_tiles = lvl15_compressed;
                ser_tanks = ser_tanks15;
                break;
            case 15:
                comp_tiles = lvl16_compressed;
                ser_tanks = ser_tanks16;
                break;
            case 16:
                comp_tiles = lvl17_compressed;
                ser_tanks = ser_tanks17;
                break;
            case 17:
                comp_tiles = lvl18_compressed;
                ser_tanks = ser_tanks18;
                break;
            case 18:
                comp_tiles = lvl19_compressed;
                ser_tanks = ser_tanks19;
                break;
            case 19:
                comp_tiles = lvl20_compressed;
                ser_tanks = ser_tanks20;
                break;
        }
        ti_Write(comp_tiles, sizeof(uint8_t), lvls[i].compressed_tile_size, appVar);
        ti_Write(ser_tanks, sizeof(serialized_tank_t), lvls[i].num_tanks, appVar);
    }

    ti_CloseAll();
    ti_SetArchiveStatus("TANKSLPK", true);

#endif
}

void decompress_tiles(const void *comp_tiles) {
    //Decompress tile data
    zx7_Decompress(tiles, comp_tiles);
    for(uint8_t row = LEVEL_SIZE_Y - 2; row > 0; row--) {
        tile_t (*orig_tiles)[LEVEL_SIZE_X - 2] = (tile_t(*)[LEVEL_SIZE_X - 2])tiles;
        memmove(&tiles[row][1], orig_tiles[row - 1], LEVEL_SIZE_X - 2);
        tiles[row][0] = tiles[row][LEVEL_SIZE_X - 1] = 1;
    }
    memset(tiles[0], 1, LEVEL_SIZE_X);
    memset(tiles[LEVEL_SIZE_Y - 1], 1, LEVEL_SIZE_X);
}
