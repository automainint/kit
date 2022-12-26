#ifndef KIT_THREAD_H
#define KIT_THREAD_H

#ifndef KIT_DISABLE_SYSTEM_THREADS
#  include "time.h"
#  include <stddef.h>

#  if defined(__cplusplus)
#    define _Noreturn [[noreturn]]
#  elif defined(_MSC_VER)
#    define _Noreturn __declspec(noreturn)
#  endif

#  if !defined(_WIN32) || defined(__CYGWIN__)
#    include <pthread.h>
#  endif

#  ifndef _Thread_local
#    if defined(__cplusplus)
/* C++11 doesn't need `_Thread_local` keyword or macro */
#    elif !defined(__STDC_NO_THREADS__)
/* threads are optional in C11, _Thread_local present in this
 * condition */
#    elif defined(_MSC_VER)
#      define _Thread_local __declspec(thread)
#    elif defined(__GNUC__)
#      define _Thread_local __thread
#    else
/* Leave _Thread_local undefined so that use of _Thread_local would
 * not promote to a non-thread-local global variable
 */
#    endif
#  endif

#  if !defined(__cplusplus)
/*
 * C11 thread_local() macro
 * C++11 and above already have thread_local keyword
 */
#    ifndef thread_local
#      if _MSC_VER
#        define thread_local __declspec(thread)
#      else
#        define thread_local _Thread_local
#      endif
#    endif
#  endif

#  ifdef __cplusplus
extern "C" {
#  endif

typedef void (*tss_dtor_t)(void *);
typedef int (*thrd_start_t)(void *);

#  if defined(_WIN32) && !defined(__CYGWIN__)
typedef struct {
  void *handle;
} thrd_t;
typedef unsigned long tss_t;
#  else
typedef pthread_t     thrd_t;
typedef pthread_key_t tss_t;
#  endif

enum {
  thrd_success = 0,
  thrd_timedout,
  thrd_error,
  thrd_busy,
  thrd_nomem,
  thrd_wrong_stack_size
};

int            thrd_create(thrd_t *, thrd_start_t, void *);
int            thrd_create_with_stack(thrd_t *, thrd_start_t, void *,
                                      ptrdiff_t stack_size);
thrd_t         thrd_current(void);
int            thrd_detach(thrd_t);
int            thrd_equal(thrd_t, thrd_t);
_Noreturn void thrd_exit(int);
int            thrd_join(thrd_t, int *);
int            thrd_sleep(const struct timespec *, struct timespec *);
void           thrd_yield(void);

#  ifdef __cplusplus
}
#  endif

#endif

#endif
