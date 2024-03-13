// ast.h
// ~~~~~
// AST structure and routine defintions.
#ifndef BHCC_AST_H
#define BHCC_AST_H

typedef struct {
} func_decl;

typedef struct {
} var_decl;

typedef struct {
  func_decl *decls;
  var_decl *globals;
} program;

#endif
