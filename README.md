# Analyze cribbage hands

Given a cribbage hand (six cards), which two cards should you discard to the
crib to maximize your chances of getting the best score?

Example:

```
$ g++ -std=c++11 -O3 -DNDEBUG cribbage.cpp
$ ./a.out "7C 9H 5H 5C 5D JS"
[ 7C 9H 5H 5C 5D JS ]
7C 9H [21.8 1.1 0..29] [13.4 1.0 0..29]
7C 5H [15.6 0.5 2..16] [3.2 0.8 2..16]
7C 5C [16.0 0.6 2..20] [2.8 1.2 2..20]
7C 5D [15.6 0.5 2..16] [3.2 0.8 2..16]
7C JS [15.3 0.6 0..20] [7.6 0.6 0..20]
9H 5H [15.7 0.6 2..20] [3.5 1.2 2..20]
9H 5C [15.1 0.5 2..16] [4.1 0.7 2..16]
9H 5D [15.1 0.5 2..16] [4.1 0.7 2..16]
9H JS [16.7 0.7 0..20] [7.7 0.7 0..20]
5H 5C [14.0 0.7 2..23] [-3.4 2.4 2..23]
5H 5D [14.0 0.7 2..23] [-3.4 2.4 2..23]
5H JS [12.9 0.6 2..21] [-1.6 1.9 2..21]
5C 5D [14.0 0.7 2..23] [-3.4 2.4 2..23]
5C JS [12.9 0.6 2..21] [-1.6 1.9 2..21]
5D JS [12.9 0.6 2..21] [-1.6 1.9 2..21]

```

For example:

```
7C 9H [21.8 1.1 0..29] [13.4 1.0 0..29]
```

This line says if you discard the seven of clubs (7C) and the nine of hearts
(9H) you will get an average score of 21.8 if the crib is yours, or 13.4 if
the crib is not yours.

The numbers in brackets (e.g., [12.9 0.6 2..21]) show the mean, standard
deviation, minimum and maximum scores.  The first set of numbers is for when
the crib is yours; the second set, the crib is your opponent's.  (A smaller
standard deviation means you're more likely to get the average score.)

There's also a Nim language implementation.

```
nim --opt:speed c cribbage.nim
./cribbage 7c9h5h5c5djs
```
