#include "../../kit/move_back.h"

#define KIT_TEST_FILE move_back
#include "../../kit_test/test.h"

static int is_equal(int const x, int const y) {
  return x == y;
}

static int is_equal_ref(int const *const x, int const y) {
  return *x == y;
}

static int is_even(int const x, int const _) {
  return (x % 2) == 0;
}

static int is_even_ref(int const *const x, int const _) {
  return (*x % 2) == 0;
}

TEST("move back val") {
  int v[] = { 1, 2, 2, 2, 1, 1 };

  struct {
    int  size;
    int *values;
  } ref = { .size = sizeof v / sizeof *v, .values = v };

  MOVE_BACK(ref.size, ref, 2, is_equal);

  REQUIRE_EQ(ref.size, 3);
  REQUIRE_EQ(v[0], 1);
  REQUIRE_EQ(v[1], 1);
  REQUIRE_EQ(v[2], 1);
}

TEST("move back ref val") {
  int v[] = { 1, 2, 2, 2, 1, 1 };

  struct {
    int  size;
    int *values;
  } ref = { .size = sizeof v / sizeof *v, .values = v };

  MOVE_BACK_REF(ref.size, ref, 2, is_equal_ref);

  REQUIRE(ref.size == 3);
  REQUIRE(v[0] == 1);
  REQUIRE(v[1] == 1);
  REQUIRE(v[2] == 1);
}

TEST("move back 1") {
  int v[] = { 1, 2, 3, 4, 5, 6 };

  struct {
    int  size;
    int *values;
  } ref = { .size = sizeof v / sizeof *v, .values = v };

  MOVE_BACK(ref.size, ref, 0, is_even);

  REQUIRE(ref.size == 3);
  REQUIRE(v[0] == 1);
  REQUIRE(v[1] == 5);
  REQUIRE(v[2] == 3);
}

TEST("move back 2") {
  int v[] = { 2, 4, 6, 1, 3, 5 };

  struct {
    int  size;
    int *values;
  } ref = { .size = sizeof v / sizeof *v, .values = v };

  MOVE_BACK(ref.size, ref, 0, is_even);

  REQUIRE(ref.size == 3);
  REQUIRE(v[0] == 5);
  REQUIRE(v[1] == 3);
  REQUIRE(v[2] == 1);
}

TEST("move back 3") {
  int v[] = { 1, 3, 5, 2, 4, 6 };

  struct {
    int  size;
    int *values;
  } ref = { .size = sizeof v / sizeof *v, .values = v };

  MOVE_BACK(ref.size, ref, 0, is_even);

  REQUIRE(ref.size == 3);
  REQUIRE(v[0] == 1);
  REQUIRE(v[1] == 3);
  REQUIRE(v[2] == 5);
}

TEST("move back ref 1") {
  int v[] = { 1, 2, 3, 4, 5, 6 };

  struct {
    int  size;
    int *values;
  } ref = { .size = sizeof v / sizeof *v, .values = v };

  MOVE_BACK_REF(ref.size, ref, 0, is_even_ref);

  REQUIRE(ref.size == 3);
  REQUIRE(v[0] == 1);
  REQUIRE(v[1] == 5);
  REQUIRE(v[2] == 3);
}

TEST("move back ref 2") {
  int v[] = { 2, 4, 6, 1, 3, 5 };

  struct {
    int  size;
    int *values;
  } ref = { .size = sizeof v / sizeof *v, .values = v };

  MOVE_BACK_REF(ref.size, ref, 0, is_even_ref);

  REQUIRE(ref.size == 3);
  REQUIRE(v[0] == 5);
  REQUIRE(v[1] == 3);
  REQUIRE(v[2] == 1);
}

TEST("move back ref 3") {
  int v[] = { 1, 3, 5, 2, 4, 6 };

  struct {
    int  size;
    int *values;
  } ref = { .size = sizeof v / sizeof *v, .values = v };

  MOVE_BACK_REF(ref.size, ref, 0, is_even_ref);

  REQUIRE(ref.size == 3);
  REQUIRE(v[0] == 1);
  REQUIRE(v[1] == 3);
  REQUIRE(v[2] == 5);
}
