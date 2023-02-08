#pragma once

#include "card.cpp"
#include "common.cpp"

void DrawCard(UiLayout *layout, Card card) {
	UiLayoutDrawImage(layout, &card52_images[card]);
}

void DrawCardBack(UiLayout *layout) {
	LOCAL_PERSIST int t = 0;
	UiLayoutDrawImage(layout, &card52_back_images[t]);
	t = (t + 1) % 4;
}

struct CardTable {
	bool initialized;

	int last_player_turn;
	int player_turn;
	int table_turn;
	int have_turn[4];

	int      player_cards_len[4];
	Card     player_cards[4][13];
	UiLayout player_cards_layout[4][13];

	int table_last_cards[13];
	int dumping_cards[52];

	UiLayout drop_layout;
	UiLayout drop_text_layout;
	GameFont drop_font;
	char     *drop_text;

	int       select_cards_len;
	Card      select_cards[13];
	UiLayout  select_cards_layout[13];

	int        grab_cards_len;
	Card       grab_cards[13];
	UiLayout   grab_cards_layout[4][13];	
};

/// NOTE(): Texture and Table init
void CardTableInit(CardTable *table) {
	CardImagesInit();

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

			UiLayout layout = {};
			layout.x = 0;
			layout.y = 0;
			layout.w = card_width;
			layout.h = card_height;

			table->player_cards_layout[i][j] = layout;
		}
	}

	UiLayout drop_layout = {};

	drop_layout.w = window_width - (400);
	drop_layout.h = window_height - (400);
	drop_layout.x = (window_width - drop_layout.w) / 2;
	drop_layout.y = (window_height - drop_layout.h) / 2;
	table->drop_layout = drop_layout;

	UiLayout drop_text_layout = {};
	drop_text_layout.x = drop_layout.x + 20;
	drop_text_layout.y = drop_layout.y + 20;
	table->drop_text_layout = drop_text_layout;

	GameFontInit(&table->drop_font, LIBERATION_MONO_PATH, 24);

	table->initialized = true;
}

void CardTableRemoveCard(CardTable *table, int player, Card card) {
	int *len = &table->player_cards_len[player];
	Card *cards = table->player_cards[player];
	UiLayout *layouts = table->player_cards_layout[player];

	int remove_idx = CardListIndex(cards, *len, card);

	if (remove_idx == -1) {
		return;
	}

	CardListRemoveIndex(cards, layouts, len, remove_idx);
}

void CardTableTryRelaseGrab(CardTable *table) {
	int len = table->grab_cards_len;
	if (len > 0) {
		UiLayout *layout = table->grab_cards_layout[len-1];
		bool in_drop_layout = UiLayoutCollsionChecking(layout, &table->drop_layout);
		if (in_drop_layout) {
			CardComResult combine_result = CardCombine(table->grab_cards, len);
			table->drop_text = (char *)card_com_string[combine_result.com];
			while(len-- > 0) {
				Card card = table->grab_cards[len];
				CardTableRemoveCard(table, 0, card);
			}
		}
	}

	ForRange (i, 0, len) {
		UiLayout *layout = table->grab_cards_layout[i];
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
		UiLayout *layout = &table->player_cards_layout[hover_i][hover_j];
		if (!CardListContains(table->grab_cards, table->grab_cards_len, card)) {
			layout->mouse_rel_x = input->mouse.x - layout->x;
			layout->mouse_rel_y = input->mouse.y - layout->y;
			layout->is_grabbed = true;
			table->grab_cards[table->grab_cards_len] = card;
			table->grab_cards_layout[table->grab_cards_len] = *layout;
			table->grab_cards_len++;
		}
	}

	int len = table->grab_cards_len;
	ForRange (i, 0, len) {
		UiLayout *layout = table->grab_images[i];
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
			UiLayout *layout = table->player_images[0][i];
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
			UiLayout *layout = table->player_images[1][i];
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
			UiLayout *layout = table->player_images[2][i];
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
			UiLayout *layout = table->player_images[3][i];
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
			UiLayout *layout = table->player_images[i][0];
			if (UiLayoutMouseIn(layout, &input->mouse)) {
				hover_i = i;
				hover_j = 0;
				continue;
			}
		}

		ForRange (j, 0, len - 1) {
			UiLayout *layout_0 = table->player_images[i][j];
			UiLayout *layout_1 = table->player_images[i][j+1];
			bool in0 = UiLayoutMouseIn(layout_0, mouse);
			bool in1 = UiLayoutMouseIn(layout_1, mouse);
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
			UiLayout *layout = table->player_images[i][j];
			if (UiLayoutCollsionChecking(layout, table->drop_layout)) {
				drop_i = i;
				drop_j = j;
			}
		}
	}
	UiLayoutRender(table->drop_layout, renderer, (drop_i != -1));

	/// NOTE(): Render
	ForRange (i, 0, 4) {
		int len = table->player_cards_len[i];
		ForRange (j, 0, len) {
			UiLayout *layout = table->player_images[i][j];
			bool hl = ((i == hover_i) && (j == hover_j)) || ((i == drop_i) && (j == drop_j));

			if (i != 0) {
				LOCAL_PERSIST int back_idx = 0;
				back_idx = (back_idx + 1) % 8000;
				layout->texture = card52_back_images[back_idx / 2000];
			}

			UiLayoutRender(layout, renderer, hl);
		}

		GameTextRender(&table->drop_text, renderer, 0xffffff);
	}
}
