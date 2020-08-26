#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include "collision.h"
#include "util.h"
#include "graphics.h"
#include "ai.h"
#include "tank.h"
#include "globals.h"
#include "profiler.h"
#include "level.h"

void ai_process_move(tank_t *tank) {
    profiler_add(ai_move);
    switch(tank->type) {
        default:
            break;
        case (BASIC):
        case (INVISIBLE):
        case (MINE):
            move_random(tank);
            break;
        case (MISSILE):
            move_away(tank);
            break;
        case (RED):
        case (FAST):
        case (BLACK):
            move_toward(tank);
    }
    profiler_end(ai_move);
}

void ai_process_fire(tank_t *tank) {
    profiler_add(ai_aim);
    switch(tank->type) {
        default:
            break;
        case (IMMOBILE):
            profiler_add(ai_aim_random);
            aim_random(tank);
            profiler_end(ai_aim_random);
            break;
        case (BASIC):
        case (MINE):
        case (RED):
            profiler_add(ai_aim_reflect);
            aim_reflect(tank);
            profiler_end(ai_aim_reflect);
            break;
        case (MISSILE):
            aim_current(tank);
            break;
        case (IMMOB_MISSILE):
        case (FAST):
        case (INVISIBLE):
        case (BLACK):
            aim_future(tank);
    }
    profiler_end(ai_aim);
}

void move_random(tank_t *tank) {
    profiler_add(ai_move_random);
    tile_t left1_tile = get_tile_at_offset(tank, DEGREES_TO_ANGLE(-30), 2 * TANK_SIZE);
    tile_t right1_tile = get_tile_at_offset(tank, DEGREES_TO_ANGLE(30), 2 * TANK_SIZE);
    tile_t left2_tile = get_tile_at_offset(tank, DEGREES_TO_ANGLE(-30), TANK_SIZE);
    tile_t right2_tile = get_tile_at_offset(tank, DEGREES_TO_ANGLE(30), TANK_SIZE);
    bool left = TILE_HEIGHT(left1_tile) || TILE_TYPE(left1_tile) == HOLE || TILE_HEIGHT(left2_tile) ||
                TILE_TYPE(left2_tile) == HOLE;
    bool right = TILE_HEIGHT(right1_tile) || TILE_TYPE(right1_tile) == HOLE || TILE_HEIGHT(right2_tile) ||
                 TILE_TYPE(right2_tile) == HOLE;

    if(left && right) {
        tank->tread_rot += DEGREES_TO_ANGLE(180);
    } else {
        if(left) {
            tank->tread_rot += DEGREES_TO_ANGLE(5);
        } else if(right) {
            tank->tread_rot -= DEGREES_TO_ANGLE(5);
        }
    }
    tank->tread_rot += randInt(0, DEGREES_TO_ANGLE(6)) - DEGREES_TO_ANGLE(3);
    // todo: fast/slow tanks
    set_velocity(tank, tank_velocities[tank->type]);
    profiler_end(ai_move_random);
}

void move_away(tank_t *tank) {
    move_random(tank); // todo: fix
}

void move_toward(tank_t *tank) {
    move_random(tank); // todo: fix
}

//Spin randomly rotation speed:
//Angle 1: 10.78 degrees
//Angle 2: 36.25 degrees
//delta: 25.47 degrees = 28.5 * 2^16 rot units
//t: 1 s
//Basically 1 << 16 rot unit per frame.
void aim_random(tank_t *tank) {
    if(!randInt(0, TARGET_FPS - 1)) tank->ai_fire.random.clockwise = !tank->ai_fire.random.clockwise;
    if(tank->ai_fire.random.clockwise) {
        tank->barrel_rot += 0x010000;
    } else {
        tank->barrel_rot -= 0x010000;
    }
    if(!can_shoot(tank)) return;
    if(pointing_at_target(tank, &tanks[0].phys, 1, false)) {
        fire_shell(tank);
    }
}

//todo: add some visualizations as I have absolutely no idea wtf is going on here
//it worked well in my head, okay?
void aim_reflect(tank_t *tank) {
    ai_fire_reflect_state_t *ai = &tank->ai_fire.reflect;
    if(!can_shoot(tank)) return;
    //Loop through all X values, then all Y values
    if(tank->ai_fire.reflect.scan_dir == 0) {
        //Reflect off of x values
        uint8_t x, xT, yT;
        uint24_t rX;
        int24_t yInt;
        bool left;
        line_seg_t line;
        tile_t tile;

        if(ai->scan_pos > LEVEL_SIZE_X - 1) {
            ai->scan_pos = 1;
            ai->scan_dir = 1;
            point_at_player(tank, &tanks[0].phys);
            if(pointing_at_target(tank, &tanks[0].phys, max_bounces[tank->type], false)) {
                fire_shell(tank);
            }
            return; //I know this kinda skips a tick but whatever
        }
        x = ai->scan_pos++;
        rX = TILE_TO_X_COORD(x);
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
        yT = COORD_TO_Y_TILE(yInt);
        tile = tiles[yT][xT];
#ifdef DBG_DRAW
        gfx_SetColor(COL_RED);
        drawLine(&line);
        gfx_SetColor(COL_BLACK);
        gfx_VertLine(rX, 0, LCD_HEIGHT);
        gfx_HorizLine(0, yInt, LCD_WIDTH);
#endif

        if(xT != 0 && xT < LEVEL_SIZE_X && yT != 0 && yT < LEVEL_SIZE_Y)
            if(!TILE_HEIGHT(tile) || TILE_TYPE(tile) == DESTROYED) {
                //printf_("No tile at intersect point (%u, %u) (is %X)\n", xT, yT, tile);
                return;
            }
        //if so, check if pointing_at_target
        tank->barrel_rot = fast_atan2(line.y2 - line.y1, line.x2 - line.x1);
        if(pointing_at_target(tank, &tanks[0].phys, max_bounces[tank->type], false)) {
            //if so, fire
            fire_shell(tank);
        }
    } else {
        //Reflect off of y values
        uint8_t y, xT, yT;
        uint24_t rY;
        int24_t xInt;
        bool up;
        line_seg_t line;
        tile_t tile;

        if(ai->scan_pos > LEVEL_SIZE_Y - 1) {
            ai->scan_pos = 1;
            ai->scan_dir = 0;
            point_at_player(tank, &tanks[0].phys);
            if(pointing_at_target(tank, &tanks[0].phys, max_bounces[tank->type], false)) {
                fire_shell(tank);
            }
            return; //I know this kinda skips a tick but whatever
        }
        y = ai->scan_pos++;
        rY = TILE_TO_Y_COORD(y);
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
        xT = COORD_TO_X_TILE(xInt);
        yT = y - !up;
        tile = tiles[yT][xT];
#ifdef DBG_DRAW
        gfx_SetColor(COL_RED);
        drawLine(&line);
        gfx_SetColor(COL_BLACK);
        gfx_HorizLine(0, rY, LCD_WIDTH);
        gfx_VertLine(xInt, 0, LCD_HEIGHT);
#endif
        if(xT != 0 && xT < LEVEL_SIZE_X && yT != 0 && yT < LEVEL_SIZE_Y)
            if(!TILE_HEIGHT(tile) || TILE_TYPE(tile) == DESTROYED) return;
        //if so, check if pointing_at_target
        tank->barrel_rot = fast_atan2(line.y2 - line.y1, line.x2 - line.x1);
        if(pointing_at_target(tank, &tanks[0].phys, max_bounces[tank->type], false)) {
            //if so, fire
            fire_shell(tank);
        }
    }
}

//Aim with no reflections
void aim_current(tank_t *tank) {
    if(can_shoot(tank)) {
        point_at_player(tank, &tanks[0].phys);
        if(pointing_at_target(tank, &tanks[0].phys, 0, false)) {
            fire_shell(tank);
        }
    }
}

void aim_future(tank_t *tank) {

}

tile_t get_tile_at_offset(tank_t *tank, angle_t angle_offset, int24_t distance) {
    angle_t angle = tank->tread_rot + angle_offset;
    int24_t x = tank->phys.position_x + tank->phys.width / 2 + distance * fast_cos(angle) / TRIG_SCALE;
    int24_t y = tank->phys.position_y + tank->phys.height / 2 + distance * fast_sin(angle) / TRIG_SCALE;
    int8_t tile_x = COORD_TO_X_TILE(x);
    int8_t tile_y = COORD_TO_X_TILE(y);
    if(tile_x < 0 || tile_x >= LEVEL_SIZE_X) return 1;
    if(tile_y < 0 || tile_y >= LEVEL_SIZE_Y) return 1;
    return tiles[COORD_TO_Y_TILE(y)][COORD_TO_X_TILE(x)];
}

// todo: check tank's future position
bool pointing_at_target(tank_t *tank, physics_body_t *target, uint8_t max_bounces, bool future) {
    uint8_t bounces;
    uint24_t posX = center_x(&tank->phys);
    uint24_t posY = center_y(&tank->phys);
    angle_t angle = tank->barrel_rot;
    for(bounces = 0; bounces <= max_bounces; bounces++) {
        bool reflectAxis;
        line_seg_t line;
        profiler_add(raycast);
        reflectAxis = raycast(posX, posY, angle, &line);
        profiler_end(raycast);
#ifdef DBG_DRAW
        gfx_SetColor(COL_LIVES_TXT);
        drawLine(&line);
#endif
        if(seg_collides_body(&line, target)) {
            return true;
        }
        //move pos to new position and reflect angle
        posX = line.x2;
        posY = line.y2;
        if(!reflectAxis) {
            //reflect X component
            angle = DEGREES_TO_ANGLE(180) - angle;
        } else {
            //reflect Y component
            angle = -angle;
        }
    }
    return 0;
}

//Point directly at the player with no bounces or motion compensation
void point_at_player(tank_t *tank, physics_body_t *target) {
    int24_t dx = center_x(target) - center_x(&tank->phys);
    int24_t dy = center_y(target) - center_y(&tank->phys);
    tank->barrel_rot = fast_atan2(dy, dx);
}
