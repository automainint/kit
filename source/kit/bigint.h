#ifndef KIT_BIGINT_H
#define KIT_BIGINT_H

#include "string_ref.h"
#include <assert.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef KIT_BIGINT_SIZE
#  define KIT_BIGINT_SIZE 64
#endif

#if __STDC_VERSION__ >= 199901L
static_assert(sizeof(uint8_t) == 1, "uint8_t size should be 1 byte");
static_assert(sizeof(uint32_t) == 4,
              "uint32_t size should be 4 bytes");
static_assert(sizeof(uint64_t) == 8,
              "uint64_t size should be 8 bytes");
static_assert(KIT_BIGINT_SIZE > 0 && (KIT_BIGINT_SIZE % 8) == 0,
              "Invalid big integer size");
#endif

typedef struct {
  uint32_t v[KIT_BIGINT_SIZE / 4];
} kit_bigint_t;

typedef uint_fast8_t kit_bit_t;

#ifdef __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wunused-function"
#  pragma GCC diagnostic ignored "-Wunknown-pragmas"
#  pragma GCC            push_options
#  pragma GCC            optimize("O3")
#endif

static kit_bigint_t kit_bi_uint32(uint32_t const x) {
  kit_bigint_t z;
  memset(&z, 0, sizeof z);
  z.v[0] = x;
  return z;
}

static kit_bigint_t kit_bi_uint64(uint64_t const x) {
  kit_bigint_t z;
  memset(&z, 0, sizeof z);
  z.v[0] = (uint32_t) (x & 0xffffffff);
  z.v[1] = (uint32_t) (x >> 32);
  return z;
}

static kit_bigint_t kit_bi_int32(int32_t const x) {
  kit_bigint_t z;
  memset(&z, x < 0 ? -1 : 0, sizeof z);
  z.v[0] = x;
  return z;
}

static kit_bigint_t kit_bi_int64(int64_t const x) {
  kit_bigint_t z;
  memset(&z, x < 0 ? -1 : 0, sizeof z);
  z.v[0] = (uint32_t) (((uint64_t) x) & 0xffffffff);
  z.v[1] = (uint32_t) (((uint64_t) x) >> 32);
  return z;
}

static int kit_bi_is_zero(kit_bigint_t const x) {
  ptrdiff_t i;
  for (i = 0; i < KIT_BIGINT_SIZE / 4; i++)
    if (x.v[i] != 0)
      return 0;
  return 1;
}

static int kit_bi_is_neg(kit_bigint_t const x) {
  return (x.v[KIT_BIGINT_SIZE / 4 - 1] & 0x80000000) != 0;
}

static int kit_bi_equal(kit_bigint_t const x, kit_bigint_t const y) {
  return kit_ar_equal_bytes(1, KIT_BIGINT_SIZE, x.v, 1,
                            KIT_BIGINT_SIZE, y.v);
}

static int kit_bi_compare(kit_bigint_t const x,
                          kit_bigint_t const y) {
  ptrdiff_t i;
  for (i = KIT_BIGINT_SIZE / 4 - 1; i >= 0; i--)
    if (x.v[i] < y.v[i])
      return -1;
    else if (x.v[i] > y.v[i])
      return 1;
  return 0;
}

static ptrdiff_t kit_bi_significant_bit_count(kit_bigint_t const x) {
  ptrdiff_t n = KIT_BIGINT_SIZE / 4 - 1;

  while (n > 0 && x.v[n] == 0) n--;

  uint32_t const i32 = x.v[n];

  if (i32 == 0)
    return 0;

  ptrdiff_t const bits = (i32 & 0x80000000u) != 0   ? 32
                         : (i32 & 0x40000000u) != 0 ? 31
                         : (i32 & 0x20000000u) != 0 ? 30
                         : (i32 & 0x10000000u) != 0 ? 29
                         : (i32 & 0x8000000u) != 0  ? 28
                         : (i32 & 0x4000000u) != 0  ? 27
                         : (i32 & 0x2000000u) != 0  ? 26
                         : (i32 & 0x1000000u) != 0  ? 25
                         : (i32 & 0x800000u) != 0   ? 24
                         : (i32 & 0x400000u) != 0   ? 23
                         : (i32 & 0x200000u) != 0   ? 22
                         : (i32 & 0x100000u) != 0   ? 21
                         : (i32 & 0x80000u) != 0    ? 20
                         : (i32 & 0x40000u) != 0    ? 19
                         : (i32 & 0x20000u) != 0    ? 18
                         : (i32 & 0x10000u) != 0    ? 17
                         : (i32 & 0x8000u) != 0     ? 16
                         : (i32 & 0x4000u) != 0     ? 15
                         : (i32 & 0x2000u) != 0     ? 14
                         : (i32 & 0x1000u) != 0     ? 13
                         : (i32 & 0x800u) != 0      ? 12
                         : (i32 & 0x400u) != 0      ? 11
                         : (i32 & 0x200u) != 0      ? 10
                         : (i32 & 0x100u) != 0      ? 9
                         : (i32 & 0x80u) != 0       ? 8
                         : (i32 & 0x40u) != 0       ? 7
                         : (i32 & 0x20u) != 0       ? 6
                         : (i32 & 0x10u) != 0       ? 5
                         : (i32 & 0x08u) != 0       ? 4
                         : (i32 & 0x04u) != 0       ? 3
                         : (i32 & 0x02u) != 0       ? 2
                                                    : 1;

  return n * 32 + bits;
}

static kit_bigint_t kit_bi_and(kit_bigint_t const x,
                               kit_bigint_t const y) {
  kit_bigint_t z;
  ptrdiff_t    i;

  for (i = 0; i < KIT_BIGINT_SIZE / 4; i++) z.v[i] = x.v[i] & y.v[i];

  return z;
}

static kit_bigint_t kit_bi_or(kit_bigint_t const x,
                              kit_bigint_t const y) {
  kit_bigint_t z;
  ptrdiff_t    i;

  for (i = 0; i < KIT_BIGINT_SIZE / 4; i++) z.v[i] = x.v[i] | y.v[i];

  return z;
}

static kit_bigint_t kit_bi_xor(kit_bigint_t const x,
                               kit_bigint_t const y) {
  kit_bigint_t z;
  ptrdiff_t    i;

  for (i = 0; i < KIT_BIGINT_SIZE / 4; i++) z.v[i] = x.v[i] ^ y.v[i];

  return z;
}

static kit_bigint_t kit_bi_shl_uint(kit_bigint_t const x,
                                    uint32_t const     y) {
  kit_bigint_t z;
  memset(&z, 0, sizeof z);

  ptrdiff_t const words = (ptrdiff_t) (y / 32);
  ptrdiff_t const bits  = (ptrdiff_t) (y % 32);
  ptrdiff_t       i;

  for (i = words; i < KIT_BIGINT_SIZE / 4; i++) {
    z.v[i] |= x.v[i - words] << bits;
    if (bits != 0 && i + 1 < KIT_BIGINT_SIZE / 4)
      z.v[i + 1] = x.v[i - words] >> (32 - bits);
  }

  return z;
}

static kit_bigint_t kit_bi_shr_uint(kit_bigint_t const x,
                                    uint32_t const     y) {
  kit_bigint_t z;
  memset(&z, 0, sizeof z);

  ptrdiff_t const words = (ptrdiff_t) (y / 32);
  ptrdiff_t const bits  = (ptrdiff_t) (y % 32);
  ptrdiff_t       i;

  for (i = KIT_BIGINT_SIZE / 4 - words - 1; i >= 0; i--) {
    z.v[i] |= x.v[i + words] >> bits;
    if (bits != 0 && i > 0)
      z.v[i - 1] = x.v[i + words] << (32 - bits);
  }

  return z;
}

static kit_bit_t kit_bi_carry(uint32_t const x, uint32_t const y,
                              kit_bit_t const carry) {
  assert(carry == 0 || carry == 1);
  return 0xffffffffu - x < y || 0xffffffffu - x - y < carry ? 1 : 0;
}

/*  Increment.
 */
static kit_bigint_t kit_bi_inc(kit_bigint_t const x) {
  kit_bigint_t z;
  kit_bit_t    carry = 1;
  ptrdiff_t    i;

  for (i = 0; i < KIT_BIGINT_SIZE / 4; i++) {
    z.v[i] = x.v[i] + carry;
    carry  = kit_bi_carry(x.v[i], 0, carry);
  }

  return z;
}

/*  Decrement
 */
static kit_bigint_t kit_bi_dec(kit_bigint_t const x) {
  kit_bigint_t z;
  kit_bit_t    carry = 0;
  ptrdiff_t    i;

  for (i = 0; i < KIT_BIGINT_SIZE / 4; i++) {
    z.v[i] = x.v[i] + 0xffffffff + carry;
    carry  = kit_bi_carry(x.v[i], 0xffffffff, carry);
  }

  return z;
}

/*  Addition.
 */
static kit_bigint_t kit_bi_add(kit_bigint_t const x,
                               kit_bigint_t const y) {
  kit_bigint_t z;
  kit_bit_t    carry = 0;
  ptrdiff_t    i;

  for (i = 0; i < KIT_BIGINT_SIZE / 4; i++) {
    z.v[i] = x.v[i] + y.v[i] + carry;
    carry  = kit_bi_carry(x.v[i], y.v[i], carry);
  }

  return z;
}

/*  Negation.
 */
static kit_bigint_t kit_bi_neg(kit_bigint_t const x) {
  kit_bigint_t y;
  kit_bit_t    carry = 1;
  ptrdiff_t    i;

  for (i = 0; i < KIT_BIGINT_SIZE / 4; i++) {
    y.v[i] = (x.v[i] ^ 0xffffffff) + carry;
    carry  = kit_bi_carry(x.v[i] ^ 0xffffffff, 0, carry);
  }

  return y;
}

/*  Subtraction.
 */
static kit_bigint_t kit_bi_sub(kit_bigint_t const x,
                               kit_bigint_t const y) {
  kit_bigint_t z;
  kit_bit_t    carry = 1;
  ptrdiff_t    i;

  for (i = 0; i < KIT_BIGINT_SIZE / 4; i++) {
    z.v[i] = x.v[i] + (y.v[i] ^ 0xffffffff) + carry;
    carry  = kit_bi_carry(x.v[i], (y.v[i] ^ 0xffffffff), carry);
  }

  return z;
}

static kit_bigint_t kit_bi_mul_uint32(kit_bigint_t const x,
                                      uint32_t const     y) {
  kit_bigint_t z;
  ptrdiff_t    i, k;

  memset(&z, 0, sizeof z);

  if (y != 0)
    for (i = 0; i < KIT_BIGINT_SIZE / 4; i++) {
      if (x.v[i] == 0)
        continue;

      uint64_t carry = ((uint64_t) x.v[i]) * ((uint64_t) y);

      for (k = i; k < KIT_BIGINT_SIZE / 4 && carry != 0; k++) {
        uint64_t const sum = ((uint64_t) z.v[k]) + carry;
        z.v[k]             = ((uint32_t) (sum & 0xffffffffull));
        carry              = sum >> 32;
      }
    }

  return z;
}

/*  Multiplication.
 */
static kit_bigint_t kit_bi_mul(kit_bigint_t const x,
                               kit_bigint_t const y) {
  kit_bigint_t z;
  ptrdiff_t    i, j, k;

  memset(&z, 0, sizeof z);

  for (i = 0; i < KIT_BIGINT_SIZE / 4; i++) {
    if (x.v[i] == 0)
      continue;

    for (j = 0; i + j < KIT_BIGINT_SIZE / 4; j++) {
      if (y.v[j] == 0)
        continue;

      uint64_t carry = ((uint64_t) x.v[i]) * ((uint64_t) y.v[j]);

      for (k = i + j; k < KIT_BIGINT_SIZE / 4 && carry != 0; k++) {
        uint64_t const sum = ((uint64_t) z.v[k]) + carry;
        z.v[k]             = ((uint32_t) (sum & 0xffffffffull));
        carry              = sum >> 32;
      }
    }
  }

  return z;
}

typedef struct {
  kit_bit_t    undefined;
  kit_bigint_t quotient;
  kit_bigint_t remainder;
} kit_bi_division_t;

/*  Unsigned division.
 */
static kit_bi_division_t kit_bi_udiv(kit_bigint_t const x,
                                     kit_bigint_t       y) {
  kit_bi_division_t z;
  memset(&z, 0, sizeof z);

  ptrdiff_t const y_bits = kit_bi_significant_bit_count(y);

  if (y_bits == 0) {
    z.undefined = 1;
    return z;
  }

  ptrdiff_t const x_bits = kit_bi_significant_bit_count(x);
  ptrdiff_t       shift  = x_bits - y_bits;

  z.remainder = x;
  z.quotient  = kit_bi_uint32(0);

  y = kit_bi_shl_uint(y, (uint32_t) shift);

  while (shift >= 0) {
    if (kit_bi_compare(z.remainder, y) >= 0) {
      z.remainder = kit_bi_sub(z.remainder, y);
      z.quotient.v[shift / 32] |= (1u << (shift % 32));
    }

    y = kit_bi_shr_uint(y, 1);
    shift--;
  }

  return z;
}

/*  Signed division.
 *
 *  Remainder is always a non-negative value less than absolute value
 *  of y.
 */
static kit_bi_division_t kit_bi_div(kit_bigint_t const x,
                                    kit_bigint_t const y) {
  int const x_neg = kit_bi_is_neg(x);
  int const y_neg = kit_bi_is_neg(y);

  kit_bigint_t const x_abs = x_neg ? kit_bi_neg(x) : x;
  kit_bigint_t const y_abs = y_neg ? kit_bi_neg(y) : y;

  if (x_neg == y_neg)
    return kit_bi_udiv(x_abs, y_abs);

  kit_bi_division_t z = kit_bi_udiv(x_abs, y_abs);

  if (!kit_bi_is_zero(z.remainder) && !y_neg)
    z.quotient = kit_bi_dec(kit_bi_neg(z.quotient));
  else
    z.quotient = kit_bi_neg(z.quotient);

  return z;
}

static void kit_bi_serialize(kit_bigint_t const in,
                             uint8_t *const     out) {
  ptrdiff_t i;

  assert(out != NULL);

  for (i = 0; i < KIT_BIGINT_SIZE / 4; i++) {
    out[i * 4]     = (uint8_t) (in.v[i] & 0xff);
    out[i * 4 + 1] = (uint8_t) ((in.v[i] >> 8) & 0xff);
    out[i * 4 + 2] = (uint8_t) ((in.v[i] >> 16) & 0xff);
    out[i * 4 + 3] = (uint8_t) ((in.v[i] >> 24) & 0xff);
  }
}

static kit_bigint_t kit_bi_deserialize(uint8_t const *const in) {
  ptrdiff_t    i;
  kit_bigint_t out;

  assert(in != NULL);

  memset(&out, 0, sizeof out);

  for (i = 0; i < KIT_BIGINT_SIZE; i++)
    out.v[i / 4] |= ((uint32_t) in[i]) << (8 * (i % 4));

  return out;
}

static uint8_t kit_bin_digit(char const hex) {
  assert(hex == '0' || hex == '1');
  return hex == '1' ? 1 : 0;
}

static kit_bigint_t kit_bi_from_bin(kit_str_t const bin) {
  kit_bigint_t z;
  ptrdiff_t    i;

  memset(&z, 0, sizeof z);

  for (i = 0; i < bin.size && i / 8 < KIT_BIGINT_SIZE; i++) {
    uint8_t const digit = kit_bin_digit(bin.values[bin.size - i - 1]);
    z.v[i / 32] |= digit << (i % 32);
  }

  return z;
}

static uint8_t kit_dec_digit(char const c) {
  assert('c' >= '0' && c <= '9');
  return c >= '0' && c <= '9' ? (uint8_t) (c - '0') : 0;
}

static kit_bigint_t kit_bi_from_dec(kit_str_t const dec) {
  kit_bigint_t z      = kit_bi_uint32(0);
  kit_bigint_t factor = kit_bi_uint32(1);
  ptrdiff_t    i;

  for (i = 0; i < dec.size; i++) {
    uint32_t const digit = kit_dec_digit(
        dec.values[dec.size - i - 1]);
    z      = kit_bi_add(z, kit_bi_mul_uint32(factor, digit));
    factor = kit_bi_mul_uint32(factor, 10);
  }

  return z;
}

static uint8_t kit_hex_digit(char const hex) {
  assert((hex >= '0' && hex <= '9') || (hex >= 'a' && hex <= 'f') ||
         (hex >= 'A' && hex <= 'F'));

  if (hex >= '0' && hex <= '9')
    return hex - '0';
  if (hex >= 'a' && hex <= 'f')
    return hex - 'a';
  if (hex >= 'A' && hex <= 'F')
    return hex - 'A';

  return 0;
}

static kit_bigint_t kit_bi_from_hex(kit_str_t const hex) {
  kit_bigint_t z;
  ptrdiff_t    i;

  memset(&z, 0, sizeof z);

  for (i = 0; i < hex.size && i / 2 < KIT_BIGINT_SIZE; i++) {
    uint8_t const digit = kit_hex_digit(hex.values[hex.size - i - 1]);
    z.v[i / 8] |= digit << (4 * (i % 8));
  }

  return z;
}

static const uint8_t KIT_BASE32_DIGITS[] = {
  ['1'] = 0,  ['2'] = 1,  ['3'] = 2,  ['4'] = 3,  ['5'] = 4,
  ['6'] = 5,  ['7'] = 6,  ['8'] = 7,  ['9'] = 8,  ['a'] = 9,
  ['b'] = 10, ['c'] = 11, ['d'] = 12, ['e'] = 13, ['f'] = 14,
  ['g'] = 15, ['h'] = 16, ['j'] = 17, ['k'] = 18, ['m'] = 19,
  ['n'] = 20, ['p'] = 21, ['q'] = 22, ['r'] = 23, ['s'] = 24,
  ['t'] = 25, ['u'] = 26, ['v'] = 27, ['w'] = 28, ['x'] = 29,
  ['y'] = 30, ['z'] = 31
};

static uint8_t kit_base32_digit(char const c) {
  assert(c >= '\0' && c < sizeof KIT_BASE32_DIGITS);
  assert(c == '1' ||
         KIT_BASE32_DIGITS[(size_t) (unsigned char) c] != 0);

  return c >= '\0' && c < sizeof KIT_BASE32_DIGITS
             ? KIT_BASE32_DIGITS[(size_t) (unsigned char) c]
             : 0;
}

static kit_bigint_t kit_bi_from_base32(kit_str_t const base32) {
  kit_bigint_t z;
  ptrdiff_t    i;

  memset(&z, 0, sizeof z);

  for (i = 0; i < base32.size; i++) {
    z = kit_bi_shl_uint(z, 5 * i);
    z.v[0] |= kit_base32_digit(base32.values[i]);
  }

  return z;
}

static const uint8_t KIT_BASE58_DIGITS[] = {
  ['1'] = 0,  ['2'] = 1,  ['3'] = 2,  ['4'] = 3,  ['5'] = 4,
  ['6'] = 5,  ['7'] = 6,  ['8'] = 7,  ['9'] = 8,  ['A'] = 9,
  ['B'] = 10, ['C'] = 11, ['D'] = 12, ['E'] = 13, ['F'] = 14,
  ['G'] = 15, ['H'] = 16, ['J'] = 17, ['K'] = 18, ['L'] = 19,
  ['M'] = 20, ['N'] = 21, ['P'] = 22, ['Q'] = 23, ['R'] = 24,
  ['S'] = 25, ['T'] = 26, ['U'] = 27, ['V'] = 28, ['W'] = 29,
  ['X'] = 30, ['Y'] = 31, ['Z'] = 32, ['a'] = 33, ['b'] = 34,
  ['c'] = 35, ['d'] = 36, ['e'] = 37, ['f'] = 38, ['g'] = 39,
  ['h'] = 40, ['i'] = 41, ['j'] = 42, ['k'] = 43, ['m'] = 44,
  ['n'] = 45, ['o'] = 46, ['p'] = 47, ['q'] = 48, ['r'] = 49,
  ['s'] = 50, ['t'] = 51, ['u'] = 52, ['v'] = 53, ['w'] = 54,
  ['x'] = 55, ['y'] = 56, ['z'] = 57
};

static uint8_t kit_base58_digit(char const c) {
  assert(c >= '\0' && c < sizeof KIT_BASE58_DIGITS);
  assert(c == '1' ||
         KIT_BASE58_DIGITS[(size_t) (unsigned char) c] != 0);

  return c >= '\0' && c < sizeof KIT_BASE58_DIGITS
             ? KIT_BASE58_DIGITS[(size_t) (unsigned char) c]
             : 0;
}

static kit_bigint_t kit_bi_from_base58(kit_str_t const base58) {
  kit_bigint_t z      = kit_bi_uint32(0);
  kit_bigint_t factor = kit_bi_uint32(1);
  ptrdiff_t    i;

  for (i = 0; i < base58.size; i++) {
    uint32_t const digit = kit_base58_digit(
        base58.values[base58.size - i - 1]);
    z      = kit_bi_add(z, kit_bi_mul_uint32(factor, digit));
    factor = kit_bi_mul_uint32(factor, 58);
  }

  return z;
}

#ifdef __GNUC__
#  pragma GCC            pop_options
#  pragma GCC diagnostic pop
#endif

#define KIT_BIN(static_str_) \
  kit_bi_from_bin(kit_str(sizeof(static_str_) - 1, (static_str_)))

#define KIT_DEC(static_str_) \
  kit_bi_from_dec(kit_str(sizeof(static_str_) - 1, (static_str_)))

#define KIT_HEX(static_str_) \
  kit_bi_from_hex(kit_str(sizeof(static_str_) - 1, (static_str_)))

#define KIT_BASE32(static_str_) \
  kit_bi_from_base32(kit_str(sizeof(static_str_) - 1, (static_str_)))

#define KIT_BASE58(static_str_) \
  kit_bi_from_base58(kit_str(sizeof(static_str_) - 1, (static_str_)))

#ifndef KIT_DISABLE_SHORT_NAMES
#  define bigint_t kit_bigint_t
#  define bi_uint32 kit_bi_uint32
#  define bi_uint64 kit_bi_uint64
#  define bi_int32 kit_bi_int32
#  define bi_int64 kit_bi_int64
#  define bi_is_zero kit_bi_is_zero
#  define bi_is_neg kit_bi_is_neg
#  define bi_equal kit_bi_equal
#  define bi_compare kit_bi_compare
#  define bi_carry kit_bi_carry
#  define bi_inc kit_bi_inc
#  define bi_dec kit_bi_dec
#  define bi_add kit_bi_add
#  define bi_neg kit_bi_neg
#  define bi_sub kit_bi_sub
#  define bi_mul kit_bi_mul
#  define bi_div kit_bi_div
#  define bi_serialize kit_bi_serialize
#  define bi_deserialize kit_bi_deserialize
#  define BIN KIT_BIN
#  define DEC KIT_DEC
#  define HEX KIT_HEX
#  define BASE32 KIT_BASE32
#  define BASE58 KIT_BASE58
#endif

#ifdef __cplusplus
}
#endif

#endif
