// ast.h
// ~~~~~
// AST structure and routine defintions.
#ifndef BHCC_AST_H
#define BHCC_AST_H
#include "tokens.h"

// Varaible and function kw modifiers.
typedef enum {
  m_static = 0x1,
  m_const = 0x2,
  m_extern = 0x4,
} modifiers;

static const int test;

typedef u_int8_t modifier;

// For variable declarations, we can represent pointer information with a 16 bit
// integer.
// If the first bit is a 1, then this variable is pointer.
// If the second bit is a 1, then this variable is a const pointer.
// If the third bit is 1, then this variable is a stack allocated array.
// The remaining bits 13 are used for the size of the array.
// clang-format off
// E.g 0000 0000 0000 0001 -> Pointer
// 	   0000 0000 0000 0011 -> Constant pointer
//     0000 0000 1000 0100 -> Stack allocated array of size 32.
// clang-format on
typedef u_int16_t ptr_info;

typedef struct {
  // Store the type as a string for ease with typedefs later.
  const char *type;

  // Pointer and modifier information
  ptr_info p_info;
  modifier m_info;

  // Name of variable
  const char *id;
} var_decl;

typedef struct {
} func_decl;

typedef struct {
  func_decl *decls;
  var_decl *globals;
} program;

#endif
