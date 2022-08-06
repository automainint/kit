#ifndef KIT_INPUT_BUFFER_H
#define KIT_INPUT_BUFFER_H

#include "dynamic_array.h"
#include "input_stream.h"

#ifdef __cplusplus
extern "C" {
#endif

struct kit_ib_handle {
  _Bool     error;
  ptrdiff_t offset;
  void     *internal;
  DA(data, char);
};

struct kit_ib_handle kit_ib_wrap(struct kit_is_handle upstream,
                                 struct kit_allocator alloc);

struct kit_ib_handle kit_ib_read(struct kit_ib_handle buf,
                                 ptrdiff_t            size);

void kit_ib_destroy(struct kit_ib_handle buf);

#define KIT_IB_WRAP(upstream) \
  kit_ib_wrap(upstream, kit_alloc_default())

#ifndef KIT_DISABLE_SHORT_NAMES
#  define ib_handle kit_ib_handle
#  define ib_wrap kit_ib_wrap
#  define ib_read kit_ib_read
#  define ib_destroy kit_ib_destroy

#  define IB_WRAP KIT_IB_WRAP
#endif

#ifdef __cplusplus
}
#endif

#endif
