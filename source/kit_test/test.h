#ifndef KIT_TEST_TEST_H
#define KIT_TEST_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#ifndef KIT_TEST_FILE
#  define KIT_TEST_FILE kit_test
#endif

#ifndef KIT_TESTS_SIZE_LIMIT
#  define KIT_TESTS_SIZE_LIMIT 0x1000
#endif

#ifndef KIT_TEST_ASSERTIONS_LIMIT
#  define KIT_TEST_ASSERTIONS_LIMIT 0x50
#endif

typedef void (*kit_test_report_fn)(int test_index, char const *file,
                                   int line, int ok);
typedef void (*kit_test_run_fn)(
    int kit_test_index_, kit_test_report_fn kit_test_report_fn_);

typedef struct {
  char const     *test_name;
  kit_test_run_fn test_fn;
  int             assertions;
  char const     *file[KIT_TEST_ASSERTIONS_LIMIT];
  int             line[KIT_TEST_ASSERTIONS_LIMIT];
  int             status[KIT_TEST_ASSERTIONS_LIMIT];
} kit_test_case_t;

typedef struct {
  int             size;
  kit_test_case_t tests[KIT_TESTS_SIZE_LIMIT];
} kit_tests_list_t;

extern kit_tests_list_t kit_tests_list;

#define KIT_TEST_CONCAT4_(a, b, c, d) a##b##c##d
#define KIT_TEST_CONCAT3_(a, b, c) KIT_TEST_CONCAT4_(a, b, _, c)

#ifdef __cplusplus
#  define KIT_TEST_ON_START_(f)                              \
    static void f(void);                                     \
    static int  KIT_TEST_CONCAT3_(_kit_test_init_, __LINE__, \
                                  f) = (f(), 0);             \
    static void f(void)
#else
#  ifdef _MSC_VER
#    ifdef __cplusplus
#      define KIT_EXTERN_C_ extern "C"
#    else
#      define KIT_EXTERN_C_
#    endif

#    pragma section(".CRT$XCU", read)
#    define KIT_TEST_ON_START_2_(f, p)                               \
      static void f(void);                                           \
      __declspec(allocate(".CRT$XCU")) void (*f##_)(void) = f;       \
      __pragma(comment(linker, "/include:" p #f "_")) static void f( \
          void)
#    ifdef _WIN64
#      define KIT_TEST_ON_START_(f) KIT_TEST_ON_START_2_(f, "")
#    else
#      define KIT_TEST_ON_START_(f) KIT_TEST_ON_START_2_(f, "_")
#    endif
#  else
#    define KIT_TEST_ON_START_(f)                       \
      static void f(void) __attribute__((constructor)); \
      static void f(void)
#  endif
#endif

void kit_test_register(char const *name, kit_test_run_fn fn);

#define KIT_TEST(name)                                              \
  static void KIT_TEST_CONCAT3_(kit_test_run_, __LINE__,            \
                                KIT_TEST_FILE)(int,                 \
                                               kit_test_report_fn); \
  KIT_TEST_ON_START_(                                               \
      KIT_TEST_CONCAT3_(kit_test_case_, __LINE__, KIT_TEST_FILE)) { \
    kit_test_register(                                              \
        name,                                                       \
        KIT_TEST_CONCAT3_(kit_test_run_, __LINE__, KIT_TEST_FILE)); \
  }                                                                 \
  static void KIT_TEST_CONCAT3_(kit_test_run_, __LINE__,            \
                                KIT_TEST_FILE)(                     \
      int kit_test_index_, kit_test_report_fn kit_test_report_fn_)

#define KIT_REQUIRE(...)                                   \
  kit_test_report_fn_(kit_test_index_, __FILE__, __LINE__, \
                      (__VA_ARGS__))

int kit_run_tests(int argc, char **argv);

#ifndef KIT_DISABLE_SHORT_NAMES
#  define TEST KIT_TEST
#  define REQUIRE KIT_REQUIRE

#  define test_register kit_test_register
#  define run_tests kit_run_tests
#endif

#ifdef __cplusplus
}
#endif

#endif
