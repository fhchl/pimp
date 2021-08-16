#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>
#include <assert.h>

#include "pimp.h"
#include "wav.h"

void check_error() {
    WAV_CONST WavErr* err = wav_err();
    if (err->code > 0) {
        printf("error: %s\n", err->message);
        exit(EXIT_FAILURE);
    } else
        wav_err_clear();
}

void check_format(WavFile* fp) {
    if (wav_get_format(fp) != WAV_FORMAT_IEEE_FLOAT) {
        printf("error: wrong format %d", wav_get_format(fp));
        exit(EXIT_FAILURE);
    }
}

pfloat* float2lmst(float* arr, size_t len) {
    pfloat* ret = malloc(sizeof(pfloat) * len);
    for (int i = 0; i < len; i++) {
        ret[i] = (pfloat)arr[i];
    }
    return ret;
}

AudioBuf* audiobuf_new(uint samplerate, size_t length, pfloat* data) {
    AudioBuf* buf   = malloc(sizeof(AudioBuf));
    buf->data       = data;
    buf->samplerate = samplerate;
    buf->length     = length;
    return buf;
}

AudioBuf* audiobuf_from_wav(char* path) {
    float*   data_float;
    pfloat*  data;
    WavFile* fp;

    fp = wav_open(path, WAV_OPEN_READ);
    check_error();
    check_format(fp);

    size_t length     = wav_get_length(fp);
    uint   samplerate = wav_get_sample_rate(fp);

    data_float   = malloc(sizeof(float) * length);
    size_t count = wav_read(fp, data_float, length);
    if (count != length)
        check_error();

    data = float2lmst(data_float, length);

    wav_close(fp);
    free(data_float);

    return audiobuf_new(samplerate, length, data);
}

void audiobuf_destroy(AudioBuf* buf) {
    free(buf->data);
    free(buf);
}

void audiobuf_left_extend(AudioBuf* buf, pfloat x) {
    memmove(&buf->data[1], &buf->data[0], (buf->length - 1) * sizeof(pfloat));
    buf->data[0] = x;
}


LMSFilter* lms_new(size_t length, pfloat stepsize, pfloat leakage) {
    LMSFilter* lms = malloc(sizeof(LMSFilter));
    lms->w         = calloc(length, sizeof(pfloat));
    lms->length    = length;
    lms->stepsize  = stepsize;
    lms->eps       = 1e-8;
    lms->leakage   = 1;
    return lms;
}

void lms_set_w(LMSFilter* self, pfloat* w) {
    for (size_t i = 0; i < self->length; i++)
        self->w[i] = w[i];
}

void lms_destory(LMSFilter* self) {
    free(self->w);
    free(self);
}

pfloat lms_predict(LMSFilter* self, pfloat* xbuf) {
    pfloat y = 0;
    for (size_t i = 0; i < self->length; i++) {
        y += self->w[i] * xbuf[i];
    }
    return y;
}

void lms_update(LMSFilter* self, pfloat* xbuf, pfloat e) {
    pfloat pow = 0;
    for (size_t i = 0; i < self->length; i++)
        pow += xbuf[i] * xbuf[i];

    pfloat stepsize = self->stepsize / (pow + self->eps);
    for (size_t i = 0; i < self->length; i++) {
        self->w[i] *= self->leakage;
        self->w[i] += stepsize * e * xbuf[i];
    }
}

void lms_train(LMSFilter* self, pfloat* xs, pfloat* ys, size_t length) {
    AudioBuf* xbuf = audiobuf_new(0, self->length,
                                  calloc(self->length, sizeof(pfloat)));

    pfloat y_hat, x, y, e;
    for (size_t i = 0; i < length; i++) {
        x = xs[i];
        y = ys[i];
        audiobuf_left_extend(xbuf, x);
        y_hat = lms_predict(self, xbuf->data);
        e     = y - y_hat;
        lms_update(self, xbuf->data, e);
    }

    audiobuf_destroy(xbuf);
}


AudioBuf* create_sweep(pfloat duration, uint sr, pfloat amp, pfloat postsilence)
{
    size_t length = (size_t)round(duration * sr);
    assert(length > 2);
    size_t length_silence = (size_t)round(postsilence *sr);

    double omega_start = 2 * M_PI * sr / length;
    double omega_end = 2 * M_PI * sr / 2;

    pfloat* sweep = calloc(length + length_silence, sizeof(pfloat));
    pfloat phase, logdiv, t;
    for (size_t i = 0; i < length; i++)
    {
        t = i * duration / length;
        logdiv = log(omega_end / omega_start);
        phase = omega_start * duration / logdiv * (exp(t / duration * logdiv) - 1);
        sweep[i] = sin(phase) * amp;
    }

    return audiobuf_new(sr, length + length_silence, sweep);
}