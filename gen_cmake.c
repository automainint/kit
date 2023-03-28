#if 0
gcc -fsanitize=address,leak gen_cmake.c -o gen_cmake && ./gen_cmake
exit
#endif

#include "include/kit.inl.h"

#include <stdio.h>

kit_allocator_t ALLOC;

typedef struct {
  kit_status_t status;
  int          have_cmakelists;
} gen_result_t;

int is_header(str_t const file) {
  return file.size > 2 && file.values[file.size - 2] == '.' &&
         file.values[file.size - 1] == 'h';
}

int is_source(str_t const file) {
  return file.size > 2 && file.values[file.size - 2] == '.' &&
         file.values[file.size - 1] == 'c';
}

gen_result_t gen_cmakelists_for(str_t const target,
                                str_t const folder) {
  gen_result_t result = { KIT_OK, 0 };

  path_list_t const list = file_enum_folder(folder, ALLOC);

  if (list.status != KIT_OK) {
    printf("Error: file_enum_folder failed with code %d.\n",
           list.status);
    result.status = list.status;
    return result;
  }

  int have_headers = 0;
  int have_sources = 0;
  int have_folders = 0;

  FILE *out = NULL;

  string_t cmakelists_path = path_join(folder, SZ("CMakeLists.txt"),
                                       ALLOC);

  DA_APPEND(cmakelists_path, '\0');

  if (cmakelists_path.size == 0 ||
      cmakelists_path.values[cmakelists_path.size - 1] != '\0') {
    result.status          = KIT_ERROR_BAD_ALLOC;
    result.have_cmakelists = 0;
    DA_DESTROY(cmakelists_path);
    path_list_destroy(list);
    return result;
  }

#define OPEN_CMAKELISTS                          \
  do {                                           \
    if (out == NULL) {                           \
      out = fopen(cmakelists_path.values, "wt"); \
      if (out == NULL) {                         \
        result.status          = 101;            \
        result.have_cmakelists = 0;              \
        DA_DESTROY(cmakelists_path);             \
        path_list_destroy(list);                 \
        return result;                           \
      }                                          \
    }                                            \
  } while (0)

#define CHECK_TYPE(type_)                                             \
  string_t const full_path_ = path_join(folder, WRAP_STR(item),       \
                                        ALLOC);                       \
  int            check_ = path_type(WRAP_STR(full_path_)) == (type_); \
  DA_DESTROY(full_path_);                                             \
  if (!check_)                                                        \
  continue

  for (ptrdiff_t i = 0; i < list.files.size; i++) {
    str_t const item = WRAP_STR(list.files.values[i]);

    CHECK_TYPE(PATH_FILE);

    if (!is_header(item))
      continue;

    OPEN_CMAKELISTS;

    if (!have_headers)
      fprintf(out, "target_sources(\n  %*s\n    PUBLIC",
              (int) target.size, BS(target));

    fprintf(
        out,
        "\n      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/%*s>",
        (int) item.size, BS(item));

    have_headers = 1;
  }

  ptrdiff_t const LINE_LIMIT  = 80;
  ptrdiff_t       line_length = LINE_LIMIT;

  for (ptrdiff_t i = 0; i < list.files.size; i++) {
    str_t const item = WRAP_STR(list.files.values[i]);

    CHECK_TYPE(PATH_FILE);

    if (!is_source(item))
      continue;

    OPEN_CMAKELISTS;

    if (!have_headers && !have_sources)
      fprintf(out, "target_sources(\n  %*s\n    PRIVATE",
              (int) target.size, BS(target));
    else if (!have_sources)
      fprintf(out, "\n    PRIVATE");

    if (line_length + item.size > LINE_LIMIT) {
      fprintf(out, "\n     ");
      line_length = 5;
    }

    fprintf(out, " %*s", (int) item.size, BS(item));
    line_length += 1 + item.size;

    have_sources = 1;
  }

  if (have_headers || have_sources)
    fprintf(out, ")\n");

  for (ptrdiff_t i = 0; i < list.files.size; i++) {
    str_t const item = WRAP_STR(list.files.values[i]);

    CHECK_TYPE(PATH_FOLDER);

    string_t const     subfolder = path_join(folder, item, ALLOC);
    gen_result_t const r         = gen_cmakelists_for(target,
                                                      WRAP_STR(subfolder));
    DA_DESTROY(subfolder);
    if (r.status != KIT_OK) {
      result.status = r.status;
      break;
    }

    if (!r.have_cmakelists)
      continue;

    OPEN_CMAKELISTS;

    if (have_headers || have_sources)
      fprintf(out, "\n");

    fprintf(out, "add_subdirectory(%*s)\n", (int) item.size,
            BS(item));

    have_folders = 1;
  }

  if (out != NULL)
    fclose(out);

  DA_DESTROY(cmakelists_path);
  path_list_destroy(list);

  if (result.status == KIT_OK)
    result.have_cmakelists = have_headers || have_sources ||
                             have_folders;

  if (!result.have_cmakelists && out != NULL)
    file_remove(WRAP_STR(cmakelists_path));

  return result;
}

int main(int argc, char **argv) {
  ALLOC = kit_alloc_default();

  str_t const targets[] = { SZ("kit"), SZ("kit_test"),
                            SZ("kit_test_suite") };

  str_t const folders[] = {
    SZ("." PATH_DELIM "source" PATH_DELIM "kit"),
    SZ("." PATH_DELIM "source" PATH_DELIM "kit_test"),
    SZ("." PATH_DELIM "source" PATH_DELIM "test/unittests")
  };

  int ok = 1;

  for (ptrdiff_t i = 0; i < sizeof folders / sizeof *folders; i++)
    ok = ok &&
         gen_cmakelists_for(targets[i], folders[i]).status == KIT_OK;

  return ok ? 0 : 1;
}
