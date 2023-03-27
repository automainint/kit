#ifndef KIT_STRING_REF_H
#define KIT_STRING_REF_H

#include "array_ref.h"

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

#ifdef __GNUC__
#  pragma GCC            pop_options
#  pragma GCC diagnostic pop
#endif

#define KIT_SZ(static_str_) \
  kit_str(sizeof(static_str_) - 1, (static_str_))

#define KIT_WRAP_STR(string_) \
  kit_str((string_).size, (string_).values)

#ifndef KIT_DISABLE_SHORT_NAMES
#  define string_mut_t kit_string_mut_t
#  define string_ref_t kit_string_ref_t
#  define out_str_t kit_out_str_t
#  define str_t kit_str_t

#  define SZ KIT_SZ
#  define WRAP_STR KIT_WRAP_STR
#endif

#ifdef __cplusplus
}
#endif

#endif
