#include "globals.h"

game_t game; //Game global, so I can reuse those names elsewhere if needed

tank_t* tanks; //List of all active tanks.
tile_t tiles[LEVEL_SIZE_X * LEVEL_SIZE_Y]; //Currently active tilemap data