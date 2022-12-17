#ifndef KIT_SECURE_RANDOM_H
#define KIT_SECURE_RANDOM_H

#include "status.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void kit_secure_random_init();
void kit_secure_random_cleanup();

void kit_secure_random(ptrdiff_t size, void *data);

#ifndef KIT_DISABLE_SHORT_NAMES
#  define secure_random_init kit_secure_random_init
#  define secure_random_cleanup kit_secure_random_cleanup
#  define secure_random kit_secure_random
#endif

#ifdef __cplusplus
}
#endif

#endif
