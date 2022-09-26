#ifndef KIT_DYNAMIC_ARRAY_H
#define KIT_DYNAMIC_ARRAY_H

#include "allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  ptrdiff_t       capacity;
  ptrdiff_t       size;
  void           *values;
  kit_allocator_t alloc;
} kit_da_void_t;

void kit_da_init(kit_da_void_t *array, ptrdiff_t element_size,
                 ptrdiff_t size, kit_allocator_t alloc);

void kit_da_resize(kit_da_void_t *array, ptrdiff_t element_size,
                   ptrdiff_t size);

/*  Declare dynamic array type.
 */
#define KIT_DA_TYPE(name_, element_type_) \
  typedef struct {                        \
    ptrdiff_t       capacity;             \
    ptrdiff_t       size;                 \
    element_type_  *values;               \
    kit_allocator_t alloc;                \
  } name_

/*  Declare dynamic array.
 */
#define KIT_DA(name_, element_type_) \
  struct {                           \
    ptrdiff_t       capacity;        \
    ptrdiff_t       size;            \
    element_type_  *values;          \
    kit_allocator_t alloc;           \
  } name_

/*  Initialize dynamic array.
 */
#define KIT_DA_INIT(array_, size_, alloc_) \
  kit_da_init((kit_da_void_t *) &(array_), \
              sizeof((array_).values[0]), (size_), (alloc_))

/*  Declare and initialize dynamic array.
 */
#define KIT_DA_CREATE(name_, element_type_, size_) \
  KIT_DA(name_, element_type_);                    \
  KIT_DA_INIT(name_, (size_), kit_alloc_default())

/*  Destroy dynamic array.
 */
#define KIT_DA_DESTROY(array_)                        \
  do {                                                \
    if ((array_).values != NULL)                      \
      (array_).alloc.deallocate((array_).alloc.state, \
                                (array_).values);     \
  } while (0)

/*  Resize dynamic array.
 */
#define KIT_DA_RESIZE(array_, size_)         \
  kit_da_resize((kit_da_void_t *) &(array_), \
                sizeof((array_).values[0]), size_)

/*  Append a value to dynamic array.
 */
#define KIT_DA_APPEND(array_, value_)                \
  do {                                               \
    ptrdiff_t const kit_index_back_ = (array_).size; \
    KIT_DA_RESIZE((array_), kit_index_back_ + 1);    \
    if (kit_index_back_ < (array_).size)             \
      (array_).values[kit_index_back_] = (value_);   \
  } while (0)

/*  Insert a value into dynamic array.
 */
#define KIT_DA_INSERT(array_, index_, value_)                  \
  do {                                                         \
    ptrdiff_t const kit_index_back_ = (array_).size;           \
    ptrdiff_t const kit_indert_n_   = (index_);                \
    KIT_DA_RESIZE((array_), kit_index_back_ + 1);              \
    if (kit_index_back_ + 1 == (array_).size) {                \
      for (ptrdiff_t kit_i_ = kit_index_back_;                 \
           kit_i_ > kit_indert_n_; kit_i_--)                   \
        (array_).values[kit_i_] = (array_).values[kit_i_ - 1]; \
      (array_).values[kit_indert_n_] = (value_);               \
    }                                                          \
  } while (0)

/*  Erase a value from dynamic array.
 */
#define KIT_DA_ERASE(array_, index_)                            \
  do {                                                          \
    for (ptrdiff_t i_ = (index_) + 1; i_ < (array_).size; i_++) \
      (array_).values[i_ - 1] = (array_).values[i_];            \
    KIT_DA_RESIZE((array_), (array_).size - 1);                 \
  } while (0)

KIT_DA_TYPE(kit_string_t, char);

#ifndef KIT_DISABLE_SHORT_NAMES
#  define da_void_t kit_da_void_t
#  define da_init kit_da_init
#  define da_resize kit_da_resize
#  define string_t kit_string_t

#  define DA_TYPE KIT_DA_TYPE
#  define DA KIT_DA
#  define DA_INIT KIT_DA_INIT
#  define DA_CREATE KIT_DA_CREATE
#  define DA_DESTROY KIT_DA_DESTROY
#  define DA_RESIZE KIT_DA_RESIZE
#  define DA_APPEND KIT_DA_APPEND
#  define DA_INSERT KIT_DA_INSERT
#  define DA_ERASE KIT_DA_ERASE
#endif

#ifdef __cplusplus
}
#endif

#endif
