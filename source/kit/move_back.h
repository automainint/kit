#ifndef KIT_MOVE_BACK_H
#define KIT_MOVE_BACK_H

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define KIT_MOVE_BACK_INL(new_size, array, ...)          \
  do {                                                   \
    ptrdiff_t     end_ = (array).size;                   \
    unsigned char temp_[sizeof *(array).values];         \
    for (ptrdiff_t index_ = 0; index_ < end_;) {         \
      if (__VA_ARGS__) {                                 \
        end_--;                                          \
        if (index_ != end_) {                            \
          memcpy(temp_, (array).values + end_,           \
                 sizeof *(array).values);                \
          (array).values[end_] = (array).values[index_]; \
          memcpy((array).values + index_, temp_,         \
                 sizeof *(array).values);                \
        }                                                \
      } else                                             \
        index_++;                                        \
    }                                                    \
    (new_size) = end_;                                   \
  } while (0)

#define KIT_MOVE_BACK(new_size, array, value, cond) \
  KIT_MOVE_BACK_INL(new_size, array,                \
                    (cond) ((array).values[index_], (value)))

#define KIT_MOVE_BACK_REF(new_size, array, value, cond) \
  KIT_MOVE_BACK_INL(new_size, array,                    \
                    (cond) ((array).values + index_, (value)))

#ifndef KIT_DISABLE_SHORT_NAMES
#  define MOVE_BACK_INL KIT_MOVE_BACK_INL
#  define MOVE_BACK KIT_MOVE_BACK
#  define MOVE_BACK_REF KIT_MOVE_BACK_REF
#endif

#ifdef __cplusplus
}
#endif

#endif
