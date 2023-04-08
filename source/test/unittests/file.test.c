#include "../../kit/file.h"
#include "../../kit/string_ref.h"
#include <string.h>

#define KIT_TEST_FILE file
#include "../../kit_test/test.h"

TEST("file path cache") {
  kit_allocator_t alloc = kit_alloc_default();

  string_t user  = path_user(alloc);
  string_t cache = path_cache(alloc);

  DA_RESIZE(cache, cache.size + 1);
  cache.values[cache.size - 1] = '\0';
  DA_RESIZE(cache, cache.size - 1);

  string_t expected =
#if defined(_WIN32) && !defined(__CYGWIN__)
      path_join(WRAP_STR(user), SZ("AppData" PATH_DELIM "Local"),
                alloc);
#elif defined(__APPLE__)
      path_join(WRAP_STR(user), SZ("Library" PATH_DELIM "Caches"),
                alloc);
#else
      path_join(WRAP_STR(user), SZ(".cache"), alloc);
#endif

  REQUIRE(AR_EQUAL(cache, expected));

  DA_DESTROY(user);
  DA_DESTROY(cache);
  DA_DESTROY(expected);
}

TEST("file path normalize one") {
  str_t foo      = SZ("foo/bar/../baz");
  str_t foo_norm = SZ("foo" PATH_DELIM "baz");

  string_t bar = path_norm(foo, kit_alloc_default());

  REQUIRE(AR_EQUAL(foo_norm, bar));

  DA_DESTROY(bar);
}

TEST("file path normalize two") {
  str_t foo      = SZ("foo/bar/../../baz");
  str_t foo_norm = SZ("baz");

  string_t bar = path_norm(foo, kit_alloc_default());

  REQUIRE(AR_EQUAL(foo_norm, bar));

  DA_DESTROY(bar);
}

TEST("file path normalize parent") {
  str_t foo      = SZ("../baz");
  str_t foo_norm = SZ(".." PATH_DELIM "baz");

  string_t bar = path_norm(foo, kit_alloc_default());

  REQUIRE(AR_EQUAL(foo_norm, bar));

  DA_DESTROY(bar);
}

TEST("file path normalize double parent") {
  str_t foo      = SZ("foo/../../baz");
  str_t foo_norm = SZ(".." PATH_DELIM "baz");

  string_t bar = path_norm(foo, kit_alloc_default());

  REQUIRE(AR_EQUAL(foo_norm, bar));

  DA_DESTROY(bar);
}

TEST("file path normalize windows delim") {
  str_t foo      = SZ("foo\\bar\\..\\baz");
  str_t foo_norm = SZ("foo" PATH_DELIM "baz");

  string_t bar = path_norm(foo, kit_alloc_default());

  REQUIRE(AR_EQUAL(foo_norm, bar));

  DA_DESTROY(bar);
}

TEST("file path join no delim") {
  str_t foo    = SZ("foo");
  str_t bar    = SZ("bar");
  str_t joined = SZ("foo" PATH_DELIM "bar");

  string_t foobar = path_join(foo, bar, kit_alloc_default());

  REQUIRE(AR_EQUAL(joined, foobar));

  DA_DESTROY(foobar);
}

TEST("file path join delim left") {
  str_t foo    = SZ("foo/");
  str_t bar    = SZ("bar");
  str_t joined = SZ("foo" PATH_DELIM "bar");

  string_t foobar = path_join(foo, bar, kit_alloc_default());

  REQUIRE(AR_EQUAL(joined, foobar));

  DA_DESTROY(foobar);
}

TEST("file path join delim right") {
  str_t foo    = SZ("foo");
  str_t bar    = SZ("/bar");
  str_t joined = SZ("foo" PATH_DELIM "bar");

  string_t foobar = path_join(foo, bar, kit_alloc_default());

  REQUIRE(AR_EQUAL(joined, foobar));

  DA_DESTROY(foobar);
}

TEST("file path join delim both") {
  str_t foo    = SZ("foo/");
  str_t bar    = SZ("/bar");
  str_t joined = SZ("foo" PATH_DELIM "bar");

  string_t foobar = path_join(foo, bar, kit_alloc_default());

  REQUIRE(AR_EQUAL(joined, foobar));

  DA_DESTROY(foobar);
}

TEST("file path user") {
  string_t user = path_user(kit_alloc_default());

  REQUIRE(user.size > 0);

  DA_DESTROY(user);
}

TEST("file path index relative") {
  str_t foobar = SZ("foo/bar");
  str_t foo    = SZ("foo");
  str_t bar    = SZ("bar");

  REQUIRE(AR_EQUAL(path_index(foobar, 0), foo));
  REQUIRE(AR_EQUAL(path_index(foobar, 1), bar));
  REQUIRE(path_index(foobar, 2).size == 0);
}

TEST("file path index absolute") {
  str_t foobar = SZ("/foo/bar");
  str_t foo    = SZ("foo");
  str_t bar    = SZ("bar");

  REQUIRE(AR_EQUAL(path_index(foobar, 0), foo));
  REQUIRE(AR_EQUAL(path_index(foobar, 1), bar));
  REQUIRE(path_index(foobar, 2).size == 0);
}

TEST("file path index windows disk name") {
  str_t foobar = SZ("c:\\foo\\bar");
  str_t disk   = SZ("c:");
  str_t foo    = SZ("foo");
  str_t bar    = SZ("bar");

  REQUIRE(AR_EQUAL(path_index(foobar, 0), disk));
  REQUIRE(AR_EQUAL(path_index(foobar, 1), foo));
  REQUIRE(AR_EQUAL(path_index(foobar, 2), bar));
  REQUIRE(path_index(foobar, 3).size == 0);
}

TEST("file path take relative") {
  str_t foobar  = SZ("foo/bar/");
  str_t foo     = SZ("foo");
  str_t bar     = SZ("foo/bar");
  str_t bar_end = SZ("foo/bar/");

  REQUIRE(AR_EQUAL(path_take(foobar, 0), foo));
  REQUIRE(AR_EQUAL(path_take(foobar, 1), bar));
  REQUIRE(AR_EQUAL(path_take(foobar, 2), bar_end));
}

TEST("file path take absolute") {
  str_t foobar = SZ("/foo/bar");
  str_t foo    = SZ("/foo");
  str_t bar    = SZ("/foo/bar");

  REQUIRE(AR_EQUAL(path_take(foobar, 0), foo));
  REQUIRE(AR_EQUAL(path_take(foobar, 1), bar));
}

TEST("file path take windows disk name") {
  str_t foobar = SZ("c:\\foo\\bar");
  str_t disk   = SZ("c:");
  str_t foo    = SZ("c:\\foo");
  str_t bar    = SZ("c:\\foo\\bar");

  REQUIRE(AR_EQUAL(path_take(foobar, 0), disk));
  REQUIRE(AR_EQUAL(path_take(foobar, 1), foo));
  REQUIRE(AR_EQUAL(path_take(foobar, 2), bar));
}

TEST("file create folder") { }

TEST("file create folder recursive") { }

TEST("file remove") { }

TEST("file remove folder") { }

TEST("file remove recursive") { }

TEST("file enum folder") { }
