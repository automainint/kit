#include "../../kit/input_buffer.h"

#define KIT_TEST_FILE input_buffer
#include "../../kit_test/test.h"

TEST("input buffer read once") {
  str_t       text  = { .size = 3, .values = "foo" };
  is_handle_t in    = IS_WRAP_STRING(text);
  ib_handle_t first = IB_WRAP(in);

  ib_handle_t second = ib_read(first, 3);

  REQUIRE(second.status == KIT_OK);
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

static int is_integer_(str_t const data) {
  for (ptrdiff_t i = 0; i < data.size; i++)
    if (data.values[i] < '0' || data.values[i] > '9')
      return 0;
  return 1;
}

TEST("input buffer read integer once") {
  str_t       text  = { .size = 9, .values = "31415 foo" };
  str_t       num   = { .size = 5, .values = "31415" };
  is_handle_t in    = IS_WRAP_STRING(text);
  ib_handle_t first = IB_WRAP(in);

  ib_handle_t second = ib_read_while(first, is_integer_);

  REQUIRE(second.status == KIT_OK);
  REQUIRE(second.data.size == 5);
  REQUIRE(AR_EQUAL(num, second.data));

  ib_destroy(second);
  ib_destroy(first);
  is_destroy(in);
}

TEST("input buffer read integer twice") {
  str_t       text  = { .size = 6, .values = "314 15" };
  str_t       num_0 = { .size = 3, .values = "314" };
  str_t       num_1 = { .size = 2, .values = "15" };
  is_handle_t in    = IS_WRAP_STRING(text);
  ib_handle_t first = IB_WRAP(in);

  ib_handle_t second = ib_read_while(first, is_integer_);
  ib_handle_t third  = ib_read(second, 1);
  ib_handle_t fourth = ib_read_while(third, is_integer_);

  REQUIRE(fourth.status == KIT_OK);
  REQUIRE(second.data.size == 3);
  REQUIRE(fourth.data.size == 2);
  REQUIRE(AR_EQUAL(num_0, second.data));
  REQUIRE(AR_EQUAL(num_1, fourth.data));

  ib_destroy(first);
  ib_destroy(second);
  ib_destroy(third);
  ib_destroy(fourth);
  is_destroy(in);
}
