CXX      = g++
CXXFLAGS = -std=c++17 -O0 -g --coverage

LIB_SRC  = $(wildcard str/*.cpp)

GTEST_SRC  = $(wildcard tests/gtest/*.cpp)
BOOST_SRC  = $(wildcard tests/boost/*.cpp)
CATCH_SRC  = $(wildcard tests/catch2/*.cpp)

GTEST_BIN  = build/gtest/run
BOOST_BIN  = build/boost/run
CATCH_BIN  = build/catch2/run

GTEST_LIBS = -lgtest -lgtest_main -pthread
BOOST_LIBS = -lboost_unit_test_framework
CATCH_LIBS = -lCatch2Main -lCatch2

GCOVR = gcovr -r . --filter 'str/' --exclude-throw-branches --exclude-unreachable-branches

BENCH_BIN  = build/bench/run
BENCH_LIBS = -lbenchmark -pthread

all: test test_boost test_catch2

$(GTEST_BIN): $(LIB_SRC) $(GTEST_SRC)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $^ $(GTEST_LIBS) -o $@

$(BOOST_BIN): $(LIB_SRC) $(BOOST_SRC)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $^ $(BOOST_LIBS) -o $@

$(CATCH_BIN): $(LIB_SRC) $(CATCH_SRC)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $^ $(CATCH_LIBS) -o $@

test: $(GTEST_BIN)
	rm -f $(dir $<)*.gcda
	./$<

test_boost: $(BOOST_BIN)
	rm -f $(dir $<)*.gcda
	./$<

test_catch2: $(CATCH_BIN)
	rm -f $(dir $<)*.gcda
	./$<

coverage: test
	mkdir -p coverage/gtest
	$(GCOVR) --html-details coverage/gtest/index.html --print-summary build/gtest

coverage_boost: test_boost
	mkdir -p coverage/boost
	$(GCOVR) --html-details coverage/boost/index.html --print-summary build/boost

coverage_catch2: test_catch2
	mkdir -p coverage/catch2
	$(GCOVR) --html-details coverage/catch2/index.html --print-summary build/catch2

coverage_all: coverage coverage_boost coverage_catch2

bench:
	mkdir -p build/bench
	$(CXX) -std=c++17 -O2 $(LIB_SRC) benchmarks/bench.cpp $(BENCH_LIBS) -o $(BENCH_BIN)
	./$(BENCH_BIN)

lint:
	clang-tidy $(LIB_SRC) -- -std=c++17 -I.
	cppcheck --enable=style --inconclusive --std=c++17 -q str/
	cd golang && golangci-lint run ./...

clean:
	rm -rf build coverage

.PHONY: bench lint all test test_boost test_catch2 coverage coverage_boost coverage_catch2 coverage_all clean
