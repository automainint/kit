#define KIT_BENCH_FILE foo
#include "../../kit_test/bench.h"

BENCHMARK("foo") {
  BENCHMARK_BEGIN;
  {
    volatile int x = 0;
    for (int i = 0; i < 200000; i++) x++;
  }
  BENCHMARK_END;
}

BENCHMARK("bar") { }
