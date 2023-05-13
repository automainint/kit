#ifndef KIT_BENCH_BENCH_H
#define KIT_BENCH_BENCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "test.h"
#include <stdint.h>

#ifndef KIT_TEST_FILE
#  define KIT_TEST_FILE kit_bench
#endif

#ifndef KIT_BENCHS_SIZE_LIMIT
#  define KIT_BENCHS_SIZE_LIMIT 200
#endif

#ifndef KIT_BENCH_MAX_REPEATS
#  define KIT_BENCH_MAX_REPEATS 4000
#endif

#ifndef KIT_BENCH_MAX_CYCLES
#  define KIT_BENCH_MAX_CYCLES 40
#endif

#ifndef KIT_BENCH_REPEATS_DEFAULT_1
#  define KIT_BENCH_REPEATS_DEFAULT_1 40
#endif

#ifndef KIT_BENCH_REPEATS_DEFAULT_2
#  define KIT_BENCH_REPEATS_DEFAULT_2 400
#endif

typedef void (*kit_bench_set_repeats_limit_fn)(int bench_index,
                                               int repeats_limit);
typedef int (*kit_bench_loop_fn)(int bench_index);
typedef void (*kit_bench_begin_fn)(int bench_index);
typedef void (*kit_bench_end_fn)(int bench_index);

typedef void (*kit_bench_run_fn)(
    int                            kit_bench_index_,
    kit_bench_set_repeats_limit_fn kit_bench_set_repeats_limit_,
    kit_bench_loop_fn              kit_bench_loop_,
    kit_bench_begin_fn             kit_bench_begin_,
    kit_bench_end_fn               kit_bench_end_);

typedef struct {
  char const      *bench_name;
  char const      *bench_file;
  kit_bench_run_fn bench_fn;
  int64_t          sec[KIT_BENCH_MAX_REPEATS];
  int32_t          nsec[KIT_BENCH_MAX_REPEATS];
  int64_t          duration_nsec[KIT_BENCH_MAX_REPEATS];
  int64_t          duration_sorted_nsec[KIT_BENCH_MAX_REPEATS];
  int              repeats;
  int              cycles_size;
  int              cycles[KIT_BENCH_MAX_CYCLES];
  int              cycle;
  int              signal;
  int              ready;
} kit_benchmark_t;

typedef struct {
  int             size;
  kit_benchmark_t v[KIT_BENCHS_SIZE_LIMIT];
} kit_benchs_list_t;

extern kit_benchs_list_t kit_benchs_list;

void kit_bench_register(char const *name, char const *file,
                        kit_bench_run_fn fn);

#define KIT_BENCHMARK(name)                                          \
  static void KIT_TEST_CONCAT3_(kit_bench_run_, __LINE__,            \
                                KIT_TEST_FILE)(                      \
      int, kit_bench_set_repeats_limit_fn, kit_bench_loop_fn,        \
      kit_bench_begin_fn, kit_bench_end_fn);                         \
  KIT_TEST_ON_START_(                                                \
      KIT_TEST_CONCAT3_(kit_benchmark_, __LINE__, KIT_TEST_FILE)) {  \
    kit_bench_register(                                              \
        name, __FILE__,                                              \
        KIT_TEST_CONCAT3_(kit_bench_run_, __LINE__, KIT_TEST_FILE)); \
  }                                                                  \
  static void KIT_TEST_CONCAT3_(kit_bench_run_, __LINE__,            \
                                KIT_TEST_FILE)(                      \
      int                            kit_bench_index_,               \
      kit_bench_set_repeats_limit_fn kit_bench_set_repeats_limit_,   \
      kit_bench_loop_fn              kit_bench_loop_,                \
      kit_bench_begin_fn             kit_bench_begin_,               \
      kit_bench_end_fn               kit_bench_end_)

#define KIT_BENCHMARK_REPEAT(repeats_limit_) \
  kit_bench_set_repeats_limit_(kit_bench_index_, repeats_limit_)

#define KIT_BENCHMARK_BEGIN                   \
  while (kit_bench_loop_(kit_bench_index_)) { \
    kit_bench_begin_(kit_bench_index_);       \
    {

#define KIT_BENCHMARK_END           \
  }                                 \
  kit_bench_end_(kit_bench_index_); \
  }

/*  FIXME
 */
#define KIT_DO_NOT_OPTIMIZE(x)        \
  do {                                \
    volatile void *bench_ptr_ = &(x); \
    (void) bench_ptr_;                \
  } while (0)

int kit_run_benchmarks(int argc, char **argv);

#ifndef KIT_DISABLE_SHORT_NAMES
#  define BENCHMARK KIT_BENCHMARK
#  define BENCHMARK_REPEAT KIT_BENCHMARK_REPEAT
#  define BENCHMARK_BEGIN KIT_BENCHMARK_BEGIN
#  define BENCHMARK_END KIT_BENCHMARK_END
#  define DO_NOT_OPTIMIZE KIT_DO_NOT_OPTIMIZE

#  define bench_register kit_bench_register
#  define run_benchmarks kit_run_benchmarks
#endif

#ifdef __cplusplus
}
#endif

#endif
