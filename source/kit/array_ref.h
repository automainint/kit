#ifndef KIT_ARRAY_REF_H
#define KIT_ARRAY_REF_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*kit_ar_compare_fn)(void const *left, void const *right);

int kit_ar_equal_bytes(ptrdiff_t left_element_size,
                       ptrdiff_t left_size, void const *left_data,
                       ptrdiff_t right_element_size,
                       ptrdiff_t right_size, void const *right_data);

int kit_ar_compare(ptrdiff_t left_element_size, ptrdiff_t left_size,
                   void const *left_data,
                   ptrdiff_t right_element_size, ptrdiff_t right_size,
                   void const *right_data, kit_ar_compare_fn compare);

#define KIT_AR_MUT(type_) \
  struct {                       \
    ptrdiff_t size;              \
    type_    *values;            \
  } 

#define KIT_AR(type_) \
  struct {                   \
    ptrdiff_t    size;       \
    type_ const *values;     \
  } 

#define KIT_AR_MUT_WRAP(name_, element_type_, array_)           \
  struct {                                                      \
    ptrdiff_t      size;                                        \
    element_type_ *values;                                      \
  } name_ = { .size   = (sizeof(array_) / sizeof((array_)[0])), \
              .values = (array_) }

#define KIT_AR_WRAP(name_, element_type_, array_)               \
  struct {                                                      \
    ptrdiff_t            size;                                  \
    element_type_ const *values;                                \
  } name_ = { .size   = (sizeof(array_) / sizeof((array_)[0])), \
              .values = (array_) }

#define KIT_AR_EQUAL(left_, right_)                              \
  kit_ar_equal_bytes(sizeof((left_).values[0]), (left_).size,    \
                     (left_).values, sizeof((right_).values[0]), \
                     (right_).size, (right_).values)

#define KIT_AR_COMPARE(left_, right_, compare_)              \
  kit_ar_compare(sizeof((left_).values[0]), (left_).size,    \
                 (left_).values, sizeof((right_).values[0]), \
                 (right_).size, (right_).values,             \
                 (kit_ar_compare_fn) (compare_))

#ifndef KIT_DISABLE_SHORT_NAMES
#  define ar_compare_fn kit_ar_compare_fn
#  define ar_equal_bytes kit_ar_equal_bytes
#  define ar_compare kit_ar_compare

#  define AR_MUT KIT_AR_MUT
#  define AR KIT_AR
#  define AR_MUT_WRAP KIT_AR_MUT_WRAP
#  define AR_WRAP KIT_AR_WRAP
#  define AR_EQUAL KIT_AR_EQUAL
#  define AR_COMPARE KIT_AR_COMPARE
#endif

#ifdef __cplusplus
}
#endif

#endif
