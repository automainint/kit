#include "../../kit/atomic.h"

#define KIT_TEST_FILE atomic
#include "../../kit_test/test.h"

TEST("atomic store and load") {
  ATOMIC(int) value;
  atomic_store_explicit(&value, 20, memory_order_relaxed);
  REQUIRE(atomic_load_explicit(&value, memory_order_relaxed) == 20);
}

TEST("atomic exchange") {
  ATOMIC(int) value;
  atomic_store_explicit(&value, 20, memory_order_relaxed);
  REQUIRE(atomic_exchange_explicit(&value, 42,
                                   memory_order_relaxed) == 20);
  REQUIRE(atomic_load_explicit(&value, memory_order_relaxed) == 42);
}

TEST("atomic fetch add") {
  ATOMIC(int) value;
  atomic_store_explicit(&value, 20, memory_order_relaxed);
  REQUIRE(atomic_fetch_add_explicit(&value, 22,
                                    memory_order_relaxed) == 20);
  REQUIRE(atomic_load_explicit(&value, memory_order_relaxed) == 42);
}
