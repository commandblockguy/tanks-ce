#ifndef TANKS_PHYSICS_H
#define TANKS_PHYSICS_H

#include <stdbool.h>
#include <stdint.h>
#include <TINYSTL/vector.h>
#include "collision.h"
#include "fwd.h"

// Tick / Frame rate
#define TARGET_TICK_RATE 30

enum {
    NONE = 0, UP = 1, DOWN = 2, LEFT = 4, RIGHT = 8
};
typedef uint8_t direction_t;

#define MAX_OBJECTS 255

class PhysicsBody {
public:
    PhysicsBody();
    virtual ~PhysicsBody();

    int position_x;
    int position_y;
    int velocity_x;
    int velocity_y;
    uint width;
    uint height;
    PhysicsBody *parent;

    // Whether or not to collide with holes
    bool respect_holes;

    // An array of physics objects, sorted from least to greatest Y position
    static tinystl::vector<PhysicsBody*> objects;

    uint center_x() const;
    uint center_y() const;

    bool detect_collision(PhysicsBody *other) const;
    bool is_point_inside(int x, int y) const;
    direction_t process_reflection();
    bool center_distance_less_than(PhysicsBody *other, uint dis) const;
    bool collides_line(line_seg_t *seg) const;

    static void sort();

    virtual void kill();
    virtual void process() = 0;
    virtual void render();

    // Polymorphic ping-pong (aka "visitor pattern," apparently)
    virtual void handle_collision(PhysicsBody *other) = 0;
    virtual void collide(Tank *tank) = 0;
    virtual void collide(Shell *shell) = 0;
    virtual void collide(Mine *mine) = 0;
};

#endif //TANKS_PHYSICS_H
