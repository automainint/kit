#include "file.h"

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

static int is_delim(char const c) {
  return c == '/' || c == '\\';
}

kit_string_t kit_path_norm(kit_str_t const       path,
                           kit_allocator_t const alloc) {
  str_t const parent = SZ("..");

  string_t norm;
  DA_INIT(norm, path.size, alloc);
  assert(norm.size == path.size);

  if (norm.size != path.size)
    return norm;

  memcpy(norm.values, path.values, path.size);

  for (ptrdiff_t i1 = 0, i = 0; i < path.size; i++) {
    if (!is_delim(path.values[i]))
      continue;

    str_t const s = { .size   = i - i1 - 1,
                      .values = path.values + i1 + 1 };
    if (AR_EQUAL(s, parent)) {
      int       have_parent = 0;
      ptrdiff_t i0          = 0;

      for (ptrdiff_t j = 0; j < i1; j++) {
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

  for (ptrdiff_t i = 0; i < norm.size; i++) {
    if (norm.values[i] != '\0') {
      if (is_delim(norm.values[i]))
        norm.values[size] = KIT_PATH_DELIM;
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
  joined.values[left_size] = KIT_PATH_DELIM;
  memcpy(joined.values + left_size + 1, right_values, right_size);

  return joined;
}

kit_string_t kit_path_user(kit_allocator_t const alloc) {
  char           *home = getenv(KIT_ENV_HOME);
  ptrdiff_t const size = home != NULL ? (ptrdiff_t) strlen(home) : 0;

  string_t user;
  DA_INIT(user, size, alloc);
  assert(user.size == size);

  if (user.size > 0)
    memcpy(user.values, home, user.size);
  else {
    DA_RESIZE(user, 1);
    assert(user.size == 1);

    if (user.size == 1)
      user.values[0] = '.';
  }

  return user;
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
static void unix_prepare_path_(char *const     buf,
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
  for (ptrdiff_t i = 0;; i++) {
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
  int type = kit_path_type(path);

  switch (type) {
    case KIT_PATH_FILE: return kit_file_remove(path);

    case KIT_PATH_FOLDER: {
      kit_path_list_t list = kit_file_enum_folder(path, alloc);
      if (list.status != KIT_OK) {
        kit_path_list_destroy(list);
        return list.status;
      }
      for (ptrdiff_t i = 0; i < list.files.size; i++) {
        str_t const s = { .size   = list.files.values[i].size,
                          .values = list.files.values[i].values };
        kit_file_remove_recursive(s, alloc);
      }
      kit_path_list_destroy(list);
      return kit_file_remove_folder(path);
    }

    default:;
  }

  return KIT_ERROR_FILE_DO_NOT_EXISTS;
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

kit_file_size_result_t kit_file_size(kit_str_t const path) {
  kit_file_size_result_t result;
  memset(&result, 0, sizeof result);

  PREPARE_PATH_BUF_;

#if defined(_WIN32) && !defined(__CYGWIN__)
  HFILE f = CreateFileW(buf, GENERIC_READ, FILE_SHARE_READ, NULL,
                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (f != INVALID_HANDLE_VALUE) {
    DWORD high;
    DWORD low = GetFileSize(f, &high);
    CloseHandle(f);

    result.status = KIT_OK;
    result.size   = (((uint64_t) high) << 32) | (uint64_t) low;
    return result;
  }
#else
  struct stat info;
  if (stat(buf, &info) == 0) {
    result.status = KIT_OK;
    result.size   = (uint64_t) info.st_size;
    return result;
  }
#endif

  result.status = KIT_ERROR_FILE_DO_NOT_EXISTS;
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
    ptrdiff_t const n = list.size;
    DA_RESIZE(list, n + 1);
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
  for (ptrdiff_t i = 0; i < list.files.size; i++)
    DA_DESTROY(list.files.values[i]);
  DA_DESTROY(list.files);
}
