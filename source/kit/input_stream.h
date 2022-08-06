#ifndef KIT_INPUT_STREAM_H
#define KIT_INPUT_STREAM_H

#include "allocator.h"
#include "string_ref.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef ptrdiff_t (*kit_is_read_fn)(void       *state,
                                    kit_out_str destination);

struct kit_is_handle {
  void          *state;
  kit_is_read_fn read;
};

struct kit_is_handle kit_is_wrap_string(kit_cstr             string,
                                        struct kit_allocator alloc);

void kit_is_destroy(struct kit_is_handle in);

#define KIT_IS_WRAP_STRING(string) \
  kit_is_wrap_string((string), kit_alloc_default())

#define KIT_IS_READ(in, destination) \
  (in).read((in).state, (destination))

#ifndef KIT_DISABLE_SHORT_NAMES
#  define is_read_fn kit_is_read_fn
#  define is_handle kit_is_handle
#  define is_wrap_string kit_is_wrap_string
#  define is_destroy kit_is_destroy

#  define IS_WRAP_STRING KIT_IS_WRAP_STRING
#  define IS_READ KIT_IS_READ
#endif

#ifdef __cplusplus
}
#endif

#endif
