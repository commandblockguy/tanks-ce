#ifndef TANKS_TANK_H
#define TANKS_TANK_H

#include "../fwd.h"

#include <cstdint>
#include <graphx.h>

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

#define TANK_SPEED_SLOW (2.25 * TILE_SIZE / TARGET_TICK_RATE)
#define TANK_SPEED_NORMAL (2.5 * TILE_SIZE / TARGET_TICK_RATE)
#define TANK_SPEED_HIGH (3 * TILE_SIZE / TARGET_TICK_RATE)
#define TANK_SPEED_BLACK (4 * TILE_SIZE / TARGET_TICK_RATE) // todo

#define SHOT_COOLDOWN 5
#define MINE_COOLDOWN 10

#define TREAD_DISTANCE (TILE_SIZE / 3)

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

struct serialized_tank {
    //A tank as stored in the level file
    tank_type_t type;
    uint8_t start_x; //Tile the tank starts on
    uint8_t start_y;
};

class Tank: public PhysicsBody {
public:
    Tank(const struct serialized_tank *ser_tank, uint8_t id);
    ~Tank();

    tank_type_t type;
    uint8_t id;
    uint8_t start_x;
    uint8_t start_y;
    angle_t tread_rot{0}; //Rotation of tank treads. Determines the direction of the tank.
    angle_t barrel_rot{0}; //Rotation of the barrel. Determines the direction shots are fired in
    uint8_t num_shells{0};
    uint8_t num_mines{0};

    // todo: make private
    union ai_move_state ai_move{};
    union ai_fire_state ai_fire{};

    inline uint8_t max_shells() const { return types[type].max_shells; }
    inline uint8_t max_bounces() const { return types[type].max_bounces; }
    inline uint8_t max_mines() const { return types[type].max_mines; }
    inline uint8_t velocity() const { return types[type].velocity; }

    void kill();
    void process();
    void render(uint8_t layer);
    void fire_shell();
    void lay_mine();
    bool can_shoot() const;
    bool can_lay_mine() const;
    void set_velocity(int velocity);

    void handle_explosion();

    void handle_collision(PhysicsBody *other);
    void collide(Tank *tank);
    void collide(Shell *shell);
    void collide(MineDetector *detector);

private:
    struct type_data {
        uint8_t max_shells;
        uint8_t max_bounces;
        uint8_t max_mines;
        uint8_t velocity;
    };
    static const type_data types[NUM_TANK_TYPES];

    uint8_t shot_cooldown{0};
    uint8_t mine_cooldown{0};
    uint tread_distance{TREAD_DISTANCE};

    // Used to (somewhat lazily) draw stuff on the background across two frame buffers
    bool draw_treads{false};
    gfx_sprite_t *tread_sprite{nullptr};
    gfx_region_t tread_pos{};
};

#endif //TANKS_TANK_H
