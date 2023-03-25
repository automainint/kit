#include "bench.h"

#define _GNU_SOURCE
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

kit_benchs_list_t kit_benchs_list = { 0 };

static void bench_begin(int i) {
  int const n = kit_benchs_list.benchs[i].repeats++;

  if (n >= KIT_BENCH_REPEATS)
    return;

  struct timespec tv;
  timespec_get(&tv, TIME_UTC);

  kit_benchs_list.benchs[i].sec[n]  = (int64_t) tv.tv_sec;
  kit_benchs_list.benchs[i].nsec[n] = (int64_t) tv.tv_nsec;
}

static void bench_end(int i) {
  int const n = kit_benchs_list.benchs[i].repeats - 1;

  if (n < 0 || n >= KIT_BENCH_REPEATS)
    return;

  struct timespec tv;
  timespec_get(&tv, TIME_UTC);

  int64_t const sec = ((int64_t) tv.tv_sec) -
                      kit_benchs_list.benchs[i].sec[n];
  int64_t const nsec = ((int64_t) tv.tv_nsec) -
                       kit_benchs_list.benchs[i].nsec[n];

  kit_benchs_list.benchs[i].duration_nsec[n] = sec * 1000000000 +
                                               nsec;
}

enum { white, blue, light, yellow, red, green };

static char const *const color_codes[] = {
  [white] = "\x1b[38m",  [blue] = "\x1b[34m", [light] = "\x1b[37m",
  [yellow] = "\x1b[33m", [red] = "\x1b[31m",  [green] = "\x1b[32m"
};

static int print_color(int c) {
  return printf("%s", color_codes[c]);
}

void kit_bench_register(char const *name, char const *file,
                        kit_bench_run_fn fn) {
  int n = kit_benchs_list.size++;
  if (n < KIT_BENCHS_SIZE_LIMIT) {
    kit_benchs_list.benchs[n].bench_fn   = fn;
    kit_benchs_list.benchs[n].bench_name = name;
    kit_benchs_list.benchs[n].bench_file = file;
    kit_benchs_list.benchs[n].repeats    = 0;
  }
}

static jmp_buf kit_bench_restore_execution;

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
  longjmp(kit_bench_restore_execution, signum);
}

static void setup_signals() {
  int i;

  for (i = 0; i < sizeof signums / sizeof *signums; i++) {
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

static int run_bench(volatile int i) {
  int signum = setjmp(kit_bench_restore_execution);

  if (signum != 0) {
    kit_benchs_list.benchs[i].signal = signum;
    return 0;
  }

  kit_benchs_list.benchs[i].bench_fn(i, bench_begin, bench_end);
  return 1;
}

int compare_64_(void const *x_, void const *y_) {
  int64_t const *x = (int64_t const *) x_;
  int64_t const *y = (int64_t const *) y_;
  return *x - *y;
}

int kit_run_benchmarks(int argc, char **argv) {
  int success_count   = 0;
  int status          = 0;
  int no_color        = 0;
  int line_width      = 20;
  int carriage_return = 1;

  int i, j;

  char const *specific_bench = NULL;

  setup_signals();

  for (i = 0; i < argc; i++)
    if (strcmp("--no-term-color", argv[i]) == 0)
      no_color = 1;
    else if (strcmp("--no-carriage-return", argv[i]) == 0)
      carriage_return = 0;
    else if (strcmp("--match", argv[i]) == 0)
      specific_bench = argv[++i];

  if (specific_bench != NULL) {
    no_color || print_color(light);
    printf("Run benchmarks matching ");
    no_color || print_color(white);
    printf("*%s*", specific_bench);
    no_color || print_color(light);
    printf("\n\n");
  }

  char const *file         = NULL;
  ptrdiff_t   file_root    = -1;
  int         benchs_total = 0;

  for (i = 0; i < kit_benchs_list.size && i < KIT_BENCHS_SIZE_LIMIT;
       i++) {
    if (specific_bench != NULL &&
        strstr(kit_benchs_list.benchs[i].bench_name,
               specific_bench) == NULL)
      continue;
    benchs_total++;
    int const l = 2 +
                  (int) strlen(kit_benchs_list.benchs[i].bench_name);
    if (line_width < l)
      line_width = l;
  }

  if (benchs_total > 0) {
    char const *const s = kit_benchs_list.benchs[0].bench_file;

    for (j = 1; j < kit_benchs_list.size && j < KIT_BENCHS_SIZE_LIMIT;
         j++) {
      if (specific_bench != NULL &&
          strstr(kit_benchs_list.benchs[j].bench_name,
                 specific_bench) == NULL)
        continue;
      if (strcmp(s, kit_benchs_list.benchs[j].bench_file) == 0)
        continue;
      int k = 0;
      for (; s[k] != '\0' &&
             kit_benchs_list.benchs[j].bench_file[k] != '\0' &&
             s[k] == kit_benchs_list.benchs[j].bench_file[k];
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

  no_color || print_color(blue);
  printf("# ");
  no_color || print_color(light);
  printf("BENCHMARK");
  printf("%*c", line_width - 9, ' ');
  no_color || print_color(green);
  printf(" LOW     ");
  no_color || print_color(light);
  printf("|");
  no_color || print_color(blue);
  printf(" MEDIAN  ");
  no_color || print_color(light);
  printf("|");
  no_color || print_color(yellow);
  printf(" HIGH    ");
  no_color || print_color(light);
  printf(" in nanoseconds\n\n");

  for (i = 0; i < kit_benchs_list.size && i < KIT_BENCHS_SIZE_LIMIT;
       i++) {
    if (specific_bench != NULL &&
        strstr(kit_benchs_list.benchs[i].bench_name,
               specific_bench) == NULL)
      continue;
    if (file == NULL ||
        strcmp(file, kit_benchs_list.benchs[i].bench_file) != 0) {
      if (file != NULL)
        printf("\n");
      file = kit_benchs_list.benchs[i].bench_file;
      no_color || print_color(blue);
      printf("* ");
      no_color || print_color(white);
      printf("%s\n", file + file_root);
    }

    !carriage_return || no_color || print_color(yellow);
    carriage_return || no_color || print_color(light);
    printf("` %s ", kit_benchs_list.benchs[i].bench_name);
    !carriage_return || printf("\r");
    fflush(stdout);

    int bench_status = run_bench(i);

    if (kit_benchs_list.benchs[i].repeats > KIT_BENCH_REPEATS)
      bench_status = 0;

    !carriage_return || no_color || print_color(light);
    !carriage_return ||
        printf("` %s ", kit_benchs_list.benchs[i].bench_name);

    int const l = (int) strlen(kit_benchs_list.benchs[i].bench_name);
    printf("%*c", line_width - l, ' ');

    if (kit_benchs_list.benchs[i].repeats <= 0) {
      no_color || print_color(yellow);
      printf("                             0 runs\n");
      success_count++;
    } else if (bench_status == 0) {
      no_color || print_color(red);
      printf("                             FAIL\n");
      status = 1;
    } else {
      int const repeats = kit_benchs_list.benchs[i].repeats;

      qsort(kit_benchs_list.benchs[i].duration_nsec, repeats,
            sizeof *kit_benchs_list.benchs[i].duration_nsec,
            compare_64_);

      int64_t const average =
          kit_benchs_list.benchs[i].duration_nsec[repeats / 2];
      int64_t const floor =
          kit_benchs_list.benchs[i].duration_nsec[repeats / 10];
      int64_t const roof =
          kit_benchs_list.benchs[i]
              .duration_nsec[repeats - repeats / 10 - 1];

      no_color || print_color(white);
      printf("%-8lld", (long long) floor);
      no_color || print_color(light);
      printf("| ");
      no_color || print_color(white);
      printf("%-8lld", (long long) average);
      no_color || print_color(light);
      printf("| ");
      no_color || print_color(white);
      printf("%-8lld", (long long) roof);
      no_color || print_color(light);
      printf(" %d runs\n", repeats);
      success_count++;
    }
  }

  printf("\n");

  if (status != 0) {
    for (i = 0; i < kit_benchs_list.size && i < KIT_BENCHS_SIZE_LIMIT;
         i++) {
      if (specific_bench != NULL &&
          strstr(kit_benchs_list.benchs[i].bench_name,
                 specific_bench) == NULL)
        continue;
      if (kit_benchs_list.benchs[i].signal != 0) {
        int signum = kit_benchs_list.benchs[i].signal;
        if (signum >= 0 &&
            signum < sizeof signames / sizeof *signames &&
            signames[signum] != NULL) {
          no_color || print_color(light);
          printf("Signal \"%s\" (%d) for \"", signames[signum],
                 signum);
          no_color || print_color(white);
          printf("%s", kit_benchs_list.benchs[i].bench_name);
          no_color || print_color(light);
          printf("\" in \"");
          no_color || print_color(white);
          printf("%s",
                 kit_benchs_list.benchs[i].bench_file + file_root);
          no_color || print_color(light);
          printf("\"!.\n");
        } else {
          no_color || print_color(light);
          printf("Unknown signal (%d) for \"", signum);
          no_color || print_color(white);
          printf("%s", kit_benchs_list.benchs[i].bench_name);
          no_color || print_color(light);
          printf("\" in \"");
          no_color || print_color(white);
          printf("%s",
                 kit_benchs_list.benchs[i].bench_file + file_root);
          no_color || print_color(light);
          printf("\"!.\n");
        }
      }
    }

    printf("\n");
  }

  if (kit_benchs_list.size > KIT_BENCHS_SIZE_LIMIT) {
    no_color || print_color(light);
    printf("Too many benchmarks!\n\n");
    status = 1;
  }

  if (status == 0) {
    no_color || print_color(green);
    printf("DONE\n");
  } else {
    no_color || print_color(red);
    printf("DONE WITH ERRORS\n");
  }

  no_color || print_color(white);
  printf("\n");
  return status;
}
