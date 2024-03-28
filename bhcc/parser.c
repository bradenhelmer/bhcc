// parser.c
// ~~~~~~~~
// Parsing routines
#include "compiler.h"
#include "tokens.h"

extern void lex(token *);
extern void init_lexer(char *);

static token curr_token;

static void parse_translation_unit(program *ast);

static void parse_translation_unit(program *ast) {
  puts("Parsing Translation Unit...");
}

void parse_program(compiler *c) {

  init_token(&curr_token);
  init_lexer(&c->file_src[0]);

  while (curr_token.kind != BHCC_EOF) {
	lex(&curr_token);
	print_token(&curr_token, 0);
  }
}
