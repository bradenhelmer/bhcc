// fend.c
// ~~~~~~
// Compiler front end implementations.
#include "compiler.h"
#include "error.h"
#include "tokens.h"
#include <ctype.h>
#include <stdint.h>
#include <string.h>

#define KW_TABLE_SIZE 216
#define KW_TABLE_OFFSET 1093

// Entry in kw_table, holds the keyword itself and its token enum value.
typedef struct {
  const char *kw;
  tok_kind enum_val;
} kw_table_entry;

kw_table_entry kw_table[KW_TABLE_SIZE];

// Hashing function specific to C keywords, creates a perfect mapping that can
// be consolidated into a table size of 216, hence the defined constants above.
static uint32_t __kw_hash(const char *kw) {
  size_t len = strnlen(kw, MAX_KW_LEN);

  uint32_t result = *((uint16_t *)kw);
  result <<= 16;
  result |= *((uint16_t *)(kw + len - 2));
  result %= KW_COUNT;
  result += 3 * kw[0] * 3 + kw[1] * 2 + 1;
  result -= KW_TABLE_OFFSET;
  return result;
}

static void __init_kw_table() {
  int i;
  tok_kind keyword_enums[] = {
#define KEYWORD(X) KW_##X,
#include "tok_def.h"
  };

  for (i = 0; i < KW_TABLE_SIZE; ++i) {
    kw_table_entry entry = {.kw = NULL, .enum_val = UNKNOWN};
    kw_table[i] = entry;
  }

  for (i = 0; i < KW_COUNT; ++i) {
    uint32_t hash = __kw_hash(keywords[i]);
    kw_table_entry entry = {.kw = keywords[i], .enum_val = keyword_enums[i]};
    kw_table[hash] = entry;
  }
}

static kw_table_entry *__kw_hash_lookup(const char *word) {
  const size_t hash = __kw_hash(word);
  if (hash < 0 || hash >= KW_TABLE_SIZE)
    return NULL;
  kw_table_entry entry = kw_table[hash];
  if (entry.enum_val == UNKNOWN)
    return NULL;
  if (memcmp(entry.kw, word, strlen(word)))
    return &kw_table[hash];
  else
    return NULL;
}

// Notes:
// - Lexical routines assume that a token has been lexed once a
//   matching valid token has been found regardless of semantics.
// - The end pointer of a token points to the last token char.
//   	           e.g for the token /=
//     the end field would point here ^.

// Lex horizontal and veritcal whitespace, return new pointer.
static char *lex_whitespace(char *curr_ptr) {
  while (*curr_ptr == ' ' || *curr_ptr == '\t' || *curr_ptr == '\n' ||
         *curr_ptr == '\r') {
    curr_ptr++;
  }
  return curr_ptr;
}

// Lex an inline comment, function is called when token '//' is
// encountered and runs until a new line is encountered.
static char *lex_inline_comment(token *tok, char *curr_ptr) {
  do {
    curr_ptr++;
  } while (*curr_ptr != '\n');
  tok->end = curr_ptr - 1;
  return curr_ptr;
}

// Lex a multi line block comment enclosed in /* ... */
static char *lex_block_comment(token *tok, char *curr_ptr) {
  while (1) {
    curr_ptr++;
    if (*curr_ptr == 0)
      bhcc_errorln_simple("Unterminated block comment!");
    if (*curr_ptr == '*' && *(curr_ptr + 1) == '/')
      break;
  }
  curr_ptr++;
  tok->end = curr_ptr;
  return ++curr_ptr;
}

// Lex a possible keyword from a lexed identifier.
static void lex_possible_keyword(token *tok) {
  const size_t len = get_token_length(tok);
  char *tok_str = get_tok_str(tok);
  if (len < 2 || len > MAX_KW_LEN)
    return;
  kw_table_entry *lookup = __kw_hash_lookup(tok_str);
  if (lookup != NULL)
    tok->kind = lookup->enum_val;
  free(tok_str);
}

// Lex an identifier string starting with _ or a letter.
static char *lex_identifer(token *tok, char *curr_ptr) {
  do {
    curr_ptr++;
  } while (isalpha(*curr_ptr) || *curr_ptr == '_' || isdigit(*curr_ptr));
  tok->end = (curr_ptr++ - 1);
  lex_possible_keyword(tok);
  return curr_ptr;
}

static char *lex_numeric_literal(token *tok, char *curr_ptr) {}

static char *lex_next_token(token *tok, char *curr_ptr) {

  // Remove possible H/V whitespace
  curr_ptr = lex_whitespace(curr_ptr);

  // Clear token
  tok->start = curr_ptr;
  tok->end = curr_ptr;
  tok->kind = UNKNOWN;

  switch (*curr_ptr) {

  // EOF ?
  case 0:
    tok->kind = BHCC_EOF;
    break;
  case '/': {
    const char peek = *(curr_ptr + 1);
    if (peek == '/') {
      tok->kind = INLINE_COMMENT;
      return lex_inline_comment(tok, curr_ptr);
    } else if (peek == '*') {
      tok->kind = BLOCK_COMMENT;
      return lex_block_comment(tok, curr_ptr);
    } else if (peek == '=') {
      tok->end = ++curr_ptr;
      tok->kind = DIV_ASSIGN;
      return ++curr_ptr;
    } else {
      tok->end = curr_ptr;
      tok->kind = DIV;
      return ++curr_ptr;
    }
  }
  // Identifiers
  // clang-format off
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
    case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
    case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
    case 'V': case 'W': case 'X': case 'Y': case 'Z':
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
    case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
    case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
    case 'v': case 'w': case 'x': case 'y': case 'z':
    case '_':
	  tok->kind = ID;
	  return lex_identifer(tok, curr_ptr);
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
	  tok->kind = NUM_LIT;
	  return lex_numeric_literal(tok, curr_ptr);
    // clang-format on
  }
  return curr_ptr;
}

void parse_program(compiler *c) {

  __init_kw_table();

  char *curr_char;
  token curr_token;
  init_token(&curr_token);
  curr_char = &c->file_src[0];
  while (curr_token.kind != BHCC_EOF) {
    curr_char = lex_next_token(&curr_token, curr_char);
    if (curr_token.kind != BHCC_EOF)
      print_token(&curr_token);
  }
}
