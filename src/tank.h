#ifndef TANKS_TANK_H
#define TANKS_TANK_H

#include <stdbool.h>
#include <stdint.h>
#include "util.h"
#include "collision.h"
#include "shell.h"
#include "mine.h"

enum {
    PLAYER = 0, //blue
    IMMOBILE = 1, //brown
    BASIC = 2, //grey
    MISSILE = 3, //turquoise
    MINE = 4, //yellow
    RED = 5,
    IMMOB_MISSILE = 6, //green
    FAST = 7, //purple
    INVISIBLE = 8, //white
    BLACK = 9,
    NUM_TANK_TYPES = 10
};

typedef uint8_t tankType_t;

typedef struct {
    //A tank, used while gameplay occurs
    tankType_t type;
    bool alive; //Whether this tank is alive or exploded.
    uint8_t start_x;
    uint8_t start_y;
    physicsBody_t phys;
    angle_t tread_rot; //Rotation of tank treads. Determines the direction of the tank.
    angle_t barrel_rot; //Rotation of the barrel. Determines the direction shots are fired in
    shell_t shells[5]; //Shells that belong to this tank. Players can shoot up to 5, and each type of tank is limited to a different number.
    mine_t mines[4]; //Mines that belong to this tank. Players and some tanks can lay up to two.
    union ai_move* ai_move;
    union ai_fire* ai_fire;
} tank_t;

void processTank(tank_t* tank);

bool fireShell(tank_t* tank); //PEW PEW PEW

bool layMine(tank_t* tank); //Lay a mine under the tank

//true if a tank has a slot for a shell
bool canShoot(tank_t* tank);

//Number of shots each type of tank can have on-screen at any one time
extern const uint8_t max_shells[];
//Number of times a shell from a certain tank type can bounce off a wall
extern const uint8_t max_bounces[];
//Number of mines each type of tank can have on-screen at any one time
extern const uint8_t max_mines[];

#endif //TANKS_TANK_H
