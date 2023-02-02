#include <stdio.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_image.h"

#include "SDL2/SDL_mouse.h"
#include "game.cpp"
#include "card.cpp"
#include "common.h"


GLOBAL bool running = true;

GLOBAL SDL_Window *window;
GLOBAL SDL_Renderer *renderer;

void ProcessInputKey(GameKeyState *state, bool is_down) {
	if (state->is_down != is_down) {
		state->is_down = is_down;
	}
}

void ProcessInput(GameInput *input) {
	GameMouse *mouse = &input->mouse;
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

			GameKeyState *up = &input->controller.up;
			GameKeyState *down = &input->controller.down;
			GameKeyState *left = &input->controller.left;
			GameKeyState *right = &input->controller.right;

			ProcessInputKey(up, !is_up);
			ProcessInputKey(down, !is_down);
			ProcessInputKey(left, !is_left);
			ProcessInputKey(right, !is_right);

			continue;
		}

		if (event.type == SDL_KEYDOWN) {
			int32_t scancode = event.key.keysym.scancode;

			bool is_up = (scancode == SDL_SCANCODE_W) || (scancode == SDL_SCANCODE_UP);
			bool is_down = (scancode == SDL_SCANCODE_S) || (scancode == SDL_SCANCODE_DOWN);
			bool is_left = (scancode == SDL_SCANCODE_A) || (scancode == SDL_SCANCODE_LEFT);
			bool is_right = (scancode == SDL_SCANCODE_D) || (scancode == SDL_SCANCODE_RIGHT);

			GameKeyState *up = &input->controller.up;
			GameKeyState *down = &input->controller.down;
			GameKeyState *left = &input->controller.left;
			GameKeyState *right = &input->controller.right;

			ProcessInputKey(up, is_up);
			ProcessInputKey(down, is_down);
			ProcessInputKey(left, is_left);
			ProcessInputKey(right, is_right);

			continue;
		}

		if (event.type == SDL_MOUSEBUTTONUP) {
			bool is_left = (event.button.button == SDL_BUTTON_LEFT);
			bool is_right = (event.button.button == SDL_BUTTON_RIGHT);
			bool is_middle = (event.button.button == SDL_BUTTON_MIDDLE);

			GameKeyState *left = &mouse->left;
			GameKeyState *right = &mouse->right;
			GameKeyState *middle = &mouse->middle;

			ProcessInputKey(left, !is_left);
			ProcessInputKey(right, !is_right);
			ProcessInputKey(middle, !is_middle);
			continue;
		}

		if (event.type == SDL_MOUSEBUTTONDOWN) {
			bool is_left = (event.button.button == SDL_BUTTON_LEFT);
			bool is_right = (event.button.button == SDL_BUTTON_RIGHT);
			bool is_middle = (event.button.button == SDL_BUTTON_MIDDLE);

			GameKeyState *left = &mouse->left;
			GameKeyState *right = &mouse->right;
			GameKeyState *middle = &mouse->middle;			

			ProcessInputKey(left, is_left);
			ProcessInputKey(right, is_right);
			ProcessInputKey(middle, is_middle);			
		}
	}
}

void UpdateAndRender(GameInput *input, GameMemory *memory) {
	if (!memory->initialzed) {
		memory->initialzed = true;
		memory->len = Gb(1);
		memory->base = Alloc(memory->len);

		CardTable *table = (CardTable *) memory->base;
		CardInit(renderer, table);
	}

	CardTable *table = (CardTable *) memory->base;
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	CardTableUpdateAndRender(renderer, input, table);

	SDL_Rect rect = {};
	rect.x = input->mouse.x - 10;
	rect.y = input->mouse.y - 10;
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

	err = SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	Assert(!err);

	GameInput input = {};
	GameMemory memory = {};

	while (running) {
		ProcessInput(&input);
		UpdateAndRender(&input, &memory);
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
