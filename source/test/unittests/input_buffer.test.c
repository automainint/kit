#include "../../kit/input_buffer.h"

#define KIT_TEST_FILE input_buffer
#include "../../kit_test/test.h"

TEST("input buffer") {
  str_t       text  = { .size = 3, .values = "foo" };
  is_handle_t in    = IS_WRAP_STRING(text);
  ib_handle_t first = IB_WRAP(in);

  ib_handle_t second = ib_read(first, 3);

  REQUIRE(!second.error);
  REQUIRE(second.data.size == 3);
  REQUIRE(AR_EQUAL(text, second.data));

  ib_destroy(second);
  ib_destroy(first);
  is_destroy(in);
}
