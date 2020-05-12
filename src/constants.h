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

//Comment this out to remove the debug overdraws
#define DBG_DRAW

//Target frame rate
#define TARGET_FPS 30

//Size of the gameplay area in tiles
#define LEVEL_SIZE_X 22
#define LEVEL_SIZE_Y 17

//Pixel size of each square object
#define TILE_SIZE 256
#define TANK_SIZE TILE_SIZE
#define SHELL_SIZE (TILE_SIZE * 3 / 14)
#define MINE_SIZE TILE_SIZE

//Distance from center of tank new shells appear
#define BARREL_LENGTH (TANK_SIZE * 5 / 14)

//148 px / 1 s * 1 tile / 48 px = 3.08 tiles / sec
#define TANK_SPEED_SLOW (2 * TILE_SIZE / TARGET_FPS) //TODO
#define TANK_SPEED_NORMAL (3.08 * TILE_SIZE / TARGET_FPS)
#define TANK_SPEED_BLACK 5 //TODO

//Speed of shells in pixels per tick
//Standard:
//434 px X / 48 px/tile = 11.4210526316 tiles
//157 px Y / 32 px/tile = 4.90625 tiles
//d = 12.4302748271 tile
//123 frames=2.05 seconds
//6.06354869615 tiles/second
#define SHELL_SPEED_STANDARD (6.06354869615 * TILE_SIZE / TARGET_FPS)
//Fast
//238 px X / 48 px/tile = 4.95833333333 tiles
//37  px Y / 32 px/tile = 1.15625 tiles
//d = 5.09136361959 tiles
//30 frames = 1/2 second
//10.1827272392 tiles/second
#define SHELL_SPEED_MISSILE (10.1827272392 * TILE_SIZE / TARGET_FPS)

//10 seconds until detonation
#define MINE_COUNTDOWN (10 * TARGET_FPS + EXPLOSION_ANIM)
//2 seconds spent pulsing
#define MINE_WARNING (2 * TARGET_FPS + EXPLOSION_ANIM)
//TODO: better data for this
//time after a enemy enters the range of a mine
#define MINE_TRIGGERED (TARGET_FPS * 2 / 5 + EXPLOSION_ANIM)
//2/15ths of a second per pulse
#define PULSE_TIME ((uint8_t)(2 * TARGET_FPS / 15))

//Amount of time the explosion takes
//1/2 second in the original, may reduce to save sprite size
#define EXPLOSION_ANIM (TARGET_FPS / 2)

//Player action cooldown
#define SHOT_COOLDOWN 5
#define MINE_COOLDOWN 10

//TODO: Radius in pixels that enemy tanks will cause mines to explode
#define MINE_DETECT_RANGE (2 * TILE_SIZE)

//120 pixels / 48 px/tile = 2.5 tiles
#define MINE_EXPLOSION_RADIUS (2.5 * TILE_SIZE)

#define PLAYER_BARREL_ROTATION DEGREES_TO_ANGLE(5)
//1/3 of a second for 90 degree rotation
#define PLAYER_TREAD_ROTATION (DEGREES_TO_ANGLE(90) / (TARGET_FPS / 3))

//amount of time in milliseconds the mission start screen displays
#define MISSION_START_TIME 3000
//Font size
#define MISSION_NUMBER_TEXT 3
#define ENEMY_TANK_TEXT 2

// Game status
enum {
	IN_PROGRESS,
	QUIT,
	NEXT_LEVEL,
	WIN,
	LOSE
};

#endif /* H_CONSTANTS */
