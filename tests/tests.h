#include "unity.h"
#include "pimp.h"
#include "audiobuf.h"

#define TEST_ASSERT_ARRAY_WITHIN(DELTA, EXPECTED, ACTUAL, NUM_ELEMENTS) \
_Generic((DELTA), \
         float: _test_assert_float_array_within, \
         double: _test_assert_array_double_within \
)((DELTA), (EXPECTED), (ACTUAL), (NUM_ELEMENTS))

#define TEST_ASSERT_WITHIN(DELTA, EXPECTED, ACTUAL) \
_Generic((DELTA), \
         float: _test_assert_float_within, \
         double: _test_assert_double_within \
)((DELTA), (EXPECTED), (ACTUAL))

void _test_assert_float_within(pfloat delta, pfloat expected, pfloat actual)
{
    TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual);
}

void _test_assert_double_within(pfloat delta, pfloat expected, pfloat actual)
{
    TEST_ASSERT_DOUBLE_WITHIN(delta, expected, actual);
}

void _test_assert_array_double_within(pfloat delta, pfloat* expected, pfloat* actual, size_t num_elements) {
    for (size_t i = 0; i < num_elements; i++) {
        TEST_ASSERT_DOUBLE_WITHIN(delta, expected[i], actual[i]);
    }
}

void _test_assert_float_array_within(pfloat delta, pfloat* expected, pfloat* actual, size_t num_elements) {
    for (size_t i = 0; i < num_elements; i++) {
        TEST_ASSERT_FLOAT_WITHIN(delta, expected[i], actual[i]);
    }
}

