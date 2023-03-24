#include "allocator.h"

#include <assert.h>

#ifndef KIT_DISABLE_SYSTEM_MALLOC
#  include <stdlib.h>
#endif

static void *allocate(int request, void *state, ptrdiff_t size,
                      ptrdiff_t previous_size, void *pointer) {
#ifndef KIT_DISABLE_SYSTEM_MALLOC
  switch (request) {
    case KIT_ALLOCATE:
      assert(previous_size == 0);
      assert(pointer == NULL);
      return malloc(size);

    case KIT_DEALLOCATE:
      assert(size == 0);
      assert(pointer != NULL);
      free(pointer);
      return NULL;

    case KIT_REALLOCATE:
      /*  FIXME
       *  Not implemented.
       */
      assert(0);
      return NULL;

    default: assert(0);
  }
#else
  assert(0);
#endif
  return NULL;
}

#ifndef KIT_ENABLE_CUSTOM_ALLOC_DISPATCH
void *kit_alloc_dispatch(kit_allocator_t alloc, int request,
                         ptrdiff_t size, ptrdiff_t previous_size,
                         void *pointer) {
  assert(alloc.allocate != NULL);
  if (alloc.allocate == NULL)
    return NULL;
  return alloc.allocate(request, alloc.state, size, previous_size,
                        pointer);
}
#endif

kit_allocator_t kit_alloc_default(void) {
  kit_allocator_t alloc = { .state = NULL, .allocate = allocate };
  return alloc;
}
