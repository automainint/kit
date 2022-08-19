#include "../../kit_test/test.h"

#include <cstdio>
#include <cstdlib>
#include <stdexcept>

TEST("c++ exception") {
  printf("** C++ EXCEPTION\n\n");
  throw std::exception {};
}

TEST("abort") {
  printf("** ABORT\n\n");
  abort();
}

TEST("invalid access") {
  printf("** INVALID ACCESS\n\n");
  *(volatile int *) nullptr = 42;
}

int main(int argc, char **argv) {
  printf(" %% run_tests\n\n");
  if (run_tests(argc, argv) != 1)
    return 1;

  return 0;
}
