#if 0
gcc -fsanitize=address,undefined,leak build.c -o build_c && ./build_c
exit
#endif

/*********************************************************************
 *                                                                   *
 *    KIT Library                                                    *
 *                                                                   *
 *********************************************************************/

//  Just paste the Kit library code here.

#include "include/kit.inl.h"

/*********************************************************************
 *                                                                   *
 *    BUILD_C                                                        *
 *                                                                   *
 *    Experimental pure C build system.                              *
 *    Work in progress.                                              *
 *                                                                   *
 *********************************************************************/

#include <stdio.h>

#define DL PATH_DELIM

enum { MAX_STR = 4096, MAX_DEPENDENCIES = 100, MAX_UNITS = 400 };

typedef DA(ptrdiff_t) index_array_t;

typedef enum { UNIT_NONE, UNIT_HEADER, UNIT_SOURCE } unit_type_t;

typedef struct {
  string_t      path;
  unit_type_t   type;
  index_array_t dependencies;
} unit_t;

typedef DA(unit_t) unit_array_t;

unit_array_t units;

kit_allocator_t ALLOC;

int is_header(str_t file) {
  ptrdiff_t const n = file.size;
  return n > 2 && file.values[n - 2] == '.' &&
         file.values[n - 1] == 'h';
}

int is_source(str_t file) {
  ptrdiff_t const n = file.size;
  return n > 2 && file.values[n - 2] == '.' &&
         file.values[n - 1] == 'c';
}

unit_type_t unit_type(str_t file) {
  if (is_header(file))
    return UNIT_HEADER;
  if (is_source(file))
    return UNIT_SOURCE;
  return UNIT_NONE;
}

void enum_sources(str_t folder) {
  path_list_t const list = file_enum_folder(folder, ALLOC);

  for (ptrdiff_t i = 0; i < list.files.size; i++) {
    str_t       item  = WRAP_STR(list.files.values[i]);
    string_t    file  = path_join(folder, item, ALLOC);
    path_type_t type  = path_type(WRAP_STR(file));
    unit_type_t utype = unit_type(WRAP_STR(file));

    switch (type) {
      case PATH_FILE:
        if (utype != UNIT_NONE) {
          unit_t unit = { .path = file, .type = utype };
          DA_INIT(unit.dependencies, 0, ALLOC);
          DA_APPEND(units, unit);
        } else
          DA_DESTROY(file);
        break;

      case PATH_FOLDER: enum_sources(WRAP_STR(file));
      default: DA_DESTROY(file);
    }
  }

  path_list_destroy(list);
}

void skip_comment(FILE *f) {
  char c = fgetc(f);
  if (c != '/' && c != '*')
    return;
  if (c == '/')
    while (c != '\n') {
      c = fgetc(f);
      if (c == '\\')
        fgetc(f);
    }
  else
    while (c != '*') {
      c = fgetc(f);
      if (c == '*' && fgetc(f) == '/')
        break;
    }
}

typedef struct {
  int      ok;
  int      is_local;
  string_t path;
} include_t;

include_t parse_include(FILE *f) {
  include_t result = { .ok = 0 };
  char      c      = ' ';
  while (c == ' ') c = fgetc(f);
  if (c != '#')
    return result;
  c = ' ';
  while (c == ' ') c = fgetc(f);
  if (c != 'i')
    return result;
  char s[] = "nclude";
  for (int i = 0; i + 1 < sizeof s; i++)
    if (fgetc(f) != s[i])
      return result;
  c = ' ';
  while (c == ' ') c = fgetc(f);
  if (c != '"' && c != '<')
    return result;
  if (c == '"')
    result.is_local = 1;
  else
    result.is_local = 0;
  DA_INIT(result.path, 1, ALLOC);
  for (;;) {
    char z = fgetc(f);
    if (c == '"' && z == '"')
      break;
    if (c == '<' && z == '>')
      break;
    if (z == '\n' || z == '\r' || z == '\0') {
      DA_DESTROY(result.path);
      return result;
    }
    result.path.values[result.path.size - 1] = z;
    DA_RESIZE(result.path, result.path.size + 1);
  }
  DA_RESIZE(result.path, result.path.size - 1);
  if (result.path.size == 0) {
    DA_DESTROY(result.path);
    return result;
  }
  c = ' ';
  while (c == ' ') c = fgetc(f);
  if (c != '\n' && c != '\r') {
    DA_DESTROY(result.path);
    return result;
  }
  result.ok = 1;
  return result;
}

str_t base_path(str_t path) {
  ptrdiff_t n = 0;
  while (path_index(path, n).size != 0) n++;
  return path_take(path, n - 2);
}

void parse_unit(unit_t *u) {
  FILE *f = fopen(BS(u->path), "rt");
  if (f == NULL) {
    printf("Can't read %s\n", BS(u->path));
    return;
  }

  str_t base = base_path(WRAP_STR(u->path));

  while (!feof(f)) {
    char c = fgetc(f);
    switch (c) {
      case '/': skip_comment(f); break;
      case '\n': {
        include_t inc = parse_include(f);
        if (inc.ok) {
          if (inc.is_local) {
            string_t temp     = path_join(base, WRAP_STR(inc.path),
                                          ALLOC);
            string_t inc_path = path_norm(WRAP_STR(temp), ALLOC);
            for (ptrdiff_t i = 0; i < units.size; i++)
              if (AR_EQUAL(units.values[i].path, inc_path)) {
                ptrdiff_t k = u->dependencies.size;
                DA_RESIZE(u->dependencies, k + 1);
                u->dependencies.values[k] = i;
                break;
              }
            DA_DESTROY(temp);
            DA_DESTROY(inc_path);
          }
          DA_DESTROY(inc.path);
        }
      } break;
      default:;
    }
  }

  fclose(f);
}

void parse_units(void) {
  for (ptrdiff_t i = 0; i < units.size; i++)
    parse_unit(units.values + i);
}

int main(int argc, char **argv) {
  ALLOC = kit_alloc_default();
  DA_INIT(units, 0, ALLOC);

  enum_sources(SZ("." DL "src"));
  enum_sources(SZ("." DL "source"));
  parse_units();

  for (ptrdiff_t i = 0; i < units.size; i++) {
    unit_t *u = units.values + i;
    if (u->type == UNIT_HEADER)
      printf("[ Header ] ");
    else
      printf("[ Source ] ");
    printf("%s\n", BS(u->path));
    for (ptrdiff_t j = 0; j < u->dependencies.size; j++)
      printf(" ->        %s\n",
             BS(units.values[u->dependencies.values[j]].path));
  }

  for (ptrdiff_t i = 0; i < units.size; i++) {
    DA_DESTROY(units.values[i].path);
    DA_DESTROY(units.values[i].dependencies);
  }
  DA_DESTROY(units);

  return 0;
}
