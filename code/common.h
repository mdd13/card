#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define Assert(condition) if (!condition) { fprintf(stderr, "%s:%d:0 %s\n", __FILE__,  __LINE__, SDL_GetError()); exit(1); }
#define GLOBAL static
#define ReturnDefer(x) {result = x; goto defer;}
#define Swap(x, y) {auto t = x; x = y; y = t;}

GLOBAL const char *title = "Card";
GLOBAL const int32_t window_width = 1280;
GLOBAL const int32_t window_height = 720;
