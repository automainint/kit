#include "test.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <setjmp.h>
#include <signal.h>

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

enum { white, blue, light, yellow, red, green };

char const *const color_codes[] = {
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
    kit_tests_list.tests[n].test_fn    = fn;
    kit_tests_list.tests[n].test_name  = name;
    kit_tests_list.tests[n].test_file  = file;
    kit_tests_list.tests[n].assertions = 0;
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
  int no_color              = 0;
  int line_width            = 20;
  int carriage_return       = 1;

  for (int i = 0; i < argc; i++)
    if (strcmp("--no-term-color", argv[i]) == 0)
      no_color = 1;
    else if (strcmp("--no-carriage-return", argv[i]) == 0)
      carriage_return = 0;
    else if (strcmp("--quiet", argv[i]) == 0)
      quiet = 1;

  if (quiet)
    no_color = 1;

  char const *file      = NULL;
  ptrdiff_t   file_root = -1;

  for (int i = 0; i < kit_tests_list.size && i < KIT_TESTS_SIZE_LIMIT;
       i++) {
    int const l = 2 + (int) strlen(kit_tests_list.tests[i].test_name);
    if (line_width < l)
      line_width = l;
  }

  if (kit_tests_list.size > 0) {
    char const *const s = kit_tests_list.tests[0].test_file;

    for (int j = 1;
         j < kit_tests_list.size && j < KIT_TESTS_SIZE_LIMIT; j++) {
      if (strcmp(s, kit_tests_list.tests[j].test_file) == 0)
        continue;
      int k = 0;
      for (; s[k] != '\0' &&
             kit_tests_list.tests[j].test_file[k] != '\0' &&
             s[k] == kit_tests_list.tests[j].test_file[k];
           k++) { }
      if (file_root == -1 || file_root > k)
        file_root = k;
    }

    if (file_root == -1) {
      for (int i = 0; s[i] != '\0'; i++)
        if (s[i] == '/' || s[i] == '\\')
          file_root = i + 1;
    }
  }

  for (int i = 0; i < kit_tests_list.size && i < KIT_TESTS_SIZE_LIMIT;
       i++) {
    if (file == NULL ||
        strcmp(file, kit_tests_list.tests[i].test_file) != 0) {
      if (file != NULL)
        quiet || printf("\n");
      file = kit_tests_list.tests[i].test_file;
      no_color || print_color(blue);
      quiet || printf("* ");
      no_color || print_color(white);
      quiet || printf("%s\n", file + file_root);
    }

    !carriage_return || no_color || print_color(yellow);
    carriage_return || no_color || print_color(light);
    quiet || printf("` %s ", kit_tests_list.tests[i].test_name);
    quiet || fflush(stdout);

    struct timespec begin, end;
    timespec_get(&begin, TIME_UTC);

    int test_status = run_test(i);

    timespec_get(&end, TIME_UTC);
    int duration = (int) (ns_to_ms(end.tv_nsec - begin.tv_nsec) +
                          sec_to_ms(end.tv_sec - begin.tv_sec));

    for (int j = 0; j < kit_tests_list.tests[i].assertions; j++)
      if (kit_tests_list.tests[i].status[j] == 0) {
        fail_assertion_count++;
        test_status = 0;
      }

    if (kit_tests_list.tests[i].assertions >
        KIT_TEST_ASSERTIONS_LIMIT)
      test_status = 0;

    total_assertion_count += kit_tests_list.tests[i].assertions;

    if (carriage_return) {
      no_color || print_color(light);
      quiet || printf("\r` %s ", kit_tests_list.tests[i].test_name);
    }

    int const l = (int) strlen(kit_tests_list.tests[i].test_name);
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
  }

  no_color || print_color(white);
  quiet || printf("\n%d of %d tests passed.\n", success_count,
                  kit_tests_list.size);
  quiet || printf("%d of %d assertions passed.\n\n",
                  total_assertion_count - fail_assertion_count,
                  total_assertion_count);

  no_color || print_color(light);

  if (status != 0) {
    for (int i = 0;
         i < kit_tests_list.size && i < KIT_TESTS_SIZE_LIMIT; i++) {
      if (kit_tests_list.tests[i].signal != 0) {
        int signum = kit_tests_list.tests[i].signal;
        if (signum >= 0 &&
            signum < sizeof signames / sizeof *signames &&
            signames[signum] != NULL) {
          no_color || print_color(light);
          quiet || printf("Signal \"%s\" (%d) for \"",
                          signames[signum], signum);
          no_color || print_color(white);
          quiet || printf("%s", kit_tests_list.tests[i].test_name);
          no_color || print_color(light);
          quiet || printf("\" in \"");
          no_color || print_color(white);
          quiet || printf("%s", kit_tests_list.tests[i].test_file +
                                    file_root);
          no_color || print_color(light);
          quiet || printf("\"!.\n");
        } else {
          no_color || print_color(light);
          quiet || printf("Unknown signal (%d) for \"", signum);
          no_color || print_color(white);
          quiet || printf("%s", kit_tests_list.tests[i].test_name);
          no_color || print_color(light);
          quiet || printf("\" in \"");
          no_color || print_color(white);
          quiet || printf("%s", kit_tests_list.tests[i].test_file +
                                    file_root);
          no_color || print_color(light);
          quiet || printf("\"!.\n");
        }
      }
      if (kit_tests_list.tests[i].assertions >
          KIT_TEST_ASSERTIONS_LIMIT) {
        no_color || print_color(light);
        quiet || printf("Too many assertions for \"");
        no_color || print_color(white);
        quiet || printf("%s", kit_tests_list.tests[i].test_name);
        no_color || print_color(light);
        quiet || printf("\" in \"");
        no_color || print_color(white);
        quiet || printf("%s", kit_tests_list.tests[i].test_file +
                                  file_root);
        no_color || print_color(light);
        quiet || printf("\"!.\n");
      } else
        for (int j = 0; j < kit_tests_list.tests[i].assertions; j++)
          if (!kit_tests_list.tests[i].status[j]) {
            no_color || print_color(light);
            quiet || printf("Assertion on line ");
            no_color || print_color(white);
            quiet || printf("%d", kit_tests_list.tests[i].line[j]);
            no_color || print_color(light);
            quiet || printf(" in \"");
            no_color || print_color(white);
            quiet || printf("%s", kit_tests_list.tests[i].test_file +
                                      file_root);
            no_color || print_color(light);
            quiet || printf("\" failed.\n");
          }
    }

    quiet || printf("\n");
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
