#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "audiobuf.h"
#include "pimp.h"
#include "tests.h"

#define LEN 2
#define BLOCKLEN 2

BlockLMSFilter* blms;

void setUp(void) {
    blms = blms_init(LEN, BLOCKLEN, 0.1, 1);
}

void tearDown(void) {
    blms_destroy(blms);
}

void test_set_get_w(void) {
    pfloat   w[LEN]        = {1, 0};
    pfloat   _w[2 * LEN]   = {0};
    pfloat   zeros[LEN]    = {0};
    pcomplex ones[LEN + 1] = {1., 1., 1.};

    blms_set_w(blms, w);

    // w is allpass
    TEST_ARRAY_WITHIN(1e-8, ones, blms->W, LEN + 1);

    blms_get_w(blms, _w);
    // first half is w
    TEST_ARRAY_EQUAL(w, _w, LEN);
    // second is zero
    TEST_ARRAY_EQUAL(zeros, &_w[LEN], LEN);
}

void test_BlockLMSFilter_predict(void) {
    pfloat w[LEN] = {0, 0.5};
    blms_set_w(blms, w);

    AudioBuf* in  = audiobuf_from_wav("../tests/data/x.wav");
    AudioBuf* out = audiobuf_from_wav("../tests/data/y_1.wav");
    pfloat*   x   = in->data;
    pfloat*   y   = out->data;

    pfloat   xbuf[2 * LEN]   = {0};
    pcomplex Xbuf[LEN + 1]   = {0};
    pfloat   y_hat[BLOCKLEN] = {0};

    // printf("W:\t");
    // PRINT_ARRAY(blms->W, LEN+1);

    for (size_t i = 0; i < in->len / BLOCKLEN; i++) {
        // prepare Xbuf
        block_right_extend(2 * LEN, BLOCKLEN, xbuf, x);
        rfft(blms->plan, 2 * LEN, xbuf, Xbuf);

        blms_predict(blms, Xbuf, y_hat);

        // printf("x:\t");
        // PRINT_ARRAY(x, BLOCKLEN);
        // printf("xbuf:\t");
        // PRINT_ARRAY(xbuf, 2*LEN);
        // printf("y:\t");
        // PRINT_ARRAY(y, BLOCKLEN);
        // printf("yhat:\t");
        // PRINT_ARRAY(y_hat, BLOCKLEN);
        // printf("_Y:\t");
        // PRINT_ARRAY((pfloat *)(blms->_Y), 2*LEN); // somehow interleaved samples are copied into yhat
        // printf("\n");

        TEST_ARRAY_WITHIN(1e-8, y, y_hat, BLOCKLEN);

        x += BLOCKLEN;
        y += BLOCKLEN;
    }

    audiobuf_destroy(in);
    audiobuf_destroy(out);
}

void test_BlockLMSFilter_update_predict(void) {
    pfloat w[LEN] = {0, 0.5};
    blms_set_w(blms, w);

    AudioBuf* in  = audiobuf_from_wav("../tests/data/x.wav");
    AudioBuf* out = audiobuf_from_wav("../tests/data/y_1.wav");
    pfloat*   x   = in->data;
    pfloat*   y   = out->data;

    pfloat   xbuf[2 * LEN]   = {0};
    pcomplex Xbuf[LEN + 1]   = {0};
    pfloat   y_hat[BLOCKLEN] = {0};
    pfloat   e[BLOCKLEN]     = {0};
    pfloat   ebuf[LEN]       = {0};

    for (size_t i = 0; i < in->len / BLOCKLEN; i++) {
        block_right_extend(2 * LEN, BLOCKLEN, xbuf, x);
        rfft(blms->plan, 2 * LEN, xbuf, Xbuf);
        blms_predict(blms, Xbuf, y_hat);
        for (size_t i = 0; i < BLOCKLEN; i++)
            e[i] = y[i] - y_hat[i];
        block_right_extend(LEN, BLOCKLEN, ebuf, e);
        blms_update(blms, Xbuf, ebuf);

        x += BLOCKLEN;
        y += BLOCKLEN;
    }

    pfloat _w[2 * LEN] = {0};
    pfloat zeros[LEN]  = {0};
    blms_get_w(blms, _w);
    TEST_ARRAY_EQUAL(w, _w, LEN);
    TEST_ARRAY_EQUAL(zeros, &_w[LEN], LEN);

    TEST_ARRAY_WITHIN(1e-8, y - BLOCKLEN, y_hat, BLOCKLEN);
    TEST_ARRAY_WITHIN(1e-8, zeros, e, BLOCKLEN);

    audiobuf_destroy(in);
    audiobuf_destroy(out);
}

void test_BlockLMSFilter_train_0(void) {
    pfloat wtrue[LEN] = {0.5, 0};

    AudioBuf* x = audiobuf_from_wav("../tests/data/x.wav");
    AudioBuf* y = audiobuf_from_wav("../tests/data/y_0.wav");

    blms_train(blms, x->len, x->data, y->data);

    pfloat w[2 * LEN];
    blms_get_w(blms, w);

    TEST_ARRAY_WITHIN(1e-8, wtrue, w, LEN);

    audiobuf_destroy(x);
    audiobuf_destroy(y);
}

void test_BlockLMSFilter_train_1(void) {
    pfloat wtrue[LEN] = {0, 0.5};

    AudioBuf* x = audiobuf_from_wav("../tests/data/x.wav");
    AudioBuf* y = audiobuf_from_wav("../tests/data/y_1.wav");

    blms_train(blms, x->len, x->data, y->data);

    pfloat w[2 * LEN];
    blms_get_w(blms, w);

    TEST_ARRAY_WITHIN(1e-8, wtrue, w, LEN);

    audiobuf_destroy(x);
    audiobuf_destroy(y);
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_set_get_w);
    RUN_TEST(test_BlockLMSFilter_predict);
    RUN_TEST(test_BlockLMSFilter_update_predict);
    RUN_TEST(test_BlockLMSFilter_train_0);
    RUN_TEST(test_BlockLMSFilter_train_1);
    return UNITY_END();
}