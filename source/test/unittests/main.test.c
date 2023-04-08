#include "../../kit_test/bench.h"
#include "../../kit_test/test.h"

int main(int argc, char **argv) {
  int status = run_tests(argc, argv);
  if (status == 0)
    status = run_benchmarks(argc, argv);
  return status;
}
