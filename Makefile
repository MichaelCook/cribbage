CC = clang

CFLAGS = \
  -O3 \
  -Werror \
  -Wpedantic \
  -Weverything \
  -Wno-format-nonliteral \
  -Wno-disabled-macro-expansion \
  -Wno-padded \
  -Wno-reserved-identifier \
  -std=c2x \

CXX = g++-10

CXXFLAGS = \
  -O3 \
  -Werror \
  -Wpedantic \
  -Wall \
  -std=c++20 \

ifdef DEBUG
  CFLAGS += -g -fsanitize=address
  CXXFLAGS += -g -fsanitize=address
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
    5S-4D-JD-4C-5C-5H \
    AS-AD-AC-AH-TH-JH \
    AS-AD-JD-AC-AH-9H \
    AH-3H-7H-9H-TH-JH \
    AH-3H-7H-9H-TH-JH \
    JS-AH-3H-7H-9H-TH \
    7S-AH-3H-9H-TH-JH \
    JS-AH-3H-7H-9H-TH \
    2S-5D-3C-AH-9H-JH \
    7S-8D-7C-7H-8H-9H \

else
  HAND = 5S-4D-JD-4C-5C-5H
endif

.PHONY: default
default:
	echo "Use 'make all' to build everything and run the tests"

.PHONY: all
all: check-py test-c test-rust test-cpp test-go test-typescript test-nim test-python test-julia
ifdef TIMING
	./format-timing $(TIMINGLOG)
endif

cribbage-c: cribbage.c
	$(CC) $(CFLAGS) -o $@ cribbage.c -lm

cribbage-cpp: cribbage.cpp
	$(CXX) $(CXXFLAGS) -o $@ cribbage.cpp

cribbage-nim: cribbage.nim
	nim c --out:$@ $(NIMFLAGS) cribbage.nim

export RUST_BACKTRACE=full

.PHONY: cribbage-rust
cribbage-rust:
	cd cribbage-rust && cargo build $(CARGOFLAGS)

cribbage.js: cribbage.ts node_modules
	tsc --outFile $@ $(TSCFLAGS) cribbage.ts

node_modules:
	npm install @types/node

.PHONY: cribbage-go
cribbage-go: cribbage.go timing
	go build -o $@ cribbage.go

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
test-python: timing check-py
	$(TIMING) ./cribbage.py $(HAND)

# TODO: compile cribbage.jl first, then do timing measurements
.PHONY: test-julia
test-julia: timing
	$(TIMING) ./cribbage.jl $(HAND)

.PHONY: test-go
test-go: cribbage-go timing
	$(TIMING) ./cribbage-go $(HAND)

.PHONY: test-rust
test-rust: cribbage-rust timing
	$(TIMING) cribbage-rust/target/release/cribbage $(HAND)

.PHONY: test-typescript
test-typescript: cribbage.js timing
	$(TIMING) node cribbage.js $(HAND)

.PHONY: clean
clean:
	git clean -fdX -e '!*~' .

.PHONY: timing
timing:
ifdef TIMING
	true > $(TIMINGLOG)
endif

# $1 - name of a Python script
# $2 - any additional modules to include in the analysis
define check-py
$1.check-py~: $1 $2
	mypy --strict --no-error-summary $1 $2
	pylint --enable-all-extensions --rcfile ~/lib/pylint.config $1 $2
	flake8 --color=never --config ~/lib/flake8.config $1 $2
	touch $$@
check-py: $1.check-py~
endef

$(eval $(call check-py, cribbage.py))
$(eval $(call check-py, scores.py))
