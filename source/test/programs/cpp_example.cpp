#include "../../kit_test/test.h"

TEST("foo") {
  REQUIRE(20 + 22 == 42);
}

TEST("bar") {
  REQUIRE(true);
}

auto main(int argc, char **argv) -> int {
  return run_tests(argc, argv);
}
