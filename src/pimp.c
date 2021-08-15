#include <stdlib.h>
#include <stdio.h>
#include "wav.h"

typedef double lms_t;

void check_error()
{
    WAV_CONST WavErr *err = wav_err();
    if (err->code > 0)
    {
        printf("error: %s\n", err->message);
        exit(EXIT_FAILURE);
    }
    else
        wav_err_clear();
}

void check_format(WavFile *fp)
{
    if (wav_get_format(fp) != WAV_FORMAT_IEEE_FLOAT)
    {
        printf("error: wrong format %d", wav_get_format(fp));
        exit(EXIT_FAILURE);
    }
}

lms_t *float2lmst(float *arr, size_t len)
{
    lms_t *ret = malloc(sizeof(lms_t) * len);
    for (int i = 0; i < len; i++)
    {
        ret[i] = (lms_t)arr[i];
    }
    return ret;
}

typedef struct
{
    uint samplerate;
    size_t length;
    lms_t *data;
} AudioBuf;

AudioBuf *audiobuf_new(uint samplerate, size_t length, lms_t *data)
{
    AudioBuf *buf = malloc(sizeof(AudioBuf));
    buf->data = data;
    buf->samplerate = samplerate;
    buf->length = length;
    return buf;
}

AudioBuf *audiobuf_from_wav(char *path)
{
    size_t count;
    size_t length;
    uint samplerate;
    float *data_float;
    lms_t *data;
    WavFile *fp;

    fp = wav_open(path, WAV_OPEN_READ);
    check_error();
    check_format(fp);

    length = wav_get_length(fp);
    samplerate = wav_get_sample_rate(fp);

    data_float = malloc(sizeof(float) * length);
    count = wav_read(fp, data_float, length);
    if (count != length)
        check_error();

    data = float2lmst(data_float, length);

    wav_close(fp);
    free(data_float);

    return audiobuf_new(samplerate, length, data);
}

void audiobuf_destroy(AudioBuf *buf)
{
    free(buf->data);
    free(buf);
}

void audiobuf_left_extend(AudioBuf *buf, lms_t x)
{
    // TODO: use memmove(&items[k+1], &items[k], (numItems-k-1)*sizeof(double));
    for (size_t i = buf->length - 1; i > 0; i--)
        buf->data[i] = buf->data[i-1];
    buf->data[0] = x;
}

typedef struct
{
    size_t length;
    lms_t stepsize;
    lms_t leakage;
    lms_t *w;
} LMSFilter;

LMSFilter *lms_new(size_t length, lms_t stepsize, lms_t leakage)
{
    LMSFilter *lms = malloc(sizeof(LMSFilter));
    lms->w = calloc(length, sizeof(lms_t));
    lms->length = length;
    lms->stepsize = stepsize;
    return lms;
}

void *lms_set_w(LMSFilter* self, lms_t* w)
{
    for (size_t i = 0; i < self->length; i++) self->w[i] = w[i];
}

void lms_destory(LMSFilter *self)
{
    free(self->w);
    free(self);
}

lms_t lms_predict(LMSFilter *self, lms_t *x)
{
    lms_t y = 0;
    for (size_t i = 0; i < self->length; i++)
    {
        y += self->w[i] * x[i];
    }
    return y;
}
