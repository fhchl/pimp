#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "audiobuf.h"
#include "pimp.h"
#include "tests.h"

void setUp(void) {}

void tearDown(void) {}


#define LEN2 2
#define BLOCKLEN2 2

void test_set_get_w(void) {
    pfloat   w[LEN2]        = {1, 0};
    pfloat   _w[2 * LEN2]   = {0};
    pfloat   zeros[LEN2]    = {0};
    pcomplex ones[LEN2 + 1];
    for (size_t i = 0; i < LEN2+1; i++) ones[i] = 1;

    BlockLMSFilter* blms = blms_init(LEN2, BLOCKLEN2, 0.1, 1);
    blms_set_w(blms, w);

    // w is allpass
    TEST_ARRAY_WITHIN(1e-5, ones, blms->W, LEN2 + 1);

    blms_get_w(blms, _w);
    // first half is w
    TEST_ARRAY_EQUAL(w, _w, LEN2);
    // second is zero
    TEST_ARRAY_EQUAL(zeros, &_w[LEN2], LEN2);

    blms_destroy(blms);
}

void test_BlockLMSFilter_predict(void) {
    pfloat w[LEN2] = {0, 0.5};

    BlockLMSFilter* blms = blms_init(LEN2, BLOCKLEN2, 0.1, 1);
    blms_set_w(blms, w);

    AudioBuf* in  = audiobuf_from_wav("../tests/data/x.wav");
    AudioBuf* out = audiobuf_from_wav("../tests/data/y_1.wav");
    pfloat*   x   = in->data;
    pfloat*   y   = out->data;

    pfloat   xbuf[2 * LEN2]   = {0};
    pcomplex Xbuf[LEN2 + 1]   = {0};
    pfloat   y_hat[BLOCKLEN2] = {0};

    // printf("W:\t");
    // PRINT_ARRAY(blms->W, LEN2+1);

    for (size_t i = 0; i < in->len / BLOCKLEN2; i++) {
        // prepare Xbuf
        block_right_extend(2 * LEN2, BLOCKLEN2, xbuf, x);
        rfft(blms->plan, xbuf, Xbuf);

        blms_predict(blms, Xbuf, y_hat);

        // printf("x:\t");
        // PRINT_ARRAY(x, BLOCKLEN2);
        // printf("xbuf:\t");
        // PRINT_ARRAY(xbuf, 2*LEN2);
        // printf("y:\t");
        // PRINT_ARRAY(y, BLOCKLEN2);
        // printf("yhat:\t");
        // PRINT_ARRAY(y_hat, BLOCKLEN2);
        // printf("_Y:\t");
        // PRINT_ARRAY((pfloat *)(blms->_Y), 2*LEN2); // somehow interleaved samples are copied into yhat
        // printf("\n");

        TEST_ARRAY_WITHIN(1e-5, y, y_hat, BLOCKLEN2);

        x += BLOCKLEN2;
        y += BLOCKLEN2;
    }

    audiobuf_destroy(in);
    audiobuf_destroy(out);
    blms_destroy(blms);
}

void test_BlockLMSFilter_update_predict(void) {
    pfloat w[LEN2] = {0, 0.5};
    BlockLMSFilter* blms = blms_init(LEN2, BLOCKLEN2, 0.1, 1);

    blms_set_w(blms, w);

    AudioBuf* in  = audiobuf_from_wav("../tests/data/x.wav");
    AudioBuf* out = audiobuf_from_wav("../tests/data/y_1.wav");
    pfloat*   x   = in->data;
    pfloat*   y   = out->data;

    pfloat   xbuf[2 * LEN2]   = {0};
    pcomplex Xbuf[LEN2 + 1]   = {0};
    pfloat   y_hat[BLOCKLEN2] = {0};
    pfloat   e[BLOCKLEN2]     = {0};
    pfloat   ebuf[LEN2]       = {0};

    for (size_t i = 0; i < in->len / BLOCKLEN2; i++) {
        block_right_extend(2 * LEN2, BLOCKLEN2, xbuf, x);
        rfft(blms->plan, xbuf, Xbuf);
        blms_predict(blms, Xbuf, y_hat);
        for (size_t i = 0; i < BLOCKLEN2; i++)
            e[i] = y[i] - y_hat[i];
        block_right_extend(LEN2, BLOCKLEN2, ebuf, e);
        blms_update(blms, Xbuf, ebuf);

        x += BLOCKLEN2;
        y += BLOCKLEN2;
    }

    pfloat _w[2 * LEN2] = {0};
    pfloat zeros[LEN2]  = {0};
    blms_get_w(blms, _w);
    TEST_ARRAY_WITHIN(1e-5, w, _w, LEN2);
    TEST_ARRAY_WITHIN(1e-5, zeros, &_w[LEN2], LEN2);

    TEST_ARRAY_WITHIN(1e-5, y - BLOCKLEN2, y_hat, BLOCKLEN2);
    TEST_ARRAY_WITHIN(1e-5, zeros, e, BLOCKLEN2);

    audiobuf_destroy(in);
    audiobuf_destroy(out);
    blms_destroy(blms);
}


#define LEN 4
#define BLOCKLEN 2

void test_BlockLMSFilter_train(void) {
    pfloat w0[LEN] = {0.5, 0};
    pfloat w1[LEN] = {0, 0.5};
    pfloat w3[LEN] = {0, 0.5, -0.5};
    pfloat w4[LEN] = {0.1, 0.5, -0.5};
    pfloat w5[LEN] = {0.1, 0.5, -0.5, 1};

    pfloat* wtrue[] = {w0, w1, w3, w4, w5};

    AudioBuf* x = audiobuf_from_wav("../tests/data/x.wav");
    for (size_t i = 0; i < sizeof(wtrue) / sizeof(*wtrue); i++) {
        BlockLMSFilter* blms = blms_init(LEN, BLOCKLEN, 0.1, 1);

        printf("test_BlockLMSFilter_train: %d \n", (int)i);
        char path[256];
        snprintf(path, sizeof(path), "../tests/data/y_%d.wav", (int)i);
        AudioBuf* y = audiobuf_from_wav(path);

        blms_train(blms, x->len, x->data, y->data);

        pfloat w[2 * LEN];
        blms_get_w(blms, w);

        PRINT_ARRAY(wtrue[i], LEN);
        PRINT_ARRAY(w, LEN);
        TEST_ARRAY_WITHIN(1e-5, wtrue[i], w, LEN);

        audiobuf_destroy(y);
        blms_destroy(blms);
    }
    audiobuf_destroy(x);
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_set_get_w);
    RUN_TEST(test_BlockLMSFilter_predict);
    RUN_TEST(test_BlockLMSFilter_update_predict);
    RUN_TEST(test_BlockLMSFilter_train);
    return UNITY_END();
}
