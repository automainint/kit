#include "../../kit/secure_random.h"
#include <string.h>

#define KIT_TEST_FILE secure_random
#include "../../kit_test/test.h"

TEST("secure random") {
  int i, j;

  int v[20];
  memset(v, 0, sizeof v);

  secure_random(40, v);
  secure_random(40, v + 10);

  int repeats = 0;

  for (i = 1; i < sizeof v / sizeof *v; i++)
    for (j = 0; j < i; j++)
      if (v[i] == v[j])
        repeats++;

  REQUIRE(repeats == 0);
}
