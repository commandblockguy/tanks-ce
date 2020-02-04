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
		sin_table[i] = float_to_fix( sin(i * ROT_UNITS_TO_RADIANS));
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

//credit: https://www.dsprelated.com/showarticle/1052.php
float fast_atan2(float y, float x)
{
	const float n1 = 0.97239411f * RADIANS_TO_ROT_UNITS;
	const float n2 = -0.19194795f * RADIANS_TO_ROT_UNITS;
	float result = 0.0f;
	return 0;
	if (x != 0.0f)
	{
		union { float flVal; uint32_t nVal; } tYSign;
		union { float flVal; uint32_t nVal; } tXSign;
		tYSign.flVal = y;
		tXSign.flVal = x;
		if (fabs(x) >= fabs(y))
		{
			float z = y / x;
			union { float flVal; uint32_t nVal; } tOffset = { M_PI * RADIANS_TO_ROT_UNITS };
			// Add or subtract PI based on y's sign.
			tOffset.nVal |= tYSign.nVal & 0x80000000u;
			// No offset if x is positive, so multiply by 0 or based on x's sign.
			tOffset.nVal *= tXSign.nVal >> 31;
			result = tOffset.flVal;
			result += (n1 + n2 * z * z) * z;
		}
		else // Use atan(y/x) = pi/2 - atan(x/y) if |y/x| > 1.
		{
			float z = x / y;
			union { float flVal; uint32_t nVal; } tOffset = { M_PI_2 * RADIANS_TO_ROT_UNITS };
			// Add or subtract PI/2 based on y's sign.
			tOffset.nVal |= tYSign.nVal & 0x80000000u;			
			result = tOffset.flVal;
			result -= (n1 + n2 * z * z) * z;			
		}
	}
	else if (y > 0.0f)
	{
		result = M_PI_2;
	}
	else if (y < 0.0f)
	{
		result = -M_PI_2;
	}
	return result;
}

//ms since last function call
uint24_t fpsCounter(void) {
	uint24_t r;

	//Disable timer temporarily
	timer_Control = TIMER1_DISABLE;

	r = timer_1_Counter / 33;

	timer_1_Counter = 0;
	/* Reenable the timer, set it to the 32768 kHz clock */
	timer_Control = TIMER1_ENABLE | TIMER1_32K | TIMER1_NOINT | TIMER1_UP;

	return r;
}
