#ifndef KIT_ALLOCATOR_H
#define KIT_ALLOCATOR_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *(*kit_allocate_fn)(void *state, size_t size);
typedef void (*kit_deallocate_fn)(void *state, void *pointer);

struct kit_allocator {
  void              *state;
  kit_allocate_fn   allocate;
  kit_deallocate_fn deallocate;
};

struct kit_allocator kit_alloc_default();

#ifdef __cplusplus
}
#endif

#endif
