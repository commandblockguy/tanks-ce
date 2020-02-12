#ifndef TANKS_SHELL_H
#define TANKS_SHELL_H

#include <stdbool.h>
#include "collision.h"

typedef struct {
    bool alive; //Whether this shell is processed
    physicsBody_t phys;
    uint8_t bounces; //Number of times the shell can bounce off a wall without exploding
    bool left_tank_hitbox; //Whether the shell has exited the tank hitbox yet. Used to stop shells from blowing up the tank that fired them.
} shell_t;

//void processShell(shell_t* shell, tank_t* tank);

//Bounce a shell off a wall
//Returns whether or not the shell is still alive
bool shellRicochet(shell_t* shell, direction_t dir);

#endif //TANKS_SHELL_H
