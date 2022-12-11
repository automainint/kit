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

#define KIT_UWORD_MAX ((uint_fast32_t) -1)
#define KIT_UWORD_SIZE sizeof(uint_fast32_t)

static_assert(sizeof(uint8_t) == 1, "uint8_t size should be 1 byte");
static_assert(sizeof(uint32_t) == 4,
              "uint32_t size should be 4 bytes");
static_assert(sizeof(uint64_t) == 8,
              "uint64_t size should be 8 bytes");
static_assert(KIT_BIGINT_SIZE > 0 && (KIT_BIGINT_SIZE % 8) == 0,
              "Invalid big integer size");

typedef struct {
  union {
    struct {
      uint8_t v8[KIT_BIGINT_SIZE];
    };
    struct {
      uint32_t v32[KIT_BIGINT_SIZE / 4];
    };
    struct {
      uint64_t v64[KIT_BIGINT_SIZE / 8];
    };
    struct {
      uint_fast32_t v[KIT_BIGINT_SIZE / KIT_UWORD_SIZE];
    };
  };
} kit_bigint_t;

typedef uint_fast32_t kit_uword_t;
typedef uint_fast8_t  kit_bit_t;

#ifdef __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wunused-function"
#  pragma GCC diagnostic ignored "-Wunknown-pragmas"
#  pragma GCC            push_options
#  pragma GCC            optimize("O3")
#endif

static kit_bigint_t kit_bi_uword(kit_uword_t const x) {
  kit_bigint_t z;
  memset(z.v8, 0, KIT_BIGINT_SIZE);
  z.v[0] = x;
  return z;
}

static kit_bigint_t kit_bi_uint64(uint64_t const x) {
  kit_bigint_t z;
  memset(z.v8, 0, KIT_BIGINT_SIZE);
  z.v64[0] = x;
  return z;
}

static int kit_bi_equal(kit_bigint_t const x, kit_bigint_t const y) {
  return kit_ar_equal_bytes(1, KIT_BIGINT_SIZE, x.v8, 1,
                            KIT_BIGINT_SIZE, y.v8);
}

static int kit_bi_compare(kit_bigint_t const x,
                          kit_bigint_t const y) {
  for (ptrdiff_t i = KIT_BIGINT_SIZE / KIT_UWORD_SIZE - 1; i >= 0;
       i--)
    if (x.v[i] < y.v[i])
      return -1;
    else if (x.v[i] > y.v[i])
      return 1;
  return 0;
}

static ptrdiff_t kit_bi_significant_bit_count(kit_bigint_t const x) {
  ptrdiff_t bytes = KIT_BIGINT_SIZE - 1;

  while (bytes > 0 && x.v8[bytes] == 0) bytes--;

  uint8_t const byte = x.v8[bytes];

  if (byte == 0)
    return 0;

  ptrdiff_t const bits = (byte & 0x80) != 0   ? 8
                         : (byte & 0x40) != 0 ? 7
                         : (byte & 0x20) != 0 ? 6
                         : (byte & 0x10) != 0 ? 5
                         : (byte & 0x08) != 0 ? 4
                         : (byte & 0x04) != 0 ? 3
                         : (byte & 0x02) != 0 ? 2
                                              : 1;

  return bytes * 8 + bits;
}

static kit_bigint_t kit_bi_and(kit_bigint_t const x,
                               kit_bigint_t const y) {
  kit_bigint_t z;

  for (ptrdiff_t i = 0; i < KIT_BIGINT_SIZE / KIT_UWORD_SIZE; i++)
    z.v[i] = x.v[i] & y.v[i];

  return z;
}

static kit_bigint_t kit_bi_or(kit_bigint_t const x,
                              kit_bigint_t const y) {
  kit_bigint_t z;

  for (ptrdiff_t i = 0; i < KIT_BIGINT_SIZE / KIT_UWORD_SIZE; i++)
    z.v[i] = x.v[i] | y.v[i];

  return z;
}

static kit_bigint_t kit_bi_xor(kit_bigint_t const x,
                               kit_bigint_t const y) {
  kit_bigint_t z;

  for (ptrdiff_t i = 0; i < KIT_BIGINT_SIZE / KIT_UWORD_SIZE; i++)
    z.v[i] = x.v[i] ^ y.v[i];

  return z;
}

static kit_bigint_t kit_bi_shl_uword(kit_bigint_t const x,
                                     kit_uword_t        y) {
  kit_bigint_t z;
  memset(z.v8, 0, KIT_BIGINT_SIZE);

  ptrdiff_t const words = (ptrdiff_t) (y / (8 * KIT_UWORD_SIZE));
  ptrdiff_t const bits  = (ptrdiff_t) (y % (8 * KIT_UWORD_SIZE));

  for (ptrdiff_t i = words; i < KIT_BIGINT_SIZE / KIT_UWORD_SIZE;
       i++) {
    z.v[i] |= x.v[i - words] << bits;
    if (bits != 0 && i + 1 < KIT_BIGINT_SIZE / KIT_UWORD_SIZE)
      z.v[i + 1] = x.v[i - words] >> (8 * KIT_UWORD_SIZE - bits);
  }

  return z;
}

static kit_bigint_t kit_bi_shr_uword(kit_bigint_t const x,
                                     kit_uword_t        y) {
  kit_bigint_t z;
  memset(z.v8, 0, KIT_BIGINT_SIZE);

  ptrdiff_t const words = (ptrdiff_t) (y / (8 * KIT_UWORD_SIZE));
  ptrdiff_t const bits  = (ptrdiff_t) (y % (8 * KIT_UWORD_SIZE));

  for (ptrdiff_t i = KIT_BIGINT_SIZE / KIT_UWORD_SIZE - words - 1;
       i >= 0; i--) {
    z.v[i] |= x.v[i + words] >> bits;
    if (bits != 0 && i > 0)
      z.v[i - 1] = x.v[i + words] << (8 * KIT_UWORD_SIZE - bits);
  }

  return z;
}

static kit_bit_t kit_bi_carry(kit_uword_t const x,
                              kit_uword_t const y,
                              kit_bit_t const   carry) {
  assert(carry == 0 || carry == 1);
  return KIT_UWORD_MAX - x < y || KIT_UWORD_MAX - x - y < carry ? 1
                                                                : 0;
}

static kit_bigint_t kit_bi_add(kit_bigint_t const x,
                               kit_bigint_t const y) {
  kit_bigint_t z;
  kit_bit_t    carry = 0;

  for (ptrdiff_t i = 0; i < KIT_BIGINT_SIZE / KIT_UWORD_SIZE; i++) {
    z.v[i] = x.v[i] + y.v[i] + carry;
    carry  = kit_bi_carry(x.v[i], y.v[i], carry);
  }

  return z;
}

static kit_bigint_t kit_bi_neg(kit_bigint_t const x) {
  kit_bigint_t y;
  kit_bit_t    carry = 1;

  for (ptrdiff_t i = 0; i < KIT_BIGINT_SIZE / KIT_UWORD_SIZE; i++) {
    y.v[i] = (x.v[i] ^ KIT_UWORD_MAX) + carry;
    carry  = kit_bi_carry(x.v[i] ^ KIT_UWORD_MAX, 0, carry);
  }

  return y;
}

static kit_bigint_t kit_bi_sub(kit_bigint_t const x,
                               kit_bigint_t const y) {
  kit_bigint_t z;
  kit_bit_t    carry = 1;

  for (ptrdiff_t i = 0; i < KIT_BIGINT_SIZE / KIT_UWORD_SIZE; i++) {
    z.v[i] = x.v[i] + (y.v[i] ^ KIT_UWORD_MAX) + carry;
    carry  = kit_bi_carry(x.v[i], (y.v[i] ^ KIT_UWORD_MAX), carry);
  }

  return z;
}

static kit_bigint_t kit_bi_mul_uint32(kit_bigint_t const x,
                                      uint32_t const     y) {
  kit_bigint_t z;
  memset(z.v8, 0, KIT_BIGINT_SIZE);

  if (y != 0)
    for (ptrdiff_t i = 0; i < KIT_BIGINT_SIZE / 4; i++) {
      if (x.v32[i] == 0)
        continue;

      uint64_t carry = ((uint64_t) x.v32[i]) * ((uint64_t) y);

      for (ptrdiff_t k = i; k < KIT_BIGINT_SIZE / 4 && carry != 0;
           k++) {
        uint64_t const sum = ((uint64_t) z.v32[k]) + carry;
        z.v32[k]           = ((uint32_t) (sum & 0xffffffffull));
        carry              = sum >> 32;
      }
    }

  return z;
}

static kit_bigint_t kit_bi_mul(kit_bigint_t const x,
                               kit_bigint_t const y) {
  kit_bigint_t z;
  memset(z.v8, 0, KIT_BIGINT_SIZE);

  for (ptrdiff_t i = 0; i < KIT_BIGINT_SIZE / 4; i++) {
    if (x.v32[i] == 0)
      continue;

    for (ptrdiff_t j = 0; i + j < KIT_BIGINT_SIZE / 4; j++) {
      if (y.v32[j] == 0)
        continue;

      uint64_t carry = ((uint64_t) x.v32[i]) * ((uint64_t) y.v32[j]);

      for (ptrdiff_t k = i + j; k < KIT_BIGINT_SIZE / 4 && carry != 0;
           k++) {
        uint64_t const sum = ((uint64_t) z.v32[k]) + carry;
        z.v32[k]           = ((uint32_t) (sum & 0xffffffffull));
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

static kit_bi_division_t kit_bi_div(kit_bigint_t const x,
                                    kit_bigint_t       y) {
  kit_bi_division_t result;
  memset(&result, 0, sizeof result);

  ptrdiff_t const y_bits = kit_bi_significant_bit_count(y);

  if (y_bits == 0) {
    result.undefined = 1;
    return result;
  }

  ptrdiff_t const x_bits = kit_bi_significant_bit_count(x);
  ptrdiff_t       shift  = x_bits - y_bits;

  result.remainder = x;
  result.quotient  = kit_bi_uword(0);

  y = kit_bi_shl_uword(y, (kit_uword_t) shift);

  while (shift >= 0) {
    if (kit_bi_compare(result.remainder, y) >= 0) {
      result.remainder = kit_bi_sub(result.remainder, y);
      result.quotient.v8[shift / 8] |= (1u << (shift % 8));
    }

    y = kit_bi_shr_uword(y, 1);
    shift--;
  }

  return result;
}

static uint8_t kit_bin_digit(char const hex) {
  return hex == '1' ? 1 : 0;
}

static kit_bigint_t kit_bi_bin(kit_str_t const bin) {
  kit_bigint_t z;
  memset(z.v8, 0, KIT_BIGINT_SIZE);

  for (ptrdiff_t i = 0; i < bin.size && i / 8 < KIT_BIGINT_SIZE;
       i++) {
    uint8_t const digit = kit_bin_digit(bin.values[bin.size - i - 1]);
    z.v8[i / 8] |= digit << (i % 8);
  }

  return z;
}

static uint8_t kit_dec_digit(char const c) {
  return c >= '0' && c <= '9' ? (uint8_t) (c - '0') : 0;
}

static kit_bigint_t kit_bi_dec(kit_str_t const dec) {
  kit_bigint_t z      = kit_bi_uword(0);
  kit_bigint_t factor = kit_bi_uword(1);

  for (ptrdiff_t i = 0; i < dec.size; i++) {
    uint32_t const digit = kit_dec_digit(
        dec.values[dec.size - i - 1]);
    z      = kit_bi_add(z, kit_bi_mul_uint32(factor, digit));
    factor = kit_bi_mul_uint32(factor, 10);
  }

  return z;
}

static uint8_t kit_hex_digit(char const hex) {
  if (hex >= '0' && hex <= '9')
    return hex - '0';
  if (hex >= 'a' && hex <= 'f')
    return hex - 'a';
  if (hex >= 'A' && hex <= 'F')
    return hex - 'A';
  return 0;
}

static kit_bigint_t kit_bi_hex(kit_str_t const hex) {
  kit_bigint_t z;
  memset(z.v8, 0, KIT_BIGINT_SIZE);

  for (ptrdiff_t i = 0; i < hex.size && i / 2 < KIT_BIGINT_SIZE;
       i++) {
    uint8_t const digit = kit_hex_digit(hex.values[hex.size - i - 1]);
    z.v8[i / 2] |= ((i % 2) == 0) ? digit : (digit << 4);
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
  return c >= '\0' && c < (sizeof KIT_BASE32_DIGITS)
             ? KIT_BASE32_DIGITS[(size_t) (unsigned char) c]
             : 0;
}

static kit_bigint_t kit_bi_base32(kit_str_t const base32) {
  kit_bigint_t z;
  memset(z.v8, 0, KIT_BIGINT_SIZE);

  for (ptrdiff_t i = 0; i < base32.size; i++) {
    z = kit_bi_shl_uword(z, 5 * i);
    z.v8[i] |= kit_base32_digit(base32.values[i]);
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
  return c >= '\0' && c < (sizeof KIT_BASE58_DIGITS)
             ? KIT_BASE58_DIGITS[(size_t) (unsigned char) c]
             : 0;
}

static kit_bigint_t kit_bi_base58(kit_str_t const base58) {
  kit_bigint_t z      = kit_bi_uword(0);
  kit_bigint_t factor = kit_bi_uword(1);

  for (ptrdiff_t i = 0; i < base58.size; i++) {
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
  kit_bi_bin(kit_str(sizeof(static_str_) - 1, (static_str_)))

#define KIT_DEC(static_str_) \
  kit_bi_dec(kit_str(sizeof(static_str_) - 1, (static_str_)))

#define KIT_HEX(static_str_) \
  kit_bi_hex(kit_str(sizeof(static_str_) - 1, (static_str_)))

#define KIT_BASE32(static_str_) \
  kit_bi_base32(kit_str(sizeof(static_str_) - 1, (static_str_)))

#define KIT_BASE58(static_str_) \
  kit_bi_base58(kit_str(sizeof(static_str_) - 1, (static_str_)))

#ifndef KIT_DISABLE_SHORT_NAMES
#  define bigint_t kit_bigint_t
#  define bi_uword kit_bi_uword
#  define bi_uint64 kit_bi_uint64
#  define bi_equal kit_bi_equal
#  define bi_carry kit_bi_carry
#  define bi_add kit_bi_add
#  define bi_neg kit_bi_neg
#  define bi_sub kit_bi_sub
#  define bi_mul kit_bi_mul
#  define bi_div kit_bi_div
#  define hex_digit kit_hex_digit
#  define bi_hex kit_bi_hex
#  define bi_base58 kit_bi_base58
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
