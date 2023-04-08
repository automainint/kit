#ifndef KIT_ALLOCATOR_H
#define KIT_ALLOCATOR_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

enum { KIT_ALLOCATE, KIT_DEALLOCATE, KIT_REALLOCATE };

typedef void *(*kit_allocate_fn)(int request, void *state,
                                 ptrdiff_t size,
                                 ptrdiff_t previous_size,
                                 void     *pointer);

typedef struct {
  void           *state;
  kit_allocate_fn allocate;
} kit_allocator_t;

/*  Application should implement this function if custom allocator
 *  dispatch is enabled.
 *
 *  See KIT_ENABLE_CUSTOM_ALLOC_DISPATCH macro.
 */
void *kit_alloc_dispatch(kit_allocator_t alloc, int request,
                         ptrdiff_t size, ptrdiff_t previous_size,
                         void *pointer);

kit_allocator_t kit_alloc_default(void);

#ifdef __cplusplus
}
#endif

#endif
