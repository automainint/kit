#include "test.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

kit_tests_list_t kit_tests_list = { 0 };

static void report(int i, char const *file, int line, int ok) {
  int const n = kit_tests_list.tests[i].assertions++;

  if (n >= KIT_TEST_ASSERTIONS_LIMIT)
    return;

  kit_tests_list.tests[i].file[n]   = file;
  kit_tests_list.tests[i].line[n]   = line;
  kit_tests_list.tests[i].status[n] = ok;
}

static long long ns_to_ms(long long ns) {
  return (ns + 500000) / 1000000;
}

static long long sec_to_ms(long long sec) {
  return 1000 * sec;
}

enum { white, yellow, red, green };

static void color_code(int term_color, int c) {
  if (term_color) {
    if (c == white)
      printf("\x1b[37m");
    if (c == yellow)
      printf("\x1b[33m");
    if (c == red)
      printf("\x1b[31m");
    if (c == green)
      printf("\x1b[32m");
  }
}

void kit_test_register(char const *name, kit_test_run_fn fn) {
  int n = kit_tests_list.size++;
  if (n < KIT_TESTS_SIZE_LIMIT) {
    kit_tests_list.tests[n].test_fn    = fn;
    kit_tests_list.tests[n].test_name  = name;
    kit_tests_list.tests[n].assertions = 0;
  }
}

int kit_run_tests(int argc, char **argv) {
  int success_count         = 0;
  int fail_assertion_count  = 0;
  int total_assertion_count = 0;
  int status                = 0;
  int term_color            = 1;

  for (int i = 0; i < argc; i++)
    if (strcmp("--no-term-color", argv[i]) == 0)
      term_color = 0;

  for (int i = 0; i < kit_tests_list.size && i < KIT_TESTS_SIZE_LIMIT;
       i++) {
    color_code(term_color, yellow);
    printf("[ RUN... ] %s ", kit_tests_list.tests[i].test_name);
    color_code(term_color, white);

    struct timespec begin, end;
    timespec_get(&begin, TIME_UTC);

    kit_tests_list.tests[i].test_fn(i, report);

    timespec_get(&end, TIME_UTC);
    int duration = (int) (ns_to_ms(end.tv_nsec - begin.tv_nsec) +
                          sec_to_ms(end.tv_sec - begin.tv_sec));

    printf("\r");

    int test_status = 1;

    for (int j = 0; j < kit_tests_list.tests[i].assertions; j++)
      if (kit_tests_list.tests[i].status[j] == 0) {
        fail_assertion_count++;
        test_status = 0;
      }

    if (kit_tests_list.tests[i].assertions >
        KIT_TEST_ASSERTIONS_LIMIT)
      test_status = 0;

    total_assertion_count += kit_tests_list.tests[i].assertions;

    if (test_status == 0) {
      color_code(term_color, red);
      printf("[ RUN    ] %s\n", kit_tests_list.tests[i].test_name);
      printf("[ FAILED ] %s", kit_tests_list.tests[i].test_name);
      color_code(term_color, white);
      if (duration > 0)
        printf(" - %d ms", duration);
      printf("\n");
      status = 1;
    } else {
      color_code(term_color, green);
      printf("[ RUN    ] %s\n", kit_tests_list.tests[i].test_name);
      printf("[     OK ] %s", kit_tests_list.tests[i].test_name);
      color_code(term_color, white);
      if (duration > 0)
        printf(" - %d ms", duration);
      printf("\n");
      success_count++;
    }
  }

  printf("\n%d of %d tests passed.\n", success_count,
         kit_tests_list.size);

  printf("%d of %d assertions passed.\n\n",
         total_assertion_count - fail_assertion_count,
         total_assertion_count);

  if (status != 0) {
    for (int i = 0;
         i < kit_tests_list.size && i < KIT_TESTS_SIZE_LIMIT; i++) {
      if (kit_tests_list.tests[i].assertions >
          KIT_TEST_ASSERTIONS_LIMIT)
        printf("Too many assertions for \"%s\" in \"%s\"!\n",
               kit_tests_list.tests[i].test_name,
               kit_tests_list.tests[i]
                   .file[KIT_TEST_ASSERTIONS_LIMIT - 1]);
      else
        for (int j = 0; j < kit_tests_list.tests[i].assertions; j++)
          if (!kit_tests_list.tests[i].status[j])
            printf("Assertion on line %d in \"%s\" failed\n",
                   kit_tests_list.tests[i].line[j],
                   kit_tests_list.tests[i].file[j]);
    }

    printf("\n");
  }

  if (kit_tests_list.size > KIT_TESTS_SIZE_LIMIT) {
    printf("Too many tests!\n\n");
    status = 1;
  }

  if (status == 0) {
    color_code(term_color, green);
    printf("OK\n");
  } else {
    color_code(term_color, red);
    printf("FAILED\n");
  }

  color_code(term_color, white);
  return status;
}
