#pragma once


#include "common.cpp"

#include "ui_layout.cpp"

#include "game_text.cpp"
#include "game_input.cpp"

#include "card_table.cpp"


struct GameMemory {
	b32 initialzed;

	usize len;
	void  *base;
};

void GameUpdateAndRender(GameInput *input,
						 GameMemory *memory) {
	CardTable *card_table = GetFromMem(CardTable, memory->base, 0);
	if (!card_table->initialized) {
		// NOTE: 52 cards, 1 drop area, 2 buttons
		CardTableInit(card_table);
	}

	SDL_SetRenderDrawColor(global_renderer, 41, 43, 46, 255);
	SDL_RenderClear(global_renderer);

	CardTableUpdateAndRender(global_renderer, input, card_table);

	SDL_Rect rect = {};
	rect.x = input->mouse.x - 10;
	rect.y = input->mouse.y - 10;
	rect.w = 20;
	rect.h = 20;
	SDL_SetRenderDrawColor(global_renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(global_renderer, &rect);
	
	SDL_RenderPresent(global_renderer);
}
