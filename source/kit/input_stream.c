#include "input_stream.h"

#include <string.h>

enum input_stream_type { input_stream_cstr };

struct is_state_basic {
  ptrdiff_t            type;
  struct kit_allocator alloc;
};

struct is_state_cstr {
  ptrdiff_t            type;
  struct kit_allocator alloc;
  cstr                 string;
};

static _Bool check_type(void *state, ptrdiff_t type) {
  struct is_state_basic *basic = (struct is_state_basic *) state;
  return basic != NULL && basic->type == type;
}

static ptrdiff_t min(ptrdiff_t a, ptrdiff_t b) {
  if (a < b)
    return a;
  return b;
}

static ptrdiff_t read_cstr(void *state, out_str destination) {
  if (!check_type(state, input_stream_cstr))
    return 0;
  struct is_state_cstr *cstr = (struct is_state_cstr *) state;
  ptrdiff_t size = min(destination.size, cstr->string.size);
  memcpy(destination.values, cstr->string.values, size);
  cstr->string.values += size;
  cstr->string.size -= size;
  return size;
}

struct is_handle is_wrap_string(cstr                 string,
                                struct kit_allocator alloc) {
  struct is_handle in;
  memset(&in, 0, sizeof in);
  struct is_state_cstr *state =
      (struct is_state_cstr *) alloc.allocate(
          alloc.state, sizeof(struct is_state_cstr));
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

void is_destroy(struct is_handle in) {
  struct is_state_basic *basic = (struct is_state_basic *) in.state;
  if (basic != NULL)
    basic->alloc.deallocate(basic->alloc.state, in.state);
}
