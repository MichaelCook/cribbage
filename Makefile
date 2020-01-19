CXX = clang++

CXXFLAGS = \
  -O3 \
  -Werror \
  -Weverything \
  -Wno-c++98-compat \
  -std=c++11 \
  -fsanitize=address \

.PHONY: all
all:
	$(CXX) $(CXXFLAGS) cribbage.cpp
	./a.out 7c9h5h5c5djs
	./a.out "2H 2C 2D 9S QH QC" "3H 3C 8D 7S 5H 2C"
