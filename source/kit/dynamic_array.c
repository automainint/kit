#include "dynamic_array.h"

#include <string.h>

void kit_da_init(struct kit_da_void *array, ptrdiff_t element_size,
                 ptrdiff_t size, struct kit_allocator alloc) {
  memset(array, 0, sizeof(struct kit_da_void));

  if (size > 0)
    array->values = alloc.allocate(alloc.state, element_size * size);

  if (array->values != NULL) {
    array->capacity = size;
    array->size     = size;
  }

  array->alloc = alloc;
}

static ptrdiff_t eval_capacity(ptrdiff_t current_cap,
                               ptrdiff_t required_cap) {
  if (current_cap == 0)
    return required_cap;
  ptrdiff_t cap = current_cap;
  while (cap < required_cap) cap *= 2;
  return cap;
}

void kit_da_resize(struct kit_da_void *array, ptrdiff_t element_size,
                   ptrdiff_t size) {
  if (size <= array->capacity) {
    array->size = size;
  } else {
    ptrdiff_t capacity = eval_capacity(array->capacity, size);

    void *bytes = array->alloc.allocate(array->alloc.state,
                                        element_size * capacity);
    if (bytes != NULL) {
      if (array->size > 0)
        memcpy(bytes, array->values, element_size * array->size);
      if (array->values != NULL)
        array->alloc.deallocate(array->alloc.state, array->values);
      array->capacity = capacity;
      array->size     = size;
      array->values   = bytes;
    }
  }
}