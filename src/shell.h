#ifndef TANKS_SHELL_H
#define TANKS_SHELL_H

#include <stdbool.h>
#include "collision.h"
#include "physics.h"
#include "fwd.h"

#define SHELL_SIZE (TILE_SIZE * 3 / 14)

//Speed of shells in pixels per tick
//Standard:
//434 px X / 48 px/tile = 11.4210526316 tiles
//157 px Y / 32 px/tile = 4.90625 tiles
//d = 12.4302748271 tile
//123 frames=2.05 seconds
//6.06354869615 tiles/second
#define SHELL_SPEED_STANDARD (6.06354869615 * TILE_SIZE / TARGET_TICK_RATE)
//Fast
//238 px X / 48 px/tile = 4.95833333333 tiles
//37  px Y / 32 px/tile = 1.15625 tiles
//d = 5.09136361959 tiles
//30 frames = 1/2 second
//10.1827272392 tiles/second
#define SHELL_SPEED_MISSILE (10.1827272392 * TILE_SIZE / TARGET_TICK_RATE)

class Shell: public PhysicsBody {
public:
    Shell();
    ~Shell();

    uint8_t bounces; //Number of times the shell can bounce off a wall without exploding
    bool left_tank_hitbox; //Whether the shell has exited the tank hitbox yet. Used to stop shells from blowing up the tank that fired them.
    uint8_t direction;
    Tank *tank;

    void process();
    void render();
    //Bounce a shell off a wall
    //Returns whether or not the shell is still alive
    bool ricochet(direction_t dir);
    void update_direction();

    static uint8_t angle_to_shell_direction(angle_t angle) {
        return ((uint8_t) -((angle >> 16) - 64)) >> 4;
    }

    void handle_collision(PhysicsBody *other);
    void collide(Tank *tank);
    void collide(Shell *shell);
    void collide(Mine *mine);
};

#endif //TANKS_SHELL_H
