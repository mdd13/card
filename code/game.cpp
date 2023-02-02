#pragma once

#include "common.h"

struct GameKeyState {
	bool is_down;
};

struct GameController {
	GameKeyState up;
	GameKeyState down;
	GameKeyState left;
	GameKeyState right;
};

struct GameMouse {
	int32_t x;
	int32_t y;

	GameKeyState left;
	GameKeyState right;
	GameKeyState middle;
};

struct GameScroll {
	bool is_up;
	bool is_down;
};

struct GameInput {
	GameMouse      mouse;
	GameScroll     scroll;
	GameController controller;
};

struct GameMemory {
	bool initialzed;
	
	size_t len;
	void   *base;
};
