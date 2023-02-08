#include <stdio.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_image.h"

#include "SDL2/SDL_mouse.h"
#include "game.cpp"
#include "card.cpp"
#include "common.cpp"


GLOBAL b32 running = true;

void ProcessInputButton(GameKeyState *state, b32 is_down) {
	if (state->is_down != is_down) {
		state->is_down = is_down;
		state->half_transition_count = 1;
	} else {
		state->half_transition_count = 0;
	}
}

void ProcessInput(GameInput *input) {
	GameMouse *mouse = &input->mouse;
	SDL_GetMouseState(&mouse->x, &mouse->y);
	b32 mouse_event_happened = false;
	
	GameController *controller = &input->controller;
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			running = false;
			break;
		}

		if (event.type == SDL_KEYUP) {
			i32 scancode = event.key.keysym.scancode;

			b32 is_up = (scancode == SDL_SCANCODE_W) || (scancode == SDL_SCANCODE_UP);
			b32 is_down = (scancode == SDL_SCANCODE_S) || (scancode == SDL_SCANCODE_DOWN);
			b32 is_left = (scancode == SDL_SCANCODE_A) || (scancode == SDL_SCANCODE_LEFT);
			b32 is_right = (scancode == SDL_SCANCODE_D) || (scancode == SDL_SCANCODE_RIGHT);

			ProcessInputButton(&controller->up, !is_up);
			ProcessInputButton(&controller->down, !is_down);
			ProcessInputButton(&controller->left, !is_left);
			ProcessInputButton(&controller->right, !is_right);

			continue;
		}

		if (event.type == SDL_KEYDOWN) {
			i32 scancode = event.key.keysym.scancode;

			b32 is_up = (scancode == SDL_SCANCODE_W) || (scancode == SDL_SCANCODE_UP);
			b32 is_down = (scancode == SDL_SCANCODE_S) || (scancode == SDL_SCANCODE_DOWN);
			b32 is_left = (scancode == SDL_SCANCODE_A) || (scancode == SDL_SCANCODE_LEFT);
			b32 is_right = (scancode == SDL_SCANCODE_D) || (scancode == SDL_SCANCODE_RIGHT);

			ProcessInputButton(&controller->up, is_up);
			ProcessInputButton(&controller->down, is_down);
			ProcessInputButton(&controller->left, is_left);
			ProcessInputButton(&controller->right, is_right);

			continue;
		}

		if (event.type == SDL_MOUSEBUTTONUP) {
			b32 is_left = !(event.button.button == SDL_BUTTON_LEFT);
			b32 is_right = !(event.button.button == SDL_BUTTON_RIGHT);
			b32 is_middle = !(event.button.button == SDL_BUTTON_MIDDLE);
			
			ProcessInputButton(&mouse->left, is_left);
			ProcessInputButton(&mouse->right, is_right);
			ProcessInputButton(&mouse->middle, is_middle);

			mouse_event_happened = true;
			continue;
		}

		if (event.type == SDL_MOUSEBUTTONDOWN) {
			b32 is_left = (event.button.button == SDL_BUTTON_LEFT);
			b32 is_right = (event.button.button == SDL_BUTTON_RIGHT);
			b32 is_middle = (event.button.button == SDL_BUTTON_MIDDLE);
			
			ProcessInputButton(&mouse->left, is_left);
			ProcessInputButton(&mouse->right, is_right);
			ProcessInputButton(&mouse->middle, is_middle);

			mouse_event_happened = true;
			continue;
		}
	}

	if (!mouse_event_happened) {
		ProcessInputButton(&mouse->left, mouse->left.is_down);
		ProcessInputButton(&mouse->right, mouse->right.is_down);
		ProcessInputButton(&mouse->middle, mouse->middle.is_down);
	}
	
}

i32 main() {
	i32 err = SDL_Init(SDL_INIT_EVERYTHING);
	Assert(!err);
	i32 ok = IMG_Init(IMG_INIT_PNG);
	Assert(ok);
	err = TTF_Init();
	Assert(!err);
	err = SDLNet_Init();
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
i32 WinMain() {
	return main();
}
#endif
