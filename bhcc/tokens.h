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

static const char *token_enum_strs[] = {
#define TOKEN(X) #X,
#include "tok_def.h"
};

static const char *keywords[] = {
#define KEYWORD(X) #X,
#include "tok_def.h"
};

typedef enum {
#define INT_LIT_SUFFIX(X) X,
#include "tok_def.h"
} int_lit_spec;

static const char *int_suffix_enum_strs[] = {
#define INT_LIT_SUFFIX(X) #X,
#include "tok_def.h"
};

typedef enum {
#define FP_LIT_SUFFIX(X) X,
#include "tok_def.h"
} fp_lit_spec;

static const char *fp_suffix_enum_strs[] = {
#define FP_LIT_SUFFIX(X) #X,
#include "tok_def.h"
};

typedef struct {
  char *start;
  char *end;
  tok_kind kind;
  int_lit_spec i_spec;
  fp_lit_spec fp_spec;
} token;

static void init_token(token *tok) {
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

static inline int is_integer_lit(tok_kind k) {
  return k == INT_LIT || k == OCT_LIT || k == HEX_INT_LIT;
}

static inline int is_fp_lit(tok_kind k) {
  return k == FP_LIT || k == HEX_FP_LIT;
}

static void print_token(token *tok, int verbose) {
  char *tok_str = get_tok_str(tok);
  if (verbose) {
    printf("Kind: %s Str: %s", token_enum_strs[tok->kind], tok_str);
    if (is_integer_lit(tok->kind)) {
      printf(" Suffix: %s\n", int_suffix_enum_strs[tok->i_spec]);
    } else if (is_fp_lit(tok->kind)) {
      printf(" Suffix: %s\n", fp_suffix_enum_strs[tok->fp_spec]);
    } else {
      printf("\n");
    }
  } else
    printf("%s\n", tok_str);
  free(tok_str);
}

static void print_token_enum(tok_kind k) { printf("%s\n", token_enum_strs[k]); }

#endif
