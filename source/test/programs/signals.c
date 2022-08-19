#include "../../kit_test/test.h"

#include <stdlib.h>

TEST("abort") {
#if !defined(_WIN32) || defined(__CYGWIN__)
  abort();
#endif
}

TEST("invalid access") {
#if !defined(_WIN32) || defined(__CYGWIN__)
  *(volatile int *) NULL = 42;
#endif
}

int main(int argc, char **argv) {
  if (run_tests(argc, argv) != 1)
    return 1;

  return 0;
}
