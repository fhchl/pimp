#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "pimp.h"
#include "tests.h"
#include "audiobuf.h"

void setUp(void) {}
void tearDown(void) {}

void test_LMSFilter_predict(void) {
    size_t     len = 2;
    pfloat     w[]    = {0, 0.5};
    LMSFilter* filt   = lms_init(len, 0, 1);
    lms_set_w(filt, w);

    AudioBuf* x    = audiobuf_from_wav("../tests/data/x.wav");
    AudioBuf* y    = audiobuf_from_wav("../tests/data/y_1.wav");
    AudioBuf* xbuf = audiobuf_new(0, len, calloc(len, sizeof(pfloat)));

    pfloat y_hat;
    for (size_t i = 0; i < x->len; i++) {
        audiobuf_left_extend(xbuf, x->data[i]);
        y_hat = lms_predict(filt, xbuf->data);
        TEST_EQUAL(y->data[i], y_hat);
    }

    audiobuf_destroy(x);
    audiobuf_destroy(y);
    audiobuf_destroy(xbuf);
    lms_destroy(filt);
}

void test_LMSFilter_update_predict(void) {
    size_t     len = 2;
    pfloat     w[]    = {0, 0.5};
    LMSFilter* filt   = lms_init(len, 0.1, 1);

    AudioBuf* xs   = audiobuf_from_wav("../tests/data/x.wav");
    AudioBuf* ys   = audiobuf_from_wav("../tests/data/y_1.wav");
    AudioBuf* xbuf = audiobuf_new(0, len, calloc(len, sizeof(pfloat)));

    pfloat y_hat=0, x=0, y=0, e=0;
    for (size_t i = 0; i < xs->len; i++) {
        x = xs->data[i];
        y = ys->data[i];
        audiobuf_left_extend(xbuf, x);
        y_hat = lms_predict(filt, xbuf->data);
        e     = y - y_hat;
        lms_update(filt, xbuf->data, e);
    }

    TEST_ARRAY_WITHIN((1e-5), w, filt->w, len);
    TEST_WITHIN(1e-5, y, y_hat);
    TEST_WITHIN(1e-5, 0, e);

    audiobuf_destroy(xs);
    audiobuf_destroy(ys);
    audiobuf_destroy(xbuf);
    lms_destroy(filt);
}

void test_LMSFilter_train_0(void) {
    size_t     len = 2;
    pfloat     w_0[]  = {0.5, 0};
    LMSFilter* filt   = lms_init(len, 0.1, 1);

    AudioBuf* xs = audiobuf_from_wav("../tests/data/x.wav");
    AudioBuf* ys = audiobuf_from_wav("../tests/data/y_0.wav");

    lms_train(filt, xs->len, xs->data, ys->data);

    TEST_ARRAY_WITHIN(1e-5, w_0, filt->w, len);

    audiobuf_destroy(xs);
    audiobuf_destroy(ys);
    lms_destroy(filt);
}

#define LEN 8

void test_LMSFilter_train(void) {
    pfloat w0[LEN] = {0.5, 0};
    pfloat w1[LEN] = {0, 0.5};
    pfloat w3[LEN] = {0, 0.5, -0.5};
    pfloat w4[LEN] = {0.1, 0.5, -0.5};
    pfloat w5[LEN] = {0.1, 0.5, -0.5, 1};

    pfloat* wtrue[] = {w0, w1, w3, w4, w5};

    AudioBuf* x = audiobuf_from_wav("../tests/data/x.wav");
    for (size_t i = 0; i < sizeof(wtrue) / sizeof(*wtrue); i++)
    {
        LMSFilter* lms = lms_init(LEN, 0.1, 1);

        printf("test_LMSFilter_train: %ld \n", i);
        char path[256];
        snprintf(path, sizeof(path), "../tests/data/y_%ld.wav", i);
        AudioBuf* y = audiobuf_from_wav(path);

        lms_train(lms, x->len, x->data, y->data);

        TEST_ARRAY_WITHIN(1e-5, wtrue[i], lms->w, LEN);

        audiobuf_destroy(y);
        lms_destroy(lms);
    }
    audiobuf_destroy(x);
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_LMSFilter_predict);
    RUN_TEST(test_LMSFilter_update_predict);
    RUN_TEST(test_LMSFilter_train);
    return UNITY_END();
}