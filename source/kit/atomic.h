#ifndef KIT_ATOMIC_H
#define KIT_ATOMIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifndef _MSC_VER
#  include <stdatomic.h>

#  define KIT_ATOMIC(type_) _Atomic type_
#else
enum {
  memory_order_relaxed,
  memory_order_consume,
  memory_order_acquire,
  memory_order_release,
  memory_order_acq_rel,
  memory_order_seq_cst
};
#  ifdef _WIN64
#    define KIT_ATOMIC_VAR int64_t
#  else
#    define KIT_ATOMIC_VAR int32_t
#  endif
#  define KIT_ATOMIC(type_) volatile KIT_ATOMIC_VAR

void kit_atomic_store_explicit(volatile KIT_ATOMIC_VAR *var,
                               KIT_ATOMIC_VAR           value,
                               int                      memory_order);

KIT_ATOMIC_VAR kit_atomic_load_explicit(volatile KIT_ATOMIC_VAR *var,
                                        int memory_order);

KIT_ATOMIC_VAR kit_atomic_fetch_add_explicit(
    volatile KIT_ATOMIC_VAR *var, KIT_ATOMIC_VAR value,
    int memory_order);

KIT_ATOMIC_VAR kit_atomic_exchange_explicit(
    volatile KIT_ATOMIC_VAR *var, KIT_ATOMIC_VAR value,
    int memory_order);

#  define atomic_store_explicit kit_atomic_store_explicit
#  define atomic_load_explicit kit_atomic_load_explicit
#  define atomic_fetch_add_explicit kit_atomic_fetch_add_explicit
#  define atomic_exchange_explicit kit_atomic_exchange_explicit
#endif

#ifndef KIT_DISABLE_SHORT_NAMES
#  define ATOMIC KIT_ATOMIC
#endif

#ifdef __cplusplus
}
#endif

#endif
