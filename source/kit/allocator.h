#ifndef KIT_ALLOCATOR_H
#define KIT_ALLOCATOR_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *(*kit_allocate_fn)(void *state, size_t size);
typedef void (*kit_deallocate_fn)(void *state, void *pointer);

typedef struct {
  void             *state;
  kit_allocate_fn   allocate;
  kit_deallocate_fn deallocate;
} kit_allocator_t;

kit_allocator_t kit_alloc_default(void);

#ifdef __cplusplus
}
#endif

#endif
