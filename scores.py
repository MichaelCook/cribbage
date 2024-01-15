#!/usr/bin/env python
# Copyright (c) 2021, Michael Cook <michael@waxrat.com>. All rights reserved.

#
# There are just under 13 million possible cribbage hands
#
# This chart shows the distribution of the different scores.
# So, about 8% of the time you'll get a zero hand.
# About 43% of the time you'll get either 2 or 4 points.
# About 3 quarters of the time, you'll get either 0, 2, 4, 6 or 8 points.
#

from typing import Final
from collections.abc import Generator

SUITS: Final = 'HCDS'

RANK_TO_VALUE: Final = {
    'A': 1,
    '2': 2,
    '3': 3,
    '4': 4,
    '5': 5,
    '6': 6,
    '7': 7,
    '8': 8,
    '9': 9,
    'T': 10,
    'J': 10,
    'Q': 10,
    'K': 10,
}

RANK_TO_ORDER: Final = {
    'A': 1,
    '2': 2,
    '3': 3,
    '4': 4,
    '5': 5,
    '6': 6,
    '7': 7,
    '8': 8,
    '9': 9,
    'T': 10,
    'J': 11,
    'Q': 12,
    'K': 13,
}

class Card:

    def __init__(self, rank: str, suit: str) -> None:
        assert rank in RANK_TO_VALUE
        assert suit in SUITS
        self.rank = rank
        self.suit = suit

    def __eq__(self, other: 'Card') -> bool:  # type: ignore
        return self.rank == other.rank and self.suit == other.suit

    def __hash__(self) -> int:
        return hash((self.rank, self.suit))

    def __str__(self) -> str:
        return self.rank + self.suit

    def order(self) -> int:
        return RANK_TO_ORDER[self.rank]

class Hand:

    def __init__(self) -> None:
        self.cards: list[Card] = []

    def value(self, i: int) -> int:
        r = self.cards[i].rank
        return RANK_TO_VALUE[r]

    def size(self) -> int:
        return len(self.cards)

    def push(self, card: Card) -> None:
        assert card not in self.cards
        self.cards.append(card)

    def pop(self) -> Card:
        return self.cards.pop()

    def has(self, card: Card) -> bool:
        return card in self.cards

    def __str__(self) -> str:
        return ' '.join(str(card) for card in self.cards)

    def copy(self) -> 'Hand':
        h = Hand()
        h.cards = self.cards[:]
        return h

def make_hand(s: str) -> Hand:
    hand = Hand()
    rank = None

    for c in s.upper():
        if c in SUITS:
            if rank is None:
                raise RuntimeError(f'Malformed hand: {s}')
            hand.push(Card(rank, c))
            rank = None
            continue

        if c in RANK_TO_VALUE:
            if rank is not None:
                raise RuntimeError(f'Malformed hand: {s}')
            rank = c
            continue

        if c not in ' -':
            raise RuntimeError(f'Malformed hand: {s}')

    if rank is not None:
        raise RuntimeError(f'Malformed hand: {s}')

    return hand

# pylint: disable=misplaced-comparison-constant
assert '5H 5C 5S JD 5D' == str(make_hand('5H 5C 5S JD 5D'))
assert '5H 5C 5S JD 5D' == str(make_hand('5h5c5sjd5d'))
assert 'AH AS JH AC AD' == str(make_hand('ah-as-jh-ac-ad'))

def score_15s(hand: Hand) -> int:
    assert hand.size() == 5

    a = hand.value(0)
    b = hand.value(1)
    c = hand.value(2)
    d = hand.value(3)
    e = hand.value(4)

    num_15s = 0

    # five cards - C(5,5)=1
    if a + b + c + d + e == 15:
        num_15s += 1

    # four cards - C(5,4)=5
    if a + b + c + d == 15:
        num_15s += 1
    if a + b + c + e == 15:
        num_15s += 1
    if a + b + d + e == 15:
        num_15s += 1
    if a + c + d + e == 15:
        num_15s += 1
    if b + c + d + e == 15:
        num_15s += 1

    # three cards - C(5,3)=10
    if a + b + c == 15:
        num_15s += 1
    if a + b + d == 15:
        num_15s += 1
    if a + b + e == 15:
        num_15s += 1
    if a + c + d == 15:
        num_15s += 1
    if a + c + e == 15:
        num_15s += 1
    if a + d + e == 15:
        num_15s += 1
    if b + c + d == 15:
        num_15s += 1
    if b + c + e == 15:
        num_15s += 1
    if b + d + e == 15:
        num_15s += 1
    if c + d + e == 15:
        num_15s += 1

    # two cards - C(5,2)=10
    if a + b == 15:
        num_15s += 1
    if a + c == 15:
        num_15s += 1
    if a + d == 15:
        num_15s += 1
    if a + e == 15:
        num_15s += 1
    if b + c == 15:
        num_15s += 1
    if b + d == 15:
        num_15s += 1
    if b + e == 15:
        num_15s += 1
    if c + d == 15:
        num_15s += 1
    if c + e == 15:
        num_15s += 1
    if d + e == 15:
        num_15s += 1

    return 2 * num_15s

assert 4 == score_15s(make_hand('AH 2H 3H JH QH'))
assert 8 == score_15s(make_hand('5H 2H 3H JH QH'))
assert 16 == score_15s(make_hand('5H 5S 5C 5D TH'))
assert 8 == score_15s(make_hand('6C 6D 4D 4S 5D'))

def score_pairs(hand: Hand) -> int:
    num_pairs = 0
    for ai, a in enumerate(hand.cards):
        for b in hand.cards[ai + 1:]:
            if a.rank == b.rank:
                num_pairs += 1
    return 2 * num_pairs

assert 12 == score_pairs(make_hand('5H 5S 5C 5D TH'))
assert 8 == score_pairs(make_hand('TS 5S 5C 5D TH'))
assert 4 == score_pairs(make_hand('6C 6D 4D 4S 5D'))

X: Final = -1  # match any rank
RUN_PATTERNS: Final = (
    # (score, (deltas...)),
    (12, (0, 1, 1, 0)),  # AA233
    ( 9, (1, 1, 0, 0)),  # A2333    # noqa: E201 whitespace after '('
    ( 9, (1, 0, 0, 1)),  # A2223    # noqa: E201
    ( 9, (0, 0, 1, 1)),  # AAA23    # noqa: E201
    ( 8, (1, 1, 1, 0)),  # A2344    # noqa: E201
    ( 8, (1, 1, 0, 1)),  # A2334    # noqa: E201
    ( 8, (1, 0, 1, 1)),  # A2234    # noqa: E201
    ( 8, (0, 1, 1, 1)),  # AA234    # noqa: E201
    ( 6, (X, 1, 1, 0)),  # xA233    # noqa: E201
    ( 6, (X, 1, 0, 1)),  # xA223    # noqa: E201
    ( 6, (X, 0, 1, 1)),  # xAA23    # noqa: E201
    ( 6, (1, 1, 0, X)),  # A233x    # noqa: E201
    ( 6, (1, 0, 1, X)),  # A223x    # noqa: E201
    ( 6, (0, 1, 1, X)),  # AA23x    # noqa: E201
    ( 5, (1, 1, 1, 1)),  # A2345    # noqa: E201
    ( 4, (X, 1, 1, 1)),  # xA234    # noqa: E201
    ( 4, (1, 1, 1, X)),  # A234x    # noqa: E201
    ( 3, (X, X, 1, 1)),  # xxA23    # noqa: E201
    ( 3, (X, 1, 1, X)),  # xA23x    # noqa: E201
    ( 3, (1, 1, X, X)),  # A23xx    # noqa: E201
)

def score_runs(hand: Hand) -> int:
    assert hand.size() == 5

    # Make a sorted sequence of the orders of the cards in the hand.  The
    # order of Ace is 1, Two is 2, ..., Ten is 10, Jack is 11, Queen is 12,
    # King is 13.
    orders = list(card.order() for card in hand.cards)
    orders.sort()

    # Compare the sorted hand to the patterns.  Look at the difference between
    # the two cards in each pair of adjacent cards.  Stop at the first match.
    for score, deltas in RUN_PATTERNS:
        previous = orders[0]
        j = 0
        while True:
            delta = deltas[j]
            order = orders[j + 1]
            if delta not in (X, order - previous):
                break
            previous = order
            j += 1
            if j == 4:
                return score
    return 0

assert 9 == score_runs(make_hand('AH 2H 3H 3D 3C'))
assert 9 == score_runs(make_hand('KH KD KC JH QH'))  # same pattern A2333
assert 9 == score_runs(make_hand('AH 2H 2D 2C 3H'))
assert 9 == score_runs(make_hand('AH AD AC 2H 3H'))
assert 8 == score_runs(make_hand('AH 2H 3H 4H 4D'))
assert 8 == score_runs(make_hand('AH 2H 3H 3D 4H'))
assert 8 == score_runs(make_hand('AH 2H 2C 3H 4H'))
assert 8 == score_runs(make_hand('AS AH 2H 3H 4H'))
assert 6 == score_runs(make_hand('JH AH 2H 3D 3H'))
assert 6 == score_runs(make_hand('JH AH 2S 2H 3H'))
assert 6 == score_runs(make_hand('JH AH AS 2H 3H'))
assert 6 == score_runs(make_hand('AH 2H 3S 3H JH'))
assert 6 == score_runs(make_hand('AH 2H 2S 3H JH'))
assert 6 == score_runs(make_hand('AH AS 2H 3H JH'))
assert 5 == score_runs(make_hand('AH 2H 3H 4H 5H'))
assert 4 == score_runs(make_hand('JH AH 2H 3H 4H'))
assert 4 == score_runs(make_hand('AH 2H 3H 4H JH'))
assert 3 == score_runs(make_hand('JH QH AH 2H 3H'))
assert 3 == score_runs(make_hand('JH AH 2H 3H TH'))
assert 3 == score_runs(make_hand('AH 2H 3H JH TH'))
assert 0 == score_runs(make_hand('AH 8H 3H JH TH'))
assert 12 == score_runs(make_hand('6C 6D 4D 4S 5D'))

def score_flush(hand: Hand, is_crib: bool) -> int:
    assert hand.size() == 5
    suit = hand.cards[0].suit
    for card in hand.cards[1:4]:
        if suit != card.suit:
            return 0
    # First 4 are the same suit, check the cut card
    if suit == hand.cards[4].suit:
        return 5
    # In the crib, a flush counts only if all five cards are the same suit
    if is_crib:
        return 0
    return 4

assert 5 == score_flush(make_hand('5H 6H 7H 8H 9H'), False)
assert 4 == score_flush(make_hand('5H 6H 7H 8H 9D'), False)
assert 0 == score_flush(make_hand('5H 6H 7H 8H 9D'), True)
assert 0 == score_flush(make_hand('5H 6H 7H 8D 9D'), False)

def score_nobs(hand: Hand) -> int:
    # nobs: one point for the Jack of the same suit as the cut card
    assert hand.size() == 5
    cut_suit = hand.cards[4].suit
    if any(card.rank == 'J' and card.suit == cut_suit for card in hand.cards[:4]):
        return 1
    return 0

assert 1 == score_nobs(make_hand('JH 2C 3C 4C 5H'))
assert 0 == score_nobs(make_hand('JH 2C 3C 4C 5C'))

def score_hand(hand: Hand | str, is_crib: bool) -> int:
    if isinstance(hand, str):
        hand = make_hand(hand)
    return score_15s(hand) + \
        score_pairs(hand) + \
        score_runs(hand) + \
        score_flush(hand, is_crib) + \
        score_nobs(hand)

assert 12 == score_hand('AH AS JH AC AD', False)  # 4oak ("of a kind")
assert 13 == score_hand('AH AS JD AC AD', False)  # ...plus right jack
assert 5 == score_hand('AH 3H 7H TH JH', False)   # 5 hearts
assert 5 == score_hand('AH 3H 7H TH JH', True)    # 5 hearts but crib
assert 4 == score_hand('AH 3H 7H TH JS', False)   # 4 hearts
assert 0 == score_hand('AH 3H 7S TH JH', False)   # 4 hearts but with cut
assert 0 == score_hand('AH 3H 7H TH JS', True)    # 4 hearts but crib
assert 7 == score_hand('AH 2S 3C 5D JH', False)   # 15/4 + run/3
assert 20 == score_hand('7H 7S 7C 8D 8H', False)  # 15/12 + 3oak + 2oak
assert 15 == score_hand('AH 2H 3H 3S 3D', False)  # triple run/3
assert 15 == score_hand('3H AH 3S 2H 3D', False)  # triple run/3
assert 29 == score_hand('5H 5C 5S JD 5D', False)
assert 28 == score_hand('5H 5C 5S 5D JD', False)
assert 24 == score_hand('6C 4D 6D 4S 5D', False)

# ---------------------------------------------------------------------------

# Iterate over all of the ways of choosing `num_choose` cards
# from the given hand `hand`.
# TODO: Should simply replace this with itertools.combinations
def choose(hand: Hand, num_choose: int) -> Generator[Hand, None, None]:
    chosen = Hand()
    i = 0
    i_stack: list[int] = []

    while True:
        if chosen.size() == num_choose:
            yield chosen
            chosen.pop()
            i = i_stack.pop() + 1
        elif i != hand.size():
            chosen.push(hand.cards[i])
            i_stack.append(i)
            i += 1
        elif i_stack:
            chosen.pop()
            i = i_stack.pop() + 1
        else:
            break

def make_deck() -> Hand:
    # Make an entire deck of cards
    deck = Hand()
    for suit in SUITS:
        for rank in RANK_TO_VALUE:
            card = Card(rank, suit)
            deck.push(card)
    return deck

def main() -> None:
    deck: Final = make_deck()
    scores = [0] * 30    # 0..29
    for hand in choose(deck, 4):
        for cut in deck.cards:
            if hand.has(cut):
                continue
            hand.push(cut)
            score = score_hand(hand, False)
            hand.pop()
            scores[score] += 1
    num_hands = sum(scores)
    for score, count in enumerate(scores):
        perc = count * 100 / num_hands
        hashmarks = '|' * round(4 * perc)
        print(f'{score:2} {count:8} {perc:6.2f}% {hashmarks}')
    print(f'   {num_hands:8} 100.00%')

if __name__ == '__main__':
    main()
