#ifndef KIT_SHA256_H
#define KIT_SHA256_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum { KIT_SHA256_BLOCK_SIZE = 32 };

typedef struct {
  uint8_t v[KIT_SHA256_BLOCK_SIZE];
} kit_sha256_hash_t;

kit_sha256_hash_t kit_sha256(ptrdiff_t size, uint8_t const *data);

#ifdef __cplusplus
}
#endif

#ifndef KIT_DISABLE_SHORT_NAMES
#  define SHA256_BLOCK_SIZE KIT_SHA256_BLOCK_SIZE
#  define sha256_hash_t kit_sha256_hash_t
#  define sha256 kit_sha256
#endif

#endif
