CC = clang

CFLAGS = \
  -O3 \
  -Werror \
  -Wpedantic \
  -Weverything \
  -Wno-format-nonliteral \
  -Wno-disabled-macro-expansion \
  -Wno-padded \
  -std=c2x \

CXX = clang++

CXXFLAGS = \
  -O3 \
  -Werror \
  -Wpedantic \
  -Weverything \
  -Wno-c++98-compat \
  -std=c++2a \

ifdef DEBUG
  CFLAGS += -fsanitize=address
  CXXFLAGS += -fsanitize=address
else
  CFLAGS += -DNDEBUG
  CXXFLAGS += -DNDEBUG
endif

NIMFLAGS = \
  --verbosity:0 \
  --hints:off \
  --opt:speed \

CARGOFLAGS = \
  --release \
  --quiet \

TSCFLAGS = \
  --target ESNEXT \

TIMINGLOG = timing.log~
TIMING = time --output=$(TIMINGLOG) --append \
--format='%e $(patsubst test-%,%,$@)'

ifdef TIMING
  HAND = \
    5H-5C-5S-JD-4C-4D \
    AH-AS-JH-AC-AD-TH \
    AH-AS-JD-AC-AD-9H \
    AH-3H-7H-TH-JH-9H \
    AH-3H-7H-TH-JH-9H \
    AH-3H-7H-TH-JS-9H \
    AH-3H-7S-TH-JH-9H \
    AH-3H-7H-TH-JS-9H \
    AH-2S-3C-5D-JH-9H \
    7H-7S-7C-8D-8H-9H \

else
  HAND = 5H-5C-5S-JD-4C-4D
endif

.PHONY: all
all: test-c test-rust test-cpp test-typescript test-nim test-python
ifdef TIMING
	./format-timing $(TIMINGLOG)
endif

cribbage-c: cribbage.c
	$(CC) $(CFLAGS) -o $@ cribbage.c -lm

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
	npm install @types/node

.PHONY: test-c
test-c: cribbage-c timing
	$(TIMING) ./cribbage-c $(HAND)

.PHONY: test-cpp
test-cpp: cribbage-cpp timing
	$(TIMING) ./cribbage-cpp $(HAND)

.PHONY: test-nim
test-nim: cribbage-nim timing
	$(TIMING) ./cribbage-nim $(HAND)

.PHONY: test-python
test-python: timing
	$(TIMING) ./cribbage.py $(HAND)

.PHONY: test-rust
test-rust: cribbage-rust timing
	$(TIMING) cribbage-rust/target/release/cribbage $(HAND)

.PHONY: test-typescript
test-typescript: cribbage.js timing
	$(TIMING) node cribbage.js $(HAND)

.PHONY: clean
clean:
	rm -rf cribbage-nim cribbage-c cribbage-cpp cribbage.js node_modules cribbage-rust/Cargo.lock $(TIMINGLOG)
	cd cribbage-rust && cargo clean

.PHONY: timing
timing:
ifdef TIMING
	true > $(TIMINGLOG)
endif
