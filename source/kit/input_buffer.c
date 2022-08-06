#include "input_buffer.h"

#include <string.h>

struct internal_buffer {
  ptrdiff_t            ref_count;
  struct kit_is_handle upstream;
  struct kit_allocator alloc;
  DA(data, char);
};

static struct internal_buffer *buf_init(struct kit_is_handle upstream,
                                        struct kit_allocator alloc) {
  struct internal_buffer *buf;
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

static struct kit_allocator buf_alloc(void *p) {
  return ((struct internal_buffer *) p)->alloc;
}

static void buf_acquire(void *p) {
  struct internal_buffer *buf = (struct internal_buffer *) p;
  buf->ref_count++;
}

static void buf_release(void *p) {
  struct internal_buffer *buf = (struct internal_buffer *) p;
  if (--buf->ref_count == 0) {
    DA_DESTROY(buf->data);
    buf->alloc.deallocate(buf->alloc.state, buf);
  }
}

static void buf_adjust(void *p, ptrdiff_t size) {
  struct internal_buffer *buf    = (struct internal_buffer *) p;
  ptrdiff_t               offset = buf->data.size;
  if (offset < size) {
    DA_RESIZE(buf->data, size);
    kit_out_str destination = { .size   = size - offset,
                                .values = buf->data.values + offset };
    ptrdiff_t   n           = KIT_IS_READ(buf->upstream, destination);
    DA_RESIZE(buf->data, offset + n);
  }
}

static ptrdiff_t min(ptrdiff_t a, ptrdiff_t b) {
  if (a < b)
    return a;
  return b;
}

static ptrdiff_t buf_read(void *p, ptrdiff_t offset,
                          kit_out_str destination) {
  struct internal_buffer *buf = (struct internal_buffer *) p;
  ptrdiff_t n = min(destination.size, buf->data.size - offset);
  memcpy(destination.values, buf->data.values + offset, n);
  return n;
}

struct kit_ib_handle kit_ib_wrap(struct kit_is_handle upstream,
                                 struct kit_allocator alloc) {
  struct kit_ib_handle buf;
  memset(&buf, 0, sizeof buf);
  buf.error = 0;
  DA_INIT(buf.data, 0, alloc);
  buf.internal = buf_init(upstream, alloc);
  if (buf.internal == NULL)
    buf.error = 1;
  return buf;
}

struct kit_ib_handle kit_ib_read(struct kit_ib_handle buf,
                                 ptrdiff_t            size) {
  struct kit_ib_handle next;
  memset(&next, 0, sizeof next);
  if (buf.error) {
    next.error = 1;
  } else {
    buf_acquire(buf.internal);
    buf_adjust(buf.internal, buf.offset + size);
    DA_INIT(next.data, size, buf_alloc(buf.internal));
    if (next.data.size != size)
      next.error = 1;
    kit_out_str destination = { .size   = next.data.size,
                                .values = next.data.values };
    ptrdiff_t   n = buf_read(buf.internal, buf.offset, destination);
    next.offset   = buf.offset + n;
    next.internal = buf.internal;
    DA_RESIZE(next.data, n);
    if (next.data.size != n)
      next.error = 1;
  }
  return next;
}

void kit_ib_destroy(struct kit_ib_handle buf) {
  buf_release(buf.internal);
  DA_DESTROY(buf.data);
}
