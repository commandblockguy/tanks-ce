#ifndef TANKS_PHYSICSBODY_H
#define TANKS_PHYSICSBODY_H

#include <cstdint>
#include <TINYSTL/vector.h>

#include <stdbool.h>

#include "../physics/collision.h"
#include "../fwd.h"
#include "../physics/physics.h"

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
    bool active;

    bool tile_collisions;
    // Whether or not to collide with holes
    bool respect_holes;

    // An array of physics objects, sorted from least to greatest Y position
    static tinystl::vector<PhysicsBody*> objects;

    uint center_x() const;
    uint center_y() const;

    bool detect_collision(PhysicsBody *other) const;
    bool is_point_inside(int x, int y) const;
    direction_t process_tile_collision();
    bool center_distance_less_than(PhysicsBody *other, uint dis) const;
    bool collides_line(line_seg_t *seg) const;

    void tick();

    static void sort();
    static void remove_inactive();

    virtual void process() = 0;
    virtual void render(uint8_t layer);

    virtual void handle_tile_collision(direction_t dir);
    virtual void handle_explosion();

    // Polymorphic ping-pong (aka "visitor pattern," apparently)
    virtual void handle_collision(PhysicsBody *other) = 0;
    virtual void collide(Tank *tank) = 0;
    virtual void collide(Shell *shell) = 0;
    virtual void collide(Mine *mine) = 0;
    virtual void collide(MineDetector *detector) = 0;
};

#endif //TANKS_PHYSICSBODY_H
