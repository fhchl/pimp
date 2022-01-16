#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "tests.h"
#include "pimp.h"
#include "audiobuf.h"

void setUp(void) {}
void tearDown(void) {}

void test_RLSFilter_predict(void) {
    size_t     len = 3;
    pfloat     w[]    = {0, 0.5, 0};
    RLSFilter* filt   = rls_init(len, 1, 10);
    rls_set_w(filt, w);

    AudioBuf* x    = audiobuf_from_wav("../tests/data/x.wav");
    AudioBuf* y    = audiobuf_from_wav("../tests/data/y_1.wav");
    AudioBuf* xbuf = audiobuf_new(0, len, calloc(len, sizeof(pfloat)));

    pfloat y_hat;
    for (size_t i = 0; i < x->len; i++) {
        audiobuf_left_extend(xbuf, x->data[i]);
        y_hat = rls_predict(filt, xbuf->data);
        TEST_EQUAL(y->data[i], y_hat);
    }

    audiobuf_destroy(x);
    audiobuf_destroy(y);
    audiobuf_destroy(xbuf);
    rls_destroy(filt);
}

void test_RLSFilter_update_predict(void) {
    size_t     len = 3;
    pfloat     w[]    = {0, 0.5, 0};
    RLSFilter* filt   = rls_init(len, 1, 10);

    AudioBuf* xs   = audiobuf_from_wav("../tests/data/x.wav");
    AudioBuf* ys   = audiobuf_from_wav("../tests/data/y_1.wav");
    AudioBuf* xbuf = audiobuf_new(0, len, calloc(len, sizeof(pfloat)));

    pfloat y_hat, x, y=0, e=0;
    for (size_t i = 0; i < xs->len; i++) {
        x = xs->data[i];
        y = ys->data[i];
        audiobuf_left_extend(xbuf, x);
        y_hat = rls_predict(filt, xbuf->data);
        e     = y - y_hat;
        rls_update(filt, xbuf->data, e);
    }

    TEST_ARRAY_WITHIN(1e-5, w, filt->w, len);
    TEST_WITHIN(1e-5, y, y_hat);
    TEST_WITHIN(1e-5, 0, e);

    audiobuf_destroy(xs);
    audiobuf_destroy(ys);
    audiobuf_destroy(xbuf);
    rls_destroy(filt);
}

#define LEN 8

void test_RLSFilter_train(void) {
    pfloat w0[LEN] = {0.5, 0};
    pfloat w1[LEN] = {0, 0.5};
    pfloat w3[LEN] = {0, 0.5, -0.5};
    pfloat w4[LEN] = {0.1, 0.5, -0.5};

    pfloat* wtrue[4] = {w0, w1, w3, w4};

    AudioBuf* x = audiobuf_from_wav("../tests/data/x.wav");
    for (size_t i = 0; i < sizeof(wtrue) / sizeof(*wtrue); i++)
    {
        RLSFilter* rls = rls_init(LEN, 1, 10000);

        printf("test_RLSFilter_train: %ld \n", i);
        char path[256];
        snprintf(path, sizeof(path), "../tests/data/y_%ld.wav", i);
        AudioBuf* y = audiobuf_from_wav(path);

        rls_train(rls, x->len, x->data, y->data);

        TEST_ARRAY_WITHIN(1e-5, wtrue[i], rls->w, LEN);

        audiobuf_destroy(y);
        rls_destroy(rls);
    }
    audiobuf_destroy(x);
}


// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_RLSFilter_predict);
    RUN_TEST(test_RLSFilter_update_predict);
    RUN_TEST(test_RLSFilter_train);
    return UNITY_END();
}