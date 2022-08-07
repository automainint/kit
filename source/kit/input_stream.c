#include "input_stream.h"

#include <string.h>

enum { input_stream_cstr };

typedef struct {
  ptrdiff_t       type;
  kit_allocator_t alloc;
} kit_is_state_basic_t;

typedef struct {
  ptrdiff_t       type;
  kit_allocator_t alloc;
  kit_cstr_t      string;
} kit_is_state_cstr_t;

static _Bool check_type(void *state, ptrdiff_t type) {
  kit_is_state_basic_t *basic = (kit_is_state_basic_t *) state;
  return basic != NULL && basic->type == type;
}

static ptrdiff_t min(ptrdiff_t a, ptrdiff_t b) {
  if (a < b)
    return a;
  return b;
}

static ptrdiff_t read_cstr(void *state, kit_out_str_t destination) {
  if (!check_type(state, input_stream_cstr))
    return 0;
  kit_is_state_cstr_t *cstr = (kit_is_state_cstr_t *) state;
  ptrdiff_t size = min(destination.size, cstr->string.size);
  memcpy(destination.values, cstr->string.values, size);
  cstr->string.values += size;
  cstr->string.size -= size;
  return size;
}

kit_is_handle_t kit_is_wrap_string(kit_cstr_t      string,
                                   kit_allocator_t alloc) {
  kit_is_handle_t in;
  memset(&in, 0, sizeof in);
  kit_is_state_cstr_t *state = (kit_is_state_cstr_t *) alloc.allocate(
      alloc.state, sizeof(kit_is_state_cstr_t));
  if (state != NULL) {
    memset(state, 0, sizeof *state);
    state->type   = input_stream_cstr;
    state->string = string;
    state->alloc  = alloc;
    in.state      = state;
    in.read       = read_cstr;
  }
  return in;
}

void kit_is_destroy(kit_is_handle_t in) {
  kit_is_state_basic_t *basic = (kit_is_state_basic_t *) in.state;
  if (basic != NULL)
    basic->alloc.deallocate(basic->alloc.state, in.state);
}
