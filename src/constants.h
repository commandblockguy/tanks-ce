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

#include "level.h"

//Comment this out to stop the levels from being bundled with the program
#define CREATE_LEVEL_APPVAR

//Target frame rate
#define TARGET_FPS 30

//Offset from sides of screen
#define MAP_OFFSET_X 16

//Size of the gameplay area in tiles
#define LEVEL_SIZE_X 22
#define LEVEL_SIZE_Y 17

//Pixel size of each square object
#define TILE_SIZE 13
#define TANK_SIZE 13
#define SHELL_SIZE 3
#define MINE_SIZE 13

//Shifted by 6 bits
typedef uint16_t ufix;
typedef int16_t fix;
#define SHIFT_AMOUNT 6
#define SHIFT_MASK ((1 << SHIFT_AMOUNT) - 1)

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

//Player action cooldown
#define SHOT_COOLDOWN 5
#define MINE_COOLDOWN 10

//TODO: Radius in pixels that enemy tanks will cause mines to explode
#define MINE_DETECT_RANGE 20
//TODO: Radius in pixels that the centers of objects must be inside to be blown up by mines
//I know this is inaccurate but mines are so rarely used it really doesn't matter
#define MINE_EXPLOSION_RADIUS 20

//TODO: rate at which things turn
#define PLAYER_BARREL_ROTATION 4
#define PLAYER_TREAD_ROTATION 0

//TODO: amount of time in milliseconds the mission start screen displays
#define MISSION_START_TIME 2000
//Font size
#define MISSION_NUMBER_TEXT 3
#define ENEMY_TANK_TEXT 2

#define BLACK 7
#define WHITE gfx_black

//Number and pixel size of subregions to be used in collision detection
#define COLLISION_SUBREGIONS_X 4
#define COLLISION_SUBREGIONS_Y 4
#define SUBREGION_SIZE_X (TILE_SIZE << SHIFT_AMOUNT) * LEVEL_SIZE_X / COLLISION_SUBREGIONS_X 
#define SUBREGION_SIZE_Y (TILE_SIZE << SHIFT_AMOUNT) * LEVEL_SIZE_Y / COLLISION_SUBREGIONS_Y

#define ROT_UNITS_TO_RADIANS M_PI / 128

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
	uint8_t shotCooldown; //How many more ticks before we can fire another shot
	uint8_t mineCooldown;
} Game;

#endif /* H_CONSTANTS */
