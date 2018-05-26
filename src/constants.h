#ifndef H_CONSTANTS
#define H_CONSTANTS

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Comment this out to stop the levels from being bundled with the program
#define CREATE_LEVEL_APPVAR

//Target frame rate
#define TARGET_FPS 30

//Offset from sides of screen
#define MAP_OFFSET_X 16

//Pixel size of each square object
#define TILE_SIZE 13
#define TANK_SIZE 13
#define SHELL_SIZE 3
#define MINE_SIZE 13

//Distance from center of tank new bullets appear
#define BARREL_LENGTH 5

//TODO: Driving speed of tanks
#define TANK_SPEED_NORMAL 2
#define TANK_SPEED_FAST 5

//Speed of bullets in pixels per tick
#define SHELL_SPEED_STANDARD 0.351083438 * TARGET_FPS / TILE_SIZE
#define SHELL_SPEED_MISSILE 0.587418544 * TARGET_FPS / TILE_SIZE

//TODO: Time till mine detonation
#define MINE_COUNTDOWN 100
#define MINE_TRIGGERED 10

//TODO: Radius in pixels that enemy tanks will cause mines to explode
#define MINE_DETECT_RANGE 20

//TODO: rate at which things turn
#define PLAYER_BARREL_ROTATION 4
#define PLAYER_TREAD_ROTATION 0

#define ROT_UNITS_TO_RADIANS M_PI / 128

#endif /* H_CONSTANTS */
