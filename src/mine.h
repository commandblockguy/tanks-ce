#ifndef TANKS_MINE_H
#define TANKS_MINE_H

#include <stdbool.h>
#include <intce.h>
#include "collision.h"

typedef struct {
    bool alive; //Whether this mine is processed
    physicsBody_t phys;
    uint24_t countdown; //Number of physics loops until explosions occur
} mine_t;

void detonate(mine_t *mine);

//void processMine(mine_t *mine, tank_t *tank);

#endif //TANKS_MINE_H
