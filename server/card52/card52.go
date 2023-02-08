package card52

type CardSuit int32

const (
	CARD_HEART CardSuit = iota
	CARD_DIAMOND
	CARD_CLUB
	CARD_SPADE
	CARD_SUIT_TOTAL
)

type CardKey int32

const (
	CARD_A CardKey = iota
	CARD_K
	CARD_Q
	CARD_J
	CARD_10
	CARD_9
	CARD_8
	CARD_7
	CARD_6
	CARD_5
	CARD_4
	CARD_3
	CARD_2
	CARD_KEY_TOTAL
)

type Card int32

func GetCardSuit(card Card) CardSuit {
	return CardSuit(card % 4)
}

func GetCardKey(card Card) CardKey {
	return CardKey(card / 4)
}

type CardGame interface {
	Submit(cards []Card) func
}
