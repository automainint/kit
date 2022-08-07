#include "../../kit/string_ref.h"

#define KIT_TEST_FILE string_ref
#include "../../kit_test/test.h"

TEST("static string wrap") {
  SZ(ref, "foo bar");

  REQUIRE(ref.size == 7);
  REQUIRE(ref.values[0] == 'f');
  REQUIRE(ref.values[1] == 'o');
  REQUIRE(ref.values[2] == 'o');
  REQUIRE(ref.values[3] == ' ');
  REQUIRE(ref.values[4] == 'b');
  REQUIRE(ref.values[5] == 'a');
  REQUIRE(ref.values[6] == 'r');
}
