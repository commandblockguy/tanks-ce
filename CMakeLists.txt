cmake_minimum_required(VERSION 3.15)
project(tanks C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_FLAGS "-S -nostdinc -isystem ~/CEdev/include -Dinterrupt=\"__attribute__\(\(__interrupt__\)\)\" -Dreentrant= -D_EZ80 -Wno-main-return-type")

include_directories($ENV{CEDEV}/include)

add_custom_target(Make ALL $(MAKE) WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# A dummy target to make CLion do stuff
add_executable(dummy
        src/tiles/lvlpack.c
        src/tiles/lvlpack.h
        src/ai.c
        src/ai.h
        src/collision.c
        src/collision.h
        src/graphics.c
        src/graphics.h
        src/level.c
        src/level.h
        src/main.c
        src/util.c
        src/util.h src/tank.c src/tank.h src/shell.c src/shell.h src/mine.c src/mine.h src/globals.c src/globals.h src/profiler.c src/profiler.h src/gui.c src/gui.h src/partial_redraw.c src/partial_redraw.h src/dynamic_sprites.c src/dynamic_sprites.h src/ai_state.h)
