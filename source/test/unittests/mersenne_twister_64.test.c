#include "../../kit/mersenne_twister_64.h"

#define KIT_TEST_FILE mersenne_twister_64
#include "../../kit_test/test.h"

enum { SIZE = 1000 };

TEST("mt64 same seeds") {
  uint64_t seed = mt64_seed();

  mt64_state_t foo, bar;
  mt64_init(&foo, seed);
  mt64_init(&bar, seed);

  int ok = 1;
  for (ptrdiff_t i = 0; i < SIZE; i++)
    ok = ok && mt64_generate(&foo) == mt64_generate(&bar);

  REQUIRE(ok);
}

TEST("mt64 different seeds") {
  mt64_state_t foo, bar;
  mt64_init(&foo, 42);
  mt64_init(&bar, 4242424242);

  ptrdiff_t difference_count = 0;
  for (ptrdiff_t i = 0; i < SIZE; i++)
    if (mt64_generate(&foo) != mt64_generate(&bar))
      difference_count++;

  REQUIRE(difference_count > SIZE / 2);
}
