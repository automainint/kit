#include "dynamic_array.h"

#include <assert.h>
#include <string.h>

void kit_da_init(kit_da_void_t *array, ptrdiff_t element_size,
                 ptrdiff_t size, kit_allocator_t alloc) {
  assert(array != NULL);
  assert(element_size > 0);
  assert(size >= 0);

  memset(array, 0, sizeof(kit_da_void_t));

  if (size > 0)
    array->values = kit_alloc_dispatch(alloc, KIT_ALLOCATE,
                                       element_size * size, 0, NULL);

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

    void *bytes = kit_alloc_dispatch(
        array->alloc, KIT_ALLOCATE, element_size * capacity, 0, NULL);

    if (bytes != NULL) {
      if (array->size > 0)
        memcpy(bytes, array->values, element_size * array->size);
      if (array->values != NULL)
        kit_alloc_dispatch(array->alloc, KIT_DEALLOCATE, 0, 0,
                           array->values);
      array->capacity = capacity;
      array->size     = size;
      array->values   = bytes;
    }
  }
}
