cmake_minimum_required(VERSION 3.15)
project(tanks C CXX)

set(CMAKE_C_FLAGS "-S -nostdinc -isystem $ENV{CEDEV}/include -Dinterrupt=\"__attribute__\(\(__interrupt__\)\)\" -Dreentrant= -D_EZ80 -Wno-main-return-type")
set(CMAKE_CXX_FLAGS "-S -nostdinc -isystem $ENV{CEDEV}/include -Dinterrupt=\"__attribute__\(\(__interrupt__\)\)\" -Dreentrant= -D_EZ80 -Wno-main-return-type")

include_directories($ENV{CEDEV}/include)

add_custom_target(Make ALL $(MAKE) debug WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# A dummy target to make CLion do stuff
add_executable(dummy
        src/ai/ai.cpp
        src/ai/ai.h
        src/physics/collision.cpp
        src/physics/collision.h
        src/graphics/graphics.cpp
        src/graphics/graphics.h
        src/level.cpp
        src/level.h
        src/main.cpp
        src/util/util.cpp
        src/util/util.h src/objects/tank.cpp src/objects/tank.h src/objects/shell.cpp src/objects/shell.h src/objects/mine.cpp src/objects/mine.h src/util/profiler.c src/util/profiler.h src/graphics/partial_redraw.cpp src/graphics/partial_redraw.h src/graphics/dynamic_sprites.cpp src/graphics/dynamic_sprites.h src/ai/ai_state.h src/objects/physicsbody.cpp src/objects/physicsbody.h src/input.cpp src/input.h src/fwd.h src/util/trig.cpp src/util/trig.h src/physics/physics.h src/game.cpp src/game.h src/objects/mine_detector.cpp src/objects/mine_detector.h src/util/debug_malloc.h src/graphics/tiles.cpp src/graphics/tiles.h src/graphics/repalettize.cpp src/graphics/repalettize.h src/graphics/tank_sprite.cpp src/graphics/tank_sprite.h src/gui/error.cpp src/gui/error.h src/gui/pause.cpp src/gui/pause.h src/gui/kill_counts.cpp src/gui/kill_counts.h src/gui/transition.cpp src/gui/transition.h src/gui/banner.cpp src/gui/banner.h src/gui/aim_indicator.cpp src/gui/aim_indicator.h src/test/test.cpp src/test/test.h)
