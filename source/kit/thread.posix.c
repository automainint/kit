#ifndef KIT_DISABLE_SYSTEM_THREADS
#  if !defined(_WIN32) || defined(__CYGWIN__)

#    include <assert.h>
#    include <errno.h>
#    include <limits.h>
#    include <sched.h>
#    include <stdint.h> /* intptr_t */
#    include <stdlib.h>
#    include <unistd.h>

#    include "allocator.h"
#    include "condition_variable.h"
#    include "mutex.h"
#    include "thread.h"

/*
Configuration macro:

  EMULATED_THREADS_USE_NATIVE_TIMEDLOCK
    Use pthread_mutex_timedlock() for `mtx_timedlock()'
    Otherwise use mtx_trylock() + *busy loop* emulation.
*/
#    if !defined(__CYGWIN__) && !defined(__APPLE__) && \
        !defined(__NetBSD__)
#      define EMULATED_THREADS_USE_NATIVE_TIMEDLOCK
#    endif

/*
Implementation limits:
  - Conditionally emulation for "mutex with timeout"
    (see EMULATED_THREADS_USE_NATIVE_TIMEDLOCK macro)
*/
typedef struct {
  thrd_start_t    func;
  void           *arg;
  kit_allocator_t alloc;
} impl_thrd_param_t;

static void *impl_thrd_routine(void *p) {
  impl_thrd_param_t pack = *((impl_thrd_param_t *) p);
  pack.alloc.deallocate(pack.alloc.state, p);
  return (void *) (intptr_t) pack.func(pack.arg);
}

void call_once(once_flag *flag, void (*func)(void)) {
  pthread_once(flag, func);
}

int cnd_broadcast(cnd_t *cond) {
  assert(cond != NULL);
  return (pthread_cond_broadcast(cond) == 0) ? thrd_success
                                             : thrd_error;
}

void cnd_destroy(cnd_t *cond) {
  assert(cond);
  pthread_cond_destroy(cond);
}

int cnd_init(cnd_t *cond) {
  assert(cond != NULL);
  return (pthread_cond_init(cond, NULL) == 0) ? thrd_success
                                              : thrd_error;
}

int cnd_signal(cnd_t *cond) {
  assert(cond != NULL);
  return (pthread_cond_signal(cond) == 0) ? thrd_success : thrd_error;
}

int cnd_timedwait(cnd_t *cond, mtx_t *mtx,
                  const struct timespec *abs_time) {
  int rt;

  assert(mtx != NULL);
  assert(cond != NULL);
  assert(abs_time != NULL);

  rt = pthread_cond_timedwait(cond, mtx, abs_time);
  if (rt == ETIMEDOUT)
    return thrd_timedout;
  return (rt == 0) ? thrd_success : thrd_error;
}

int cnd_wait(cnd_t *cond, mtx_t *mtx) {
  assert(mtx != NULL);
  assert(cond != NULL);
  return (pthread_cond_wait(cond, mtx) == 0) ? thrd_success
                                             : thrd_error;
}

void mtx_destroy(mtx_t *mtx) {
  assert(mtx != NULL);
  pthread_mutex_destroy(mtx);
}

/*
 * XXX: Workaround when building with -O0 and without pthreads link.
 *
 * In such cases constant folding and dead code elimination won't be
 * available, thus the compiler will always add the pthread_mutexattr*
 * functions into the binary. As we try to link, we'll fail as the
 * symbols are unresolved.
 *
 * Ideally we'll enable the optimisations locally, yet that does not
 * seem to work.
 *
 * So the alternative workaround is to annotate the symbols as weak.
 * Thus the linker will be happy and things don't clash when building
 * with -O1 or greater.
 */
#    if defined(HAVE_FUNC_ATTRIBUTE_WEAK) && !defined(__CYGWIN__)
__attribute__((weak)) int pthread_mutexattr_init(
    pthread_mutexattr_t *attr);

__attribute__((weak)) int pthread_mutexattr_settype(
    pthread_mutexattr_t *attr, int type);

__attribute__((weak)) int pthread_mutexattr_destroy(
    pthread_mutexattr_t *attr);
#    endif

int mtx_init(mtx_t *mtx, int type) {
  pthread_mutexattr_t attr;
  assert(mtx != NULL);
  if (type != mtx_plain && type != mtx_timed &&
      type != (mtx_plain | mtx_recursive) &&
      type != (mtx_timed | mtx_recursive))
    return thrd_error;

  if ((type & mtx_recursive) == 0) {
    pthread_mutex_init(mtx, NULL);
    return thrd_success;
  }

  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(mtx, &attr);
  pthread_mutexattr_destroy(&attr);
  return thrd_success;
}

int mtx_lock(mtx_t *mtx) {
  assert(mtx != NULL);
  return (pthread_mutex_lock(mtx) == 0) ? thrd_success : thrd_error;
}

int mtx_timedlock(mtx_t *mtx, const struct timespec *ts) {
  assert(mtx != NULL);
  assert(ts != NULL);

  {
#    ifdef EMULATED_THREADS_USE_NATIVE_TIMEDLOCK
    int rt;
    rt = pthread_mutex_timedlock(mtx, ts);
    if (rt == 0)
      return thrd_success;
    return (rt == ETIMEDOUT) ? thrd_timedout : thrd_error;
#    else
    time_t expire = time(NULL);
    expire += ts->tv_sec;
    while (mtx_trylock(mtx) != thrd_success) {
      time_t now = time(NULL);
      if (expire < now)
        return thrd_timedout;
      // busy loop!
      thrd_yield();
    }
    return thrd_success;
#    endif
  }
}

int mtx_trylock(mtx_t *mtx) {
  assert(mtx != NULL);
  return (pthread_mutex_trylock(mtx) == 0) ? thrd_success : thrd_busy;
}

int mtx_unlock(mtx_t *mtx) {
  assert(mtx != NULL);
  return (pthread_mutex_unlock(mtx) == 0) ? thrd_success : thrd_error;
}

int thrd_create_with_stack(thrd_t *thr, thrd_start_t func, void *arg,
                           ptrdiff_t const require_stack_size) {
  impl_thrd_param_t *pack;
  assert(thr != NULL);
  assert(require_stack_size == 0 ||
         require_stack_size >= PTHREAD_STACK_MIN);
  pthread_attr_t  attr;
  pthread_attr_t *attr_p = NULL;
  if (require_stack_size > 0) {
    ptrdiff_t const page_size  = (ptrdiff_t) sysconf(_SC_PAGESIZE);
    ptrdiff_t const delta      = require_stack_size % page_size;
    ptrdiff_t const stack_size = delta == 0 ? require_stack_size
                                            : require_stack_size +
                                                  page_size - delta;
    if (pthread_attr_init(&attr) != 0)
      return thrd_nomem;
    if (pthread_attr_setstacksize(&attr, (size_t) stack_size) != 0)
      return thrd_wrong_stack_size;
    attr_p = &attr;
  }
  kit_allocator_t alloc = kit_alloc_default();
  pack                  = (impl_thrd_param_t *) alloc.allocate(
                       alloc.state, sizeof(impl_thrd_param_t));
  if (!pack) {
    if (attr_p)
      pthread_attr_destroy(attr_p);
    return thrd_nomem;
  }
  pack->func  = func;
  pack->arg   = arg;
  pack->alloc = alloc;
  if (pthread_create(thr, attr_p, impl_thrd_routine, pack) != 0) {
    alloc.deallocate(alloc.state, pack);
    if (attr_p)
      pthread_attr_destroy(attr_p);
    return thrd_error;
  }
  if (attr_p)
    pthread_attr_destroy(attr_p);
  return thrd_success;
}

int thrd_create(thrd_t *thr, thrd_start_t func, void *arg) {
  return thrd_create_with_stack(thr, func, arg, 0);
}
thrd_t thrd_current(void) {
  return pthread_self();
}

int thrd_detach(thrd_t thr) {
  return (pthread_detach(thr) == 0) ? thrd_success : thrd_error;
}

int thrd_equal(thrd_t thr0, thrd_t thr1) {
  return pthread_equal(thr0, thr1);
}

_Noreturn void thrd_exit(int res) {
  pthread_exit((void *) (intptr_t) res);
}

int thrd_join(thrd_t thr, int *res) {
  void *code;
  if (pthread_join(thr, &code) != 0)
    return thrd_error;
  if (res)
    *res = (int) (intptr_t) code;
  return thrd_success;
}

int thrd_sleep(const struct timespec *time_point,
               struct timespec       *remaining) {
  assert(time_point != NULL);
  return nanosleep(time_point, remaining);
}

void thrd_yield(void) {
  sched_yield();
}

#  endif
#endif
