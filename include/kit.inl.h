#ifndef KIT_INL_H
#define KIT_INL_H
/*********************************************************************
 *                                                                   *
 *    File:   ./source/kit/status.h                                  *
 *                                                                   *
 *********************************************************************/
#ifndef KIT_STATUS_H
#define KIT_STATUS_H
#ifdef __cplusplus
extern "C" {
#endif
enum {
  KIT_OK,
  KIT_ERROR_BAD_ALLOC,
  KIT_ERROR_MKDIR_FAILED,
  KIT_ERROR_RMDIR_FAILED,
  KIT_ERROR_UNLINK_FAILED,
  KIT_ERROR_FILE_ALREADY_EXISTS,
  KIT_ERROR_FILE_DO_NOT_EXIST,
  KIT_ERROR_PATH_TOO_LONG
};
typedef int kit_status_t;
#ifdef __cplusplus
}
#endif
#endif
/*********************************************************************
 *                                                                   *
 *    File:   ./source/kit/allocator.h                               *
 *                                                                   *
 *********************************************************************/
#ifndef KIT_ALLOCATOR_H
#define KIT_ALLOCATOR_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
enum { KIT_ALLOCATE, KIT_DEALLOCATE, KIT_REALLOCATE };
typedef void *(*kit_allocate_fn)(int request, void *state,
                                 ptrdiff_t size,
                                 ptrdiff_t previous_size,
                                 void     *pointer);
typedef struct {
  void           *state;
  kit_allocate_fn allocate;
} kit_allocator_t;
/*  Application should implement this function if custom allocator
 *  dispatch is enabled.
 *
 *  See KIT_ENABLE_CUSTOM_ALLOC_DISPATCH
 */
void *kit_alloc_dispatch(kit_allocator_t alloc, int request,
                         ptrdiff_t size, ptrdiff_t previous_size,
                         void *pointer);
kit_allocator_t kit_alloc_default(void);
#ifdef __cplusplus
}
#endif
#endif
/*********************************************************************
 *                                                                   *
 *    File:   ./source/kit/array_ref.h                               *
 *                                                                   *
 *********************************************************************/
#ifndef KIT_ARRAY_REF_H
#define KIT_ARRAY_REF_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef int (*kit_ar_compare_fn)(void const *left, void const *right);
int kit_ar_equal_bytes(ptrdiff_t left_element_size,
                       ptrdiff_t left_size, void const *left_data,
                       ptrdiff_t right_element_size,
                       ptrdiff_t right_size, void const *right_data);
int kit_ar_compare(ptrdiff_t left_element_size, ptrdiff_t left_size,
                   void const *left_data,
                   ptrdiff_t right_element_size, ptrdiff_t right_size,
                   void const *right_data, kit_ar_compare_fn compare);
#define KIT_AR_MUT(type_) \
  struct {                       \
    ptrdiff_t size;              \
    type_    *values;            \
  }
#define KIT_AR(type_) \
  struct {                   \
    ptrdiff_t    size;       \
    type_ const *values;     \
  }
#define KIT_AR_MUT_WRAP(name_, element_type_, array_)           \
  struct {                                                      \
    ptrdiff_t      size;                                        \
    element_type_ *values;                                      \
  } name_ = { .size   = (sizeof(array_) / sizeof((array_)[0])), \
              .values = (array_) }
#define KIT_AR_WRAP(name_, element_type_, array_)               \
  struct {                                                      \
    ptrdiff_t            size;                                  \
    element_type_ const *values;                                \
  } name_ = { .size   = (sizeof(array_) / sizeof((array_)[0])), \
              .values = (array_) }
#define KIT_AR_EQUAL(left_, right_)                              \
  kit_ar_equal_bytes(sizeof((left_).values[0]), (left_).size,    \
                     (left_).values, sizeof((right_).values[0]), \
                     (right_).size, (right_).values)
#define KIT_AR_COMPARE(left_, right_, compare_)              \
  kit_ar_compare(sizeof((left_).values[0]), (left_).size,    \
                 (left_).values, sizeof((right_).values[0]), \
                 (right_).size, (right_).values,             \
                 (kit_ar_compare_fn) (compare_))
#ifndef KIT_DISABLE_SHORT_NAMES
#  define ar_compare_fn kit_ar_compare_fn
#  define ar_equal_bytes kit_ar_equal_bytes
#  define ar_compare kit_ar_compare
#  define AR_MUT KIT_AR_MUT
#  define AR KIT_AR
#  define AR_MUT_WRAP KIT_AR_MUT_WRAP
#  define AR_WRAP KIT_AR_WRAP
#  define AR_EQUAL KIT_AR_EQUAL
#  define AR_COMPARE KIT_AR_COMPARE
#endif
#ifdef __cplusplus
}
#endif
#endif
/*********************************************************************
 *                                                                   *
 *    File:   ./source/kit/dynamic_array.h                           *
 *                                                                   *
 *********************************************************************/
#ifndef KIT_DYNAMIC_ARRAY_H
#define KIT_DYNAMIC_ARRAY_H
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
/*  Dynamic array type definition.
 */
#define KIT_DA(element_type_) \
  struct {                    \
    ptrdiff_t       capacity; \
    ptrdiff_t       size;     \
    element_type_  *values;   \
    kit_allocator_t alloc;    \
  }
/*  Initialize dynamic array.
 */
#define KIT_DA_INIT(array_, size_, alloc_) \
  kit_da_init((kit_da_void_t *) &(array_), \
              sizeof((array_).values[0]), (size_), (alloc_))
/*  Declare and initialize dynamic array.
 */
#define KIT_DA_CREATE(name_, element_type_, size_) \
  KIT_DA(element_type_) name_;                     \
  KIT_DA_INIT(name_, (size_), kit_alloc_default())
/*  Destroy dynamic array.
 */
#define KIT_DA_DESTROY(array_)                                 \
  do {                                                         \
    if ((array_).values != NULL)                               \
      kit_alloc_dispatch((array_).alloc, KIT_DEALLOCATE, 0, 0, \
                         (array_).values);                     \
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
    ptrdiff_t       kit_i_;                                    \
    ptrdiff_t const kit_index_back_ = (array_).size;           \
    ptrdiff_t const kit_indert_n_   = (index_);                \
    KIT_DA_RESIZE((array_), kit_index_back_ + 1);              \
    if (kit_index_back_ + 1 == (array_).size) {                \
      for (kit_i_ = kit_index_back_; kit_i_ > kit_indert_n_;   \
           kit_i_--)                                           \
        (array_).values[kit_i_] = (array_).values[kit_i_ - 1]; \
      (array_).values[kit_indert_n_] = (value_);               \
    }                                                          \
  } while (0)
/*  Erase a value from dynamic array.
 */
#define KIT_DA_ERASE(array_, index_)                  \
  do {                                                \
    ptrdiff_t i_;                                     \
    for (i_ = (index_) + 1; i_ < (array_).size; i_++) \
      (array_).values[i_ - 1] = (array_).values[i_];  \
    KIT_DA_RESIZE((array_), (array_).size - 1);       \
  } while (0)
typedef KIT_DA(char) kit_string_t;
#ifndef KIT_DISABLE_SHORT_NAMES
#  define da_void_t kit_da_void_t
#  define da_init kit_da_init
#  define da_resize kit_da_resize
#  define string_t kit_string_t
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
/*********************************************************************
 *                                                                   *
 *    File:   ./source/kit/string_ref.h                              *
 *                                                                   *
 *********************************************************************/
#ifndef KIT_STRING_REF_H
#define KIT_STRING_REF_H
#include <string.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef KIT_AR_MUT(char) kit_string_mut_t;
typedef KIT_AR(char) kit_string_ref_t;
typedef kit_string_mut_t kit_out_str_t;
typedef kit_string_ref_t kit_str_t;
#ifdef __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wunused-function"
#  pragma GCC diagnostic ignored "-Wunknown-pragmas"
#  pragma GCC            push_options
#  pragma GCC            optimize("O3")
#endif
static kit_str_t kit_str(ptrdiff_t const   size,
                         char const *const static_string) {
  kit_str_t const s = { .size = size, .values = static_string };
  return s;
}
/*  Make a barbarian string for C standard library functions.
 *  Not thread safe.
 *  Use with caution.
 */
static char const *kit_make_bs(kit_str_t const s) {
  static char buf[8][4096];
  static int  index = 0;
  ptrdiff_t   n     = s.size;
  if (n > 4095)
    n = 4095;
  memcpy(buf[index], s.values, n);
  buf[index][n]      = '\0';
  char const *result = buf[index];
  index              = (index + 1) % 8;
  return result;
}
#ifdef __GNUC__
#  pragma GCC            pop_options
#  pragma GCC diagnostic pop
#endif
#define KIT_SZ(static_str_) \
  kit_str(sizeof(static_str_) - 1, (static_str_))
#define KIT_WRAP_BS(string_) kit_str(strlen(string_), (string_))
#define KIT_WRAP_STR(string_) \
  kit_str((string_).size, (string_).values)
#ifndef KIT_DISABLE_SHORT_NAMES
#  define BS(string_) kit_make_bs(KIT_WRAP_STR(string_))
#  define string_mut_t kit_string_mut_t
#  define string_ref_t kit_string_ref_t
#  define out_str_t kit_out_str_t
#  define str_t kit_str_t
#  define SZ KIT_SZ
#  define WRAP_BS KIT_WRAP_BS
#  define WRAP_STR KIT_WRAP_STR
#endif
#ifdef __cplusplus
}
#endif
#endif
/*********************************************************************
 *                                                                   *
 *    File:   ./source/kit/file.h                                    *
 *                                                                   *
 *********************************************************************/
#ifndef KIT_FILE_H
#define KIT_FILE_H
#ifdef __cplusplus
extern "C" {
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
#  define KIT_PATH_DELIM_C '\\'
#  define KIT_PATH_DELIM "\\"
#  define KIT_ENV_HOME "USERPROFILE"
#else
#  define KIT_PATH_DELIM_C '/'
#  define KIT_PATH_DELIM "/"
#  define KIT_ENV_HOME "HOME"
#endif
typedef enum {
  KIT_PATH_NONE,
  KIT_PATH_FILE,
  KIT_PATH_FOLDER
} kit_path_type_t;
kit_string_t kit_path_norm(kit_str_t path, kit_allocator_t alloc);
kit_string_t kit_path_join(kit_str_t left, kit_str_t right,
                           kit_allocator_t alloc);
kit_string_t kit_path_user(kit_allocator_t alloc);
kit_string_t kit_path_cache(kit_allocator_t alloc);
kit_str_t kit_path_index(kit_str_t path, ptrdiff_t index);
kit_str_t kit_path_take(kit_str_t path, ptrdiff_t count);
kit_status_t kit_file_create_folder(kit_str_t path);
kit_status_t kit_file_create_folder_recursive(kit_str_t path);
kit_status_t kit_file_remove(kit_str_t path);
kit_status_t kit_file_remove_folder(kit_str_t path);
kit_status_t kit_file_remove_recursive(kit_str_t       path,
                                       kit_allocator_t alloc);
kit_path_type_t kit_path_type(kit_str_t path);
typedef struct {
  kit_status_t status;
  int64_t time_modified_sec;
  int32_t time_modified_nsec;
  int64_t size;
} kit_file_info_t;
kit_file_info_t kit_file_info(kit_str_t path);
typedef struct {
  kit_status_t status;
  KIT_DA(kit_string_t) files;
} kit_path_list_t;
kit_path_list_t kit_file_enum_folder(kit_str_t       path,
                                     kit_allocator_t alloc);
void kit_path_list_destroy(kit_path_list_t list);
#ifndef KIT_DISABLE_SHORT_NAMES
#  define path_norm kit_path_norm
#  define path_join kit_path_join
#  define path_user kit_path_user
#  define path_cache kit_path_cache
#  define path_index kit_path_index
#  define path_take kit_path_take
#  define file_create_folder kit_file_create_folder
#  define file_create_folder_recursive \
    kit_file_create_folder_recursive
#  define file_remove kit_file_remove
#  define file_remove_folder kit_file_remove_folder
#  define file_remove_recursive kit_file_remove_recursive
#  define path_type kit_path_type
#  define file_size_result_t kit_file_size_result_t
#  define file_size kit_file_size
#  define path_type_t kit_path_type_t
#  define path_list_t kit_path_list_t
#  define file_enum_folder kit_file_enum_folder
#  define path_list_destroy kit_path_list_destroy
#  define PATH_DELIM_C KIT_PATH_DELIM_C
#  define PATH_DELIM KIT_PATH_DELIM
#  define ENV_HOME KIT_ENV_HOME
#  define PATH_NONE KIT_PATH_NONE
#  define PATH_FILE KIT_PATH_FILE
#  define PATH_FOLDER KIT_PATH_FOLDER
#endif
#ifdef __cplusplus
}
#endif
#endif
/*********************************************************************
 *                                                                   *
 *    File:   ./source/kit/allocator.c                               *
 *                                                                   *
 *********************************************************************/
#include <assert.h>
#ifndef KIT_DISABLE_SYSTEM_MALLOC
#  include <stdlib.h>
#endif
static void *allocate(int request, void *state, ptrdiff_t size,
                      ptrdiff_t previous_size, void *pointer) {
#ifndef KIT_DISABLE_SYSTEM_MALLOC
  switch (request) {
    case KIT_ALLOCATE:
      assert(previous_size == 0);
      assert(pointer == NULL);
      return malloc(size);
    case KIT_DEALLOCATE:
      assert(size == 0);
      assert(pointer != NULL);
      free(pointer);
      return NULL;
    case KIT_REALLOCATE:
      /*  FIXME
       *  Not implemented.
       */
      assert(0);
      return NULL;
    default: assert(0);
  }
#else
  assert(0);
#endif
  return NULL;
}
#ifndef KIT_ENABLE_CUSTOM_ALLOC_DISPATCH
void *kit_alloc_dispatch(kit_allocator_t alloc, int request,
                         ptrdiff_t size, ptrdiff_t previous_size,
                         void *pointer) {
  assert(alloc.allocate != NULL);
  if (alloc.allocate == NULL)
    return NULL;
  return alloc.allocate(request, alloc.state, size, previous_size,
                        pointer);
}
#endif
kit_allocator_t kit_alloc_default(void) {
  kit_allocator_t alloc = { .state = NULL, .allocate = allocate };
  return alloc;
}
/*********************************************************************
 *                                                                   *
 *    File:   ./source/kit/array_ref.c                               *
 *                                                                   *
 *********************************************************************/
#include <string.h>
int kit_ar_equal_bytes(ptrdiff_t left_element_size,
                       ptrdiff_t left_size, void const *left_data,
                       ptrdiff_t right_element_size,
                       ptrdiff_t right_size, void const *right_data) {
  ptrdiff_t i;
  if (left_element_size != right_element_size)
    return 0;
  if (left_size != right_size)
    return 0;
  for (i = 0; i < left_size; i++)
    if (memcmp((char const *) left_data + i * left_element_size,
               (char const *) right_data + i * left_element_size,
               left_element_size) != 0)
      return 0;
  return 1;
}
int kit_ar_compare(ptrdiff_t left_element_size, ptrdiff_t left_size,
                   void const *left_data,
                   ptrdiff_t right_element_size, ptrdiff_t right_size,
                   void const       *right_data,
                   kit_ar_compare_fn compare) {
  ptrdiff_t i;
  if (left_element_size < right_element_size)
    return -1;
  if (left_element_size > right_element_size)
    return 1;
  for (i = 0; i < left_size && i < right_size; i++) {
    int const c = compare(
        (char const *) left_data + i * left_element_size,
        (char const *) right_data + i * left_element_size);
    if (c != 0)
      return c;
  }
  if (left_size < right_size)
    return -1;
  if (left_size > right_size)
    return 1;
  return 0;
}
/*********************************************************************
 *                                                                   *
 *    File:   ./source/kit/dynamic_array.c                           *
 *                                                                   *
 *********************************************************************/
#include <assert.h>
#include <string.h>
void kit_da_init(kit_da_void_t *array, ptrdiff_t element_size,
                 ptrdiff_t size, kit_allocator_t alloc) {
  assert(array != NULL);
  assert(element_size > 0);
  assert(size >= 0);
  assert(alloc.allocate != NULL);
  memset(array, 0, sizeof(kit_da_void_t));
  if (size > 0)
    array->values = kit_alloc_dispatch(alloc, KIT_ALLOCATE,
                                       element_size * size, 0, NULL);
  if (array->values != NULL) {
    array->capacity = size;
    array->size     = size;
  }
  array->alloc = alloc;
}
static ptrdiff_t eval_capacity(ptrdiff_t current_cap,
                               ptrdiff_t required_cap) {
  if (current_cap == 0)
    return required_cap;
  ptrdiff_t cap = current_cap;
  while (cap < required_cap) cap *= 2;
  return cap;
}
void kit_da_resize(kit_da_void_t *array, ptrdiff_t element_size,
                   ptrdiff_t size) {
  assert(array != NULL);
  assert(element_size > 0);
  assert(size >= 0);
  if (size <= array->capacity) {
    array->size = size;
  } else {
    ptrdiff_t capacity = eval_capacity(array->capacity, size);
    assert(array->alloc.allocate != NULL);
    void *bytes = kit_alloc_dispatch(
        array->alloc, KIT_ALLOCATE, element_size * capacity, 0, NULL);
    if (bytes != NULL) {
      if (array->size > 0)
        memcpy(bytes, array->values, element_size * array->size);
      if (array->values != NULL)
        kit_alloc_dispatch(array->alloc, KIT_DEALLOCATE, 0, 0,
                           array->values);
      array->capacity = capacity;
      array->size     = size;
      array->values   = bytes;
    }
  }
}
/*********************************************************************
 *                                                                   *
 *    File:   ./source/kit/file.c                                    *
 *                                                                   *
 *********************************************************************/
#include <assert.h>
#include <stdlib.h>
#include <string.h>
enum { PATH_BUF_SIZE = 4096 };
#if defined(_WIN32) && !defined(__CYGWIN__)
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN 1
#  endif
#  include <stdint.h>
#  include <Windows.h>
#else
#  include <dirent.h>
#  include <sys/stat.h>
#  include <unistd.h>
#endif
#ifdef __APPLE__
#  define st_mtim st_mtimespec
#endif
static int is_delim(char const c) {
  return c == '/' || c == '\\';
}
static kit_string_t kit_get_env_(char *const           name,
                                 kit_allocator_t const alloc) {
  char *const     val  = getenv(name);
  ptrdiff_t const size = val != NULL ? (ptrdiff_t) strlen(val) : 0;
  string_t result;
  DA_INIT(result, size, alloc);
  assert(result.size == size);
  if (result.size == size && size > 0)
    memcpy(result.values, val, result.size);
  else
    DA_RESIZE(result, 0);
  return result;
}
kit_string_t kit_path_norm(kit_str_t const       path,
                           kit_allocator_t const alloc) {
  str_t const parent = SZ("..");
  ptrdiff_t   i, i1, j;
  string_t norm;
  DA_INIT(norm, path.size, alloc);
  assert(norm.size == path.size);
  if (norm.size != path.size)
    return norm;
  memcpy(norm.values, path.values, path.size);
  for (i1 = 0, i = 0; i < path.size; i++) {
    if (!is_delim(path.values[i]))
      continue;
    str_t const s = { .size   = i - i1 - 1,
                      .values = path.values + i1 + 1 };
    if (AR_EQUAL(s, parent)) {
      int       have_parent = 0;
      ptrdiff_t i0          = 0;
      for (j = 0; j < i1; j++) {
        if (norm.values[j] != '\0')
          have_parent = 1;
        if (is_delim(norm.values[j]))
          i0 = j;
      }
      if (have_parent) {
        memset(norm.values + i0, '\0', i - i0);
        if (!is_delim(path.values[i0]))
          norm.values[i] = '\0';
      }
    }
    i1 = i;
  }
  ptrdiff_t size = 0;
  for (i = 0; i < norm.size; i++) {
    if (norm.values[i] != '\0') {
      if (is_delim(norm.values[i]))
        norm.values[size] = KIT_PATH_DELIM_C;
      else
        norm.values[size] = norm.values[i];
      size++;
    }
  }
  norm.size = size;
  return norm;
}
kit_string_t kit_path_join(kit_str_t const       left,
                           kit_str_t const       right,
                           kit_allocator_t const alloc) {
  ptrdiff_t   left_size    = left.size;
  ptrdiff_t   right_size   = right.size;
  char const *right_values = right.values;
  if (left_size > 0 && is_delim(left.values[left_size - 1]))
    left_size--;
  if (right_size > 0 && is_delim(right.values[0])) {
    right_size--;
    right_values++;
  }
  kit_string_t joined;
  DA_INIT(joined, left_size + right_size + 1, alloc);
  assert(joined.size == left_size + right_size + 1);
  if (joined.size != left_size + right_size + 1)
    return joined;
  memcpy(joined.values, left.values, left_size);
  joined.values[left_size] = KIT_PATH_DELIM_C;
  memcpy(joined.values + left_size + 1, right_values, right_size);
  return joined;
}
kit_string_t kit_path_user(kit_allocator_t const alloc) {
  kit_string_t user = kit_get_env_(KIT_ENV_HOME, alloc);
  if (user.size == 0) {
    DA_RESIZE(user, 1);
    if (user.size == 1)
      user.values[0] = '.';
  }
  return user;
}
kit_string_t kit_path_cache(kit_allocator_t alloc) {
  kit_string_t cache, user;
#if defined(_WIN32) && !defined(__CYGWIN__)
  cache = kit_get_env_("LOCALAPPDATA", alloc);
  if (cache.size != 0)
    return cache;
  DA_DESTROY(cache);
#endif
  cache = kit_get_env_("XDG_CACHE_HOME", alloc);
  if (cache.size != 0)
    return cache;
  DA_DESTROY(cache);
  user = kit_path_user(alloc);
  cache =
#ifdef __APPLE__
      kit_path_join(WRAP_STR(user), SZ("Library" PATH_DELIM "Caches"),
                    alloc);
#else
      kit_path_join(WRAP_STR(user), SZ(".cache"), alloc);
#endif
  DA_DESTROY(user);
  return cache;
}
kit_str_t kit_path_index(kit_str_t const path,
                         ptrdiff_t const index) {
  str_t s = { .size = 0, .values = NULL };
  ptrdiff_t i0 = 0;
  ptrdiff_t i  = 0;
  ptrdiff_t n  = 0;
  for (; i < path.size; i++) {
    if (!is_delim(path.values[i]))
      continue;
    if (i0 < i) {
      if (n++ == index) {
        s.values = path.values + i0;
        s.size   = i - i0;
        return s;
      }
    }
    i0 = i + 1;
  }
  if (n == index) {
    s.values = path.values + i0;
    s.size   = i - i0;
  }
  return s;
}
kit_str_t kit_path_take(kit_str_t const path, ptrdiff_t const count) {
  str_t s = { .size = 0, .values = path.values };
  ptrdiff_t i0 = 0;
  ptrdiff_t i  = 0;
  ptrdiff_t n  = 0;
  for (; i < path.size; i++) {
    if (!is_delim(path.values[i]))
      continue;
    if (i0 < i) {
      if (n++ == count) {
        s.size = i;
        return s;
      }
    }
    i0 = i + 1;
  }
  if (n == count)
    s.size = i;
  return s;
}
#if defined(_WIN32) && !defined(__CYGWIN__)
static void win32_prepare_path_(WCHAR *const    buf,
                                kit_str_t const path) {
  assert(path.size == 0 || path.values != NULL);
  assert(path.size + 5 < PATH_BUF_SIZE);
  memset(buf, 0, PATH_BUF_SIZE);
  buf[0] = L'\\';
  buf[1] = L'\\';
  buf[2] = L'?';
  buf[3] = L'\\';
  if (path.size > 0 && path.size + 5 < PATH_BUF_SIZE)
    for (ptrdiff_t i = 0; i < path.size; i++) {
      if (path.values[i] == '/')
        buf[4 + i] = L'\\';
      else
        buf[4 + i] = path.values[i];
    }
}
#  define PREPARE_PATH_BUF_   \
    WCHAR buf[PATH_BUF_SIZE]; \
    win32_prepare_path_(buf, path)
#else
static void unix_prepare_path_(char *const buf,
                               kit_str_t const path) {
  assert(path.size == 0 || path.values != NULL);
  assert(path.size + 1 < PATH_BUF_SIZE);
  memset(buf, 0, PATH_BUF_SIZE);
  if (path.size > 0 && path.size + 1 < PATH_BUF_SIZE)
    memcpy(buf, path.values, path.size);
}
#  define PREPARE_PATH_BUF_  \
    char buf[PATH_BUF_SIZE]; \
    unix_prepare_path_(buf, path)
#endif
kit_status_t kit_file_create_folder(kit_str_t const path) {
  PREPARE_PATH_BUF_;
#if defined(_WIN32) && !defined(__CYGWIN__)
  return CreateDirectoryW(buf, NULL) ? KIT_OK
                                     : KIT_ERROR_MKDIR_FAILED;
#else
  return mkdir(buf, 0755) == 0 ? KIT_OK : KIT_ERROR_MKDIR_FAILED;
#endif
}
kit_status_t kit_file_create_folder_recursive(kit_str_t const path) {
  ptrdiff_t i;
  for (i = 0;; i++) {
    str_t const part = kit_path_take(path, i);
    int const   type = kit_path_type(part);
    if (type == KIT_PATH_FILE)
      return KIT_ERROR_FILE_ALREADY_EXISTS;
    if (type == KIT_PATH_NONE) {
      kit_status_t const s = kit_file_create_folder(part);
      if (s != KIT_OK)
        return s;
    }
    if (part.size == path.size)
      break;
  }
  return KIT_OK;
}
kit_status_t kit_file_remove(kit_str_t const path) {
  PREPARE_PATH_BUF_;
#if defined(_WIN32) && !defined(__CYGWIN__)
  return DeleteFileW(buf) ? KIT_OK : KIT_ERROR_UNLINK_FAILED;
#else
  return unlink(buf) == 0 ? KIT_OK : KIT_ERROR_UNLINK_FAILED;
#endif
}
kit_status_t kit_file_remove_folder(kit_str_t const path) {
  PREPARE_PATH_BUF_;
#if defined(_WIN32) && !defined(__CYGWIN__)
  return RemoveDirectoryW(buf) ? KIT_OK : KIT_ERROR_RMDIR_FAILED;
#else
  return rmdir(buf) == 0 ? KIT_OK : KIT_ERROR_RMDIR_FAILED;
#endif
}
kit_status_t kit_file_remove_recursive(kit_str_t const       path,
                                       kit_allocator_t const alloc) {
  int       type = kit_path_type(path);
  ptrdiff_t i;
  switch (type) {
    case KIT_PATH_FILE: return kit_file_remove(path);
    case KIT_PATH_FOLDER: {
      kit_path_list_t list = kit_file_enum_folder(path, alloc);
      if (list.status != KIT_OK) {
        kit_path_list_destroy(list);
        return list.status;
      }
      for (i = 0; i < list.files.size; i++) {
        str_t const s = { .size   = list.files.values[i].size,
                          .values = list.files.values[i].values };
        kit_file_remove_recursive(s, alloc);
      }
      kit_path_list_destroy(list);
      return kit_file_remove_folder(path);
    }
    default:;
  }
  return KIT_ERROR_FILE_DO_NOT_EXIST;
}
kit_path_type_t kit_path_type(kit_str_t const path) {
  PREPARE_PATH_BUF_;
#if defined(_WIN32) && !defined(__CYGWIN__)
  if (PathFileExistsW(buf)) {
    if ((GetFileAttributesW(buf) & FILE_ATTRIBUTE_DIRECTORY) != 0)
      return KIT_PATH_FOLDER;
    else
      return KIT_PATH_FILE;
  }
#else
  struct stat info;
  if (stat(buf, &info) == 0) {
    if (S_ISREG(info.st_mode))
      return KIT_PATH_FILE;
    if (S_ISDIR(info.st_mode))
      return KIT_PATH_FOLDER;
  }
#endif
  return KIT_PATH_NONE;
}
kit_file_info_t kit_file_info(kit_str_t const path) {
  kit_file_info_t result;
  memset(&result, 0, sizeof result);
  PREPARE_PATH_BUF_;
#if defined(_WIN32) && !defined(__CYGWIN__)
  HANDLE f = CreateFileW(buf, GENERIC_READ, FILE_SHARE_READ, NULL,
                         OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (f != INVALID_HANDLE_VALUE) {
    FILETIME ft;
    if (GetFileTime(f, NULL, NULL, &ft) != 0) {
      uint64_t const nsec100 = (((uint64_t) ft.dwHighDateTime)
                                << 32) |
                               (uint64_t) ft.dwLowDateTime;
      result.time_modified_sec  = (int64_t) (nsec100 / 10000000);
      result.time_modified_nsec = (int32_t) (100 *
                                             (nsec100 % 10000000));
    } else {
      assert(0);
    }
    DWORD high;
    DWORD low = GetFileSize(f, &high);
    result.size   = (int64_t) ((((uint64_t) high) << 32) |
                             (uint64_t) low);
    result.status = KIT_OK;
    CloseHandle(f);
    return result;
  }
#else
  struct stat info;
  if (stat(buf, &info) == 0 && S_ISREG(info.st_mode)) {
    result.size = (int64_t) info.st_size;
#  ifndef st_mtime
    /*  No support for nanosecond timestamps.
     */
    result.time_modified_sec = (int64_t) info.st_mtime;
#  else
    result.time_modified_sec  = (int64_t) info.st_mtim.tv_sec;
    result.time_modified_nsec = (int32_t) info.st_mtim.tv_nsec;
#  endif
    result.status = KIT_OK;
    return result;
  }
#endif
  result.status = KIT_ERROR_FILE_DO_NOT_EXIST;
  return result;
}
kit_path_list_t kit_file_enum_folder(kit_str_t const       path,
                                     kit_allocator_t const alloc) {
  PREPARE_PATH_BUF_;
  kit_path_list_t result = { .status = KIT_OK };
  DA_INIT(result.files, 0, alloc);
#if defined(_WIN32) && !defined(__CYGWIN__)
  if (path.size + 7 >= PATH_BUF_SIZE) {
    result.status = KIT_ERROR_PATH_TOO_LONG;
    return result;
  }
  buf[path.size + 4] = '\\';
  buf[path.size + 5] = '*';
  WIN32_FIND_DATAW data;
  HANDLE           find = FindFirstFileW(buf, &data);
  if (find == INVALID_HANDLE_VALUE)
    return result;
  do {
    ptrdiff_t const n = result.files.size;
    DA_RESIZE(result.files, n + 1);
    if (result.files.size != n + 1) {
      result.status = KIT_ERROR_BAD_ALLOC;
      break;
    }
    ptrdiff_t size = 0;
    while (size < MAX_PATH && data.cFileName[size] != L'\0') size++;
    DA_INIT(result.files.values[n], size, alloc);
    if (result.files.values[n].size != size) {
      DA_RESIZE(result.files, n);
      result.status = KIT_ERROR_BAD_ALLOC;
      break;
    }
    for (ptrdiff_t i = 0; i < size; i++)
      result.files.values[n].values[i] = data.cFileName[i];
  } while (FindNextFileW(find, &data) != 0);
  FindClose(find);
#else
  DIR *directory = opendir(buf);
  if (directory == NULL)
    return result;
  for (;;) {
    struct dirent *entry = readdir(directory);
    if (entry == NULL)
      break;
    if (entry->d_name[0] == '.')
      continue;
    ptrdiff_t const n = result.files.size;
    DA_RESIZE(result.files, n + 1);
    if (result.files.size != n + 1) {
      result.status = KIT_ERROR_BAD_ALLOC;
      break;
    }
    ptrdiff_t const size = (ptrdiff_t) strlen(entry->d_name);
    DA_INIT(result.files.values[n], size, alloc);
    if (result.files.values[n].size != size) {
      DA_RESIZE(result.files, n);
      result.status = KIT_ERROR_BAD_ALLOC;
      break;
    }
    if (size > 0)
      memcpy(result.files.values[n].values, entry->d_name, size);
  }
  closedir(directory);
#endif
  return result;
}
void kit_path_list_destroy(kit_path_list_t list) {
  ptrdiff_t i;
  for (i = 0; i < list.files.size; i++)
    DA_DESTROY(list.files.values[i]);
  DA_DESTROY(list.files);
}
#endif
