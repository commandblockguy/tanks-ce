#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "debug.h"
#include "util.h"
#include "graphx.h"

void gen_lookups(void);

fix_t sin_table[64];

bool lookups_set = false;

void gen_lookups(void) {
	uint8_t i;
	for(i = 0; i < 64; i++) {
		sin_table[i] = float_to_fix(sin(i * ROT_UNITS_TO_RADIANS));
	}
	lookups_set = true;
}

fix_t fast_sin(uint8_t rot) {
	if(!lookups_set) gen_lookups();
	if(rot <  64 ) return sin_table[rot];
	if(rot == 64 ) return to_ufix(1);
	if(rot <  128) return sin_table[128 - rot];
	if(rot <  192) return -sin_table[rot - 128];
	if(rot == 192) return to_ufix(-1);
	              return -sin_table[256 - rot];
}
