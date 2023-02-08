#pragma once

#include "common.cpp"
#include "game_image.cpp"

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

GLOBAL bool card52_image_initialized;
GLOBAL GameImage card52_images[52];
GLOBAL GameImage card52_back_images[4];
GLOBAL int32_t card_width = 96;
GLOBAL int32_t card_height = 144;

void CardImagesInit() {
	if (card52_image_initialized) {
		return;
	}
	
	for (int i = 0; i < 52; ++i) {
		GameImageLoadFile(&card52_images[i], CardImgFile[i]);
	}
	GameImageLoadFile(&card52_back_images[0], CardBackImg(1));
	GameImageLoadFile(&card52_back_images[1], CardBackImg(2));
	GameImageLoadFile(&card52_back_images[2], CardBackImg(3));
	GameImageLoadFile(&card52_back_images[3], CardBackImg(4));

	card52_image_initialized = true;
}
