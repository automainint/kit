#include "atomic.h"

#ifdef _MSC_VEC
#  include <intrin.h>

#  ifdef _WIN64
#    pragma intrinsic(_InterlockedExchange64)
#    pragma intrinsic(_InterlockedExchangeAdd64)

#    define InterlockedExchange_ _InterlockedExchange64
#    define InterlockedExchangeAdd_ _InterlockedExchangeAdd64
#  else
#    pragma intrinsic(_InterlockedExchange32)
#    pragma intrinsic(_InterlockedExchangeAdd32)

#    define InterlockedExchange_ _InterlockedExchange32
#    define InterlockedExchangeAdd_ _InterlockedExchangeAdd32
#  endif

void kit_atomic_store_explicit(volatile KIT_ATOMIC_VAR *var,
                               KIT_ATOMIC_VAR           value,
                               int memory_order) {
  InterlockedExchange_(var, value);
}

KIT_ATOMIC_VAR kit_atomic_load_explicit(volatile KIT_ATOMIC_VAR *var,
                                        int memory_order) {
  return *var;
}

KIT_ATOMIC_VAR kit_atomic_fetch_add_explicit(
    volatile KIT_ATOMIC_VAR *var, KIT_ATOMIC_VAR value,
    int memory_order) {
  return InterlockedExchangeAdd_(var, value);
}

KIT_ATOMIC_VAR kit_atomic_exchange_explicit(
    volatile KIT_ATOMIC_VAR *var, KIT_ATOMIC_VAR value,
    int memory_order) {
  return InterlockedExchange_(var, value);
}
#endif
