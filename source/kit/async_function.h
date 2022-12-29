#ifndef KIT_ASYNC_FUNCTION_H
#define KIT_ASYNC_FUNCTION_H

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
  kit_af_request_execute         = 0,
  kit_af_request_resume          = 1,
  kit_af_request_join            = 2,
  kit_af_request_resume_and_join = 3
};

typedef struct {
  int _;
} kit_af_void;

typedef void (*kit_af_state_machine)(void *self_void_, int request_);
typedef void (*kit_af_execute)(void *state, void *coro_state,
                               int request);

typedef struct {
  long long      _internal;
  void          *state;
  kit_af_execute execute;
} kit_af_execution_context;

#define KIT_AF_STATE_DATA                    \
  struct {                                   \
    int                      _index;         \
    kit_af_state_machine     _state_machine; \
    kit_af_execution_context _context;       \
  }

typedef struct {
  KIT_AF_STATE_DATA;
} kit_af_type_void;

#define KIT_AF_INTERNAL(coro_) (*((kit_af_type_void *) (coro_)))

#define KIT_AF_STATE(ret_type_, name_, ...) \
  struct name_##_coro_state_ {              \
    KIT_AF_STATE_DATA;                      \
    ret_type_ return_value;                 \
    __VA_ARGS__                             \
  }

#define KIT_AF_DECL(name_) void name_(void *self_void_, int request_)

#define KIT_CORO_IMPL(name_)                                      \
  KIT_AF_DECL(name_) {                                            \
    struct name_##_coro_state_ *self =                            \
        (struct name_##_coro_state_ *) self_void_;                \
    if (request_ != kit_af_request_execute) {                     \
      if (self->_context.execute != NULL)                         \
        self->_context.execute(self->_context.state, self_void_,  \
                               request_);                         \
      else if (request_ == kit_af_request_join ||                 \
               request_ == kit_af_request_resume_and_join)        \
        self->_state_machine(self_void_, kit_af_request_execute); \
      return;                                                     \
    }                                                             \
    switch (self->_index) {                                       \
      case 0:;

#define KIT_AF_LINE_() __LINE__

#define KIT_CORO_END \
  }                  \
  self->_index = -1; \
  }

#define KIT_CORO_DECL(ret_type_, name_, ...)   \
  KIT_AF_STATE(ret_type_, name_, __VA_ARGS__); \
  KIT_AF_DECL(name_)

#define KIT_CORO(ret_type_, name_, ...)        \
  KIT_AF_STATE(ret_type_, name_, __VA_ARGS__); \
  KIT_CORO_IMPL(name_)

#define KIT_CORO_DECL_VOID(name_, ...) \
  KIT_CORO_DECL(kit_af_void, name_, __VA_ARGS__)

#define KIT_CORO_VOID(name_, ...) \
  KIT_CORO(kit_af_void, name_, __VA_ARGS__)

#define KIT_STATIC_CORO(ret_type_, name_, ...) \
  KIT_AF_STATE(ret_type_, name_, __VA_ARGS__); \
  static KIT_CORO_IMPL(name_)

#define KIT_STATIC_CORO_VOID(name_, ...) \
  KIT_STATIC_CORO(kit_af_void, name_, __VA_ARGS__)

#define KIT_AF_YIELD(...)                \
  do {                                   \
    self->_index       = KIT_AF_LINE_(); \
    self->return_value = __VA_ARGS__;    \
    return;                              \
    case KIT_AF_LINE_():;                \
  } while (0)

#define KIT_AF_YIELD_VOID          \
  do {                             \
    self->_index = KIT_AF_LINE_(); \
    return;                        \
    case KIT_AF_LINE_():;          \
  } while (0)

#define KIT_AF_RETURN(...)            \
  do {                                \
    self->_index       = -1;          \
    self->return_value = __VA_ARGS__; \
    return;                           \
  } while (0)

#define KIT_AF_RETURN_VOID \
  do {                     \
    self->_index = -1;     \
    return;                \
  } while (0)

#define KIT_AF_AWAIT(promise_)                                     \
  do {                                                             \
    case KIT_AF_LINE_():                                           \
      if ((promise_)._index != -1) {                               \
        self->_index = KIT_AF_LINE_();                             \
        (promise_)._state_machine(&(promise_),                     \
                                  kit_af_request_resume_and_join); \
      }                                                            \
      if ((promise_)._index != -1)                                 \
        return;                                                    \
  } while (0)

#define KIT_AF_YIELD_AWAIT(promise_)                               \
  do {                                                             \
    case KIT_AF_LINE_():                                           \
      if ((promise_)._index != -1) {                               \
        self->_index = KIT_AF_LINE_();                             \
        (promise_)._state_machine(&(promise_),                     \
                                  kit_af_request_resume_and_join); \
        self->return_value = (promise_).return_value;              \
        return;                                                    \
      }                                                            \
  } while (0)

#define KIT_AF_TYPE(coro_) struct coro_##_coro_state_

#define KIT_AF_INITIAL(coro_)             \
  ._index = 0, ._state_machine = (coro_), \
  ._context = { .state = NULL, .execute = NULL }

#define KIT_AF_CREATE(promise_, coro_, ...) \
  KIT_AF_TYPE(coro_)                        \
  promise_ = { KIT_AF_INITIAL(coro_), __VA_ARGS__ }

#define KIT_AF_INIT(promise_, coro_, ...)            \
  do {                                               \
    KIT_AF_CREATE(kit_af_temp_, coro_, __VA_ARGS__); \
    (promise_) = kit_af_temp_;                       \
  } while (0)

#define KIT_AF_INIT_EXPLICIT(promise_, size_, coro_func_) \
  do {                                                    \
    memset(&(promise_), 0, size_);                        \
    (promise_)._state_machine = (coro_func_);             \
  } while (0)

#define KIT_AF_EXECUTION_CONTEXT(promise_, ...)               \
  do {                                                        \
    kit_af_execution_context kit_af_temp_ = { ._internal = 0, \
                                              __VA_ARGS__ };  \
    (promise_)._context                   = kit_af_temp_;     \
  } while (0)

#define KIT_AF_RESUME(promise_) \
  (promise_)._state_machine(&(promise_), kit_af_request_resume)

#define KIT_AF_RESUME_N(promises_, size_)            \
  do {                                               \
    int kit_af_index_;                               \
    for (kit_af_index_ = 0; kit_af_index_ < (size_); \
         kit_af_index_++)                            \
      KIT_AF_RESUME((promises_)[kit_af_index_]);     \
  } while (0)

#define KIT_AF_JOIN(promise_)                                   \
  ((promise_)._state_machine(&(promise_), kit_af_request_join), \
   (promise_).return_value)

#define KIT_AF_JOIN_N(promises_, size_)              \
  do {                                               \
    int kit_af_index_;                               \
    for (kit_af_index_ = 0; kit_af_index_ < (size_); \
         kit_af_index_++)                            \
      KIT_AF_JOIN((promises_)[kit_af_index_]);       \
  } while (0)

#define KIT_AF_RESUME_AND_JOIN(promise_)                      \
  ((promise_)._state_machine(&(promise_),                     \
                             kit_af_request_resume_and_join), \
   (promise_).return_value)

#define KIT_AF_RESUME_AND_JOIN_N(promises_, size_) \
  do {                                             \
    KIT_AF_RESUME_N((promises_), (size_));         \
    KIT_AF_JOIN_N((promises_), (size_));           \
  } while (0)

#define KIT_AF_RESUME_ALL(promises_) \
  KIT_AF_RESUME_N((promises_),       \
                  sizeof(promises_) / sizeof((promises_)[0]))

#define KIT_AF_JOIN_ALL(promises_) \
  KIT_AF_JOIN_N((promises_),       \
                sizeof(promises_) / sizeof((promises_)[0]))

#define KIT_AF_RESUME_AND_JOIN_ALL(promises_)               \
  KIT_AF_RESUME_AND_JOIN_N((promises_), sizeof(promises_) / \
                                            sizeof((promises_)[0]))

#define KIT_AF_FINISHED(promise_) ((promise_)._index == -1)

#define KIT_AF_FINISHED_N(return_, promises_, size_)      \
  do {                                                    \
    int kit_af_index_;                                    \
    (return_) = 1;                                        \
    for (kit_af_index_ = 0; kit_af_index_ < (size_);      \
         kit_af_index_++)                                 \
      if (!KIT_AF_FINISHED((promises_)[kit_af_index_])) { \
        (return_) = 0;                                    \
        break;                                            \
      }                                                   \
  } while (0)

#define KIT_AF_FINISHED_ALL(return_, promises_) \
  KIT_AF_FINISHED_N((return_), (promises_),     \
                    sizeof(promises_) / sizeof((promises_)[0]))

#define KIT_AF_AWAIT_N(promises_, size_)                     \
  do {                                                       \
    int kit_af_done_;                                        \
    case KIT_AF_LINE_():                                     \
      self->_index = KIT_AF_LINE_();                         \
      KIT_AF_RESUME_AND_JOIN_N((promises_), (size_));        \
      KIT_AF_FINISHED_N(kit_af_done_, (promises_), (size_)); \
      if (!kit_af_done_)                                     \
        return;                                              \
  } while (0)

#define KIT_AF_AWAIT_ALL(promises_) \
  KIT_AF_AWAIT_N((promises_),       \
                 sizeof(promises_) / sizeof((promises_)[0]))

#define KIT_AF_EXECUTE(promise_)                       \
  KIT_AF_INTERNAL(promise_)._state_machine((promise_), \
                                           kit_af_request_execute)

#ifndef KIT_DISABLE_SHORT_NAMES
#  ifndef KIT_DISABLE_AF_SELF_SHORTCUT
#    define af self->
#  endif

#  define af_request kit_af_request
#  define af_request_execute kit_af_request_execute
#  define af_request_resume kit_af_request_resume
#  define af_request_join kit_af_request_join
#  define af_request_resume_and_join kit_af_request_resume_and_join
#  define af_void kit_af_void
#  define af_state_machine kit_af_state_machine
#  define af_execute kit_af_execute
#  define af_execution_context kit_af_execution_context
#  define af_type_void kit_af_type_void

#  define AF_STATE_DATA KIT_AF_STATE_DATA
#  define AF_INTERNAL KIT_AF_INTERNAL
#  define AF_STATE KIT_AF_STATE
#  define AF_DECL KIT_AF_DECL
#  define CORO_IMPL KIT_CORO_IMPL
#  define CORO_END KIT_CORO_END
#  define CORO_DECL KIT_CORO_DECL
#  define CORO KIT_CORO
#  define CORO_DECL_VOID KIT_CORO_DECL_VOID
#  define STATIC_CORO KIT_STATIC_CORO
#  define STATIC_CORO_VOID KIT_STATIC_CORO_VOID
#  define CORO_VOID KIT_CORO_VOID
#  define AF_YIELD KIT_AF_YIELD
#  define AF_YIELD_VOID KIT_AF_YIELD_VOID
#  define AF_RETURN KIT_AF_RETURN
#  define AF_RETURN_VOID KIT_AF_RETURN_VOID
#  define AF_AWAIT KIT_AF_AWAIT
#  define AF_YIELD_AWAIT KIT_AF_YIELD_AWAIT
#  define AF_TYPE KIT_AF_TYPE
#  define AF_INITIAL KIT_AF_INITIAL
#  define AF_CREATE KIT_AF_CREATE
#  define AF_INIT KIT_AF_INIT
#  define AF_INIT_EXPLICIT KIT_AF_INIT_EXPLICIT
#  define AF_EXECUTION_CONTEXT KIT_AF_EXECUTION_CONTEXT
#  define AF_RESUME KIT_AF_RESUME
#  define AF_RESUME_N KIT_AF_RESUME_N
#  define AF_JOIN KIT_AF_JOIN
#  define AF_JOIN_N KIT_AF_JOIN_N
#  define AF_RESUME_AND_JOIN KIT_AF_RESUME_AND_JOIN
#  define AF_RESUME_AND_JOIN_N KIT_AF_RESUME_AND_JOIN_N
#  define AF_RESUME_ALL KIT_AF_RESUME_ALL
#  define AF_JOIN_ALL KIT_AF_JOIN_ALL
#  define AF_RESUME_AND_JOIN_ALL KIT_AF_RESUME_AND_JOIN_ALL
#  define AF_FINISHED KIT_AF_FINISHED
#  define AF_FINISHED_N KIT_AF_FINISHED_N
#  define AF_FINISHED_ALL KIT_AF_FINISHED_ALL
#  define AF_AWAIT_N KIT_AF_AWAIT_N
#  define AF_AWAIT_ALL KIT_AF_AWAIT_ALL
#  define AF_EXECUTE KIT_AF_EXECUTE
#endif

#ifdef __cplusplus
}
#endif

#endif
