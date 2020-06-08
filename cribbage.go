package main

import (
    "os"
    "fmt"
    "sort"
    "math"
)

func equals(a, b string) {
    if a != b {
        panic(fmt.Sprintf("*OOPS* '%v' != '%v'", a, b))
    }
}

func equali(a, b int) {
    if a != b {
        panic(fmt.Sprintf("*OOPS* '%v' != '%v'", a, b))
    }
}

func assert(a bool) {
    if !a {
        panic(fmt.Sprintf("*OOPS*"))
    }
}

type rankT int
type suitT int

type cardT struct {
    rank rankT
    suit suitT
}

func (card cardT) String() string {
    return fmt.Sprintf("%c%c", card.rank, card.suit)
}

type handT struct {
    slots    [52]cardT
    numCards int
}

func (hand *handT) cards() []cardT {
    return hand.slots[:hand.numCards]
}

func (hand *handT) push(card cardT) {
    assert(!hand.has(card))
    hand.slots[hand.numCards] = card
    hand.numCards++
}

func (hand *handT) pop() cardT {
    hand.numCards--
    return hand.slots[hand.numCards]
}

func (hand handT) has(wanted cardT) bool {
    for _, card := range hand.cards() {
        if wanted == card {
            return true
        }
    }
    return false
}

func (hand handT) value(i int) int {
    r := hand.cards()[i].rank
    switch r {
    case 'A':
        return 1
    case '2', '3', '4', '5', '6', '7', '8', '9':
        return int(r) - '0'
    case 'T', 'J', 'Q', 'K':
        return 10
    }
    panic(fmt.Sprintf("bad rank %v", r))
}

func (hand handT) order(i int) int {
    r := hand.cards()[i].rank
    switch r {
    case 'A':
        return 1
    case '2', '3', '4', '5', '6', '7', '8', '9':
        return int(r) - '0'
    case 'T':
        return 10
    case 'J':
        return 11
    case 'Q':
        return 12
    case 'K':
        return 13
    }
    panic(fmt.Sprintf("bad rank %v", r))
}

func (hand handT) String() string {
    sep := false
    text := ""
    for _, card := range hand.cards() {
        if sep {
            text += " "
        }
        sep = true
        text += card.String()
    }
    return text
}

func makeHand(text string) handT {
    hand := handT{}
    rank := rankT(-1)
    for _, c := range text {
        if c >= 'a' && c <= 'z' {
            c += 'A' - 'a'
        }
        switch c {
        case 'H', 'C', 'S', 'D':
            if rank == -1 {
                panic(fmt.Sprintf("Malformed hand '%s'", text))
            }
            hand.push(cardT{rank, suitT(c)})
            rank = -1
        case 'A', '2', '3', '4', '5', '6', '7', '8', '9',
             'T', 'J', 'Q', 'K':
            if rank != -1 {
                panic(fmt.Sprintf("Malformed hand '%s'", text))
            }
            rank = rankT(c)
        case ' ', '-':
            break
        default:
            panic(fmt.Sprintf("Malformed hand '%s'", text))
        }
    }
    if rank != -1 {
        panic(fmt.Sprintf("Malformed hand '%s'", text))
    }
    return hand
}

func score15s(hand handT) int {
    equali(hand.numCards, 5)

    a := hand.value(0)
    b := hand.value(1)
    c := hand.value(2)
    d := hand.value(3)
    e := hand.value(4)

    num15s := 0

    // five cards - C(5,5)=1
    if a + b + c + d + e == 15 {
        num15s++
    }

    // four cards - C(5,4)=5
    if a + b + c + d == 15 {
        num15s++
    }
    if a + b + c + e == 15 {
        num15s++
    }
    if a + b + d + e == 15 {
        num15s++
    }
    if a + c + d + e == 15 {
        num15s++
    }
    if b + c + d + e == 15 {
        num15s++
    }

    // three cards - C(5,3)=10
    if a + b + c == 15 {
        num15s++
    }
    if a + b + d == 15 {
        num15s++
    }
    if a + b + e == 15 {
        num15s++
    }
    if a + c + d == 15 {
        num15s++
    }
    if a + c + e == 15 {
        num15s++
    }
    if a + d + e == 15 {
        num15s++
    }
    if b + c + d == 15 {
        num15s++
    }
    if b + c + e == 15 {
        num15s++
    }
    if b + d + e == 15 {
        num15s++
    }
    if c + d + e == 15 {
        num15s++
    }

    // two cards - C(5,2)=10
    if a + b == 15 {
        num15s++
    }
    if a + c == 15 {
        num15s++
    }
    if a + d == 15 {
        num15s++
    }
    if a + e == 15 {
        num15s++
    }
    if b + c == 15 {
        num15s++
    }
    if b + d == 15 {
        num15s++
    }
    if b + e == 15 {
        num15s++
    }
    if c + d == 15 {
        num15s++
    }
    if c + e == 15 {
        num15s++
    }
    if d + e == 15 {
        num15s++
    }

    return 2 * num15s
}

func scorePairs(hand handT) int {
    numPairs := 0
    for ai := 0; ai < hand.numCards - 1; ai++ {
        for bi := ai + 1; bi < hand.numCards; bi++ {
            if hand.slots[ai].rank == hand.slots[bi].rank {
                numPairs++
            }
        }
    }
    return 2 * numPairs
}

type patternT struct {
    score int
    delta [4]int
}
const x = -1 // match any rank
var patterns = []patternT{
    patternT{12, [4]int{0, 1, 1, 0}}, // AA233
    patternT{ 9, [4]int{1, 1, 0, 0}}, // A2333
    patternT{ 9, [4]int{1, 0, 0, 1}}, // A2223
    patternT{ 9, [4]int{0, 0, 1, 1}}, // AAA23
    patternT{ 8, [4]int{1, 1, 1, 0}}, // A2344
    patternT{ 8, [4]int{1, 1, 0, 1}}, // A2334
    patternT{ 8, [4]int{1, 0, 1, 1}}, // A2234
    patternT{ 8, [4]int{0, 1, 1, 1}}, // AA234
    patternT{ 6, [4]int{x, 1, 1, 0}}, // xA233
    patternT{ 6, [4]int{x, 1, 0, 1}}, // xA223
    patternT{ 6, [4]int{x, 0, 1, 1}}, // xAA23
    patternT{ 6, [4]int{1, 1, 0, x}}, // A233x
    patternT{ 6, [4]int{1, 0, 1, x}}, // A223x
    patternT{ 6, [4]int{0, 1, 1, x}}, // AA23x
    patternT{ 5, [4]int{1, 1, 1, 1}}, // A2345
    patternT{ 4, [4]int{x, 1, 1, 1}}, // xA234
    patternT{ 4, [4]int{1, 1, 1, x}}, // A234x
    patternT{ 3, [4]int{x, x, 1, 1}}, // xxA23
    patternT{ 3, [4]int{x, 1, 1, x}}, // xA23x
    patternT{ 3, [4]int{1, 1, x, x}}, // A23xx
}

func scoreRuns(hand handT) int {
    equali(hand.numCards, 5)

    // Make a sorted sequence of the orders of the cards in the hand.
    // The order of Ace is 1, Two is 2, ..., Ten is 10, Jack is 11,
    // Queen is 12, King is 13.
    orders := []int{
        hand.order(0),
        hand.order(1),
        hand.order(2),
        hand.order(3),
        hand.order(4),
    }
    sort.Ints(orders)

    // Compare the sorted hand to the PATTERNS.  Look at the difference between
    // the two cards in each pair of adjacent cards.  Stop at the first match.
    for _, pattern := range patterns {
        previous := orders[0]
        j := 0
        for {
            delta := pattern.delta[j]
            order := orders[j + 1]
            if delta != x && delta != order - previous {
                break
            }
            previous = order
            j++
            if j == 4 {
                return pattern.score
            }
        }
    }
    return 0
}

func scoreFlush(hand handT, isCrib bool) int {
    equali(hand.numCards, 5)
    suit := hand.slots[0].suit
    for i := 1; i < 4; i++ {
        if suit != hand.slots[i].suit {
            return 0
        }
    }
    // First 4 are the same suit, check the cut card
    if suit == hand.slots[4].suit {
        return 5
    }
    // In the crib, a flush counts only if all five cards are the same suit
    if isCrib {
        return 0
    }
    return 4
}

func scoreNobs(hand handT) int {
    // nobs: one point for the Jack of the same suit as the cut card
    equali(hand.numCards, 5)
    cutSuit := hand.slots[4].suit
    for i := 0; i < 4; i++ {
        if hand.slots[i].rank == 'J' &&
           hand.slots[i].suit == cutSuit {
            return 1
        }
    }
    return 0
}

func scoreHand(hand handT, isCrib bool) int {
    return score15s(hand) +
           scorePairs(hand) +
           scoreRuns(hand) +
           scoreFlush(hand, isCrib) +
           scoreNobs(hand)
}

const (
    maxScore  = 29 + 24 // 29 in hand, 24 in crib (44665)
    minScore  = -29     // 0 in hand, 29 in opp crib
    numScores = maxScore - minScore + 1
)

type tallyT struct {
    scores [numScores]int
}

func (tally *tallyT) increment(i int) {
    tally.scores[i - minScore]++
}

type statisticsT struct {
    mean float64
    stdev float64
    min int
    max int
}

func (stats statisticsT) String() string {
    return fmt.Sprintf("%.1f %.1f %d..%d", stats.mean, stats.stdev, stats.min, stats.max)
}

func makeStatistics(tally tallyT, numHands int) statisticsT {
    min := 0
    for i, score := range tally.scores {
        if score != 0 {
            min = i + minScore
            break
        }
    }

    max := 0
    for i, score := range tally.scores {
        if score != 0 {
            max = i + minScore
        }
    }

    sum := 0.0
    for score := min; score <= max; score++ {
        sum += float64(score * tally.scores[score - minScore])
    }
    mean := sum / float64(numHands)

    sumdev := 0.0
    for score := min; score <= max; score++ {
        d := float64(score) - mean
        sumdev += d * d;
    }
    stdev := math.Sqrt(sumdev / float64(numHands))

    return statisticsT{mean, stdev, min, max}
}

func choose(hand handT, numChoose int) <-chan handT {
    ch := make(chan handT);
    go func () {
        chosen := handT{}
        i := 0
        iStack := []int{}
        for {
            if chosen.numCards == numChoose {
                ch <- chosen
                chosen.pop()
                // i = iStack.pop() + 1
                i = iStack[len(iStack) - 1] + 1
                iStack = iStack[:len(iStack)-1]
            } else if i != hand.numCards {
                chosen.push(hand.cards()[i])
                iStack = append(iStack, i)
                i++
            } else if len(iStack) > 0 {
                chosen.pop()
                // i = iStack.pop() + 1
                i = iStack[len(iStack) - 1] + 1
                iStack = iStack[:len(iStack)-1]
            } else {
                break
            }
        }
        close(ch)
    } ();
    return ch
}

func makeDeck(exclude handT) handT {
    // Make an entire deck of cards but leave out any cards in `exclude`
    deck := handT{}
    for _, suit := range "HCDS" {
        for _, rank := range "A23456789TJQK" {
            card := cardT{rankT(rank), suitT(suit)}
            if !exclude.has(card) {
                deck.push(card)
            }
        }
    }
    return deck
}

func analyzeHand(hand handT) {
    /*
      Find all possible pairs of cards to discard to the crib.
      There are C(6,2)=15 possible discards in a cribbage hand.
     */
    for discard := range choose(hand, 2) {
        keep := handT{}
        for _, card := range hand.cards() {
            if !discard.has(card) {
                keep.push(card)
            }
        }

        deck := makeDeck(hand)
        equali(deck.numCards, 46)

        mineTally := tallyT{}    // scores when the crib is mine
        theirsTally := tallyT{}  // scores then the crib is theirs
        numHands := 0
        for chosen := range choose(deck, 3) {
            numHands++
            cut := chosen.slots[2]

            hold := keep
            hold.push(cut)

            crib := discard
            crib.push(chosen.slots[0])
            crib.push(chosen.slots[1])
            crib.push(cut)

            holdScore := scoreHand(hold, false)
            cribScore := scoreHand(crib, true)

            mineScore := holdScore + cribScore
            theirsScore := holdScore - cribScore

            mineTally.increment(mineScore)
            theirsTally.increment(theirsScore)
        }
        equali(numHands, 15180)  // sanity check, expecting C(46,3)

        ifMine := makeStatistics(mineTally, numHands)
        ifTheirs := makeStatistics(theirsTally, numHands)

        fmt.Printf("%s [%s] [%s]\n", discard, ifMine, ifTheirs)
    }
}

func main() {
    fmt.Println("numScores:", numScores)

    equals("5H 5C 5S JD 5D", makeHand("5H 5C 5S JD 5D").String())
    equals("5H 5C 5S JD 5D", makeHand("5h5c5sjd5d").String())
    equals("AH AS JH AC AD", makeHand("ah-as-jh-ac-ad").String())

    assert(makeHand("5H").has(cardT{'5', 'H'}))
    assert(!makeHand("5C").has(cardT{'5', 'H'}))
    assert(!makeHand("6H").has(cardT{'5', 'H'}))

    equali( 4, score15s(makeHand("AH 2H 3H JH QH")))
    equali( 8, score15s(makeHand("5H 2H 3H JH QH")))
    equali(16, score15s(makeHand("5H 5S 5C 5D TH")))
    equali( 8, score15s(makeHand("6C 6D 4D 4S 5D")))

    equali(12, scorePairs(makeHand("5H 5S 5C 5D TH")))
    equali( 8, scorePairs(makeHand("TS 5S 5C 5D TH")))
    equali( 4, scorePairs(makeHand("6C 6D 4D 4S 5D")))

    equali( 9, scoreRuns(makeHand("AH 2H 3H 3D 3C")))
    equali( 9, scoreRuns(makeHand("KH KD KC JH QH")))  // same pattern A2333
    equali( 9, scoreRuns(makeHand("AH 2H 2D 2C 3H")))
    equali( 9, scoreRuns(makeHand("AH AD AC 2H 3H")))
    equali( 8, scoreRuns(makeHand("AH 2H 3H 4H 4D")))
    equali( 8, scoreRuns(makeHand("AH 2H 3H 3D 4H")))
    equali( 8, scoreRuns(makeHand("AH 2H 2C 3H 4H")))
    equali( 8, scoreRuns(makeHand("AS AH 2H 3H 4H")))
    equali( 6, scoreRuns(makeHand("JH AH 2H 3D 3H")))
    equali( 6, scoreRuns(makeHand("JH AH 2S 2H 3H")))
    equali( 6, scoreRuns(makeHand("JH AH AS 2H 3H")))
    equali( 6, scoreRuns(makeHand("AH 2H 3S 3H JH")))
    equali( 6, scoreRuns(makeHand("AH 2H 2S 3H JH")))
    equali( 6, scoreRuns(makeHand("AH AS 2H 3H JH")))
    equali( 5, scoreRuns(makeHand("AH 2H 3H 4H 5H")))
    equali( 4, scoreRuns(makeHand("JH AH 2H 3H 4H")))
    equali( 4, scoreRuns(makeHand("AH 2H 3H 4H JH")))
    equali( 3, scoreRuns(makeHand("JH QH AH 2H 3H")))
    equali( 3, scoreRuns(makeHand("JH AH 2H 3H TH")))
    equali( 3, scoreRuns(makeHand("AH 2H 3H JH TH")))
    equali( 0, scoreRuns(makeHand("AH 8H 3H JH TH")))
    equali(12, scoreRuns(makeHand("6C 6D 4D 4S 5D")))

    equali(5, scoreFlush(makeHand("5H 6H 7H 8H 9H"), false))
    equali(4, scoreFlush(makeHand("5H 6H 7H 8H 9D"), false))
    equali(0, scoreFlush(makeHand("5H 6H 7H 8H 9D"), true))
    equali(0, scoreFlush(makeHand("5H 6H 7H 8D 9D"), false))

    equali(1, scoreNobs(makeHand("JH 2C 3C 4C 5H")))
    equali(0, scoreNobs(makeHand("JH 2C 3C 4C 5C")))

    equali(12, scoreHand(makeHand("AH AS JH AC AD"), false)) // 4oak ("of a kind")
    equali(13, scoreHand(makeHand("AH AS JD AC AD"), false)) // ...plus right jack
    equali( 5, scoreHand(makeHand("AH 3H 7H TH JH"), false)) // 5 hearts
    equali( 5, scoreHand(makeHand("AH 3H 7H TH JH"), true))  // 5 hearts but crib
    equali( 4, scoreHand(makeHand("AH 3H 7H TH JS"), false)) // 4 hearts
    equali( 0, scoreHand(makeHand("AH 3H 7S TH JH"), false)) // 4 hearts but with cut
    equali( 0, scoreHand(makeHand("AH 3H 7H TH JS"), true))  // 4 hearts but crib
    equali( 7, scoreHand(makeHand("AH 2S 3C 5D JH"), false)) // 15/4 + run/3
    equali(20, scoreHand(makeHand("7H 7S 7C 8D 8H"), false)) // 15/12 + 3oak + 2oak
    equali(15, scoreHand(makeHand("AH 2H 3H 3S 3D"), false)) // triple run/3
    equali(15, scoreHand(makeHand("3H AH 3S 2H 3D"), false)) // triple run/3
    equali(29, scoreHand(makeHand("5H 5C 5S JD 5D"), false))
    equali(28, scoreHand(makeHand("5H 5C 5S 5D JD"), false))
    equali(24, scoreHand(makeHand("6C 4D 6D 4S 5D"), false))

    {
        t := tallyT{}
        for i, v := range []int{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 907, 411, 1419, 650,
                           1855, 663, 1908, 931, 1671, 650, 1699, 530, 607, 137,
                           291, 160, 228, 111, 66, 106, 5, 61, 7, 26, 0, 30, 0,
                           41, 0, 4, 3, 0, 0, 0, 2, 0, 0, 1 } {
            t.scores[i] = v
        }
        s := makeStatistics(t, 15180)
        equals(s.String(), "22.9 0.8 16..53")
    }

    for _, arg := range os.Args[1:] {
        hand := makeHand(arg)
        if hand.numCards != 6 {
            panic(fmt.Sprintf("Wrong number of cards in hand: %s", hand))
        }
        fmt.Printf("[ %s ]\n", hand)
        analyzeHand(hand)
        fmt.Println()
    }
}
