#ifndef KIT_TIME_H
#define KIT_TIME_H

#include <time.h>

#ifndef TIME_UTC
#  define TIME_UTC 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef KIT_NEED_STRUCT_TIMESPEC
#  ifdef KIT_HAVE_BITS_TYPES_STRUCT_TIMESPEC_H
#    include <bits/types/struct_timespec.h>
#  else
struct timespec {
  time_t tv_sec;  /* Seconds      - >= 0 */
  long   tv_nsec; /* Nanoseconds  - [0, 999999999] */
};
#  endif
#endif

#ifdef KIT_NEED_TIMESPEC_GET
int timespec_get(struct timespec *ts, int base);
#endif

#ifdef __cplusplus
}
#endif

#endif
