#include "../../kit/dynamic_array.h"

#define KIT_TEST_FILE dynamic_array
#include "../../kit_test/test.h"

TEST("dynamic array empty") {
  DA_CREATE(v, char, 0);

  REQUIRE(v.size == 0);

  DA_DESTROY(v);
}

TEST("dynamic array resize") {
  DA_CREATE(v, char, 0);
  DA_RESIZE(v, 10);

  REQUIRE(v.size == 10);

  DA_DESTROY(v);
}

TEST("dynamic array grow") {
  DA_CREATE(v, char, 2);
  DA_RESIZE(v, 10);

  REQUIRE(v.size == 10);

  DA_DESTROY(v);
}

TEST("dynamic array diminish") {
  DA_CREATE(v, char, 10);
  DA_RESIZE(v, 9);

  REQUIRE(v.size == 9);

  DA_DESTROY(v);
}

TEST("dynamic array of chars") {
  DA_CREATE(v, char, 100);

  REQUIRE(v.size == 100);
  REQUIRE(v.capacity >= 100);
  REQUIRE(v.values != NULL);

  DA_DESTROY(v);
}

TEST("dynamic array push") {
  DA_CREATE(v, char, 0);
  DA_APPEND(v, 'x');

  REQUIRE(v.size == 1);
  REQUIRE(v.values[0] == 'x');

  DA_DESTROY(v);
}

TEST("dynamic array insert front") {
  DA_CREATE(v, char, 3);

  v.values[0] = 'a';
  v.values[1] = 'b';
  v.values[2] = 'c';

  DA_INSERT(v, 0, 'x');

  REQUIRE(v.size == 4);
  REQUIRE(v.values[0] == 'x');
  REQUIRE(v.values[1] == 'a');
  REQUIRE(v.values[2] == 'b');
  REQUIRE(v.values[3] == 'c');

  DA_DESTROY(v);
}

TEST("dynamic array insert back") {
  DA_CREATE(v, char, 3);

  v.values[0] = 'a';
  v.values[1] = 'b';
  v.values[2] = 'c';

  DA_INSERT(v, 3, 'x');

  REQUIRE(v.size == 4);
  REQUIRE(v.values[0] == 'a');
  REQUIRE(v.values[1] == 'b');
  REQUIRE(v.values[2] == 'c');
  REQUIRE(v.values[3] == 'x');

  DA_DESTROY(v);
}

TEST("dynamic array insert middle") {
  DA_CREATE(v, char, 3);

  v.values[0] = 'a';
  v.values[1] = 'b';
  v.values[2] = 'c';

  DA_INSERT(v, 2, 'x');

  REQUIRE(v.size == 4);
  REQUIRE(v.values[0] == 'a');
  REQUIRE(v.values[1] == 'b');
  REQUIRE(v.values[2] == 'x');
  REQUIRE(v.values[3] == 'c');

  DA_DESTROY(v);
}

TEST("dynamic array erase front") {
  DA_CREATE(v, char, 3);

  v.values[0] = 'a';
  v.values[1] = 'b';
  v.values[2] = 'c';

  DA_ERASE(v, 0);

  REQUIRE(v.size == 2);
  REQUIRE(v.values[0] == 'b');
  REQUIRE(v.values[1] == 'c');

  DA_DESTROY(v);
}

TEST("dynamic array erase back") {
  DA_CREATE(v, char, 3);

  v.values[0] = 'a';
  v.values[1] = 'b';
  v.values[2] = 'c';

  DA_ERASE(v, 2);

  REQUIRE(v.size == 2);
  REQUIRE(v.values[0] == 'a');
  REQUIRE(v.values[1] == 'b');

  DA_DESTROY(v);
}

TEST("dynamic array erase middle") {
  DA_CREATE(v, char, 3);

  v.values[0] = 'a';
  v.values[1] = 'b';
  v.values[2] = 'c';

  DA_ERASE(v, 1);

  REQUIRE(v.size == 2);
  REQUIRE(v.values[0] == 'a');
  REQUIRE(v.values[1] == 'c');

  DA_DESTROY(v);
}

TEST("dynamic array of ints") {
  DA_CREATE(v, int, 10);
  DA_RESIZE(v, 5);
  v.values[4] = 42;
  DA_APPEND(v, 43);

  REQUIRE(v.size == 6);
  REQUIRE(v.values[4] == 42);
  REQUIRE(v.values[5] == 43);

  DA_DESTROY(v);
}
