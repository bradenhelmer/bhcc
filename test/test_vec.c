// test_vec.c
// ~~~~~~~~~~
// Tests for the vector module in the bhcc toolchain.
#include "test_util.h"
#include "util/vec.h"
#include <assert.h>
#include <stdio.h>

static void test_vector_init(void) {
  vec int_vec;
  vector_init(&int_vec, 64);
  BHCC_ASSERT(int_vec.capacity == 64, int_vec.capacity, 64ul, int_vec.capacity,
              zu);
  vector_destroy(&int_vec);
  BHCC_TEST_PASSED
}

static void test_vector_at(void) {
  vec int_vec;
  vector_init(&int_vec, 64);
  int *p = (void *)malloc(sizeof(int));
  *p = 1;
  vector_append(&int_vec, p);
  int actual = *((int *)vector_at(&int_vec, 0));
  BHCC_ASSERT(actual == 1, vector_at(&int_vec, 0), 1, actual, d);
  vector_destroy(&int_vec);
  BHCC_TEST_PASSED
}

static void test_vector_append(void) {
  vec int_vec;
  vector_init(&int_vec, 64);

  // Initial appends
  int *p;
  for (int i = 0; i < 32; ++i) {
    p = (void *)malloc(sizeof(int));
    *p = i;
    vector_append(&int_vec, p);
    int actual = *((int *)vector_at(&int_vec, i));
    BHCC_ASSERT(actual == i, vector_at(&int_vec, i), i, actual, d);
  }

  // Do some more and check if resized properly
  for (int i = 0; i < 33; ++i) {
    p = (void *)malloc(sizeof(int));
    *p = i;
    vector_append(&int_vec, p);
  }

  BHCC_ASSERT(int_vec.size == 65, int_vec.size, 65ul, int_vec.size, zu);

  BHCC_ASSERT(int_vec.capacity == 128, int_vec.capacity, 128ul,
              int_vec.capacity, zu);

  vector_destroy(&int_vec);
  BHCC_TEST_PASSED
}

static void test_vector_resize(void) {
  vec int_vec;
  vector_init(&int_vec, 64);

  size_t old = int_vec.capacity;
  for (size_t cap = old * 2; cap <= (old << 16); cap <<= 1) {
    vector_resize(&int_vec);
    BHCC_ASSERT(int_vec.capacity == cap, int_vec.capacity, cap,
                int_vec.capacity, zu);
  }

  vector_destroy(&int_vec);
  BHCC_TEST_PASSED
}

void test_vector_main(void) {
  puts("Running vector tests...");
  test_vector_init();
  test_vector_at();
  test_vector_append();
  test_vector_resize();
  puts("All vector tests passed...");
}
