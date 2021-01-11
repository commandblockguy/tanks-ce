#include "game.h"

#include <new>
#include <debug.h>
#include "objects/tank.h"
#include "globals.h"
#include "graphics/graphics.h"
#include "graphics/gui.h"
#include "input.h"
#include "util/profiler.h"

bool start_mission(const serialized_tank_t *ser_tanks) {
    dbg_printf("starting mission\n");
    bool tank_type_used[NUM_TANK_TYPES] = {false};

    while(!PhysicsBody::objects.empty()) {
        // Delete objects without killing
        delete PhysicsBody::objects[0];
    }

    game.num_tanks = 0;

    for(uint8_t i = 0; i < game.level.num_tanks; i++) {
        if(!game.alive_tanks[i]) continue;
        //dbg_printf("tank created: %p\n", tank);
        Tank *tank = new (std::nothrow) Tank(&ser_tanks[i], i);
        if(!tank) {
            dbg_printf("Failed to allocate tank\n");
            continue;
        }
        tank_type_used[ser_tanks[i].type] = true;
    }

    for(uint8_t x = 1; x < LEVEL_SIZE_X - 1; x++) {
        for(uint8_t y = 1; y < LEVEL_SIZE_Y - 1; y++) {
            if(TILE_TYPE(tiles[y][x]) == DESTROYED)
                tiles[y][x] = TILE_HEIGHT(tiles[y][x]) | DESTRUCTIBLE;
        }
    }

    for(uint8_t type = 1; type < NUM_TANK_TYPES; type++) {
        if(tank_type_used[type]) {
            if(!init_tank_sprites(type)) {
                dbg_sprintf(dbgerr, "Ran out of memory when allocating tank sprites\n");
                return false;
            }
        } else {
            free_tank_sprites(type);
        }
    }

    mission_start_screen(game.mission, game.lives, game.num_tanks - 1);

    init_timer();
    needs_redraw = true;
    return true;
}

uint8_t play_mission(const serialized_tank_t *ser_tanks) {
    start_mission(ser_tanks);
    while(true) {
        profiler_start(total);

        //handle player input
        uint8_t status = handle_input();
        if(status) return status;


        profiler_start(physics);
        //dbg_printf("0: %p\n", PhysicsBody::objects[0]);
        for(auto & object : PhysicsBody::objects) {
            //dbg_printf("%p\n", *it);
            object->process();
        }
        profiler_start(pb_collision);
        process_collisions();
        profiler_end(pb_collision);
        profiler_end(physics);

        if(!game.player_alive) {
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

        PhysicsBody::sort();

        render();

        profiler_end(total);
        profiler_start(frame_wait);
        limit_framerate();
        profiler_end(frame_wait);
        profiler_tick();
    }
}

uint8_t play_level(const void *comp_tiles, const serialized_tank_t *ser_tanks) {
    decompress_tiles(comp_tiles);

    for(uint8_t i = 0; i < game.level.num_tanks; i++){
        game.alive_tanks[i] = true;
    }

    uint8_t status;
    do status = play_mission(ser_tanks);
    while(status == RETRY);

    return status;
}
