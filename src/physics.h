#ifndef TANKS_PHYSICS_H
#define TANKS_PHYSICS_H

#include <stdbool.h>
#include <stdint.h>

// Tick / Frame rate
#define TARGET_TICK_RATE 30

enum {
    NONE = 0, UP = 1, DOWN = 2, LEFT = 4, RIGHT = 8
};
typedef uint8_t direction_t;

enum physics_types {
    PHYS_TANK,
    PHYS_SHELL,
    PHYS_MINE,
    PHYS_MINE_DETECTOR
};

typedef struct {
    uint8_t type;
    int24_t position_x;
    int24_t position_y;
    int24_t velocity_x;
    int24_t velocity_y;
    uint24_t width;
    uint24_t height;
} physics_body_t;

#define MAX_OBJECTS 255

// An array of physics objects, sorted from least to greatest Y position
extern physics_body_t *objects[MAX_OBJECTS];
extern uint8_t num_objects;

inline uint24_t center_x(const physics_body_t *p) { return p->position_x + p->width / 2; }
inline uint24_t center_y(const physics_body_t *p) { return p->position_y + p->height / 2; }

bool add_object(physics_body_t *phys);
void remove_object(physics_body_t *phys);
void sort_objects(void);

#endif //TANKS_PHYSICS_H
