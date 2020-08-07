#ifndef TANKS_TANK_H
#define TANKS_TANK_H

typedef uint8_t tank_type_t;

#include <stdbool.h>
#include <stdint.h>
#include "collision.h"
#include "graphics.h"
#include "level.h"
#include "mine.h"
#include "shell.h"
#include "util.h"

#define TANK_SIZE TILE_SIZE

//Distance from center of tank new shells appear
#define BARREL_LENGTH (TANK_SIZE * 5 / 14)

//148 px / 1 s * 1 tile / 48 px = 3.08 tiles / sec
#define TANK_SPEED_SLOW (2 * TILE_SIZE / TARGET_FPS) //TODO
#define TANK_SPEED_NORMAL (3.08 * TILE_SIZE / TARGET_FPS)
#define TANK_SPEED_BLACK 5 //TODO

enum {
    PLAYER = 0, //blue
    IMMOBILE = 1, //brown
    BASIC = 2, //grey
    MISSILE = 3, //turquoise
    MINE = 4, //yellow
    RED = 5, IMMOB_MISSILE = 6, //green
    FAST = 7, //purple
    INVISIBLE = 8, //white
    BLACK = 9, NUM_TANK_TYPES = 10
};

typedef struct {
    //A tank, used while gameplay occurs
    tank_type_t type;
    bool alive; //Whether this tank is alive or exploded.
    uint8_t start_x;
    uint8_t start_y;
    physics_body_t phys;
    angle_t tread_rot; //Rotation of tank treads. Determines the direction of the tank.
    angle_t barrel_rot; //Rotation of the barrel. Determines the direction shots are fired in
    shell_t shells[5]; //Shells that belong to this tank. Players can shoot up to 5, and each type of tank is limited to a different number.
    mine_t mines[4]; //Mines that belong to this tank. Players and some tanks can lay up to two.
    union ai_move *ai_move;
    union ai_fire *ai_fire;
} tank_t;

typedef struct {
    //A tank as stored in the level file
    tank_type_t type;
    uint8_t start_x; //Tile the tank starts on
    uint8_t start_y;
} serialized_tank_t;

void deserialize_tank(tank_t *tank, const serialized_tank_t *ser_tank); //Convert a serialized tank into an actual one

void process_tank(tank_t *tank);

void process_mine(mine_t *mine, tank_t *tank);

void process_shell(shell_t *shell, tank_t *tank);

bool fire_shell(tank_t *tank); //PEW PEW PEW

bool lay_mine(tank_t *tank); //Lay a mine under the tank

//true if a tank has a slot for a shell
bool can_shoot(tank_t *tank);

void set_velocity(tank_t *tank, int24_t velocity);

//Number of shots each type of tank can have on-screen at any one time
extern const uint8_t max_shells[];
//Number of times a shell from a certain tank type can bounce off a wall
extern const uint8_t max_bounces[];
//Number of mines each type of tank can have on-screen at any one time
extern const uint8_t max_mines[];

#endif //TANKS_TANK_H
