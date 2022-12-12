#ifndef KIT_FILE_H
#define KIT_FILE_H

#include "dynamic_array.h"
#include "status.h"
#include "string_ref.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#  define KIT_PATH_DELIM '\\'
#  define KIT_ENV_HOME "USERPROFILE"
#else
#  define KIT_PATH_DELIM '/'
#  define KIT_ENV_HOME "HOME"
#endif

typedef enum {
  KIT_PATH_NONE,
  KIT_PATH_FILE,
  KIT_PATH_FOLDER
} kit_path_type_t;

enum { KIT_FILE_SIZE_ERROR = -1 };

kit_string_t kit_path_norm(kit_str_t path, kit_allocator_t alloc);

kit_string_t kit_path_join(kit_str_t left, kit_str_t right,
                           kit_allocator_t alloc);

kit_string_t kit_path_user(kit_allocator_t alloc);

kit_str_t kit_path_index(kit_str_t path, ptrdiff_t index);

kit_str_t kit_path_take(kit_str_t path, ptrdiff_t count);

kit_status_t kit_file_create_folder(kit_str_t path);

kit_status_t kit_file_create_folder_recursive(kit_str_t path);

kit_status_t kit_file_remove(kit_str_t path);

kit_status_t kit_file_remove_folder(kit_str_t path);

kit_status_t kit_file_remove_recursive(kit_str_t       path,
                                       kit_allocator_t alloc);

kit_path_type_t kit_path_type(kit_str_t path);

ptrdiff_t kit_file_size(kit_str_t path);

typedef KIT_DA(kit_string_t) kit_path_list_t;

kit_path_list_t kit_file_enum_folder(kit_str_t       path,
                                     kit_allocator_t alloc);

void kit_path_list_destroy(kit_path_list_t list);

#ifndef KIT_DISABLE_SHORT_NAMES
#  define path_norm kit_path_norm
#  define path_join kit_path_join
#  define path_user kit_path_user
#  define path_index kit_path_index
#  define path_take kit_path_take
#  define file_create_folder kit_file_create_folder
#  define file_create_folder_recursive \
    kit_file_create_folder_recursive
#  define file_remove kit_file_remove
#  define file_remove_folder kit_file_remove_folder
#  define file_remove_recursive kit_file_remove_recursive
#  define path_type kit_path_type
#  define file_size kit_file_size
#  define path_type_t kit_path_type_t
#  define path_list_t kit_path_list_t
#  define file_enum_folder kit_file_enum_folder
#  define path_list_destroy kit_path_list_destroy
#endif

#ifdef __cplusplus
}
#endif

#endif
