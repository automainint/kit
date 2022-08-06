#include "input_stream.h"

#include <string.h>

enum input_stream_type { input_stream_cstr };

struct kit_is_state_basic {
  ptrdiff_t            type;
  struct kit_allocator alloc;
};

struct kit_is_state_cstr {
  ptrdiff_t            type;
  struct kit_allocator alloc;
  kit_cstr             string;
};

static _Bool check_type(void *state, ptrdiff_t type) {
  struct kit_is_state_basic *basic = (struct kit_is_state_basic *)
      state;
  return basic != NULL && basic->type == type;
}

static ptrdiff_t min(ptrdiff_t a, ptrdiff_t b) {
  if (a < b)
    return a;
  return b;
}

static ptrdiff_t read_cstr(void *state, kit_out_str destination) {
  if (!check_type(state, input_stream_cstr))
    return 0;
  struct kit_is_state_cstr *cstr = (struct kit_is_state_cstr *) state;
  ptrdiff_t size = min(destination.size, cstr->string.size);
  memcpy(destination.values, cstr->string.values, size);
  cstr->string.values += size;
  cstr->string.size -= size;
  return size;
}

struct kit_is_handle kit_is_wrap_string(kit_cstr             string,
                                        struct kit_allocator alloc) {
  struct kit_is_handle in;
  memset(&in, 0, sizeof in);
  struct kit_is_state_cstr *state =
      (struct kit_is_state_cstr *) alloc.allocate(
          alloc.state, sizeof(struct kit_is_state_cstr));
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

void kit_is_destroy(struct kit_is_handle in) {
  struct kit_is_state_basic *basic = (struct kit_is_state_basic *)
                                         in.state;
  if (basic != NULL)
    basic->alloc.deallocate(basic->alloc.state, in.state);
}
