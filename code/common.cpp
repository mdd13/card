#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_net.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


typedef bool    b1;
typedef int32_t b32;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  f32;
typedef double f64;

typedef size_t  usize;

#define Assert(condition) if (!(condition)) { *(volatile i32 *)0 = 0; }
#define I32ERNAL static
#define GLOBAL static
#define LOCAL_PERSIST static
#define ReturnDefer(x) {result = x; goto defer;}
#define Swap(x, y) {auto t = x; x = y; y = t;}
#define ForRange(i, l, h) for (i32 i = l; i < h; i++)
#define Kb(byte) ((i64)(byte << 10))
#define Mb(byte) ((i64)(Kb(byte) << 10))
#define Gb(byte) ((i64)(Mb(byte) << 10))
#define GetFromMem(typ, ptr, pad) (typ *)((usize *)ptr + pad)

GLOBAL const char *title = "Card";
GLOBAL i32 window_width  = 1280;
GLOBAL i32 window_height = 720;

GLOBAL SDL_Window *global_window;
GLOBAL SDL_Renderer *global_renderer;

#define LIBERATION_MONO_PATH "font/LiberationMono-Regular.ttf"

