use std::fmt;
use itertools::Itertools;

#[derive(Copy, Clone)]
#[derive(PartialEq)]
enum Rank {
    Ace = 1,
    Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten,
    Jack, Queen, King
}

static RANKS: [Rank; 13] = [
    Rank::Ace,
    Rank::Two, Rank::Three, Rank::Four, Rank::Five,
    Rank::Six, Rank::Seven, Rank::Eight, Rank::Nine,
    Rank::Ten, Rank::Jack, Rank::Queen, Rank::King
];

impl Rank {
    pub fn from_char(c: char) -> Option<Rank> {
        match c {
            'A' => Some(Rank::Ace),
            '2' => Some(Rank::Two),
            '3' => Some(Rank::Three),
            '4' => Some(Rank::Four),
            '5' => Some(Rank::Five),
            '6' => Some(Rank::Six),
            '7' => Some(Rank::Seven),
            '8' => Some(Rank::Eight),
            '9' => Some(Rank::Nine),
            'T' => Some(Rank::Ten),
            'J' => Some(Rank::Jack),
            'Q' => Some(Rank::Queen),
            'K' => Some(Rank::King),
            _ => None,
        }
    }
}

impl fmt::Display for Rank {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let c: char = match self {
            Rank::Ace => 'A',
            Rank::Two => '2',
            Rank::Three => '3',
            Rank::Four => '4',
            Rank::Five => '5',
            Rank::Six => '6',
            Rank::Seven => '7',
            Rank::Eight => '8',
            Rank::Nine => '9',
            Rank::Ten => 'T',
            Rank::Jack => 'J',
            Rank::Queen => 'Q',
            Rank::King => 'K',
        };
        write!(f, "{}", c)
    }
}

#[derive(Copy, Clone)]
#[derive(PartialEq)]
enum Suit {
    Heart, Club, Diamond, Spade
}

static SUITS: [Suit; 4] = [
    Suit::Heart, Suit::Club, Suit::Diamond, Suit::Spade
];

impl fmt::Display for Suit {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let c: char = match self {
            Suit::Heart => 'H',
            Suit::Club => 'C',
            Suit::Diamond => 'D',
            Suit::Spade => 'S',
        };
        write!(f, "{}", c)
    }
}

impl Suit {
    pub fn from_char(c: char) -> Option<Suit> {
        match c {
            'H' => Some(Suit::Heart),
            'C' => Some(Suit::Club),
            'D' => Some(Suit::Diamond),
            'S' => Some(Suit::Spade),
            _ => None,
        }
    }
}

#[derive(Copy, Clone)]
#[derive(PartialEq)]
struct Card {
    rank: Rank,
    suit: Suit,
}

impl fmt::Display for Card {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{}{}", self.rank, self.suit)
    }
}

#[derive(Clone)]
struct Hand {
    cards: Vec<Card>
}

impl Hand {
    pub fn new() -> Hand { // TODO: impl Default?
        Hand {
            cards: Vec::with_capacity(52)
        }
    }

    pub fn make(text: &str) -> Hand {
        let mut hand = Hand::new();
        let mut rank: Option<Rank> = None;
        for c in text.to_uppercase().chars() {
            if let Some(suit) = Suit::from_char(c) {
                match rank {
                    None => panic!("Malformed hand: {}", text),
                    Some(r) => hand.cards.push(Card{rank: r, suit}),
                }
                rank = None;
            }
            else if let Some(r) = Rank::from_char(c) {
                if rank.is_some() {
                    panic!("Malformed hand: {}", text)
                }
                rank = Some(r);
            }
            else if c != ' ' && c != '-' {
                panic!("Malformed hand: {}", text);
            }
        }
        hand
    }

    pub fn from_vec(cards: Vec<&Card>) -> Hand {
        let mut hand = Hand::new();
        for card in cards {
            hand.cards.push(*card);
        }
        hand
    }

    pub fn num_cards(&self) -> usize {
        self.cards.len()
    }

    pub fn value(&self, i: usize) -> i32 {
        std::cmp::min(10, self.cards[i].rank as i32)
    }

    pub fn has(&self, wanted: &Card) -> bool {
        for card in self.cards.iter() {
            if card == wanted {
                return true
            }
        }
        false
    }

    pub fn push_back(&mut self, card: Card) {
        assert!(!self.has(&card));
        self.cards.push(card);
    }
}

impl fmt::Display for Hand {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let mut sep = false;
        for card in &self.cards {
            if sep {
                f.write_str(" ")?;
            }
            sep = true;
            f.write_str(&card.to_string())?;
        }
        Ok(())
    }
}

fn score_fifteens(hand: &Hand) -> i32 {
    assert!(hand.num_cards() == 5);

    let a = hand.value(0);
    let b = hand.value(1);
    let c = hand.value(2);
    let d = hand.value(3);
    let e = hand.value(4);

    let mut num_15s = 0;

    // five cards - C(5,5)=1
    if a + b + c + d + e == 15 {
        num_15s += 1;
    }

    // four cards - C(5,4)=5
    if a + b + c + d == 15 {
        num_15s += 1;
    }
    if a + b + c + e == 15 {
        num_15s += 1;
    }
    if a + b + d + e == 15 {
        num_15s += 1;
    }
    if a + c + d + e == 15 {
        num_15s += 1;
    }
    if b + c + d + e == 15 {
        num_15s += 1;
    }

    // three cards - C(5,3)=10
    if a + b + c == 15 {
        num_15s += 1;
    }
    if a + b + d == 15 {
        num_15s += 1;
    }
    if a + b + e == 15 {
        num_15s += 1;
    }
    if a + c + d == 15 {
        num_15s += 1;
    }
    if a + c + e == 15 {
        num_15s += 1;
    }
    if a + d + e == 15 {
        num_15s += 1;
    }
    if b + c + d == 15 {
        num_15s += 1;
    }
    if b + c + e == 15 {
        num_15s += 1;
    }
    if b + d + e == 15 {
        num_15s += 1;
    }
    if c + d + e == 15 {
        num_15s += 1;
    }

    // two cards - C(5,2)=10
    if a + b == 15 {
        num_15s += 1;
    }
    if a + c == 15 {
        num_15s += 1;
    }
    if a + d == 15 {
        num_15s += 1;
    }
    if a + e == 15 {
        num_15s += 1;
    }
    if b + c == 15 {
        num_15s += 1;
    }
    if b + d == 15 {
        num_15s += 1;
    }
    if b + e == 15 {
        num_15s += 1;
    }
    if c + d == 15 {
        num_15s += 1;
    }
    if c + e == 15 {
        num_15s += 1;
    }
    if d + e == 15 {
        num_15s += 1;
    }

    2 * num_15s
}

fn score_pairs(hand: &Hand) -> i32 {
    let mut num_pairs = 0;
    for (ai, a) in (&hand.cards)[..hand.num_cards() - 1].iter().enumerate() {
        for b in (&hand.cards)[ai+1..hand.num_cards()].iter() {
            if a.rank == b.rank {
                num_pairs += 1;
            }
        }
    }
    2 * num_pairs
}

fn score_runs(hand: &Hand) -> i32 {
    assert!(hand.num_cards() == 5);

    // Make a sorted sequence of the orders of the cards in the hand.
    // The order of Ace is 1, Two is 2, ..., Ten is 10, Jack is 11,
    // Queen is 12, King is 13.
    let mut orders: Vec<i32> = (&hand.cards).into_iter()
        .map(|card| { card.rank as i32 }).collect();
    orders.sort();

    struct Pattern {
        score: i32,
        delta: [i32; 4],
    };
    let x = -1; // match any rank
    let patterns = [
        Pattern{score: 12, delta: [ 0, 1, 1, 0 ]}, // AA233
        Pattern{score:  9, delta: [ 1, 1, 0, 0 ]}, // A2333
        Pattern{score:  9, delta: [ 1, 0, 0, 1 ]}, // A2223
        Pattern{score:  9, delta: [ 0, 0, 1, 1 ]}, // AAA23
        Pattern{score:  8, delta: [ 1, 1, 1, 0 ]}, // A2344
        Pattern{score:  8, delta: [ 1, 1, 0, 1 ]}, // A2334
        Pattern{score:  8, delta: [ 1, 0, 1, 1 ]}, // A2234
        Pattern{score:  8, delta: [ 0, 1, 1, 1 ]}, // AA234
        Pattern{score:  6, delta: [ x, 1, 1, 0 ]}, // xA233
        Pattern{score:  6, delta: [ x, 1, 0, 1 ]}, // xA223
        Pattern{score:  6, delta: [ x, 0, 1, 1 ]}, // xAA23
        Pattern{score:  6, delta: [ 1, 1, 0, x ]}, // A233x
        Pattern{score:  6, delta: [ 1, 0, 1, x ]}, // A223x
        Pattern{score:  6, delta: [ 0, 1, 1, x ]}, // AA23x
        Pattern{score:  5, delta: [ 1, 1, 1, 1 ]}, // A2345
        Pattern{score:  4, delta: [ x, 1, 1, 1 ]}, // xA234
        Pattern{score:  4, delta: [ 1, 1, 1, x ]}, // A234x
        Pattern{score:  3, delta: [ x, x, 1, 1 ]}, // xxA23
        Pattern{score:  3, delta: [ x, 1, 1, x ]}, // xA23x
        Pattern{score:  3, delta: [ 1, 1, x, x ]}, // A23xx
    ];

    // Compare the sorted hand to the patterns.  Look at the difference between
    // the two cards in each pair of adjacent cards.  Stop at the first match.
    for pattern in patterns.iter() {
        let mut previous = orders[0];
        let mut j = 0;
        loop {
            let delta = pattern.delta[j];
            let order = orders[j + 1];
            if delta != x && delta != order - previous {
                break
            }
            previous = order;
            j += 1;
            if j == 4 {
                return pattern.score;
            }
        }
    }
    0
}

fn score_flush(hand: &Hand, is_crib: bool) -> i32 {
    assert!(hand.num_cards() == 5);
    let suit = hand.cards[0].suit;
    for card in hand.cards[1..4].iter() {
        if suit != card.suit {
            return 0;
        }
    }
    // First 4 are the same suit, check the cut card
    if suit == hand.cards[4].suit {
        return 5;
    }
    // In the crib, a flush counts only if all five cards are the same suit
    if is_crib {
        return 0;
    }
    4
}

fn score_nobs(hand: &Hand) -> i32 {
    // nobs: one point for the Jack of the same suit as the cut card
    assert!(hand.num_cards() == 5);
    let cut_suit = hand.cards[4].suit;
    for card in hand.cards[..4].iter() {
        if card.rank == Rank::Jack && card.suit == cut_suit {
            return 1;
        }
    }
    0
}

fn score_hand(hand: &Hand, is_crib: bool) -> i32 {
    score_fifteens(&hand) +
    score_pairs(&hand) +
    score_runs(&hand) +
    score_flush(&hand, is_crib) +
    score_nobs(&hand)
}

fn make_deck(exclude: &Hand) -> Hand {
    // Make an entire deck of cards but leave out any cards in `exclude`
    let mut deck = Hand::new();
    for suit in &SUITS {
        for rank in &RANKS {
            let card = Card{rank: *rank, suit: *suit};
            if !exclude.has(&card) {
                deck.push_back(card);
            }
        }
    }
    deck
}

const MAX_SCORE: i32 = 29 + 24;  // 29 in hand, 24 in crib (44665)
const MIN_SCORE: i32 = -29;      // 0 in hand, 29 in opp crib
const NUM_SCORES: i32 = MAX_SCORE - MIN_SCORE + 1;

struct Tally {
    scores: [i32; NUM_SCORES as usize],
}

impl Tally {
    pub fn new() -> Tally { // TODO: impl Default?
        Tally {
            scores: [0; NUM_SCORES as usize],
        }
    }

    pub fn increment(&mut self, score: i32) {
        let i: usize = (score - MIN_SCORE) as usize;
        self.scores[i] += 1;
    }
}

struct Statistics {
    mean: f64,
    stdev: f64,
    min: i32,
    max: i32,
}

impl fmt::Display for Statistics {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:.1} {:.1} {}..{}", self.mean, self.stdev, self.min, self.max)
    }
}

impl Statistics {
    fn make(tally: Tally, num_hands: i32) -> Statistics {
        let mut min = 0;
        for (i, score) in tally.scores.iter().enumerate() {
            if *score != 0 {
                min = (i as i32) + MIN_SCORE;
                break;
            }
        }

        let mut max = 0;
        for (i, score) in tally.scores.iter().enumerate().rev() {
            if *score != 0 {
                max = (i as i32) + MIN_SCORE;
                break;
            }
        }

        let mut sum = 0.0;
        for score in min..=max {
            sum += (score * tally.scores[(score - MIN_SCORE) as usize]) as f64;
        }
        let mean = sum / (num_hands as f64);

        let mut sumdev = 0.0;
        for score in min..=max {
            let d = (score as f64) - mean;
            sumdev += d * d;
        }
        let stdev = (sumdev / (num_hands as f64)).sqrt();

        Statistics{mean, stdev, min, max}
    }
}

fn analyze_hand(hand: &Hand) {
    /*
      Find all possible pairs of cards to discard to the crib.
      There are C(6,2)=15 possible discards in a cribbage hand.
     */
    println!("[ {} ]", hand);

    for from_hand in hand.cards.iter().combinations(2) {
        let discarding = Hand::from_vec(from_hand);

        let mut keeping = Hand::new();
        for card in hand.cards.iter() {
            if !discarding.has(&card) {
                keeping.push_back(*card);
            }
        }

        let deck = make_deck(hand);
        let mut mine_tally = Tally::new();    // scores when the crib is mine
        let mut theirs_tally = Tally::new();  // scores then the crib is theirs
        let mut num_hands = 0;
        for from_deck in deck.cards.iter().combinations(3) {
            num_hands += 1;
            let cut = from_deck[2];

            let mut hold = keeping.clone();
            hold.push_back(*cut);

            let mut crib = discarding.clone();
            crib.push_back(*from_deck[0]);
            crib.push_back(*from_deck[1]);
            crib.push_back(*cut);

            let hold_score = score_hand(&hold, false);
            let crib_score = score_hand(&crib, true);

            let mine_score = hold_score + crib_score;
            let theirs_score = hold_score - crib_score;

            mine_tally.increment(mine_score);
            theirs_tally.increment(theirs_score);
        }
        assert!(num_hands == 15180); // sanity check, expecting C(46,3)

        let if_mine = Statistics::make(mine_tally, num_hands);
        let if_theirs = Statistics::make(theirs_tally, num_hands);

        println!("{} [{}] [{}]", discarding, if_mine, if_theirs);
    }
}

fn main() {
    assert!("5H 5C 5S JD 5D" == Hand::make("5H 5C 5S JD 5D").to_string());
    assert!("5H 5C 5S JD 5D" == Hand::make("5h5c5sjd5d").to_string());
    assert!("AH AS JH AC AD" == Hand::make("ah-as-jh-ac-ad").to_string());

    assert!(4 == score_fifteens(&Hand::make("AH 2H 3H JH QH")));
    assert!(8 == score_fifteens(&Hand::make("5H 2H 3H JH QH")));
    assert!(16 == score_fifteens(&Hand::make("5H 5S 5C 5D TH")));
    assert!(8 == score_fifteens(&Hand::make("6C 6D 4D 4S 5D")));

    assert!(12 == score_pairs(&Hand::make("5H 5S 5C 5D TH")));
    assert!(8 == score_pairs(&Hand::make("TS 5S 5C 5D TH")));
    assert!(4 == score_pairs(&Hand::make("6C 6D 4D 4S 5D")));

    assert!(9 == score_runs(&Hand::make("AH 2H 3H 3D 3C")));
    assert!(9 == score_runs(&Hand::make("KH KD KC JH QH")));  // same pattern A2333
    assert!(9 == score_runs(&Hand::make("AH 2H 2D 2C 3H")));
    assert!(9 == score_runs(&Hand::make("AH AD AC 2H 3H")));
    assert!(8 == score_runs(&Hand::make("AH 2H 3H 4H 4D")));
    assert!(8 == score_runs(&Hand::make("AH 2H 3H 3D 4H")));
    assert!(8 == score_runs(&Hand::make("AH 2H 2C 3H 4H")));
    assert!(8 == score_runs(&Hand::make("AS AH 2H 3H 4H")));
    assert!(6 == score_runs(&Hand::make("JH AH 2H 3D 3H")));
    assert!(6 == score_runs(&Hand::make("JH AH 2S 2H 3H")));
    assert!(6 == score_runs(&Hand::make("JH AH AS 2H 3H")));
    assert!(6 == score_runs(&Hand::make("AH 2H 3S 3H JH")));
    assert!(6 == score_runs(&Hand::make("AH 2H 2S 3H JH")));
    assert!(6 == score_runs(&Hand::make("AH AS 2H 3H JH")));
    assert!(5 == score_runs(&Hand::make("AH 2H 3H 4H 5H")));
    assert!(4 == score_runs(&Hand::make("JH AH 2H 3H 4H")));
    assert!(4 == score_runs(&Hand::make("AH 2H 3H 4H JH")));
    assert!(3 == score_runs(&Hand::make("JH QH AH 2H 3H")));
    assert!(3 == score_runs(&Hand::make("JH AH 2H 3H TH")));
    assert!(3 == score_runs(&Hand::make("AH 2H 3H JH TH")));
    assert!(0 == score_runs(&Hand::make("AH 8H 3H JH TH")));
    assert!(12 == score_runs(&Hand::make("6C 6D 4D 4S 5D")));

    assert!(5 == score_flush(&Hand::make("5H 6H 7H 8H 9H"), false));
    assert!(4 == score_flush(&Hand::make("5H 6H 7H 8H 9D"), false));
    assert!(0 == score_flush(&Hand::make("5H 6H 7H 8H 9D"), true));
    assert!(0 == score_flush(&Hand::make("5H 6H 7H 8D 9D"), false));

    assert!(1 == score_nobs(&Hand::make("JH 2C 3C 4C 5H")));
    assert!(0 == score_nobs(&Hand::make("JH 2C 3C 4C 5C")));

    assert!(12 == score_hand(&Hand::make("AH AS JH AC AD"), false)); // 4oak ("of a kind")
    assert!(13 == score_hand(&Hand::make("AH AS JD AC AD"), false)); // ...plus right jack
    assert!(5 == score_hand(&Hand::make("AH 3H 7H TH JH"), false));  // 5 hearts
    assert!(5 == score_hand(&Hand::make("AH 3H 7H TH JH"), true));   // 5 hearts but crib
    assert!(4 == score_hand(&Hand::make("AH 3H 7H TH JS"), false));  // 4 hearts
    assert!(0 == score_hand(&Hand::make("AH 3H 7S TH JH"), false));  // 4 hearts but with cut
    assert!(0 == score_hand(&Hand::make("AH 3H 7H TH JS"), true));   // 4 hearts but crib
    assert!(4 + 3 == score_hand(&Hand::make("AH 2S 3C 5D JH"), false)); // 15/4 + run/3
    assert!(12 + 6 + 2 == score_hand(&Hand::make("7H 7S 7C 8D 8H"), false)); // 15/12 + 3oak + 2oak
    assert!(15 == score_hand(&Hand::make("AH 2H 3H 3S 3D"), false)); // triple run/3
    assert!(15 == score_hand(&Hand::make("3H AH 3S 2H 3D"), false)); // triple run/3
    assert!(29 == score_hand(&Hand::make("5H 5C 5S JD 5D"), false));
    assert!(28 == score_hand(&Hand::make("5H 5C 5S 5D JD"), false));
    assert!(24 == score_hand(&Hand::make("6C 4D 6D 4S 5D"), false));

    {
        let mut t = Tally::new();
        for (i, v) in [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 907, 411, 1419, 650, 1855, 663, 1908, 931,
                        1671, 650, 1699, 530, 607, 137, 291, 160, 228, 111, 66, 106,
                        5, 61, 7, 26, 0, 30, 0, 41, 0, 4, 3, 0, 0, 0, 2, 0,
                        0, 1 ].iter().enumerate() {
            t.scores[i] = *v;
        }
        let s = Statistics::make(t, 15180);
        assert!(s.to_string() == "22.9 0.8 16..53");
    }

    let args: Vec<String> = std::env::args().collect();
    for arg in args[1..].iter() {
        analyze_hand(&Hand::make(&arg));
    }
}
