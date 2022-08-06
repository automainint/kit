#include "array_ref.h"

#include <string.h>

_Bool ar_equal_bytes(ptrdiff_t left_element_size, ptrdiff_t left_size,
                     void const *left_data,
                     ptrdiff_t   right_element_size,
                     ptrdiff_t right_size, void const *right_data) {
  if (left_element_size != right_element_size)
    return 0;
  if (left_size != right_size)
    return 0;
  for (ptrdiff_t i = 0; i < left_size; i++)
    if (memcmp((char const *) left_data + i * left_element_size,
               (char const *) right_data + i * left_element_size,
               left_element_size) != 0)
      return 0;
  return 1;
}

int ar_compare(ptrdiff_t left_element_size, ptrdiff_t left_size,
               void const *left_data, ptrdiff_t right_element_size,
               ptrdiff_t right_size, void const *right_data,
               ar_compare_fn compare) {
  if (left_element_size < right_element_size)
    return -1;
  if (left_element_size > right_element_size)
    return 1;
  for (ptrdiff_t i = 0; i < left_size && i < right_size; i++) {
    int const c = compare(
        (char const *) left_data + i * left_element_size,
        (char const *) right_data + i * left_element_size);
    if (c != 0)
      return c;
  }
  if (left_size < right_size)
    return -1;
  if (left_size > right_size)
    return 1;
  return 0;
}
