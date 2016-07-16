# cribbage
Analyze cribbage hands.

Given a cribbage hand (six cards), which two cards should you discard to the
crib to maximize your chances of getting the best score?

Example:

```
$ g++ -std=c++11 -O3 -DNDEBUG cribbage.cpp
$ ./a.out "2H 2C 2D 9S QH QC"
[ 2H 2C 2D 9S QH QC ]
Discard 2H 2C, average score 9.1 (if your crib), or -2.4 (if theirs).
Discard 2H 2D, average score 9.1 (if your crib), or -2.4 (if theirs).
Discard 2H 9S, average score 9.7 (if your crib), or 1.4 (if theirs).
Discard 2H QH, average score 7.8 (if your crib), or -0.9 (if theirs).
Discard 2H QC, average score 7.2 (if your crib), or -0.4 (if theirs).
Discard 2C 2D, average score 9.1 (if your crib), or -2.4 (if theirs).
Discard 2C 9S, average score 9.7 (if your crib), or 1.4 (if theirs).
Discard 2C QH, average score 7.2 (if your crib), or -0.4 (if theirs).
Discard 2C QC, average score 7.6 (if your crib), or -0.8 (if theirs).
Discard 2D 9S, average score 9.7 (if your crib), or 1.4 (if theirs).
Discard 2D QH, average score 7.2 (if your crib), or -0.4 (if theirs).
Discard 2D QC, average score 7.2 (if your crib), or -0.4 (if theirs).
Discard 9S QH, average score 11.0 (if your crib), or 3.7 (if theirs).
Discard 9S QC, average score 11.0 (if your crib), or 3.7 (if theirs).
Discard QH QC, average score 14.5 (if your crib), or 3.8 (if theirs).

$
```
