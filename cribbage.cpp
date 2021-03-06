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
#include <compare>
#include <string_view>
#include <type_traits>

#if 0 // 1 to facilitate constexpr/static_assert debugging
#  define constexpr
#  define token_paste_(A, B) A ## B
#  define token_paste(A, B) token_paste_(A, B)
#  define static_assert(EXPR) [[maybe_unused]] int token_paste(once, __LINE__) = (assert(EXPR), 0)
#endif

namespace {

using std::cout;
using std::endl;

using Rank = int; // 'A', '2', '3', ..., 'J', 'Q', 'K'
using Suit = int; // 'S', 'D', 'C', 'H'

struct [[nodiscard]] Card {
  Rank rank = 0;
  Suit suit = 0;

  constexpr Card() noexcept = default;

  constexpr Card(Rank r, Suit s) noexcept : rank{r}, suit{s} {}

  constexpr auto operator<=>(Card const&) const noexcept = default;
};

struct [[nodiscard]] Hand {
  size_t num_cards = 0;
  static constexpr size_t max_cards = 52;
  Card cards[max_cards];

  constexpr size_t size() const noexcept { return num_cards; }

  constexpr Card const &card(size_t i) const {
    if (i >= num_cards)
      throw std::runtime_error("No card " + std::to_string(i));
    return cards[i];
  }

  constexpr int value(size_t i) const {
    auto r = card(i).rank;
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
      throw std::runtime_error("Invalid rank");
    }
  }

  constexpr int order(size_t i) const {
    auto r = card(i).rank;
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
      throw std::runtime_error("Invalid rank");
    }
  }

  constexpr void push(Card const &card) {
    if (num_cards == max_cards)
      throw std::runtime_error("Too many cards in hand");
    cards[num_cards] = card;
    ++num_cards;
  }

  constexpr void pop() {
    if (num_cards == 0)
      throw std::runtime_error("Empty hand");
    --num_cards;
  }

  constexpr bool has(Card const &card) const noexcept {
    for (size_t i = 0; i != num_cards; ++i)
      if (card == cards[i])
        return true;
    return false;
  }
};

void sort(Hand& hand)
{
    std::sort(hand.cards, hand.cards + hand.num_cards);
}

std::ostream &operator<<(std::ostream &os, Card const &card) {
  if (card.rank == 0)
    os << '-';
  else
    os << char(card.rank);
  if (card.suit == 0)
    os << '-';
  else
    os << char(card.suit);
  return os;
}

std::ostream &operator<<(std::ostream &os, Hand const &hand) {
  if (hand.num_cards != 0) {
    os << hand.cards[0];
    for (size_t i = 1; i != hand.num_cards; ++i)
      os << ' ' << hand.cards[i];
  }
  return os;
}

constexpr char to_upper(char c) // std::toupper is not constexpr
{
    if (c >= 'a' && c <= 'z')
        return c - 'a' + 'A';
    return c;
}

constexpr bool is_space(char c) // std::isspace is not constexpr
{
    return c == ' ';
}

constexpr Hand make_hand(std::string_view hand) {
  Hand h;
  Rank rank = 0;
  for (auto c : hand) {
    c = to_upper(c);
    switch (c) {
    case 'H':
    case 'C':
    case 'S':
    case 'D':
      if (rank == 0)
        throw std::runtime_error("Malformed hand '" + std::string(hand) + '\'');
      h.push(Card(rank, c));
      rank = 0;
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
      if (rank != 0)
        throw std::runtime_error("Malformed hand '" + std::string(hand) + '\'');
      rank = c;
      break;
    case '-':
      break;
    default:
      if (is_space(c))
        break;
      throw std::runtime_error("Malformed hand '" + std::string(hand) + '\'');
    }
  }
  if (rank != 0)
      throw std::runtime_error("Malformed hand '" + std::string(hand));
  return h;
}

constexpr int score_15s(Hand const &hand) {
  auto a = hand.value(0);
  auto b = hand.value(1);
  auto c = hand.value(2);
  auto d = hand.value(3);
  auto e = hand.value(4);
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

constexpr int score_pairs(Hand const &hand) {
  size_t n = hand.size();
  int num_pairs = 0;
  for (size_t i = 0; i < n - 1; ++i) {
    auto &card = hand.card(i);
    for (size_t j = i; ++j < n;) {
      if (card.rank == hand.card(j).rank)
        ++num_pairs;
    }
  }

  return 2 * num_pairs;
}

constexpr int score_runs(Hand const &hand) {
  if (hand.size() != 5)
    throw std::runtime_error("Wrong number of cards in hand");

  // Make a sorted copy of the hand, but use only the order
  // of each rank, ignore the suit.
#if 1 // this approach is just little faster than std::sort (0.93s versus 0.98s)
  int orders[5];
  for (size_t i = 0; i < 5; ++i) {
    auto order = hand.order(i);
    for (size_t j = 0;; ++j) {
      if (j == i) {
        // insert at end
        orders[i] = order;
        break;
      }
      if (order < orders[j]) {
        // insert before [j]
        do
          std::swap(order, orders[j]);
        while (++j < i);
        orders[j] = order;
        break;
      }
    }
  }
#else
  int orders[5] = {
    hand.order(0),
    hand.order(1),
    hand.order(2),
    hand.order(3),
    hand.order(4),
  };
  std::sort(std::begin(orders), std::end(orders));
#endif

  constexpr int X = -1; // match any rank
  struct {
    int score;
    int delta[4];
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
  constexpr auto num_patterns = sizeof(patterns) / sizeof(patterns[0]);
  for (size_t i = 0; i < num_patterns; ++i) {
    auto &pattern = patterns[i];
    auto previous = orders[0];
    for (size_t j = 0;; ++j) {
      if (j == 4)
        return pattern.score;
      auto delta = pattern.delta[j];
      auto order = orders[j + 1];
      if (delta != X && delta != order - previous)
        break;
      previous = order;
    }
  }

  return 0;
}

constexpr int score_flush(Hand const &hand, bool is_crib) {
  size_t n = hand.size();
  if (n != 5)
    throw std::runtime_error("Wrong number of cards in hand");
  auto suit = hand.card(0).suit;
  for (size_t i = 1; i < 4; ++i)
    if (suit != hand.card(i).suit)
      return 0;
  // first 4 are same suit
  if (suit == hand.card(4).suit)
    return 5;
  // In the crib, a flush counts only if all five cards are the same suit.
  if (is_crib)
    return 0;
  return 4;
}

constexpr int score_right_jack(Hand const &hand) {
  size_t n = hand.size();
  if (n == 0)
    throw std::runtime_error("Empty hand");
  --n;
  auto cut_suit = hand.card(n).suit;
  for (size_t i = 0; i < n; ++i) {
    auto &card = hand.card(i);
    if (card.rank == 'J' && card.suit == cut_suit)
      return 1;
  }
  return 0;
}

constexpr int score_hand(Hand const &hand, bool is_crib) {
  return score_15s(hand) +
         score_pairs(hand) +
         score_runs(hand) +
         score_flush(hand, is_crib) +
         score_right_jack(hand);
}

constexpr int score_hand(std::string_view hand, bool is_crib) {
  return score_hand(make_hand(hand), is_crib);
}

static_assert(score_hand("AH AS JH AC AD", false) == 12); // 4oak
static_assert(score_hand("AH AS JH AC AH", false) == 13); // ...plus right jack
static_assert(score_hand("AH 3H 7H TH JH", false) == 5);  // 5 hearts
static_assert(score_hand("AH 3H 7H TH JH", true) == 5);   // 5 hearts but crib
static_assert(score_hand("AH 3H 7H TH JS", false) == 4);  // 4 hearts
static_assert(score_hand("AH 3H 7S TH JH", false) == 0);  // 4 hearts but with cut
static_assert(score_hand("AH 3H 7H TH JS", true) == 0);   // 4 hearts but crib
static_assert(score_hand("AH 2S 3C 5D JH", false) == 4 + 3); // 15/4 + run/3
static_assert(score_hand("7H 7S 7C 8D 8H", false) == 12 + 6 + 2); // 15/12 + 3oak + 2oak
static_assert(score_hand("AH 2H 3H 3S 3D", false) == 15); // triple run/3
static_assert(score_hand("3H AH 3S 2H 3D", false) == 15); // triple run/3
static_assert(score_hand("5H 5C 5S JD 5D", false) == 29);
static_assert(score_hand("5H 5C 5S 5D JD", false) == 28);
static_assert(score_hand("6C 4D 6D 4S 5D", false) == 24);

// ---------------------------------------------------------------------------

/* A ChoiceHandler is a type like `void f(Hand const& choice)`.  That is, a
   function (or function-like object) that takes one argument, a `Hand const&`
   object, and the returned value (if any) is ignored */
template<typename T>
concept ChoiceHandler = std::invocable<T, Hand const&>;

template <ChoiceHandler T>
constexpr
void for_each_choice(Hand const &hand, size_t offset, size_t num_choose,
                     Hand &chosen, T const &func) {
  if (chosen.size() == num_choose) {
    func(chosen);
    return;
  }
  while (offset != hand.size()) {
    chosen.push(hand.cards[offset]);
    ++offset;
    for_each_choice(hand, offset, num_choose, chosen, func);
    chosen.pop();
  }
}

template <ChoiceHandler T>
constexpr
void for_each_choice(Hand const &hand, size_t num_choose, T const &func) {
  Hand discard;
  for_each_choice(hand, 0u, num_choose, discard, func);
}

constexpr Hand make_deck(Hand const &exclude) {
  Hand deck;
  for (auto suit : { 'H', 'C', 'D', 'S' }) {
    for (auto rank : { 'A', '2', '3', '4', '5', '6', '7',
                       '8', '9', 'T', 'J', 'Q', 'K' }) {
      Card card(rank, suit);
      if (!exclude.has(card))
        deck.push(card);
    }
  }
  return deck;
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

void analyze_hand(Hand const &hand) {
  /*
    Find all possible pairs of cards to discard to the crib.
    There are C(6,2)=15 possible discards in a cribbage hand.
   */
  cout << "[ " << hand << " ]\n";
  assert(hand.size() == 6);

  for_each_choice(hand, 2, [&hand](Hand const &discard) {
    Hand hold;
    for (size_t i = 0; i != hand.size(); ++i) {
      auto &card = hand.card(i);
      if (!discard.has(card))
        hold.push(card);
    }

    Hand deck{ make_deck(hand) };
    assert(deck.size() == 46);

    Tally mine_tally;           // scores when the crib is mine
    Tally theirs_tally;         // scores then the crib is theirs
    int num_hands = 0;
    for_each_choice(deck, 2, [&](Hand const& chosen)
    {
      auto card1 = chosen.card(0);
      auto card2 = chosen.card(1);

      Hand crib(discard);
      crib.push(card1);
      crib.push(card2);
      assert(crib.size() == 4);

      for (size_t i = 0; i != deck.size(); ++i) {
        auto &cut = deck.card(i);
        if (cut == card1 || cut == card2)
          continue;

        hold.push(cut);
        auto hold_score = score_hand(hold, false);
        hold.pop();

        crib.push(cut);
        auto crib_score = score_hand(crib, true);
        crib.pop();

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
    const auto hand{ make_hand("5H 5C 5S JD 5D") }; // 29 hand
    const auto cut{ hand.cards[4] };
    const auto deck{ make_deck(hand) };
    int best = 0;
    for_each_choice(deck, 4, [&](Hand crib) {
      crib.push(cut);
      auto score = score_hand(crib, true);
      if (best <= score) {
        sort(crib);
        best = score;
        cout << score << ' ' << crib << '\n';
    }
    });
  }

} catch (std::exception const &exc) {
  std::clog << "Caught exception: " << exc.what() << std::endl;
  return EXIT_FAILURE;
}
