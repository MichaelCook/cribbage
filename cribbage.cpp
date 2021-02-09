/* Copyright (c) 2016, Michael Cook <michael@waxrat.com>. All rights reserved. */

/*
 Analyze cribbage hands.

 Given a cribbage hand (six cards), which two cards should you discard
 to the crib to maximize your chances of getting the best score?
*/

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <cstring>
#include <string_view>
#include <type_traits>
#include <bitset>

#if 0 // 1 to facilitate constexpr/static_assert debugging
#  define constexpr
#  define token_paste_(A, B) A ## B
#  define token_paste(A, B) token_paste_(A, B)
#  define static_assert(EXPR) [[maybe_unused]] int token_paste(once, __LINE__) = (assert(EXPR), 0)
#endif

namespace {

using std::cout;
using std::endl;

constexpr char suit_chars[] = "SDCH";
constexpr char rank_chars[] = "A23456789TJQK";

using Suit = unsigned; // 0='S', 1='D', 2='C', 3='H'
using Rank = unsigned; // 0='A', 1='2', ... 12='K'

constexpr size_t bit_count(uint64_t v) noexcept
{
  if constexpr(false)
    // std::bitset count is not constexpr
    return std::bitset<64>(v).count();

  size_t n = 0;
  while (v) {
    v &= v - 1; // clear right-most 1-bit
    ++n;
  }
  return n;
}

class Hand;

class [[nodiscard]] Card {
  uint64_t card_ = 0; // a single 1-bit, or 0 if no card

  friend class Hand;

  constexpr explicit Card(uint64_t card) noexcept
  : card_(card)
  {
    assert(bit_count(card_) <= 1);
  }

public:

  constexpr Card() noexcept {};

  constexpr explicit operator bool () const noexcept {
    return card_ != 0;
  }

  constexpr Card(Rank r, Suit s) noexcept
  : card_{uint64_t(1) << (s * 16 + r)}
  {
    assert(s < 4);
    assert(r < 13);
    assert(bit_count(card_) == 1);
  }

  constexpr Rank rank() const noexcept {
    assert(bit_count(card_) == 1);
    uint64_t n = bit_count(card_ - 1);
    return n % 16;
  }

  constexpr Suit suit() const noexcept {
    assert(bit_count(card_) == 1);
    uint64_t n = bit_count(card_ - 1);
    return n / 16;
  }

  constexpr int value() const noexcept {
    return std::min(rank() + 1, 10u); // 1..10
  }

};

constexpr Rank rank_from_char(char c) noexcept {
  // std::strchr is not constexpr
  for (const char* p = rank_chars;; ++p) {
    if (*p == c)
      return p - rank_chars;
    assert(*p != '\0');
  }
}

constexpr Suit suit_from_char(char c) noexcept {
  // std::strchr is not constexpr
  for (const char* p = suit_chars;; ++p) {
    if (*p == c)
      return p - suit_chars;
    assert(*p != '\0');
  }
}

constexpr Card make_card(char rank, char suit)
{
  return Card{rank_from_char(rank), suit_from_char(suit)};
}

constexpr Card make_card(std::string_view str)
{
  assert(str.length() == 2);
  return make_card(str[0], str[1]);
}

std::ostream &operator<<(std::ostream &os, Card card) {
  assert(card.rank() < sizeof(rank_chars) - 1);
  assert(card.suit() < sizeof(suit_chars) - 1);
  return os << rank_chars[card.rank()]
            << suit_chars[card.suit()];
}

class [[nodiscard]] Hand {
  uint64_t cards_ = 0;

public:

  constexpr Hand() noexcept {}

  constexpr explicit Hand(uint64_t cards) noexcept
  : cards_{cards}
  {}

  constexpr size_t size() const noexcept {
    return bit_count(cards_);
  }

  constexpr bool has(Card card) const noexcept {
    return (cards_ & card.card_) != 0;
  }

  constexpr void insert(Card card) noexcept {
    assert(!has(card));
    cards_ |= card.card_;
  }

  constexpr void remove(Card card) noexcept {
    assert(has(card));
    cards_ &= ~card.card_;
  }

  constexpr void remove(Hand other) noexcept {
    assert((cards_ & other.cards_) == other.cards_);
    cards_ &= ~other.cards_;
  }

  // Remove one card from the hand.
  // Return a false card if this hand is empty
  constexpr Card take() noexcept {
    uint64_t before = cards_;
    cards_ &= cards_ - 1; // clear right-most 1-bit
    return Card{cards_ ^ before};
  }
};

// Each of the 52 cards is represented by a 1-bit in a uint64_t.
// Each rank within a suit is a 1-bit in a uint16_t.
// 0x1fff is 13 bits representing all cards in a suit.
constexpr Hand all_cards{0x1fff'1fff'1fff'1fff};
static_assert(all_cards.size() == 52);

std::ostream &operator<<(std::ostream &os, Hand hand) {
  bool sep = false;
  while (auto card = hand.take()) {
    if (sep)
      os << ' ';
    os << card;
    sep = true;
  }
  return os;
}

constexpr char to_upper(char c) noexcept { // std::toupper is not constexpr
    if (c >= 'a' && c <= 'z')
        return c - 'a' + 'A';
    return c;
}

constexpr bool is_space(char c) noexcept { // std::isspace is not constexpr
    return c == ' ';
}

constexpr Hand make_hand(std::string_view str) {
  Hand h;
  char rank_char = 0;
  for (auto c : str) {
    c = to_upper(c);
    switch (c) {
    case 'H':
    case 'C':
    case 'S':
    case 'D':
      if (rank_char == 0)
        throw std::runtime_error("Malformed hand '" + std::string(str) + '\'');
      h.insert(make_card(rank_char, c));
      rank_char = 0;
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
      if (rank_char != 0)
        throw std::runtime_error("Malformed hand '" + std::string(str) + '\'');
      rank_char = c;
      break;
    case '-':
      break;
    default:
      if (is_space(c))
        break;
      throw std::runtime_error("Malformed hand '" + std::string(str) + '\'');
    }
  }
  if (rank_char != 0)
      throw std::runtime_error("Malformed hand '" + std::string(str));
  return h;
}

constexpr int score_15s(Hand hand, Card cut) {
  assert(hand.size() == 4);
  auto a = hand.take().value();
  auto b = hand.take().value();
  auto c = hand.take().value();
  auto d = hand.take().value();
  auto e = cut.value();
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

static_assert(4 == score_15s(make_hand("AH 2H 3H JH"), make_card("QH")));
static_assert(8 == score_15s(make_hand("5H 2H 3H JH"), make_card("QH")));
static_assert(16 == score_15s(make_hand("5H 5S 5C 5D"), make_card("TH")));
static_assert(8 == score_15s(make_hand("6C 6D 4D 4S"), make_card("5D")));

constexpr int score_pairs(Hand hand, Card cut) {
  assert(hand.size() == 4);
  auto a = hand.take().rank();
  auto b = hand.take().rank();
  auto c = hand.take().rank();
  auto d = hand.take().rank();
  auto e = cut.rank();
  int num_pairs = 0;

  if (a == b)
    ++num_pairs;
  if (a == c)
    ++num_pairs;
  if (a == d)
    ++num_pairs;
  if (a == e)
    ++num_pairs;

  if (b == c)
    ++num_pairs;
  if (b == d)
    ++num_pairs;
  if (b == e)
    ++num_pairs;

  if (c == d)
    ++num_pairs;
  if (c == e)
    ++num_pairs;

  if (d == e)
    ++num_pairs;

  return 2 * num_pairs;
}

static_assert(12 == score_pairs(make_hand("5H 5S 5C 5D"), make_card("TH")));
static_assert(8 == score_pairs(make_hand("TS 5S 5C 5D"), make_card("TH")));
static_assert(4 == score_pairs(make_hand("6C 6D 4D 4S"), make_card("5D")));

constexpr int score_runs(Hand hand, Card cut) {
  assert(hand.size() == 4);

  // Make a sorted copy of the hand, but use only the rank
  // of each card, ignore the suit.
  Rank ranks[] = {
    hand.take().rank(),
    hand.take().rank(),
    hand.take().rank(),
    hand.take().rank(),
    cut.rank(),
  };
  std::sort(std::begin(ranks), std::end(ranks));

  constexpr unsigned X = 99; // match any rank
  constexpr struct {
    int score;
    unsigned delta[4];
  } const patterns[] = {
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
  for (auto& pattern : patterns) {
    auto previous = ranks[0];
    for (size_t j = 0;; ++j) {
      if (j == 4)
        return pattern.score;
      auto delta = pattern.delta[j];
      auto rank = ranks[j + 1];
      if (delta != X && delta != rank - previous)
        break;
      previous = rank;
    }
  }

  return 0;
}

static_assert(9 == score_runs(make_hand("AH 2H 3H 3D"), make_card("3C")));
static_assert(9 == score_runs(make_hand("KH KD KC JH"), make_card("QH")));  // same pattern A2333
static_assert(9 == score_runs(make_hand("AH 2H 2D 2C"), make_card("3H")));
static_assert(9 == score_runs(make_hand("AH AD AC 2H"), make_card("3H")));
static_assert(8 == score_runs(make_hand("AH 2H 3H 4H"), make_card("4D")));
static_assert(8 == score_runs(make_hand("AH 2H 3H 3D"), make_card("4H")));
static_assert(8 == score_runs(make_hand("AH 2H 2C 3H"), make_card("4H")));
static_assert(8 == score_runs(make_hand("AS AH 2H 3H"), make_card("4H")));
static_assert(6 == score_runs(make_hand("JH AH 2H 3D"), make_card("3H")));
static_assert(6 == score_runs(make_hand("JH AH 2S 2H"), make_card("3H")));
static_assert(6 == score_runs(make_hand("JH AH AS 2H"), make_card("3H")));
static_assert(6 == score_runs(make_hand("AH 2H 3S 3H"), make_card("JH")));
static_assert(6 == score_runs(make_hand("AH 2H 2S 3H"), make_card("JH")));
static_assert(6 == score_runs(make_hand("AH AS 2H 3H"), make_card("JH")));
static_assert(5 == score_runs(make_hand("AH 2H 3H 4H"), make_card("5H")));
static_assert(4 == score_runs(make_hand("JH AH 2H 3H"), make_card("4H")));
static_assert(4 == score_runs(make_hand("AH 2H 3H 4H"), make_card("JH")));
static_assert(3 == score_runs(make_hand("JH QH AH 2H"), make_card("3H")));
static_assert(3 == score_runs(make_hand("JH AH 2H 3H"), make_card("TH")));
static_assert(3 == score_runs(make_hand("AH 2H 3H JH"), make_card("TH")));
static_assert(0 == score_runs(make_hand("AH 8H 3H JH"), make_card("TH")));
static_assert(12 == score_runs(make_hand("6C 6D 4D 4S"), make_card("5D")));

constexpr int score_flush(Hand hand, Card cut, bool is_crib) {
  assert(hand.size() == 4);
  auto a = hand.take().suit();
  auto b = hand.take().suit();
  auto c = hand.take().suit();
  auto d = hand.take().suit();
  auto e = cut.suit();

  if (a != b)
    return 0;
  if (a != c)
    return 0;
  if (a != d)
    return 0;

  // All 4 cards in `hand` are the same suit

  if (a == e)
    return 5;

  // In the crib, a flush counts only if all five cards are the same suit.
  if (is_crib)
    return 0;
  return 4;
}

static_assert(5 == score_flush(make_hand("5H 6H 7H 8H"), make_card("9H"), false));
static_assert(4 == score_flush(make_hand("5H 6H 7H 8H"), make_card("9D"), false));
static_assert(0 == score_flush(make_hand("5H 6H 7H 8H"), make_card("9D"), true));
static_assert(0 == score_flush(make_hand("5H 6H 7H 8D"), make_card("9D"), false));

constexpr int score_nobs(Hand hand, Card cut) noexcept {
  assert(hand.size() == 4);
  auto const suit = cut.suit();

  constexpr auto jack = 10;

  auto a = hand.take();
  if (a.rank() == jack && a.suit() == suit)
    return 1;

  auto b = hand.take();
  if (b.rank() == jack && b.suit() == suit)
    return 1;

  auto c = hand.take();
  if (c.rank() == jack && c.suit() == suit)
    return 1;

  auto d = hand.take();
  if (d.rank() == jack && d.suit() == suit)
    return 1;

  return 0;
}

static_assert(1 == score_nobs(make_hand("JH 2C 3C 4C"), make_card("5H")));
static_assert(0 == score_nobs(make_hand("JH 2C 3C 4C"), make_card("5C")));

constexpr int score_hand(Hand hand, Card cut, bool is_crib) {
  return score_15s(hand, cut) +
         score_pairs(hand, cut) +
         score_runs(hand, cut) +
         score_flush(hand, cut, is_crib) +
         score_nobs(hand, cut);
}

[[maybe_unused]]
constexpr int score_hand(std::string_view hand, std::string_view cut, bool is_crib) {
  return score_hand(make_hand(hand), make_card(cut), is_crib);
}

static_assert(score_hand("AH AS JH AC", "AD", false) == 12); // 4oak
static_assert(score_hand("AH AS JD AC", "AD", false) == 13); // ...plus right jack
static_assert(score_hand("AH 3H 7H TH", "JH", false) == 5);  // 5 hearts
static_assert(score_hand("AH 3H 7H TH", "JH", true) == 5);   // 5 hearts but crib
static_assert(score_hand("AH 3H 7H TH", "JS", false) == 4);  // 4 hearts
static_assert(score_hand("AH 3H 7S TH", "JH", false) == 0);  // 4 hearts but with cut
static_assert(score_hand("AH 3H 7H TH", "JS", true) == 0);   // 4 hearts but crib
static_assert(score_hand("AH 2S 3C 5D", "JH", false) == 4 + 3); // 15/4 + run/3
static_assert(score_hand("7H 7S 7C 8D", "8H", false) == 12 + 6 + 2); // 15/12 + 3oak + 2oak
static_assert(score_hand("AH 2H 3H 3S", "3D", false) == 15); // triple run/3
static_assert(score_hand("3H AH 3S 2H", "3D", false) == 15); // triple run/3
static_assert(score_hand("5H 5C 5S JD", "5D", false) == 29);
static_assert(score_hand("5H 5C 5S 5D", "JD", false) == 28);
static_assert(score_hand("6C 4D 6D 4S", "5D", false) == 24);

// ---------------------------------------------------------------------------

/* A ChoiceHandler is a type like `void f(Hand choice)`.  That is, a
   function (or function-like object) that takes one argument, a `Hand`
   object, and the returned value (if any) is ignored */
template<typename T>
concept ChoiceHandler = std::invocable<T, Hand>;

template <ChoiceHandler T>
constexpr
void for_each_choice_internal(Hand hand, size_t num_choose, Hand chosen, T const &func) {
  if (chosen.size() == num_choose) {
    func(chosen);
    return;
  }
  while (auto card = hand.take()) {
    chosen.insert(card);
    for_each_choice_internal(hand, num_choose, chosen, func);
    chosen.remove(card);
  }
}

template <ChoiceHandler T>
constexpr
void for_each_choice(Hand hand, size_t num_choose, T const &func) {
  for_each_choice_internal(hand, num_choose, Hand{}, func);
}

struct [[nodiscard]] Tally {
  static constexpr int max_score = 29 + 24; // 29 in hand, 24 in crib (44665)
  static constexpr int min_score = -29;     // 0 in hand, 29 in opp crib
  static constexpr size_t size = max_score - min_score + 1;
  int scores[size];
  void increment(int score)
  {
    int i = score - min_score;
    assert(i >= 0 && size_t(i) < size);
    ++scores[i];
  }
  Tally()
  {
    std::fill(scores, scores + size, 0);
  }
  Tally(std::initializer_list<int> s)
  {
    int i = 0;
    for (auto v : s)
        scores[i++] = v;
    assert(i == size);
  }
};

struct [[nodiscard]] Statistics {
  double mean{};
  double stdev{};
  int min{};
  int max{};

  Statistics() = delete;
  constexpr Statistics(Tally const &t, int num_hands);
};

std::ostream &operator<<(std::ostream &os, Statistics const &st) {
  os << std::fixed << std::setprecision(1);
  return os << st.mean << ' ' << st.stdev << ' ' << st.min << ".." << st.max;
}

constexpr Statistics::Statistics(Tally const &t, int num_hands) {
  min = 0;
  for (int i = 0; size_t(i) < Tally::size; ++i)
    if (t.scores[i] != 0) {
      min = i + Tally::min_score;
      break;
    }

  max = 0;
  for (int i = Tally::size; --i >= 0;)
    if (t.scores[i] != 0) {
      max = i + Tally::min_score;
      break;
    }

  double sum = 0;
  for (int score = min; score <= max; ++score)
    sum += double(score) * t.scores[score - Tally::min_score];
  mean = sum / num_hands;

  double sumdev = 0;
  for (int score = min; score <= max; ++score) {
    auto d = score - mean;
    sumdev += d * d;
  }
  stdev = sqrt(sumdev / num_hands);
}

void analyze_hand(Hand hand) {
  /*
    Find all possible pairs of cards to discard to the crib.
    There are C(6,2)=15 possible discards in a cribbage hand.
   */
  cout << "[ " << hand << " ]\n";
  assert(hand.size() == 6);

  for_each_choice(hand, 2, [&hand](Hand discard) {
    Hand hold{hand};
    hold.remove(discard);

    Hand deck{all_cards};
    deck.remove(hand);
    assert(deck.size() == 46);

    Tally mine_tally;           // scores when the crib is mine
    Tally theirs_tally;         // scores then the crib is theirs
    int num_hands = 0;
    for_each_choice(deck, 2, [&](Hand chosen) {
      auto remaining_deck{deck};
      remaining_deck.remove(chosen);
      assert(remaining_deck.size() == 44);

      auto card1 = chosen.take();
      auto card2 = chosen.take();

      Hand crib{discard};
      crib.insert(card1);
      crib.insert(card2);
      assert(crib.size() == 4);

      while (auto cut = remaining_deck.take()) {
        auto hold_score = score_hand(hold, cut, false);
        auto crib_score = score_hand(crib, cut, true);

        auto mine_score = hold_score + crib_score;
        auto theirs_score = hold_score - crib_score;

        ++num_hands;

        mine_tally.increment(mine_score);
        theirs_tally.increment(theirs_score);
      }
    });
    // deck size: 46, C(46,2)=1035
    // remaining_deck size: 44
    assert(num_hands == 1035 * 44);

    /* Calculate statistics (mean, standard deviation, min and max)
       for both situations when it's my crib and when it's theirs. */
    Statistics if_mine(mine_tally, num_hands);
    Statistics if_theirs(theirs_tally, num_hands);

    cout << discard << " [" << if_mine << ']' << " [" << if_theirs << "]\n";
  });
  cout << '\n';
}

void analyze_hand(std::string_view str) {
  auto hand = make_hand(str);
  if (hand.size() != 6)
    throw std::runtime_error("Expected six cards '" + std::string(str) + '\'');
  analyze_hand(hand);
}

} // namespace

int main(int, char **argv)
try {

#ifndef NDEBUG
  {
    auto as = make_card('A', 'S');
    assert(as.rank() == 0);
    assert(as.suit() == 0);

    auto kc = make_card('K', 'C');
    assert(kc.rank() == 12);
    assert(kc.suit() == 2);

    auto jh = make_card('J', 'H');
    assert(jh.rank() == 10);
    assert(jh.suit() == 3);
  }

  // unit test for Statistics
  {
    Tally t{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
             0, 0, 0, 0, 0, 907, 411, 1419, 650, 1855, 663, 1908, 931,
             1671, 650, 1699, 530, 607, 137, 291, 160, 228, 111, 66, 106,
             5, 61, 7, 26, 0, 30, 0, 41, 0, 4, 3, 0, 0, 0, 2, 0, 0, 1 };
    Statistics s{t, 15180};
    std::ostringstream ss;
    ss << s;
    if (ss.str() != "22.9 0.8 16..53")
    {
      throw "oops";
    }
  }
#endif

  while (*++argv)
    analyze_hand(*argv);

  // with 29 in your hand, what's the most you could have in the crib?
  if ((false)) {
    const auto hand = make_hand("5H 5C 5S JD"); // 29 hand
    const auto cut = make_card("5D");
    assert(score_hand(hand, cut, false) == 29);
    Hand deck{all_cards};
    deck.remove(hand);
    deck.remove(cut);
    int best = 0;
    for_each_choice(deck, 4, [&](Hand crib) {
      auto score = score_hand(crib, cut, true);
      if (best <= score) {
        best = score;
        cout << crib << " = " << score << '\n';
      }
    });
  }

} catch (std::exception const &exc) {
  std::clog << "Caught exception: " << exc.what() << std::endl;
  return EXIT_FAILURE;
}
