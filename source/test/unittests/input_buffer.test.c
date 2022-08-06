#include "../../kit/input_buffer.h"

#define KIT_TEST_FILE input_buffer
#include "../../kit_test/test.h"

TEST("input buffer") {
  cstr             text  = { .size = 3, .values = "foo" };
  struct is_handle in    = IS_WRAP_STRING(text);
  struct ib_handle first = IB_WRAP(in);

  struct ib_handle second = ib_read(first, 3);

  REQUIRE(!second.error);
  REQUIRE(second.data.size == 3);
  REQUIRE(AR_EQUAL(text, second.data));

  ib_destroy(second);
  ib_destroy(first);
  is_destroy(in);
}
