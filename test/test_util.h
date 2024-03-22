// test_util.h
// ~~~~~~~~~~~
// Utilities for testing.
#include <stdarg.h>

#define __curr_test__ __func__

#define BHCC_ASSERT(EXPR, FAILURE_MESSAGE, ...)                                \
  if (!(EXPR)) {                                                               \
    printf("%s failed!\n", __curr_test__);                                     \
    fprintf(stderr, FAILURE_MESSAGE, __VA_ARGS__);                             \
    exit(1);                                                                   \
  }
#define BHCC_TEST_PASSED printf("%s passed!\n", __curr_test__);
