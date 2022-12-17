#ifndef KIT_MERSENNE_TWISTER_64_H
#define KIT_MERSENNE_TWISTER_64_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
  KIT_MT64_N = 312,
};

typedef struct {
  uint64_t mt[KIT_MT64_N];
  uint64_t index;
} kit_mt64_state_t;

void kit_mt64_init_array(kit_mt64_state_t *state, ptrdiff_t size,
                         uint64_t const *seed);

void kit_mt64_init(kit_mt64_state_t *state, uint64_t seed);

uint64_t kit_mt64_generate(kit_mt64_state_t *state);

#ifndef KIT_DISABLE_SHORT_NAMES
#  define mt64_state_t kit_mt64_state_t
#  define mt64_init_array kit_mt64_init_array
#  define mt64_init kit_mt64_init
#  define mt64_generate kit_mt64_generate
#  define mt64_seed kit_mt64_seed
#endif

#ifdef __cplusplus
}
#endif

#endif
