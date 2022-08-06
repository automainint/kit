#ifndef KIT_ARRAY_REF_H
#define KIT_ARRAY_REF_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*ar_compare_fn)(void const *left, void const *right);

_Bool ar_equal_bytes(ptrdiff_t left_element_size, ptrdiff_t left_size,
                     void const *left_data,
                     ptrdiff_t   right_element_size,
                     ptrdiff_t right_size, void const *right_data);

int ar_compare(ptrdiff_t left_element_size, ptrdiff_t left_size,
               void const *left_data, ptrdiff_t right_element_size,
               ptrdiff_t right_size, void const *right_data,
               ar_compare_fn compare);

#define AR(name_, type_) \
  struct {               \
    ptrdiff_t size;      \
    type_    *values;    \
  } name_

#define AR_CONST(name_, type_) \
  struct {                     \
    ptrdiff_t    size;         \
    type_ const *values;       \
  } name_

#define AR_TYPE(name_, element_type_) \
  struct name_ {                      \
    ptrdiff_t      size;              \
    element_type_ *values;            \
  }

#define AR_TYPE_CONST(name_, element_type_) \
  struct name_ {                            \
    ptrdiff_t            size;              \
    element_type_ const *values;            \
  }

#define AR_EQUAL(left_, right_)                              \
  ar_equal_bytes(sizeof((left_).values[0]), (left_).size,    \
                 (left_).values, sizeof((right_).values[0]), \
                 (right_).size, (right_).values)

#define AR_COMPARE(left_, right_, compare_)              \
  ar_compare(sizeof((left_).values[0]), (left_).size,    \
             (left_).values, sizeof((right_).values[0]), \
             (right_).size, (right_).values,             \
             (ar_compare_fn) (compare_))

#ifdef __cplusplus
}
#endif

#endif
