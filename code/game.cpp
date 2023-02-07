#pragma once


#include "common.cpp"
#include "game_layout.cpp"
#include "game_text.cpp"
#include "game_input.cpp"

#include "card_table.cpp"


struct GameMemory {
	bool initialzed;

	size_t len;
	void   *base;
};

void GameUpdateAndRender(GameInput *input,
						 GameMemory *memory,
						 SDL_Renderer *renderer) {
	GameLayoutPool *layout_pool = (GameLayoutPool *)memory->base;
	CardTable *card_table = GetFromMem(CardTable, memory->base, sizeof(GameLayoutPool));

	if (!layout_pool->initialized) {
		GameLayoutPoolInit(layout_pool);
	}

	if (!card_table->initialized) {
		// NOTE: 52 cards, 1 drop area, 2 buttons
		GameLayoutPoolExpand(layout_pool, 55);
		CardTableInit(card_table, layout_pool, renderer);
	}

	SDL_SetRenderDrawColor(renderer, 41, 43, 46, 255);
	SDL_RenderClear(renderer);

	CardTableUpdateAndRender(renderer, input, card_table);

	SDL_Rect rect = {};
	rect.x = input->mouse.x - 10;
	rect.y = input->mouse.y - 10;
	rect.w = 20;
	rect.h = 20;

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(renderer, &rect);

	SDL_RenderPresent(renderer);
}
