#include "../../kit/input_buffer.h"

#define KIT_TEST_FILE input_buffer
#include "../../kit_test/test.h"

TEST("input buffer read once") {
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

TEST("input buffer read again") {
  str_t       text  = { .size = 6, .values = "foobar" };
  str_t       foo   = { .size = 3, .values = "foo" };
  is_handle_t in    = IS_WRAP_STRING(text);
  ib_handle_t first = IB_WRAP(in);

  ib_handle_t second = ib_read(first, 3);
  ib_handle_t third  = ib_read(first, 3);

  REQUIRE(AR_EQUAL(foo, second.data));
  REQUIRE(AR_EQUAL(foo, third.data));

  ib_destroy(third);
  ib_destroy(second);
  ib_destroy(first);
  is_destroy(in);
}

TEST("input buffer read twice") {
  str_t       text  = { .size = 6, .values = "foobar" };
  str_t       foo   = { .size = 3, .values = "foo" };
  str_t       bar   = { .size = 3, .values = "bar" };
  is_handle_t in    = IS_WRAP_STRING(text);
  ib_handle_t first = IB_WRAP(in);

  ib_handle_t second = ib_read(first, 3);
  ib_handle_t third  = ib_read(second, 3);

  REQUIRE(AR_EQUAL(foo, second.data));
  REQUIRE(AR_EQUAL(bar, third.data));

  ib_destroy(third);
  ib_destroy(second);
  ib_destroy(first);
  is_destroy(in);
}
