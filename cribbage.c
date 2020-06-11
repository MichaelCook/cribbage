/* Copyright (c) 2020, Michael Cook <michael@waxrat.com>. All rights reserved. */

/*
 Analyze cribbage hands.

 Given a cribbage hand (six cards), which two cards should you discard
 to the crib to maximize your chances of getting the best score?
*/

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

__attribute__((noreturn))
static void die(char const* fmt, ...)
{
    fprintf(stderr, "ERROR: ");
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}

typedef int Rank; // 'A', '2', '3', ..., 'J', 'Q', 'K'
typedef int Suit; // 'S', 'D', 'C', 'H'

typedef struct Card {
    Rank rank;
    Suit suit;
} Card;

static void Card_init(Card* this, Rank rank, Suit suit)
{
    this->rank = rank;
    this->suit = suit;
}

static bool Card_equal(Card a, Card b)
{
    return a.rank == b.rank && a.suit == b.suit;
}

#define MAX_CARDS 52

typedef struct Hand {
    size_t num_cards;
    Card cards[MAX_CARDS];
} Hand;

static void Hand_init(Hand* this)
{
    this->num_cards = 0;
}

static size_t Hand_size(Hand const* this)
{
    return this->num_cards;
}

static void Hand_copy(Hand* this, Hand const* that)
{
    size_t num_cards = that->num_cards;
    this->num_cards = num_cards;
    for (size_t i = 0; i < num_cards; ++i)
        this->cards[i] = that->cards[i];
}

static Card Hand_card(Hand const* this, size_t i)
{
    assert(i < this->num_cards);
    return this->cards[i];
}

static int Hand_value(Hand const* this, size_t i)
{
    Rank r = Hand_card(this, i).rank;
    switch (r) {
    case 'A':
        return 1;
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return r - '0';
    case 'T':
    case 'J':
    case 'Q':
    case 'K':
        return 10;
    default:
        assert(false);
        return 0;
    }
}

static int Hand_order(Hand const* this, size_t i)
{
    Rank r = Hand_card(this, i).rank;
    switch (r) {
    case 'A':
        return 1;
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return r - '0';
    case 'T':
        return 10;
    case 'J':
        return 11;
    case 'Q':
        return 12;
    case 'K':
        return 13;
    default:
        assert(false);
        return 0;
    }
}

static void Hand_push(Hand* this, Card card)
{
    assert(this->num_cards < MAX_CARDS);
    this->cards[this->num_cards++] = card;
}

static void Hand_pop(Hand* this)
{
    assert(this->num_cards > 0);
    --this->num_cards;
}

static bool Hand_has(Hand const* this, Card card)
{
    for (size_t i = 0; i != this->num_cards; ++i)
        if (Card_equal(card, this->cards[i]))
            return true;
    return false;
}

static Hand* Hand_make(Hand* hand, char const *text)
{
    Hand_init(hand);
    Rank rank = 0;
    for (char const* s = text; *s; ++s) {
        int c = toupper((unsigned char) *s);
        switch (c) {
        case 'H':
        case 'C':
        case 'S':
        case 'D':
            {
                if (rank == 0)
                    die("Malformed hand '%s'", text);
                Card card;
                Card_init(&card, rank, c);
                Hand_push(hand, card);
                rank = 0;
            }
            break;
        case 'A':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case 'T':
        case 'J':
        case 'Q':
        case 'K':
            {
                if (rank != 0)
                    die("Malformed hand '%s'", text);
                rank = c;
            }
            break;
        case ' ':
        case '-':
            break;
        default:
            die("Malformed hand '%s'", text);
        }
    }
    if (rank != 0)
        die("Malformed hand '%s'", text);
    return hand;
}

static void Hand_print(Hand const* this)
{
    bool sep = false;
    for (size_t i = 0; i < this->num_cards; ++i) {
        if (sep)
            putchar(' ');
        sep = true;
        putchar(this->cards[i].rank);
        putchar(this->cards[i].suit);
    }
}

static int score_fifteens(Hand const* hand) {
    int a = Hand_value(hand, 0);
    int b = Hand_value(hand, 1);
    int c = Hand_value(hand, 2);
    int d = Hand_value(hand, 3);
    int e = Hand_value(hand, 4);
    int num_15s = 0;

    // five cards - C(5,5)=1
    if (a + b + c + d + e == 15)
        ++num_15s;

    // four cards - C(5,4)=5
    if (a + b + c + d == 15)
        ++num_15s;
    if (a + b + c + e == 15)
        ++num_15s;
    if (a + b + d + e == 15)
        ++num_15s;
    if (a + c + d + e == 15)
        ++num_15s;
    if (b + c + d + e == 15)
        ++num_15s;

    // three cards - C(5,3)=10
    if (a + b + c == 15)
        ++num_15s;
    if (a + b + d == 15)
        ++num_15s;
    if (a + b + e == 15)
        ++num_15s;
    if (a + c + d == 15)
        ++num_15s;
    if (a + c + e == 15)
        ++num_15s;
    if (a + d + e == 15)
        ++num_15s;
    if (b + c + d == 15)
        ++num_15s;
    if (b + c + e == 15)
        ++num_15s;
    if (b + d + e == 15)
        ++num_15s;
    if (c + d + e == 15)
        ++num_15s;

    // two cards - C(5,2)=10
    if (a + b == 15)
        ++num_15s;
    if (a + c == 15)
        ++num_15s;
    if (a + d == 15)
        ++num_15s;
    if (a + e == 15)
        ++num_15s;
    if (b + c == 15)
        ++num_15s;
    if (b + d == 15)
        ++num_15s;
    if (b + e == 15)
        ++num_15s;
    if (c + d == 15)
        ++num_15s;
    if (c + e == 15)
        ++num_15s;
    if (d + e == 15)
        ++num_15s;

    return 2 * num_15s;
}

static int score_pairs(Hand const* hand)
{
    size_t n = Hand_size(hand);
    int num_pairs = 0;
    for (size_t i = 0; i < n - 1; ++i) {
        Card card = Hand_card(hand, i);
        for (size_t j = i; ++j < n;) {
            if (card.rank == Hand_card(hand, j).rank)
                ++num_pairs;
        }
    }

    return 2 * num_pairs;
}

#define X -1 // match any rank
typedef struct Pattern {
    int score;
    int delta[4];
} Pattern;
static Pattern const patterns[] = {
    { 12, { 0, 1, 1, 0 } }, // AA233
    {  9, { 1, 1, 0, 0 } }, // A2333
    {  9, { 1, 0, 0, 1 } }, // A2223
    {  9, { 0, 0, 1, 1 } }, // AAA23
    {  8, { 1, 1, 1, 0 } }, // A2344
    {  8, { 1, 1, 0, 1 } }, // A2334
    {  8, { 1, 0, 1, 1 } }, // A2234
    {  8, { 0, 1, 1, 1 } }, // AA234
    {  6, { X, 1, 1, 0 } }, // xA233
    {  6, { X, 1, 0, 1 } }, // xA223
    {  6, { X, 0, 1, 1 } }, // xAA23
    {  6, { 1, 1, 0, X } }, // A233x
    {  6, { 1, 0, 1, X } }, // A223x
    {  6, { 0, 1, 1, X } }, // AA23x
    {  5, { 1, 1, 1, 1 } }, // A2345
    {  4, { X, 1, 1, 1 } }, // xA234
    {  4, { 1, 1, 1, X } }, // A234x
    {  3, { X, X, 1, 1 } }, // xxA23
    {  3, { X, 1, 1, X } }, // xA23x
    {  3, { 1, 1, X, X } }, // A23xx
};
#define NUM_PATTERNS (sizeof(patterns) / sizeof(patterns[0]))

static int score_runs(Hand const* hand)
{
    assert(Hand_size(hand) == 5);

    // Make a sorted copy of the hand, but use only the order
    // of each rank, ignore the suit.
    int orders[5];
    for (size_t i = 0; i < 5; ++i) {
        int order = Hand_order(hand, i);
        for (size_t j = 0;; ++j) {
            if (j == i) {
                // insert at end
                orders[i] = order;
                break;
            }
            if (order < orders[j]) {
                // insert before [j]
                do {
                    int tmp = order;
                    order = orders[j];
                    orders[j] = tmp;
                } while (++j < i);
                orders[j] = order;
                break;
            }
        }
    }
    for (size_t i = 0; i < NUM_PATTERNS; ++i) {
        Pattern const* pattern = &patterns[i];
        int previous = orders[0];
        for (size_t j = 0;; ++j) {
            if (j == 4)
                return pattern->score;
            int delta = pattern->delta[j];
            int order = orders[j + 1];
            if (delta != X && delta != order - previous)
                break;
            previous = order;
        }
    }

    return 0;
}

static int score_flush(Hand const* hand, bool is_crib)
{
    assert(Hand_size(hand) == 5);
    int suit = Hand_card(hand, 0).suit;
    for (size_t i = 1; i < 4; ++i)
        if (suit != Hand_card(hand, i).suit)
            return 0;
    // first 4 are same suit
    if (suit == Hand_card(hand, 4).suit)
        return 5;
    // In the crib, a flush counts only if all five cards are the same suit.
    if (is_crib)
        return 0;
    return 4;
}

static int score_nobs(Hand const* hand)
{
    size_t n = Hand_size(hand);
    assert(n != 0);
    --n;
    int cut_suit = Hand_card(hand, n).suit;
    for (size_t i = 0; i < n; ++i) {
        Card card = Hand_card(hand, i);
        if (card.rank == 'J' && card.suit == cut_suit)
            return 1;
    }
    return 0;
}

static int score_hand(Hand const* hand, bool is_crib)
{
    return score_fifteens(hand) +
        score_pairs(hand) +
        score_runs(hand) +
        score_flush(hand, is_crib) +
        score_nobs(hand);
}

static void make_deck(Hand* deck, Hand const* exclude)
{
    static char const suits[] = "HCDS";
    static char const ranks[] = "A23456789TJQK";
    Hand_init(deck);
    for (char const* suit = suits; *suit; ++suit) {
        for (char const* rank = ranks; *rank; ++rank) {
            Card card;
            Card_init(&card, *rank, *suit);
            if (!Hand_has(exclude, card))
                Hand_push(deck, card);
        }
    }
}

#define MAX_SCORE (29 + 24)           // 29 in hand, 24 in crib (44665)
#define MIN_SCORE (-29)               // 0 in hand, 29 in opp crib
#define NUM_SCORES (MAX_SCORE - MIN_SCORE + 1)

typedef struct Tally {
    int scores[NUM_SCORES];
} Tally;

static void Tally_init(Tally* this)
{
    memset(this->scores, 0, sizeof(this->scores));
}

static void Tally_increment(Tally* this, int score)
{
    int i = score - MIN_SCORE;
    assert(i >= 0 && i < NUM_SCORES);
    ++this->scores[i];
}

typedef struct Statistics {
    double mean, stdev;
    int min, max;
} Statistics;

static void Statistics_init(Statistics* this, Tally const* t, int num_hands)
{
    int min = 0;
    for (int i = 0; i < NUM_SCORES; ++i)
        if (t->scores[i] != 0) {
            min = i + MIN_SCORE;
            break;
        }

    int max = 0;
    for (int i = NUM_SCORES; --i >= 0;)
        if (t->scores[i] != 0) {
            max = i + MIN_SCORE;
            break;
        }

    double sum = 0;
    for (int score = min; score <= max; ++score)
        sum += (double) score * t->scores[score - MIN_SCORE];
    double const mean = sum / num_hands;

    double sumdev = 0;
    for (int score = min; score <= max; ++score) {
        double d = score - mean;
        sumdev += d * d;
    }
    double const stdev = sqrt(sumdev / num_hands);

    this->min = min;
    this->max = max;
    this->mean = mean;
    this->stdev = stdev;
}

static char const* Statistics_toString(Statistics const* this, char* buf, size_t size)
{
    snprintf(buf, size, "%.1f %.1f %d..%d", this->mean, this->stdev, this->min, this->max);
    return buf;
}

static void Statistics_print(Statistics const* this)
{
    char buf[64];
    printf("%s", Statistics_toString(this, buf, sizeof(buf)));
}

#define MAX_CHOOSE 3

typedef struct Choose {
    Hand const* hand;
    size_t num_choose;
    Hand* chosen;
    size_t i;
    size_t i_stack_depth;
    size_t i_stack[MAX_CHOOSE];
    bool yielded;
} Choose;

static void Choose_init(Choose* this, Hand const* hand, size_t num_choose, Hand* chosen)
{
    Hand_init(chosen);
    this->hand = hand;
    this->num_choose = num_choose;
    this->chosen = chosen;
    this->i = 0;
    this->i_stack_depth = 0;
    this->yielded = false;
}

static bool Choose_next(Choose* this)
{
    if (this->yielded) {
        this->yielded = false;
        Hand_pop(this->chosen);
        --this->i_stack_depth;
        assert(this->i_stack_depth >= 0 && this->i_stack_depth < MAX_CHOOSE);
        this->i = this->i_stack[this->i_stack_depth] + 1;
    }

    for (;;) {
        if (Hand_size(this->chosen) == this->num_choose) {
            this->yielded = true;
            return true;
        }
        if (this->i != Hand_size(this->hand)) {
            Hand_push(this->chosen, Hand_card(this->hand, this->i));
            assert(this->i_stack_depth >= 0 && this->i_stack_depth < MAX_CHOOSE);
            this->i_stack[this->i_stack_depth] = this->i++;
            this->i_stack_depth++;
        }
        else if (this->i_stack_depth > 0) {
            Hand_pop(this->chosen);
            this->i_stack_depth--;
            assert(this->i_stack_depth >= 0 && this->i_stack_depth < MAX_CHOOSE);
            this->i = this->i_stack[this->i_stack_depth] + 1;
        }
        else {
            return false;
        }
    }
}

static void analyze_hand(Hand const* hand)
{
    /*
      Find all possible pairs of cards to discard to the crib.
      There are C(6,2)=15 possible discards in a cribbage hand.
    */
    printf("[ ");
    Hand_print(hand);
    printf(" ]\n");

    Choose choose_to_discard;
    Hand discard;
    Choose_init(&choose_to_discard, hand, 2, &discard);
    while (Choose_next(&choose_to_discard)) {
        Hand keep;
        Hand_init(&keep);
        for (size_t i = 0; i != Hand_size(hand); ++i) {
            Card card = Hand_card(hand, i);
            if (!Hand_has(&discard, card))
                Hand_push(&keep, card);
        }

        Hand deck;
        make_deck(&deck, hand);
        assert(Hand_size(&deck) == 46);

        Tally mine_tally;           // scores when the crib is mine
        Tally_init(&mine_tally);

        Tally theirs_tally;         // scores then the crib is theirs
        Tally_init(&theirs_tally);

        int num_hands = 0;
        Choose choose_from_deck;
        Hand chosen;
        Choose_init(&choose_from_deck, &deck, 3, &chosen);
        while (Choose_next(&choose_from_deck)) {
            Card cut = Hand_card(&chosen, 2);

            Hand hold;
            Hand_copy(&hold, &keep);
            Hand_push(&hold, cut);

            Hand crib;
            Hand_copy(&crib, &discard);
            Hand_push(&crib, Hand_card(&chosen, 0));
            Hand_push(&crib, Hand_card(&chosen, 1));
            Hand_push(&crib, cut);

            int hold_score = score_hand(&hold, false);
            int crib_score = score_hand(&crib, true);

            int mine_score = hold_score + crib_score;
            int theirs_score = hold_score - crib_score;

            Tally_increment(&mine_tally, mine_score);
            Tally_increment(&theirs_tally, theirs_score);
            ++num_hands;
        }
        assert(num_hands == 15180); // sanity check, expecting C(46,3)

        /* Calculate statistics (mean, standard deviation, min and max)
           for both situations when it's my crib and when it's theirs. */
        Statistics if_mine;
        Statistics_init(&if_mine, &mine_tally, num_hands);

        Statistics if_theirs;
        Statistics_init(&if_theirs, &theirs_tally, num_hands);

        Hand_print(&discard);
        printf(" [");
        Statistics_print(&if_mine);
        printf("] [");
        Statistics_print(&if_theirs);
        printf("]\n");
    }

    putchar('\n');
}

// ---------------------------------------------------------------------------

#define CHECK(EXPR) do {                                                \
    if (!(EXPR)) {                                                      \
        die("CHECK failed at %s line %d", __FILE__, __LINE__);          \
    }                                                                   \
} while (0)

int main(int argc __attribute__((unused)), char **argv)
{
    Hand hand;

    CHECK(4 == score_fifteens(Hand_make(&hand, "AH 2H 3H JH QH")));
    CHECK(8 == score_fifteens(Hand_make(&hand, "5H 2H 3H JH QH")));
    CHECK(16 == score_fifteens(Hand_make(&hand, "5H 5S 5C 5D TH")));
    CHECK(8 == score_fifteens(Hand_make(&hand, "6C 6D 4D 4S 5D")));
    CHECK(12 == score_pairs(Hand_make(&hand, "5H 5S 5C 5D TH")));
    CHECK(8 == score_pairs(Hand_make(&hand, "TS 5S 5C 5D TH")));
    CHECK(4 == score_pairs(Hand_make(&hand, "6C 6D 4D 4S 5D")));
    CHECK(9 == score_runs(Hand_make(&hand, "AH 2H 3H 3D 3C")));
    CHECK(9 == score_runs(Hand_make(&hand, "KH KD KC JH QH")));  // same pattern A2333
    CHECK(9 == score_runs(Hand_make(&hand, "AH 2H 2D 2C 3H")));
    CHECK(9 == score_runs(Hand_make(&hand, "AH AD AC 2H 3H")));
    CHECK(8 == score_runs(Hand_make(&hand, "AH 2H 3H 4H 4D")));
    CHECK(8 == score_runs(Hand_make(&hand, "AH 2H 3H 3D 4H")));
    CHECK(8 == score_runs(Hand_make(&hand, "AH 2H 2C 3H 4H")));
    CHECK(8 == score_runs(Hand_make(&hand, "AS AH 2H 3H 4H")));
    CHECK(6 == score_runs(Hand_make(&hand, "JH AH 2H 3D 3H")));
    CHECK(6 == score_runs(Hand_make(&hand, "JH AH 2S 2H 3H")));
    CHECK(6 == score_runs(Hand_make(&hand, "JH AH AS 2H 3H")));
    CHECK(6 == score_runs(Hand_make(&hand, "AH 2H 3S 3H JH")));
    CHECK(6 == score_runs(Hand_make(&hand, "AH 2H 2S 3H JH")));
    CHECK(6 == score_runs(Hand_make(&hand, "AH AS 2H 3H JH")));
    CHECK(5 == score_runs(Hand_make(&hand, "AH 2H 3H 4H 5H")));
    CHECK(4 == score_runs(Hand_make(&hand, "JH AH 2H 3H 4H")));
    CHECK(4 == score_runs(Hand_make(&hand, "AH 2H 3H 4H JH")));
    CHECK(3 == score_runs(Hand_make(&hand, "JH QH AH 2H 3H")));
    CHECK(3 == score_runs(Hand_make(&hand, "JH AH 2H 3H TH")));
    CHECK(3 == score_runs(Hand_make(&hand, "AH 2H 3H JH TH")));
    CHECK(0 == score_runs(Hand_make(&hand, "AH 8H 3H JH TH")));
    CHECK(12 == score_runs(Hand_make(&hand, "6C 6D 4D 4S 5D")));
    CHECK(5 == score_flush(Hand_make(&hand, "5H 6H 7H 8H 9H"), false));
    CHECK(4 == score_flush(Hand_make(&hand, "5H 6H 7H 8H 9D"), false));
    CHECK(0 == score_flush(Hand_make(&hand, "5H 6H 7H 8H 9D"), true));
    CHECK(0 == score_flush(Hand_make(&hand, "5H 6H 7H 8D 9D"), false));
    CHECK(1 == score_nobs(Hand_make(&hand, "JH 2C 3C 4C 5H")));
    CHECK(0 == score_nobs(Hand_make(&hand, "JH 2C 3C 4C 5C")));
    CHECK(12 == score_hand(Hand_make(&hand, "AH AS JH AC AD"), false)); // 4oak ("of a kind")
    CHECK(13 == score_hand(Hand_make(&hand, "AH AS JD AC AD"), false)); // ...plus right jack
    CHECK(5 == score_hand(Hand_make(&hand, "AH 3H 7H TH JH"), false));  // 5 hearts
    CHECK(5 == score_hand(Hand_make(&hand, "AH 3H 7H TH JH"), true));   // 5 hearts but crib
    CHECK(4 == score_hand(Hand_make(&hand, "AH 3H 7H TH JS"), false));  // 4 hearts
    CHECK(0 == score_hand(Hand_make(&hand, "AH 3H 7S TH JH"), false));  // 4 hearts but with cut
    CHECK(0 == score_hand(Hand_make(&hand, "AH 3H 7H TH JS"), true));   // 4 hearts but crib
    CHECK(4 + 3 == score_hand(Hand_make(&hand, "AH 2S 3C 5D JH"), false)); // 15/4 + run/3
    CHECK(12 + 6 + 2 == score_hand(Hand_make(&hand, "7H 7S 7C 8D 8H"), false)); // 15/12 + 3oak + 2oak
    CHECK(15 == score_hand(Hand_make(&hand, "AH 2H 3H 3S 3D"), false)); // triple run/3
    CHECK(15 == score_hand(Hand_make(&hand, "3H AH 3S 2H 3D"), false)); // triple run/3
    CHECK(29 == score_hand(Hand_make(&hand, "5H 5C 5S JD 5D"), false));
    CHECK(28 == score_hand(Hand_make(&hand, "5H 5C 5S 5D JD"), false));
    CHECK(24 == score_hand(Hand_make(&hand, "6C 4D 6D 4S 5D"), false));

    {
        Tally t = {
            {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 907, 411, 1419, 650, 1855, 663, 1908, 931,
                1671, 650, 1699, 530, 607, 137, 291, 160, 228, 111, 66, 106,
                5, 61, 7, 26, 0, 30, 0, 41, 0, 4, 3, 0, 0, 0, 2, 0, 0, 1
            }
        };
        Statistics s;
        Statistics_init(&s, &t, 15180);
        char buf[64];
        Statistics_toString(&s, buf, sizeof(buf));
        CHECK(strcmp(buf, "22.9 0.8 16..53") == 0);
    }

    while (*++argv)
        analyze_hand(Hand_make(&hand, *argv));
}
