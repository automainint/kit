#include "../../kit/input_stream.h"

#define KIT_TEST_FILE input_stream
#include "../../kit_test/test.h"

TEST("input stream wrap string") {
  char foo[] = "test";
  char bar[] = "test";

  cstr foo_ref = { .size = sizeof(foo) - 1, .values = foo };
  cstr bar_ref = { .size = sizeof(bar) - 1, .values = bar };

  struct is_handle in = IS_WRAP_STRING(foo_ref);

  char    buf[4];
  out_str buf_ref = { .size = sizeof(buf), .values = buf };

  REQUIRE(IS_READ(in, buf_ref) == buf_ref.size);
  REQUIRE(AR_EQUAL(foo_ref, bar_ref));
  REQUIRE(AR_EQUAL(buf_ref, bar_ref));

  is_destroy(in);
}
