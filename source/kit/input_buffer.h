#ifndef KIT_INPUT_BUFFER_H
#define KIT_INPUT_BUFFER_H

#include "dynamic_array.h"
#include "input_stream.h"
#include "status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  kit_status_t status;
  ptrdiff_t    offset;
  void        *internal;
  kit_string_t data;
} kit_ib_handle_t;

kit_ib_handle_t kit_ib_wrap(kit_is_handle_t upstream,
                            kit_allocator_t alloc);

kit_ib_handle_t kit_ib_read(kit_ib_handle_t buf, ptrdiff_t size);

void kit_ib_destroy(kit_ib_handle_t buf);

#define KIT_IB_WRAP(upstream) \
  kit_ib_wrap(upstream, kit_alloc_default())

#ifndef KIT_DISABLE_SHORT_NAMES
#  define ib_handle_t kit_ib_handle_t
#  define ib_wrap kit_ib_wrap
#  define ib_read kit_ib_read
#  define ib_destroy kit_ib_destroy

#  define IB_WRAP KIT_IB_WRAP
#endif

#ifdef __cplusplus
}
#endif

#endif
