#include "time.h"

#ifdef KIT_NEED_TIMESPEC_GET

#  if defined(_WIN32) && !defined(__CYGWIN__)

#    ifndef WIN32_LEAN_AND_MEAN
#      define WIN32_LEAN_AND_MEAN 1
#    endif
#    include <windows.h>

int timespec_get(struct timespec *ts, int base) {
/*  difference between 1970 and 1601 */
#    define _TIMESPEC_IMPL_UNIX_EPOCH_IN_TICKS 116444736000000000ull
/*  1 tick is 100 nanoseconds */
#    define _TIMESPEC_IMPL_TICKS_PER_SECONDS 10000000ull
  if (ts == NULL)
    return 0;
  if (base == TIME_UTC) {
    FILETIME       ft;
    ULARGE_INTEGER date;
    LONGLONG       ticks;

    GetSystemTimeAsFileTime(&ft);
    date.HighPart = ft.dwHighDateTime;
    date.LowPart  = ft.dwLowDateTime;
    ticks         = (LONGLONG) (date.QuadPart -
                        _TIMESPEC_IMPL_UNIX_EPOCH_IN_TICKS);
    ts->tv_sec    = ticks / _TIMESPEC_IMPL_TICKS_PER_SECONDS;
    ts->tv_nsec   = (ticks % _TIMESPEC_IMPL_TICKS_PER_SECONDS) * 100;
    return base;
  }
  return 0;
#    undef _TIMESPEC_IMPL_UNIX_EPOCH_IN_TICKS
#    undef _TIMESPEC_IMPL_TICKS_PER_SECONDS
}

#  elif defined(KIT_HAVE_CLOCK_GETTIME)
int timespec_get(struct timespec *ts, int base) {
  if (ts == NULL)
    return 0;
  if (base == TIME_UTC) {
    clock_gettime(CLOCK_REALTIME, ts);
    return base;
  }
  return 0;
}
#  else
#    include <sys/time.h>

int timespec_get(struct timespec *ts, int base) {
  if (ts == NULL)
    return 0;
  if (base == TIME_UTC) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    ts->tv_sec  = tv.tv_sec;
    ts->tv_nsec = tv.tv_usec * 1000;
    return base;
  }
  return 0;
}
#  endif

#endif
