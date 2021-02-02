#include "game.h"

#include <new>
#include <debug.h>
#include "objects/tank.h"
#include "graphics/graphics.h"
#include "input.h"
#include "util/profiler.h"
#include "graphics/tank_sprite.h"
#include "gui/error.h"
#include "gui/transition.h"

struct game game;

bool start_mission(const struct serialized_tank *ser_tanks) {
    dbg_printf("starting mission\n");

    PhysicsBody::remove_all();

    game.num_tanks = 0;

    for(uint8_t i = 0; i < game.level.num_tanks; i++) {
        if(!game.alive_tanks[i]) continue;
        //dbg_printf("tank created: %p\n", tank);
        Tank *tank = new(std::nothrow) Tank(&ser_tanks[i], i);
        if(!tank) {
            ERROR("Failed to allocate tank");
        }
    }

    for(uint8_t x = 1; x < LEVEL_SIZE_X - 1; x++) {
        for(uint8_t y = 1; y < LEVEL_SIZE_Y - 1; y++) {
            if(TILE_TYPE(game.tiles[y][x]) == DESTROYED)
                game.tiles[y][x] = TILE_HEIGHT(game.tiles[y][x]) | DESTRUCTIBLE;
        }
    }

    game.tick = 0;

    free_tank_sprites();

    mission_start_screen(game.mission, game.lives, game.num_tanks - 1);

    init_timer();
    needs_redraw = true;
    return true;
}

uint8_t play_mission(const struct serialized_tank *ser_tanks) {
    start_mission(ser_tanks);
    while(true) {
        profiler_start(total);

        //handle player input
        uint8_t status = handle_input();
        if(status) return status;


        profiler_start(physics);
        for(uint i = 0; i < PhysicsBody::objects.size(); i++) {
            PhysicsBody::objects[i]->process();
        }
        profiler_start(pb_collision);
        process_collisions();
        profiler_end(pb_collision);
        for(auto &object : PhysicsBody::objects) {
            object->tick();
        }
        PhysicsBody::remove_inactive();
        PhysicsBody::sort();
        profiler_end(physics);

        if(!game.player) {
            profiler_end(total);
            game.lives--;
            if(!game.lives) {
                return LOSE;
            } else {
                return RETRY;
            }
        }

        if(game.num_tanks == 1) {
            profiler_end(total);
            return NEXT_LEVEL;
        }

        render();

        game.tick++;

        profiler_end(total);
        profiler_start(frame_wait);
        limit_framerate();
        profiler_end(frame_wait);
        profiler_tick();
    }
}

uint8_t play_level(const void *comp_tiles, const struct serialized_tank *ser_tanks) {
    decompress_tiles(comp_tiles);

    for(uint8_t i = 0; i < game.level.num_tanks; i++) {
        game.alive_tanks[i] = true;
    }

    uint8_t status;
    do status = play_mission(ser_tanks);
    while(status == RETRY);

    return status;
}
