#include "bench.h"

#define _GNU_SOURCE
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

kit_benchs_list_t kit_benchs_list = { 0 };

static void bench_set_repeats_limit(int i, int repeats_limit) {
  if (kit_benchs_list.v[i].ready)
    return;
  if (kit_benchs_list.v[i].cycles_size >= KIT_BENCH_MAX_CYCLES)
    return;
  kit_benchs_list.v[i].cycles[kit_benchs_list.v[i].cycles_size] =
      repeats_limit;
  kit_benchs_list.v[i].cycles_size++;
}

static int bench_loop(int i) {
  if (!kit_benchs_list.v[i].ready)
    return 0;
  return kit_benchs_list.v[i].repeats <
         kit_benchs_list.v[i].cycles[kit_benchs_list.v[i].cycle];
}

static void bench_begin(int i) {
  int const n = kit_benchs_list.v[i].repeats++;

  if (n >= KIT_BENCH_MAX_REPEATS)
    return;

  struct timespec tv;
  timespec_get(&tv, TIME_UTC);

  kit_benchs_list.v[i].sec[n]  = (int64_t) tv.tv_sec;
  kit_benchs_list.v[i].nsec[n] = (int64_t) tv.tv_nsec;
}

static void bench_end(int i) {
  int const n = kit_benchs_list.v[i].repeats - 1;

  if (n < 0 || n >= KIT_BENCH_MAX_REPEATS)
    return;

  struct timespec tv;
  timespec_get(&tv, TIME_UTC);

  int64_t const sec = ((int64_t) tv.tv_sec) -
                      kit_benchs_list.v[i].sec[n];
  int64_t const nsec = ((int64_t) tv.tv_nsec) -
                       kit_benchs_list.v[i].nsec[n];

  kit_benchs_list.v[i].duration_nsec[n] = sec * 1000000000 + nsec;
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
    kit_benchmark_t *const bench = kit_benchs_list.v + n;

    bench->bench_fn    = fn;
    bench->bench_name  = name;
    bench->bench_file  = file;
    bench->cycles_size = 0;
    bench->ready       = 0;
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

static int run_bench(volatile int i) {
  int signum = setjmp(kit_bench_restore_execution);

  if (signum != 0) {
    kit_benchs_list.v[i].signal = signum;
    return 0;
  }

  kit_benchs_list.v[i].bench_fn(i, bench_set_repeats_limit,
                                bench_loop, bench_begin, bench_end);
  return 1;
}

int compare_64_(void const *x_, void const *y_) {
  int64_t const *x = (int64_t const *) x_;
  int64_t const *y = (int64_t const *) y_;
  return *x - *y;
}

int compare_32_(void const *x_, void const *y_) {
  int const *x = (int const *) x_;
  int const *y = (int const *) y_;
  return *x - *y;
}

int kit_run_benchmarks(int argc, char **argv) {
  int success_count   = 0;
  int status          = 0;
  int no_color        = 0;
  int line_width      = 20;
  int carriage_return = 1;

  char const *specific_bench = NULL;

  setup_signals();

  for (int i = 0; i < argc; i++)
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

  for (int i = 0;
       i < kit_benchs_list.size && i < KIT_BENCHS_SIZE_LIMIT; i++) {
    if (specific_bench != NULL &&
        strstr(kit_benchs_list.v[i].bench_name, specific_bench) ==
            NULL)
      continue;
    benchs_total++;
    int const l = 2 + (int) strlen(kit_benchs_list.v[i].bench_name);
    if (line_width < l)
      line_width = l;
  }

  if (benchs_total > 0) {
    char const *const s = kit_benchs_list.v[0].bench_file;

    for (int j = 1;
         j < kit_benchs_list.size && j < KIT_BENCHS_SIZE_LIMIT; j++) {
      kit_benchmark_t *const bench = kit_benchs_list.v + j;

      if (specific_bench != NULL &&
          strstr(bench->bench_name, specific_bench) == NULL)
        continue;
      if (strcmp(s, bench->bench_file) == 0)
        continue;
      int k = 0;
      for (; s[k] != '\0' && bench->bench_file[k] != '\0' &&
             s[k] == bench->bench_file[k];
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

  /*  Prepare cycles.
   */

  for (int i = 0;
       i < kit_benchs_list.size && i < KIT_BENCHS_SIZE_LIMIT; i++) {
    kit_benchmark_t *const bench = kit_benchs_list.v + i;

    if (specific_bench != NULL &&
        strstr(bench->bench_name, specific_bench) == NULL)
      continue;

    run_bench(i);

    if (bench->cycles_size == 0) {
      bench->cycles_size = 2;
      bench->cycles[0]   = KIT_BENCH_REPEATS_DEFAULT_1;
      bench->cycles[1]   = KIT_BENCH_REPEATS_DEFAULT_2;
    }

    qsort(bench->cycles, bench->cycles_size, sizeof *bench->cycles,
          compare_32_);

    kit_benchs_list.v[i].ready = 1;
  }

  /*  Run cycles.
   */

  for (int cycle = 0; cycle < KIT_BENCH_MAX_CYCLES; cycle++) {
    /*  Prepare cycle.
     */

    int cycles_done = 1;

    for (int i = 0;
         i < kit_benchs_list.size && i < KIT_BENCHS_SIZE_LIMIT; i++) {
      kit_benchmark_t *const bench = kit_benchs_list.v + i;

      if (specific_bench != NULL &&
          strstr(bench->bench_name, specific_bench) == NULL)
        continue;
      if (cycle >= bench->cycles_size)
        continue;

      bench->repeats = 0;
      bench->cycle   = cycle;
      cycles_done    = 0;
    }

    if (cycles_done)
      break;

    /*  Run benchmarks.
     */

    for (int i = 0;
         i < kit_benchs_list.size && i < KIT_BENCHS_SIZE_LIMIT; i++) {
      kit_benchmark_t *const bench = kit_benchs_list.v + i;

      if (specific_bench != NULL &&
          strstr(bench->bench_name, specific_bench) == NULL)
        continue;
      if (cycle >= bench->cycles_size)
        continue;

      if (file == NULL || strcmp(file, bench->bench_file) != 0) {
        if (file != NULL)
          printf("\n");
        file = bench->bench_file;
        no_color || print_color(blue);
        printf("* ");
        no_color || print_color(white);
        printf("%s\n", file + file_root);
      }

      !carriage_return || no_color || print_color(yellow);
      carriage_return || no_color || print_color(light);
      printf("` %s ", bench->bench_name);
      !carriage_return || printf("\r");
      fflush(stdout);

      int bench_status = run_bench(i);

      if (bench->repeats > KIT_BENCH_MAX_REPEATS)
        bench_status = 0;

      !carriage_return || no_color || print_color(light);
      !carriage_return || printf("` %s ", bench->bench_name);

      int const l = (int) strlen(bench->bench_name);
      printf("%*c", line_width - l, ' ');

      if (bench->repeats <= 0) {
        no_color || print_color(yellow);
        printf("                             0 runs\n");
        success_count++;
      } else if (bench_status == 0) {
        no_color || print_color(red);
        printf("                             FAIL\n");
        status = 1;
      } else {
        int const repeats = bench->repeats;

        memcpy(bench->duration_sorted_nsec, bench->duration_nsec,
               repeats * sizeof *bench->duration_sorted_nsec);
        qsort(bench->duration_sorted_nsec, repeats,
              sizeof *bench->duration_sorted_nsec, compare_64_);

        int64_t const average =
            bench->duration_sorted_nsec[repeats / 2];
        int64_t const floor =
            bench->duration_sorted_nsec[repeats / 20];
        int64_t const roof =
            bench->duration_sorted_nsec[repeats - repeats / 20 - 1];

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
  }

  printf("\n");

  if (status != 0) {
    for (int i = 0;
         i < kit_benchs_list.size && i < KIT_BENCHS_SIZE_LIMIT; i++) {
      kit_benchmark_t *const bench = kit_benchs_list.v + i;

      if (specific_bench != NULL &&
          strstr(bench->bench_name, specific_bench) == NULL)
        continue;
      if (bench->signal != 0) {
        int signum = bench->signal;
        if (signum >= 0 &&
            signum < sizeof signames / sizeof *signames &&
            signames[signum] != NULL) {
          no_color || print_color(light);
          printf("Signal \"%s\" (%d) for \"", signames[signum],
                 signum);
          no_color || print_color(white);
          printf("%s", bench->bench_name);
          no_color || print_color(light);
          printf("\" in \"");
          no_color || print_color(white);
          printf("%s", bench->bench_file + file_root);
          no_color || print_color(light);
          printf("\"!.\n");
        } else {
          no_color || print_color(light);
          printf("Unknown signal (%d) for \"", signum);
          no_color || print_color(white);
          printf("%s", bench->bench_name);
          no_color || print_color(light);
          printf("\" in \"");
          no_color || print_color(white);
          printf("%s", bench->bench_file + file_root);
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
