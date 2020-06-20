# Analyze Cribbage Hands

Given a cribbage hand (six cards), which two cards should you discard to the
crib to maximize your chances of getting the best score?

This repo contains implementations in multiple computer languages:

* C
* C++
* Go (golang)
* Nim (nimlang)
* Python
* Rust
* TypeScript

### Example Output

Here's a sample run:

```
$ ./cribbage-cpp "7C 9H 5H 5C 5D JS"
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

### Performance

| Elapsed (s) | Normalized | Language |
| ----- | ----- | --- |
|  0.25 |   1.0 | c |
|  0.41 |   1.5 | cpp |
|  0.53 |   2.1 | rust |
|  2.47 |   9.9 | go |
|  2.55 |  10.2 | typescript |
| 13.97 |  55.9 | nim |
| 41.79 | 167.2 | python |

### Run All

```
$ make TIMING=
./cribbage-c 5H-5C-5S-JD-4C-4D
[ 5H 5C 5S JD 4C 4D ]
5H 5C [15.7 0.5 6..38] [-1.3 0.4 -16..13]
5H 5S [15.7 0.5 6..38] [-1.3 0.4 -16..13]
5H JD [14.9 0.5 6..35] [1.2 0.4 -12..16]
5H 4C [16.3 0.6 8..41] [3.1 0.4 -16..13]
5H 4D [16.3 0.6 8..41] [3.1 0.4 -16..13]
5C 5S [15.7 0.5 6..38] [-1.3 0.4 -16..13]
5C JD [14.9 0.5 6..35] [1.2 0.4 -12..16]
5C 4C [16.3 0.6 8..41] [3.1 0.4 -16..13]
5C 4D [16.3 0.6 8..41] [3.1 0.4 -16..13]
5S JD [14.9 0.5 6..35] [1.2 0.4 -12..16]
5S 4C [16.3 0.6 8..41] [3.1 0.4 -16..13]
5S 4D [16.3 0.6 8..41] [3.1 0.4 -16..13]
JD 4C [16.7 0.5 8..38] [9.1 0.4 -5..23]
JD 4D [16.7 0.5 8..38] [9.0 0.4 -7..23]
4C 4D [22.9 0.8 16..53] [11.4 0.6 -10..27]

cd cribbage-rust && cargo build --release --quiet
cribbage-rust/target/release/cribbage 5H-5C-5S-JD-4C-4D
(same output)

./cribbage-cpp 5H-5C-5S-JD-4C-4D
(same output)

node cribbage.js 5H-5C-5S-JD-4C-4D
(same output)

./cribbage-nim 5H-5C-5S-JD-4C-4D
(same output)

./cribbage.py 5H-5C-5S-JD-4C-4D
(same output)

go build -o cribbage-go cribbage.go
./cribbage-go 5H-5C-5S-JD-4C-4D
(same output)

$
```
