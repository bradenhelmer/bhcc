// tokens.h
// ~~~~~~~~
// Token related things.
#ifndef BHCC_TOKENS_H
#define BHCC_TOKENS_H
#include <stddef.h>
#include <stdio.h>
#include <string.h>

typedef enum {
#define TOKEN(X) X,
#include "tok_def.h"
  NUM_TOKENS
} tok_kind;

typedef enum { U, L, UL, LL, ULL, F, D } num_lit_spec;

typedef struct {
  char *start;
  char *end;
  tok_kind kind;
  num_lit_spec spec;
} token;

static size_t get_token_length(token *tok) {
  return (size_t)(tok->end - tok->start) + 1;
}

static void print_token(token *tok) {
  const size_t len = get_token_length(tok);
  char tok_str[len+1];
  memcpy(tok_str, tok->start, sizeof(char) * len);
  tok_str[len] = '\0';
  printf("%s\n", tok_str);
}

#endif
