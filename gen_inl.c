#if 0
gcc gen_inl.c -o gen_inl && ./gen_inl
exit
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char const *const SOURCES[] = {
  "./source/kit/status.h",        "./source/kit/allocator.h",
  "./source/kit/array_ref.h",     "./source/kit/dynamic_array.h",
  "./source/kit/string_ref.h",    "./source/kit/file.h",
  "./source/kit/allocator.c",     "./source/kit/array_ref.c",
  "./source/kit/dynamic_array.c", "./source/kit/file.c"
};

char const *repeat(int n, char c) {
  static char buf[200];
  for (int i = 0; i < n && i < 199; i++) buf[i] = c;
  buf[n] = '\0';
  return buf;
}

int skip_whitespaces(char const *const line, int i) {
  while (line[i] == ' ') i++;
  return i;
}

int skip(char const *const line, int i, char const *const s) {
  int j = 0;
  while (s[j] != '\0') {
    if (line[i] != s[j])
      return -1;
    i++;
    j++;
  }
  return i;
}

int is_local_include(char const *const line) {
  if (line[0] != '#')
    return 0;
  int i = skip_whitespaces(line, 1);
  i     = skip(line, i, "include");
  if (i == -1)
    return 0;
  i = skip_whitespaces(line, i);
  if (line[i] != '"')
    return 0;
  return 1;
}

int is_empty_line(char const *const line) {
  int i = skip_whitespaces(line, 0);
  return line[i] == '\0' || line[i] == '\n';
}

int write_file(FILE *out, char const *const source) {
  assert(out != NULL);
  assert(source != NULL);

  FILE *in = fopen(source, "rt");

  if (in == NULL) {
    fprintf(stderr, "File not found: %s\n", source);
    return 1;
  }

  fprintf(out, "/*%s*\n", repeat(70, '*'));
  fprintf(out, " *%s*\n", repeat(70, ' '));
  fprintf(out, " *%s*\n", repeat(70, ' '));
  fprintf(out, " *    FILE:   %s%s*\n", source,
          repeat(58 - strlen(source), ' '));
  fprintf(out, " *%s*\n", repeat(70, ' '));
  fprintf(out, " *%s*\n", repeat(70, ' '));
  fprintf(out, " *%s*/\n", repeat(70, '*'));

  char line[200];
  char buf[400];
  int  prev_empty = 0;

  while (fgets(line, 199, in) != NULL) {
    int empty = is_empty_line(line) || is_local_include(line);

    if (empty) {
      if (!prev_empty)
        fprintf(out, "\n");
    } else {
      int j = 0;
      for (int i = 0; line[i] != '\0'; i++) {
        if (line[i] == '\t') {
          buf[j++] = ' ';
          buf[j++] = ' ';
        } else if (line[i] != '\n' && line[i] != '\r')
          buf[j++] = line[i];
      }
      buf[j] = '\0';
      fprintf(out, "%s\n", buf);
    }

    prev_empty = empty;
  }

  fclose(in);
  fprintf(out, "\n");
  return 0;
}

int main(int argc, char **argv) {
  char const *const out_file = "./include/kit.inl.h";
  FILE             *out      = fopen(out_file, "wt");
  if (out == NULL) {
    fprintf(stderr, "Can't write: %s\n", out_file);
    return 1;
  }

  fprintf(out, "#ifndef KIT_INL_H\n");
  fprintf(out, "#define KIT_INL_H\n");
  fprintf(out, "\n");

  for (int i = 0; i < sizeof SOURCES / sizeof *SOURCES; i++)
    if (write_file(out, SOURCES[i]) != 0) {
      fclose(out);
      return 1;
    }

  fprintf(out, "#endif\n");

  fclose(out);
  return 0;
}
