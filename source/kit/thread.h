#ifndef KIT_THREAD_H
#define KIT_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _MSC_VER
#  include "pthread.h"
#else
typedef void *(*kit_thread_routine_)(void *);
typedef void *pthread_t;

int pthread_create(pthread_t *new_thread, void *attrs,
                   kit_thread_routine_ routine, void *user_data);

void *pthread_join(pthread_t thread, void *return_value);
#endif

#ifdef __cplusplus
}
#endif

#endif
