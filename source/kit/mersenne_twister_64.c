#include "mersenne_twister_64.h"

#include "secure_random.h"

#define MM 156
#define MATRIX_A 0xb5026f5aa96619e9ull
#define UM 0xffffffff80000000ull
#define LM 0x7fffffffull

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
  static uint64_t const mag01[2] = { 0ull, MATRIX_A };

  int      i;
  uint64_t x;

  if (state->index >= KIT_MT64_N) {
    for (i = 0; i < KIT_MT64_N - MM; i++) {
      x            = (state->mt[i] & UM) | (state->mt[i + 1] & LM);
      state->mt[i] = state->mt[i + MM] ^ (x >> 1u) ^
                     mag01[(int) (x & 1ull)];
    }

    for (; i < KIT_MT64_N - 1; i++) {
      x            = (state->mt[i] & UM) | (state->mt[i + 1] & LM);
      state->mt[i] = state->mt[i + (MM - KIT_MT64_N)] ^ (x >> 1u) ^
                     mag01[(int) (x & 1ull)];
    }

    x = (state->mt[KIT_MT64_N - 1] & UM) | (state->mt[0] & LM);
    state->mt[KIT_MT64_N - 1] = state->mt[MM - 1] ^ (x >> 1u) ^
                                mag01[(int) (x & 1ull)];

    state->index = 0;
  }

  x = state->mt[state->index++];

  x ^= (x >> 29u) & 0x5555555555555555ull;
  x ^= (x << 17u) & 0x71d67fffeda60000ull;
  x ^= (x << 37u) & 0xfff7eee000000000ull;
  x ^= (x >> 43u);

  return x;
}

uint64_t kit_mt64_seed() {
  uint64_t seed;
  kit_secure_random(sizeof seed, &seed);
  return seed;
}
