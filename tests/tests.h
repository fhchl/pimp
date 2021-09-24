// generic tests for floats and doubles

#include "pimp.h"
#include "unity.h"

#define TEST_ARRAY_WITHIN(DELTA, EXPECTED, ACTUAL, NUM_ELEMENTS)                                                                                      \
    _Generic(*(EXPECTED),                                                                                                                             \
             float                                                                                                                                    \
             : _test_assert_array_float_within((float)(DELTA), (float*)(EXPECTED), (float*)(ACTUAL), (NUM_ELEMENTS)),                                 \
               double                                                                                                                                 \
             : _test_assert_array_double_within((double)(DELTA), (double*)(EXPECTED), (double*)(ACTUAL), (NUM_ELEMENTS)),                             \
               complex float                                                                                                                          \
             : _test_assert_array_complex_float_within((complex float)(DELTA), (complex float*)(EXPECTED), (complex float*)(ACTUAL), (NUM_ELEMENTS)), \
               complex double                                                                                                                         \
             : _test_assert_array_complex_double_within((complex double)(DELTA), (complex double*)(EXPECTED), (complex double*)(ACTUAL), (NUM_ELEMENTS)))
// Casts needed as all expressions in Generics branches must be valid (https://stackoverflow.com/a/24746034/2629879)

#define TEST_ARRAY_EQUAL(EXPECTED, ACTUAL, NUM_ELEMENTS)                            \
    _Generic(*(EXPECTED),                                                           \
             float                                                                  \
             : TEST_ASSERT_EQUAL_FLOAT_ARRAY((EXPECTED), (ACTUAL), (NUM_ELEMENTS)), \
               double                                                               \
             : TEST_ASSERT_EQUAL_DOUBLE_ARRAY((EXPECTED), (ACTUAL), (NUM_ELEMENTS)))

#define TEST_WITHIN(DELTA, EXPECTED, ACTUAL)                      \
    _Generic((EXPECTED) + (ACTUAL),                               \
             float                                                \
             : TEST_ASSERT_FLOAT_WITHIN((DELTA), (EXPECTED), (ACTUAL)), \
               double                                             \
             : TEST_ASSERT_DOUBLE_WITHIN((DELTA), (EXPECTED), (ACTUAL)))

#define TEST_EQUAL(EXPECTED, ACTUAL)                      \
    _Generic((EXPECTED) + (ACTUAL),                       \
             float                                        \
             : TEST_ASSERT_EQUAL_FLOAT((EXPECTED), (ACTUAL)), \
               double                                     \
             : TEST_ASSERT_EQUAL_DOUBLE((EXPECTED), (ACTUAL)))

void _test_assert_array_float_within(float delta, float* expected, float* actual, size_t num_elements) {
    for (size_t i = 0; i < num_elements; i++) {
        TEST_ASSERT_FLOAT_WITHIN(delta, expected[i], actual[i]);
    }
}

void _test_assert_array_double_within(double delta, double* expected, double* actual, size_t num_elements) {
    for (size_t i = 0; i < num_elements; i++) {
        TEST_ASSERT_DOUBLE_WITHIN(delta, expected[i], actual[i]);
    }
}

void _test_assert_array_complex_double_within(complex double delta, complex double* expected, complex double* actual, size_t num_elements) {
    for (size_t i = 0; i < num_elements; i++) {
        TEST_ASSERT_DOUBLE_WITHIN(delta, creal(expected[i]), creal(actual[i]));
        TEST_ASSERT_DOUBLE_WITHIN(delta, cimag(expected[i]), cimag(actual[i]));
    }
}

void _test_assert_array_complex_float_within(complex float delta, complex float* expected, complex float* actual, size_t num_elements) {
    for (size_t i = 0; i < num_elements; i++) {
        TEST_ASSERT_FLOAT_WITHIN(delta, creal(expected[i]), creal(actual[i]));
        TEST_ASSERT_FLOAT_WITHIN(delta, cimag(expected[i]), cimag(actual[i]));
    }
}

#define PRINT_ARRAY(ARR, NUM_ELEMENTS)                        \
    _Generic(*(ARR),                                          \
             pfloat                                           \
             : _print_array_real((pfloat*)ARR, NUM_ELEMENTS), \
               pcomplex                                       \
             : _print_array_complex((pcomplex*)ARR, NUM_ELEMENTS))

void _print_array_real(pfloat* arr, size_t n) {
  for (size_t i = 0; i < n; i++)
  {
    printf("%.3f ", arr[i]);
  }
  printf("\n");
}

void _print_array_complex(pcomplex* arr, size_t n) {
  for (size_t i = 0; i < n; i++)
  {
    printf("%.3f+%.3fi ", creal(arr[i]), cimag(arr[i]));
  }
  printf("\n");
}
