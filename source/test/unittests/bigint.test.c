#define KIT_BIGINT_SIZE 256
#include "../../kit/bigint.h"

#define KIT_TEST_FILE bigint
#include "../../kit_test/test.h"

static_assert(sizeof(bigint_t) == 256, "KIT_BIGINT_SIZE check");

TEST("bigint bin hex") {
  REQUIRE(bi_equal(HEX("10"), BIN("10000")));
  REQUIRE(bi_equal(HEX("20"), BIN("100000")));
  REQUIRE(bi_equal(HEX("40"), BIN("1000000")));
  REQUIRE(bi_equal(HEX("80"), BIN("10000000")));
  REQUIRE(bi_equal(HEX("310"), BIN("1100010000")));
  REQUIRE(bi_equal(HEX("420"), BIN("10000100000")));
  REQUIRE(bi_equal(HEX("540"), BIN("10101000000")));
  REQUIRE(bi_equal(HEX("780"), BIN("11110000000")));
}

TEST("bigint hex add") {
  REQUIRE(bi_equal(
      bi_add(HEX("4242424242424242424242424242424242424242"),
             HEX("1111111111111111111111111111111111111111")),
      HEX("5353535353535353535353535353535353535353")));
}

TEST("bigint hex sub") {
  REQUIRE(bi_equal(
      bi_sub(HEX("4242424242424242424242424242424242424242"),
             HEX("1111111111111111111111111111111111111111")),
      HEX("3131313131313131313131313131313131313131")));
}

TEST("bigint base58") {
  REQUIRE(bi_equal(BASE58("31"), bi_uint32(58 * 2)));
}

TEST("bigint base58 add") {
  REQUIRE(bi_equal(
      bi_add(BASE58("4242424242424242424242424242424242424242"),
             BASE58("2222222222222222222222222222222222222222")),
      BASE58("5353535353535353535353535353535353535353")));
}

TEST("bigint base58 sub") {
  REQUIRE(bi_equal(
      bi_sub(BASE58("4242424242424242424242424242424242424242"),
             BASE58("2222222222222222222222222222222222222222")),
      BASE58("3131313131313131313131313131313131313131")));
}

TEST("bigint base58 mul") {
  REQUIRE(bi_equal(bi_mul(BASE58("2111111111111111111111"),
                          BASE58("foofoofoofoofoo")),
                   BASE58("foofoofoofoofoo111111111111111111111")));
}

TEST("bigint div") {
  REQUIRE(bi_equal(bi_div(bi_int32(-1), bi_int32(-1)).quotient,
                   bi_int32(1)));
  REQUIRE(bi_equal(bi_div(bi_int32(-1), bi_int32(-1)).remainder,
                   bi_int32(0)));
  REQUIRE(bi_equal(bi_div(bi_int32(-3), bi_int32(2)).quotient,
                   bi_int32(-2)));
  REQUIRE(bi_equal(bi_div(bi_int32(-3), bi_int32(2)).remainder,
                   bi_int32(1)));
  REQUIRE(bi_equal(bi_div(bi_int32(3), bi_int32(-2)).quotient,
                   bi_int32(-1)));
  REQUIRE(bi_equal(bi_div(bi_int32(3), bi_int32(-2)).remainder,
                   bi_int32(1)));
  REQUIRE(bi_equal(bi_div(bi_int32(-3), bi_int32(4)).quotient,
                   bi_int32(-1)));
  REQUIRE(bi_equal(bi_div(bi_int32(-3), bi_int32(4)).remainder,
                   bi_int32(3)));
  REQUIRE(bi_equal(bi_div(bi_int32(3), bi_int32(-4)).quotient,
                   bi_int32(0)));
  REQUIRE(bi_equal(bi_div(bi_int32(3), bi_int32(-4)).remainder,
                   bi_int32(3)));


  REQUIRE(
      bi_equal(bi_div(HEX("100"), HEX("10")).quotient, HEX("10")));

  REQUIRE(bi_equal(bi_div(bi_mul(BASE58("foofoofoofoofoofoo"),
                                 BASE58("barbarbarbarbarbar")),
                          BASE58("barbarbarbarbarbar"))
                       .quotient,
                   BASE58("foofoofoofoofoofoo")));

  REQUIRE(bi_equal(bi_div(bi_mul(BASE58("foofoofoofoofoofoofoofoo"),
                                 BASE58("barbarbarbarbarbar")),
                          BASE58("barbarbarbarbarbar"))
                       .quotient,
                   BASE58("foofoofoofoofoofoofoofoo")));

  REQUIRE(bi_equal(
      bi_div(
          bi_mul(BASE58("foofoofoofoofoofoofoofoofoofoofoofoofoofoofo"
                        "ofoofoofoo"),
                 BASE58("barbarbarbarbarbarbarbarbarbarbarbarbarbarba"
                        "rbar")),
          BASE58("barbarbarbarbarbarbarbarbarbarbarbarbarbarbarbar"))
          .quotient,
      BASE58(
          "foofoofoofoofoofoofoofoofoofoofoofoofoofoofoofoofoofoo")));
}

