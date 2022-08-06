#ifndef KIT_STRING_REF_H
#define KIT_STRING_REF_H

#include "array_ref.h"

#ifdef __cplusplus
extern "C" {
#endif

KIT_AR_TYPE(kit_string_ref, char);
KIT_AR_TYPE_CONST(kit_string_cref, char);

typedef struct kit_string_ref  kit_out_str;
typedef struct kit_string_cref kit_cstr;

#ifndef KIT_DISABLE_SHORT_NAMES
#  define string_ref kit_string_ref
#  define string_cref kit_string_cref
#  define out_str kit_out_str
#  define cstr kit_cstr
#endif

#ifdef __cplusplus
}
#endif

#endif
