#include "mersenne_twister_64.h"

#include "time.h"

void kit_mt64_init_array(kit_mt64_state_t *const state,
                         ptrdiff_t const         size,
                         uint64_t const *const   seed) {
  for (ptrdiff_t i = 0; i < size && i < KIT_MT64_N; i++)
    state->mt[i] = seed[i];
  for (state->index = size; state->index < KIT_MT64_N; state->index++)
    state->mt[state->index] = (6364136223846793005ull *
                                   (state->mt[state->index - 1] ^
                                    (state->mt[state->index - 1] >>
                                     62u)) +
                               state->index);
}

void kit_mt64_init(kit_mt64_state_t *const state,
                   uint64_t const          seed) {
  kit_mt64_init_array(state, 1, &seed);
}

uint64_t kit_mt64_generate(kit_mt64_state_t *const state) {
  static uint64_t const mag01[2] = { 0ull, 0xB5026F5AA96619E9ull };

  int      i;
  uint64_t x;

  if (state->index >= KIT_MT64_N) {
    for (i = 0; i < KIT_MT64_N - 156; i++) {
      x = (state->mt[i] & 0xFFFFFFFF80000000ull) |
          (state->mt[i + 1] & 0x7FFFFFFFull);
      state->mt[i] = state->mt[i + 156] ^ (x >> 1u) ^
                     mag01[(int) (x & 1ull)];
    }

    for (; i < KIT_MT64_N - 1; i++) {
      x = (state->mt[i] & 0xFFFFFFFF80000000ull) |
          (state->mt[i + 1] & 0x7FFFFFFFull);
      state->mt[i] = state->mt[i + (156 - KIT_MT64_N)] ^ (x >> 1u) ^
                     mag01[(int) (x & 1ull)];
    }

    x = (state->mt[KIT_MT64_N - 1] & 0xFFFFFFFF80000000ull) |
        (state->mt[0] & 0x7FFFFFFFull);
    state->mt[KIT_MT64_N - 1] = state->mt[156 - 1] ^ (x >> 1u) ^
                                mag01[(int) (x & 1ull)];

    state->index = 0;
  }

  x = state->mt[state->index++];

  x ^= (x >> 29u) & 0x5555555555555555ull;
  x ^= (x << 17u) & 0x71D67FFFEDA60000ull;
  x ^= (x << 37u) & 0xFFF7EEE000000000ull;
  x ^= (x >> 43u);

  return x;
}

uint64_t kit_mt64_seed() {
  struct timespec t;
  timespec_get(&t, TIME_UTC);

  kit_mt64_state_t s;
  kit_mt64_init(&s, (uint64_t) t.tv_nsec);

  return kit_mt64_generate(&s);
}
