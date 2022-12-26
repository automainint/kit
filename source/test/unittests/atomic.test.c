#include "../../kit/atomic.h"
#include "../../kit/thread.h"

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

enum { THREAD_COUNT = 20, TICK_COUNT = 10000 };

static int test_8_(void *p) {
  ATOMIC(int8_t) *x = (ATOMIC(int8_t) *) p;

  for (ptrdiff_t i = 0; i < TICK_COUNT; i++) {
    atomic_fetch_add_explicit(x, 20, memory_order_relaxed);
    thrd_yield();
    atomic_fetch_add_explicit(x, 22, memory_order_relaxed);
    thrd_yield();
    atomic_fetch_add_explicit(x, -42, memory_order_relaxed);
    thrd_yield();
  }
}

TEST("atomic types") {
  ATOMIC(int8_t) b_1;
  ATOMIC(int8_t) b_2;
  ATOMIC(int16_t) i16;
  ATOMIC(int32_t) i32;
  ATOMIC(int64_t) i64;

  atomic_store_explicit(&b_1, 42, memory_order_relaxed);
  atomic_store_explicit(&b_2, 43, memory_order_relaxed);
  atomic_store_explicit(&i16, 4242, memory_order_relaxed);
  atomic_store_explicit(&i32, 42424242, memory_order_relaxed);
  atomic_store_explicit(&i64, 4242424242424242ll,
                        memory_order_relaxed);

  atomic_fetch_add_explicit(&b_1, -20, memory_order_relaxed);
  atomic_fetch_add_explicit(&b_2, -20, memory_order_relaxed);
  atomic_fetch_add_explicit(&i16, -2020, memory_order_relaxed);
  atomic_fetch_add_explicit(&i32, -20202020, memory_order_relaxed);
  atomic_fetch_add_explicit(&i64, -2020202020202020ll,
                            memory_order_relaxed);

  REQUIRE(atomic_exchange_explicit(&b_1, 0, memory_order_relaxed) ==
          22);
  REQUIRE(atomic_exchange_explicit(&b_2, 0, memory_order_relaxed) ==
          23);
  REQUIRE(atomic_exchange_explicit(&i16, 0, memory_order_relaxed) ==
          2222);
  REQUIRE(atomic_exchange_explicit(&i32, 0, memory_order_relaxed) ==
          22222222);
  REQUIRE(atomic_exchange_explicit(&i64, 0, memory_order_relaxed) ==
          2222222222222222ll);

  REQUIRE(atomic_load_explicit(&b_1, memory_order_relaxed) == 0);
  REQUIRE(atomic_load_explicit(&b_2, memory_order_relaxed) == 0);
  REQUIRE(atomic_load_explicit(&i16, memory_order_relaxed) == 0);
  REQUIRE(atomic_load_explicit(&i32, memory_order_relaxed) == 0);
  REQUIRE(atomic_load_explicit(&i64, memory_order_relaxed) == 0ll);
}

TEST("atomic byte concurrency") {
  ATOMIC(int8_t) foo;
  ATOMIC(int8_t) bar;

  atomic_store_explicit(&foo, 42, memory_order_relaxed);
  atomic_store_explicit(&bar, 43, memory_order_relaxed);

  thrd_t threads[THREAD_COUNT];
  for (ptrdiff_t i = 0; i < THREAD_COUNT; i++)
    thrd_create(threads + i, test_8_,
                (void *) ((i % 2) ? &foo : &bar));
  for (ptrdiff_t i = 0; i < THREAD_COUNT; i++)
    thrd_join(threads[i], NULL);

  REQUIRE(atomic_load_explicit(&foo, memory_order_relaxed) == 42);
  REQUIRE(atomic_load_explicit(&bar, memory_order_relaxed) == 43);
}

static int test_16_(void *p) {
  ATOMIC(int16_t) *x = (ATOMIC(int16_t) *) p;

  for (ptrdiff_t i = 0; i < TICK_COUNT; i++) {
    atomic_fetch_add_explicit(x, 2020, memory_order_relaxed);
    thrd_yield();
    atomic_fetch_add_explicit(x, 2222, memory_order_relaxed);
    thrd_yield();
    atomic_fetch_add_explicit(x, -4242, memory_order_relaxed);
    thrd_yield();
  }
}

TEST("atomic int16 concurrency") {
  ATOMIC(int16_t) foo;
  ATOMIC(int16_t) bar;

  atomic_store_explicit(&foo, 42, memory_order_relaxed);
  atomic_store_explicit(&bar, 43, memory_order_relaxed);

  thrd_t threads[THREAD_COUNT];
  for (ptrdiff_t i = 0; i < THREAD_COUNT; i++)
    thrd_create(threads + i, test_16_,
                (void *) ((i % 2) ? &foo : &bar));
  for (ptrdiff_t i = 0; i < THREAD_COUNT; i++)
    thrd_join(threads[i], NULL);

  REQUIRE(atomic_load_explicit(&foo, memory_order_relaxed) == 42);
  REQUIRE(atomic_load_explicit(&bar, memory_order_relaxed) == 43);
}

static int test_32_(void *p) {
  ATOMIC(int32_t) *x = (ATOMIC(int32_t) *) p;

  for (ptrdiff_t i = 0; i < TICK_COUNT; i++) {
    atomic_fetch_add_explicit(x, 202020, memory_order_relaxed);
    thrd_yield();
    atomic_fetch_add_explicit(x, 222222, memory_order_relaxed);
    thrd_yield();
    atomic_fetch_add_explicit(x, -424242, memory_order_relaxed);
    thrd_yield();
  }
}

TEST("atomic int32 concurrency") {
  ATOMIC(int32_t) foo;
  ATOMIC(int32_t) bar;

  atomic_store_explicit(&foo, 42, memory_order_relaxed);
  atomic_store_explicit(&bar, 43, memory_order_relaxed);

  thrd_t threads[THREAD_COUNT];
  for (ptrdiff_t i = 0; i < THREAD_COUNT; i++)
    thrd_create(threads + i, test_32_,
                (void *) ((i % 2) ? &foo : &bar));
  for (ptrdiff_t i = 0; i < THREAD_COUNT; i++)
    thrd_join(threads[i], NULL);

  REQUIRE(atomic_load_explicit(&foo, memory_order_relaxed) == 42);
  REQUIRE(atomic_load_explicit(&bar, memory_order_relaxed) == 43);
}

static int test_64_(void *p) {
  ATOMIC(int64_t) *x = (ATOMIC(int64_t) *) p;

  for (ptrdiff_t i = 0; i < TICK_COUNT; i++) {
    atomic_fetch_add_explicit(x, 20202020202020ll,
                              memory_order_relaxed);
    thrd_yield();
    atomic_fetch_add_explicit(x, 22222222222222ll,
                              memory_order_relaxed);
    thrd_yield();
    atomic_fetch_add_explicit(x, -42424242424242ll,
                              memory_order_relaxed);
    thrd_yield();
  }
}

TEST("atomic int64 concurrency") {
  ATOMIC(int64_t) foo;
  ATOMIC(int64_t) bar;

  atomic_store_explicit(&foo, 42, memory_order_relaxed);
  atomic_store_explicit(&bar, 43, memory_order_relaxed);

  thrd_t threads[THREAD_COUNT];
  for (ptrdiff_t i = 0; i < THREAD_COUNT; i++)
    thrd_create(threads + i, test_64_,
                (void *) ((i % 2) ? &foo : &bar));
  for (ptrdiff_t i = 0; i < THREAD_COUNT; i++)
    thrd_join(threads[i], NULL);

  REQUIRE(atomic_load_explicit(&foo, memory_order_relaxed) == 42);
  REQUIRE(atomic_load_explicit(&bar, memory_order_relaxed) == 43);
}
