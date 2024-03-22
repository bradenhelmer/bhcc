// vec_test.c
// ~~~~~~~~~~
// Tests for the vector module in the bhcc toolchain.
#include "test_util.h"
#include "util/vec.h"
#include <assert.h>
#include <stdio.h>

static void test_vec_init(void) {
  printf("Testing vector initialization...\n");
  vec int_vec = VECTOR_CREATE(int, 64);
  BHCC_ASSERT(int_vec.capacity == 64,
              "Unexpected value for capacity!\nExpected: %zu\nActual: %zu\n",
              63ul, int_vec.capacity);
  BHCC_TEST_PASSED
}

void test_vec_main(void) {
  test_vec_init();
}
