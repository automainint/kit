#ifndef KIT_STRING_REF_H
#define KIT_STRING_REF_H

#include "array_ref.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef KIT_AR_MUT(char) kit_string_mut_t;
typedef KIT_AR(char) kit_string_ref_t;

typedef kit_string_mut_t kit_out_str_t;
typedef kit_string_ref_t kit_str_t;

#ifdef __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wunused-function"
#  pragma GCC diagnostic ignored "-Wunknown-pragmas"
#  pragma GCC            push_options
#  pragma GCC            optimize("O3")
#endif

static kit_str_t kit_str(ptrdiff_t const   size,
                         char const *const static_string) {
  kit_str_t const s = { .size = size, .values = static_string };
  return s;
}

/*  Make a barbarian string for C standard library functions.
 *  Not thread safe.
 *  Use with caution.
 */
static char const *kit_make_bs(kit_str_t const s) {
  static char buf[8][4096];
  static int  index = 0;
  ptrdiff_t   n     = s.size;
  if (n > 4095)
    n = 4095;
  memcpy(buf[index], s.values, n);
  buf[index][n]      = '\0';
  char const *result = buf[index];
  index              = (index + 1) % 8;
  return result;
}

#ifdef __GNUC__
#  pragma GCC            pop_options
#  pragma GCC diagnostic pop
#endif

#define KIT_SZ(static_str_) \
  kit_str(sizeof(static_str_) - 1, (static_str_))

#define KIT_WRAP_BS(string_) kit_str(strlen(string_), (string_))

#define KIT_WRAP_STR(string_) \
  kit_str((string_).size, (string_).values)

#ifndef KIT_DISABLE_SHORT_NAMES
#  define string_mut_t kit_string_mut_t
#  define string_ref_t kit_string_ref_t
#  define out_str_t kit_out_str_t
#  define str_t kit_str_t

#  define SZ KIT_SZ
#  define BS kit_make_bs
#  define WRAP_BS KIT_WRAP_BS
#  define WRAP_STR KIT_WRAP_STR
#endif

#ifdef __cplusplus
}
#endif

#endif
