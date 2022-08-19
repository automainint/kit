#include "../../kit_test/test.h"

#include <stdlib.h>

TEST("abort") {
  //abort();
}

TEST("invalid access") {
  //*(volatile int *) NULL = 42;
}

int main(int argc, char **argv) {
  if (run_tests(argc, argv) != 1)
    return 1;

  return 0;
}
