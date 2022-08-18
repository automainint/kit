#include "../../kit/threads.h"

#define KIT_TEST_FILE thread
#include "../../kit_test/test.h"

static int test_nothing(void *_) { }

static int test_run(void *data) {
  int *n = (int *) data;
  return *n + 20;
}

static int test_exit(void *data) {
  int *n = (int *) data;

  *n = 1;
  thrd_exit(3);
  *n = 2;
  return 4;
}

static int test_yield(void *data) {
  thrd_yield();
}

static int test_sleep(void *data) {
  struct timespec t = { .tv_sec = 0, .tv_nsec = 10000000 };
  thrd_sleep(&t, NULL);
}

TEST("thread run") {
  thrd_t t;
  int    data = 22;
  int    result;
  REQUIRE(thrd_create(&t, test_run, &data) == thrd_success);
  REQUIRE(thrd_join(t, &result) == thrd_success);
  REQUIRE(result == 42);
}

TEST("thread equal") {
  thrd_t foo, bar;
  REQUIRE(thrd_create(&foo, test_nothing, NULL) == thrd_success);
  REQUIRE(thrd_create(&bar, test_nothing, NULL) == thrd_success);
  REQUIRE(thrd_equal(foo, foo));
  REQUIRE(!thrd_equal(foo, bar));
  REQUIRE(!thrd_equal(foo, thrd_current()));
}

TEST("thread exit") {
  thrd_t foo;
  int    data;
  int    result;
  REQUIRE(thrd_create(&foo, test_exit, &data) == thrd_success);
  REQUIRE(thrd_join(foo, &result) == thrd_success);
  REQUIRE(data == 1);
  REQUIRE(result == 3);
}

TEST("thread yield") {
  thrd_t foo;
  REQUIRE(thrd_create(&foo, test_yield, NULL) == thrd_success);
  REQUIRE(thrd_join(foo, NULL) == thrd_success);
}

TEST("thread sleep") {
  thrd_t foo;
  REQUIRE(thrd_create(&foo, test_sleep, NULL) == thrd_success);
  REQUIRE(thrd_join(foo, NULL) == thrd_success);
}

TEST("thread detach") {
  thrd_t foo;
  REQUIRE(thrd_create(&foo, test_nothing, NULL) == thrd_success);
  REQUIRE(thrd_detach(foo) == thrd_success);

  struct timespec t = { .tv_sec = 0, .tv_nsec = 10000000 };
  thrd_sleep(&t, NULL);
}
