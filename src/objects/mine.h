#ifndef TANKS_MINE_H
#define TANKS_MINE_H

class Mine;

#include <stdbool.h>
#include <cstdint>
#include "../physics/collision.h"
#include "physicsbody.h"
#include "../fwd.h"

#define MINE_SIZE TILE_SIZE

//10 seconds until detonation
#define MINE_COUNTDOWN (10 * TARGET_TICK_RATE + EXPLOSION_ANIM)
//2 seconds spent pulsing
#define MINE_WARNING (2 * TARGET_TICK_RATE + EXPLOSION_ANIM)
//TODO: better data for this
//time after a enemy enters the range of a mine
#define MINE_TRIGGERED (TARGET_TICK_RATE * 2 / 5 + EXPLOSION_ANIM)

//Amount of time the explosion takes
//1/2 second in the original, may reduce to save sprite size
#define EXPLOSION_ANIM (TARGET_TICK_RATE / 2)

//TODO:
#define MINE_DETECT_RANGE (2 * TILE_SIZE)

//120 pixels / 48 px/tile = 2.5 tiles
#define MINE_EXPLOSION_RADIUS (2.5 * TILE_SIZE)

class Mine: public PhysicsBody {
public:
    Mine(Tank *tank);
    ~Mine();

    
    uint countdown; //Number of physics loops until explosions occur

    void kill();
    void process();
    void render(uint8_t layer);

    void handle_collision(PhysicsBody *other);
    void collide(Tank *tank);
    void collide(Shell *shell);
    void collide(Mine *mine);
};

#endif //TANKS_MINE_H
