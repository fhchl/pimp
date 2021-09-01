#ifndef __PIMP_H__
#define __PIMP_H__

#include "wav.h"
#include <complex.h>
#include <stdio.h>
#include <stdlib.h>

typedef float complex pcomplex;
typedef float         pfloat;

typedef struct {
    uint    samplerate;
    size_t  len;
    pfloat* data;
} AudioBuf;

AudioBuf* audiobuf_new(uint samplerate, size_t len, pfloat data[len]);
AudioBuf* audiobuf_from_wav(char* path);
void      audiobuf_destroy(AudioBuf* buf);
void      audiobuf_left_extend(AudioBuf* buf, pfloat x);
AudioBuf* create_sweep(pfloat duration, uint sr, pfloat amp, pfloat postsilence);

typedef struct {
    size_t  len;
    pfloat  stepsize;
    pfloat  leakage;
    pfloat  eps;
    pfloat* w;
} LMSFilter;

LMSFilter* lms_init(size_t len, pfloat stepsize, pfloat leakage);
void       lms_destory(LMSFilter* self);
void       lms_set_w(LMSFilter* self, pfloat w[self->len]);
void       lms_update(LMSFilter* self, pfloat x[self->len], pfloat e);
void       lms_train(LMSFilter* self, size_t len, pfloat xs[len], pfloat ys[len]);
pfloat     lms_predict(LMSFilter* self, pfloat x[self->len]);

typedef struct {
    size_t   len;   // filter length
    pfloat   alpha; // memory-fading coefficient
    pfloat   r;     // measurement noise variance
    pfloat   q;     // process noise variance
    pfloat*  k;     // Kalman gain
    pfloat*  Px;    // temp
    pfloat*  w;     // Filter estimate
    pfloat** P;     // Filter covariance estimate
} RLSFilter;

RLSFilter* rls_init(size_t len, pfloat alpha, pfloat Pinit);
void       rls_destory(RLSFilter* self);
void       rls_set_w(RLSFilter* self, pfloat w[self->len]);
void       rls_update(RLSFilter* self, pfloat x[self->len], pfloat e);
void       rls_train(RLSFilter* self, size_t len, pfloat xs[len], pfloat ys[len]);
pfloat     rls_predict(RLSFilter* self, pfloat x[self->len]);

#endif /* __PIMP_H__ */