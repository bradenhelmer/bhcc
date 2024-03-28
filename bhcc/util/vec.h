// vec.h
// ~~~~~
// Vector interface definitions.
#ifndef BHCC_UTIL_VEC_H
#define BHCC_UTIL_VEC_H
#include "error.h"
#include <stdlib.h>

typedef struct {
  void **data;
  size_t capacity;
  size_t size;
} vec;

inline static void vector_init(vec *v, size_t capacity) {
  v->data = (void **)malloc(sizeof(void *) * capacity);
  v->capacity = capacity;
  v->size = 0;
}

inline static void *vector_at(vec *v, size_t idx) {
  if (idx >= v->capacity) {
    bhcc_errorln_simple("Attempted to index vector out of bounds!");
  }
  return v->data[idx];
}

inline static void vector_resize(vec *v) {
  void **new_data = realloc(v->data, sizeof(void *) * v->capacity * 2);
  if (new_data) {
    v->capacity *= 2;
    v->data = new_data;
  }
}

inline static void vector_append(vec *v, void *item) {
  if (v->size == v->capacity) {
    vector_resize(v);
  }
  v->data[(v->size)++] = item;
}

inline static void vector_destroy(vec *v) {
  if (v->size > 0) {
    for (int i = 0; i < v->size; ++i) {
      free(v->data[i]);
    }
  }
  free(v->data);
}
#endif
