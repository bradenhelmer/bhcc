// vec.h
// ~~~~~
// Vector interface definitions.
#include <stdlib.h>

typedef struct {
  void **data;
  size_t capacity;
  size_t size;
} vec;

#define VECTOR_CREATE(type, cap)                                               \
  { .data = (void **)malloc(sizeof(type*) * cap), .capacity = cap, .size = 0 }

#define VECTOR_INSERT(v, i) v.data[v.size++] = i
#define VECTOR_DESTROY(v)                                                      \
  for (int i = 0; i < v.size; ++i) {                                          \
    free(v.data[i]);                                                          \
  }
