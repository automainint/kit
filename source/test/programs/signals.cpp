#include "../../kit_test/test.h"

#include <cstdlib>
#include <stdexcept>

/*  FIXME
 *  MSVC tests fail in GitHub Actions.
 */
#ifndef _MSC_VER
TEST("c++ exception") {
  throw std::exception {};
}

TEST("abort") {
  abort();
}

TEST("invalid access") {
  *(volatile int *) nullptr = 42;
}
#endif

int main(int argc, char **argv) {
#ifndef _MSC_VER
  if (run_tests(argc, argv) != 1)
    return 1;
#endif

  return 0;
}
