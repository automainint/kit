#include "../../kit/array_ref.h"

#define KIT_TEST_FILE array_ref
#include "../../kit_test/test.h"

TEST("array ref const wrap") {
  int foo[] = { 1, 2, 3 };
  AR_WRAP(ref, int, foo);

  REQUIRE(ref.size == 3);
  REQUIRE(ref.values[0] == 1);
  REQUIRE(ref.values[1] == 2);
  REQUIRE(ref.values[2] == 3);
}

TEST("array ref wrap") {
  int foo[] = { 1, 2, 3 };
  AR_MUT_WRAP(ref, int, foo);

  REQUIRE(ref.size == 3);
  REQUIRE(ref.values[0] == 1);
  REQUIRE(ref.values[1] == 2);
  REQUIRE(ref.values[2] == 3);

  ref.values[1] = 42;
  REQUIRE(ref.values[1] == 42);
}

TEST("array ref equal") {
  int foo[] = { 1, 2, 3, 4, 5, 6, 7 };
  int bar[] = { 3, 4, 5 };

  AR(foo_ref, int) = { .size = 3, .values = foo + 2 };
  AR(bar_ref, int) = { .size = 3, .values = bar };

  REQUIRE(AR_EQUAL(foo_ref, bar_ref));
}

static int compare(int const *left, int const *right) {
  return *left - *right;
}

TEST("array ref compare") {
  int foo[] = { 1, 2, 3, 5 };
  int bar[] = { 1, 2, 4, 5 };

  AR(foo_ref, int) = { .size = 3, .values = foo };
  AR(bar_ref, int) = { .size = 3, .values = bar };

  REQUIRE(AR_COMPARE(foo_ref, bar_ref, compare) < 0);
  REQUIRE(AR_COMPARE(bar_ref, foo_ref, compare) > 0);
  REQUIRE(AR_COMPARE(foo_ref, foo_ref, compare) == 0);
}

TEST("array ref different element sizes") {
  int  foo[] = { 1, 2, 3 };
  char bar[] = { 1, 2, 3 };

  AR(foo_ref, int)  = { .size = 3, .values = foo };
  AR(bar_ref, char) = { .size = 3, .values = bar };

  REQUIRE(!AR_EQUAL(foo_ref, bar_ref));
  REQUIRE(AR_COMPARE(foo_ref, bar_ref, compare) > 0);
  REQUIRE(AR_COMPARE(bar_ref, foo_ref, compare) < 0);
}
