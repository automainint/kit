#ifndef KIT_ASYNC_FUNCTION_H
#define KIT_ASYNC_FUNCTION_H

#include <stddef.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wunused-function"
#  pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif

typedef struct {
  int _;
} kit_af_void;

typedef void (*kit_af_state_machine)(void *self_void_);

#define KIT_AF_STATE_DATA                \
  struct {                               \
    int                  _index;         \
    ptrdiff_t            _id;            \
    kit_af_state_machine _state_machine; \
  }

typedef struct {
  KIT_AF_STATE_DATA;
} kit_af_type_void;

#define KIT_AF_INTERNAL(coro_) (*((kit_af_type_void *) (coro_)))

#ifdef KIT_ENABLE_CUSTOM_ASYNC_FUNCTION_DISPATCH
/*  Application should implement this function if custom async
 *  function dispatch is enabled.
 *
 *  See KIT_ENABLE_CUSTOM_ASYNC_FUNCTION_DISPATCH macro.
 */
void kit_async_function_dispatch(void *promise);
#else
static void kit_async_function_dispatch(void *promise) {
  /*  Dynamic dispatch by default.
   */
  KIT_AF_INTERNAL(promise)._state_machine(promise);
}
#endif

#ifdef __GNUC__
#  pragma GCC diagnostic pop
#endif

#define KIT_AF_STATE(ret_type_, name_, ...) \
  struct name_##_coro_state_ {              \
    KIT_AF_STATE_DATA;                      \
    ret_type_ return_value;                 \
    __VA_ARGS__                             \
  }

#define KIT_AF_DECL(name_) void name_(void *self_void_)

#define KIT_CORO_IMPL(name_)                       \
  KIT_AF_DECL(name_) {                             \
    struct name_##_coro_state_ *self =             \
        (struct name_##_coro_state_ *) self_void_; \
    switch (self->_index) {                        \
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

#define KIT_AF_EXECUTE(promise_) \
  kit_async_function_dispatch(&(promise_))

#define KIT_AF_NEXT(promise_) \
  (kit_async_function_dispatch(&(promise_)), (promise_).return_value)

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

#define KIT_AF_AWAIT(promise_)                    \
  do {                                            \
    case KIT_AF_LINE_():                          \
      if ((promise_)._index != -1) {              \
        self->_index = KIT_AF_LINE_();            \
        kit_async_function_dispatch(&(promise_)); \
      }                                           \
      if ((promise_)._index != -1)                \
        return;                                   \
  } while (0)

#define KIT_AF_YIELD_AWAIT(promise_)                  \
  do {                                                \
    case KIT_AF_LINE_():                              \
      if ((promise_)._index != -1) {                  \
        self->_index = KIT_AF_LINE_();                \
        kit_async_function_dispatch(&(promise_));     \
        self->return_value = (promise_).return_value; \
        return;                                       \
      }                                               \
  } while (0)

#define KIT_AF_TYPE(coro_) struct coro_##_coro_state_

#define KIT_AF_INITIAL(id_, coro_) \
  ._index = 0, ._id = (id_), ._state_machine = (coro_)

#define KIT_AF_CREATE(promise_, coro_, ...) \
  KIT_AF_TYPE(coro_)                        \
  promise_ = { KIT_AF_INITIAL(0, coro_), __VA_ARGS__ }

#define KIT_AF_CREATE_ID(promise_, id_, ...) \
  KIT_AF_TYPE(coro_)                         \
  promise_ = { KIT_AF_INITIAL(id_, NULL), __VA_ARGS__ }

#define KIT_AF_INIT(promise_, coro_, ...)                    \
  do {                                                       \
    KIT_AF_CREATE(kit_af_temp_, coro_, __VA_ARGS__);         \
    memcpy(&(promise_), &kit_af_temp_, sizeof kit_af_temp_); \
  } while (0)

#define KIT_AF_INIT_ID(promise_, id_, ...)                   \
  do {                                                       \
    KIT_AF_CREATE_ID(kit_af_temp_, id_, __VA_ARGS__);        \
    memcpy(&(promise_), &kit_af_temp_, sizeof kit_af_temp_); \
  } while (0)

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

#ifndef KIT_DISABLE_SHORT_NAMES
#  define af_void kit_af_void
#  define af_state_machine kit_af_state_machine
#  define af_type_void kit_af_type_void

#  define AF_STATE_DATA KIT_AF_STATE_DATA
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
#  define AF_EXECUTE KIT_AF_EXECUTE
#  define AF_NEXT KIT_AF_NEXT
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
#  define AF_FINISHED KIT_AF_FINISHED
#  define AF_FINISHED_N KIT_AF_FINISHED_N
#  define AF_FINISHED_ALL KIT_AF_FINISHED_ALL
#endif

#ifdef __cplusplus
}
#endif

#endif
