// test_util.h
// ~~~~~~~~~~~
// Utilities for testing.
#include <stdarg.h>

#define __curr_test__ __func__

#define BHCC_ASSERT(EXPR, LOC, EXPECTED, ACTUAL, FMT)                          \
  if (!(EXPR)) {                                                               \
    printf("%s failed!\n", __curr_test__);                                     \
    fprintf(stderr,                                                            \
            "Unexpected value for %s !\nExpected: %" #FMT "\nActual: %" #FMT   \
            "\n",                                                              \
            #LOC, EXPECTED, ACTUAL);                                           \
    exit(1);                                                                   \
  }

#define BHCC_TEST_PASSED printf("%s passed!\n", __curr_test__);
