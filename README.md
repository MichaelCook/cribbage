# cribbage
Analyze cribbage hands.

Given a cribbage hand (six cards), which two cards should you discard to the
crib to maximize your chances of getting the best score?

Example:

```
$ g++ -std=c++11 -O3 -DNDEBUG cribbage.cpp
$ ./a.out 7c9h5h5c5djs
[ 7C 9H 5H 5C 5D JS ]
Discard 7C 9H, average score 21.8 (if your crib), or 13.4 (if theirs). [21.8 1.1 0..29] [13.4 1.0 0..29]
Discard 7C 5H, average score 15.6 (if your crib), or 3.2 (if theirs). [15.6 0.5 2..16] [3.2 0.8 2..16]
Discard 7C 5C, average score 16.0 (if your crib), or 2.8 (if theirs). [16.0 0.6 2..20] [2.8 1.2 2..20]
Discard 7C 5D, average score 15.6 (if your crib), or 3.2 (if theirs). [15.6 0.5 2..16] [3.2 0.8 2..16]
Discard 7C JS, average score 15.3 (if your crib), or 7.6 (if theirs). [15.3 0.6 0..20] [7.6 0.6 0..20]
Discard 9H 5H, average score 15.7 (if your crib), or 3.5 (if theirs). [15.7 0.6 2..20] [3.5 1.2 2..20]
Discard 9H 5C, average score 15.1 (if your crib), or 4.1 (if theirs). [15.1 0.5 2..16] [4.1 0.7 2..16]
Discard 9H 5D, average score 15.1 (if your crib), or 4.1 (if theirs). [15.1 0.5 2..16] [4.1 0.7 2..16]
Discard 9H JS, average score 16.7 (if your crib), or 7.7 (if theirs). [16.7 0.7 0..20] [7.7 0.7 0..20]
Discard 5H 5C, average score 14.0 (if your crib), or -3.4 (if theirs). [14.0 0.7 2..23] [-3.4 2.4 2..23]
Discard 5H 5D, average score 14.0 (if your crib), or -3.4 (if theirs). [14.0 0.7 2..23] [-3.4 2.4 2..23]
Discard 5H JS, average score 12.9 (if your crib), or -1.6 (if theirs). [12.9 0.6 2..21] [-1.6 1.9 2..21]
Discard 5C 5D, average score 14.0 (if your crib), or -3.4 (if theirs). [14.0 0.7 2..23] [-3.4 2.4 2..23]
Discard 5C JS, average score 12.9 (if your crib), or -1.6 (if theirs). [12.9 0.6 2..21] [-1.6 1.9 2..21]
Discard 5D JS, average score 12.9 (if your crib), or -1.6 (if theirs). [12.9 0.6 2..21] [-1.6 1.9 2..21]

$
```

7C means the seven of clubs, for example.

The numbers in brackets (e.g., [12.9 0.6 2..21]) show the mean, standard
deviation, minimum and maximum scores.  The first set of numbers is for when
the crib is yours; the second set, the crib is your opponent's.  (A smaller
standard deviation means you're more likely to get the average score.)

There's also a Nim language implementation.

```
$ nim --opt:speed c cribbage.nim
$ ./cribbage 7c9h5h5c5djs
```
