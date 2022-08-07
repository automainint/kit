#ifndef KIT_INPUT_STREAM_H
#define KIT_INPUT_STREAM_H

#include "allocator.h"
#include "string_ref.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef ptrdiff_t (*kit_is_read_fn)(void         *state,
                                    kit_out_str_t destination);

typedef struct {
  void          *state;
  kit_is_read_fn read;
} kit_is_handle_t;

kit_is_handle_t kit_is_wrap_string(kit_str_t       string,
                                   kit_allocator_t alloc);

void kit_is_destroy(kit_is_handle_t in);

#define KIT_IS_WRAP_STRING(string) \
  kit_is_wrap_string((string), kit_alloc_default())

#define KIT_IS_READ(in, destination) \
  (in).read((in).state, (destination))

#ifndef KIT_DISABLE_SHORT_NAMES
#  define is_read_fn kit_is_read_fn
#  define is_handle_t kit_is_handle_t
#  define is_wrap_string kit_is_wrap_string
#  define is_destroy kit_is_destroy

#  define IS_WRAP_STRING KIT_IS_WRAP_STRING
#  define IS_READ KIT_IS_READ
#endif

#ifdef __cplusplus
}
#endif

#endif
