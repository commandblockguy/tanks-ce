#include <keypadc.h>
#include "input.h"
#include "util/profiler.h"
#include "objects/tank.h"
#include "globals.h"
#include "graphics/gui.h"

#define PLAYER_BARREL_ROTATION DEGREES_TO_ANGLE(5)
//1/3 of a second for 90 degree rotation
#define PLAYER_TREAD_ROTATION (DEGREES_TO_ANGLE(90) / (TARGET_TICK_RATE / 3))

void handle_movement() {
    Tank *player = game.player;
    angle_t target_rot;
    uint8_t keys = 0;

    if(kb_IsDown(kb_KeyDown)) keys |= DOWN;
    if(kb_IsDown(kb_KeyLeft)) keys |= LEFT;
    if(kb_IsDown(kb_KeyRight)) keys |= RIGHT;
    if(kb_IsDown(kb_KeyUp)) keys |= UP;

    switch(keys) {
        default:
            player->set_velocity(0);
            return;
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

    int diff = player->tread_rot - target_rot;
    if((uint)abs(diff) > DEGREES_TO_ANGLE(90)) {
        player->tread_rot += DEGREES_TO_ANGLE(180);
        diff = (int) (player->tread_rot - target_rot);
    }
    if(diff < -(int) PLAYER_TREAD_ROTATION) {
        player->tread_rot += PLAYER_TREAD_ROTATION;
    } else if(diff > (int) PLAYER_TREAD_ROTATION) {
        player->tread_rot -= PLAYER_TREAD_ROTATION;
    } else {
        player->tread_rot = target_rot;
    }

    if((uint)abs(diff) <= DEGREES_TO_ANGLE(45)) {
        player->set_velocity(TANK_SPEED_NORMAL);
    } else {
        player->set_velocity(0);
    }
}

uint8_t handle_input() {
    profiler_start(input);
    Tank *player = game.player;

    handle_movement();

    if(kb_IsDown(kb_Key2nd)) {
        player->fire_shell();
    }
    if(kb_IsDown(kb_KeyAlpha)) {
        player->lay_mine();
    }
    if(kb_IsDown(kb_KeyMode)) {
        player->barrel_rot -= PLAYER_BARREL_ROTATION;
    }
    if(kb_IsDown(kb_KeyGraphVar)) {
        player->barrel_rot += PLAYER_BARREL_ROTATION;
    }
    if(kb_IsDown(kb_KeyAdd)) {
        switch(pause_menu()) {
            default:
            case 0:
                break;
            case 1:
                // todo: restart
            case 2:
                return QUIT;
        }
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
