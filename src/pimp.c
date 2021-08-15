#include <stdlib.h>
#include <stdio.h>
#include <tgmath.h>

#include "wav.h"
#include "pimp.h"

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

pfloat *float2lmst(float *arr, size_t len)
{
    pfloat *ret = malloc(sizeof(pfloat) * len);
    for (int i = 0; i < len; i++)
    {
        ret[i] = (pfloat)arr[i];
    }
    return ret;
}

AudioBuf *audiobuf_new(uint samplerate, size_t length, pfloat *data)
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
    pfloat *data;
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

void audiobuf_left_extend(AudioBuf *buf, pfloat x)
{
    // TODO: use memmove(&items[k+1], &items[k], (numItems-k-1)*sizeof(double));
    for (size_t i = buf->length - 1; i > 0; i--)
        buf->data[i] = buf->data[i-1];
    buf->data[0] = x;
}

LMSFilter *lms_new(size_t length, pfloat stepsize, pfloat leakage)
{
    LMSFilter *lms = malloc(sizeof(LMSFilter));
    lms->w = calloc(length, sizeof(pfloat));
    lms->length = length;
    lms->stepsize = stepsize;
    lms->eps = 1e-8;
    lms->leakage = 1;
    return lms;
}

void lms_set_w(LMSFilter* self, pfloat* w)
{
    for (size_t i = 0; i < self->length; i++) self->w[i] = w[i];
}

void lms_destory(LMSFilter *self)
{
    free(self->w);
    free(self);
}

pfloat lms_predict(LMSFilter *self, pfloat *xbuf)
{
    pfloat y = 0;
    for (size_t i = 0; i < self->length; i++)
    {
        y += self->w[i] * xbuf[i];
    }
    return y;
}

void lms_update(LMSFilter *self, pfloat* xbuf, pfloat e)
{
    pfloat pow = 0;
    for (size_t i = 0; i < self->length; i++) pow += xbuf[i]*xbuf[i];

    pfloat stepsize = self->stepsize / (pow + self->eps);
    for (size_t i = 0; i < self->length; i++) {
        self->w[i] *= self->leakage;
        self->w[i] += stepsize * e * xbuf[i];
    }
}

void lms_train(LMSFilter *self, pfloat* xs, pfloat* ys, size_t length)
{
    AudioBuf* xbuf = audiobuf_new(0, self->length, calloc(self->length, sizeof(pfloat)));

    pfloat y_hat, x, y, e;
    for (size_t i = 0; i < length; i++)
    {
        x = xs[i];
        y = ys[i];
        audiobuf_left_extend(xbuf, x);
        y_hat = lms_predict(self, xbuf->data);
        e = y - y_hat;
        lms_update(self, xbuf->data, e);
    }

    audiobuf_destroy(xbuf);
}