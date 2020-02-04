#ifndef H_AI
#define H_AI

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "level.h"
#include "objects.h"

void ai_process_move(Tank* tank);
void ai_process_fire(Tank* tank);

#endif /* H_AI */