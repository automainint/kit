#include "../../kit/thread.h"

#define KIT_TEST_FILE thread
#include "../../kit_test/test.h"

static void *test_thread_fn(void *data) {
  ptrdiff_t *value = (ptrdiff_t *) data;
  return (void *) (*value + 20);
}

TEST("run thread") {
  pthread_t t;
  ptrdiff_t value = 22;
  pthread_create(&t, NULL, test_thread_fn, &value);
  void *result;
  pthread_join(t, &result);
  REQUIRE((ptrdiff_t) result == 42);
}
