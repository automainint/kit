#include "input_buffer.h"

#include <string.h>

typedef struct {
  ptrdiff_t       ref_count;
  kit_is_handle_t upstream;
  kit_allocator_t alloc;
  kit_string_t    data;
} internal_buffer_t;

static internal_buffer_t *buf_init(kit_is_handle_t upstream,
                                   kit_allocator_t alloc) {
  internal_buffer_t *buf;
  buf = alloc.allocate(alloc.state, sizeof *buf);
  if (buf != NULL) {
    memset(buf, 0, sizeof *buf);
    buf->ref_count = 1;
    buf->upstream  = upstream;
    buf->alloc     = alloc;
    DA_INIT(buf->data, 0, alloc);
  }
  return buf;
}

static kit_allocator_t buf_alloc(void *p) {
  return ((internal_buffer_t *) p)->alloc;
}

static void buf_acquire(void *p) {
  internal_buffer_t *buf = (internal_buffer_t *) p;
  buf->ref_count++;
}

static void buf_release(void *p) {
  internal_buffer_t *buf = (internal_buffer_t *) p;
  if (--buf->ref_count == 0) {
    DA_DESTROY(buf->data);
    buf->alloc.deallocate(buf->alloc.state, buf);
  }
}

static void buf_adjust(void *p, ptrdiff_t size) {
  internal_buffer_t *buf    = (internal_buffer_t *) p;
  ptrdiff_t          offset = buf->data.size;
  if (offset < size) {
    DA_RESIZE(buf->data, size);
    kit_out_str_t destination = {
      .size = size - offset, .values = buf->data.values + offset
    };
    ptrdiff_t n = KIT_IS_READ(buf->upstream, destination);
    DA_RESIZE(buf->data, offset + n);
  }
}

static ptrdiff_t min(ptrdiff_t a, ptrdiff_t b) {
  if (a < b)
    return a;
  return b;
}

static ptrdiff_t buf_read(void *p, ptrdiff_t offset,
                          kit_out_str_t destination) {
  internal_buffer_t *buf = (internal_buffer_t *) p;
  ptrdiff_t n = min(destination.size, buf->data.size - offset);
  memcpy(destination.values, buf->data.values + offset, n);
  return n;
}

kit_ib_handle_t kit_ib_wrap(kit_is_handle_t upstream,
                            kit_allocator_t alloc) {
  kit_ib_handle_t buf;
  memset(&buf, 0, sizeof buf);
  buf.status = KIT_OK;
  DA_INIT(buf.data, 0, alloc);
  buf.internal = buf_init(upstream, alloc);
  if (buf.internal == NULL)
    buf.status = KIT_ERROR;
  return buf;
}

kit_ib_handle_t kit_ib_read(kit_ib_handle_t buf, ptrdiff_t size) {
  kit_ib_handle_t next;
  memset(&next, 0, sizeof next);
  if (buf.status != KIT_OK) {
    next.status = buf.status;
  } else {
    buf_acquire(buf.internal);
    buf_adjust(buf.internal, buf.offset + size);
    DA_INIT(next.data, size, buf_alloc(buf.internal));
    if (next.data.size != size)
      next.status = KIT_ERROR;
    kit_out_str_t destination = { .size   = next.data.size,
                                  .values = next.data.values };
    ptrdiff_t     n = buf_read(buf.internal, buf.offset, destination);
    next.offset     = buf.offset + n;
    next.internal   = buf.internal;
    DA_RESIZE(next.data, n);
    if (next.data.size != n)
      next.status = KIT_ERROR;
  }
  return next;
}

void kit_ib_destroy(kit_ib_handle_t buf) {
  buf_release(buf.internal);
  DA_DESTROY(buf.data);
}
