// compiler.h
// ~~~~~~~~~~
// Core compiler driver definitions.
#ifndef BHCC_COMPILER_H
#define BHCC_COMPILER_H
#include "ast.h"
#include <unistd.h>

typedef struct {
  size_t file_len;
  const char *file_name;
  char *file_src;
  program ast;
} compiler;

#endif
