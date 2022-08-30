#ifndef KIT_ATOMIC_H
#define KIT_ATOMIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifndef _MSC_VER
#  include <stdatomic.h>
#  define KIT_ATOMIC(type_) type_ _Atomic
#else
#  define KIT_ATOMIC(type_) type_ volatile

enum {
  memory_order_relaxed,
  memory_order_consume,
  memory_order_acquire,
  memory_order_release,
  memory_order_acq_rel,
  memory_order_seq_cst
};

void kit_atomic_store_explicit_8(uint8_t volatile *var, uint8_t value,
                                 int memory_order);

void kit_atomic_store_explicit_16(uint16_t volatile *var,
                                  uint16_t value, int memory_order);

void kit_atomic_store_explicit_32(uint32_t volatile *var,
                                  uint32_t value, int memory_order);

void kit_atomic_store_explicit_64(uint64_t volatile *var,
                                  uint64_t value, int memory_order);

uint8_t kit_atomic_load_explicit_8(uint8_t volatile *var,
                                   int               memory_order);

uint16_t kit_atomic_load_explicit_16(uint16_t volatile *var,
                                     int                memory_order);

uint32_t kit_atomic_load_explicit_32(uint32_t volatile *var,
                                     int                memory_order);

uint64_t kit_atomic_load_explicit_64(uint64_t volatile *var,
                                     int                memory_order);

uint8_t kit_atomic_exchange_explicit_8(uint8_t volatile *var,
                                       uint8_t           value,
                                       int memory_order);

uint16_t kit_atomic_exchange_explicit_16(uint16_t volatile *var,
                                         uint16_t           value,
                                         int memory_order);

uint32_t kit_atomic_exchange_explicit_32(uint32_t volatile *var,
                                         uint32_t           value,
                                         int memory_order);

uint64_t kit_atomic_exchange_explicit_64(uint64_t volatile *var,
                                         uint64_t           value,
                                         int memory_order);

uint8_t kit_atomic_fetch_add_explicit_8(uint8_t volatile *var,
                                        uint8_t           value,
                                        int memory_order);

uint16_t kit_atomic_fetch_add_explicit_16(uint16_t volatile *var,
                                          uint16_t           value,
                                          int memory_order);

uint32_t kit_atomic_fetch_add_explicit_32(uint32_t volatile *var,
                                          uint32_t           value,
                                          int memory_order);

uint64_t kit_atomic_fetch_add_explicit_64(uint64_t volatile *var,
                                          uint64_t           value,
                                          int memory_order);

#  define atomic_store_explicit(var_, value_, memory_order_)        \
    do {                                                            \
      static_assert(sizeof *(var_) == 1 || sizeof *(var_) == 2 ||   \
                        sizeof *(var_) == 3 || sizeof *(var_) == 4, \
                    "Wrong atomic variable type");                  \
      static_assert(sizeof(value_) <= sizeof *(var_),               \
                    "Wrong value type");                            \
      if (sizeof *(var_) == 1)                                      \
        kit_atomic_store_explicit_8((uint8_t volatile *) (var_),    \
                                    (uint8_t) (value_),             \
                                    (memory_order_));               \
      if (sizeof *(var_) == 2)                                      \
        kit_atomic_store_explicit_16((uint16_t volatile *) (var_),  \
                                     (uint16_t) (value_),           \
                                     (memory_order_));              \
      if (sizeof *(var_) == 4)                                      \
        kit_atomic_store_explicit_32((uint32_t volatile *) (var_),  \
                                     (uint32_t) (value_),           \
                                     (memory_order_));              \
      if (sizeof *(var_) == 8)                                      \
        kit_atomic_store_explicit_64((uint64_t volatile *) (var_),  \
                                     (uint64_t) (value_),           \
                                     (memory_order_));              \
    } while (0)

#  define atomic_load_explicit(var_, memory_order_)                 \
    (static_assert(sizeof *(var_) == 1 || sizeof *(var_) == 2 ||    \
                       sizeof *(var_) == 3 || sizeof *(var_) == 4,  \
                   "Wrong atomic variable type"),                   \
     (sizeof *(var_) == 1                                           \
          ? kit_atomic_load_explicit_8((uint8_t volatile *) (var_), \
                                       (value_), (memory_order_))   \
      : sizeof *(var_) == 2 ? kit_atomic_load_explicit_16(          \
                                  (uint16_t volatile *) (var_),     \
                                  (value_), (memory_order_))        \
      : sizeof *(var_) == 4 ? kit_atomic_load_explicit_32(          \
                                  (uint32_t volatile *) (var_),     \
                                  (value_), (memory_order_))        \
                            : kit_atomic_load_explicit_64(          \
                                  (uint64_t volatile *) (var_),     \
                                  (value_), (memory_order_))))

#  define atomic_exchange_explicit(var_, value_, memory_order_)      \
    (static_assert(sizeof *(var_) == 1 || sizeof *(var_) == 2 ||     \
                       sizeof *(var_) == 3 || sizeof *(var_) == 4,   \
                   "Wrong atomic variable type"),                    \
     static_assert(sizeof(value_) <= sizeof *(var_),                 \
                   "Wrong value type"),                              \
     (sizeof *(var_) == 1 ? kit_atomic_exchange_explicit_8(          \
                                (uint8_t volatile *) (var_),         \
                                (uint8_t) (value_), (memory_order_)) \
      : sizeof *(var_) == 2                                          \
          ? kit_atomic_exchange_explicit_16(                         \
                (uint16_t volatile *) (var_), (uint16_t) (value_),   \
                (memory_order_))                                     \
      : sizeof *(var_) == 4                                          \
          ? kit_atomic_exchange_explicit_32(                         \
                (uint32_t volatile *) (var_), (uint32_t) (value_),   \
                (memory_order_))                                     \
          : kit_atomic_exchange_explicit_64(                         \
                (uint64_t volatile *) (var_), (uint64_t) (value_),   \
                (memory_order_))))

#  define atomic_fetch_add_explicit(var_, value_, memory_order_)     \
    (static_assert(sizeof *(var_) == 1 || sizeof *(var_) == 2 ||     \
                       sizeof *(var_) == 3 || sizeof *(var_) == 4,   \
                   "Wrong atomic variable type"),                    \
     static_assert(sizeof(value_) <= sizeof *(var_),                 \
                   "Wrong value type"),                              \
     (sizeof *(var_) == 1 ? kit_atomic_fetch_add_explicit_8(         \
                                (uint8_t volatile *) (var_),         \
                                (uint8_t) (value_), (memory_order_)) \
      : sizeof *(var_) == 2                                          \
          ? kit_atomic_fetch_add_explicit_16(                        \
                (uint16_t volatile *) (var_), (uint16_t) (value_),   \
                (memory_order_))                                     \
      : sizeof *(var_) == 4                                          \
          ? kit_atomic_fetch_add_explicit_32(                        \
                (uint32_t volatile *) (var_), (uint32_t) (value_),   \
                (memory_order_))                                     \
          : kit_atomic_fetch_add_explicit_64(                        \
                (uint64_t volatile *) (var_), (uint64_t) (value_),   \
                (memory_order_))))
#endif

#ifndef KIT_DISABLE_SHORT_NAMES
#  define ATOMIC KIT_ATOMIC
#endif

#ifdef __cplusplus
}
#endif

#endif
