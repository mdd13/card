#pragma once
/// TODO: Use header file instead
#include "common.h"
#include "mem.cpp"
#include "entity.cpp"
#include "game.cpp"
#include "random.cpp"

#include <string.h>

/// Các loại bài, theo thứ tự:
///
/// - Cơ, Rô, Chuồn, Bích
enum CardSuit {
	CARD_HEART,
	CARD_DIAMOND,
	CARD_CLUB,
	CARD_SPADE,
};

/// TODO: Use macro to generate strings from enum
const char *CardSuitString[] = {
	"CARD_HEART",
	"CARD_DIAMOND",
	"CARD_CLUB",
	"CARD_SPADE",
};

/// Giá trị trên các quân bài
enum CardValue {
	CARD_2,
	CARD_A,
	CARD_K,
	CARD_Q,
	CARD_J,
	CARD_10,
	CARD_9,
	CARD_8,
	CARD_7,
	CARD_6,
	CARD_5,
	CARD_4,
	CARD_3,
};

const char *CardValueString[] = {
	"CARD_2",
	"CARD_A",
	"CARD_K",
	"CARD_Q",
	"CARD_J",
	"CARD_10",
	"CARD_9",
	"CARD_8",
	"CARD_7",
	"CARD_6",
	"CARD_5",
	"CARD_4",
	"CARD_3",
};

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
};

#define CARD_COM_TOTAL 7

const char *CardComString[] {
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

typedef int Card;

inline CardSuit GetCardSuit(Card card) {
	return (CardSuit)(card % 4);
}

inline CardValue GetCardValue(Card card) {
	return (CardValue)(card / 4);
}

bool CardsIsSequence(Card *cards, int len) {
	if (GetCardValue(cards[0]) == CARD_2) {
		return false;
	}

	for (int i = 1; i < len; ++i) {
		CardValue v0 = GetCardValue(cards[i - 1]);
		CardValue v1 = GetCardValue(cards[i]);
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

	if (GetCardValue(cards[0]) == CARD_2) {
		return false;
	}

	bool is_sequence = true;
	for (int i = 2; i < len; i += 2) {
		CardValue v0 = GetCardValue(cards[i - 2]);
		CardValue v1 = GetCardValue(cards[i - 1]);
		CardValue v2 = GetCardValue(cards[i]);
		if (v0 != v1 || v1 + 1 != v2) {
			return false;
		}
	}

	return true;
}

CardComResult CardCombine(Card *cards, int len) {
	CardComResult result = {};

	if (len == 1) {
		CardValue value = GetCardValue(cards[0]);
		result.com = CARD_COM_SINGLE;
		result.value = cards[0];
		ReturnDefer(result);
	}

	for (int i = 0; i < len - 1; ++i) {
		Card mn_val = cards[i];
		int mn_idx = i;

		for (int j = i + 1; j < len; ++j) {
			if (mn_val > cards[j]) {
				mn_val = cards[j];
				mn_idx = j;
			}
		}

		if (mn_idx != i) {
			Swap(cards[i], cards[mn_idx]);
		}
	}

	if (len == 2) {
		if (GetCardValue(cards[0]) == GetCardValue(cards[1])) {
			CardValue value = GetCardValue(cards[0]);
			result.com = CARD_COM_DOUBLE;
			result.value = cards[0] + value * 4;
			ReturnDefer(result);
		}
		goto error_handle;
	}

	if (CardsIsSequence(cards, len)) {
		int sum = cards[0];
		for (int i = 1; i < len; ++i) {
			sum += GetCardValue(cards[i]) * 4;
		}
		result.com = CARD_COM_SEQUENCE;
		result.value = sum;
		ReturnDefer(result);
	}

	if (CardsIsDoubleSequence(cards, len)) {
		int sum = cards[0];
		for (int i = 1; i < len; ++i) {
			sum += GetCardValue(cards[i]) * 4;
		}
		result.com = CARD_COM_SEQUENCE_DOUBLE;
		result.value = sum;
		ReturnDefer(result);
	}

	if (len == 3) {
		if (GetCardValue(cards[0]) == GetCardValue(cards[1]) &&
			GetCardValue(cards[1]) == GetCardValue(cards[2])) {
			CardValue value = GetCardValue(cards[0]);
			result.com = CARD_COM_TRIPS;
			result.value = cards[0] +  value * 8;
			ReturnDefer(result);
		}
		goto error_handle;
	}

	if (len == 4) {
		if (GetCardValue(cards[0]) == GetCardValue(cards[1]) &&
			GetCardValue(cards[1]) == GetCardValue(cards[2]) &&
			GetCardValue(cards[2]) == GetCardValue(cards[3])) {
			CardValue value = GetCardValue(cards[0]);

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

const char *CardPlayStatusString[] {
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
		if (GetCardValue(r1.value) == CARD_2) {
			goto handle_ok;
		}
		goto handle_err;
	}

	if (r1.com == CARD_COM_DOUBLE) {
		if (GetCardValue(r1.value / r1.len) == CARD_2 && r0.len > 6) {
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
		if (GetCardValue(r1.value) == CARD_2) {
			goto handle_ok;
		}
		goto handle_err;
	}

	if (r1.com == CARD_COM_DOUBLE) {
		if (GetCardValue(r1.value / r1.len) == CARD_2) {
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
CardCompareCall card_com_matrix[CARD_COM_TOTAL][CARD_COM_TOTAL];

void InitCardComMatrix() {
	for (int i = 0; i < CARD_COM_TOTAL; ++i) {
		for (int j = 0; j < CARD_COM_TOTAL; ++j) {
			card_com_matrix[i][j] = 0;
		}
	}

	card_com_matrix[CARD_COM_SINGLE][CARD_COM_SINGLE] = &CardPlayValue;
	card_com_matrix[CARD_COM_DOUBLE][CARD_COM_DOUBLE] = &CardPlayValue;
	card_com_matrix[CARD_COM_TRIPS][CARD_COM_TRIPS] = &CardPlayValue;
	card_com_matrix[CARD_COM_QUADS][CARD_COM_QUADS] = &CardPlayValue;
	card_com_matrix[CARD_COM_SEQUENCE][CARD_COM_SEQUENCE] = &CardPlayValue;
	card_com_matrix[CARD_COM_SEQUENCE_DOUBLE][CARD_COM_SEQUENCE_DOUBLE] = &CardPlaySequenceDouble;

	card_com_matrix[CARD_COM_SEQUENCE_DOUBLE][CARD_COM_SINGLE] = &CardPlaySequenceDouble;
	card_com_matrix[CARD_COM_SEQUENCE_DOUBLE][CARD_COM_DOUBLE] = &CardPlaySequenceDouble;

	card_com_matrix[CARD_COM_QUADS][CARD_COM_SINGLE] = &CardPlayQuads;
	card_com_matrix[CARD_COM_QUADS][CARD_COM_DOUBLE] = &CardPlayQuads;
}

CardPlayResult CardPlay(Card *cards_0, int len_0,
						Card *cards_1, int len_1) {
	CardComResult r0 = CardCombine(cards_0, len_0);
	CardComResult r1 = CardCombine(cards_1, len_1);

	CardCompareCall call = card_com_matrix[r0.com][r1.com];
	if (!call) {
		CardPlayResult result = {CARD_PLAY_ERR};
		return result;
	}

	return (*call)(r0, r1);
}

inline Card CardBuild(CardSuit suit, CardValue value) {
	return value * 4 + suit;
}

/// NOTE: Remember to free when done using
char *CardString(Card card) {
	CardSuit suit = GetCardSuit(card);
	CardValue value = GetCardValue(card);

	const char *suit_string = CardSuitString[suit];
	const char *value_string = CardValueString[value];

	char *result = (char *)Alloc(strlen(suit_string) + strlen(value_string) + 1);

	sprintf(result, "%s %s", suit_string, value_string);

	return result;
}

struct CardTable {
	int last_player_turn;
	int player_turn;
	int table_turn;
	int have_turn[4];

	int    player_cards_len[4];
	Card   player_cards[4][13];
	Entity player_entities[4][13];

	int table_last_cards[13];
	int dumping_cards[52];
};

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

GLOBAL SDL_Texture *card_images[52];
GLOBAL int32_t card_width = 96;
GLOBAL int32_t card_height = 144;
void CardTextureInit(SDL_Renderer *renderer) {
	for (int i = 0; i < 52; ++i) {
		card_images[i] = IMG_LoadTexture(renderer, CardImgFile[i]);
		Assert(card_images[i]);
	}
}

/// NOTE(): Texture and Table init
void CardTableInit(CardTable *table) {
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

			Entity new_entity = {};
			new_entity.x = 0;
			new_entity.y = 0;
			new_entity.w = card_width;
			new_entity.h = card_height;

			new_entity.texture = card_images[card];

			table->player_entities[i][j] = new_entity;
		}
	}
}

void CardInit(SDL_Renderer *renderer, CardTable *table) {
	if (!card_images[0]) {
		CardTextureInit(renderer);
	}

	CardTableInit(table);
}

void CardTableUpdateAndRender(SDL_Renderer *renderer, GameInput *input, CardTable *table) {
	// Update player_0's hand
	{
		int hand_width = (table->player_cards_len[0] + 1) * (card_width / 2);
		int x = (window_width - hand_width) / 2;
		int y = window_height - card_height - 40;

		int len = table->player_cards_len[0];
		ForRange (i, 0, len) {
			Entity *entity = &table->player_entities[0][i];
			if (!entity->is_grabbed) {
				entity->x = x;
				entity->y = y;
			} else {
				EntityGrab(entity, input);
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
			Entity *entity = &table->player_entities[1][i];
			if (!entity->is_grabbed) {
				entity->x = x;
				entity->y = y;
			} else {
				EntityGrab(entity, input);
			}
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
			Entity *entity = &table->player_entities[2][i];
			if (!entity->is_grabbed) {
				entity->x = x;
				entity->y = y;
			} else {
				EntityGrab(entity, input);
			}
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
			Entity *entity = &table->player_entities[3][i];
			if (!entity->is_grabbed) {
				entity->x = x;
				entity->y = y;				
			} else {
				EntityGrab(entity, input);
			}
			y = y + card_height / 4;
		}
	}

	int hover_i = -1;
	int hover_j = -1;

	ForRange (i, 0, 4) {
		int len = table->player_cards_len[i];
		ForRange (j, 0, len - 1) {
			Entity *entity_0 = &table->player_entities[i][j];
			Entity *entity_1 = &table->player_entities[i][j+1];
			bool in0 = EntityMouseIn(entity_0, &input->mouse);
			bool in1 = EntityMouseIn(entity_1, &input->mouse);
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

	if (hover_i >= 0) {
		Entity *entity = &table->player_entities[hover_i][hover_j];
		EntityGrab(entity, input);
	}

	/// NOTE(): Render
	ForRange (i, 0, 4) {
		int len = table->player_cards_len[i];
		ForRange (j, 0, len) {
			Entity *entity = &table->player_entities[i][j];
			EntityRender(entity, renderer, (i == hover_i) && (j == hover_j));
		}
	}
}
