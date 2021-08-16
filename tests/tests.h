#include "unity.h"
#include "pimp.h"

void TEST_ASSERT_DOUBLE_ARRAY_WITHIN(pfloat delta, pfloat* expected, pfloat* actual, size_t num_elements) {
    for (size_t i = 0; i < num_elements; i++) {
        TEST_ASSERT_DOUBLE_WITHIN(delta, expected[i], actual[i]);
    }
}