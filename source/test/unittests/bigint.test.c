#include "../../kit/bigint.h"

#define KIT_TEST_FILE bigint
#include "../../kit_test/test.h"

TEST("bigint hex add") {
  SZ(foo, "4242424242424242424242424242424242424242");
  SZ(bar, "1111111111111111111111111111111111111111");
  SZ(sum, "5353535353535353535353535353535353535353");

  REQUIRE(bi_equal(bi_add(bi_hex(foo), bi_hex(bar)), bi_hex(sum)));
}

TEST("bigint hex sub") {
  SZ(foo, "4242424242424242424242424242424242424242");
  SZ(bar, "1111111111111111111111111111111111111111");
  SZ(dif, "3131313131313131313131313131313131313131");

  REQUIRE(bi_equal(bi_sub(bi_hex(foo), bi_hex(bar)), bi_hex(dif)));
}

TEST("bigint base58") {
  SZ(foo, "31");

  REQUIRE(bi_equal(bi_base58(foo), bi_uword(58 * 2)));
}

TEST("bigint base58 add") {
  SZ(foo, "4242424242424242424242424242424242424242");
  SZ(bar, "2222222222222222222222222222222222222222");
  SZ(sum, "5353535353535353535353535353535353535353");

  REQUIRE(bi_equal(bi_add(bi_base58(foo), bi_base58(bar)),
                   bi_base58(sum)));
}

TEST("bigint base58 sub") {
  SZ(foo, "42");
  SZ(bar, "22");
  SZ(dif, "31");

  REQUIRE(bi_equal(bi_sub(bi_base58(foo), bi_base58(bar)),
                   bi_base58(dif)));
}

