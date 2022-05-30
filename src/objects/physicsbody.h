#ifndef TANKS_PHYSICSBODY_H
#define TANKS_PHYSICSBODY_H

#include <cstdint>
#include <vector.h>

#include <stdbool.h>

#include "../physics/collision.h"
#include "../fwd.h"
#include "../physics/physics.h"

class PhysicsBody {
public:
    PhysicsBody(uint width, uint height);
    virtual ~PhysicsBody();

    int position_x;
    int position_y;
    int velocity_x{0};
    int velocity_y{0};
    const uint width;
    const uint height;
    PhysicsBody *parent{nullptr};
    bool active{true};

    bool tile_collisions;
    // Whether or not to collide with holes
    bool respect_holes;

    // An array of physics objects, sorted from least to greatest Y position
    static ezSTL::vector<PhysicsBody*> objects;

    uint center_x() const;
    uint center_y() const;

    bool detect_collision(PhysicsBody *other) const;
    bool is_point_inside(int x, int y) const;
    direction_t process_tile_collision();
    bool center_distance_less_than(PhysicsBody *other, uint dis) const;
    bool collides_line(struct line_seg *seg) const;

    void tick();

    static void sort();
    static void remove_all();
    static void remove_inactive();

    virtual void process() = 0;
    virtual void render(uint8_t layer);

    virtual void handle_tile_collision(direction_t dir);
    virtual void handle_explosion();

    // Polymorphic ping-pong (aka "visitor pattern," apparently)
    virtual void handle_collision(PhysicsBody *other) = 0;
    virtual void collide([[maybe_unused]] Tank *tank);
    virtual void collide([[maybe_unused]] Shell *shell);
    virtual void collide([[maybe_unused]] Mine *mine);
    virtual void collide([[maybe_unused]] MineDetector *detector);
};

#endif //TANKS_PHYSICSBODY_H
