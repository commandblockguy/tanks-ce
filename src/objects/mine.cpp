#include "mine.h"
#include "../level.h"
#include "../globals.h"
#include "../graphics/gui.h"
#include "../graphics/graphics.h"
#include "../graphics/dynamic_sprites.h"
#include "../util/profiler.h"

Mine::Mine(Tank *tank) {
    width = MINE_SIZE;
    height = MINE_SIZE;
    respect_holes = true;

    parent = tank;

    countdown = MINE_COUNTDOWN;
}

Mine::~Mine() {
    if(parent) {
        // todo: see if there's a way to do this without an ugly cast
        ((Tank*)parent)->num_mines--;
    }
}

void Mine::process() {
    //Ignore mines which have already finished their countdowns
    if(!countdown) return;

    profiler_add(mines);
    if(--countdown == EXPLOSION_ANIM) {
        kill();
        profiler_end(mines);
        return;
    }

//todo: range detection
    profiler_end(mines);
}

void Mine::render(uint8_t layer) {
    if(layer != 0) return;
    profiler_add(render_mines);

    uint8_t sprite;
    if(countdown < MINE_WARNING) {
        sprite = countdown % 4;
    } else {
        sprite = 0;
    }
    render_obscured_object(mine_sprites, mine_x_offsets, mine_y_offsets, this, sprite, 0);
    profiler_end(render_mines);
}

void Mine::kill() {
    countdown = EXPLOSION_ANIM - 1;

    //The original game uses a radius, not a square
    //Don't tell anyone.

    for(uint8_t j = COORD_TO_X_TILE(center_x() - MINE_EXPLOSION_RADIUS);
        j <= COORD_TO_X_TILE(center_x() + MINE_EXPLOSION_RADIUS); j++) {
        if(j < 0 || j >= LEVEL_SIZE_X) continue;
        for(uint8_t k = COORD_TO_Y_TILE(center_y() - MINE_EXPLOSION_RADIUS);
            k <= COORD_TO_Y_TILE(center_y() + MINE_EXPLOSION_RADIUS); k++) {
            if(k < 0 || k >= LEVEL_SIZE_Y) continue;
            if(TILE_TYPE(tiles[k][j]) == DESTRUCTIBLE) tiles[k][j] |= DESTROYED;
            needs_redraw = true;
        }
    }

    // Kill any nearby physics objects
    // todo: unbork this
    for(auto *it = objects.begin(); it < objects.end();) {
        if(*it != this && center_distance_less_than(*it, MINE_EXPLOSION_RADIUS)) {
            (**it).kill();
        } else it++;
    }

    bang();

    generate_bg_tilemap();

    delete this;
}

void Mine::handle_collision(PhysicsBody *other) {
    other->collide(this);
}

void Mine::collide(__attribute__((unused)) Tank *tank) {
    // don't do anything
}

void Mine::collide(Shell *shell) {
    shell->collide(this);
}

void Mine::collide(__attribute__((unused)) Mine *mine) {
    // don't do anything
}
