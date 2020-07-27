#ifndef H_GRAPHICS
#define H_GRAPHICS

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "level.h"

enum colors {
    COL_WHITE = 1,
    COL_BLACK = 2,
    COL_RED = 3,
    COL_BG = 4,
    COL_RHOM_1 = 5,
    COL_RHOM_2 = 6,
    COL_RIB_SHADOW = 7,
    COL_GOLD = 8,
    COL_TXT_SHADOW = 9,
    COL_LIVES_TXT = 10,
    COL_OLIVE_BANDS = 11
};

#define TILE_PIXEL_SIZE 13

/* Calculate the screen-space position for a given world-space point */
#define SCREEN_X(x) ((x) * TILE_PIXEL_SIZE / 256 + MAP_OFFSET_X)
#define SCREEN_Y(y) ((y) * TILE_PIXEL_SIZE / 256)

/* Calculate the screen-space distance for a given world-space distance */
#define SCREEN_DELTA_X(x) ((x) * TILE_PIXEL_SIZE / 256)
#define SCREEN_DELTA_Y(y) ((y) * TILE_PIXEL_SIZE / 256)

void displayScores(void); //Display high scores

void displayKillCounts(void);

void missionStartScreen(uint8_t mission, uint8_t lives, uint8_t num_tanks); //Display the mission start screen

void render(level_t *level); //Render tilemap, tanks, and UI during the game loop

void renderPhysicsBody(physicsBody_t *phys);

void displayUI(void); //Display UI during a mission

void drawLine(lineSeg_t* ls);

#endif