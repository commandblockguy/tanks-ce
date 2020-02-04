cmake_minimum_required(VERSION 3.15)
project(tanks C)

set(CMAKE_C_STANDARD 90)

include_directories($ENV{CEDEV}/include)

add_custom_target(Make ALL $(MAKE) WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# A dummy target to make CLion do stuff
add_executable(dummy
        src/gfx/tiles_gfx.c
        src/gfx/tiles_gfx.h
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
        src/objects.c
        src/objects.h
        src/util.c
        src/util.h)
