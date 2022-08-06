#ifndef KIT_TEST_TEST_H
#define KIT_TEST_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <string.h>

#ifndef KIT_TEST_FILE
#  define KIT_TEST_FILE kit_test
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
                                _Bool);
typedef void (*kit_test_function)(int, kit_test_report);

struct kit_test_case {
  char              test_name[KIT_TEST_STRING_SIZE];
  kit_test_function test_fn;
  int               assertions;
  char const       *file[KIT_TEST_ASSERTIONS_LIMIT];
  int               line[KIT_TEST_ASSERTIONS_LIMIT];
  _Bool             status[KIT_TEST_ASSERTIONS_LIMIT];
};

struct kit_tests_list {
  int                  size;
  struct kit_test_case tests[KIT_TESTS_SIZE_LIMIT];
};

extern struct kit_tests_list kit_tests_list;

#ifdef _MSC_VER
#  pragma section(".CRT$XCU", read)
#  define KIT_TEST_ON_START_2_(f, p)                               \
    static void f(void);                                           \
    __declspec(allocate(".CRT$XCU")) void (*f##_)(void) = f;       \
    __pragma(comment(linker, "/include:" p #f "_")) static void f( \
        void)
#  ifdef _WIN64
#    define KIT_TEST_ON_START_(f) KIT_TEST_ON_START_2_(f, "")
#  else
#    define KIT_TEST_ON_START_(f) KIT_TEST_ON_START_2_(f, "_")
#  endif
#else
#  define KIT_TEST_ON_START_(f)                       \
    static void f(void) __attribute__((constructor)); \
    static void f(void)
#endif

#define KIT_TEST_CONCAT4_(a, b, c, d) a##b##c##d
#define KIT_TEST_CONCAT3_(a, b, c) KIT_TEST_CONCAT4_(a, b, _, c)

#define KIT_TEST(name)                                               \
  static void KIT_TEST_CONCAT3_(                                     \
      kit_test_run_, __LINE__, KIT_TEST_FILE)(int, kit_test_report); \
  KIT_TEST_ON_START_(                                                \
      KIT_TEST_CONCAT3_(kit_test_case_, __LINE__, KIT_TEST_FILE)) {  \
    int n = kit_tests_list.size;                                     \
    if (n < KIT_TESTS_SIZE_LIMIT) {                                  \
      kit_tests_list.size++;                                         \
      kit_tests_list.tests[n].test_fn = KIT_TEST_CONCAT3_(           \
          kit_test_run_, __LINE__, KIT_TEST_FILE);                   \
      strcpy(kit_tests_list.tests[n].test_name, name);               \
      kit_tests_list.tests[n].assertions = 0;                        \
    }                                                                \
  }                                                                  \
  static void KIT_TEST_CONCAT3_(kit_test_run_, __LINE__,             \
                                KIT_TEST_FILE)(                      \
      int kit_test_index_, kit_test_report kit_test_report_)

#define KIT_REQUIRE(ok) \
  kit_test_report_(kit_test_index_, __FILE__, __LINE__, (ok))

int kit_run_tests(int argc, char **argv);

#ifndef KIT_DISABLE_SHORT_NAMES
#  define TEST KIT_TEST
#  define REQUIRE KIT_REQUIRE

#  define run_tests kit_run_tests
#endif

#ifdef __cplusplus
}
#endif

#endif
