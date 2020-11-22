#ifndef TANKS_PHYSICS_H
#define TANKS_PHYSICS_H

#include <stdbool.h>
#include <stdint.h>
#include "collision.h"

// Tick / Frame rate
#define TARGET_TICK_RATE 30

enum {
    NONE = 0, UP = 1, DOWN = 2, LEFT = 4, RIGHT = 8
};
typedef uint8_t direction_t;

#define MAX_OBJECTS 255

class PhysicsBody {
public:
    int24_t position_x;
    int24_t position_y;
    int24_t velocity_x;
    int24_t velocity_y;
    uint24_t width;
    uint24_t height;

    // Whether or not to collide with holes
    bool respect_holes;

    // An array of physics objects, sorted from least to greatest Y position
    static PhysicsBody *objects[MAX_OBJECTS];
    static uint8_t num_objects;

    uint24_t center_x() const;
    uint24_t center_y() const;

    bool detect_collision(PhysicsBody *other) const;
    bool is_point_inside(int24_t x, int24_t y) const;
    direction_t process_reflection();
    bool center_distance_less_than(PhysicsBody *other, uint24_t dis) const;
    bool collides_line(line_seg_t *seg) const;

    bool add();
    void remove();
    static void sort();

    virtual void process() = 0;
    virtual void render();
};

#endif //TANKS_PHYSICS_H
