CC=gcc

CFLAGS=-g -Wall -std=c99 -O3 -mavx2 -fopenmp

SRC_DIR=../src

# default mode
MODE ?= naive

TEST_BIN=test.out
BENCH_BIN=bench_test.out

.PHONY: all run bench clean clean_all build_test build_bench

all: run

# ---------- BUILD RULES ----------
build_test:
	$(CC) \
	test_driver.c \
	$(SRC_DIR)/compute_$(MODE).c \
	$(SRC_DIR)/io.o \
	-I$(SRC_DIR) \
	$(CFLAGS) \
	-o $(TEST_BIN)

build_bench:
	$(CC) \
	bench_test.c \
	$(SRC_DIR)/compute_$(MODE).c \
	$(SRC_DIR)/io.o \
	-I$(SRC_DIR) \
	$(CFLAGS) \
	-o $(BENCH_BIN)

# ---------- CLEAN ----------
clean:
	rm -f $(TEST_BIN) $(BENCH_BIN)

# ---------- RUN (always fresh) ----------(Clean->Compile->Run)
run: clean build_test
	./$(TEST_BIN)

bench: clean build_bench
	./$(BENCH_BIN)

# ---------- OPTIONAL MODE HELPERS ----------(Easy to run commands)
run_naive:
	$(MAKE) run MODE=naive

run_optimized:
	$(MAKE) run MODE=optimized

bench_naive:
	$(MAKE) bench MODE=naive

bench_optimized:
	$(MAKE) bench MODE=optimized