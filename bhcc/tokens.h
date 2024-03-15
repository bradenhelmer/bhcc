// tokens.h
// ~~~~~~~~
// Token related things.
#ifndef BHCC_TOKENS_H
#define BHCC_TOKENS_H
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
#define TOKEN(X) X,
#include "tok_def.h"
  NUM_TOKENS
} tok_kind;

const char *keywords[] = {
#define KEYWORD(X) #X,
#include "tok_def.h"
};

typedef enum { U, L, UL, LL, ULL } int_lit_spec;
typedef enum { F, D, LD, } fp_lit_spec;

typedef struct {
  char *start;
  char *end;
  tok_kind kind;
  int_lit_spec i_spec;
  fp_lit_spec fp_spec;
} token;

void init_token(token *tok) {
  tok->kind = UNKNOWN;
  tok->start = NULL;
  tok->end = NULL;
}

static size_t get_token_length(token *tok) {
  return (size_t)(tok->end - tok->start) + 1;
}

// Gets token string from token struct.
// MUST CALL FREE ON RETURNED POINTER.
static char *get_tok_str(token *tok) {
  const size_t len = get_token_length(tok);
  char *tok_str = (char *)malloc(sizeof(char) * len + 1);
  memcpy(tok_str, tok->start, sizeof(char) * len);
  tok_str[len] = '\0';
  return tok_str;
}

static void print_token(token *tok) {
  char *tok_str = get_tok_str(tok);
  printf("%s\n", tok_str);
  free(tok_str);
}

#endif
