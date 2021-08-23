#include "mine.h"

#include "../graphics/graphics.h"
#include "../graphics/dynamic_sprites.h"
#include "../util/profiler.h"
#include "../graphics/partial_redraw.h"
#include "mine_detector.h"
#include "../graphics/tiles.h"
#include "../game.h"

#include <new>

Mine::Mine(Tank *tank):
    PhysicsBody(MINE_SIZE, MINE_SIZE),
    countdown(MINE_COUNTDOWN) {
    tile_collisions = false;
    respect_holes = false;

    position_x = tank->position_x + (TANK_SIZE - MINE_SIZE) / 2;
    position_y = tank->position_y + (TANK_SIZE - MINE_SIZE) / 2;

    parent = tank;

    new (std::nothrow) MineDetector(this);
}

Mine::~Mine() {
    if(parent) {
        ((Tank*)parent)->num_mines--;
    }
}

void Mine::process() {
    //Ignore mines which have already finished their countdowns
    if(!countdown) return;

    profiler_add(mines);
    if(--countdown == EXPLOSION_ANIM) {
        detonate();
    }
    if(countdown == 0) {
        active = false;
    }

//todo: range detection
    profiler_end(mines);
}

void Mine::render(uint8_t layer) {
    if(layer != 1) return;
    profiler_add(render_mines);

    if(countdown > EXPLOSION_ANIM) {
        uint8_t sprite;
        if(countdown < MINE_WARNING) {
            sprite = countdown % 4;
        } else {
            sprite = 0;
        }
        gfx_region_t region;
        get_sprite_footprint(&region, this, mine_sprites, mine_x_offsets, mine_y_offsets, sprite);
        if(pdraw_RectRegion(&region)) {
            gfx_TransparentSprite(mine_sprites[sprite], region.xmin, region.ymin);
            redraw_tiles(&region, 0);
        }
    } else {
        // todo: maybe resize the sprite to make it more animated
        uint x = SCREEN_X(center_x()) - explosion_width / 2;
        uint8_t y = SCREEN_Y(center_y()) + TILE_PIXEL_SIZE_Y * MINE_EXPLOSION_RADIUS / TILE_SIZE - explosion_height;
        gfx_region_t region;
        region.xmin = x;
        region.ymin = y;
        region.xmax = region.xmin + explosion_width;
        region.ymax = region.ymin + explosion_height;

        gfx_GetClipRegion(&region);

        if(pdraw_RectRegion(&region)) {
            gfx_TransparentSprite(explosion, x, y);
            // todo: fix this
            //redraw_tiles(&region, 0);
        }
    }
    profiler_end(render_mines);
}

void Mine::detonate() {
    if(countdown < EXPLOSION_ANIM) {
        // Don't explode multiple times
        return;
    }

    countdown = EXPLOSION_ANIM - 1;

    // todo: The original game uses a radius, not a square

    for(uint8_t j = COORD_TO_X_TILE(center_x() - MINE_EXPLOSION_RADIUS);
        j <= COORD_TO_X_TILE(center_x() + MINE_EXPLOSION_RADIUS); j++) {
        if(j < 0 || j >= LEVEL_SIZE_X) continue;
        for(uint8_t k = COORD_TO_Y_TILE(center_y() - MINE_EXPLOSION_RADIUS);
            k <= COORD_TO_Y_TILE(center_y() + MINE_EXPLOSION_RADIUS); k++) {
            if(k < 0 || k >= LEVEL_SIZE_Y) continue;
            if(TILE_TYPE(game.tiles[k][j]) == DESTRUCTIBLE) game.tiles[k][j] = 0;
            needs_redraw = true;
        }
    }

    // Kill any nearby physics objects
    for(auto & obj : objects) {
        if(obj != this && center_distance_less_than(obj, MINE_EXPLOSION_RADIUS)) {
            obj->handle_explosion();
        }
    }

    generate_bg_tilemap();
}

void Mine::handle_explosion() {
    detonate();
}

void Mine::handle_collision(PhysicsBody *other) {
    other->collide(this);
}

void Mine::collide(Shell *shell) {
    shell->collide(this);
}
