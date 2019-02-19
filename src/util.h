#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#ifndef H_UTIL

fix_t fast_sin(uint8_t rot);

#define fast_cos(rot) fast_sin(64 - rot)

#endif