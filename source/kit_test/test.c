#include "test.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <setjmp.h>
#include <signal.h>

#include <stdlib.h>

kit_tests_list_t kit_tests_list = { 0 };

static void report(int i, int line, int ok) {
  int const n = kit_tests_list.tests[i].assertions++;

  if (n >= KIT_TEST_ASSERTIONS_LIMIT)
    return;

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

void kit_test_register(char const *name, char const *file,
                       kit_test_run_fn fn) {
  int n = kit_tests_list.size++;
  if (n < KIT_TESTS_SIZE_LIMIT) {
    kit_tests_list.tests[n].test_fn    = fn;
    kit_tests_list.tests[n].test_name  = name;
    kit_tests_list.tests[n].test_file  = file;
    kit_tests_list.tests[n].assertions = 0;
  }
}

static jmp_buf kit_test_restore_execution;

static int const signums[] = { SIGILL, SIGABRT, SIGFPE, SIGSEGV,
                               SIGTERM };

static char const *const signames[] = {
  [SIGILL]  = "Illegal instruction",
  [SIGABRT] = "Abnormal termination",
  [SIGFPE]  = "Erroneous arithmetic operation",
  [SIGSEGV] = "Invalid access to storage",
  [SIGTERM] = "Termination request"
};

static void handle_signal(int signum) {
  printf("SIGNAL: %d\n\n", signum);
  exit(42);
  longjmp(kit_test_restore_execution, signum);
}

static void setup_signals() {
  for (int i = 0; i < sizeof signums / sizeof *signums; i++) {
#if defined(_WIN32) && !(defined __CYGWIN__)
    signal(signums[i], handle_signal);
#else
    struct sigaction action;
    memset(&action, 0, sizeof action);
    action.sa_handler = handle_signal;

    sigaction(signums[i], &action, NULL);
#endif
  }
}

static int run_test(volatile int i) {
  int signum = setjmp(kit_test_restore_execution);

  if (signum != 0) {
    kit_tests_list.tests[i].signal = signum;
    return 0;
  }

  kit_tests_list.tests[i].test_fn(i, report);
  return 1;
}

int kit_run_tests(int argc, char **argv) {
  setup_signals();

  int success_count         = 0;
  int fail_assertion_count  = 0;
  int total_assertion_count = 0;
  int status                = 0;
  int quiet                 = 0;
  int term_color            = 1;
  int carriage_return       = 1;

  for (int i = 0; i < argc; i++)
    if (strcmp("--no-term-color", argv[i]) == 0)
      term_color = 0;
    else if (strcmp("--no-carriage-return", argv[i]) == 0)
      carriage_return = 0;
    else if (strcmp("--quiet", argv[i]) == 0)
      quiet = 1;

  if (quiet)
    term_color = 0;

  for (int i = 0; i < kit_tests_list.size && i < KIT_TESTS_SIZE_LIMIT;
       i++) {
    color_code(term_color, yellow);
    quiet ||
        printf("[ RUN... ] %s ", kit_tests_list.tests[i].test_name);
    if (!carriage_return)
      quiet || printf("\n");
    color_code(term_color, white);

    struct timespec begin, end;
    timespec_get(&begin, TIME_UTC);

    int test_status = run_test(i);

    timespec_get(&end, TIME_UTC);
    int duration = (int) (ns_to_ms(end.tv_nsec - begin.tv_nsec) +
                          sec_to_ms(end.tv_sec - begin.tv_sec));

    if (carriage_return)
      quiet || printf("\r");

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
      quiet || printf("[ RUN    ] %s\n",
                      kit_tests_list.tests[i].test_name);
      quiet ||
          printf("[ FAILED ] %s", kit_tests_list.tests[i].test_name);
      color_code(term_color, white);
      if (duration > 0)
        quiet || printf(" - %d ms", duration);
      quiet || printf("\n");
      status = 1;
    } else {
      color_code(term_color, green);
      quiet || printf("[ RUN    ] %s\n",
                      kit_tests_list.tests[i].test_name);
      quiet ||
          printf("[     OK ] %s", kit_tests_list.tests[i].test_name);
      color_code(term_color, white);
      if (duration > 0)
        quiet || printf(" - %d ms", duration);
      quiet || printf("\n");
      success_count++;
    }
  }

  quiet || printf("\n%d of %d tests passed.\n", success_count,
                  kit_tests_list.size);

  quiet || printf("%d of %d assertions passed.\n\n",
                  total_assertion_count - fail_assertion_count,
                  total_assertion_count);

  if (status != 0) {
    for (int i = 0;
         i < kit_tests_list.size && i < KIT_TESTS_SIZE_LIMIT; i++) {
      if (kit_tests_list.tests[i].signal != 0) {
        int signum = kit_tests_list.tests[i].signal;
        if (signum >= 0 &&
            signum < sizeof signames / sizeof *signames)
          quiet ||
              printf("Signal \"%s\" (%d) for \"%s\" in \"%s\"!\n",
                     signames[signum], signum,
                     kit_tests_list.tests[i].test_name,
                     kit_tests_list.tests[i].test_file);
        else
          quiet ||
              printf("Unknown signal (%d) for \"%s\" in \"%s\"!\n",
                     signum, kit_tests_list.tests[i].test_name,
                     kit_tests_list.tests[i].test_file);
      }
      if (kit_tests_list.tests[i].assertions >
          KIT_TEST_ASSERTIONS_LIMIT)
        quiet || printf("Too many assertions for \"%s\" in \"%s\"!\n",
                        kit_tests_list.tests[i].test_name,
                        kit_tests_list.tests[i].test_file);
      else
        for (int j = 0; j < kit_tests_list.tests[i].assertions; j++)
          if (!kit_tests_list.tests[i].status[j])
            quiet || printf("Assertion on line %d in \"%s\" failed\n",
                            kit_tests_list.tests[i].line[j],
                            kit_tests_list.tests[i].test_file);
    }

    quiet || printf("\n");
  }

  if (kit_tests_list.size > KIT_TESTS_SIZE_LIMIT) {
    quiet || printf("Too many tests!\n\n");
    status = 1;
  }

  if (status == 0) {
    color_code(term_color, green);
    quiet || printf("OK\n");
  } else {
    color_code(term_color, red);
    quiet || printf("FAILED\n");
  }

  color_code(term_color, white);
  return status;
}
