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

typedef i32 Card;

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
GLOBAL i32 card_width = 96;
GLOBAL i32 card_height = 144;
void CardImagesInit() {
	if (card52_image_initialized) {
		return;
	}
	
	for (i32 i = 0; i < 52; ++i) {
		GameImageLoadFile(&card52_images[i], CardImgFile[i]);
	}
	GameImageLoadFile(&card52_back_images[0], CardBackImg(1));
	GameImageLoadFile(&card52_back_images[1], CardBackImg(2));
	GameImageLoadFile(&card52_back_images[2], CardBackImg(3));
	GameImageLoadFile(&card52_back_images[3], CardBackImg(4));

	card52_image_initialized = true;
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

	char *result = (char *)GameMemAlloc(strlen(suit_string) + strlen(value_string) + 1);

	sprintf(result, "%s %s", suit_string, value_string);
	return result;
}

i32 CardListIndex(Card *cards, i32 len, Card card) {
	ForRange (i, 0, len) {
		if (cards[i] == card) {
			return i;
		}
	}
	return -1;
}

bool CardListContains(Card *cards, i32 len, Card card) {
	ForRange (i, 0, len) {
		if (cards[i] == card) {
			return true;
		}
	}
	return false;
}

void CardListRemoveIndex(Card *cards, UiLayout *layouts, i32 *len, i32 idx) {
	if (idx >= 0) {
		memmove(cards + idx, cards + idx + 1, (((*len) - idx - 1) * sizeof(Card)));
		memmove(layouts + idx, layouts + idx + 1, (((*len) - idx - 1) * sizeof(UiLayout)));
	}
	(*len)--;
}

void CardListRemove(Card *cards, UiLayout *layouts, i32 *len, Card card) {
	i32 idx = CardListIndex(cards, *len, card);
	return CardListRemoveIndex(cards, layouts, len, idx);
}

void CardDraw(UiLayout *layout, Card card) {
	UiLayoutDrawImage(layout, &card52_images[card]);
}

void CardBackDraw(UiLayout *layout) {
	// LOCAL_PERSIST i32 t = 0;
	// UiLayoutDrawImage(layout, &card52_back_images[t / 1000]);
	// t = (t + 1) % 4000;
	UiLayoutDrawImage(layout, &card52_back_images[0]);
}
