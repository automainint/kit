#include "../../kit_test/test.h"

TEST("foo") {
  for (int i = 0; i <= KIT_TEST_ASSERTIONS_LIMIT; i++) REQUIRE(1);
}

int main(int argc, char **argv) {
  if (run_tests(argc, argv) != 1)
    return 1;

  return 0;
}
