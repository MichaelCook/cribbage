CXX = clang++

CXXFLAGS = \
  -O3 \
  -Wall \
  -Werror \
  -std=c++11 \

.PHONY: all
all:
	$(CXX) $(CXXFLAGS) cribbage.cpp
	./a.out 7c9h5h5c5djs
	./a.out "2H 2C 2D 9S QH QC" "3H 3C 8D 7S 5H 2C"
