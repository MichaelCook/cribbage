CXX = clang++

CXXFLAGS = \
  -O3 \
  -Werror \
  -Wpedantic \
  -Weverything \
  -Wno-c++98-compat \
  -std=c++2a \
  -fsanitize=address \

.PHONY: all
all: build
	./a.out 7c9h5h5c5djs
	./a.out "2H 2C 2D 9S QH QC" "3H 3C 8D 7S 5H 2C"

.PHONY: build
build:
	$(CXX) $(CXXFLAGS) cribbage.cpp
