#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "pimp.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

#define TEST_AUDIOBUF_LENGTH 100

void test_AudioBuf_LeftExtend(void)
{
    uint samplerate = 100;
    size_t length = TEST_AUDIOBUF_LENGTH;
    AudioBuf* buf = audiobuf_new(samplerate, length, calloc(length, sizeof(lms_t)));

    lms_t expected[TEST_AUDIOBUF_LENGTH] = {0};
    TEST_ASSERT_EQUAL_DOUBLE_ARRAY(expected, buf->data, length);

    audiobuf_left_extend(buf, 1);
    expected[0] = 1;
    TEST_ASSERT_EQUAL_DOUBLE_ARRAY(expected, buf->data, length);

    audiobuf_left_extend(buf, 2);
    expected[0] = 2;
    expected[1] = 1;
    TEST_ASSERT_EQUAL_DOUBLE_ARRAY(expected, buf->data, length);

    audiobuf_left_extend(buf, 3);
    expected[0] = 3;
    expected[1] = 2;
    expected[2] = 1;
    TEST_ASSERT_EQUAL_DOUBLE_ARRAY(expected, buf->data, length);

    audiobuf_left_extend(buf, 4);
    expected[0] = 4;
    expected[1] = 3;
    expected[2] = 2;
    expected[3] = 1;
    TEST_ASSERT_EQUAL_DOUBLE_ARRAY(expected, buf->data, length);

    audiobuf_left_extend(buf, 5);
    expected[0] = 5;
    expected[1] = 4;
    expected[2] = 3;
    expected[3] = 2;
    expected[4] = 1;
    TEST_ASSERT_EQUAL_DOUBLE_ARRAY(expected, buf->data, length);

    audiobuf_destroy(buf);
}


// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_AudioBuf_LeftExtend);
    return UNITY_END();
}