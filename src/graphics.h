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
//Hopefully we can remove this when we remove AABB rendering
//Hopefully we can now stop referring to myself in the plural
#include "collision.h"
#include "level.h"
#include "gfx/tiles_gfx.h"

void displayScores(void); //Display high scores

void missionStart(uint8_t mission, uint8_t lives, uint8_t num_tanks); //Display the mission start screen

void render(uint8_t* tiles, Level* level, Tank* tanks); //Render tilemap, tanks, and UI during the game loop

void renderAABB(AABB bb);

void displayUI(void); //Display UI during a mission

void draw_line(LineSeg* ls);

#endif