# Copyright (c) 2020, Michael Cook <michael@waxrat.com>. All rights reserved.

#
# Analyze cribbage hands.
#
# Given a cribbage hand (six cards), which two cards should you discard
# to the crib to maximize your chances of getting the best score?
#

import os
import strformat
import strutils
import sequtils
import algorithm
import math

const
  rankChars = "A23456789TJQK"
  suitChars = "HCDS"

type
  Rank {.pure.} = enum
    Ace = 1,
    Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten,
    Jack, Queen, King

  Suit {.pure.} = enum
    Heart, Club, Diamond, Spade

  Card = object
    rank: Rank
    suit: Suit

  Hand = object
    num_cards: int
    # [0..3] are the four cards dealt the hand after discard and [4] is the
    # cut card, or
    # [0..5] are the six cards dealt into the hand, or
    # [0..51] an entire deck.
    slots: array[52, Card]

proc cards(hand: Hand): seq[Card] =
  return hand.slots[0..<hand.num_cards]

proc `$`(r: Rank): char =
  return rankChars[ord(r) - 1]

proc `$`(s: Suit): char =
  return suitChars[ord(s)]

proc `$`(c: Card): string =
  return fmt"{c.rank}{c.suit}"

proc `$`(hand: Hand): string =
  return join(hand.cards, " ")

proc value(hand: Hand, i: int): int =
  let rank = hand.cards[i].rank
  case rank:
    of Ace..Nine:
      return ord(rank)
    else:
      # Ten..King are each worth 10
      return 10

proc has(hand: Hand, c: Card): bool =
  return contains(hand.cards, c)

proc push_back(hand: var Hand, c: Card) =
  if hand.has(c):
    raise newException(ValueError, "Duplicate card " & $c & " in hand " & $hand)
  if hand.num_cards == len(hand.slots):
    raise newException(ValueError, "Too many cards " & $c & " in hand " & $hand)
  hand.slots[hand.num_cards] = c
  inc hand.num_cards

proc pop_back(hand: var Hand) =
  assert hand.num_cards > 0
  dec hand.num_cards

proc make_hand(s: string): Hand =
  var
    hand: Hand
    rank = -1
    i: int

  for c in toUpperAscii(s):
    i = suitChars.find(c)
    if i != -1:
      if rank == -1:
        raise newException(ValueError, "Malformed hand: " & s)
      hand.push_back(Card(rank: Rank(rank), suit: Suit(i)))
      rank = -1
      continue

    i = rankChars.find(c)
    if i != -1:
      if rank != -1:
        raise newException(ValueError, "Malformed hand: " & s)
      rank = i + 1
      continue

    if c != ' ' and c != '-':
        raise newException(ValueError, "Malformed hand: " & s)

  if rank != -1:
    raise newException(ValueError, "Malformed hand: " & s)

  return hand

assert "5H 5C 5S JD 5D" == $make_hand("5H 5C-5S-JD 5D")
assert "5H 5C 5S JD 5D" == $make_hand("5h5c5sjd5d")
assert "AH AS JH AC AD" == $make_hand("ah as jh ac ad")

proc score_fifteens(hand: Hand): int =
  let
    a = hand.value(0)
    b = hand.value(1)
    c = hand.value(2)
    d = hand.value(3)
    e = hand.value(4)

  var
    num_15s = 0

  # five cards - C(5,5)=1
  if a + b + c + d + e == 15:
    inc num_15s

  # four cards - C(5,4)=5
  if a + b + c + d == 15:
    inc num_15s
  if a + b + c + e == 15:
    inc num_15s
  if a + b + d + e == 15:
    inc num_15s
  if a + c + d + e == 15:
    inc num_15s
  if b + c + d + e == 15:
    inc num_15s

  # three cards - C(5,3)=10
  if a + b + c == 15:
    inc num_15s
  if a + b + d == 15:
    inc num_15s
  if a + b + e == 15:
    inc num_15s
  if a + c + d == 15:
    inc num_15s
  if a + c + e == 15:
    inc num_15s
  if a + d + e == 15:
    inc num_15s
  if b + c + d == 15:
    inc num_15s
  if b + c + e == 15:
    inc num_15s
  if b + d + e == 15:
    inc num_15s
  if c + d + e == 15:
    inc num_15s

  # two cards - C(5,2)=10
  if a + b == 15:
    inc num_15s
  if a + c == 15:
    inc num_15s
  if a + d == 15:
    inc num_15s
  if a + e == 15:
    inc num_15s
  if b + c == 15:
    inc num_15s
  if b + d == 15:
    inc num_15s
  if b + e == 15:
    inc num_15s
  if c + d == 15:
    inc num_15s
  if c + e == 15:
    inc num_15s
  if d + e == 15:
    inc num_15s

  return 2 * num_15s

assert 4 == score_fifteens(make_hand("AH 2H 3H JH QH"))
assert 8 == score_fifteens(make_hand("5H 2H 3H JH QH"))
assert 16 == score_fifteens(make_hand("5H 5S 5C 5D TH"))

proc score_pairs(hand: Hand): int =
  var
    num_pairs = 0

  for ai, a in hand.cards[0..<hand.num_cards].pairs:
    for b in hand.cards[ai+1..<hand.num_cards]:
      if a.rank == b.rank:
        inc num_pairs

  return 2 * num_pairs

assert 12 == score_pairs(make_hand("5H 5S 5C 5D TH"))
assert 8 == score_pairs(make_hand("TS 5S 5C 5D TH"))

proc score_runs(hand: Hand): int =
  assert hand.num_cards == 5

  # Make an ordered sequence of the orders of the cards in the hand.
  # The order of Ace is 1, Two is 2, ..., Ten is 10, Jack is 11,
  # Queen is 12, # King is 13.
  var orders = map(hand.cards, proc (card: Card): int =
                                 ord(card.rank))
  sort orders

  type
    Pattern = object
      score: int
      delta: array[4, int]

  let X = -1 # match any rank
  let patterns = [
    Pattern(score: 9, delta: [ 1, 1, 0, 0 ]), # A2333
    Pattern(score: 9, delta: [ 1, 0, 0, 1 ]), # A2223
    Pattern(score: 9, delta: [ 0, 0, 1, 1 ]), # AAA23
    Pattern(score: 8, delta: [ 1, 1, 1, 0 ]), # A2344
    Pattern(score: 8, delta: [ 1, 1, 0, 1 ]), # A2334
    Pattern(score: 8, delta: [ 1, 0, 1, 1 ]), # A2234
    Pattern(score: 8, delta: [ 0, 1, 1, 1 ]), # AA234
    Pattern(score: 6, delta: [ X, 1, 1, 0 ]), # xA233
    Pattern(score: 6, delta: [ X, 1, 0, 1 ]), # xA223
    Pattern(score: 6, delta: [ X, 0, 1, 1 ]), # xAA23
    Pattern(score: 6, delta: [ 1, 1, 0, X ]), # A233x
    Pattern(score: 6, delta: [ 1, 0, 1, X ]), # A223x
    Pattern(score: 6, delta: [ 0, 1, 1, X ]), # AA23x
    Pattern(score: 5, delta: [ 1, 1, 1, 1 ]), # A2345
    Pattern(score: 4, delta: [ X, 1, 1, 1 ]), # xA234
    Pattern(score: 4, delta: [ 1, 1, 1, X ]), # A234x
    Pattern(score: 3, delta: [ X, X, 1, 1 ]), # xxA23
    Pattern(score: 3, delta: [ X, 1, 1, X ]), # xA23x
    Pattern(score: 3, delta: [ 1, 1, X, X ]), # A23xx
  ]

  # Compare the sorted hand to the patterns.  Look at the difference between
  # the two cards in each pair of adjacent cards.  Stop at the first match.
  for pattern in patterns:
    var
      previous = orders[0]
      j = 0
    while true:
      let
        delta = pattern.delta[j]
        order = orders[j + 1]
      if delta != X and delta != order - previous:
        break
      previous = order
      inc j
      if j == 4:
        return pattern.score;

  return 0

assert 9 == score_runs(make_hand("AH 2H 3H 3D 3C"))
assert 9 == score_runs(make_hand("KH KD KC JH QH"))  # same pattern
assert 9 == score_runs(make_hand("AH 2H 2D 2C 3H"))
assert 9 == score_runs(make_hand("AH AD AC 2H 3H"))
assert 8 == score_runs(make_hand("AH 2H 3H 4H 4D"))
assert 8 == score_runs(make_hand("AH 2H 3H 3D 4H"))
assert 8 == score_runs(make_hand("AH 2H 2C 3H 4H"))
assert 8 == score_runs(make_hand("AS AH 2H 3H 4H"))
assert 6 == score_runs(make_hand("JH AH 2H 3D 3H"))
assert 6 == score_runs(make_hand("JH AH 2S 2H 3H"))
assert 6 == score_runs(make_hand("JH AH AS 2H 3H"))
assert 6 == score_runs(make_hand("AH 2H 3S 3H JH"))
assert 6 == score_runs(make_hand("AH 2H 2S 3H JH"))
assert 6 == score_runs(make_hand("AH AS 2H 3H JH"))
assert 5 == score_runs(make_hand("AH 2H 3H 4H 5H"))
assert 4 == score_runs(make_hand("JH AH 2H 3H 4H"))
assert 4 == score_runs(make_hand("AH 2H 3H 4H JH"))
assert 3 == score_runs(make_hand("JH QH AH 2H 3H"))
assert 3 == score_runs(make_hand("JH AH 2H 3H TH"))
assert 3 == score_runs(make_hand("AH 2H 3H JH TH"))
assert 0 == score_runs(make_hand("AH 8H 3H JH TH"))

proc score_flush(hand: Hand, is_crib: bool): int =
  assert hand.num_cards == 5
  let suit = hand.cards[0].suit
  for card in hand.cards[1..3]:
    if suit != card.suit:
      return 0
  # First 4 are the same suit, check the cut card
  if suit == hand.cards[4].suit:
    return 5
  # In the crib, a flush counts only if all five cards are the same suit
  if is_crib:
    return 0
  return 4

assert 5 == score_flush(make_hand("5H 6H 7H 8H 9H"), false)
assert 4 == score_flush(make_hand("5H 6H 7H 8H 9D"), false)
assert 0 == score_flush(make_hand("5H 6H 7H 8H 9D"), true)
assert 0 == score_flush(make_hand("5H 6H 7H 8D 9D"), false)

proc score_right_jack(hand: Hand): int =
  assert hand.num_cards == 5
  let cut_suit = hand.cards[4].suit
  for card in hand.cards[0..3]:
    if card.rank == Rank.Jack and card.suit == cut_suit:
      return 1
  return 0

assert 1 == score_right_jack(make_hand("JH 2C 3C 4C 5H"))
assert 0 == score_right_jack(make_hand("JH 2C 3C 4C 5C"))

proc score_hand(hand: Hand, is_crib: bool): int =
  return score_fifteens(hand) +
         score_pairs(hand) +
         score_runs(hand) +
         score_flush(hand, is_crib) +
         score_right_jack(hand)

proc score_hand(hand: string, is_crib: bool): int =
  return score_hand(make_hand(hand), is_crib)

assert 12 == score_hand("AH AS JH AC AD", false) # 4oak ("of a kind")
assert 13 == score_hand("AH AS JD AC AD", false) # ...plus right jack
assert 5 == score_hand("AH 3H 7H TH JH", false)  # 5 hearts
assert 5 == score_hand("AH 3H 7H TH JH", true)   # 5 hearts but crib
assert 4 == score_hand("AH 3H 7H TH JS", false)  # 4 hearts
assert 0 == score_hand("AH 3H 7S TH JH", false)  # 4 hearts but with cut
assert 0 == score_hand("AH 3H 7H TH JS", true)   # 4 hearts but crib
assert 4 + 3 == score_hand("AH 2S 3C 5D JH", false) # 15/4 + run/3
assert 12 + 6 + 2 == score_hand("7H 7S 7C 8D 8H", false) # 15/12 + 3oak + 2oak
assert 15 == score_hand("AH 2H 3H 3S 3D", false) # triple run/3
assert 15 == score_hand("3H AH 3S 2H 3D", false) # triple run/3
assert 29 == score_hand("5H 5C 5S JD 5D", false)
assert 28 == score_hand("5H 5C 5S 5D JD", false)

# ---------------------------------------------------------------------------

proc for_each_choice_helper(hand: Hand,
                            offset: int,
                            num_choose: int,
                            chosen: var Hand,
                            fun: proc(choice: Hand)): int =
  if chosen.num_cards == num_choose:
    fun(chosen)
    return 1

  var
    i = offset
    num_hands = 0
  while i != hand.num_cards:
    chosen.push_back(hand.cards[i])
    inc i
    num_hands += for_each_choice_helper(hand, i, num_choose, chosen, fun)
    chosen.pop_back()
  return num_hands

proc for_each_choice(hand: Hand, num_choose: int,
                     fun: proc(choice: Hand)): int =
  var chosen: Hand
  return for_each_choice_helper(hand, 0, num_choose, chosen, fun)

#block:
#  var deck: Hand
#  deck.push_back(Card(rank: Rank.Ace, suit: Suit.Heart))
#  deck.push_back(Card(rank: Rank.Two, suit: Suit.Heart))
#  deck.push_back(Card(rank: Rank.Three, suit: Suit.Heart))
#  deck.push_back(Card(rank: Rank.Four, suit: Suit.Heart))
#  deck.push_back(Card(rank: Rank.Five, suit: Suit.Heart))
#  echo "deck: ", deck
#  echo for_each_choice(deck, 3, proc (choice: Hand) =
#                                  echo "> ", choice)
#  echo "--done--"
#  echo for_each_choice(deck, 2, proc (choice: Hand) =
#                                  echo "> ", choice)
#  echo "--done--"
#  echo for_each_choice(deck, 1, proc (choice: Hand) =
#                                  echo "> ", choice)
#  echo "--done--"
#  echo for_each_choice(deck, 4, proc (choice: Hand) =
#                                  echo "> ", choice)
#  echo "--done--"
#  echo for_each_choice(deck, 5, proc (choice: Hand) =
#                                  echo "> ", choice)

proc make_deck(exclude: Hand): Hand =
  var deck: Hand
  for suit in Suit:
    for rank in Rank:
      let card = Card(rank: rank, suit: suit)
      if not exclude.has(card):
        deck.push_back(card)
  return deck

const max_score = 29

type
  Tally = object
    scores: array[max_score + 1, int]

  Statistics = object
    mean, stdev: float
    min, max: int

proc `$`(st: Statistics): string =
  return fmt"{st.mean:.1f} {st.stdev:.1f} {st.min}..{st.max}"

proc make_statistics(t: Tally, num_hands: int): Statistics =
  var min = 0
  for score in 0..max_score:
    if t.scores[score] != 0:
      min = score
      break

  var max = 0
  for score in countdown(max_score, 0):
    if t.scores[score] != 0:
      max = score
      break

  var sum = 0.0
  for score in 0..max_score:
    sum += float(score * t.scores[score])
  let mean = sum / float(num_hands)

  var sumdev = 0.0
  for score in 0..max_score:
    let d = float(score) - mean
    sumdev += d * d * float(score)
  let stdev = sqrt(sumdev / float(num_hands))

  return Statistics(mean: mean, stdev: stdev, min: min, max: max)

proc analyze_hand(hand: Hand) =
  # Find all possible pairs of cards to discard to the crib.
  # There are C(6,2)=15 possible discards in a cribbage hand.

  discard for_each_choice(hand, 2, proc (discarding: Hand) =
    var keeping: Hand
    for i in 0 ..< hand.num_cards:
      let card = hand.cards[i]
      if not discarding.has(card):
        keeping.push_back(card)

    let deck = make_deck(hand)
    var hold_tally, crib_tally: Tally
    let num_hands = for_each_choice(deck, 3, proc (chosen: Hand) =
        let cut = chosen.cards[2]

        var hold = keeping
        hold.push_back(cut)

        var crib = discarding
        crib.push_back(chosen.cards[0])
        crib.push_back(chosen.cards[1])
        crib.push_back(cut)

        let hold_score = score_hand(hold, false)
        let crib_score = score_hand(crib, true)

        inc hold_tally.scores[hold_score]
        inc crib_tally.scores[crib_score]
    )
    assert num_hands == 15180 # sanity check, expecting C(46,3)

    # Calculate statistics (mean, standard deviation, min and max)
    # for both situations when it's my crib and when it's theirs.
    var sum, diff: Tally
    for score in 0..max_score:
      sum.scores[score] = hold_tally.scores[score] + crib_tally.scores[score]
      diff.scores[score] = hold_tally.scores[score] - crib_tally.scores[score]

    let
      if_mine = make_statistics(sum, num_hands)
      if_theirs = make_statistics(diff, num_hands)

    echo fmt"{if_mine.mean:6.2f} {if_theirs.mean:6.2f} ",
         fmt"Discard {discarding}, ",
         fmt"average score {if_mine.mean:.1f} (if your crib), ",
         fmt"or {if_theirs.mean:.1f} (if theirs).",
         fmt" [{if_mine}]",
         fmt" [{if_theirs}]"
  )

# ---------------------------------------------------------------------------

for i in 1..paramCount():
  let hand = make_hand(paramStr(i))
  if hand.num_cards != 6:
    raise newException(ValueError, "Wrong number of cards in hand: " & $hand)

  echo "[ ", hand, " ]"
  analyze_hand(hand)
  echo()
