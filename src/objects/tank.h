#ifndef TANKS_TANK_H
#define TANKS_TANK_H

#include "../fwd.h"

#include <cstdint>

#include "../ai/ai_state.h"
#include "../physics/collision.h"
#include "../level.h"
#include "mine.h"
#include "shell.h"
#include "../util/util.h"

typedef uint8_t tank_type_t;

#define TANK_SIZE TILE_SIZE

//Distance from center of tank new shells appear
#define BARREL_LENGTH (TANK_SIZE * 5 / 14)

//148 px / 1 s * 1 tile / 48 px = 3.08 tiles / sec
#define TANK_SPEED_SLOW (2 * TILE_SIZE / TARGET_TICK_RATE) // todo
#define TANK_SPEED_NORMAL (3.08 * TILE_SIZE / TARGET_TICK_RATE)
#define TANK_SPEED_HIGH (4 * TILE_SIZE / TARGET_TICK_RATE) // todo
#define TANK_SPEED_BLACK (5 * TILE_SIZE / TARGET_TICK_RATE) // todo

#define MAX_NUM_TANKS 16

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

typedef struct {
    //A tank as stored in the level file
    tank_type_t type;
    uint8_t start_x; //Tile the tank starts on
    uint8_t start_y;
} serialized_tank_t;

class Tank: public PhysicsBody {
public:
    Tank(const serialized_tank_t *ser_tank, uint8_t id);
    ~Tank();

    tank_type_t type;
    uint8_t id;
    uint8_t start_x;
    uint8_t start_y;
    angle_t tread_rot; //Rotation of tank treads. Determines the direction of the tank.
    angle_t barrel_rot; //Rotation of the barrel. Determines the direction shots are fired in
    uint8_t num_shells = 0;
    uint8_t num_mines = 0;
    ai_move_state_t ai_move;
    ai_fire_state_t ai_fire;

    // Kill this physics body, then destroy it
    void kill();
    void process();
    void render(uint8_t layer);
    void fire_shell();
    void lay_mine();
    bool can_shoot() const;
    bool can_lay_mine() const;
    void set_velocity(int velocity);

    //Number of shots each type of tank can have on-screen at any one time
    static const uint8_t max_shells[];
    //Number of times a shell from a certain tank type can bounce off a wall
    static const uint8_t max_bounces[];
    //Number of mines each type of tank can have on-screen at any one time
    static const uint8_t max_mines[];
    static const uint8_t velocities[];

    void handle_collision(PhysicsBody *other);
    void collide(Tank *tank);
    void collide(Shell *shell);
    void collide(Mine *mine);
    void collide(MineDetector *detector);
};

#endif //TANKS_TANK_H
