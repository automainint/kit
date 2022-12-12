#include "../../kit/lower_bound.h"
#include "../../kit/array_ref.h"

#define KIT_TEST_FILE lower_bound
#include "../../kit_test/test.h"

static int kit_less_int(int left, int right) {
  return left < right;
}

static int kit_less_int_ref(int const *left, int const *right) {
  return *left < *right;
}

TEST("lower bound empty") {
  AR(int) ref = { .size = 0, .values = NULL };

  ptrdiff_t index;
  LOWER_BOUND(index, ref, 42, kit_less_int);
  REQUIRE(index == 0);
}

TEST("lower bound single left") {
  int const v[1] = { 42 };
  AR(int) ref    = { .size = 1, .values = v };

  ptrdiff_t index;
  LOWER_BOUND(index, ref, 42, kit_less_int);
  REQUIRE(index == 0);
}

TEST("lower bound single right") {
  int const v[1] = { 42 };
  AR(int) ref    = { .size = 1, .values = v };

  ptrdiff_t index;
  LOWER_BOUND(index, ref, 43, kit_less_int);
  REQUIRE(index == 1);
}

TEST("lower bound first of four") {
  int const v[4] = { 1, 2, 3, 4 };
  AR(int) ref    = { .size = 4, .values = v };

  ptrdiff_t index;
  LOWER_BOUND(index, ref, 1, kit_less_int);
  REQUIRE(index == 0);
}

TEST("lower bound second of four") {
  int const v[4] = { 1, 2, 3, 4 };
  AR(int) ref    = { .size = 4, .values = v };

  ptrdiff_t index;
  LOWER_BOUND(index, ref, 2, kit_less_int);
  REQUIRE(index == 1);
}

TEST("lower bound third of four") {
  int const v[4] = { 1, 2, 3, 4 };
  AR(int) ref    = { .size = 4, .values = v };

  ptrdiff_t index;
  LOWER_BOUND(index, ref, 3, kit_less_int);
  REQUIRE(index == 2);
}

TEST("lower bound forth of four") {
  int const v[4] = { 1, 2, 3, 4 };
  AR(int) ref    = { .size = 4, .values = v };

  ptrdiff_t index;
  LOWER_BOUND(index, ref, 4, kit_less_int);
  REQUIRE(index == 3);
}

TEST("lower bound fifth of four") {
  int const v[4] = { 1, 2, 3, 4 };
  AR(int) ref    = { .size = 4, .values = v };

  ptrdiff_t index;
  LOWER_BOUND(index, ref, 5, kit_less_int);
  REQUIRE(index == 4);
}

TEST("lower bound first of five") {
  int const v[5] = { 1, 2, 3, 4, 5 };
  AR(int) ref    = { .size = 5, .values = v };

  ptrdiff_t index;
  LOWER_BOUND(index, ref, 1, kit_less_int);
  REQUIRE(index == 0);
}

TEST("lower bound second of five") {
  int const v[5] = { 1, 2, 3, 4, 5 };
  AR(int) ref    = { .size = 5, .values = v };

  ptrdiff_t index;
  LOWER_BOUND(index, ref, 2, kit_less_int);
  REQUIRE(index == 1);
}

TEST("lower bound third of five") {
  int const v[5] = { 1, 2, 3, 4, 5 };
  AR(int) ref    = { .size = 5, .values = v };

  ptrdiff_t index;
  LOWER_BOUND(index, ref, 3, kit_less_int);
  REQUIRE(index == 2);
}

TEST("lower bound forth of five") {
  int const v[5] = { 1, 2, 3, 4, 5 };
  AR(int) ref    = { .size = 5, .values = v };

  ptrdiff_t index;
  LOWER_BOUND(index, ref, 4, kit_less_int);
  REQUIRE(index == 3);
}

TEST("lower bound fifth of five") {
  int const v[5] = { 1, 2, 3, 4, 5 };
  AR(int) ref    = { .size = 5, .values = v };

  ptrdiff_t index;
  LOWER_BOUND(index, ref, 5, kit_less_int);
  REQUIRE(index == 4);
}

TEST("lower bound sixth of five") {
  int const v[5] = { 1, 2, 3, 4, 5 };
  AR(int) ref    = { .size = 5, .values = v };

  ptrdiff_t index;
  LOWER_BOUND(index, ref, 6, kit_less_int);
  REQUIRE(index == 5);
}

TEST("lower bound ref first of four") {
  int const v[4]  = { 1, 2, 3, 4 };
  int const value = 1;
  AR(int) ref     = { .size = 4, .values = v };

  ptrdiff_t index;
  LOWER_BOUND_REF(index, ref, &value, kit_less_int_ref);
  REQUIRE(index == 0);
}

TEST("lower bound ref second of four") {
  int const v[4]  = { 1, 2, 3, 4 };
  int const value = 2;
  AR(int) ref     = { .size = 4, .values = v };

  ptrdiff_t index;
  LOWER_BOUND_REF(index, ref, &value, kit_less_int_ref);
  REQUIRE(index == 1);
}

TEST("lower bound ref fifth of five") {
  int const v[5]  = { 1, 2, 3, 4, 5 };
  int const value = 5;
  AR(int) ref     = { .size = 5, .values = v };

  ptrdiff_t index;
  LOWER_BOUND_REF(index, ref, &value, kit_less_int_ref);
  REQUIRE(index == 4);
}

TEST("lower bound ref sixth of five") {
  int const v[5]  = { 1, 2, 3, 4, 5 };
  int const value = 6;
  AR(int) ref     = { .size = 5, .values = v };

  ptrdiff_t index;
  LOWER_BOUND_REF(index, ref, &value, kit_less_int_ref);
  REQUIRE(index == 5);
}
