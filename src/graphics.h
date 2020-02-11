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
#include "objects.h"
#include "level.h"
#include "gfx/tiles_gfx.h"

/* Calculate the screen-space position for a given world-space point */
#define SCREEN_X(x) ((x) / PIXEL_SCALE + MAP_OFFSET_X)
#define SCREEN_Y(y) ((y) / PIXEL_SCALE)

/* Calculate the screen-space distance for a given world-space distance */
#define SCREEN_DELTA_X(x) ((x) / PIXEL_SCALE)
#define SCREEN_DELTA_Y(y) ((y) / PIXEL_SCALE)

void displayScores(void); //Display high scores

void missionStart(uint8_t mission, uint8_t lives, uint8_t num_tanks); //Display the mission start screen

void render(Level *level); //Render tilemap, tanks, and UI during the game loop

void renderPhysicsBody(PhysicsBody *phys);

void displayUI(void); //Display UI during a mission

void draw_line(LineSeg* ls);

#endif