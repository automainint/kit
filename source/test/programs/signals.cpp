#include "../../kit_test/test.h"

#include <cstdio>
#include <cstdlib>
#include <stdexcept>

TEST("c++ exception") {
  printf("** C++ EXCEPTION\n\n");
  exit(42);
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
  if (run_tests(argc, argv) != 1)
    return 1;

  return 0;
}
