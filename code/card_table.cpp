#pragma once

#include "card.cpp"
#include "common.cpp"

struct CardTable {
	bool initialized;

	i32 last_player_turn;
	i32 player_turn;
	i32 table_turn;
	i32 have_turn[4];

	i32      player_cards_len[4];
	Card     player_cards[4][13];
	UiLayout player_cards_layout[4][13];

	i32 table_last_cards[13];
	i32 dumping_cards[52];

	UiLayout drop_layout;
	UiLayout drop_text_layout;
	GameFont drop_font;
	char     *drop_text;

	i32  select_cards_len;
	Card select_cards[13];
};

/// NOTE(): Texture and Table init
void CardTableInit(CardTable *table) {
	CardImagesInit();

	table->last_player_turn = 0;
	table->player_turn = RandomI32(0, 4);
	table->table_turn = 0;
	for (i32 i = 0; i < 4; ++i) {
		table->have_turn[i] = 1;
	}

	Card deck_cards[52];
	for (i32 i = 0; i < 52; i++) {
		deck_cards[i] = i;
	}

	ShuffleI32Array(deck_cards, 52);
	for (i32 i = 0; i < 4; ++i) {
		table->player_cards_len[i] = 13;
		for (i32 j = 0; j < 13; ++j) {
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

void CardTableRemoveCard(CardTable *table, i32 player, Card card) {
	i32 len = table->player_cards_len[player];
	Card *cards = table->player_cards[player];
	UiLayout *layouts = table->player_cards_layout[player];

	i32 remove_idx = CardListIndex(cards, len, card);

	if (remove_idx == -1) {
		return;
	}

	RemoveIndex(cards, len, remove_idx);
	RemoveIndex(layouts, len, remove_idx);
	table->player_cards_len[player]--;
}

void CardTablePlaySelect(CardTable *table) {
	i32 len = table->select_cards_len;

	CardComResult combine_result = CardCombine(table->select_cards, len);

	table->drop_text = (char *)card_com_string[combine_result.com];

	if (combine_result.com == CARD_COM_ERR) {
		return;
	}

	ForRange (i, 0, len) {
		CardTableRemoveCard(table, 0, table->select_cards[i]);
	}
	table->select_cards_len = 0;
}

void CardTableTryReleaseDrag(CardTable *table, GameMouse *mouse) {
	i32 len = table->select_cards_len;
	if (len == 0) {
		return;
	}

	ForRange (i, 0, len) {
		i32 idx = CardListIndex(table->player_cards[0],
								table->player_cards_len[0],
								table->select_cards[i]);
		UiLayoutReleaseDrag(&table->player_cards_layout[0][idx]);
	}

	bool in_drop_area = UiLayoutMouseIn(&table->drop_layout, mouse);

	if (in_drop_area) {
		CardTablePlaySelect(table);
	}

	table->select_cards_len = 0;
}

void CardTableTryDrag(CardTable *table,
					  GameMouse *mouse,
					  Card card) {
	if (table->select_cards_len == 0 && card == -1) {
		return;
	}

	bool card_selected = CardListContains(
		table->select_cards,
		table->select_cards_len,
		card
	);
	if (!card_selected) {
		table->select_cards[table->select_cards_len] = card;
		table->select_cards_len++;
	}

	i32 len = table->select_cards_len;
	ForRange (i, 0, len) {
		i32 idx = CardListIndex(table->player_cards[0],
							table->player_cards_len[0],
							table->select_cards[i]);
		UiLayoutDrag(&table->player_cards_layout[0][idx], mouse);
	}
}

void CardTableTryUnselect(CardTable *table, GameMouse *mouse) {
	i32 len = table->select_cards_len;
	if (len == 0) {
		return;
	}

	ForRange (i, 0, len) {
		i32 idx = CardListIndex(table->player_cards[0],
								table->player_cards_len[0],
								table->select_cards[i]);
		UiLayoutUnselect(&table->player_cards_layout[0][idx]);
	}

	bool in_drop_area = UiLayoutMouseIn(&table->drop_layout, mouse);

	if (in_drop_area) {
		CardTablePlaySelect(table);
	}

	table->select_cards_len = 0;
}

void CardTableTrySelect(CardTable *table,
						GameMouse *mouse,
						Card card) {
	if (card == -1) {
		return;
	}
	i32 idx = CardListIndex(table->player_cards[0],
							table->player_cards_len[0],
							card);
	bool card_selected = CardListContains(table->select_cards,
										  table->select_cards_len,
										  card);
	if (!card_selected) {
		table->select_cards[table->select_cards_len] = card;
		table->select_cards_len++;
		UiLayoutSelect(&table->player_cards_layout[0][idx]);
	} else {
		RemoveIndex(table->select_cards, table->select_cards_len, idx);
		table->select_cards_len--;
		UiLayoutUnselect(&table->player_cards_layout[0][idx]);
	}
}

void CardTableUpdateAndRender(SDL_Renderer *renderer,
							  GameInput *input,
							  CardTable *table) {
	GameMouse *mouse = &input->mouse;

	// Update player_0's hand
	{
		i32 hand_width = (table->player_cards_len[0])
			* (card_width / 4 * 3)
			+ (card_width / 4);

		i32 x = (window_width - hand_width) / 2;
		i32 y = window_height - card_height - 40;

		i32 len = table->player_cards_len[0];
		ForRange (i, 0, len) {
			UiLayout *layout = &table->player_cards_layout[0][i];
			// if (!UiLayoutIsDrag(layout)) {
			// 	layout->x = x;
			// 	layout->y = y;
			// }
			layout->x = x;
			layout->y = y;
			if (UiLayoutIsSelect(layout)) {
				layout->y -= 20;
			}

			x = x + (card_width / 4 * 3);
		}
	}

	/// Update player_1's hand
	{
		i32 hand_height = (table->player_cards_len[1])
			* (card_height / 16)
			+ ((card_height / 16) * 15);
		i32 x = (window_width - card_width - 40);
		i32 y = (window_height - hand_height) / 2;

		i32 len = table->player_cards_len[1];
		ForRange (i, 0, len) {
			UiLayout *layout = &table->player_cards_layout[1][i];
			layout->x = x;
			layout->y = y;
			y = y + card_height / 15;
		}
	}

	// Update player_2's hand
	{
		i32 hand_width = (table->player_cards_len[2] + 1) * (card_width / 2);
		i32 x = (window_width - hand_width) / 2;
		i32 y = 20;

		i32 len = table->player_cards_len[2];
		ForRange (i, 0, len) {
			UiLayout *layout = &table->player_cards_layout[2][i];
			layout->x = x;
			layout->y = y;
			x = x + card_width / 2;
		}
	}

	/// Update player_3's hand
	{
		i32 hand_height = (table->player_cards_len[3])
			* (card_height / 16)
			+ ((card_height / 16) * 15);
		i32 x = 40;
		i32 y = (window_height - hand_height) / 2;

		i32 len = table->player_cards_len[3];
		ForRange (i, 0, len) {
			UiLayout *layout = &table->player_cards_layout[3][i];
			layout->x = x;
			layout->y = y;
			y = y + card_height / 15;
		}
	}

	Card hover_card = -1;
	{
		i32 len = table->player_cards_len[0];
		if (len == 1) {
			UiLayout *layout = &table->player_cards_layout[0][0];
			if (UiLayoutMouseIn(layout, &input->mouse)) {
				hover_card = table->player_cards[0][0];
			}
		} else {
			ForRange (i, 0, len - 1) {
				UiLayout *layout_0 = &table->player_cards_layout[0][i];
				UiLayout *layout_1 = &table->player_cards_layout[0][i+1];
				bool in0 = UiLayoutMouseIn(layout_0, mouse);
				bool in1 = UiLayoutMouseIn(layout_1, mouse);
				if (in0) {
					hover_card = table->player_cards[0][i];
				}
				if (in1) {
					hover_card = table->player_cards[0][i+1];
				}
			}
		}
	}

	b32 in_drop_area = UiLayoutMouseIn(&table->drop_layout, &input->mouse);

	if (mouse->left.is_down && mouse->left.half_transition_count) {
		if (in_drop_area) {
			CardTablePlaySelect(table);
		} else {
			CardTableTrySelect(table, mouse, hover_card);
		}
	}

	// NOTE: Render from here
	if (in_drop_area) {
		UiLayoutDrawColor(&table->drop_layout, 0x00ff0033);
	}

	ForRange (i, 0, 4) {
		i32 len = table->player_cards_len[i];
		ForRange (j, 0, len) {
			Card card = table->player_cards[i][j];
			UiLayout *layout = &table->player_cards_layout[i][j];
			if (i == 0) {
				CardDraw(layout, table->player_cards[i][j]);
			} else {
				CardBackDraw(layout);
			}

			if (card == hover_card) {
				UiLayoutDrawColor(layout, 0x00ff0033);
			}
		}

		UiLayoutDrawText(&table->drop_text_layout,
						 &table->drop_font,
						 table->drop_text, 0xffffffff);
	}
}
