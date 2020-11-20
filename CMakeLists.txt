cmake_minimum_required(VERSION 3.15)
project(tanks CXX)

set(CMAKE_C_FLAGS "-S -nostdinc -isystem ~/CEdev/include -Dinterrupt=\"__attribute__\(\(__interrupt__\)\)\" -Dreentrant= -D_EZ80 -Wno-main-return-type")
set(CMAKE_CXX_FLAGS "-S -nostdinc -isystem ~/CEdev/include -Dinterrupt=\"__attribute__\(\(__interrupt__\)\)\" -Dreentrant= -D_EZ80 -Wno-main-return-type")

include_directories($ENV{CEDEV}/include)

add_custom_target(Make ALL $(MAKE) WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# A dummy target to make CLion do stuff
add_executable(dummy
        src/tiles/lvlpack.c
        src/tiles/lvlpack.h
        src/ai.cpp
        src/ai.h
        src/collision.cpp
        src/collision.h
        src/graphics.cpp
        src/graphics.h
        src/level.cpp
        src/level.h
        src/main.cpp
        src/util.cpp
        src/util.h src/tank.cpp src/tank.h src/shell.cpp src/shell.h src/mine.cpp src/mine.h src/globals.cpp src/globals.h src/profiler.cpp src/profiler.h src/gui.cpp src/gui.h src/partial_redraw.cpp src/partial_redraw.h src/dynamic_sprites.cpp src/dynamic_sprites.h src/ai_state.h src/physics.cpp src/physics.h src/input.cpp src/input.h)
