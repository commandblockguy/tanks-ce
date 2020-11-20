#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "physics.h"

physics_body_t *objects[MAX_OBJECTS];
uint8_t num_objects = 0;

bool add_object(physics_body_t *phys) {
    if(num_objects == 255) return false;
    for(uint8_t i = 0; i < num_objects; i++) {
        if(phys->position_y > objects[i]->position_y) {
            memmove(objects[i + 1], objects[i], (num_objects - i) * sizeof(void*));
            objects[i] = phys;
            num_objects++;
            return true;
        }
    }
    objects[num_objects] = phys;
    num_objects++;
    return true;
}

void remove_object(physics_body_t *phys) {
    for(uint8_t i = 0; i < num_objects; i++) {
        if(phys == objects[i]) {
            memmove(objects[i], objects[i + 1], (num_objects - 1 - i) * sizeof(void*));
            break;
        }
    }
    num_objects--;
}

void sort_objects(void) {
    // Wikipedia Insertion Sort
    for(uint8_t i = 1; i < num_objects; i++) {
        physics_body_t *x = objects[i];
        int24_t y = x->position_y;
        int8_t j;
        for(j = i - 1; j >= 0 && objects[j]->position_y > y; j--) {
            objects[j + 1] = objects[j];
        }
        objects[j + 1] = x;
    }
}
