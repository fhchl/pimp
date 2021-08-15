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

void test_LMSFilter_predict(void)
{
    uint samplerate = 100;
    size_t length = 2;
    lms_t w[] = {0, 0.5};
    LMSFilter* filt = lms_new(length, 0, 1);
    lms_set_w(filt, w);

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
    lms_destory(filt);
}

void test_LMSFilter_update_predict(void)
{
    uint samplerate = 100;
    size_t length = 2;
    lms_t w[] = {0, 0.5};
    LMSFilter* filt = lms_new(length, 0.1, 1);

    AudioBuf* xs = audiobuf_from_wav("../tests/data/x.wav");
    AudioBuf* ys = audiobuf_from_wav("../tests/data/y_1.wav");
    AudioBuf* xbuf = audiobuf_new(samplerate, length, calloc(length, sizeof(lms_t)));

    lms_t y_hat, x, y, e;
    for (size_t i = 0; i < xs->length; i++)
    {
        x = xs->data[i];
        y = ys->data[i];
        audiobuf_left_extend(xbuf, x);
        y_hat = lms_predict(filt, xbuf->data);
        e = y - y_hat;
        lms_update(filt, xbuf->data, e);
    }

    TEST_ASSERT_DOUBLE_ARRAY_WITHIN(1e-8, w, filt->w, length);
    TEST_ASSERT_DOUBLE_WITHIN(1e-8, y, y_hat);
    TEST_ASSERT_DOUBLE_WITHIN(1e-8, 0, e);

    audiobuf_destroy(xs);
    audiobuf_destroy(ys);
    audiobuf_destroy(xbuf);
    lms_destory(filt);
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_LMSFilter_predict);
    RUN_TEST(test_LMSFilter_update_predict);
    return UNITY_END();
}