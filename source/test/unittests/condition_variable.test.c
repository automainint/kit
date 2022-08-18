#include "../../kit/threads.h"

#define KIT_TEST_FILE condition_variable
#include "../../kit_test/test.h"

#include <stdio.h>

typedef struct {
  mtx_t m;
  cnd_t send;
  cnd_t receive;
  int   value;
} test_data_t;

static int test_run(void *p) {
  test_data_t *data = (test_data_t *) p;

  mtx_lock(&data->m);

  data->value = 20;
  mtx_unlock(&data->m);
  cnd_broadcast(&data->send);

  cnd_wait(&data->receive, &data->m);

  data->value = 22;
  cnd_broadcast(&data->send);

  mtx_unlock(&data->m);

  return 0;
}

TEST("condition variable") {
  printf("\n\n%% condition variable\n\n");
  
  test_data_t data;
  REQUIRE(mtx_init(&data.m, mtx_plain) == thrd_success);
  REQUIRE(cnd_init(&data.send) == thrd_success);
  REQUIRE(cnd_init(&data.receive) == thrd_success);
  data.value = 0;

  thrd_t t;
  REQUIRE(thrd_create(&t, test_run, &data) == thrd_success);

  REQUIRE(mtx_lock(&data.m) == thrd_success);

  REQUIRE(cnd_wait(&data.send, &data.m) == thrd_success);
  int x = data.value;

  REQUIRE(cnd_broadcast(&data.receive) == thrd_success);

  REQUIRE(cnd_wait(&data.send, &data.m) == thrd_success);
  x += data.value;

  REQUIRE(mtx_unlock(&data.m) == thrd_success);
  REQUIRE(thrd_join(t, NULL) == thrd_success);

  mtx_destroy(&data.m);
  cnd_destroy(&data.send);
  cnd_destroy(&data.receive);

  REQUIRE(x == 42);
}
