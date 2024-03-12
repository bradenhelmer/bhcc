// bhcc compiler main entry point.
#include "io.h"
#include <stdio.h>

typedef struct comp {
  size_t file_len;
  const char *file_name;
  const char *file_src;
} compiler;

static void init_compiler(compiler *c, int arg_count, char **args) {
  c->file_name = args[1];
  c->file_src = map_source_file(c->file_name, &(c->file_len));
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage -> bhcc FILE ARGS\n");
    return 1;
  }

  // Init compiler with source file.
  compiler bhcc;
  init_compiler(&bhcc, argc, argv);

  return 0;
}
