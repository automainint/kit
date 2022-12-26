#ifndef KIT_CONDITION_VARIABLE_H
#define KIT_CONDITION_VARIABLE_H

#ifndef KIT_DISABLE_SYSTEM_THREADS
#  include "mutex.h"

#  ifdef __cplusplus
extern "C" {
#  endif

#  if defined(_WIN32) && !defined(__CYGWIN__)
typedef struct {
  void *Ptr;
} cnd_t;

typedef struct {
  volatile uintptr_t status;
} once_flag;
#  else
typedef pthread_cond_t cnd_t;
typedef pthread_once_t once_flag;
#  endif

void call_once(once_flag *, void (*)(void));
int  cnd_broadcast(cnd_t *);
void cnd_destroy(cnd_t *);
int  cnd_init(cnd_t *);
int  cnd_signal(cnd_t *);
int  cnd_timedwait(cnd_t *__restrict, mtx_t *__restrict mtx_,
                   const struct timespec *__restrict);
int  cnd_wait(cnd_t *, mtx_t *mtx_);

#  ifdef __cplusplus
}
#  endif

#endif

#endif
