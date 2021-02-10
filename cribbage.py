#!/usr/bin/env python3
# Copyright (c) 2020, Michael Cook <michael@waxrat.com>. All rights reserved.

#
# Analyze cribbage hands.
#
# Given a cribbage hand (six cards), which two cards should you discard
# to the crib to maximize your chances of getting the best score?
#

import sys
import math

suits = 'HCDS'

rank_to_value = {
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

rank_to_order = {
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

    def __init__(self, rank, suit):
        assert rank in rank_to_value
        assert suit in suits
        self.rank = rank
        self.suit = suit

    def __eq__(self, other):
        return self.rank == other.rank and self.suit == other.suit

    def __str__(self):
        return self.rank + self.suit

    def order(self):
        return rank_to_order[self.rank]

class Hand:

    def __init__(self):
        self.cards = []

    def value(self, i):
        r = self.cards[i].rank
        return rank_to_value[r]

    def size(self):
        return len(self.cards)

    def push(self, card):
        assert card not in self.cards
        self.cards.append(card)

    def pop(self):
        return self.cards.pop()

    def has(self, card):
        return card in self.cards

    def __str__(self):
        return ' '.join(map(str, self.cards))

    def copy(self):
        h = Hand()
        h.cards = self.cards[:]
        return h

def make_hand(s):
    hand = Hand()
    rank = None

    for c in s.upper():
        if c in suits:
            if rank is None:
                raise Exception(f'Malformed hand: {s}')
            hand.push(Card(rank, c))
            rank = None
            continue

        if c in rank_to_value:
            if rank is not None:
                raise Exception(f'Malformed hand: {s}')
            rank = c
            continue

        if c != ' ' and c != '-':
            raise Exception(f'Malformed hand: {s}')

    if rank is not None:
        raise Exception(f'Malformed hand: {s}')

    return hand

assert '5H 5C 5S JD 5D' == str(make_hand('5H 5C 5S JD 5D'))
assert '5H 5C 5S JD 5D' == str(make_hand('5h5c5sjd5d'))
assert 'AH AS JH AC AD' == str(make_hand('ah-as-jh-ac-ad'))

def score_15s(hand):
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

def score_pairs(hand):
    num_pairs = 0
    for ai, a in enumerate(hand.cards):
        for b in hand.cards[ai+1:]:
            if a.rank == b.rank:
                num_pairs += 1
    return 2 * num_pairs

assert 12 == score_pairs(make_hand('5H 5S 5C 5D TH'))
assert 8 == score_pairs(make_hand('TS 5S 5C 5D TH'))
assert 4 == score_pairs(make_hand('6C 6D 4D 4S 5D'))

X = -1 # match any rank
run_patterns = (
    # (score, (deltas...)),
    (12, (0, 1, 1, 0)), # AA233
    ( 9, (1, 1, 0, 0)), # A2333
    ( 9, (1, 0, 0, 1)), # A2223
    ( 9, (0, 0, 1, 1)), # AAA23
    ( 8, (1, 1, 1, 0)), # A2344
    ( 8, (1, 1, 0, 1)), # A2334
    ( 8, (1, 0, 1, 1)), # A2234
    ( 8, (0, 1, 1, 1)), # AA234
    ( 6, (X, 1, 1, 0)), # xA233
    ( 6, (X, 1, 0, 1)), # xA223
    ( 6, (X, 0, 1, 1)), # xAA23
    ( 6, (1, 1, 0, X)), # A233x
    ( 6, (1, 0, 1, X)), # A223x
    ( 6, (0, 1, 1, X)), # AA23x
    ( 5, (1, 1, 1, 1)), # A2345
    ( 4, (X, 1, 1, 1)), # xA234
    ( 4, (1, 1, 1, X)), # A234x
    ( 3, (X, X, 1, 1)), # xxA23
    ( 3, (X, 1, 1, X)), # xA23x
    ( 3, (1, 1, X, X)), # A23xx
)

def score_runs(hand):
    assert hand.size() == 5

    # Make a sorted sequence of the orders of the cards in the hand.  The
    # order of Ace is 1, Two is 2, ..., Ten is 10, Jack is 11, Queen is 12,
    # King is 13.
    orders = list(map(lambda card: card.order(), hand.cards))
    orders.sort()

    # Compare the sorted hand to the patterns.  Look at the difference between
    # the two cards in each pair of adjacent cards.  Stop at the first match.
    for score, deltas in run_patterns:
        previous = orders[0]
        j = 0
        while True:
            delta = deltas[j]
            order = orders[j + 1]
            if delta != X and delta != order - previous:
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

def score_flush(hand, is_crib):
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

def score_nobs(hand):
    # nobs: one point for the Jack of the same suit as the cut card
    assert hand.size() == 5
    cut_suit = hand.cards[4].suit
    for card in hand.cards[:4]:
        if card.rank == 'J' and card.suit == cut_suit:
            return 1
    return 0

assert 1 == score_nobs(make_hand('JH 2C 3C 4C 5H'))
assert 0 == score_nobs(make_hand('JH 2C 3C 4C 5C'))

def score_hand(hand, is_crib):
    if isinstance(hand, str):
        hand = make_hand(hand)
    return score_15s(hand) + \
        score_pairs(hand) + \
        score_runs(hand) + \
        score_flush(hand, is_crib) + \
        score_nobs(hand)

assert 12 == score_hand('AH AS JH AC AD', False) # 4oak ("of a kind")
assert 13 == score_hand('AH AS JD AC AD', False) # ...plus right jack
assert  5 == score_hand('AH 3H 7H TH JH', False) # 5 hearts
assert  5 == score_hand('AH 3H 7H TH JH', True)  # 5 hearts but crib
assert  4 == score_hand('AH 3H 7H TH JS', False) # 4 hearts
assert  0 == score_hand('AH 3H 7S TH JH', False) # 4 hearts but with cut
assert  0 == score_hand('AH 3H 7H TH JS', True)  # 4 hearts but crib
assert  7 == score_hand('AH 2S 3C 5D JH', False) # 15/4 + run/3
assert 20 == score_hand('7H 7S 7C 8D 8H', False) # 15/12 + 3oak + 2oak
assert 15 == score_hand('AH 2H 3H 3S 3D', False) # triple run/3
assert 15 == score_hand('3H AH 3S 2H 3D', False) # triple run/3
assert 29 == score_hand('5H 5C 5S JD 5D', False)
assert 28 == score_hand('5H 5C 5S 5D JD', False)
assert 24 == score_hand('6C 4D 6D 4S 5D', False)

# ---------------------------------------------------------------------------

# Iterate over all of the ways of choosing `num_choose` cards
# from the given hand `hand`.
# TODO: should simply replace this with itertools.combinations
def choose(hand, num_choose):
    chosen = Hand()
    i = 0
    i_stack = []

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

def make_deck(exclude):
    # Make an entire deck of cards but leave out any cards in `exclude`
    deck = Hand()
    for suit in suits:
        for rank in rank_to_value:
            card = Card(rank, suit)
            if not exclude.has(card):
                deck.push(card)
    return deck

max_score = 29 + 24  # 29 in hand, 24 in crib (44665)
min_score = -29      # 0 in hand, 29 in opp crib
num_scores = max_score - min_score + 1

class Tally:

    def __init__(self):
        self.scores = [0] * num_scores

    def count(self, score):
        self.scores[score - min_score] += 1

class Statistics:

    def __init__(self, mean, stdev, mins, maxs):
        self.mean = mean
        self.stdev = stdev
        self.min = mins
        self.max = maxs

    def __str__(self):
        return f'{self.mean:.1f} {self.stdev:.1f} {self.min}..{self.max}'

def make_statistics(tally, num_hands):
    # Convert `tally` to a Statistics object.  `tally` is the number of times
    # each score min_score..max_score was achieved over `num_hands` hands.
    mins = 0
    for score in range(min_score, max_score + 1):
        if tally.scores[score - min_score] != 0:
            mins = score
            break

    maxs = 0
    for score in range(max_score, min_score - 1, -1):
        if tally.scores[score - min_score] != 0:
            maxs = score
            break

    sums = 0.0
    for score in range(mins, maxs + 1):
        sums += score * tally.scores[score - min_score]
    mean = sums / num_hands

    sumdev = 0.0
    for score in range(mins, maxs + 1):
        d = score - mean
        sumdev += d * d
    stdev = math.sqrt(sumdev / float(num_hands))

    return Statistics(mean, stdev, mins, maxs)

def analyze_hand(hand):
    # Find all possible pairs of cards to discard to the crib.
    # There are C(6,2)=15 possible discards in a cribbage hand.

    for discarding in choose(hand, 2):
        hold = Hand()
        for i in range(hand.size()):
            card = hand.cards[i]
            if not discarding.has(card):
                hold.push(card)

        deck = make_deck(hand)
        assert len(deck.cards) == 46

        mine_tally = Tally()   # scores when the crib is mine
        theirs_tally = Tally() # scores then the crib is theirs
        num_hands = 0
        for chosen in choose(deck, 2):
            card1 = chosen.cards[0]
            card2 = chosen.cards[1]

            crib = discarding.copy()
            crib.push(card1)
            crib.push(card2)
            assert len(crib.cards) == 4

            for cut in deck.cards:
                if cut == card1 or cut == card2:
                    continue

                hold.push(cut)
                hold_score = score_hand(hold, False)
                hold.pop()

                crib.push(cut)
                crib_score = score_hand(crib, True)
                crib.pop()

                mine_score = hold_score + crib_score
                theirs_score = hold_score - crib_score

                num_hands += 1

                mine_tally.count(mine_score)
                theirs_tally.count(theirs_score)

        # deck size: 46, C(46,2)=1035
        # remaining_deck size: 44
        assert num_hands == 1035 * 44

        if_mine = make_statistics(mine_tally, num_hands)
        if_theirs = make_statistics(theirs_tally, num_hands)

        print(f'{discarding} [{if_mine}] [{if_theirs}]')

# ---------------------------------------------------------------------------

def main():
    for arg in sys.argv[1:]:
        hand = make_hand(arg)
        if hand.size() != 6:
            raise Exception(f'Wrong number of cards in hand: {hand}')

        print(f'[ {hand} ]')
        analyze_hand(hand)
        print()

main()
