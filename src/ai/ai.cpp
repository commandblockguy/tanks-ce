#include "ai.h"

#include <sys/util.h>

#include "../util/profiler.h"
#include "../game.h"

void ai_process_move(Tank *tank) {
    profiler_add(ai_move);
    switch(tank->type) {
        case (PLAYER):
            break;
        case (IMMOBILE):
        case (IMMOB_MISSILE):
        default:
            tank->velocity_x = 0;
            tank->velocity_y = 0;
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

void ai_process_fire(Tank *tank) {
    profiler_add(ai_aim);
    switch(tank->type) {
        default:
            break;
        case (IMMOBILE):
        case (IMMOB_MISSILE):
            profiler_add(ai_aim_random);
            aim_random(tank);
            profiler_end(ai_aim_random);
            break;
        case (BASIC):
        case (MINE):
        case (RED):
        case (FAST):
        case (INVISIBLE):
        case (BLACK):
            profiler_add(ai_aim_reflect);
            aim_reflect(tank);
            profiler_end(ai_aim_reflect);
            break;
        case (MISSILE):
            aim_current(tank);
            break;
// todo:
//        case (IMMOB_MISSILE):
//        case (FAST):
//        case (INVISIBLE):
//        case (BLACK):
//            aim_future(tank);
    }
    profiler_end(ai_aim);
}

void move_random(Tank *tank) {
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
    tank->set_velocity(tank->velocity());
    profiler_end(ai_move_random);
}

void move_away(Tank *tank) {
    move_random(tank); // todo: fix
}

void move_toward(Tank *tank) {
    move_random(tank); // todo: fix
}

//Spin randomly rotation speed:
//Angle 1: 10.78 degrees
//Angle 2: 36.25 degrees
//delta: 25.47 degrees = 28.5 * 2^16 rot units
//t: 1 s
//Basically 1 << 16 rot unit per frame.
void aim_random(Tank *tank) {
    if(!randInt(0, 31)) tank->ai_fire.random.clockwise = !tank->ai_fire.random.clockwise;
    if(tank->ai_fire.random.clockwise) {
        tank->barrel_rot += 0x010000;
    } else {
        tank->barrel_rot -= 0x010000;
    }
    if(!tank->can_shoot()) return;
    if(pointing_at_target(tank, game.player, 1, false)) {
        tank->fire_shell();
    }
}

//todo: add some visualizations as I have absolutely no idea wtf is going on here
//it worked well in my head, okay?
void aim_reflect(Tank *tank) {
    struct ai_fire_reflect_state *ai = &tank->ai_fire.reflect;
    if(!tank->can_shoot()) return;
    //Loop through all X values, then all Y values
    if(tank->ai_fire.reflect.scan_dir == AXIS_X) {
        //Reflect off of x values
        if(ai->scan_pos > LEVEL_SIZE_X - 1) {
            ai->scan_pos = 1;
            ai->scan_dir = AXIS_Y;
            point_at_player(tank, game.player);
            if(pointing_at_target(tank, game.player, tank->max_bounces(), false)) {
                tank->fire_shell();
            }
            return; //I know this kinda skips a tick but whatever
        }
        uint8_t x = ai->scan_pos++;
        uint rX = TILE_TO_X_COORD(x);
        bool left = tank->center_x() < rX;
        //if tank and the target aren't both on the same side of the X line, do nothing
        if(left != (game.player->center_x() < rX)) return;
        //if the specified X line was a mirror, where would the target appear to be?
        //lineseg between it and the center of tank
        struct line_seg line;
        line.x1 = tank->center_x();
        line.y1 = tank->center_y();
        line.x2 = 2 * rX - game.player->position_x - game.player->width / 2;
        line.y2 = game.player->center_y();
        //check if there is a tile where that lineseg intercepts the X line
        int yInt = y_intercept(&line, rX);
        uint8_t xT = x - !left;
        uint8_t yT = COORD_TO_Y_TILE(yInt);
        tile_t tile = game.tiles[yT][xT];
#ifdef DBG_DRAW
        gfx_SetColor(COL_RED);
        drawLine(&line);
        gfx_SetColor(COL_BLACK);
        gfx_VertLine(rX, 0, LCD_HEIGHT);
        gfx_HorizLine(0, yInt, LCD_WIDTH);
#endif

        if(xT != 0 && xT < LEVEL_SIZE_X && yT != 0 && yT < LEVEL_SIZE_Y)
            if(!TILE_HEIGHT(tile)) {
                //dbg_printf("No tile at intersect point (%u, %u) (is %X)\n", xT, yT, tile);
                return;
            }
        //if so, check if pointing_at_target
        tank->barrel_rot = fast_atan2(line.y2 - line.y1, line.x2 - line.x1);
        if(pointing_at_target(tank, game.player, tank->max_bounces(), false)) {
            //if so, fire
            tank->fire_shell();
        }
    } else {
        //Reflect off of y values
        if(ai->scan_pos > LEVEL_SIZE_Y - 1) {
            ai->scan_pos = 1;
            ai->scan_dir = AXIS_X;
            point_at_player(tank, game.player);
            if(pointing_at_target(tank, game.player, tank->max_bounces(), false)) {
                tank->fire_shell();
            }
            return; //I know this kinda skips a tick but whatever
        }
        uint8_t y = ai->scan_pos++;
        uint rY = TILE_TO_Y_COORD(y);
        bool up = tank->center_y() < rY;
        //if tank and the target aren't both on the same side of the Y line, do nothing
        if(up != (game.player->center_y() < rY)) return;
        //if the specified X line was a mirror, where would the target appear to be?
        //lineseg between it and the center of tank
        struct line_seg line;
        line.x1 = tank->center_x();
        line.y1 = tank->center_y();
        line.x2 = game.player->center_x();
        line.y2 = 2 * rY - game.player->position_y - game.player->height / 2;
        //check if there is a tile where that lineseg intercepts the X line
        int xInt = x_intercept(&line, rY);
        uint8_t xT = COORD_TO_X_TILE(xInt);
        uint8_t yT = y - !up;
        tile_t tile = game.tiles[yT][xT];
#ifdef DBG_DRAW
        gfx_SetColor(COL_RED);
        drawLine(&line);
        gfx_SetColor(COL_BLACK);
        gfx_HorizLine(0, rY, LCD_WIDTH);
        gfx_VertLine(xInt, 0, LCD_HEIGHT);
#endif
        if(xT != 0 && xT < LEVEL_SIZE_X && yT != 0 && yT < LEVEL_SIZE_Y)
            if(!TILE_HEIGHT(tile)) return;
        //if so, check if pointing_at_target
        tank->barrel_rot = fast_atan2(line.y2 - line.y1, line.x2 - line.x1);
        if(pointing_at_target(tank, game.player, tank->max_bounces(), false)) {
            //if so, fire
            tank->fire_shell();
        }
    }
}

//Aim with no reflections
void aim_current(Tank *tank) {
    if(tank->can_shoot()) {
        point_at_player(tank, game.player);
        if(pointing_at_target(tank, game.player, 0, false)) {
            tank->fire_shell();
        }
    }
}

void aim_future(__attribute__((unused)) Tank *tank) {

}

tile_t get_tile_at_offset(Tank *tank, angle_t angle_offset, int distance) {
    angle_t angle = tank->tread_rot + angle_offset;
    int x = tank->position_x + tank->width / 2 + distance * fast_cos(angle) / TRIG_SCALE;
    int y = tank->position_y + tank->height / 2 + distance * fast_sin(angle) / TRIG_SCALE;
    int8_t tile_x = COORD_TO_X_TILE(x);
    int8_t tile_y = COORD_TO_X_TILE(y);
    if(tile_x < 0 || tile_x >= LEVEL_SIZE_X) return 1;
    if(tile_y < 0 || tile_y >= LEVEL_SIZE_Y) return 1;
    return game.tiles[COORD_TO_Y_TILE(y)][COORD_TO_X_TILE(x)];
}

// todo: check tank's future position
bool pointing_at_target(Tank *tank, PhysicsBody *target, uint8_t max_bounces, __attribute__((unused)) bool future) {
    uint posX = tank->center_x();
    uint posY = tank->center_y();
    angle_t angle = tank->barrel_rot;
    for(uint8_t bounces = 0; bounces <= max_bounces; bounces++) {
        bool reflectAxis;
        struct line_seg line;
        profiler_add(raycast);
        reflectAxis = raycast(posX, posY, angle, game.tiles, &line);
        profiler_end(raycast);
#ifdef DBG_DRAW
        gfx_SetColor(COL_LIVES_TXT);
        drawLine(&line);
#endif
        if(target->collides_line(&line)) {
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
    return false;
}

//Point directly at the player with no bounces or motion compensation
void point_at_player(Tank *tank, PhysicsBody *target) {
    int dx = target->center_x() - tank->center_x();
    int dy = target->center_y() - tank->center_y();
    tank->barrel_rot = fast_atan2(dy, dx);
}
