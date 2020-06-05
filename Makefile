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

TIMINGLOG = timing.log~
TIME = time --output=$(TIMINGLOG) --append \
--format='| %e | %U | %S | $(patsubst test-%,%,$@) |'

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
test-cpp: cribbage-cpp timing
	$(TIME) ./cribbage-cpp $(HAND)

.PHONY: test-nim
test-nim: cribbage-nim timing
	$(TIME) ./cribbage-nim $(HAND)

.PHONY: test-python timing
test-python:
	$(TIME) ./cribbage.py $(HAND)

.PHONY: test-rust timing
test-rust: cribbage-rust
	$(TIME) cribbage-rust/target/release/cribbage $(HAND)

.PHONY: test-typescript timing
test-typescript: cribbage.js
	$(TIME) node cribbage.js $(HAND)

.PHONY: clean
clean:
	rm -rf cribbage-nim cribbage-cpp cribbage.js node_modules cribbage-rust/Cargo.lock $(TIMINGLOG)
	cd cribbage-rust && cargo clean

.PHONY: timing
timing:
	rm -f $(TIMINGLOG)
	echo '| Real | Sys | User | Language |' >$(TIMINGLOG)
	echo '| --- | --- | --- | --- |' >>$(TIMINGLOG)
