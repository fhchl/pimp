#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "pimp.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void TEST_ASSERT_DOUBLE_ARRAY_WITHIN(lms_t delta, lms_t* expected, lms_t* actual, size_t num_elements)
{
    for (size_t i = 0; i < num_elements; i++)
    {
        TEST_ASSERT_DOUBLE_WITHIN(delta, expected[i], actual[i]);
    }

}

void test_AudioBuf_LeftExtend(void)
{
    uint samplerate = 100;
    size_t length = 5;
    AudioBuf* buf = audiobuf_new(samplerate, length, calloc(length, sizeof(lms_t)));

    lms_t expected[] = {0., 0., 0., 0., 0.};
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

void test_LMSFilter_predict(void)
{
    uint samplerate = 100;
    size_t length = 2;
    lms_t w[] = {0, 0.5};
    LMSFilter* filt = lms_new(length, 0, 1);
    lms_set_w(filt, w);

    lms_t* data = calloc(length, sizeof(data));
    AudioBuf* x = audiobuf_from_wav("../tests/data/x.wav");
    AudioBuf* y = audiobuf_from_wav("../tests/data/y_1.wav");
    AudioBuf* xbuf = audiobuf_new(samplerate, length, calloc(length, sizeof(lms_t)));

    lms_t y_hat;
    for (size_t i = 0; i < x->length; i++)
    {
        audiobuf_left_extend(xbuf, x->data[i]);
        y_hat = lms_predict(filt, xbuf->data);
        TEST_ASSERT_EQUAL_DOUBLE(y->data[i], y_hat);
    }

    audiobuf_destroy(x);
    audiobuf_destroy(y);
    audiobuf_destroy(xbuf);
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_AudioBuf_LeftExtend);
    RUN_TEST(test_LMSFilter_predict);
    return UNITY_END();
}