#ifndef KIT_MUTEX_H
#define KIT_MUTEX_H

#ifndef KIT_DISABLE_SYSTEM_THREADS
#  include "time.h"

#  if !defined(_WIN32) || defined(__CYGWIN__)
#    include <pthread.h>
#  endif

#  ifdef __cplusplus
extern "C" {
#  endif

#  if defined(_WIN32) && !defined(__CYGWIN__)
typedef struct {
  void     *DebugInfo;
  long      LockCount;
  long      RecursionCount;
  void     *OwningThread;
  void     *LockSemaphore;
  uintptr_t SpinCount;
} mtx_t;
#  else
typedef pthread_mutex_t mtx_t;
#  endif

enum {
  mtx_plain     = 0,
  mtx_recursive = 1,
  mtx_timed     = 2,
};

void mtx_destroy(mtx_t *mtx_);
int  mtx_init(mtx_t *mtx_, int);
int  mtx_lock(mtx_t *mtx_);
int  mtx_timedlock(mtx_t *__restrict mtx_,
                   const struct timespec *__restrict);
int  mtx_trylock(mtx_t *mtx_);
int  mtx_unlock(mtx_t *mtx_);

#  ifdef __cplusplus
}
#  endif

#endif

#endif
