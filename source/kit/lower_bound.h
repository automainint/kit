#ifndef KIT_LOWER_BOUND_H
#define KIT_LOWER_BOUND_H

#ifdef __cplusplus
extern "C" {
#endif

#define KIT_LOWER_BOUND_INL(return_val, size, ...) \
  do {                                             \
    ptrdiff_t position_ = 0;                       \
    ptrdiff_t count_    = (size);                  \
    while (count_ > 0) {                           \
      ptrdiff_t const delta_ = count_ / 2;         \
      ptrdiff_t const index_ = position_ + delta_; \
      if (__VA_ARGS__) {                           \
        position_ += delta_ + 1;                   \
        count_ -= delta_ + 1;                      \
      } else                                       \
        count_ = delta_;                           \
    }                                              \
    (return_val) = position_;                      \
  } while (0)

#define KIT_LOWER_BOUND(return_val, array, value, op) \
  KIT_LOWER_BOUND_INL(return_val, (array).size,       \
                      (op) ((array).values[index_], (value)))

#define KIT_LOWER_BOUND_REF(return_val, array, value, op) \
  KIT_LOWER_BOUND_INL(return_val, (array).size,           \
                      (op) ((array).values + index_, (value)))

#ifndef KIT_DISABLE_SHORT_NAMES
#  define LOWER_BOUND_INL KIT_LOWER_BOUND_INL
#  define LOWER_BOUND KIT_LOWER_BOUND
#  define LOWER_BOUND_REF KIT_LOWER_BOUND_REF
#endif

#ifdef __cplusplus
}
#endif

#endif
