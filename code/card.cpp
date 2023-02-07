#pragma once
/// TODO: Use header file instead
#include "common.cpp"
#include "mem.cpp"

#include "game_text.cpp"
#include "game_input.cpp"
#include "game_layout.cpp"
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

void CardListRemoveIndex(Card *cards, GameLayout **entities, int *len, int idx) {
	if (idx >= 0) {
		memmove(cards + idx, cards + idx + 1, (((*len) - idx - 1) * sizeof(Card)));
		memmove(entities + idx, entities + idx + 1, (((*len) - idx - 1) * sizeof(GameLayout *)));
	}
	(*len)--;
}

void CardListRemove(Card *cards, GameLayout **entities, int *len, Card card) {
	int idx = CardListIndex(cards, *len, card);
	return CardListRemoveIndex(cards, entities, len, idx);
}
