#include "../../kit_test/test.h"

#include <cstdio>
#include <cstdlib>

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
