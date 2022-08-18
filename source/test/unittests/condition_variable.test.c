#include "../../kit/threads.h"

#define KIT_TEST_FILE condition_variable
#include "../../kit_test/test.h"

#include <stdio.h>

typedef struct {
  mtx_t m;
  int   in;
  int   out;
  cnd_t send;
  cnd_t receive;
  int   value;
} test_data_t;

static int test_run(void *p) {
  test_data_t *data = (test_data_t *) p;

  mtx_lock(&data->m);
  data->value = 20;
  data->out   = 1;
  mtx_unlock(&data->m);

  cnd_broadcast(&data->send);

  mtx_lock(&data->m);
  if (data->in == 0)
    cnd_wait(&data->receive, &data->m);
  data->in    = 0;
  data->value = 22;
  data->out   = 1;
  mtx_unlock(&data->m);

  cnd_broadcast(&data->send);

  return 0;
}

TEST("condition variable") {
  int ok = 1;

  for (int i = 0; i < 10; i++) {
    test_data_t data;

    data.in    = 0;
    data.out   = 0;
    data.value = 0;

    ok = ok && (mtx_init(&data.m, mtx_plain) == thrd_success);
    ok = ok && (cnd_init(&data.send) == thrd_success);
    ok = ok && (cnd_init(&data.receive) == thrd_success);

    thrd_t t;
    ok = ok && (thrd_create(&t, test_run, &data) == thrd_success);

    ok = ok && (mtx_lock(&data.m) == thrd_success);
    if (data.out == 0)
      ok = ok && (cnd_wait(&data.send, &data.m) == thrd_success);
    data.out = 0;
    int x    = data.value;
    data.in  = 1;
    ok       = ok && (mtx_unlock(&data.m) == thrd_success);

    ok = ok && (cnd_broadcast(&data.receive) == thrd_success);

    ok = ok && (mtx_lock(&data.m) == thrd_success);
    if (data.out == 0)
      ok = ok && (cnd_wait(&data.send, &data.m) == thrd_success);
    data.out = 0;
    x += data.value;
    ok = ok && (mtx_unlock(&data.m) == thrd_success);

    ok = ok && (thrd_join(t, NULL) == thrd_success);

    mtx_destroy(&data.m);
    cnd_destroy(&data.send);
    cnd_destroy(&data.receive);

    ok = ok && (x == 42);
  }

  REQUIRE(ok);
}
