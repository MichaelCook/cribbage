// Copyright (c) 2020, Michael Cook <michael@waxrat.com>. All rights reserved.

/*
 Analyze cribbage hands.

 Given a cribbage hand (six cards), which two cards should you discard
 to the crib to maximize your chances of getting the best score?
*/

enum Rank {
    Ace = 1,
    Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten,
    Jack, Queen, King
}

function RankFromChar(c: string) {
    switch (c.toUpperCase()) {
        case 'A':
            return Rank.Ace;
        case '2':
            return Rank.Two;
        case '3':
            return Rank.Three;
        case '4':
            return Rank.Four;
        case '5':
            return Rank.Five;
        case '6':
            return Rank.Six;
        case '7':
            return Rank.Seven;
        case '8':
            return Rank.Eight;
        case '9':
            return Rank.Nine;
        case 'T':
            return Rank.Ten;
        case 'J':
            return Rank.Jack;
        case 'Q':
            return Rank.Queen;
        case 'K':
            return Rank.King;
        default:
            return null;
    }
}

function RankToChar(r: Rank) {
    switch (r) {
        case Rank.Ace:
            return 'A';
        case Rank.Two:
            return '2';
        case Rank.Three:
            return '3';
        case Rank.Four:
            return '4';
        case Rank.Five:
            return '5';
        case Rank.Six:
            return '6';
        case Rank.Seven:
            return '7';
        case Rank.Eight:
            return '8';
        case Rank.Nine:
            return '9';
        case Rank.Ten:
            return 'T';
        case Rank.Jack:
            return 'J';
        case Rank.Queen:
            return 'Q';
        case Rank.King:
            return 'K';
        default:
            return null;
    }
}

enum Suit {
    Heart, Club, Diamond, Spade
}

function SuitFromChar(c: string) {
    switch (c.toUpperCase()) {
        case 'H':
            return Suit.Heart;
        case 'C':
            return Suit.Club;
        case 'D':
            return Suit.Diamond;
        case 'S':
            return Suit.Spade;
        default:
            return null;
    }
}

function SuitToChar(s: Suit) {
    switch (s) {
        case Suit.Heart:
            return 'H';
        case Suit.Club:
            return 'C';
        case Suit.Diamond:
            return 'D';
        case Suit.Spade:
            return 'S';
    }
}

class Card {
    constructor(public rank: Rank, public suit: Suit) {}

    public toString = () : string => {
        return RankToChar(this.rank) + SuitToChar(this.suit);
    }
}

class Hand {
    cards: Card [] = new Array();

    static make(text: string): Hand {
        let hand = new Hand();
        let rank: Rank = null;
        for (const c of text) {
            let suit = SuitFromChar(c);
            if (suit != null) {
                if (rank == null) {
                    throw new Error('Malformed hand: ' + text);
                }
                hand.push(new Card(rank, suit));
                rank = null;
                continue;
            }
            let r = RankFromChar(c);
            if (r != null) {
                if (rank != null) {
                    throw new Error('Malformed hand: ' + text);
                }
                rank = r;
                continue;
            }
            if (c !== ' ' && c !== '-' ) {
                throw new Error('Malformed hand: ' + text);
            }
        }
        return hand;
    }

    public clone(): Hand {
        let hand = new Hand();
        [...hand.cards] = this.cards;
        return hand;
    }

    public toString = () : string => {
        let sep = false;
        let text = '';
        for (const card of this.cards) {
            if (sep)
                text += ' ';
            sep = true;
            text += card.toString();
        }
        return text;
    }

    public value(i: number): number {
        const card = this.cards[i];
        return Math.min(10, card.rank);
    }

    public has(wanted: Card): boolean {
        for (const card of this.cards) {
            if (wanted.rank === card.rank && wanted.suit === card.suit) {
                return true;
            }
        }
        return false;
    }

    public push(card: Card): void {
        if (this.has(card)) {
            throw new Error(`Duplicate card in hand: ${card}`);
        }
        this.cards.push(card);
    }

    public pop(): Card {
        return this.cards.pop();
    }
}

function equal(a: any, b: any): void {
    if (a !== b) {
        throw new Error(`*** Not equal: ${a} != ${b}`);
    }
}

equal('5H 5C 5S JD 5D', Hand.make('5H 5C 5S JD 5D').toString());
equal('5H 5C 5S JD 5D', Hand.make('5h5c5sjd5d').toString());
equal('AH AS JH AC AD', Hand.make('ah-as-jh-ac-ad').toString());
equal(true, Hand.make('5H').has(new Card(Rank.Five, Suit.Heart)));
equal(false, Hand.make('5C').has(new Card(Rank.Five, Suit.Heart)));
equal(false, Hand.make('6H').has(new Card(Rank.Five, Suit.Heart)));

function score_fifteens(hand: Hand): number {
    equal(hand.cards.length, 5);

    const a = hand.value(0);
    const b = hand.value(1);
    const c = hand.value(2);
    const d = hand.value(3);
    const e = hand.value(4);

    let num_15s = 0;

    // five cards - C(5,5)=1
    if (a + b + c + d + e === 15) {
        num_15s += 1;
    }

    // four cards - C(5,4)=5
    if (a + b + c + d === 15) {
        num_15s += 1;
    }
    if (a + b + c + e === 15) {
        num_15s += 1;
    }
    if (a + b + d + e === 15) {
        num_15s += 1;
    }
    if (a + c + d + e === 15) {
        num_15s += 1;
    }
    if (b + c + d + e === 15) {
        num_15s += 1;
    }

    // three cards - C(5,3)=10
    if (a + b + c === 15) {
        num_15s += 1;
    }
    if (a + b + d === 15) {
        num_15s += 1;
    }
    if (a + b + e === 15) {
        num_15s += 1;
    }
    if (a + c + d === 15) {
        num_15s += 1;
    }
    if (a + c + e === 15) {
        num_15s += 1;
    }
    if (a + d + e === 15) {
        num_15s += 1;
    }
    if (b + c + d === 15) {
        num_15s += 1;
    }
    if (b + c + e === 15) {
        num_15s += 1;
    }
    if (b + d + e === 15) {
        num_15s += 1;
    }
    if (c + d + e === 15) {
        num_15s += 1;
    }

    // two cards - C(5,2)=10
    if (a + b === 15) {
        num_15s += 1;
    }
    if (a + c === 15) {
        num_15s += 1;
    }
    if (a + d === 15) {
        num_15s += 1;
    }
    if (a + e === 15) {
        num_15s += 1;
    }
    if (b + c === 15) {
        num_15s += 1;
    }
    if (b + d === 15) {
        num_15s += 1;
    }
    if (b + e === 15) {
        num_15s += 1;
    }
    if (c + d === 15) {
        num_15s += 1;
    }
    if (c + e === 15) {
        num_15s += 1;
    }
    if (d + e === 15) {
        num_15s += 1;
    }

    return 2 * num_15s;
}

equal(4, score_fifteens(Hand.make('AH 2H 3H JH QH')));
equal(8, score_fifteens(Hand.make('5H 2H 3H JH QH')));
equal(16, score_fifteens(Hand.make('5H 5S 5C 5D TH')));
equal(8, score_fifteens(Hand.make('6C 6D 4D 4S 5D')));

function score_pairs(hand: Hand): number {
    let num_pairs = 0;
    for (let ai = 0; ai < hand.cards.length - 1; ++ai) {
        for (let bi = ai + 1; bi < hand.cards.length; ++bi) {
            if (hand.cards[ai].rank === hand.cards[bi].rank) {
                num_pairs += 1;
            }
        }
    }
    return 2 * num_pairs;
}

equal(12, score_pairs(Hand.make('5H 5S 5C 5D TH')));
equal(8, score_pairs(Hand.make('TS 5S 5C 5D TH')));
equal(4, score_pairs(Hand.make('6C 6D 4D 4S 5D')));

class Pattern {
    constructor(public score: number, public delta: number[]) {}
};
const X = -1; // match any rank
const PATTERNS = [
    new Pattern( 12, [0, 1, 1, 0] ), // AA233
    new Pattern(  9, [1, 1, 0, 0] ), // A2333
    new Pattern(  9, [1, 0, 0, 1] ), // A2223
    new Pattern(  9, [0, 0, 1, 1] ), // AAA23
    new Pattern(  8, [1, 1, 1, 0] ), // A2344
    new Pattern(  8, [1, 1, 0, 1] ), // A2334
    new Pattern(  8, [1, 0, 1, 1] ), // A2234
    new Pattern(  8, [0, 1, 1, 1] ), // AA234
    new Pattern(  6, [X, 1, 1, 0] ), // xA233
    new Pattern(  6, [X, 1, 0, 1] ), // xA223
    new Pattern(  6, [X, 0, 1, 1] ), // xAA23
    new Pattern(  6, [1, 1, 0, X] ), // A233x
    new Pattern(  6, [1, 0, 1, X] ), // A223x
    new Pattern(  6, [0, 1, 1, X] ), // AA23x
    new Pattern(  5, [1, 1, 1, 1] ), // A2345
    new Pattern(  4, [X, 1, 1, 1] ), // xA234
    new Pattern(  4, [1, 1, 1, X] ), // A234x
    new Pattern(  3, [X, X, 1, 1] ), // xxA23
    new Pattern(  3, [X, 1, 1, X] ), // xA23x
    new Pattern(  3, [1, 1, X, X] ), // A23xx
];

function score_runs(hand: Hand): number {
    equal(hand.cards.length, 5);

    // Make a sorted sequence of the orders of the cards in the hand.
    // The order of Ace is 1, Two is 2, ..., Ten is 10, Jack is 11,
    // Queen is 12, King is 13.
    let orders = hand.cards.map(card => card.rank);
    orders.sort((n1,n2) => n1 - n2);

    // Compare the sorted hand to the PATTERNS.  Look at the difference between
    // the two cards in each pair of adjacent cards.  Stop at the first match.
    for (const pattern of PATTERNS) {
        let previous = orders[0];
        let j = 0;
        while (true) {
            let delta = pattern.delta[j];
            let order = orders[j + 1];
            if (delta !== X && delta !== order - previous) {
                break
            }
            previous = order;
            j += 1;
            if (j === 4) {
                return pattern.score;
            }
        }
    }
    return 0;
}

equal(9, score_runs(Hand.make("AH 2H 3H 3D 3C")));
equal(9, score_runs(Hand.make("KH KD KC JH QH")));  // same pattern A2333
equal(9, score_runs(Hand.make("AH 2H 2D 2C 3H")));
equal(9, score_runs(Hand.make("AH AD AC 2H 3H")));
equal(8, score_runs(Hand.make("AH 2H 3H 4H 4D")));
equal(8, score_runs(Hand.make("AH 2H 3H 3D 4H")));
equal(8, score_runs(Hand.make("AH 2H 2C 3H 4H")));
equal(8, score_runs(Hand.make("AS AH 2H 3H 4H")));
equal(6, score_runs(Hand.make("JH AH 2H 3D 3H")));
equal(6, score_runs(Hand.make("JH AH 2S 2H 3H")));
equal(6, score_runs(Hand.make("JH AH AS 2H 3H")));
equal(6, score_runs(Hand.make("AH 2H 3S 3H JH")));
equal(6, score_runs(Hand.make("AH 2H 2S 3H JH")));
equal(6, score_runs(Hand.make("AH AS 2H 3H JH")));
equal(5, score_runs(Hand.make("AH 2H 3H 4H 5H")));
equal(4, score_runs(Hand.make("JH AH 2H 3H 4H")));
equal(4, score_runs(Hand.make("AH 2H 3H 4H JH")));
equal(3, score_runs(Hand.make("JH QH AH 2H 3H")));
equal(3, score_runs(Hand.make("JH AH 2H 3H TH")));
equal(3, score_runs(Hand.make("AH 2H 3H JH TH")));
equal(0, score_runs(Hand.make("AH 8H 3H JH TH")));
equal(12, score_runs(Hand.make("6C 6D 4D 4S 5D")));

function score_flush(hand: Hand, is_crib: boolean): number {
    equal(hand.cards.length, 5);
    const suit = hand.cards[0].suit;
    for (let i = 1; i < 4; ++i) {
        if (suit !== hand.cards[i].suit) {
            return 0;
        }
    }
    // First 4 are the same suit, check the cut card
    if (suit === hand.cards[4].suit) {
        return 5;
    }
    // In the crib, a flush counts only if all five cards are the same suit
    if (is_crib) {
        return 0;
    }
    return 4;
}

equal(5, score_flush(Hand.make("5H 6H 7H 8H 9H"), false));
equal(4, score_flush(Hand.make("5H 6H 7H 8H 9D"), false));
equal(0, score_flush(Hand.make("5H 6H 7H 8H 9D"), true));
equal(0, score_flush(Hand.make("5H 6H 7H 8D 9D"), false));

function score_nobs(hand: Hand): number {
    // nobs: one point for the Jack of the same suit as the cut card
    equal(hand.cards.length, 5);
    const cut_suit = hand.cards[4].suit;
    for (let i = 0; i < 4; ++i) {
        if (hand.cards[i].rank === Rank.Jack &&
            hand.cards[i].suit === cut_suit) {
            return 1;
        }
    }
    return 0;
}

equal(1, score_nobs(Hand.make("JH 2C 3C 4C 5H")));
equal(0, score_nobs(Hand.make("JH 2C 3C 4C 5C")));

function score_hand(hand: Hand, is_crib: boolean): number {
    return score_fifteens(hand) +
        score_pairs(hand) +
        score_runs(hand) +
        score_flush(hand, is_crib) +
        score_nobs(hand);
}

equal(12, score_hand(Hand.make("AH AS JH AC AD"), false)); // 4oak ("of a kind")
equal(13, score_hand(Hand.make("AH AS JD AC AD"), false)); // ...plus right jack
equal(5, score_hand(Hand.make("AH 3H 7H TH JH"), false));  // 5 hearts
equal(5, score_hand(Hand.make("AH 3H 7H TH JH"), true));   // 5 hearts but crib
equal(4, score_hand(Hand.make("AH 3H 7H TH JS"), false));  // 4 hearts
equal(0, score_hand(Hand.make("AH 3H 7S TH JH"), false));  // 4 hearts but with cut
equal(0, score_hand(Hand.make("AH 3H 7H TH JS"), true));   // 4 hearts but crib
equal(4 + 3, score_hand(Hand.make("AH 2S 3C 5D JH"), false)); // 15/4 + run/3
equal(12 + 6 + 2, score_hand(Hand.make("7H 7S 7C 8D 8H"), false)); // 15/12 + 3oak + 2oak
equal(15, score_hand(Hand.make("AH 2H 3H 3S 3D"), false)); // triple run/3
equal(15, score_hand(Hand.make("3H AH 3S 2H 3D"), false)); // triple run/3
equal(29, score_hand(Hand.make("5H 5C 5S JD 5D"), false));
equal(28, score_hand(Hand.make("5H 5C 5S 5D JD"), false));
equal(24, score_hand(Hand.make("6C 4D 6D 4S 5D"), false));

function* choose(hand: Hand, num_choose: number):
    Generator<Hand, void, boolean> {

    let chosen = new Hand();
    let i = 0;
    let i_stack = new Array();

    while (true) {
        if (chosen.cards.length === num_choose) {
            yield chosen;
            chosen.pop();
            i = i_stack.pop() + 1;
        }
        else if (i !== hand.cards.length) {
            chosen.push(hand.cards[i]);
            i_stack.push(i);
            i++;
        }
        else if (i_stack.length > 0) {
            chosen.pop();
            i = i_stack.pop() + 1;
        }
        else {
            break;
        }
    }
}

function make_deck(exclude: Hand): Hand {
    // Make an entire deck of cards but leave out any cards in `exclude`
    let deck = new Hand();
    for (const suitStr in Suit) {
        const suit = Number(suitStr);
        if (isNaN(suit)) {
            continue;
        }
        for (const rankStr in Rank) {
            const rank = Number(rankStr);
            if (isNaN(rank)) {
                continue;
            }
            const card = new Card(rank, suit);
            if (!exclude.has(card)) {
                deck.push(card);
            }
        }
    }
    return deck;
}

const MAX_SCORE = 29 + 24;  // 29 in hand, 24 in crib (44665)
const MIN_SCORE = -29;      // 0 in hand, 29 in opp crib
const NUM_SCORES = MAX_SCORE - MIN_SCORE + 1;

class Tally {
    scores = Array<number>(NUM_SCORES).fill(0);

    increment(score: number): void {
        this.scores[score - MIN_SCORE] += 1;
    }
}

class Statistics {
    constructor(public mean: number, public stdev: number,
                public min: number, public max: number) {}

    static make(tally: Tally, num_hands: number): Statistics {
        let min = 0;
        for (let i = 0; i < NUM_SCORES; ++i) {
            if (tally.scores[i] !== 0) {
                min = i + MIN_SCORE;
                break;
            }
        }

        let max = 0;
        for (let i = NUM_SCORES - 1; i >= 0; --i) {
            if (tally.scores[i] !== 0) {
                max = i + MIN_SCORE;
                break;
            }
        }

        let sum = 0.0;
        for (let score = min; score <= max; ++score) {
            sum += (score * tally.scores[score - MIN_SCORE]);
        }
        const mean = sum / num_hands;

        let sumdev = 0.0;
        for (let score = min; score <= max; ++score) {
            const d = score - mean;
            sumdev += d * d;
        }
        const stdev = Math.sqrt(sumdev / num_hands);

        return new Statistics(mean, stdev, min, max);
    }

    public toString = () : string => {
        return `${this.mean.toFixed(1)} ${this.stdev.toFixed(1)} ${this.min}..${this.max}`;
    }
}

{
    let t = new Tally();
    const scores = [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 907, 411, 1419, 650, 1855, 663, 1908, 931,
        1671, 650, 1699, 530, 607, 137, 291, 160, 228, 111, 66, 106,
        5, 61, 7, 26, 0, 30, 0, 41, 0, 4, 3, 0, 0, 0, 2, 0, 0, 1 ];
    for (const i in scores) {
        t.scores[i] = scores[i];
    }
    const s = Statistics.make(t, 15180);
    equal(s.toString(), "22.9 0.8 16..53");
}

function analyze_hand(hand: Hand): void {
    /*
      Find all possible pairs of cards to discard to the crib.
      There are C(6,2)=15 possible discards in a cribbage hand.
     */
    for (const discard of choose(hand, 2)) {
        let keep = new Hand();
        for (const card of hand.cards) {
            if (!discard.has(card)) {
                keep.push(card);
            }
        }

        const deck = make_deck(hand);
        equal(deck.cards.length, 46);
        let mine_tally = new Tally();    // scores when the crib is mine
        let theirs_tally = new Tally();  // scores then the crib is theirs
        let num_hands = 0;

        for (const chosen of choose(deck, 3)) {
            num_hands++;
            const cut = chosen.cards[2];

            let hold = keep.clone();
            hold.push(cut);

            let crib = discard.clone();
            crib.push(chosen.cards[0]);
            crib.push(chosen.cards[1]);
            crib.push(cut);

            const hold_score = score_hand(hold, false);
            const crib_score = score_hand(crib, true);

            const mine_score = hold_score + crib_score;
            const theirs_score = hold_score - crib_score;

            mine_tally.increment(mine_score);
            theirs_tally.increment(theirs_score);
        }
        equal(num_hands, 15180);  // sanity check, expecting C(46,3)

        const if_mine = Statistics.make(mine_tally, num_hands);
        const if_theirs = Statistics.make(theirs_tally, num_hands);

        console.log(`${discard} [${if_mine}] [${if_theirs}]`);
    }
}

const args = process.argv.slice(2);
for (const i in args) {
    const hand = Hand.make(args[i]);
    if (hand.cards.length !== 6) {
        throw new Error(`Wrong number of cards in hand: ${hand}`);
    }
    console.log(`[ ${hand} ]`);
    analyze_hand(hand);
    console.log();
}
