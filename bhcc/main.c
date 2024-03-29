// bhcc compiler main entry point.
#include "ast.h"
#include "compiler.h"
#include "error.h"
#include "util/vec.h"

// "Public" core methods for each phase
extern char *map_source_file(const char *, size_t *);
extern void parse_program(compiler *);

// Get compiler ready
static void init_compiler(compiler *c, int arg_count, char **args) {
  c->file_name = args[1];
  c->file_src = map_source_file(c->file_name, &(c->file_len));
}

int main(int argc, char **argv) {
  if (argc < 2) {
    bhcc_errorln_simple("Usage -> bhcc FILE ARGS\n");
  }
  // Init compiler with source file.
  compiler bhcc;
  init_compiler(&bhcc, argc, argv);
  parse_program(&bhcc);

  return 0;
}
