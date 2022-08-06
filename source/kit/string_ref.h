#ifndef KIT_STRING_REF_H
#define KIT_STRING_REF_H

#include "array_ref.h"

#ifdef __cplusplus
extern "C" {
#endif

AR_TYPE(string_ref, char);
AR_TYPE_CONST(string_cref, char);

typedef struct string_ref  out_str;
typedef struct string_cref cstr;

#ifdef __cplusplus
}
#endif

#endif
