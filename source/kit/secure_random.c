#include "secure_random.h"

#include "mersenne_twister_64.h"
#include "time.h"
#include <stdio.h>

#if defined(_WIN32) && !defined(__CYGWIN__)
#  include <Windows.h>
#else
#  include <unistd.h>
#endif

static uint64_t get_available_memory() {
#if defined(_WIN32) && !defined(__CYGWIN__)
  MEMORYSTATUSEX status;
  status.dwLength = sizeof(status);
  GlobalMemoryStatusEx(&status);
  return (uint64_t) status.ullTotalPhys;
#else
  uint64_t pages     = (uint64_t) sysconf(_SC_PHYS_PAGES);
  uint64_t page_size = (uint64_t) sysconf(_SC_PAGE_SIZE);
  return pages * page_size;
#endif
}

static void secure_random_fallback(ptrdiff_t const size,
                                   void *const     data) {
  /*  Try to get some unpredictable system properties and use them to
   *  seed the pseudo random number generator.
   */

  static uint64_t n         = 0;
  static uint64_t time_sec  = 0;
  static uint64_t time_nsec = 0;

  struct timespec t;
  timespec_get(&t, TIME_UTC);

  kit_mt64_state_t state;

  if (time_sec == 0 && time_nsec == 0) {
    uint64_t seed[4] = { n++, get_available_memory(),
                         (uint64_t) t.tv_sec, (uint64_t) t.tv_nsec };
    kit_mt64_init_array(&state, sizeof seed / sizeof *seed, seed);
  } else {
    uint64_t seed[6] = { n++,
                         get_available_memory(),
                         (uint64_t) t.tv_sec,
                         (uint64_t) t.tv_nsec,
                         (uint64_t) t.tv_sec - time_sec,
                         (uint64_t) t.tv_nsec - time_nsec };
    kit_mt64_init_array(&state, sizeof seed / sizeof *seed, seed);
  }

  time_sec  = (uint64_t) t.tv_sec;
  time_nsec = (uint64_t) t.tv_nsec;

  /*  Bootstrap the generator.
   */
  for (ptrdiff_t i = 0; i < KIT_MT64_N; i++)
    kit_mt64_generate(&state);

  for (ptrdiff_t i = 0; i < size; i++)
    ((uint8_t *) data)[i] = (uint8_t) (kit_mt64_generate(&state) >>
                                       56);
}

void kit_secure_random(ptrdiff_t const size, void *const data) {
  if (size <= 0)
    return;

#if defined(_WIN32) && !defined(__CYGWIN__)
  secure_random_fallback(size, data);
#else
  FILE *f = fopen("/dev/urandom", "rb");

  if (f == NULL) {
    secure_random_fallback(size, data);
    return;
  }

  if (fread(data, 1, size, f) != size) {
    secure_random_fallback(size, data);
    fclose(f);
    return;
  }

  fclose(f);
  return;
#endif
}
