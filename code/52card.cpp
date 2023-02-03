#pragma once

enum CardSuit {
	CARD_HEART,
	CARD_DIAMOND,
	CARD_CLUB,
	CARD_SPADE,
	CARD_SUIT_TOTAL,
};

enum CardKey {
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
	CARD_2,
	CARD_KEY_TOTAL,
};

const char *CardSuitString[] = {
	"CARD_HEART",
	"CARD_DIAMOND",
	"CARD_CLUB",
	"CARD_SPADE",
};

const char *CardKeyString[] = {
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
	"CARD_2",	
};

typedef int Card;

inline CardSuit GetCardSuit(Card card) {
	return (CardSuit)(card % 4);
}

inline CardKey GetCardKey(Card card) {
	return (CardKey)(card / 4);
}
