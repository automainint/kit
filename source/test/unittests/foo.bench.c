#define KIT_BENCH_FILE foo
#include "../../kit_test/bench.h"

struct test_foo_ {
  double f;
};

BENCHMARK("foo") {
  BENCHMARK_BEGIN;
  {
    int              x = 0;
    struct test_foo_ f = { 0. };

    for (int i = 0; i < 100000; i++) {
      x += (1 << 1);
      x ^= i;
      f.f += 0.1;
    }
    DO_NOT_OPTIMIZE(x);
    DO_NOT_OPTIMIZE(f);
  }
  BENCHMARK_END;
}

BENCHMARK("bar") { }