#include "pimp.h"
#include "unity.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <tests.h>

void setUp(void) {}
void tearDown(void) {}

void test_LMSFilter_predict(void) {
    size_t     length = 2;
    pfloat     w[]    = {0, 0.5};
    LMSFilter* filt   = lms_new(length, 0, 1);
    lms_set_w(filt, w);

    AudioBuf* x    = audiobuf_from_wav("../tests/data/x.wav");
    AudioBuf* y    = audiobuf_from_wav("../tests/data/y_1.wav");
    AudioBuf* xbuf = audiobuf_new(0, length, calloc(length, sizeof(pfloat)));

    pfloat y_hat;
    for (size_t i = 0; i < x->length; i++) {
        audiobuf_left_extend(xbuf, x->data[i]);
        y_hat = lms_predict(filt, xbuf->data);
        TEST_ASSERT_EQUAL_DOUBLE(y->data[i], y_hat);
    }

    audiobuf_destroy(x);
    audiobuf_destroy(y);
    audiobuf_destroy(xbuf);
    lms_destory(filt);
}

void test_LMSFilter_update_predict(void) {
    size_t     length = 2;
    pfloat     w[]    = {0, 0.5};
    LMSFilter* filt   = lms_new(length, 0.1, 1);

    AudioBuf* xs   = audiobuf_from_wav("../tests/data/x.wav");
    AudioBuf* ys   = audiobuf_from_wav("../tests/data/y_1.wav");
    AudioBuf* xbuf = audiobuf_new(0, length, calloc(length, sizeof(pfloat)));

    pfloat y_hat, x, y, e;
    for (size_t i = 0; i < xs->length; i++) {
        x = xs->data[i];
        y = ys->data[i];
        audiobuf_left_extend(xbuf, x);
        y_hat = lms_predict(filt, xbuf->data);
        e     = y - y_hat;
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

void test_LMSFilter_train_0(void) {
    size_t     length = 2;
    pfloat     w_0[]  = {0.5, 0};
    LMSFilter* filt   = lms_new(length, 0.1, 1);

    AudioBuf* xs = audiobuf_from_wav("../tests/data/x.wav");
    AudioBuf* ys = audiobuf_from_wav("../tests/data/y_0.wav");

    lms_train(filt, xs->data, ys->data, xs->length);

    TEST_ASSERT_DOUBLE_ARRAY_WITHIN(1e-8, w_0, filt->w, length);

    audiobuf_destroy(xs);
    audiobuf_destroy(ys);
    lms_destory(filt);
}

void test_LMSFilter_train_1(void) {
    size_t     length = 2;
    pfloat     w_1[]  = {0, 0.5};
    LMSFilter* filt   = lms_new(length, 0.1, 1);

    AudioBuf* xs = audiobuf_from_wav("../tests/data/x.wav");
    AudioBuf* ys = audiobuf_from_wav("../tests/data/y_1.wav");

    lms_train(filt, xs->data, ys->data, xs->length);

    TEST_ASSERT_DOUBLE_ARRAY_WITHIN(1e-8, w_1, filt->w, length);

    audiobuf_destroy(xs);
    audiobuf_destroy(ys);
    lms_destory(filt);
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_LMSFilter_predict);
    RUN_TEST(test_LMSFilter_update_predict);
    return UNITY_END();
}