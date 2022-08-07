#include "input_stream.h"

#include <string.h>

enum { input_stream_str };

typedef struct {
  ptrdiff_t       type;
  kit_allocator_t alloc;
} kit_is_state_basic_t;

typedef struct {
  ptrdiff_t       type;
  kit_allocator_t alloc;
  kit_str_t       string;
} kit_is_state_str_t;

static int check_type(void *state, ptrdiff_t type) {
  kit_is_state_basic_t *basic = (kit_is_state_basic_t *) state;
  return basic != NULL && basic->type == type;
}

static ptrdiff_t min(ptrdiff_t a, ptrdiff_t b) {
  if (a < b)
    return a;
  return b;
}

static ptrdiff_t read_str(void *state, kit_out_str_t destination) {
  if (!check_type(state, input_stream_str))
    return 0;
  kit_is_state_str_t *str  = (kit_is_state_str_t *) state;
  ptrdiff_t           size = min(destination.size, str->string.size);
  memcpy(destination.values, str->string.values, size);
  str->string.values += size;
  str->string.size -= size;
  return size;
}

kit_is_handle_t kit_is_wrap_string(kit_str_t       string,
                                   kit_allocator_t alloc) {
  kit_is_handle_t in;
  memset(&in, 0, sizeof in);
  kit_is_state_str_t *state = (kit_is_state_str_t *) alloc.allocate(
      alloc.state, sizeof(kit_is_state_str_t));
  if (state != NULL) {
    memset(state, 0, sizeof *state);
    state->type   = input_stream_str;
    state->string = string;
    state->alloc  = alloc;
    in.state      = state;
    in.read       = read_str;
  }
  return in;
}

void kit_is_destroy(kit_is_handle_t in) {
  kit_is_state_basic_t *basic = (kit_is_state_basic_t *) in.state;
  if (basic != NULL)
    basic->alloc.deallocate(basic->alloc.state, in.state);
}
