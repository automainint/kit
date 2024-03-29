#include "test.h"

#define _GNU_SOURCE
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

kit_tests_list_t kit_tests_list = { 0 };

static void report(int i, int line, long long value,
                   long long expected) {
  int const n = kit_tests_list.v[i].assertions++;

  if (n >= KIT_TEST_ASSERTIONS_LIMIT)
    return;

  kit_tests_list.v[i].line[n]     = line;
  kit_tests_list.v[i].status[n]   = value == expected;
  kit_tests_list.v[i].value[n]    = value;
  kit_tests_list.v[i].expected[n] = expected;
}

static long long ns_to_ms(long long ns) {
  return (ns + 500000) / 1000000;
}

static long long sec_to_ms(long long sec) {
  return 1000 * sec;
}

enum { white, blue, light, yellow, red, green };

static char const *const color_codes[] = {
  [white] = "\x1b[38m",  [blue] = "\x1b[34m", [light] = "\x1b[37m",
  [yellow] = "\x1b[33m", [red] = "\x1b[31m",  [green] = "\x1b[32m"
};

static int print_color(int c) {
  return printf("%s", color_codes[c]);
}

void kit_test_register(char const *name, char const *file,
                       kit_test_run_fn fn) {
  int n = kit_tests_list.size++;
  if (n < KIT_TESTS_SIZE_LIMIT) {
    kit_tests_list.v[n].test_fn    = fn;
    kit_tests_list.v[n].test_name  = name;
    kit_tests_list.v[n].test_file  = file;
    kit_tests_list.v[n].assertions = 0;
  }
}

static jmp_buf kit_test_restore_execution;

static int const signums[] = { SIGINT, SIGILL,  SIGABRT,
                               SIGFPE, SIGSEGV, SIGTERM };

static char const *const signames[] = {
  [SIGINT]  = "Interactive attention signal",
  [SIGILL]  = "Illegal instruction",
  [SIGABRT] = "Abnormal termination",
  [SIGFPE]  = "Erroneous arithmetic operation",
  [SIGSEGV] = "Invalid access to storage",
  [SIGTERM] = "Termination request"
};

static void handle_signal(int signum) {
  longjmp(kit_test_restore_execution, signum);
}

static void setup_signals() {
  int i;

  for (i = 0; i < sizeof signums / sizeof *signums; i++) {
#if (defined(_WIN32) && !defined(__CYGWIN__)) || \
    !defined(_POSIX_C_SOURCE)
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
    kit_tests_list.v[i].signal = signum;
    return 0;
  }

  kit_tests_list.v[i].test_fn(i, report);
  return 1;
}

int kit_run_tests(int argc, char **argv) {
  int success_count         = 0;
  int fail_assertion_count  = 0;
  int total_assertion_count = 0;
  int status                = 0;
  int quiet                 = 0;
  int no_color              = 0;
  int line_width            = 20;
  int carriage_return       = 1;

  int i, j;

  char const *specific_test = NULL;

  setup_signals();

  for (i = 0; i < argc; i++)
    if (strcmp("--no-term-color", argv[i]) == 0)
      no_color = 1;
    else if (strcmp("--no-carriage-return", argv[i]) == 0)
      carriage_return = 0;
    else if (strcmp("--quiet", argv[i]) == 0)
      quiet = 1;
    else if (strcmp("--match", argv[i]) == 0)
      specific_test = argv[++i];

  quiet && (no_color = 1);

  if (specific_test != NULL) {
    no_color || print_color(light);
    quiet || printf("Run tests matching ");
    no_color || print_color(white);
    quiet || printf("*%s*", specific_test);
    no_color || print_color(light);
    quiet || printf("\n\n");
  }

  char const *file        = NULL;
  ptrdiff_t   file_root   = -1;
  int         tests_total = 0;

  for (i = 0; i < kit_tests_list.size && i < KIT_TESTS_SIZE_LIMIT;
       i++) {
    if (specific_test != NULL &&
        strstr(kit_tests_list.v[i].test_name, specific_test) == NULL)
      continue;
    tests_total++;
    int const l = 2 + (int) strlen(kit_tests_list.v[i].test_name);
    if (line_width < l)
      line_width = l;
  }

  if (tests_total > 0) {
    char const *const s = kit_tests_list.v[0].test_file;

    for (j = 1; j < kit_tests_list.size && j < KIT_TESTS_SIZE_LIMIT;
         j++) {
      if (specific_test != NULL &&
          strstr(kit_tests_list.v[j].test_name, specific_test) ==
              NULL)
        continue;
      if (strcmp(s, kit_tests_list.v[j].test_file) == 0)
        continue;
      int k = 0;
      for (;
           s[k] != '\0' && kit_tests_list.v[j].test_file[k] != '\0' &&
           s[k] == kit_tests_list.v[j].test_file[k];
           k++) { }
      if (file_root == -1 || file_root > k)
        file_root = k;
    }

    if (file_root == -1) {
      for (i = 0; s[i] != '\0'; i++)
        if (s[i] == '/' || s[i] == '\\')
          file_root = i + 1;
    }
  }

  for (i = 0; i < kit_tests_list.size && i < KIT_TESTS_SIZE_LIMIT;
       i++) {
    if (specific_test != NULL &&
        strstr(kit_tests_list.v[i].test_name, specific_test) == NULL)
      continue;
    if (file == NULL ||
        strcmp(file, kit_tests_list.v[i].test_file) != 0) {
      if (file != NULL)
        quiet || printf("\n");
      file = kit_tests_list.v[i].test_file;
      no_color || print_color(blue);
      quiet || printf("* ");
      no_color || print_color(white);
      quiet || printf("%s\n", file + file_root);
    }

    !carriage_return || no_color || print_color(yellow);
    carriage_return || no_color || print_color(light);
    quiet || printf("` %s ", kit_tests_list.v[i].test_name);
    !carriage_return || quiet || printf("\r");
    quiet || fflush(stdout);

    struct timespec begin, end;
    timespec_get(&begin, TIME_UTC);

    int test_status = run_test(i);

    timespec_get(&end, TIME_UTC);
    int duration = (int) (ns_to_ms(end.tv_nsec - begin.tv_nsec) +
                          sec_to_ms(end.tv_sec - begin.tv_sec));

    for (j = 0; j < kit_tests_list.v[i].assertions; j++)
      if (kit_tests_list.v[i].status[j] == 0) {
        fail_assertion_count++;
        test_status = 0;
      }

    if (kit_tests_list.v[i].assertions > KIT_TEST_ASSERTIONS_LIMIT)
      test_status = 0;

    total_assertion_count += kit_tests_list.v[i].assertions;

    !carriage_return || no_color || print_color(light);
    !carriage_return || quiet ||
        printf("` %s ", kit_tests_list.v[i].test_name);

    int const l = (int) strlen(kit_tests_list.v[i].test_name);
    quiet || printf("%*c", line_width - l, ' ');

    if (test_status == 0) {
      no_color || print_color(red);
      quiet || printf("FAIL");
      no_color || print_color(light);
      duration == 0 || quiet || printf(" %d ms", duration);
      quiet || printf("\n");
      status = 1;
    } else {
      no_color || print_color(green);
      quiet || printf("OK");
      no_color || print_color(light);
      duration == 0 || quiet || printf("   %d ms", duration);
      quiet || printf("\n");
      success_count++;
    }

    quiet || fflush(stdout);
  }

  no_color || print_color(white);
  quiet || printf("\n%d of %d tests passed.\n", success_count,
                  tests_total);
  quiet || printf("%d of %d assertions passed.\n\n",
                  total_assertion_count - fail_assertion_count,
                  total_assertion_count);

  no_color || print_color(light);

  if (!quiet && status != 0) {
    int have_reports = 0;

    for (i = 0; i < kit_tests_list.size && i < KIT_TESTS_SIZE_LIMIT;
         i++) {
      if (specific_test != NULL &&
          strstr(kit_tests_list.v[i].test_name, specific_test) ==
              NULL)
        continue;
      if (kit_tests_list.v[i].signal != 0) {
        int signum = kit_tests_list.v[i].signal;
        if (signum >= 0 &&
            signum < sizeof signames / sizeof *signames &&
            signames[signum] != NULL) {
          no_color || print_color(light);
          printf("Signal \"%s\" (%d) for \"", signames[signum],
                 signum);
          no_color || print_color(white);
          printf("%s", kit_tests_list.v[i].test_name);
          no_color || print_color(light);
          printf("\" in \"");
          no_color || print_color(white);

          printf("%s", kit_tests_list.v[i].test_file + file_root);
          no_color || print_color(light);
          printf("\"!.\n");
        } else {
          no_color || print_color(light);
          printf("Unknown signal (%d) for \"", signum);
          no_color || print_color(white);
          printf("%s", kit_tests_list.v[i].test_name);
          no_color || print_color(light);
          printf("\" in \"");
          no_color || print_color(white);

          printf("%s", kit_tests_list.v[i].test_file + file_root);
          no_color || print_color(light);
          printf("\"!.\n");
        }
        have_reports = 1;
      }
      if (kit_tests_list.v[i].assertions >
          KIT_TEST_ASSERTIONS_LIMIT) {
        no_color || print_color(light);
        printf("Too many assertions for \"");
        no_color || print_color(white);
        printf("%s", kit_tests_list.v[i].test_name);
        no_color || print_color(light);
        printf("\" in \"");
        no_color || print_color(white);

        printf("%s", kit_tests_list.v[i].test_file + file_root);
        no_color || print_color(light);
        printf("\"!.\n");
        have_reports = 1;
      }
    }

    have_reports &&printf("\n");
  }

  if (!quiet && status != 0) {
    for (i = 0; i < kit_tests_list.size && i < KIT_TESTS_SIZE_LIMIT;
         i++) {
      if (specific_test != NULL &&
          strstr(kit_tests_list.v[i].test_name, specific_test) ==
              NULL)
        continue;

      if (kit_tests_list.v[i].assertions <= KIT_TEST_ASSERTIONS_LIMIT)
        for (j = 0; j < kit_tests_list.v[i].assertions; j++)
          if (!kit_tests_list.v[i].status[j]) {
            no_color || print_color(light);
            printf("Assertion on line ");
            no_color || print_color(white);
            printf("%d", kit_tests_list.v[i].line[j]);
            no_color || print_color(light);
            printf(" in \"");
            no_color || print_color(white);
            printf("%s", kit_tests_list.v[i].test_file + file_root);
            no_color || print_color(light);
            printf("\" failed.\n");
            no_color || print_color(red);
            printf(" -> ");
            no_color || print_color(light);
            printf("Got wrong value");
            no_color || print_color(white);
            printf("%10lld", kit_tests_list.v[i].value[j]);
            no_color || print_color(light);
            printf("  (");
            no_color || print_color(white);
            printf("0x%08llx", kit_tests_list.v[i].value[j]);
            no_color || print_color(light);
            printf(")\n");
            no_color || print_color(green);
            printf(" -> ");
            no_color || print_color(light);
            printf("Expected value ");
            no_color || print_color(white);
            printf("%10lld", kit_tests_list.v[i].expected[j]);
            no_color || print_color(light);
            printf("  (");
            no_color || print_color(white);
            printf("0x%08llx", kit_tests_list.v[i].expected[j]);
            no_color || print_color(light);
            printf(")\n\n");
          }
    }
  }

  if (kit_tests_list.size > KIT_TESTS_SIZE_LIMIT) {
    no_color || print_color(light);
    quiet || printf("Too many tests!\n\n");
    status = 1;
  }

  if (status == 0) {
    no_color || print_color(green);
    quiet || printf("OK\n");
  } else {
    no_color || print_color(red);
    quiet || printf("FAILED\n");
  }

  no_color || print_color(white);
  quiet || printf("\n");
  return status;
}
