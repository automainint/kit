#ifndef KIT_LOWER_BOUND_H
#define KIT_LOWER_BOUND_H

#ifdef __cplusplus
extern "C" {
#endif

#define KIT_LOWER_BOUND(return_val, array, value, op)        \
  {                                                          \
    ptrdiff_t position_ = 0;                                 \
    ptrdiff_t count_    = (array).size;                      \
    while (count_ > 0) {                                     \
      ptrdiff_t delta_ = count_ / 2;                         \
      if (op((array).values[position_ + delta_], (value))) { \
        position_ += delta_ + 1;                             \
        count_ -= delta_ + 1;                                \
      } else                                                 \
        count_ = delta_;                                     \
    }                                                        \
    (return_val) = position_;                                \
  }

#define KIT_LOWER_BOUND_REF(return_val, array, value, op)      \
  {                                                            \
    ptrdiff_t position_ = 0;                                   \
    ptrdiff_t count_    = (array).size;                        \
    while (count_ > 0) {                                       \
      ptrdiff_t delta_ = count_ / 2;                           \
      if (op((array).values + position_ + delta_, &(value))) { \
        position_ += delta_ + 1;                               \
        count_ -= delta_ + 1;                                  \
      } else                                                   \
        count_ = delta_;                                       \
    }                                                          \
    (return_val) = position_;                                  \
  }

#ifdef __cplusplus
}
#endif

#endif
