#ifndef KIT_INPUT_STREAM_H
#define KIT_INPUT_STREAM_H

#include "allocator.h"
#include "string_ref.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef ptrdiff_t (*is_read_fn)(void *state, out_str destination);

struct is_handle {
  void      *state;
  is_read_fn read;
};

struct is_handle is_wrap_string(cstr                 string,
                                struct kit_allocator alloc);

void is_destroy(struct is_handle in);

#define IS_WRAP_STRING(string) \
  is_wrap_string((string), kit_alloc_default())

#define IS_READ(in, destination) (in).read((in).state, (destination))

#ifdef __cplusplus
}
#endif

#endif
