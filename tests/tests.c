#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "pimp.h"
#include "unity.h"

uint samplerate;
AudioBuf *x;
AudioBuf *y;

void setUp(void)
{
    x = audiobuf_from_wav("x.wav");
    y = audiobuf_from_wav("y_1.wav");

    if (x->samplerate != y->samplerate)
    {
        printf("samplerate missmatch: %d, %d", x->samplerate, y->samplerate);
        exit(EXIT_FAILURE);
    }
    else if (x->length != y->length)
    {
        printf("length missmatch");
        exit(EXIT_FAILURE);
    }

    samplerate = x->samplerate;
}

void tearDown(void)
{
    audiobuf_destroy(x);
    audiobuf_destroy(y);
}

void test_AudioBuf_fifo_extend(void)
{

}

void test_LMSFilter_predict(void)
{
    size_t length = 2;
    lms_t w[] = {0., 0.5};
    lms_t* y_desired = malloc(sizeof(lms_t)*y->length);

    LMSFilter* lms = lms_new(length, 1, 0);
    // initialize filter coefficients
    for (size_t i = 0; i < length; i++) lms->w[i] = w[i];

    // fill y_desired using predict
    for (size_t i = 0; i < y->length; i++)
    {
        y_desired[i] = lms_predict(lms, xbuf)
    }

    TEST_ASSERT_EQUAL_DOUBLE_ARRAY(y->data, y_desired, y->length);
}