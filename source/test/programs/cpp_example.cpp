#include "../../kit_test/test.h"

TEST("foo") {
  REQUIRE(20 + 22 == 42);
}

TEST("bar") {
  REQUIRE(true);
}

int main(int argc, char **argv) {
  return run_tests(argc, argv);
}
