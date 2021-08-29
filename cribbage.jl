#!/bin/julia

import Printf.@printf
using Combinatorics

const SUITS = "SDCH"
const RANKS = "A23456789TJQK"

@enum Suit begin
    Spades = 1
    Diamonds
    Clubs
    Hearts
end

@enum Rank begin
    Ace = 1
    Deuce
    Three
    Four
    Five
    Six
    Seven
    Eight
    Nine
    Ten
    Jack
    Queen
    King
end

struct Card
    rank::Rank
    suit::Suit
end

function Base.show(io::IO, card::Card)
    print(io, RANKS[Int(card.rank)])
    print(io, SUITS[Int(card.suit)])
end

function order(card::Card)::Int
    return Int(card.rank)
end

Cards = Array{Card, 1}

struct Hand
    cards::Cards
    Hand() = new(Cards())
end

function push(hand::Hand, card::Card)
    @assert !(card in hand.cards)
    push!(hand.cards, card)
end

function pop(hand::Hand)
    pop!(hand.cards)
end

function length(hand::Hand)::Int
    return Base.length(hand.cards)
end

function value(hand::Hand, index::Int)::Int
    return min(10, Int(hand.cards[index].rank))
end

function Base.show(io::IO, hand::Hand)
    sep = false
    for card in hand.cards
        if sep
            print(io, ' ')
        end
        print(io, card)
        sep = true
    end
end

function make_hand(s::String)::Hand
    hand = Hand()
    rank = nothing

    for c in uppercase(s)
        i = findfirst(c, SUITS)
        if !isnothing(i)
            if isnothing(rank)
                error("Malformed hand: $s")
            end
            push(hand, Card(rank, Suit(i)))
            rank = nothing
            continue
        end

        i = findfirst(c, RANKS)
        if !isnothing(i)
            if !isnothing(rank)
                error("Malformed hand: $s")
            end
            rank = Rank(i)
            continue
        end

        if c != ' ' && c != '-'
            error("Malformed hand: $s")
        end
    end

    if !isnothing(rank)
        error("Malformed hand: $s")
    end

    return hand
end

@assert "5H 5C 5S JD 5D" == string(make_hand("5H 5C 5S JD 5D"))
@assert "5H 5C 5S JD 5D" == string(make_hand("5h5c5sjd5d"))
@assert "AH AS JH AC AD" == string(make_hand("ah-as-jh-ac-ad"))

function score_15s(hand::Hand)::Int
    @assert length(hand) == 5

    a = value(hand, 1)
    b = value(hand, 2)
    c = value(hand, 3)
    d = value(hand, 4)
    e = value(hand, 5)

    num_15s = 0

    # five cards - C(5,5)=1
    if a + b + c + d + e == 15
        num_15s += 1
    end

    # four cards - C(5,4)=5
    if a + b + c + d == 15
        num_15s += 1
    end
    if a + b + c + e == 15
        num_15s += 1
    end
    if a + b + d + e == 15
        num_15s += 1
    end
    if a + c + d + e == 15
        num_15s += 1
    end
    if b + c + d + e == 15
        num_15s += 1
    end

    # three cards - C(5,3)=10
    if a + b + c == 15
        num_15s += 1
    end
    if a + b + d == 15
        num_15s += 1
    end
    if a + b + e == 15
        num_15s += 1
    end
    if a + c + d == 15
        num_15s += 1
    end
    if a + c + e == 15
        num_15s += 1
    end
    if a + d + e == 15
        num_15s += 1
    end
    if b + c + d == 15
        num_15s += 1
    end
    if b + c + e == 15
        num_15s += 1
    end
    if b + d + e == 15
        num_15s += 1
    end
    if c + d + e == 15
        num_15s += 1
    end

    # two cards - C(5,2)=10
    if a + b == 15
        num_15s += 1
    end
    if a + c == 15
        num_15s += 1
    end
    if a + d == 15
        num_15s += 1
    end
    if a + e == 15
        num_15s += 1
    end
    if b + c == 15
        num_15s += 1
    end
    if b + d == 15
        num_15s += 1
    end
    if b + e == 15
        num_15s += 1
    end
    if c + d == 15
        num_15s += 1
    end
    if c + e == 15
        num_15s += 1
    end
    if d + e == 15
        num_15s += 1
    end

    return 2 * num_15s
end

@assert  4 == score_15s(make_hand("AH 2H 3H JH QH"))
@assert  8 == score_15s(make_hand("5H 2H 3H JH QH"))
@assert 16 == score_15s(make_hand("5H 5S 5C 5D TH"))
@assert  8 == score_15s(make_hand("6C 6D 4D 4S 5D"))

function score_pairs(hand::Hand)::Int
    num_pairs = 0
    for (ai, a) in enumerate(hand.cards)
        for b in Iterators.drop(hand.cards, ai)
            if a.rank == b.rank
                num_pairs += 1
            end
        end
    end
    return 2 * num_pairs
end

@assert 12 == score_pairs(make_hand("5H 5S 5C 5D TH"))
@assert  8 == score_pairs(make_hand("TS 5S 5C 5D TH"))
@assert  4 == score_pairs(make_hand("6C 6D 4D 4S 5D"))

const X = -1            # match any rank
const RUN_PATTERNS = (
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

function score_runs(hand::Hand)::Int
    @assert length(hand) == 5

    # Make a sorted sequence of the orders of the cards in the hand.  The
    # order of Ace is 1, ..., King is 13
    orders = map(card -> order(card), hand.cards)
    sort!(orders)

    # Compare the sorted hand to the patterns.  Look at the difference between
    # the two cards in each pair of adjacent cards.  Stop at the first match
    for (score, deltas) in RUN_PATTERNS
        previous = orders[1]
        j = 1
        while true
            delta = deltas[j]
            order = orders[j + 1]
            if delta != X && delta != order - previous
                break
            end
            previous = order
            j += 1
            if j == 5
                return score
            end
        end
    end
    return 0
end

@assert  9 == score_runs(make_hand("AH 2H 3H 3D 3C"))
@assert  9 == score_runs(make_hand("KH KD KC JH QH"))  # same pattern A2333
@assert  9 == score_runs(make_hand("AH 2H 2D 2C 3H"))
@assert  9 == score_runs(make_hand("AH AD AC 2H 3H"))
@assert  8 == score_runs(make_hand("AH 2H 3H 4H 4D"))
@assert  8 == score_runs(make_hand("AH 2H 3H 3D 4H"))
@assert  8 == score_runs(make_hand("AH 2H 2C 3H 4H"))
@assert  8 == score_runs(make_hand("AS AH 2H 3H 4H"))
@assert  6 == score_runs(make_hand("JH AH 2H 3D 3H"))
@assert  6 == score_runs(make_hand("JH AH 2S 2H 3H"))
@assert  6 == score_runs(make_hand("JH AH AS 2H 3H"))
@assert  6 == score_runs(make_hand("AH 2H 3S 3H JH"))
@assert  6 == score_runs(make_hand("AH 2H 2S 3H JH"))
@assert  6 == score_runs(make_hand("AH AS 2H 3H JH"))
@assert  5 == score_runs(make_hand("AH 2H 3H 4H 5H"))
@assert  4 == score_runs(make_hand("JH AH 2H 3H 4H"))
@assert  4 == score_runs(make_hand("AH 2H 3H 4H JH"))
@assert  3 == score_runs(make_hand("JH QH AH 2H 3H"))
@assert  3 == score_runs(make_hand("JH AH 2H 3H TH"))
@assert  3 == score_runs(make_hand("AH 2H 3H JH TH"))
@assert  0 == score_runs(make_hand("AH 8H 3H JH TH"))
@assert 12 == score_runs(make_hand("6C 6D 4D 4S 5D"))

function score_flush(hand::Hand, is_crib::Bool)::Int
    @assert length(hand) == 5
    suit = hand.cards[1].suit
    for card in Iterators.take(Iterators.drop(hand.cards, 1), 3)
        if suit != card.suit
            return 0
        end
    end
    # First 4 are the same suit, check the cut card
    if suit == hand.cards[5].suit
        return 5
    end
    # In the crib, a flush counts only if all five cards are the same suit
    if is_crib
        return 0
    end
    return 4
end

@assert 5 == score_flush(make_hand("5H 6H 7H 8H 9H"), false)
@assert 4 == score_flush(make_hand("5H 6H 7H 8H 9D"), false)
@assert 0 == score_flush(make_hand("5H 6H 7H 8H 9D"), true)
@assert 0 == score_flush(make_hand("5H 6H 7H 8D 9D"), false)

function score_nobs(hand::Hand)::Int
    # nobs: one point for the Jack of the same suit as the cut card
    @assert length(hand) == 5
    cut_suit = hand.cards[5].suit
    for card in Iterators.take(hand.cards, 4)
        if card.rank == Jack && card.suit == cut_suit
            return 1
        end
    end
    return 0
end

@assert 1 == score_nobs(make_hand("JH 2C 3C 4C 5H"))
@assert 0 == score_nobs(make_hand("JH 2C 3C 4C 5C"))

function score_hand(hand::Hand, is_crib::Bool)::Int
    return score_15s(hand) +
           score_pairs(hand) +
           score_runs(hand) +
           score_flush(hand, is_crib) +
           score_nobs(hand)
end

function score_hand(hand::String, is_crib::Bool)::Int
    return score_hand(make_hand(hand), is_crib)
end

@assert 12 == score_hand("AH AS JH AC AD", false) # 4oak ("of a kind")
@assert 13 == score_hand("AH AS JD AC AD", false) # ...plus right jack
@assert  5 == score_hand("AH 3H 7H TH JH", false) # 5 hearts
@assert  5 == score_hand("AH 3H 7H TH JH", true)  # 5 hearts but crib
@assert  4 == score_hand("AH 3H 7H TH JS", false) # 4 hearts
@assert  0 == score_hand("AH 3H 7S TH JH", false) # 4 hearts but with cut
@assert  0 == score_hand("AH 3H 7H TH JS", true)  # 4 hearts but crib
@assert  7 == score_hand("AH 2S 3C 5D JH", false) # 15/4 + run/3
@assert 20 == score_hand("7H 7S 7C 8D 8H", false) # 15/12 + 3oak + 2oak
@assert 15 == score_hand("AH 2H 3H 3S 3D", false) # triple run/3
@assert 15 == score_hand("3H AH 3S 2H 3D", false) # triple run/3
@assert 29 == score_hand("5H 5C 5S JD 5D", false)
@assert 28 == score_hand("5H 5C 5S 5D JD", false)
@assert 24 == score_hand("6C 4D 6D 4S 5D", false)

# ---------------------------------------------------------------------------

function make_deck(exclude::Hand)::Hand
    # Make an entire deck of cards but leave out any cards in `exclude`
    deck = Hand()
    for suit in instances(Suit)
        for rank in instances(Rank)
            card = Card(rank, suit)
            if !(card in exclude.cards)
                push(deck, card)
            end
        end
    end
    return deck
end

make_deck() = make_deck(Hand())

const MAX_SCORE = 29 + 24  # 29 in hand, 24 in crib (44665)
const MIN_SCORE = -29      # 0 in hand, 29 in opp crib
const NUM_SCORES = MAX_SCORE - MIN_SCORE + 1

struct Tally
    scores::Array{Int32, 1}

    Tally() = new(zeros(Int32, NUM_SCORES))
end

function count(tally::Tally, score::Int64)
    tally.scores[score - MIN_SCORE + 1] += 1
end

struct Statistics
    mean::Float32
    stdev::Float32
    min_score::Int
    max_score::Int
end

function Base.show(io::IO, stats::Statistics)
    @printf(io, "%.1f %.1f %d..%d", stats.mean, stats.stdev, stats.min_score, stats.max_score)
end

function make_statistics(tally::Tally, num_hands::Int64)::Statistics
    # Convert `tally` to a Statistics object.  `tally` is the number of times
    # each score MIN_SCORE..MAX_SCORE was achieved over `num_hands` hands.
    mins = 0
    for score in MIN_SCORE:MAX_SCORE
        if tally.scores[score - MIN_SCORE + 1] != 0
            mins = score
            break
        end
    end

    maxs = 0
    for score in MAX_SCORE:-1:MIN_SCORE
        if tally.scores[score - MIN_SCORE + 1] != 0
            maxs = score
            break
        end
    end

    sums = 0.0
    for score in mins:maxs
        sums += score * tally.scores[score - MIN_SCORE + 1]
    end
    mean = sums / num_hands

    sumdev = 0.0
    for score in mins:maxs
        d = score - mean
        sumdev += d * d
    end
    stdev = sqrt(sumdev / num_hands)

    return Statistics(mean, stdev, mins, maxs)
end

function analyze_hand(hand::Hand)
    # Find all possible pairs of cards to discard to the crib.
    # There are C(6,2)=15 possible discards in a cribbage hand.

    for discarding in combinations(hand.cards, 2)
        hold = Hand()
        for card in hand.cards
            if !(card in discarding)
                push(hold, card)
            end
        end

        deck = make_deck(hand)
        @assert length(deck) == 46

        mine_tally = Tally()   # scores when the crib is mine
        theirs_tally = Tally() # scores then the crib is theirs
        num_hands = 0
        for chosen in combinations(deck.cards, 2)
            card1 = chosen[1]
            card2 = chosen[2]

            crib = Hand()
            push(crib, discarding[1])
            push(crib, discarding[2])
            push(crib, card1)
            push(crib, card2)
            @assert length(crib) == 4

            for cut in deck.cards
                if cut == card1 || cut == card2
                    continue
                end

                push(hold, cut)
                hold_score = score_hand(hold, false)
                pop(hold)

                push(crib, cut)
                crib_score = score_hand(crib, true)
                pop(crib)

                mine_score = hold_score + crib_score
                theirs_score = hold_score - crib_score

                num_hands += 1

                count(mine_tally, mine_score)
                count(theirs_tally, theirs_score)
            end
        end

        # deck size: 46, C(46,2)=1035
        # remaining_deck size: 44
        @assert num_hands == 1035 * 44

        if_mine = make_statistics(mine_tally, num_hands)
        if_theirs = make_statistics(theirs_tally, num_hands)

        println("$(discarding[1]) $(discarding[2]) [$if_mine] [$if_theirs]")
    end
end

# ---------------------------------------------------------------------------

function main()
    for arg in ARGS
        hand = make_hand(arg)
        if length(hand) != 6
            error("Wrong number of cards in hand: $hand")
        end

        println("[ $hand ]")
        analyze_hand(hand)
        println()
    end
end

main()
