#pragma once
/// TODO: Use header file instead
#include "common.cpp"
#include "mem.cpp"

#include "game_text.cpp"
#include "game_input.cpp"
#include "game_entity.cpp"
#include "game.cpp"
#include "random.cpp"
#include "52card.cpp"

#include <string.h>

/// Các loại bài, theo thứ tự:
///
/// - Cơ, Rô, Chuồn, Bích
GLOBAL int card_suit_rank[] = {
	[CARD_HEART] = 0,
	[CARD_DIAMOND] = 1,
	[CARD_CLUB] = 2,
	[CARD_SPADE] = 3,
};

/// Giá trị trên các quân bài
GLOBAL int card_key_rank[] = {
	[CARD_2] = 0,
	[CARD_A] = 1,
	[CARD_K] = 2,
	[CARD_Q] = 3,
	[CARD_J] = 4,
	[CARD_10] = 5,
	[CARD_9] = 6,
	[CARD_8] = 7,
	[CARD_7] = 8,
	[CARD_6] = 9,
	[CARD_5] = 10,
	[CARD_4] = 11,
	[CARD_3] = 12,
};

int GetCardRank(Card card) {
	CardSuit suit = GetCardSuit(card);
	CardKey key = GetCardKey(card);

	return card_key_rank[key] * 4 + card_suit_rank[suit];
}

/// Các nước đi, theo thứ tự:
///
/// - Lỗi, Cóc, Cóc 2, Đôi, Đôi 2, Bộ ba, Ba con 2, Tứ quý, Tứ quý 2, Dây, Thông
enum CardCom {
	CARD_COM_ERR,
	CARD_COM_SINGLE,
	CARD_COM_DOUBLE,
	CARD_COM_TRIPS,
	CARD_COM_QUADS,
	CARD_COM_SEQUENCE,
	CARD_COM_SEQUENCE_DOUBLE,
	CARD_COM_TOTAL,
};

const char *card_com_string[] {
	"CARD_COM_ERR",
	"CARD_COM_SINGLE",
	"CARD_COM_DOUBLE",
	"CARD_COM_TRIPS",
	"CARD_COM_QUADS",
	"CARD_COM_SEQUENCE",
	"CARD_COM_SEQUENCE_DOUBLE",
};

struct CardComResult {
	CardCom  com;
	int      value;
	int      len;
};


bool CardsIsSequence(Card *cards, int len) {
	if (GetCardKey(cards[0]) == CARD_2) {
		return false;
	}

	for (int i = 1; i < len; ++i) {
		CardKey v0 = GetCardKey(cards[i - 1]);
		CardKey v1 = GetCardKey(cards[i]);
		if (v0 + 1 != v1) {
			return false;
		}
	}
	return true;
}

bool CardsIsDoubleSequence(Card *cards, int len) {
	if (len < 6) {
		return false;
	}

	if (GetCardKey(cards[0]) == CARD_2) {
		return false;
	}

	for (int i = 2; i < len; i += 2) {
		CardKey v0 = GetCardKey(cards[i - 2]);
		CardKey v1 = GetCardKey(cards[i - 1]);
		CardKey v2 = GetCardKey(cards[i]);
		if (v0 != v1 || v1 + 1 != v2) {
			return false;
		}
	}

	return true;
}

CardComResult CardCombine(Card *cards, int len) {
	CardComResult result = {};

	if (len == 1) {
		result.com = CARD_COM_SINGLE;
		result.value = cards[0];
		ReturnDefer(result);
	}

	for (int i = 0; i < len - 1; ++i) {
		int mn_rank = GetCardRank(cards[i]);
		int mn_idx = i;

		for (int j = i + 1; j < len; ++j) {
			int rank = GetCardRank(cards[j]);
			if (mn_rank > rank) {
				mn_rank = rank;
				mn_idx = j;
			}
		}

		if (mn_idx != i) {
			Swap(cards[i], cards[mn_idx]);
		}
	}

	if (len == 2) {
		if (GetCardKey(cards[0]) == GetCardKey(cards[1])) {
			CardKey value = GetCardKey(cards[0]);
			result.com = CARD_COM_DOUBLE;
			result.value = cards[0] + value * 4;
			ReturnDefer(result);
		}
		goto error_handle;
	}

	if (CardsIsSequence(cards, len)) {
		int sum = cards[0];
		for (int i = 1; i < len; ++i) {
			sum += GetCardKey(cards[i]) * 4;
		}
		result.com = CARD_COM_SEQUENCE;
		result.value = sum;
		ReturnDefer(result);
	}

	if (CardsIsDoubleSequence(cards, len)) {
		int sum = cards[0];
		for (int i = 1; i < len; ++i) {
			sum += GetCardKey(cards[i]) * 4;
		}
		result.com = CARD_COM_SEQUENCE_DOUBLE;
		result.value = sum;
		ReturnDefer(result);
	}

	if (len == 3) {
		if (GetCardKey(cards[0]) == GetCardKey(cards[1]) &&
			GetCardKey(cards[1]) == GetCardKey(cards[2])) {
			CardKey value = GetCardKey(cards[0]);
			result.com = CARD_COM_TRIPS;
			result.value = cards[0] +  value * 8;
			ReturnDefer(result);
		}
		goto error_handle;
	}

	if (len == 4) {
		if (GetCardKey(cards[0]) == GetCardKey(cards[1]) &&
			GetCardKey(cards[1]) == GetCardKey(cards[2]) &&
			GetCardKey(cards[2]) == GetCardKey(cards[3])) {
			CardKey value = GetCardKey(cards[0]);

			result.com =  CARD_COM_QUADS;
			result.value = cards[0] +  value * 16;
			ReturnDefer(result);
		}
		goto error_handle;
	}

error_handle:
	result.com = CARD_COM_ERR;
	result.value = -1;
	ReturnDefer(result);

defer:
	result.len = len;
	return result;
}

enum CardPlayStatus {
	CARD_PLAY_ERR,
	CARD_PLAY_OK,
};

const char *card_play_result_string[] {
	"CARD_PLAY_ERR",
	"CARD_PLAY_OK",
};

struct CardPlayResult {
	CardPlayStatus status;
};

typedef CardPlayResult (*CardCompareCall)(const CardComResult &, const CardComResult &);

CardPlayResult CardPlayValue(const CardComResult &r0,
							 const CardComResult &r1) {
	CardPlayResult result = {};

	if (r0.value < r1.value) {
		result.status = CARD_PLAY_OK;
	} else {
		result.status = CARD_PLAY_ERR;
	}

	return result;
}

CardPlayResult CardPlaySequenceDouble(const CardComResult &r0,
									  const CardComResult &r1) {
	CardPlayResult result;

	if (r1.com == CARD_COM_SINGLE) {
		if (GetCardKey(r1.value) == CARD_2) {
			goto handle_ok;
		}
		goto handle_err;
	}

	if (r1.com == CARD_COM_DOUBLE) {
		if (GetCardKey(r1.value / r1.len) == CARD_2 && r0.len > 6) {
			goto handle_ok;
		}
		goto handle_err;
	}

	if (r1.com == CARD_COM_SEQUENCE_DOUBLE) {
		if (r0.len == r1.len) {
			return CardPlayValue(r0, r1);
		}
		if (r0.len > r1.len) {
			goto handle_ok;
		}
		goto handle_err;
	}

	goto handle_err;

handle_err:
	result.status = CARD_PLAY_ERR;
	return result;

handle_ok:
	result.status = CARD_PLAY_OK;
	return result;
}

CardPlayResult CardPlayQuads(const CardComResult &r0,
							 const CardComResult &r1) {
	CardPlayResult result;

	if (r1.com == CARD_COM_SINGLE) {
		if (GetCardKey(r1.value) == CARD_2) {
			goto handle_ok;
		}
		goto handle_err;
	}

	if (r1.com == CARD_COM_DOUBLE) {
		if (GetCardKey(r1.value / r1.len) == CARD_2) {
			goto handle_ok;
		}
		goto handle_err;
	}

	goto handle_err;

handle_err:
	result.status = CARD_PLAY_ERR;
	return result;

handle_ok:
	result.status = CARD_PLAY_OK;
	return result;
}

/// 3 đôi thông được chặt một heo, hoặc 3 đôi thông nhỏ hơn.
/// 4 đôi thông được heo, đôi heo, 3 đôi thông, tứ quý, 4 đôi thông nhỏ hơn
/// 4 đôi thông bất kì sẽ chặt bất kì heo hay hàng nhỏ hơn (theo vòng hoặc không tùy vùng)
/// Tứ quý được chặt heo hoặc đôi heo, 3 đôi thông, tứ quý nhỏ hơn (không cần vòng)
/// 3 con heo thì không có hàng nào chặt được.
/// Tứ quý heo (về trắng)
CardCompareCall card_com_calls[CARD_COM_TOTAL][CARD_COM_TOTAL];

void CardComCallsInit() {
	for (int i = 0; i < CARD_COM_TOTAL; ++i) {
		for (int j = 0; j < CARD_COM_TOTAL; ++j) {
			card_com_calls[i][j] = 0;
		}
	}

	card_com_calls[CARD_COM_SINGLE][CARD_COM_SINGLE] = &CardPlayValue;
	card_com_calls[CARD_COM_DOUBLE][CARD_COM_DOUBLE] = &CardPlayValue;
	card_com_calls[CARD_COM_TRIPS][CARD_COM_TRIPS] = &CardPlayValue;
	card_com_calls[CARD_COM_QUADS][CARD_COM_QUADS] = &CardPlayValue;
	card_com_calls[CARD_COM_SEQUENCE][CARD_COM_SEQUENCE] = &CardPlayValue;
	card_com_calls[CARD_COM_SEQUENCE_DOUBLE][CARD_COM_SEQUENCE_DOUBLE] = &CardPlaySequenceDouble;

	card_com_calls[CARD_COM_SEQUENCE_DOUBLE][CARD_COM_SINGLE] = &CardPlaySequenceDouble;
	card_com_calls[CARD_COM_SEQUENCE_DOUBLE][CARD_COM_DOUBLE] = &CardPlaySequenceDouble;

	card_com_calls[CARD_COM_QUADS][CARD_COM_SINGLE] = &CardPlayQuads;
	card_com_calls[CARD_COM_QUADS][CARD_COM_DOUBLE] = &CardPlayQuads;
}

CardPlayResult CardPlay(Card *cards_0, int len_0,
						Card *cards_1, int len_1) {
	CardComResult r0 = CardCombine(cards_0, len_0);
	CardComResult r1 = CardCombine(cards_1, len_1);

	CardCompareCall call = card_com_calls[r0.com][r1.com];
	if (!call) {
		CardPlayResult result = {CARD_PLAY_ERR};
		return result;
	}

	return (*call)(r0, r1);
}

inline Card CardBuild(CardSuit suit, CardKey value) {
	return value * 4 + suit;
}

/// NOTE: Remember to free when done using
char *CardString(Card card) {
	CardSuit suit = GetCardSuit(card);
	CardKey value = GetCardKey(card);

	const char *suit_string = CardSuitString[suit];
	const char *value_string = CardKeyString[value];

	char *result = (char *)Alloc(strlen(suit_string) + strlen(value_string) + 1);

	sprintf(result, "%s %s", suit_string, value_string);

	return result;
}

struct CardTable {
	bool initialized;

	int last_player_turn;
	int player_turn;
	int table_turn;
	int have_turn[4];

	int        player_cards_len[4];
	Card       player_cards[4][13];
	GameEntity *player_entities[4][13];

	int table_last_cards[13];
	int dumping_cards[52];

	GameEntity *drop_area;
	GameText   drop_text;

	int        grab_cards_len;
	Card       grab_cards[13];
	GameEntity *grab_entities[13];
};

#define CardBackImg(n) "image/52cards/card-back" #n ".png"
#define CardPng(suit, value) "image/52cards/card-" suit "-" value ".png"
#define CardPng4(value) CardPng("hearts", #value),		\
		CardPng("diamonds", #value),					\
		CardPng("clubs", #value),						\
		CardPng("spades", #value)

const char *CardImgFile[] = {
	CardPng4(2),
	CardPng4(1),
	CardPng4(13),
	CardPng4(12),
	CardPng4(11),
	CardPng4(10),
	CardPng4(9),
	CardPng4(8),
	CardPng4(7),
	CardPng4(6),
	CardPng4(5),
	CardPng4(4),
	CardPng4(3),
};

GLOBAL SDL_Texture *card52_images[52];
GLOBAL SDL_Texture *card52_back_images[4];
GLOBAL int32_t card_width = 96;
GLOBAL int32_t card_height = 144;
void CardTextureInit(SDL_Renderer *renderer) {
	for (int i = 0; i < 52; ++i) {
		card52_images[i] = IMG_LoadTexture(renderer, CardImgFile[i]);
		Assert(card52_images[i]);
	}
	card52_back_images[0] = IMG_LoadTexture(renderer, CardBackImg(1));
	card52_back_images[1] = IMG_LoadTexture(renderer, CardBackImg(2));
	card52_back_images[2] = IMG_LoadTexture(renderer, CardBackImg(3));
	card52_back_images[3] = IMG_LoadTexture(renderer, CardBackImg(4));
}

/// NOTE(): Texture and Table init
void CardTableInit(CardTable *table, GameEntityPool *entity_pool) {
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

			GameEntity *new_entity = GameEntityPoolGet(entity_pool, -1);

			new_entity->x = 0;
			new_entity->y = 0;
			new_entity->w = card_width;
			new_entity->h = card_height;

			if (i == 0) {
				new_entity->texture = card52_images[card];
			} else {
				new_entity->texture = card52_back_images[0];
			}

			table->player_entities[i][j] = new_entity;
		}
	}

	GameEntity *drop_area = GameEntityPoolGet(entity_pool, -1);

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

	table->initialized = true;
}

void CardInit(CardTable *table, GameEntityPool *entity_pool, SDL_Renderer *renderer) {
	if (!card52_images[0]) {
		CardTextureInit(renderer);
	}

	CardComCallsInit();
	GameEntityPoolResize(entity_pool, 53);
	CardTableInit(table, entity_pool);
}

int CardListIndex(Card *cards, int len, Card card) {
	ForRange (i, 0, len) {
		if (cards[i] == card) {
			return i;
		}
	}
	return -1;
}

bool CardListContains(Card *cards, int len, Card card) {
	ForRange (i, 0, len) {
		if (cards[i] == card) {
			return true;
		}
	}
	return false;
}

void CardListRemoveIndex(Card *cards, GameEntity **entities, int *len, int idx) {
	if (idx >= 0) {
		memmove(cards + idx, cards + idx + 1, (((*len) - idx - 1) * sizeof(Card)));
		memmove(entities + idx, entities + idx + 1, (((*len) - idx - 1) * sizeof(GameEntity *)));
	}
	(*len)--;
}

void CardListRemove(Card *cards, GameEntity **entities, int *len, Card card) {
	int idx = CardListIndex(cards, *len, card);
	return CardListRemoveIndex(cards, entities, len, idx);
}

void CardRemove(CardTable *table, int player, Card card) {
	int *len = &table->player_cards_len[player];
	Card *cards = table->player_cards[player];
	GameEntity **entities = table->player_entities[player];

	int remove_idx = CardListIndex(cards, *len, card);

	if (remove_idx == -1) {
		return;
	}

	CardListRemoveIndex(cards, entities, len, remove_idx);
}

void CardTableTryRelaseGrab(CardTable *table) {
	int len = table->grab_cards_len;
	if (len > 0) {
		GameEntity *entity = table->grab_entities[len-1];
		bool in_drop_area = GameEntityCollsionChecking(entity, table->drop_area);
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
		GameEntity *entity = table->grab_entities[i];
		entity->is_grabbed = false;
	}

	table->grab_cards_len = 0;
}

void CardTableTryGrab(CardTable *table,
					  GameInput *input,
					  int hover_i,
					  int hover_j) {
	if (hover_i == 0) {
		Card card = table->player_cards[hover_i][hover_j];
		GameEntity *entity = table->player_entities[hover_i][hover_j];
		if (!CardListContains(table->grab_cards, table->grab_cards_len, card)) {
			entity->mouse_rel_x = input->mouse.x - entity->x;
			entity->mouse_rel_y = input->mouse.y - entity->y;
			entity->is_grabbed = true;
			table->grab_cards[table->grab_cards_len] = card;
			table->grab_entities[table->grab_cards_len] = entity;
			table->grab_cards_len++;
		}
	}

	int len = table->grab_cards_len;
	ForRange (i, 0, len) {
		GameEntity *entity = table->grab_entities[i];
		entity->x = input->mouse.x - entity->mouse_rel_x;
		entity->y = input->mouse.y - entity->mouse_rel_y;
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
			GameEntity *entity = table->player_entities[0][i];
			if (!entity->is_grabbed) {
				entity->x = x;
				entity->y = y;
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
			GameEntity *entity = table->player_entities[1][i];
			entity->x = x;
			entity->y = y;
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
			GameEntity *entity = table->player_entities[2][i];
			entity->x = x;
			entity->y = y;
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
			GameEntity *entity = table->player_entities[3][i];
			entity->x = x;
			entity->y = y;
			y = y + card_height / 4;
		}
	}

	int hover_i = -1;
	int hover_j = -1;

	ForRange (i, 0, 4) {
		int len = table->player_cards_len[i];
		if (len == 1) {
			GameEntity *entity = table->player_entities[i][0];
			if (GameEntityMouseIn(entity, &input->mouse)) {
				hover_i = i;
				hover_j = 0;
				continue;
			}
		}

		ForRange (j, 0, len - 1) {
			GameEntity *entity_0 = table->player_entities[i][j];
			GameEntity *entity_1 = table->player_entities[i][j+1];
			bool in0 = GameEntityMouseIn(entity_0, mouse);
			bool in1 = GameEntityMouseIn(entity_1, mouse);
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
			GameEntity *entity = table->player_entities[i][j];
			if (GameEntityCollsionChecking(entity, table->drop_area)) {
				drop_i = i;
				drop_j = j;
			}
		}
	}
	GameEntityRender(table->drop_area, renderer, (drop_i != -1));

	/// NOTE(): Render
	ForRange (i, 0, 4) {
		int len = table->player_cards_len[i];
		ForRange (j, 0, len) {
			GameEntity *entity = table->player_entities[i][j];
			bool hl = ((i == hover_i) && (j == hover_j)) || ((i == drop_i) && (j == drop_j));

			if (i != 0) {
				LOCAL_PERSIST int back_idx = 0;
				back_idx = (back_idx + 1) % 8000;
				entity->texture = card52_back_images[back_idx / 2000];
			}

			GameEntityRender(entity, renderer, hl);
		}

		GameTextRender(&table->drop_text, renderer, 0xffffff);
	}
}
