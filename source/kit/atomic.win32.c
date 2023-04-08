#include "atomic.h"

#ifdef _MSC_VER
static_assert(sizeof(char) == 1, "Wrong char size");
static_assert(sizeof(short) == 2, "Wrong short size");
static_assert(sizeof(int) == 4, "Wrong int size");

#  include <intrin.h>

void kit_atomic_store_explicit_8(uint8_t volatile *var, uint8_t value,
                                 int memory_order) {
  char volatile *dst = (char volatile *) var;
  char           src = (char) value;

  switch (memory_order) {
    case memory_order_relaxed: *dst = src; break;
    default: _InterlockedExchange8(dst, src);
  }
}

void kit_atomic_store_explicit_16(uint16_t volatile *var,
                                  uint16_t value, int memory_order) {
  short volatile *dst = (short volatile *) var;
  short           src = (short) value;

  switch (memory_order) {
    case memory_order_relaxed: *dst = src; break;
    default: _InterlockedExchange16(dst, src);
  }
}

void kit_atomic_store_explicit_32(uint32_t volatile *var,
                                  uint32_t value, int memory_order) {
  int volatile *dst = (int volatile *) var;
  int           src = (int) value;

  switch (memory_order) {
    case memory_order_relaxed: *dst = src; break;
    default: _InterlockedExchange(dst, src);
  }
}

void kit_atomic_store_explicit_64(uint64_t volatile *var,
                                  uint64_t value, int memory_order) {
  __int64 volatile *dst = (__int64 volatile *) var;
  __int64           src = (__int64) value;

  switch (memory_order) {
    case memory_order_relaxed: *dst = src; break;
    default:
#  ifdef _WIN64
      _InterlockedExchange64(dst, src);
#  else
      _InterlockedExchange((int volatile *) dst, (int) src);
#  endif
  }
}

uint8_t kit_atomic_load_explicit_8(volatile uint8_t *var,
                                   int               memory_order) {
  char volatile *dst = (char volatile *) var;

  if (memory_order == memory_order_relaxed)
    return (uint8_t) *dst;

  return (uint8_t) _InterlockedOr8(dst, 0);
}

uint16_t kit_atomic_load_explicit_16(uint16_t volatile *var,
                                     int memory_order) {
  short volatile *dst = (short volatile *) var;

  if (memory_order == memory_order_relaxed)
    return (uint16_t) *dst;

  return (uint16_t) _InterlockedOr16(dst, 0);
}

uint32_t kit_atomic_load_explicit_32(uint32_t volatile *var,
                                     int memory_order) {
  int volatile *dst = (int volatile *) var;

  if (memory_order == memory_order_relaxed)
    return (uint32_t) *dst;

  return (uint32_t) _InterlockedOr(dst, 0);
}

uint64_t kit_atomic_load_explicit_64(uint64_t volatile *var,
                                     int memory_order) {
  __int64 volatile *dst = (__int64 volatile *) var;

  if (memory_order == memory_order_relaxed)
    return (uint64_t) *dst;

#  ifdef _WIN64
  return (uint64_t) _InterlockedOr64(dst, 0);
#  else
  return (uint64_t) _InterlockedOr((int volatile *) dst, 0);
#  endif
}

uint8_t kit_atomic_exchange_explicit_8(volatile uint8_t *var,
                                       uint8_t           value,
                                       int memory_order) {
  char volatile *dst = (char volatile *) var;
  char           src = (char) value;

  return (uint8_t) _InterlockedExchange8(dst, src);
}

uint16_t kit_atomic_exchange_explicit_16(uint16_t volatile *var,
                                         uint16_t           value,
                                         int memory_order) {
  short volatile *dst = (short volatile *) var;
  short           src = (short) value;

  return (uint16_t) _InterlockedExchange16(dst, src);
}

uint32_t kit_atomic_exchange_explicit_32(uint32_t volatile *var,
                                         uint32_t           value,
                                         int memory_order) {
  int volatile *dst = (int volatile *) var;
  int           src = (int) value;

  return (uint32_t) _InterlockedExchange(dst, src);
}

uint64_t kit_atomic_exchange_explicit_64(uint64_t volatile *var,
                                         uint64_t           value,
                                         int memory_order) {
  __int64 volatile *dst = (__int64 volatile *) var;
  __int64           src = (__int64) value;

#  ifdef _WIN64
  return (uint64_t) _InterlockedExchange64(dst, src);
#  else
  return (uint64_t) _InterlockedExchange((int volatile *) dst,
                                         (int) src);
#  endif
}

uint8_t kit_atomic_fetch_add_explicit_8(volatile uint8_t *var,
                                        uint8_t           value,
                                        int memory_order) {
  char volatile *dst = (char volatile *) var;
  char           src = (char) value;

  return (uint8_t) _InterlockedExchangeAdd8(dst, src);
}

uint16_t kit_atomic_fetch_add_explicit_16(uint16_t volatile *var,
                                          uint16_t           value,
                                          int memory_order) {
  short volatile *dst = (short volatile *) var;
  short           src = (short) value;

  return (uint16_t) _InterlockedExchangeAdd16(dst, src);
}

uint32_t kit_atomic_fetch_add_explicit_32(uint32_t volatile *var,
                                          uint32_t           value,
                                          int memory_order) {
  int volatile *dst = (int volatile *) var;
  int           src = (int) value;

  return (uint32_t) _InterlockedExchangeAdd(dst, src);
}

uint64_t kit_atomic_fetch_add_explicit_64(uint64_t volatile *var,
                                          uint64_t           value,
                                          int memory_order) {
  __int64 volatile *dst = (__int64 volatile *) var;
  __int64           src = (__int64) value;

#  ifdef _WIN64
  return (uint64_t) _InterlockedExchangeAdd64(dst, src);
#  else
  return (uint64_t) _InterlockedExchangeAdd((int volatile *) dst,
                                            (int) src);
#  endif
}

#endif
