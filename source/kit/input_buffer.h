#ifndef KIT_INPUT_BUFFER_H
#define KIT_INPUT_BUFFER_H

#include "dynamic_array.h"
#include "input_stream.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ib_handle {
  _Bool     error;
  ptrdiff_t offset;
  void     *internal;
  DA(data, char);
};

struct ib_handle ib_wrap(struct is_handle     upstream,
                         struct kit_allocator alloc);

struct ib_handle ib_read(struct ib_handle buf, ptrdiff_t size);

void ib_destroy(struct ib_handle buf);

#define IB_WRAP(upstream) ib_wrap(upstream, kit_alloc_default())

#ifdef __cplusplus
}
#endif

#endif
