#include "allocator.h"

#include <stdlib.h>

static void *allocate(void *_, size_t size) {
  return malloc(size);
}

static void deallocate(void *_, void *pointer) {
  free(pointer);
}

struct kit_allocator kit_alloc_default() {
  struct kit_allocator alloc = { .state      = NULL,
                                 .allocate   = allocate,
                                 .deallocate = deallocate };
  return alloc;
}
