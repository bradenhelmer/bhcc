// fend.c
// ~~~~~~
// Compiler front end implementations.
#include "compiler.h"
#include "tokens.h"
#include "util/error.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

static bool lex_init = false;
static char *curr_ptr_g;

// Entry in kw_table, holds the keyword itself and its token enum value.
typedef struct {
  const char *kw;
  tok_kind enum_val;
} kw_table_entry;

// BEGIN GPERF GENERATED CODE
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
#define TOTAL_KEYWORDS 32
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 8
#define MIN_HASH_VALUE 3
#define MAX_HASH_VALUE 46

/* maximum key range = 44, duplicates = 0 */
#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
    static unsigned int
    __kw_hash(register const char *str, register size_t len) {
  static unsigned char asso_values[] = {
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 30, 10, 0,  30, 20, 0,  10, 47, 5,  47, 47,
      20, 47, 15, 20, 5,  47, 0,  5,  0,  0,  10, 5,  47, 47, 35, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47};
  register unsigned int hval = len;

  switch (hval) {
  default:
    hval += asso_values[(unsigned char)str[2]];
  /*FALLTHROUGH*/
  case 2:
  case 1:
    hval += asso_values[(unsigned char)str[0]];
    break;
  }
  return hval;
}
// ~~~~~~~~~~~~~~~~~~~~~~~~
// END GPERF GENERATED CODE
#define KW_TABLE_SIZE MAX_HASH_VALUE + 1

kw_table_entry kw_table[KW_TABLE_SIZE];

void __init_kw_table(void) {
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
    const char *kw = keywords[i];
    kw_table_entry entry = {.kw = kw, .enum_val = keyword_enums[i]};
    kw_table[__kw_hash(kw, strlen(kw))] = entry;
  }
}

void init_lexer(char *ptr) {
  if (lex_init)
    return;
  curr_ptr_g = ptr;
  lex_init = true;
  __init_kw_table();
}

// Slightly modified version from gperf.
static kw_table_entry *__kw_hash_lookup(const char *word, size_t len) {
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH) {
    register unsigned int key = __kw_hash(word, len);
    if (key <= MAX_HASH_VALUE) {
      kw_table_entry *lookup = &kw_table[key];
      if (lookup->enum_val != UNKNOWN) {
        if (*word == *lookup->kw && !strcmp(word + 1, lookup->kw + 1)) {
          return lookup;
        }
      }
    }
  }
  return NULL;
}

// Notes:
// - Lexical routines assume that a token has been lexed once a
//   matching valid token has been found regardless of semantics.
//
// - The end pointer of a token points to the last character in the token
//   string.
//   	           e.g for the token /=
//     the end field would point here ^.
//
// - The pointer returned from each lexical routine should be the position
//   immediately after the last character in a token.
//   	           e.g for the token !=
//      the end field would point here ^.
//
// - String and character literal tokens are assumed to just contain the
//   characters present within single or double quotes. The punctuation can be
//   theoretically disregarded.

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
  kw_table_entry *lookup = __kw_hash_lookup(tok_str, len);
  if (lookup != NULL)
    tok->kind = lookup->enum_val;
  free(tok_str);
}

// Lex an identifier string starting with _ or a letter.
static char *lex_identifer(token *tok, char *curr_ptr) {
  do {
    curr_ptr++;
  } while (isalpha(*curr_ptr) || *curr_ptr == '_' || isdigit(*curr_ptr));
  tok->end = (curr_ptr - 1);
  lex_possible_keyword(tok);
  return curr_ptr;
}

static char *lex_fp_suffix(token *tok, char *curr_ptr) {
  switch (*curr_ptr) {
  case 'l':
  case 'L':
    tok->fp_spec = LD;
    break;
  case 'f':
  case 'F':
    tok->fp_spec = F;
    break;
  default:
    tok->fp_spec = D;
    tok->end = curr_ptr - 1;
    return curr_ptr;
  }
  tok->end = curr_ptr;
  return ++curr_ptr;
}

// Lex a floating point literal with a possible exponent/suffix e.g
// 1.0 -> double
// 1.0f -> float
// 1.0e+6 -> double
// .0e-6 -> double
// 1.0l -> long double
static char *lex_fp_literal(token *tok, char *curr_ptr, int e_found) {
  // If an exponent hasn't been found yet, we are under the assumption that we
  // are lexing from the decimal point '.'
  tok->kind = FP_LIT;
  curr_ptr++;
  if (!e_found) {
    do {
      if ((*curr_ptr == 'e' || *curr_ptr == 'E') && !e_found) {
        e_found = 1;
        curr_ptr++;
        if (*curr_ptr == '+' || *curr_ptr == '-')
          curr_ptr++;
      }
      curr_ptr++;
    } while (isdigit(*curr_ptr) || *curr_ptr == 'e' || *curr_ptr == 'E');
  } else {
    if (*curr_ptr == '+' || *curr_ptr == '-')
      curr_ptr++;
    while (isdigit(*curr_ptr))
      curr_ptr++;
  }
  return lex_fp_suffix(tok, curr_ptr);
}

inline static int is_int_lit_suffix_char(char c) {
  return c == 'u' || c == 'l' || c == 'U' || c == 'L';
}

static char *lex_integer_suffix(token *tok, char *curr_ptr) {
  if (is_int_lit_suffix_char(*curr_ptr)) {
    char *suffix_start = curr_ptr;
    do {
      curr_ptr++;
    } while (is_int_lit_suffix_char(*curr_ptr));
    tok->end = curr_ptr - 1;
    const size_t suffix_len = curr_ptr - suffix_start;
    switch (suffix_len) {
    case 1:
      switch (*suffix_start) {
      case 'u':
      case 'U':
        tok->i_spec = U;
        break;
      case 'l':
      case 'L':
        tok->i_spec = L;
        break;
      }
      break;
    case 2:
      switch (*suffix_start) {
      case 'u':
      case 'U':
        switch (*(suffix_start + 1)) {
        case 'l':
        case 'L':
          tok->i_spec = UL;
          break;
        default:
          bhcc_errorln_simple("Unsupported integer literal suffix!");
        }
        break;
      case 'l':
        if (*(suffix_start + 1) == 'l') {
          tok->i_spec = LL;
          break;
        }
        bhcc_errorln_simple("Unsupported integer literal suffix!");
      case 'L':
        if (*(suffix_start + 1) == 'L') {
          tok->i_spec = LL;
          break;
        }
        bhcc_errorln_simple("Unsupported integer literal suffix!");
      default:
        bhcc_errorln_simple("Unsupported integer literal suffix!");
      }
      break;
    case 3:
      // Can't have anything with u in the middle like LuL
      // For the 3 letter case, easier to just do nested conditionals within top
      // level switch.
      if (*(suffix_start + 1) == 'u' || *(suffix_start + 1) == 'U')
        bhcc_errorln_simple("Unsupported integer literal suffix!");
      switch (*suffix_start) {
      case 'u':
      case 'U':
        if ((*(suffix_start + 1) == 'L' && *(suffix_start + 2) == 'L') ||
            (*(suffix_start + 1) == 'l' && *(suffix_start + 2) == 'l')) {
          tok->i_spec = ULL;
          break;
        }
        bhcc_errorln_simple("Possible unsupported integer literal suffix!");
      case 'l':
        if (*(suffix_start + 1) == 'l') {
          if (*(suffix_start + 2) == 'U' || *(suffix_start + 2) == 'u') {
            tok->i_spec = ULL;
            break;
          }
        }
        bhcc_errorln_simple("Possible unsupported integer literal suffix!");
      case 'L':
        if (*(suffix_start + 1) == 'L') {
          if (*(suffix_start + 2) == 'U' || *(suffix_start + 2) == 'u') {
            tok->i_spec = ULL;
            break;
          }
        }
        bhcc_errorln_simple("Possible unsupported integer literal suffix!");
      }
      break;
    default:
      bhcc_errorln_simple("Possible unsupported integer literal suffix!");
    }
  } else {
    tok->i_spec = I;
  }
  return curr_ptr;
}
// Lex an integer literal with a possible suffix e.g
// 	100 -> int
// 	100u, 100U -> unsigned int
// 	100l, 100L -> long int
// 	100ul, 100lu -> unsigned long int
// 	100ll -> long long int
// 	100ull -> unsigned long long int
static char *lex_int_literal(token *tok, char *curr_ptr) {
  do {
    curr_ptr++;
    if (*curr_ptr == 'e' || *curr_ptr == 'E')
      return lex_fp_literal(tok, curr_ptr, 1);
    if (*curr_ptr == '.')
      return lex_fp_literal(tok, curr_ptr, 0);
  } while (isdigit(*curr_ptr));
  tok->end = curr_ptr - 1;

  return lex_integer_suffix(tok, curr_ptr);
}

static inline int is_hex_char(char c) {
  return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') ||
         (c >= 'a' && c <= 'b');
}

static char *lex_hex_fp_literal(token *tok, char *curr_ptr, int p_found) {
  tok->kind = HEX_FP_LIT;
  curr_ptr++;
  if (!p_found) {
    do {
      if ((*curr_ptr == 'p' || *curr_ptr == 'P') && !p_found) {
        p_found = 1;
        curr_ptr++;
        if (*curr_ptr == '+' || *curr_ptr == '-')
          curr_ptr++;
      }
      curr_ptr++;
    } while (is_hex_char(*curr_ptr) || *curr_ptr == 'p' || *curr_ptr == 'P');
  } else {
    if (*curr_ptr == '+' || *curr_ptr == '-')
      curr_ptr++;
    while (is_hex_char(*curr_ptr))
      curr_ptr++;
  }
  return lex_fp_suffix(tok, curr_ptr);
}

static char *lex_hex_int_literal(token *tok, char *curr_ptr) {
  do {
    curr_ptr++;
    if (*curr_ptr == 'p' || *curr_ptr == 'P')
      return lex_hex_fp_literal(tok, curr_ptr, 1);
    if (*curr_ptr == '.')
      return lex_hex_fp_literal(tok, curr_ptr, 0);
  } while (is_hex_char(*curr_ptr));
  tok->end = curr_ptr - 1;

  return lex_integer_suffix(tok, curr_ptr);
}

static char *lex_octal_literal(token *tok, char *curr_ptr) {
  do {
    curr_ptr++;
    if (*curr_ptr == 'e' || *curr_ptr == 'E')
      return lex_fp_literal(tok, curr_ptr, 1);
    if (*curr_ptr == '.')
      return lex_fp_literal(tok, curr_ptr, 0);
  } while (isdigit(*curr_ptr));
  tok->end = curr_ptr - 1;
  return curr_ptr;
}

// Lex a string literal e.g "Deez nuts"
static char *lex_string_literal(token *tok, char *curr_ptr) {
  tok->start = ++curr_ptr;
  while (*curr_ptr != '"') {
    curr_ptr++;
    if (*curr_ptr == '\n' || *curr_ptr == 0)
      bhcc_errorln_simple("Unterminated string literal!");
  }
  tok->end = curr_ptr++ - 1;
  return curr_ptr;
}

// Lex a character literal e.g 'x'
static char *lex_char_literal(token *tok, char *curr_ptr) {
  tok->start = ++curr_ptr;
  if (*curr_ptr == '\\')
    curr_ptr++;
  while (*curr_ptr != '\'') {
    curr_ptr++;
    if (*curr_ptr == 0)
      bhcc_errorln_simple("Unterminated character literal!");
  }
  tok->end = curr_ptr++ - 1;
  return curr_ptr;
}

// Core lexical routine. Scans and lexes a token, deducing its type.
void lex(token *tok) {

  if (!lex_init)
    bhcc_errorln_simple("Lexer not initialized!");

  // Remove possible H/V whitespace
  curr_ptr_g = lex_whitespace(curr_ptr_g);

  // Clear token
  tok->start = curr_ptr_g;
  tok->end = curr_ptr_g;
  tok->kind = UNKNOWN;

  switch (*curr_ptr_g) {

  // EOF ?
  case 0:
    tok->kind = BHCC_EOF;
    break;
  case '+': {
    switch (*(++curr_ptr_g)) {
    case '=':
      tok->kind = ADD_ASSIGN;
      tok->end = curr_ptr_g;
      ++curr_ptr_g;
      return;
    case '+':
      tok->kind = INCREMENT;
      tok->end = curr_ptr_g;
      ++curr_ptr_g;
      return;
    default:
      tok->kind = ADD;
      tok->end = (curr_ptr_g - 1);
      return;
    }
  }
  case '-': {
    switch (*(++curr_ptr_g)) {
    case '=':
      tok->kind = SUB_ASSIGN;
      tok->end = curr_ptr_g;
      ++curr_ptr_g;
      return;
    case '-':
      tok->kind = DECREMENT;
      tok->end = curr_ptr_g;
      ++curr_ptr_g;
      return;
    case '>':
      tok->kind = ARROW;
      tok->end = curr_ptr_g;
      ++curr_ptr_g;
      return;
    default:
      tok->kind = SUB;
      tok->end = (curr_ptr_g - 1);
      return;
    }
  }
  case '*': {
    switch (*(++curr_ptr_g)) {
    case '=':
      tok->kind = MUL_ASSIGN;
      tok->end = curr_ptr_g;
      ++curr_ptr_g;
      return;
    default:
      tok->kind = STAR;
      tok->end = (curr_ptr_g - 1);
      return;
    }
  }
  case '/': {
    switch (*(++curr_ptr_g)) {
    case '/':
      tok->kind = INLINE_COMMENT;
      curr_ptr_g = lex_inline_comment(tok, curr_ptr_g);
      return;
    case '*':
      tok->kind = BLOCK_COMMENT;
      curr_ptr_g = lex_block_comment(tok, curr_ptr_g);
      return;
    case '=':
      tok->end = curr_ptr_g;
      tok->kind = DIV_ASSIGN;
      ++curr_ptr_g;
      return;
    default:
      tok->end = curr_ptr_g - 1;
      tok->kind = DIV;
      return;
    }
  }
  case '%': {
    switch (*(++curr_ptr_g)) {
    case '=':
      tok->kind = MOD_ASSIGN;
      tok->end = curr_ptr_g;
      ++curr_ptr_g;
      return;
    default:
      tok->kind = MODULO;
      tok->end = (curr_ptr_g - 1);
      return;
    }
  }
  case '=': {
    switch (*(++curr_ptr_g)) {
    case '=':
      tok->kind = EQ;
      tok->end = curr_ptr_g;
      ++curr_ptr_g;
      return;
    default:
      tok->kind = ASSIGN;
      tok->end = (curr_ptr_g - 1);
      return;
    }
  }
  case '>': {
    switch (*(++curr_ptr_g)) {
    case '=':
      tok->kind = GEQ;
      tok->end = curr_ptr_g;
      ++curr_ptr_g;
      return;
    case '>': {
      switch (*(++curr_ptr_g)) {
      case '=':
        tok->kind = SHR_ASSIGN;
        tok->end = curr_ptr_g;
        ++curr_ptr_g;
        return;
      default:
        tok->kind = SHR;
        tok->end = curr_ptr_g - 1;
        return;
        ++curr_ptr_g;
        return;
      }
    }
    default:
      tok->kind = GT;
      tok->end = (curr_ptr_g - 1);
      return;
    }
  }
  case '<': {
    switch (*(++curr_ptr_g)) {
    case '=':
      tok->kind = LEQ;
      tok->end = curr_ptr_g;
      ++curr_ptr_g;
      return;
    case '<': {
      switch (*(++curr_ptr_g)) {
      case '=':
        tok->kind = SHL_ASSIGN;
        tok->end = curr_ptr_g;
        ++curr_ptr_g;
        return;
      default:
        tok->kind = SHL;
        tok->end = curr_ptr_g - 1;
        return;
      }
    }
    default:
      tok->kind = LT;
      tok->end = (curr_ptr_g - 1);
      return;
    }
  }
  case '!': {
    switch (*(++curr_ptr_g)) {
    case '=':
      tok->kind = NE;
      tok->end = curr_ptr_g;
      ++curr_ptr_g;
      return;
    default:
      tok->kind = LOG_NOT;
      tok->end = (curr_ptr_g - 1);
      return;
    }
  }
  case '&': {
    switch (*(++curr_ptr_g)) {
    case '=':
      tok->kind = BIT_ASSIGN_AND;
      tok->end = curr_ptr_g;
      ++curr_ptr_g;
      return;
    case '&':
      tok->kind = LOG_AND;
      tok->end = curr_ptr_g;
      ++curr_ptr_g;
      return;
    default:
      tok->kind = BIT_AND_OR_ADDRESS;
      tok->end = (curr_ptr_g - 1);
      return;
    }
  }
  case '|': {
    switch (*(++curr_ptr_g)) {
    case '=':
      tok->kind = BIT_ASSIGN_OR;
      tok->end = curr_ptr_g;
      ++curr_ptr_g;
      return;
    case '|':
      tok->kind = LOG_OR;
      tok->end = curr_ptr_g;
      ++curr_ptr_g;
      return;
    default:
      tok->kind = BIT_OR;
      tok->end = (curr_ptr_g - 1);
      return;
    }
  }
  case '^': {
    switch (*(++curr_ptr_g)) {
    case '=':
      tok->kind = BIT_ASSIGN_XOR;
      tok->end = curr_ptr_g;
      ++curr_ptr_g;
      return;
    default:
      tok->kind = BIT_XOR;
      tok->end = (curr_ptr_g - 1);
      return;
    }
  }
  case '.':
    if (*(curr_ptr_g + 1) == '.' && *(curr_ptr_g + 2) == '.') {
      tok->kind = VARIADIC;
      curr_ptr_g += 2;
      tok->end = curr_ptr_g;
      ++curr_ptr_g;
      return;
    }
    // Might be parsing a floating point without leading digits.
    else if (isdigit(*(curr_ptr_g + 1))) {
      curr_ptr_g = lex_fp_literal(tok, curr_ptr_g, 0);
      return;
    }
    tok->kind = DOT;
    ++curr_ptr_g;
    return;
  case ',':
    tok->kind = COMMA;
    ++curr_ptr_g;
    return;
  case '?':
    tok->kind = TERNARY_COND;
    ++curr_ptr_g;
    return;
  case ':':
    tok->kind = TERNARY_ELSE;
    ++curr_ptr_g;
    return;
  case '{':
    tok->kind = OPEN_BRACE;
    ++curr_ptr_g;
    return;
  case '}':
    tok->kind = CLOSE_BRACE;
    ++curr_ptr_g;
    return;
  case '[':
    tok->kind = OPEN_BRACKET;
    ++curr_ptr_g;
    return;
  case ']':
    tok->kind = CLOSE_BRACKET;
    ++curr_ptr_g;
    return;
  case '(':
    tok->kind = OPEN_PAREN;
    ++curr_ptr_g;
    return;
  case ')':
    tok->kind = CLOSE_PAREN;
    ++curr_ptr_g;
    return;
  case ';':
    tok->kind = SEMI_COLON;
    ++curr_ptr_g;
    return;
  case '\"':
    tok->kind = STR_LIT;
    curr_ptr_g = lex_string_literal(tok, curr_ptr_g);
  case '\'':
    tok->kind = CHAR_LIT;
    curr_ptr_g = lex_char_literal(tok, curr_ptr_g);
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
    // clang-format on
    tok->kind = ID;
    curr_ptr_g = lex_identifer(tok, curr_ptr_g);
    return;
    // Check for hexadecimal and octal literals.
  case '0':
    switch (*(++curr_ptr_g)) {
    case 'x':
    case 'X':
      tok->kind = HEX_INT_LIT;
      curr_ptr_g = lex_hex_int_literal(tok, ++curr_ptr_g);
      return;
      // clang-format off
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
      // clang-format on
      tok->kind = OCT_LIT;
      curr_ptr_g = lex_octal_literal(tok, curr_ptr_g);
      return;
    case '.':
      tok->kind = FP_LIT;
      curr_ptr_g = lex_fp_literal(tok, curr_ptr_g, 0);
      return;
    // case where its just 0.
    default:
      tok->kind = INT_LIT;
      return;
    }
    // clang-format off
  case '1': case '2': case '3': case '4': case '5':
  case '6': case '7': case '8': case '9':
    // clang-format on
    tok->kind = INT_LIT;
    curr_ptr_g = lex_int_literal(tok, curr_ptr_g);
    return;
  }
}
