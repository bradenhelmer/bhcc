// parser.c
// ~~~~~~~~
// Parsing routines
#include "tokens.h"
#include "compiler.h"

extern char *lex_next_token(token *, char *);
extern void __init_kw_table();

void parse_program(compiler *c) {
  __init_kw_table();

  char *curr_char;
  token curr_token;
  init_token(&curr_token);
  curr_char = &c->file_src[0];
  while (curr_token.kind != BHCC_EOF) {
    curr_char = lex_next_token(&curr_token, curr_char);
    if (curr_token.kind != BHCC_EOF)
      print_token(&curr_token, 1);
  }
}
