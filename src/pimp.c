#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>

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

AudioBuf* audiobuf_new(uint samplerate, size_t len, pfloat data[len]) {
    AudioBuf* buf   = malloc(sizeof *buf);
    buf->data       = data;
    buf->samplerate = samplerate;
    buf->len        = len;
    return buf;
}

AudioBuf* audiobuf_from_wav(char* path) {
    WavFile* fp = wav_open(path, WAV_OPEN_READ);
    check_error();

    if (wav_get_format(fp) != WAV_FORMAT_IEEE_FLOAT) {
        printf("error: wrong format %d", wav_get_format(fp));
        exit(EXIT_FAILURE);
    }

    size_t len        = wav_get_length(fp);
    uint   samplerate = wav_get_sample_rate(fp);
    float* data_float = malloc(len * sizeof *data_float);
    size_t count      = wav_read(fp, data_float, len);
    if (count != len)
        check_error();

    pfloat* data = malloc(len * sizeof *data);
    for (int i = 0; i < len; i++)
        data[i] = (pfloat)data_float[i];

    wav_close(fp);
    free(data_float);

    return audiobuf_new(samplerate, len, data);
}

void audiobuf_destroy(AudioBuf* buf) {
    if (buf) {
        free(buf->data);
        free(buf);
    }
}

void audiobuf_left_extend(AudioBuf* buf, pfloat x) {
    memmove(&buf->data[1], &buf->data[0], (buf->len - 1) * sizeof *(buf->data));
    buf->data[0] = x;
}

LMSFilter* lms_init(size_t len, pfloat stepsize, pfloat leakage) {
    LMSFilter* lms = malloc(sizeof *lms);
    lms->w         = calloc(len, sizeof *(lms->w));
    lms->len       = len;
    lms->stepsize  = stepsize;
    lms->eps       = 1e-8;
    lms->leakage   = 1;
    return lms;
}

void lms_set_w(LMSFilter* self, pfloat w[self->len]) {
    for (size_t i = 0; i < self->len; i++)
        self->w[i] = w[i];
}

void lms_destory(LMSFilter* self) {
    if (self) {
        free(self->w);
        free(self);
    }
}

pfloat lms_predict(LMSFilter* self, pfloat x[self->len]) {
    pfloat y = 0;
    for (size_t i = 0; i < self->len; i++)
        y += self->w[i] * x[i];
    return y;
}

void lms_update(LMSFilter* self, pfloat x[self->len], pfloat e) {
    pfloat pow = 0;
    for (size_t i = 0; i < self->len; i++)
        pow += x[i] * x[i];

    pfloat stepsize = self->stepsize / (pow + self->eps);
    for (size_t i = 0; i < self->len; i++) {
        self->w[i] *= self->leakage;
        self->w[i] += stepsize * e * x[i];
    }
}

void lms_train(LMSFilter* self, size_t len, pfloat xs[len], pfloat ys[len]) {
    AudioBuf* xbuf = audiobuf_new(0, self->len,
                                  calloc(self->len, sizeof(pfloat)));

    pfloat y_hat, x, y, e;
    for (size_t i = 0; i < len; i++) {
        x = xs[i];
        y = ys[i];
        audiobuf_left_extend(xbuf, x);
        y_hat = lms_predict(self, xbuf->data);
        e     = y - y_hat;
        lms_update(self, xbuf->data, e);
    }

    audiobuf_destroy(xbuf);
}

RLSFilter* rls_init(size_t len, pfloat alpha, pfloat Pinit) {
    assert(alpha >= 1.0);
    assert(Pinit >= 0.);

    RLSFilter* rls = malloc(sizeof *rls);
    rls->w         = calloc(len, sizeof *(rls->w));
    rls->Px        = malloc(len * sizeof *(rls->Px));
    rls->k         = malloc(len * sizeof *(rls->k));
    rls->P         = malloc(len * sizeof *(rls->P));
    for (size_t i = 0; i < len; i++)
        rls->P[i] = calloc(len, sizeof **(rls->P));

    rls->len   = len;
    rls->alpha = alpha;
    rls->q     = 0;
    rls->r     = 0;
    for (size_t i = 0; i < len; i++)
        rls->P[i][i] = Pinit;

    return rls;
};

void rls_destory(RLSFilter* self) {
    if (self) {
        free(self->w);
        free(self->Px);
        free(self->k);
        for (size_t i = 0; i < self->len; i++) {
            free(self->P[i]);
        }
        free(self->P);
        free(self);
    }
}

void rls_set_w(RLSFilter* self, pfloat w[self->len]) {
    for (size_t i = 0; i < self->len; i++)
        self->w[i] = w[i];
}

void mat_vec_mul(size_t n, pfloat y[n], pfloat* A[n], pfloat x[n]) {
    memset(y, 0, n * sizeof *y);
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            y[i] += A[i][j] * x[j];
        }
    }
}

void rls_update(RLSFilter* self, pfloat x[self->len], pfloat e) {
    // P <- alpha^2 P + I q
    for (size_t i = 0; i < self->len; i++) {
        for (size_t j = 0; j < self->len; j++)
            self->P[i][j] *= self->alpha * self->alpha;
        self->P[i][i] += self->q;
    }
    // Px <- P x  // TODO: BLIS
    mat_vec_mul(self->len, self->Px, self->P, x);
    // norm <- (x^T P x + I r)
    pfloat norm = self->r;
    for (size_t i = 0; i < self->len; i++)
        norm += self->Px[i] * x[i];
    // k <- P x (x^T P x + I r)^-1
    pfloat eps = 1e-8;
    for (size_t i = 0; i < self->len; i++)
        self->k[i] = self->Px[i] / (norm + eps);
    // w <- w + k e
    for (size_t i = 0; i < self->len; i++)
        self->w[i] += self->k[i] * e;
    // P <- P - k  x^T P  // TODO: BLIS
    for (size_t i = 0; i < self->len; i++)
        for (size_t j = 0; j < self->len; j++)
            self->P[i][j] -= self->k[i] * self->Px[j];
}

pfloat rls_predict(RLSFilter* self, pfloat x[self->len]) {
    pfloat y = 0;
    for (size_t i = 0; i < self->len; i++)
        y += self->w[i] * x[i];
    return y;
}

void rls_train(RLSFilter* self, size_t len, pfloat xs[len], pfloat ys[len]) {
    AudioBuf* xbuf = audiobuf_new(0, self->len,
                                  calloc(self->len, sizeof(pfloat)));

    pfloat y_hat, x, y, e;
    for (size_t i = 0; i < len; i++) {
        x = xs[i];
        y = ys[i];
        audiobuf_left_extend(xbuf, x);
        y_hat = rls_predict(self, xbuf->data);
        e     = y - y_hat;
        rls_update(self, xbuf->data, e);
    }

    audiobuf_destroy(xbuf);
}

AudioBuf* create_sweep(pfloat duration, uint sr, pfloat amp, pfloat postsilence) {
    size_t len = (size_t)round(duration * sr);
    assert(len > 2);
    size_t len_silence = (size_t)round(postsilence * sr);

    double omega_start = 2 * M_PI * sr / len;
    double omega_end   = 2 * M_PI * sr / 2;

    pfloat* sweep = calloc(len + len_silence, sizeof *sweep);
    pfloat  phase, logdiv, t;
    for (size_t i = 0; i < len; i++) {
        t        = i * duration / len;
        logdiv   = log(omega_end / omega_start);
        phase    = omega_start * duration / logdiv * (exp(t / duration * logdiv) - 1);
        sweep[i] = sin(phase) * amp;
    }

    return audiobuf_new(sr, len + len_silence, sweep);
}