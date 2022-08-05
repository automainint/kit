#ifndef KIT_TEST_TEST_H
#define KIT_TEST_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#ifndef KIT_TEST_FILE
#  define kit_test
#endif

#ifndef KIT_TESTS_SIZE_LIMIT
#  define KIT_TESTS_SIZE_LIMIT 0x1000
#endif

#ifndef KIT_TEST_ASSERTIONS_LIMIT
#  define KIT_TEST_ASSERTIONS_LIMIT 0x50
#endif

#ifndef KIT_TEST_STRING_SIZE
#  define KIT_TEST_STRING_SIZE 0x100
#endif

typedef void (*kit_test_report)(int, char const *file, int line,
                                bool);
typedef void (*kit_test_function)(int, kit_test_report);

struct kit_test_case {
  char              test_name[KIT_TEST_STRING_SIZE];
  kit_test_function test_fn;
  int               assertions;
  char const       *file[KIT_TEST_ASSERTIONS_LIMIT];
  int               line[KIT_TEST_ASSERTIONS_LIMIT];
  bool              status[KIT_TEST_ASSERTIONS_LIMIT];
};

struct kit_tests_list {
  int                  size;
  struct kit_test_case tests[KIT_TESTS_SIZE_LIMIT];
};

extern struct kit_tests_list kit_tests_list;

#ifdef _MSC_VER
#  pragma section(".CRT$XCU", read)
#  define KIT_TEST_ON_START_2(f, p)                                \
    static void f(void);                                           \
    __declspec(allocate(".CRT$XCU")) void (*f##_)(void) = f;       \
    __pragma(comment(linker, "/include:" p #f "_")) static void f( \
        void)
#  ifdef _WIN64
#    define KIT_TEST_ON_START(f) KIT_TEST_ON_START_2(f, "")
#  else
#    define KIT_TEST_ON_START(f) KIT_TEST_ON_START_2(f, "_")
#  endif
#else
#  define KIT_TEST_ON_START(f)                        \
    static void f(void) __attribute__((constructor)); \
    static void f(void)
#endif

#define KIT_TEST_CONCAT4(a, b, c, d) a##b##c##d
#define KIT_TEST_CONCAT3(a, b, c) KIT_TEST_CONCAT4(a, b, _, c)

#define TEST(name)                                                   \
  static void KIT_TEST_CONCAT3(kit_test_run_, __LINE__,              \
                               KIT_TEST_FILE)(int, kit_test_report); \
  KIT_TEST_ON_START(                                                 \
      KIT_TEST_CONCAT3(kit_test_case_, __LINE__, KIT_TEST_FILE)) {   \
    int n = kit_tests_list.size;                                     \
    if (n < KIT_TESTS_SIZE_LIMIT) {                                  \
      kit_tests_list.size++;                                         \
      kit_tests_list.tests[n].test_fn = KIT_TEST_CONCAT3(            \
          kit_test_run_, __LINE__, KIT_TEST_FILE);                   \
      strcpy(kit_tests_list.tests[n].test_name, name);               \
      kit_tests_list.tests[n].assertions = 0;                        \
    }                                                                \
  }                                                                  \
  static void KIT_TEST_CONCAT3(kit_test_run_, __LINE__,              \
                               KIT_TEST_FILE)(                       \
      int kit_test_index_, kit_test_report kit_test_report_)

#define REQUIRE(ok) \
  kit_test_report_(kit_test_index_, __FILE__, __LINE__, (ok))

int kit_run_tests(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif
