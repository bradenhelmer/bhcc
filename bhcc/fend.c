// fend.c
// ~~~~~~
// Compiler front end implementations.
#include "compiler.h"
#include "error.h"
#include "tokens.h"
#include <ctype.h>
#include <string.h>

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

static char *lex_possible_keyword(token *tok) {}

static char *lex_identifer(token *tok, char *curr_ptr) {}

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
	  lex_identifer(tok, curr_ptr);
	  break;
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
	  tok->kind = NUM_LIT;
	  lex_numeric_literal(tok, curr_ptr);
	  break;
    // clang-format on
  }
  return curr_ptr;
}

void parse_program(compiler *c) {
  char *curr_char;
  token curr_token;
  curr_char = &c->file_src[0];
  while (curr_token.kind != BHCC_EOF) {
    curr_char = lex_next_token(&curr_token, curr_char);
    if (curr_token.kind != BHCC_EOF)
      print_token(&curr_token);
  }
}
