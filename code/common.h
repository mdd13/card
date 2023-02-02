#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define Assert(condition) if (!condition) { fprintf(stderr, "%s:%d:0 %s\n", __FILE__,  __LINE__, SDL_GetError()); exit(1); }
#define GLOBAL static
#define LOCAL_PERSIST static
#define ReturnDefer(x) {result = x; goto defer;}
#define Swap(x, y) {auto t = x; x = y; y = t;}
#define ForRange(i, l, h) for (int i = l; i < h; i++)
#define Kb(byte) ((int64_t)(byte << 10))
#define Mb(byte) ((int64_t)(Kb(byte) << 10))
#define Gb(byte) ((int64_t)(Mb(byte) << 10))

GLOBAL const char *title = "Card";
GLOBAL const int32_t window_width = 1280;
GLOBAL const int32_t window_height = 720;
