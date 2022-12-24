#include "dynamic_array.h"

#include <assert.h>
#include <string.h>

void kit_da_init(kit_da_void_t *array, ptrdiff_t element_size,
                 ptrdiff_t size, kit_allocator_t alloc) {
  assert(array != NULL);
  assert(element_size > 0);
  assert(size >= 0);
  assert(alloc.allocate != NULL);
  assert(alloc.deallocate != NULL);

  memset(array, 0, sizeof(kit_da_void_t));

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

void kit_da_resize(kit_da_void_t *array, ptrdiff_t element_size,
                   ptrdiff_t size) {
  assert(array != NULL);
  assert(element_size > 0);
  assert(size >= 0);

  if (size <= array->capacity) {
    array->size = size;
  } else {
    ptrdiff_t capacity = eval_capacity(array->capacity, size);

    assert(array->alloc.allocate != NULL);
    assert(array->alloc.deallocate != NULL);

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
