#include "../../kit_test/test.h"

void bar(int index, kit_test_report_fn report) { }

int main(int argc, char **argv) {
  int i;
  for (i = 0; i <= KIT_TESTS_SIZE_LIMIT; i++)
    test_register("foo", __FILE__, bar);

  if (run_tests(argc, argv) != 1)
    return 1;

  return 0;
}
