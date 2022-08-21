#ifndef KIT_MOVE_BACK_H
#define KIT_MOVE_BACK_H

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define KIT_MOVE_BACK(new_size, array, value, cond)  \
  do {                                               \
    ptrdiff_t     end_ = (array).size;               \
    unsigned char temp_[sizeof *(array).values];     \
    for (ptrdiff_t i_ = 0; i_ < end_;) {             \
      if ((cond) ((array).values[i_], (value))) {    \
        end_--;                                      \
        if (i_ != end_) {                            \
          memcpy(temp_, (array).values + end_,       \
                 sizeof *(array).values);            \
          (array).values[end_] = (array).values[i_]; \
          memcpy((array).values + i_, temp_,         \
                 sizeof *(array).values);            \
        }                                            \
      } else                                         \
        i_++;                                        \
    }                                                \
    (new_size) = end_;                               \
  } while (0)

#define KIT_MOVE_BACK_REF(new_size, array, value, cond) \
  do {                                                  \
    ptrdiff_t     end_ = (array).size;                  \
    unsigned char temp_[sizeof *(array).values];        \
    for (ptrdiff_t i_ = 0; i_ < end_;) {                \
      if ((cond) (&(array).values[i_], &(value))) {     \
        end_--;                                         \
        if (i_ != end_) {                               \
          memcpy(temp_, (array).values + end_,          \
                 sizeof *(array).values);               \
          (array).values[end_] = (array).values[i_];    \
          memcpy((array).values + i_, temp_,            \
                 sizeof *(array).values);               \
        }                                               \
      } else                                            \
        i_++;                                           \
    }                                                   \
    (new_size) = end_;                                  \
  } while (0)

#ifndef KIT_DISABLE_SHORT_NAMES
#  define MOVE_BACK KIT_MOVE_BACK
#  define MOVE_BACK_REF KIT_MOVE_BACK_REF
#endif

#ifdef __cplusplus
}
#endif

#endif
