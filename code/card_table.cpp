#pragma once

#include "card.cpp"

struct CardTable {
	bool initialized;

	int last_player_turn;
	int player_turn;
	int table_turn;
	int have_turn[4];

	int        player_cards_len[4];
	Card       player_cards[4][13];
	GameLayout *player_entities[4][13];

	int table_last_cards[13];
	int dumping_cards[52];

	GameLayout *drop_area;
	GameText   drop_text;

	int        select_cards_len;
	Card       select_cards[13];
	GameLayout *select_entities[13];

	int        grab_cards_len;
	Card       grab_cards[13];
	GameLayout *grab_entities[13];

	GameLayout *button_play;
	GameLayout *button_release;
};

/// NOTE(): Texture and Table init
void CardTableInit(CardTable *table, GameLayoutPool *layout_pool, SDL_Renderer *renderer) {
	if (!card52_images[0]) {
		CardTextureInit(renderer);
	}

	table->last_player_turn = 0;
	table->player_turn = RandomInt(0, 4);
	table->table_turn = 0;
	for (int i = 0; i < 4; ++i) {
		table->have_turn[i] = 1;
	}

	Card deck_cards[52];
	for (int i = 0; i < 52; i++) {
		deck_cards[i] = i;
	}

	ShuffleIntArray(deck_cards, 52);
	for (int i = 0; i < 4; ++i) {
		table->player_cards_len[i] = 13;
		for (int j = 0; j < 13; ++j) {
			Card card = deck_cards[j * 4 + i];
			table->player_cards[i][j] = card;

			GameLayout *new_layout = GameLayoutPoolGet(layout_pool, -1);

			new_layout->x = 0;
			new_layout->y = 0;
			new_layout->w = card_width;
			new_layout->h = card_height;

			if (i == 0) {
				new_layout->texture = card52_images[card];
			} else {
				new_layout->texture = card52_back_images[0];
			}

			table->player_entities[i][j] = new_layout;
		}
	}

	GameLayout *drop_area = GameLayoutPoolGet(layout_pool, -1);

	drop_area->w = window_width - (400);
	drop_area->h = window_height - (400);
	drop_area->x = (window_width - drop_area->w) / 2;
	drop_area->y = (window_height - drop_area->h) / 2;
	table->drop_area = drop_area;

	GameText *drop_text = &table->drop_text;
	drop_text->x = drop_area->x;
	drop_text->y = drop_area->y;
	drop_text->data = (char *)"...";
	GameFontInit(&drop_text->font, "font/LiberationMono-Regular.ttf", 24);

	// GameLayout *button_play = GameLayoutPoolGet(layout_pool, -1);
	// GameLayout *button_release = GameLayoutPoolGet(layout_pool, -1);

	table->initialized = true;
}

void CardRemove(CardTable *table, int player, Card card) {
	int *len = &table->player_cards_len[player];
	Card *cards = table->player_cards[player];
	GameLayout **entities = table->player_entities[player];

	int remove_idx = CardListIndex(cards, *len, card);

	if (remove_idx == -1) {
		return;
	}

	CardListRemoveIndex(cards, entities, len, remove_idx);
}

void CardTableTryRelaseGrab(CardTable *table) {
	int len = table->grab_cards_len;
	if (len > 0) {
		GameLayout *layout = table->grab_entities[len-1];
		bool in_drop_area = GameLayoutCollsionChecking(layout, table->drop_area);
		if (in_drop_area) {
			CardComResult combine_result = CardCombine(table->grab_cards, len);
			GameTextUpdateText(
				&table->drop_text,
				(char *)card_com_string[combine_result.com]
				);

			while(len-- > 0) {
				Card card = table->grab_cards[len];
				CardRemove(table, 0, card);
			}
		}
	}

	ForRange (i, 0, len) {
		GameLayout *layout = table->grab_entities[i];
		layout->is_grabbed = false;
	}

	table->grab_cards_len = 0;
}

void CardTableTryGrab(CardTable *table,
					  GameInput *input,
					  int hover_i,
					  int hover_j) {
	if (hover_i == 0) {
		Card card = table->player_cards[hover_i][hover_j];
		GameLayout *layout = table->player_entities[hover_i][hover_j];
		if (!CardListContains(table->grab_cards, table->grab_cards_len, card)) {
			layout->mouse_rel_x = input->mouse.x - layout->x;
			layout->mouse_rel_y = input->mouse.y - layout->y;
			layout->is_grabbed = true;
			table->grab_cards[table->grab_cards_len] = card;
			table->grab_entities[table->grab_cards_len] = layout;
			table->grab_cards_len++;
		}
	}

	int len = table->grab_cards_len;
	ForRange (i, 0, len) {
		GameLayout *layout = table->grab_entities[i];
		layout->x = input->mouse.x - layout->mouse_rel_x;
		layout->y = input->mouse.y - layout->mouse_rel_y;
	}
}

void CardTableUpdateAndRender(SDL_Renderer *renderer, GameInput *input, CardTable *table) {
	GameMouse *mouse = &input->mouse;

	// Update player_0's hand
	{
		int hand_width = (table->player_cards_len[0] + 1) * (card_width / 2);
		int x = (window_width - hand_width) / 2;
		int y = window_height - card_height - 40;

		int len = table->player_cards_len[0];
		ForRange (i, 0, len) {
			GameLayout *layout = table->player_entities[0][i];
			if (!layout->is_grabbed) {
				layout->x = x;
				layout->y = y;
			}
			x = x + card_width / 2;
		}
	}

	/// Update player_1's hand
	{
		int hand_height = (table->player_cards_len[1]) * (card_height / 4)
			+ ((card_height / 4) * 3);
		int x = (window_width - card_width - 40);
		int y = (window_height - hand_height) / 2;

		int len = table->player_cards_len[1];
		ForRange (i, 0, len) {
			GameLayout *layout = table->player_entities[1][i];
			layout->x = x;
			layout->y = y;
			y = y + card_height / 4;
		}
	}

	// Update player_2's hand
	{
		int hand_width = (table->player_cards_len[2] + 1) * (card_width / 2);
		int x = (window_width - hand_width) / 2;
		int y = 20;

		int len = table->player_cards_len[2];
		ForRange (i, 0, len) {
			GameLayout *layout = table->player_entities[2][i];
			layout->x = x;
			layout->y = y;
			x = x + card_width / 2;
		}
	}

	/// Update player_3's hand
	{
		int hand_height = (table->player_cards_len[3]) * (card_height / 4)
			+ ((card_height / 4) * 3);
		int x = 40;
		int y = (window_height - hand_height) / 2;

		int len = table->player_cards_len[3];
		ForRange (i, 0, len) {
			GameLayout *layout = table->player_entities[3][i];
			layout->x = x;
			layout->y = y;
			y = y + card_height / 4;
		}
	}

	int hover_i = -1;
	int hover_j = -1;

	ForRange (i, 0, 4) {
		int len = table->player_cards_len[i];
		if (len == 1) {
			GameLayout *layout = table->player_entities[i][0];
			if (GameLayoutMouseIn(layout, &input->mouse)) {
				hover_i = i;
				hover_j = 0;
				continue;
			}
		}

		ForRange (j, 0, len - 1) {
			GameLayout *layout_0 = table->player_entities[i][j];
			GameLayout *layout_1 = table->player_entities[i][j+1];
			bool in0 = GameLayoutMouseIn(layout_0, mouse);
			bool in1 = GameLayoutMouseIn(layout_1, mouse);
			if (in0) {
				hover_i = i;
				hover_j = j;
			}
			if (in1) {
				hover_i = i;
				hover_j = j+1;
			}
		}
	}

	if (!mouse->left.is_down) {
		CardTableTryRelaseGrab(table);
	} else {
		CardTableTryGrab(table, input, hover_i, hover_j);
	}

	int drop_i = -1;
	int drop_j = -1;
	ForRange (i, 0, 4) {
		int len = table->player_cards_len[i];
		ForRange (j, 0, len) {
			GameLayout *layout = table->player_entities[i][j];
			if (GameLayoutCollsionChecking(layout, table->drop_area)) {
				drop_i = i;
				drop_j = j;
			}
		}
	}
	GameLayoutRender(table->drop_area, renderer, (drop_i != -1));

	/// NOTE(): Render
	ForRange (i, 0, 4) {
		int len = table->player_cards_len[i];
		ForRange (j, 0, len) {
			GameLayout *layout = table->player_entities[i][j];
			bool hl = ((i == hover_i) && (j == hover_j)) || ((i == drop_i) && (j == drop_j));

			if (i != 0) {
				LOCAL_PERSIST int back_idx = 0;
				back_idx = (back_idx + 1) % 8000;
				layout->texture = card52_back_images[back_idx / 2000];
			}

			GameLayoutRender(layout, renderer, hl);
		}

		GameTextRender(&table->drop_text, renderer, 0xffffff);
	}
}
