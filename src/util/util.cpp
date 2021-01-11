#include "util.h"

#include <math.h>

#include <limits.h>
#include "../graphics/graphics.h"

void init_timer() {
    timer_Disable(1);
    timer_Set(1, 32768 / TARGET_TICK_RATE);
    timer_SetReload(1, 32768 / TARGET_TICK_RATE);
    timer_Enable(1, TIMER_32K, TIMER_0INT, TIMER_DOWN);
}

void limit_framerate() {
    while(!timer_ChkInterrupt(1, TIMER_RELOADED));
    timer_AckInterrupt(1, TIMER_RELOADED);
}
