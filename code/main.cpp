#include <stdio.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_image.h"

#include "SDL2/SDL_mouse.h"
#include "game.cpp"
#include "card.cpp"
#include "common.cpp"


GLOBAL bool running = true;

void ProcessInputKey(GameKeyState *state, bool is_down) {
	if (state->is_down != is_down) {
		state->is_down = is_down;
	}
}

void ProcessInput(GameInput *input) {
	GameMouse *mouse = &input->mouse;
	GameController *controller = &input->controller;
	
	SDL_GetMouseState(&mouse->x, &mouse->y);

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

			ProcessInputKey(&controller->up, !is_up);
			ProcessInputKey(&controller->down, !is_down);
			ProcessInputKey(&controller->left, !is_left);
			ProcessInputKey(&controller->right, !is_right);

			continue;
		}

		if (event.type == SDL_KEYDOWN) {
			int32_t scancode = event.key.keysym.scancode;

			bool is_up = (scancode == SDL_SCANCODE_W) || (scancode == SDL_SCANCODE_UP);
			bool is_down = (scancode == SDL_SCANCODE_S) || (scancode == SDL_SCANCODE_DOWN);
			bool is_left = (scancode == SDL_SCANCODE_A) || (scancode == SDL_SCANCODE_LEFT);
			bool is_right = (scancode == SDL_SCANCODE_D) || (scancode == SDL_SCANCODE_RIGHT);


			ProcessInputKey(&controller->up, is_up);
			ProcessInputKey(&controller->down, is_down);
			ProcessInputKey(&controller->left, is_left);
			ProcessInputKey(&controller->right, is_right);

			continue;
		}

		if (event.type == SDL_MOUSEBUTTONUP) {
			bool is_left = (event.button.button == SDL_BUTTON_LEFT);
			bool is_right = (event.button.button == SDL_BUTTON_RIGHT);
			bool is_middle = (event.button.button == SDL_BUTTON_MIDDLE);

			ProcessInputKey(&mouse->left, !is_left);
			ProcessInputKey(&mouse->right, !is_right);
			ProcessInputKey(&mouse->middle, !is_middle);
			continue;
		}

		if (event.type == SDL_MOUSEBUTTONDOWN) {
			bool is_left = (event.button.button == SDL_BUTTON_LEFT);
			bool is_right = (event.button.button == SDL_BUTTON_RIGHT);
			bool is_middle = (event.button.button == SDL_BUTTON_MIDDLE);

			ProcessInputKey(&mouse->left, is_left);
			ProcessInputKey(&mouse->right, is_right);
			ProcessInputKey(&mouse->middle, is_middle);
		}
	}
}

int main() {
	int err = SDL_Init(SDL_INIT_EVERYTHING);
	Assert(!err);
	int ok = IMG_Init(IMG_INIT_PNG);
	Assert(ok);
	err = TTF_Init();
	Assert(!err);
	

	global_window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_OPENGL);
	Assert(global_window);

	global_renderer = SDL_CreateRenderer(global_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	Assert(global_renderer);

	err = SDL_SetRenderDrawBlendMode(global_renderer, SDL_BLENDMODE_BLEND);
	Assert(!err);

	GameInput input = {};
	GameMemory memory = {};
	memory.len = Gb(1);
	memory.base = Alloc(memory.len);
	
	while (running) {
		ProcessInput(&input);

		GameUpdateAndRender(&input, &memory);
	}

	SDL_DestroyWindow(global_window);
	SDL_DestroyRenderer(global_renderer);

	return 0;
}

#ifdef WINDOWS
int WinMain() {
	return main();
}
#endif
