#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#undef NDEBUG
#include <debug.h>

#include "constants.h"
#include "objects.h"
#include "collision.h"
#include "util.h"
#include "graphx.h" //debug reasons
#include "graphics.h" //more debug reasons
#include "ai.h"

void ai_process_move(Tank* tank) {
	switch(tank->type) {
		case(BASIC):
		case(INVISIBLE):
		case(MINE):
			move_random(tank);
			break;
		case(MISSILE):
			move_away(tank);
			break;
		case(RED):
		case(FAST):
		case(BLACK):
			move_toward(tank);
	}
}

void ai_process_fire(Tank* tank) {
	switch(tank->type) {
		case(IMMOBILE):
			aim_random(tank);
			break;
		case(BASIC):
		case(MINE):
		case(RED):
			aim_reflect(tank);
			break;
		case(MISSILE):
			aim_current(tank);
			break;
		case(IMMOB_MISSILE):
		case(FAST):
		case(INVISIBLE):
		case(BLACK):
			aim_future(tank);
	}
}

void move_random(Tank* tank) {

}

void move_away(Tank* tank) {

}

void move_toward(Tank* tank) {

}

//Spin randomly rotation speed:
//Angle 1: 10.78 degrees
//Angle 2: 36.25 degrees
//delta: 25.47 degrees = 28.5 * 2^16 rot units
//t: 1 s
//Basically 1 << 16 rot unit per frame.
void aim_random(Tank* tank) {
	int8_t i = 0;
	if(!randInt(0, TARGET_FPS - 1)) tank->ai_fire->random.clockwise = !tank->ai_fire->random.clockwise;
	if(tank->ai_fire->random.clockwise) {
		tank->barrel_rot += 0x010000;
	} else {
		tank->barrel_rot -= 0x010000;
	}
	if(!canShoot(tank)) return;
	if(pointingAtTarget(tank, &tanks[0].phys, 1, false)) {
		fire_shell(tank);
	}
}

//todo: add some visualizations as I have absolutely no idea wtf is going on here
//it worked well in my head, okay?
void aim_reflect(Tank* tank) {
	struct ai_fire_reflect* ai = &tank->ai_fire->reflect;
	if(!canShoot(tank)) return;
	//Loop through all X values, then all Y values
	if(ai->scan_dir == 0) {
		uint24_t distance, newPos;
		//Reflect off of x values
		uint8_t x, xT, yT;
		uint24_t rX;
		uint24_t yInt;
		bool left;
		LineSeg line;
		tile_t tile;

		if(ai->scan_pos > LEVEL_SIZE_X) {
			ai->scan_pos = 0;
			ai->scan_dir = 1;
			pointAtPlayer(tank, &tanks[0].phys);
			if(pointingAtTarget(tank, &tanks[0].phys, max_bounces[tank->type], false)) {
				fire_shell(tank);
			}
			return; //I know this kinda skips a tick but whatever
		}
		x = ai->scan_pos++;
		rX = tileToXPt(x);
		left = center_x(&tank->phys) < rX;
		//if tank and the target aren't both on the same side of the X line, do nothing
		if(left != (center_x(&tanks[0].phys) < rX)) return;
		//if the specified X line was a mirror, where would the target appear to be?
		//lineseg between it and the center of tank
		line.x1 = center_x(&tank->phys);
		line.y1 = center_y(&tank->phys);
		line.x2 = 2 * rX - tanks[0].phys.position_x - tanks[0].phys.width / 2;
		line.y2 = center_y(&tanks[0].phys);
		//check if there is a tile where that lineseg intercepts the X line
		yInt = y_intercept(&line, rX);
		xT = x - !left;
		yT = ptToYTile(yInt);
		tile = tiles[xT + LEVEL_SIZE_X * yT];
		#ifdef DBG_DRAW
		gfx_SetColor(COL_RED);
		draw_line(&line);
		gfx_SetColor(COL_BLACK);
		gfx_VertLine(rX, 0, LCD_HEIGHT);
		gfx_HorizLine(0, yInt, LCD_WIDTH);
		#endif

		if(xT != 0 && xT < LEVEL_SIZE_X && yT !=0 && yT < LEVEL_SIZE_Y)
			if(!tileHeight(tile) || tileType(tile) == DESTROYED) return;
		//if so, check if pointingAtTarget
		tank->barrel_rot = fast_atan2(line.y2 - line.y1, line.x2 - line.x1);
		if(pointingAtTarget(tank, &tanks[0].phys, max_bounces[tank->type], false)) {
			//if so, fire
			fire_shell(tank);
		}
	} else {
		uint24_t distance, newPos;
		//Reflect off of y values
		uint8_t y, xT, yT;
		uint24_t rY;
		int24_t xInt;
		bool up;
		LineSeg line;
		tile_t tile;

		if(ai->scan_pos > LEVEL_SIZE_Y) {
			ai->scan_pos = 0;
			ai->scan_dir = 0;
			pointAtPlayer(tank, &tanks[0].phys);
			if(pointingAtTarget(tank, &tanks[0].phys, max_bounces[tank->type], false)) {
				fire_shell(tank);
			}
			return; //I know this kinda skips a tick but whatever
		}
		y = ai->scan_pos++;
		rY = tileToYPt(y);
		up = center_y(&tank->phys) < rY;
		//if tank and the target aren't both on the same side of the Y line, do nothing
		if(up != (center_y(&tanks[0].phys) < rY)) return;
		//if the specified X line was a mirror, where would the target appear to be?
		//lineseg between it and the center of tank
		line.x1 = center_x(&tank->phys);
		line.y1 = center_y(&tank->phys);
		line.x2 = center_x(&tanks[0].phys);
		line.y2 = 2 * rY - tanks[0].phys.position_y - tanks[0].phys.height / 2;
		//check if there is a tile where that lineseg intercepts the X line
		xInt = x_intercept(&line, rY);
		xT = ptToXTile(xInt);
		yT = y - !up;
		tile = tiles[xT + LEVEL_SIZE_X * yT];
		#ifdef DBG_DRAW
		gfx_SetColor(COL_RED);
		draw_line(&line);
		gfx_SetColor(COL_BLACK);
		gfx_HorizLine(0, rY, LCD_WIDTH);
		gfx_VertLine(xInt, 0, LCD_HEIGHT);
		#endif
		if(xT != 0 && xT < LEVEL_SIZE_X && yT !=0 && yT < LEVEL_SIZE_Y)
			if(!tileHeight(tile) || tileType(tile) == DESTROYED) return;
		//if so, check if pointingAtTarget
		tank->barrel_rot = fast_atan2(line.y2 - line.y1, line.x2 - line.x1);
		if(pointingAtTarget(tank, &tanks[0].phys, max_bounces[tank->type], false)) {
			//if so, fire
			fire_shell(tank);
		}
	}
}

//Aim with no reflections
void aim_current(Tank* tank) {
	if(canShoot(tank)) {
		pointAtPlayer(tank, &tanks[0].phys);
		if(pointingAtTarget(tank, &tanks[0].phys, 0, false)) {
			fire_shell(tank);
		}
	}
}

void aim_future(Tank* tank) {

}

//credit: https://theshoemaker.de/2016/02/ray-casting-in-2d-grids/
//though I've rewritten a lot of it
//returns 0 if hits across x axis, non-zero if y axis
bool raycast(uint24_t startX, uint24_t startY, angle_t angle, LineSeg* result) {
	int24_t dirX = fast_cos(angle) / 32;
	int24_t dirY = fast_sin(angle) / 32;

	int8_t dirSignX = dirX >= 0 ? 1 : -1;
	int8_t dirSignY = dirY >= 0 ? 1 : -1;

	int8_t tileX = ptToXTile(startX);
	int8_t tileY = ptToYTile(startY);
	int24_t t = 0;

	int24_t dtX = (int24_t)(tileToXPt(tileX + (dirX >= 0 ? 1 : 0)) - startX) / dirX;
	int24_t dtY = (int24_t)(tileToYPt(tileY + (dirY >= 0 ? 1 : 0)) - startY) / dirY;

	int24_t dtXr = dirSignX * TILE_SIZE / dirX;
	int24_t dtYr = dirSignY * TILE_SIZE / dirY;

	//dbg_sprintf(dbgout, "dtXr = %i, dtYr = %i\n", dtXr, dtYr);

	//while inside the map
	while(tileX >= 0 && tileX < LEVEL_SIZE_X && tileY >= 0 && tileY < LEVEL_SIZE_Y) {
		tile_t tile = tiles[tileX + LEVEL_SIZE_X * tileY];

		//dbg_sprintf(dbgout, "tileX: %i, tileY: %i, t: %i, dtX: %i, dtY: %i, tile: %X\n", tileX, tileY, t, dtX, dtY, tile);

		if(tileHeight(tile) && tileType(tile) != DESTROYED) {
			break;
		}

		//not entirely sure how this works
		if(dtX < dtY) {
			t += dtX;
			tileX += dirSignX;
			dtY -= dtX;
			dtX = dtXr;
		} else {
			t += dtY;
			tileY += dirSignY;
			dtX -= dtY;
			dtY = dtYr;
		}

	}
	//store into result if it exists
	if(result != NULL) {
		result->x1 = startX;
		result->y1 = startY;
		result->x2 = dirX * t + startX;//curX + dirX * dt;
		result->y2 = dirY * t + startY;//curY + dirY * dt;
	}
	if(angle ==  0 || angle == 128) return 0; //return 0 if angle is horizontal - not sure why
	if(angle == 64 || angle == 192) return 1;
	return dtX != dtXr; //if dtX == dtXr, last movement was X
}

bool pointingAtTarget(Tank* tank, PhysicsBody* target, uint8_t max_bounces, bool future) {
	uint8_t bounces;
	uint24_t posX = center_x(&tank->phys);
	uint24_t posY = center_y(&tank->phys);
	angle_t angle = tank->barrel_rot;
	for(bounces = 0; bounces <= max_bounces; bounces++) {
		bool reflectAxis;
		LineSeg line;
		reflectAxis = raycast(posX, posY, angle, &line);
		#ifdef DBG_DRAW
		gfx_SetColor(COL_LIVES_TXT);
		draw_line(&line);
		#endif
		if(seg_collides_bb(&line, target)) {
		    gfx_BlitBuffer();
		    delay(500);
			return true;
		}
		//move pos to new position and reflect angle
		posX = line.x2;
		posY = line.y2;
		if(!reflectAxis) {
			//reflect X component
			angle = 128 - angle;
		} else {
			//reflect Y component
			angle = 0 - angle;
		}
	}
	return 0;
}

//Point directly at the player with no bounces or motion compensation
void pointAtPlayer(Tank *tank, PhysicsBody *target) {
	int24_t dx = center_x(target) - center_x(&tank->phys);
	int24_t dy = center_y(target) - center_y(&tank->phys);
	tank->barrel_rot = fast_atan2(dy, dx);
}
