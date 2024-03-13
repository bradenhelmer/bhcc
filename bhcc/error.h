// error.h
// ~~~~~~~
// bhcc error routines.
#ifndef BHCC_ERROR_H
#define BHCC_ERROR_H
#include <stdio.h>
#include <stdlib.h>

static void bhcc_errorln_simple(const char *msg) {
  fprintf(stderr, "%s Exiting...\n", msg);
  exit(1);
}

static void bhcc_errorln_code(const char *msg, int code) {
  fprintf(stderr, "%s Exiting...\n", msg);
  exit(code);
}

#endif
