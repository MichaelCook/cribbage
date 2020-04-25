HAND = 5H-5C-5S-JD-4C-4D

CXX = clang++

CXXFLAGS = \
  -O3 \
  -DNDEBUG \
  -Werror \
  -Wpedantic \
  -Weverything \
  -Wno-c++98-compat \
  -std=c++2a \
  -fsanitize=address \

NIMFLAGS = \
  --verbosity:0 \
  --hints:off \
  --opt:speed \

.PHONY: all
all: test-cpp test-nim test-python

cribbage-cpp: cribbage.cpp
	$(CXX) $(CXXFLAGS) -o $@ cribbage.cpp

cribbage-nim: cribbage.nim
	nim c --out:$@ $(NIMFLAGS) cribbage.nim

.PHONY: test-cpp
test-cpp: cribbage-cpp
	time ./cribbage-cpp $(HAND)

.PHONY: test-nim
test-nim: cribbage-nim
	time ./cribbage-nim $(HAND)

.PHONY: test-python
test-python:
	time ./cribbage.py $(HAND)

.PHONY: clean
clean:
	rm -fv cribbage-nim cribbage-cpp
