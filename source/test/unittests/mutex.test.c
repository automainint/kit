#include "../../kit/mutex.h"

#define KIT_TEST_FILE mutex
#include "../../kit_test/test.h"

enum { SLEEP = 400000000, TICK_COUNT = 200, THREAD_COUNT = 100 };

typedef struct {
  mtx_t lock;
  int   value;
} test_data_t;

static int test_run(void *data) {
  test_data_t *x = (test_data_t *) data;
  for (int i = 0; i < TICK_COUNT; i++) {
    mtx_lock(&x->lock);

    x->value += i;
    thrd_yield();
    x->value -= i + 42;
    thrd_yield();
    x->value += i + 20;
    thrd_yield();
    x->value += 22 - i;

    mtx_unlock(&x->lock);
  }
  return 0;
}

TEST("mutex lock") {
  test_data_t data;
  thrd_t      pool[THREAD_COUNT];
  data.value = 42;
  REQUIRE(mtx_init(&data.lock, mtx_plain) == thrd_success);

  for (ptrdiff_t i = 0; i < THREAD_COUNT; i++)
    thrd_create(pool + i, test_run, &data);
  for (ptrdiff_t i = 0; i < THREAD_COUNT; i++)
    thrd_join(pool[i], NULL);

  mtx_destroy(&data.lock);
  REQUIRE(data.value == 42);
}

static int test_lock(void *data) {
  mtx_t *m = (mtx_t *) data;
  mtx_lock(m);

  struct timespec sec = { .tv_sec = 0, .tv_nsec = SLEEP };
  thrd_sleep(&sec, NULL);

  mtx_unlock(m);

  return 0;
}

TEST("mutex try lock") {
  mtx_t m;
  REQUIRE(mtx_init(&m, mtx_plain) == thrd_success);

  thrd_t t;
  REQUIRE(thrd_create(&t, test_lock, &m) == thrd_success);

  struct timespec sec = { .tv_sec = 0, .tv_nsec = SLEEP / 2 };
  REQUIRE(thrd_sleep(&sec, NULL) == thrd_success);

  REQUIRE(mtx_trylock(&m) == thrd_busy);

  REQUIRE(thrd_join(t, NULL) == thrd_success);

  REQUIRE(mtx_trylock(&m) == thrd_success);
  REQUIRE(mtx_unlock(&m) == thrd_success);

  mtx_destroy(&m);
}
