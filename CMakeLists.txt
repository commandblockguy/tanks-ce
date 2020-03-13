cmake_minimum_required(VERSION 3.15)
project(tanks C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_FLAGS "-S -nostdinc -isystem ~/CEdev/include -Dinterrupt=\"__attribute__\(\(__interrupt__\)\)\" -Dreentrant= -D_EZ80 -Wno-main-return-type")

include_directories($ENV{CEDEV}/include)

add_custom_target(Make ALL $(MAKE) WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# A dummy target to make CLion do stuff
add_executable(dummy
        src/gfx/tileset.c
        src/tiles/lvlpack.c
        src/tiles/lvlpack.h
        src/ai.c
        src/ai.h
        src/ai_data.h
        src/collision.c
        src/collision.h
        src/constants.h
        src/graphics.c
        src/graphics.h
        src/level.c
        src/level.h
        src/main.c
        src/util.c
        src/util.h src/tank.c src/tank.h src/shell.c src/shell.h src/mine.c src/mine.h src/globals.c src/globals.h)
