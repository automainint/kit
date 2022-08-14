#ifndef KIT_STRING_REF_H
#define KIT_STRING_REF_H

#include "array_ref.h"

#ifdef __cplusplus
extern "C" {
#endif

KIT_AR_MUT_TYPE(kit_string_mut_t, char);
KIT_AR_TYPE(kit_string_ref_t, char);

typedef kit_string_mut_t kit_out_str_t;
typedef kit_string_ref_t kit_str_t;

#define KIT_SZ(name_, static_str_)                                  \
  kit_str_t name_ = {                                               \
    .size   = (sizeof(static_str_) / sizeof((static_str_)[0])) - 1, \
    .values = (static_str_)                                         \
  }

#ifndef KIT_DISABLE_SHORT_NAMES
#  define string_mut_t kit_string_mut_t
#  define string_ref_t kit_string_ref_t
#  define out_str_t kit_out_str_t
#  define str_t kit_str_t

#  define SZ KIT_SZ
#endif

#ifdef __cplusplus
}
#endif

#endif
