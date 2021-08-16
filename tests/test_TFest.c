#include "pimp.h"
#include "unity.h"
#include "tests.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void setUp(void) {}
void tearDown(void) {}

void test_generate_sweep(void) {
    AudioBuf* expected        = audiobuf_from_wav("../tests/data/sweep.wav");
    AudioBuf* actual = create_sweep(1, 1000, 0.5, 0.5);

    assert(expected->length == actual->length);

    TEST_ASSERT_DOUBLE_ARRAY_WITHIN(1e-7, expected->data, actual->data, expected->length);

    audiobuf_destroy(expected);
    audiobuf_destroy(actual);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_generate_sweep);
    return UNITY_END();
}