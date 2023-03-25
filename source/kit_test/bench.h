#ifndef KIT_BENCH_BENCH_H
#define KIT_BENCH_BENCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "test.h"
#include <stdint.h>

#ifndef KIT_BENCH_FILE
#  define KIT_BENCH_FILE kit_bench
#endif

#ifndef KIT_BENCHS_SIZE_LIMIT
#  define KIT_BENCHS_SIZE_LIMIT 0x1000
#endif

#ifndef KIT_BENCH_REPEATS
#  define KIT_BENCH_REPEATS 400
#endif

typedef void (*kit_bench_begin_fn)(int bench_index);
typedef void (*kit_bench_end_fn)(int bench_index);
typedef void (*kit_bench_run_fn)(
    int kit_bench_index_, kit_bench_begin_fn kit_bench_begin_fn_,
    kit_bench_end_fn kit_bench_end_fn_);

typedef struct {
  char const      *bench_name;
  char const      *bench_file;
  kit_bench_run_fn bench_fn;
  int64_t          sec[KIT_BENCH_REPEATS];
  int32_t          nsec[KIT_BENCH_REPEATS];
  int64_t          duration_nsec[KIT_BENCH_REPEATS];
  int              repeats;
  int              signal;
} kit_benchmark_t;

typedef struct {
  int             size;
  kit_benchmark_t benchs[KIT_BENCHS_SIZE_LIMIT];
} kit_benchs_list_t;

extern kit_benchs_list_t kit_benchs_list;

void kit_bench_register(char const *name, char const *file,
                        kit_bench_run_fn fn);

#define KIT_BENCHMARK(name)                                          \
  static void KIT_TEST_CONCAT3_(kit_bench_run_, __LINE__,            \
                                KIT_BENCH_FILE)(                     \
      int, kit_bench_begin_fn, kit_bench_end_fn);                    \
  KIT_TEST_ON_START_(                                                \
      KIT_TEST_CONCAT3_(kit_benchmark_, __LINE__, KIT_BENCH_FILE)) { \
    kit_bench_register(name, __FILE__,                               \
                       KIT_TEST_CONCAT3_(kit_bench_run_, __LINE__,   \
                                         KIT_BENCH_FILE));           \
  }                                                                  \
  static void KIT_TEST_CONCAT3_(kit_bench_run_, __LINE__,            \
                                KIT_BENCH_FILE)(                     \
      int kit_bench_index_, kit_bench_begin_fn kit_bench_begin_fn_,  \
      kit_bench_end_fn kit_bench_end_fn_)

#define KIT_BENCHMARK_BEGIN                                          \
  for (int kit_bench_repeat_ = 0;                                    \
       kit_bench_repeat_ < KIT_BENCH_REPEATS; kit_bench_repeat_++) { \
    kit_bench_begin_fn_(kit_bench_index_);                           \
    {

#define KIT_BENCHMARK_END              \
  }                                    \
  kit_bench_end_fn_(kit_bench_index_); \
  }

int kit_run_benchmarks(int argc, char **argv);

#ifndef KIT_DISABLE_SHORT_NAMES
#  define BENCHMARK KIT_BENCHMARK
#  define BENCHMARK_BEGIN KIT_BENCHMARK_BEGIN
#  define BENCHMARK_END KIT_BENCHMARK_END

#  define bench_register kit_bench_register
#  define run_benchmarks kit_run_benchmarks
#endif

#ifdef __cplusplus
}
#endif

#endif
