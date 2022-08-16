/*
 * Copyright 2022 Yonggang Luo
 * SPDX-License-Identifier: MIT
 *
 * C11 <time.h> emulation library
 */

#ifndef KIT_C11_TIME_H_INCLUDED_
#define KIT_C11_TIME_H_INCLUDED_

#include <time.h>

/*---------------------------- macros ---------------------------*/

#ifndef TIME_UTC
#  define TIME_UTC 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------- types ----------------------------*/

/*
 * On MINGW `struct timespec` present but `timespec_get` may not
 * present; On MSVC `struct timespec` and `timespec_get` present at
 * the same time; So detecting `HAVE_STRUCT_TIMESPEC` in meson script
 * dynamically.
 */
#ifndef KIT_HAVE_STRUCT_TIMESPEC
struct timespec {
  time_t tv_sec;  // Seconds - >= 0
  long   tv_nsec; // Nanoseconds - [0, 999999999]
};
#endif

/*-------------------------- functions --------------------------*/

#if !defined(KIT_HAVE_TIMESPEC_GET)
#  define KIT_HAVE_TIMESPEC_GET_NEED_DECL_
#elif defined(__APPLE__) && defined(__cplusplus) && \
    (__cplusplus < 201703L)
/* On macOS, the guard for declaration of timespec_get is by
 * (defined(__cplusplus) && __cplusplus >= 201703L),
 * fix the declaration for C++14 and lower here
 */
#  define KIT_HAVE_TIMESPEC_GET_NEED_DECL_
#endif

#ifdef KIT_HAVE_TIMESPEC_GET_NEED_DECL_
/*-------------------- 7.25.7 Time functions --------------------*/
// 7.25.6.1
int timespec_get(struct timespec *ts, int base);
#  undef KIT_HAVE_TIMESPEC_GET_NEED_DECL_
#endif

#ifdef __cplusplus
}
#endif

#endif /* KIT_C11_TIME_H_INCLUDED_ */
