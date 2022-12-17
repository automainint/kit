#ifndef KIT_DISABLE_SYSTEM_THREADS
#  if defined(_WIN32) && !defined(__CYGWIN__)

#    include <assert.h>
#    include <errno.h>
#    include <limits.h>
#    include <process.h>
#    include <stdbool.h>
#    include <stdlib.h>

#    include "allocator.h"
#    include "condition_variable.h"
#    include "mutex.h"
#    include "threads.h"

#    ifndef WIN32_LEAN_AND_MEAN
#      define WIN32_LEAN_AND_MEAN 1
#    endif
#    include <Windows.h>

/*
Configuration macro:

  EMULATED_THREADS_USE_NATIVE_CALL_ONCE
    Use native WindowsAPI one-time initialization function.
    (requires WinVista or later)
    Otherwise emulate by mtx_trylock() + *busy loop* for WinXP.

  EMULATED_THREADS_TSS_DTOR_SLOTNUM
    Max registerable TSS dtor number.
*/

#    if _WIN32_WINNT >= 0x0600
/* Prefer native WindowsAPI on newer environment. */
#      if !defined(__MINGW32__)
#        define EMULATED_THREADS_USE_NATIVE_CALL_ONCE
#      endif
#    endif
#    define EMULATED_THREADS_TSS_DTOR_SLOTNUM \
      64 /* see TLS_MINIMUM_AVAILABLE */

/* check configuration */
#    if defined(EMULATED_THREADS_USE_NATIVE_CALL_ONCE) && \
        (_WIN32_WINNT < 0x0600)
#      error EMULATED_THREADS_USE_NATIVE_CALL_ONCE requires _WIN32_WINNT>=0x0600
#    endif

static_assert(sizeof(cnd_t) == sizeof(CONDITION_VARIABLE),
              "The size of cnd_t must equal to CONDITION_VARIABLE");
static_assert(sizeof(thrd_t) == sizeof(HANDLE),
              "The size of thrd_t must equal to HANDLE");
static_assert(sizeof(tss_t) == sizeof(DWORD),
              "The size of tss_t must equal to DWORD");
static_assert(sizeof(mtx_t) == sizeof(CRITICAL_SECTION),
              "The size of mtx_t must equal to CRITICAL_SECTION");
static_assert(sizeof(once_flag) == sizeof(INIT_ONCE),
              "The size of once_flag must equal to INIT_ONCE");

/*
Implementation limits:
  - Conditionally emulation for "Initialization functions"
    (see EMULATED_THREADS_USE_NATIVE_CALL_ONCE macro)
  - Emulated `mtx_timelock()' with mtx_trylock() + *busy loop*
*/

typedef struct {
  thrd_start_t    func;
  void           *arg;
  thrd_t          thrd;
  kit_allocator_t alloc;
} impl_thrd_param_t;

struct thrd_state {
  thrd_t thrd;
  bool   handle_need_close;
};

static thread_local struct thrd_state impl_current_thread = { 0 };

static unsigned __stdcall impl_thrd_routine(void *p) {
  impl_thrd_param_t *pack_p = (impl_thrd_param_t *) p;
  impl_thrd_param_t  pack;
  int                code;
  impl_current_thread.thrd              = pack_p->thrd;
  impl_current_thread.handle_need_close = false;
  memcpy(&pack, pack_p, sizeof(impl_thrd_param_t));
  pack.alloc.deallocate(pack.alloc.state, p);
  code = pack.func(pack.arg);
  return (unsigned) code;
}

static time_t impl_timespec2msec(const struct timespec *ts) {
  return (ts->tv_sec * 1000U) + (ts->tv_nsec / 1000000L);
}

static DWORD impl_abs2relmsec(const struct timespec *abs_time) {
  const time_t    abs_ms = impl_timespec2msec(abs_time);
  struct timespec now;
  timespec_get(&now, TIME_UTC);
  const time_t now_ms = impl_timespec2msec(&now);
  const DWORD  rel_ms = (abs_ms > now_ms) ? (DWORD) (abs_ms - now_ms)
                                          : 0;
  return rel_ms;
}

#    ifdef EMULATED_THREADS_USE_NATIVE_CALL_ONCE
struct impl_call_once_param {
  void (*func)(void);
};

static BOOL CALLBACK impl_call_once_callback(PINIT_ONCE InitOnce,
                                             PVOID      Parameter,
                                             PVOID     *Context) {
  struct impl_call_once_param *param = (struct impl_call_once_param *)
      Parameter;
  (param->func)();
  ((void) InitOnce);
  ((void) Context); /* suppress warning */
  return TRUE;
}
#    endif /* ifdef EMULATED_THREADS_USE_NATIVE_CALL_ONCE */

static struct impl_tss_dtor_entry {
  tss_t      key;
  tss_dtor_t dtor;
} impl_tss_dtor_tbl[EMULATED_THREADS_TSS_DTOR_SLOTNUM];

static int impl_tss_dtor_register(tss_t key, tss_dtor_t dtor) {
  int i;
  for (i = 0; i < EMULATED_THREADS_TSS_DTOR_SLOTNUM; i++) {
    if (!impl_tss_dtor_tbl[i].dtor)
      break;
  }
  if (i == EMULATED_THREADS_TSS_DTOR_SLOTNUM)
    return 1;
  impl_tss_dtor_tbl[i].key  = key;
  impl_tss_dtor_tbl[i].dtor = dtor;
  return 0;
}

static void impl_tss_dtor_invoke(void) {
  int i;
  for (i = 0; i < EMULATED_THREADS_TSS_DTOR_SLOTNUM; i++) {
    if (impl_tss_dtor_tbl[i].dtor) {
      void *val = tss_get(impl_tss_dtor_tbl[i].key);
      if (val)
        (impl_tss_dtor_tbl[i].dtor)(val);
    }
  }
}

void call_once(once_flag *flag, void (*func)(void)) {
  assert(flag && func);
#    ifdef EMULATED_THREADS_USE_NATIVE_CALL_ONCE
  {
    struct impl_call_once_param param;
    param.func = func;
    InitOnceExecuteOnce((PINIT_ONCE) flag, impl_call_once_callback,
                        (PVOID) &param, NULL);
  }
#    else
  if (InterlockedCompareExchangePointer(
          (PVOID volatile *) &flag->status, (PVOID) 1, (PVOID) 0) ==
      0) {
    (func)();
    InterlockedExchangePointer((PVOID volatile *) &flag->status,
                               (PVOID) 2);
  } else {
    while (flag->status == 1) {
      // busy loop!
      thrd_yield();
    }
  }
#    endif
}

int cnd_broadcast(cnd_t *cond) {
  assert(cond != NULL);
  WakeAllConditionVariable((PCONDITION_VARIABLE) cond);
  return thrd_success;
}

void cnd_destroy(cnd_t *cond) {
  assert(cond != NULL);
  /* do nothing */
  (void) cond;
}

int cnd_init(cnd_t *cond) {
  assert(cond != NULL);
  InitializeConditionVariable((PCONDITION_VARIABLE) cond);
  return thrd_success;
}

int cnd_signal(cnd_t *cond) {
  assert(cond != NULL);
  WakeConditionVariable((PCONDITION_VARIABLE) cond);
  return thrd_success;
}

int cnd_timedwait(cnd_t *cond, mtx_t *mtx,
                  const struct timespec *abs_time) {
  assert(cond != NULL);
  assert(mtx != NULL);
  assert(abs_time != NULL);
  const DWORD timeout = impl_abs2relmsec(abs_time);
  if (SleepConditionVariableCS((PCONDITION_VARIABLE) cond,
                               (PCRITICAL_SECTION) mtx, timeout))
    return thrd_success;
  return (GetLastError() == ERROR_TIMEOUT) ? thrd_timedout
                                           : thrd_error;
}

int cnd_wait(cnd_t *cond, mtx_t *mtx) {
  assert(cond != NULL);
  assert(mtx != NULL);
  SleepConditionVariableCS((PCONDITION_VARIABLE) cond,
                           (PCRITICAL_SECTION) mtx, INFINITE);
  return thrd_success;
}

void mtx_destroy(mtx_t *mtx) {
  assert(mtx);
  DeleteCriticalSection((PCRITICAL_SECTION) mtx);
}

int mtx_init(mtx_t *mtx, int type) {
  assert(mtx != NULL);
  if (type != mtx_plain && type != mtx_timed &&
      type != (mtx_plain | mtx_recursive) &&
      type != (mtx_timed | mtx_recursive))
    return thrd_error;
  InitializeCriticalSection((PCRITICAL_SECTION) mtx);
  return thrd_success;
}

int mtx_lock(mtx_t *mtx) {
  assert(mtx != NULL);
  EnterCriticalSection((PCRITICAL_SECTION) mtx);
  return thrd_success;
}

int mtx_timedlock(mtx_t *mtx, const struct timespec *ts) {
  assert(mtx != NULL);
  assert(ts != NULL);
  while (mtx_trylock(mtx) != thrd_success) {
    if (impl_abs2relmsec(ts) == 0)
      return thrd_timedout;
    /* busy loop! */
    thrd_yield();
  }
  return thrd_success;
}

int mtx_trylock(mtx_t *mtx) {
  assert(mtx != NULL);
  return TryEnterCriticalSection((PCRITICAL_SECTION) mtx)
             ? thrd_success
             : thrd_busy;
}

int mtx_unlock(mtx_t *mtx) {
  assert(mtx != NULL);
  LeaveCriticalSection((PCRITICAL_SECTION) mtx);
  return thrd_success;
}

int thrd_create_with_stack(thrd_t *thr, thrd_start_t func, void *arg,
                           ptrdiff_t const stack_size) {
  impl_thrd_param_t *pack;
  uintptr_t          handle;
  assert(thr != NULL);
  assert(stack_size >= 0 && stack_size < 0x100000000);
  kit_allocator_t alloc = kit_alloc_default();
  pack                  = (impl_thrd_param_t *) alloc.allocate(
                       alloc.state, (sizeof(impl_thrd_param_t)));
  if (!pack)
    return thrd_nomem;
  pack->func  = func;
  pack->arg   = arg;
  pack->alloc = alloc;
  handle      = _beginthreadex(NULL, (unsigned) stack_size,
                               impl_thrd_routine, pack, CREATE_SUSPENDED,
                               NULL);
  if (handle == 0) {
    alloc.deallocate(alloc.state, pack);
    if (errno == EAGAIN || errno == EACCES)
      return thrd_nomem;
    return thrd_error;
  }
  thr->handle = (void *) handle;
  pack->thrd  = *thr;
  ResumeThread((HANDLE) handle);
  return thrd_success;
}

int thrd_create(thrd_t *thr, thrd_start_t func, void *arg) {
  return thrd_create_with_stack(thr, func, arg, 0);
}

thrd_t thrd_current(void) {
  /* GetCurrentThread() returns a pseudo-handle, which we need
   * to pass to DuplicateHandle(). Only the resulting handle can be
   * used from other threads.
   *
   * Note that neither handle can be compared to the one by
   * thread_create. Only the thread IDs - as returned by GetThreadId()
   * and GetCurrentThreadId() can be compared directly.
   *
   * Other potential solutions would be:
   * - define thrd_t as a thread Ids, but this would mean we'd need to
   * OpenThread for many operations
   * - use malloc'ed memory for thrd_t. This would imply using TLS for
   * current thread.
   *
   * Neither is particularly nice.
   *
   * Life would be much easier if C11 threads had different
   * abstractions for threads and thread IDs, just like C++11 threads
   * does...
   */
  struct thrd_state *state = &impl_current_thread;
  if (state->thrd.handle == NULL) {
    if (!DuplicateHandle(GetCurrentProcess(), GetCurrentThread(),
                         GetCurrentProcess(), &(state->thrd.handle),
                         0, FALSE, DUPLICATE_SAME_ACCESS)) {
      abort();
    }
    state->handle_need_close = true;
  }
  return state->thrd;
}

int thrd_detach(thrd_t thr) {
  CloseHandle(thr.handle);
  return thrd_success;
}

int thrd_equal(thrd_t thr0, thrd_t thr1) {
  return GetThreadId(thr0.handle) == GetThreadId(thr1.handle);
}

_Noreturn void thrd_exit(int res) {
  _endthreadex((unsigned) res);
}

int thrd_join(thrd_t thr, int *res) {
  DWORD w, code;
  if (thr.handle == NULL) {
    return thrd_error;
  }
  w = WaitForSingleObject(thr.handle, INFINITE);
  if (w != WAIT_OBJECT_0)
    return thrd_error;
  if (res) {
    if (!GetExitCodeThread(thr.handle, &code)) {
      CloseHandle(thr.handle);
      return thrd_error;
    }
    *res = (int) code;
  }
  CloseHandle(thr.handle);
  return thrd_success;
}

int thrd_sleep(const struct timespec *time_point,
               struct timespec       *remaining) {
  (void) remaining;
  assert(time_point);
  assert(!remaining); /* not implemented */
  Sleep((DWORD) impl_timespec2msec(time_point));
  return 0;
}

void thrd_yield(void) {
  SwitchToThread();
}

#  endif
#endif
