#pragma once

#include "common.cpp"

struct GameKeyState {
	b32 is_down;
	i32  half_transition_count;
};

struct GameController {
	GameKeyState up;
	GameKeyState down;
	GameKeyState left;
	GameKeyState right;
};

struct GameMouse {
	i32 x;
	i32 y;

	GameKeyState left;
	GameKeyState right;
	GameKeyState middle;
};

struct GameScroll {
	b32 is_up;
	b32 is_down;
};

struct GameInput {
	GameMouse      mouse;
	GameScroll     scroll;
	GameController controller;
};
