#include "allocator.h"

#ifndef KIT_DISABLE_SYSTEM_MALLOC
#  include <stdlib.h>
#endif

static void *allocate(void *_, size_t size) {
#ifndef KIT_DISABLE_SYSTEM_MALLOC
  return malloc(size);
#else
  return NULL;
#endif
}

static void deallocate(void *_, void *pointer) {
#ifndef KIT_DISABLE_SYSTEM_MALLOC
  free(pointer);
#endif
}

kit_allocator_t kit_alloc_default() {
  kit_allocator_t alloc = { .state      = NULL,
                            .allocate   = allocate,
                            .deallocate = deallocate };
  return alloc;
}
