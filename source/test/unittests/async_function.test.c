#include "../../kit/async_function.h"

#define KIT_TEST_FILE async_function
#include "../../kit_test/test.h"

AF_STATE(int, test_foo, );
static AF_DECL(test_foo);

CORO_IMPL(test_foo) {
  AF_RETURN(42);
}
CORO_END

AF_STATE(int, test_bar, );
static AF_DECL(test_bar);

CORO_IMPL(test_bar) {
  AF_YIELD_VOID;
  AF_RETURN(42);
}
CORO_END

STATIC_CORO(int, test_gen, int i; int min; int max;) {
  for (self->i = self->min; self->i < self->max; self->i++)
    AF_YIELD(self->i);
  AF_RETURN(self->max);
}
CORO_END

STATIC_CORO_VOID(test_task, ) {
  AF_YIELD_VOID;
  AF_YIELD_VOID;
  AF_RETURN_VOID;
}
CORO_END

STATIC_CORO_VOID(test_nest_task, AF_TYPE(test_task) promise;) {
  AF_INIT(self->promise, test_task, );
  AF_AWAIT(self->promise);
  AF_AWAIT(self->promise);
  AF_AWAIT(self->promise);
}
CORO_END

STATIC_CORO(int, test_nest_generator, AF_TYPE(test_gen) promise;) {
  AF_INIT(self->promise, test_gen, .min = 1, .max = 3);
  AF_YIELD_AWAIT(self->promise);
}
CORO_END

TEST("coroutine create") {
  AF_CREATE(promise, test_foo, );
  REQUIRE(!AF_FINISHED(promise));
}

TEST("coroutine init") {
  AF_TYPE(test_foo) promise;
  AF_INIT(promise, test_foo, );
  REQUIRE(!AF_FINISHED(promise));
}

TEST("coroutine init with value") {
  AF_TYPE(test_foo) promise;
  AF_INIT(promise, test_foo, .return_value = 42);
  REQUIRE(promise.return_value == 42);
  REQUIRE(!AF_FINISHED(promise));
}

TEST("coroutine create with value") {
  AF_CREATE(promise, test_foo, .return_value = -1);
  REQUIRE(promise.return_value == -1);
  REQUIRE(!AF_FINISHED(promise));
}

TEST("coroutine execute and return") {
  AF_CREATE(promise, test_foo, );
  REQUIRE(AF_NEXT(promise) == 42);
  REQUIRE(AF_FINISHED(promise));
}

TEST("coroutine execute two steps") {
  AF_CREATE(promise, test_bar, .return_value = 0);
  AF_EXECUTE(promise);
  REQUIRE(promise.return_value == 0);
  AF_EXECUTE(promise);
  REQUIRE(promise.return_value == 42);
}

TEST("coroutine generator") {
  int i;
  AF_CREATE(promise, test_gen, .min = 10, .max = 15);
  for (i = 0; i <= 5; i++) REQUIRE(AF_NEXT(promise) == 10 + i);
}

TEST("coroutine status finished") {
  AF_CREATE(promise, test_bar, );
  REQUIRE(!AF_FINISHED(promise));
  AF_EXECUTE(promise);
  REQUIRE(!AF_FINISHED(promise));
  AF_EXECUTE(promise);
  REQUIRE(AF_FINISHED(promise));
}

TEST("coroutine task") {
  AF_CREATE(promise, test_task, );
  AF_EXECUTE(promise);
  REQUIRE(!AF_FINISHED(promise));
  AF_EXECUTE(promise);
  REQUIRE(!AF_FINISHED(promise));
  AF_EXECUTE(promise);
  REQUIRE(AF_FINISHED(promise));
}

TEST("coroutine nested task") {
  AF_CREATE(promise, test_nest_task, );
  AF_EXECUTE(promise);
  REQUIRE(!AF_FINISHED(promise));
  AF_EXECUTE(promise);
  REQUIRE(!AF_FINISHED(promise));
  AF_EXECUTE(promise);
  REQUIRE(AF_FINISHED(promise));
}

TEST("coroutine nested generator") {
  AF_CREATE(promise, test_nest_generator, );
  REQUIRE(AF_NEXT(promise) == 1);
  REQUIRE(AF_NEXT(promise) == 2);
  REQUIRE(AF_NEXT(promise) == 3);
  REQUIRE(!AF_FINISHED(promise));
  AF_EXECUTE(promise);
  REQUIRE(AF_FINISHED(promise));
}
