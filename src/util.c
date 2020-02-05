#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#undef NDEBUG
#include <debug.h>
#include "util.h"
#include "graphx.h"

void gen_lookups(void);

int24_t sin_table[64];

bool lookups_set = false;

void gen_lookups(void) {
	uint8_t i;
	for(i = 0; i < 64; i++) {
		sin_table[i] = sin(i * M_PI / 128) * 256;
	}
	lookups_set = true;
}

int24_t fast_sin(angle_t angle) {
    //todo: fix angle
    angle = angle >> 16;
	if(!lookups_set) gen_lookups();
	if(angle < 64) return sin_table[angle];
	if(angle == 64) return 256;
	if(angle < 128) return sin_table[128 - angle];
	if(angle < 192) return -sin_table[angle - 128];
	if(angle == 192) return -256;
				  return -sin_table[256 - angle];
}

//credit: https://www.dsprelated.com/showarticle/1052.php
float fast_atan2(float y, float x)
{
	const float n1 = 0.97239411f * RADIANS_TO_ROT_UNITS;
	const float n2 = -0.19194795f * RADIANS_TO_ROT_UNITS;
	float result = 0.0f;
	if (x != 0.0f)
	{
		union { float flVal; uint32_t nVal; } tYSign;
		union { float flVal; uint32_t nVal; } tXSign;
		tYSign.flVal = y;
		tXSign.flVal = x;
		if (abs(x) >= abs(y))
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
	return result * (1 << 16);
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
