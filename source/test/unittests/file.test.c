#include "../../kit/file.h"
#include "../../kit/string_ref.h"
#include <string.h>

#define KIT_TEST_FILE file
#include "../../kit_test/test.h"

#if defined(_WIN32) && !defined(__CYGWIN__)
#  define S_DELIM_ "\\"
#else
#  define S_DELIM_ "/"
#endif

TEST("file path normalize one") {
  SZ(foo, "foo/bar/../baz");
  SZ(foo_norm, "foo" S_DELIM_ "baz");

  string_t bar = path_norm(foo, kit_alloc_default());

  REQUIRE(AR_EQUAL(foo_norm, bar));

  DA_DESTROY(bar);
}

TEST("file path normalize two") {
  SZ(foo, "foo/bar/../../baz");
  SZ(foo_norm, "baz");

  string_t bar = path_norm(foo, kit_alloc_default());

  REQUIRE(AR_EQUAL(foo_norm, bar));

  DA_DESTROY(bar);
}

TEST("file path normalize parent") {
  SZ(foo, "../baz");
  SZ(foo_norm, ".." S_DELIM_ "baz");

  string_t bar = path_norm(foo, kit_alloc_default());

  REQUIRE(AR_EQUAL(foo_norm, bar));

  DA_DESTROY(bar);
}

TEST("file path normalize double parent") {
  SZ(foo, "foo/../../baz");
  SZ(foo_norm, ".." S_DELIM_ "baz");

  string_t bar = path_norm(foo, kit_alloc_default());

  REQUIRE(AR_EQUAL(foo_norm, bar));

  DA_DESTROY(bar);
}

TEST("file path normalize windows delim") {
  SZ(foo, "foo\\bar\\..\\baz");
  SZ(foo_norm, "foo" S_DELIM_ "baz");

  string_t bar = path_norm(foo, kit_alloc_default());

  REQUIRE(AR_EQUAL(foo_norm, bar));

  DA_DESTROY(bar);
}

TEST("file path join no delim") {
  SZ(foo, "foo");
  SZ(bar, "bar");
  SZ(joined, "foo/bar");

  string_t foobar = path_join(foo, bar, kit_alloc_default());

  REQUIRE(AR_EQUAL(joined, foobar));

  DA_DESTROY(foobar);
}

TEST("file path join delim left") {
  SZ(foo, "foo/");
  SZ(bar, "bar");
  SZ(joined, "foo/bar");

  string_t foobar = path_join(foo, bar, kit_alloc_default());

  REQUIRE(AR_EQUAL(joined, foobar));

  DA_DESTROY(foobar);
}

TEST("file path join delim right") {
  SZ(foo, "foo");
  SZ(bar, "/bar");
  SZ(joined, "foo/bar");

  string_t foobar = path_join(foo, bar, kit_alloc_default());

  REQUIRE(AR_EQUAL(joined, foobar));

  DA_DESTROY(foobar);
}

TEST("file path join delim both") {
  SZ(foo, "foo/");
  SZ(bar, "/bar");
  SZ(joined, "foo/bar");

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
  SZ(foobar, "foo/bar");
  SZ(foo, "foo");
  SZ(bar, "bar");

  REQUIRE(AR_EQUAL(path_index(foobar, 0), foo));
  REQUIRE(AR_EQUAL(path_index(foobar, 1), bar));
  REQUIRE(path_index(foobar, 2).size == 0);
}

TEST("file path index absolute") {
  SZ(foobar, "/foo/bar");
  SZ(foo, "foo");
  SZ(bar, "bar");

  REQUIRE(AR_EQUAL(path_index(foobar, 0), foo));
  REQUIRE(AR_EQUAL(path_index(foobar, 1), bar));
  REQUIRE(path_index(foobar, 2).size == 0);
}

TEST("file path index windows disk name") {
  SZ(foobar, "c:\\foo\\bar");
  SZ(disk, "c:");
  SZ(foo, "foo");
  SZ(bar, "bar");

  REQUIRE(AR_EQUAL(path_index(foobar, 0), disk));
  REQUIRE(AR_EQUAL(path_index(foobar, 1), foo));
  REQUIRE(AR_EQUAL(path_index(foobar, 2), bar));
  REQUIRE(path_index(foobar, 3).size == 0);
}

TEST("file path take relative") {
  SZ(foobar, "foo/bar/");
  SZ(foo, "foo");
  SZ(bar, "foo/bar");
  SZ(bar_end, "foo/bar/");

  REQUIRE(AR_EQUAL(path_take(foobar, 0), foo));
  REQUIRE(AR_EQUAL(path_take(foobar, 1), bar));
  REQUIRE(AR_EQUAL(path_take(foobar, 2), bar_end));
}

TEST("file path take absolute") {
  SZ(foobar, "/foo/bar");
  SZ(foo, "/foo");
  SZ(bar, "/foo/bar");

  REQUIRE(AR_EQUAL(path_take(foobar, 0), foo));
  REQUIRE(AR_EQUAL(path_take(foobar, 1), bar));
}

TEST("file path take windows disk name") {
  SZ(foobar, "c:\\foo\\bar");
  SZ(disk, "c:");
  SZ(foo, "c:\\foo");
  SZ(bar, "c:\\foo\\bar");

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
