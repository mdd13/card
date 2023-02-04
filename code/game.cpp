#pragma once


#include "common.cpp"
#include "game_entity.cpp"
#include "game_text.cpp"
#include "game_input.cpp"
#include "card.cpp"

struct GameMemory {
	bool initialzed;

	size_t len;
	void   *base;
};

void GameUpdateAndRender(GameInput *input,
						 GameMemory *memory,
						 SDL_Renderer *renderer) {
	GameEntityPool *entity_pool = (GameEntityPool *)memory->base;
	CardTable *card_table = GetFromMem(CardTable, memory->base, sizeof(GameEntityPool));

	if (!entity_pool->initialized) {
		GameEntityPoolInit(entity_pool);
	}

	if (!card_table->initialized) {
		CardInit(card_table, entity_pool, renderer);
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
