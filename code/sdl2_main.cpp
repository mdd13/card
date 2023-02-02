#include <stdio.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "card.cpp"
#include "common.h"

GLOBAL bool running = true;

GLOBAL SDL_Window *window;
GLOBAL SDL_Renderer *renderer;

/// Mouse position
GLOBAL int32_t mouse_x;
GLOBAL int32_t mouse_y;

/// Key down state
GLOBAL bool controller_up;
GLOBAL bool controller_down;
GLOBAL bool controller_left;
GLOBAL bool controller_right;

void ProcessInput() {
	SDL_GetMouseState(&mouse_x, &mouse_y);

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			running = false;
			break;
		}

		if (event.type == SDL_KEYUP) {
			int32_t scancode = event.key.keysym.scancode;

			bool is_up = (scancode == SDL_SCANCODE_W) || (scancode == SDL_SCANCODE_UP);
			bool is_down = (scancode == SDL_SCANCODE_S) || (scancode == SDL_SCANCODE_DOWN);
			bool is_left = (scancode == SDL_SCANCODE_A) || (scancode == SDL_SCANCODE_LEFT);
			bool is_right = (scancode == SDL_SCANCODE_D) || (scancode == SDL_SCANCODE_RIGHT);

			controller_up = (controller_up && !is_up);
			controller_down = (controller_down && !is_down);
			controller_left = (controller_left && !is_left);
			controller_right = (controller_right && !is_right);

			continue;
		}

		if (event.type == SDL_KEYDOWN) {
			int32_t scancode = event.key.keysym.scancode;

			bool is_up = (scancode == SDL_SCANCODE_W) || (scancode == SDL_SCANCODE_UP);
			bool is_down = (scancode == SDL_SCANCODE_S) || (scancode == SDL_SCANCODE_DOWN);
			bool is_left = (scancode == SDL_SCANCODE_A) || (scancode == SDL_SCANCODE_LEFT);
			bool is_right = (scancode == SDL_SCANCODE_D) || (scancode == SDL_SCANCODE_RIGHT);

			controller_up = (controller_up || is_up);
			controller_down = (controller_down || is_down);
			controller_left = (controller_left || is_left);
			controller_right = (controller_right || is_right);
		}
	}
}

struct GameState {
	bool initialzed;
	CardBoard board;
};

void UpdateAndRender(GameState *state) {
	if (!state->initialzed) {
		state->initialzed = true;
		CardBoardInit(&state->board);
		CardTextureInit(renderer);
	}

	CardBoard *board = &state->board;

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	CardBoardRender(renderer, board);

	SDL_Rect rect;
	rect.x = mouse_x - 10;
	rect.y = mouse_y - 10;
	rect.w = 20;
	rect.h = 20;

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(renderer, &rect);

	SDL_RenderPresent(renderer);
}

int main() {
	int err = SDL_Init(SDL_INIT_EVERYTHING);
	Assert(!err);

	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_OPENGL);
	Assert(window);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	Assert(renderer);

	GameState state = {};

	while (running) {
		ProcessInput();
		UpdateAndRender(&state);
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	return 0;
}

#ifdef WINDOWS
int WinMain() {
	return main();
}
#endif
