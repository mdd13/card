#pragma once
/// TODO: Use header file instead
#include "common.cpp"

#include "game_mem.cpp"
#include "game_text.cpp"
#include "game_input.cpp"
#include "game_image.cpp"

#include "ui_layout.cpp"

#include "random.cpp"
#include "52card.cpp"

#include <string.h>

/// Các loại bài, theo thứ tự:
///
/// - Cơ, Rô, Chuồn, Bích
GLOBAL i32 card_suit_rank[] = {
	[CARD_HEART] = 0,
	[CARD_DIAMOND] = 1,
	[CARD_CLUB] = 2,
	[CARD_SPADE] = 3,
};

/// Giá trị trên các quân bài
GLOBAL i32 card_key_rank[] = {
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

i32 GetCardRank(Card card) {
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
	i32      value;
	i32      len;
};


bool CardsIsSequence(Card *cards, i32 len) {
	if (GetCardKey(cards[0]) == CARD_2) {
		return false;
	}

	for (i32 i = 1; i < len; ++i) {
		CardKey v0 = GetCardKey(cards[i - 1]);
		CardKey v1 = GetCardKey(cards[i]);
		if (v0 + 1 != v1) {
			return false;
		}
	}
	return true;
}

bool CardsIsDoubleSequence(Card *cards, i32 len) {
	if (len < 6) {
		return false;
	}

	if (GetCardKey(cards[0]) == CARD_2) {
		return false;
	}

	for (i32 i = 2; i < len; i += 2) {
		CardKey v0 = GetCardKey(cards[i - 2]);
		CardKey v1 = GetCardKey(cards[i - 1]);
		CardKey v2 = GetCardKey(cards[i]);
		if (v0 != v1 || v1 + 1 != v2) {
			return false;
		}
	}

	return true;
}

CardComResult CardCombine(Card *cards, i32 len) {
	CardComResult result = {};

	if (len <= 0) {
		goto error_handle;
	}
	
	if (len == 1) {
		result.com = CARD_COM_SINGLE;
		result.value = cards[0];
		ReturnDefer(result);
	}

	for (i32 i = 0; i < len - 1; ++i) {
		i32 mn_rank = GetCardRank(cards[i]);
		i32 mn_idx = i;

		for (i32 j = i + 1; j < len; ++j) {
			i32 rank = GetCardRank(cards[j]);
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
		i32 sum = cards[0];
		for (i32 i = 1; i < len; ++i) {
			sum += GetCardKey(cards[i]) * 4;
		}
		result.com = CARD_COM_SEQUENCE;
		result.value = sum;
		ReturnDefer(result);
	}

	if (CardsIsDoubleSequence(cards, len)) {
		i32 sum = cards[0];
		for (i32 i = 1; i < len; ++i) {
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
	for (i32 i = 0; i < CARD_COM_TOTAL; ++i) {
		for (i32 j = 0; j < CARD_COM_TOTAL; ++j) {
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

CardPlayResult CardPlay(Card *cards_0, i32 len_0,
						Card *cards_1, i32 len_1) {
	CardComResult r0 = CardCombine(cards_0, len_0);
	CardComResult r1 = CardCombine(cards_1, len_1);

	CardCompareCall call = card_com_calls[r0.com][r1.com];
	if (!call) {
		CardPlayResult result = {CARD_PLAY_ERR};
		return result;
	}

	return (*call)(r0, r1);
}
