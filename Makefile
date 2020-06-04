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

CARGOFLAGS = \
  --release \

TSCFLAGS = \
  --target ESNEXT \

.PHONY: all
all: test-cpp test-nim test-python test-rust test-typescript

cribbage-cpp: cribbage.cpp
	$(CXX) $(CXXFLAGS) -o $@ cribbage.cpp

cribbage-nim: cribbage.nim
	nim c --out:$@ $(NIMFLAGS) cribbage.nim

.PHONY: cribbage-rust
cribbage-rust:
	cd cribbage-rust && cargo build $(CARGOFLAGS)

cribbage.js: cribbage.ts node_modules
	tsc --outFile $@ $(TSCFLAGS) cribbage.ts

node_modules:
	npm i @types/node

.PHONY: test-cpp
test-cpp: cribbage-cpp
	time ./cribbage-cpp $(HAND)

.PHONY: test-nim
test-nim: cribbage-nim
	time ./cribbage-nim $(HAND)

.PHONY: test-python
test-python:
	time ./cribbage.py $(HAND)

.PHONY: test-rust
test-rust: cribbage-rust
	time cribbage-rust/target/release/cribbage $(HAND)

.PHONY: test-typescript
test-typescript: cribbage.js
	time node cribbage.js $(HAND)

.PHONY: clean
clean:
	rm -rf cribbage-nim cribbage-cpp cribbage.js node_modules cribbage-rust/Cargo.lock
	cd cribbage-rust && cargo clean
