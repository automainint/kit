#include "thread.h"

#ifdef _MSC_VER
#  include "atomic.h"

#  include <stdlib.h>
#  include <windows.h>

typedef struct {
  HANDLE              thread;
  kit_thread_routine_ routine;
  void               *user_data;
  void               *return_value;
} thread_data;

DWORD __stdcall run_thread_(void *p) {
  thread_data *data  = (thread_data *) p;
  data->return_value = data->routine(data->user_data);
}

int pthread_create(pthread_t *new_thread, void *attrs,
                   kit_thread_routine_ routine, void *user_data) {
  thread_data *data = (thread_data *) malloc(sizeof(thread_data));
  if (data == NULL)
    return -1;
  data->routine   = routine;
  data->user_data = user_data;
  data->thread    = CreateThread(NULL, 0, run_thread_, data, 0, NULL);
  if (data->thread == NULL)
    return -1;
  if (new_thread != NULL)
    *new_thread = data;
  return 0;
}

void *pthread_join(pthread_t thread, void *return_value) {
  thread_data *data = (thread_data *) thread;
  if (data == NULL || data->thread == NULL)
    return (void *) 0;
  WaitForSingleObject(data->thread, INFINITE);
  void *return_value = data->return_value;
  free(data);
  return return_value;
}
#endif
