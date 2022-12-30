#ifndef KIT_THREAD_DEFS_H
#define KIT_THREAD_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif

enum {
  thrd_success = 0,
  thrd_timedout,
  thrd_error,
  thrd_busy,
  thrd_nomem,
  thrd_wrong_stack_size
};

#ifdef __cplusplus
}
#endif

#endif

