#include <keypadc.h>
#include "input.h"
#include "profiler.h"
#include "tank.h"
#include "globals.h"

//Player action cooldown
#define SHOT_COOLDOWN 5
#define MINE_COOLDOWN 10

#define PLAYER_BARREL_ROTATION DEGREES_TO_ANGLE(5)
//1/3 of a second for 90 degree rotation
#define PLAYER_TREAD_ROTATION (DEGREES_TO_ANGLE(90) / (TARGET_TICK_RATE / 3))

uint8_t handle_input(void) {
    profiler_start(input);
    Tank *player = &tanks[0];
    bool moving = true;
    angle_t target_rot;
    uint8_t keys = 0;

    if(game.shotCooldown) {
        game.shotCooldown--;
    }
    if(game.mineCooldown) {
        game.mineCooldown--;
    }

    kb_Scan();

    if(kb_IsDown(kb_KeyDown)) keys |= DOWN;
    if(kb_IsDown(kb_KeyLeft)) keys |= LEFT;
    if(kb_IsDown(kb_KeyRight)) keys |= RIGHT;
    if(kb_IsDown(kb_KeyUp)) keys |= UP;

    switch(keys) {
        default:
            moving = false;
            break;
        case UP:
            target_rot = DEGREES_TO_ANGLE(270);
            break;
        case DOWN:
            target_rot = DEGREES_TO_ANGLE(90);
            break;
        case LEFT:
            target_rot = DEGREES_TO_ANGLE(180);
            break;
        case RIGHT:
            target_rot = DEGREES_TO_ANGLE(0);
            break;
        case UP | RIGHT:
            target_rot = DEGREES_TO_ANGLE(315);
            break;
        case DOWN | RIGHT:
            target_rot = DEGREES_TO_ANGLE(45);
            break;
        case UP | LEFT:
            target_rot = DEGREES_TO_ANGLE(225);
            break;
        case DOWN | LEFT:
            target_rot = DEGREES_TO_ANGLE(135);
    }

    if(moving) {
        int24_t diff = player->tread_rot - target_rot;
        if((uint24_t)abs(diff) > DEGREES_TO_ANGLE(90)) {
            player->tread_rot += DEGREES_TO_ANGLE(180);
            diff = (int24_t) (player->tread_rot - target_rot);
        }
        if(diff < -(int24_t) PLAYER_TREAD_ROTATION) {
            player->tread_rot += PLAYER_TREAD_ROTATION;
        } else if(diff > (int24_t) PLAYER_TREAD_ROTATION) {
            player->tread_rot -= PLAYER_TREAD_ROTATION;
        } else {
            player->tread_rot = target_rot;
        }

        if((uint24_t)abs(diff) <= DEGREES_TO_ANGLE(45)) {
            player->set_velocity(TANK_SPEED_NORMAL);
        } else {
            player->set_velocity(0);
        }
    } else {
        player->set_velocity(0);
    }

    if(kb_IsDown(kb_Key2nd) && !game.shotCooldown) {
        player->fire_shell();
        game.shotCooldown = SHOT_COOLDOWN;
    }
    if(kb_IsDown(kb_KeyAlpha) && !game.mineCooldown) {
        player->lay_mine();
        game.mineCooldown = MINE_COOLDOWN;
    }
    if(kb_IsDown(kb_KeyMode)) {
        player->barrel_rot -= PLAYER_BARREL_ROTATION;
    }
    if(kb_IsDown(kb_KeyGraphVar)) {
        player->barrel_rot += PLAYER_BARREL_ROTATION;
    }
    if(kb_IsDown(kb_KeyDel)) { // TODO: remove
        return NEXT_LEVEL;
    }
    if(kb_IsDown(kb_KeyClear)) {
        return QUIT;
    }
    if(kb_IsDown(kb_KeyYequ)) {
        profiler_print();
    }
    profiler_end(input);
    return 0;
}
