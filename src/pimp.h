#ifndef __PIMP_H__
#define __PIMP_H__

#include "wav.h"
#include <complex.h>
#include <stdio.h>
#include <stdlib.h>

typedef double complex pcomplex;
typedef double         pfloat;

typedef struct {
    uint    samplerate;
    size_t  length;
    pfloat* data;
} AudioBuf;

AudioBuf* audiobuf_new(uint samplerate, size_t length, pfloat* data);
AudioBuf* audiobuf_from_wav(char* path);
void      audiobuf_destroy(AudioBuf* buf);
void      audiobuf_left_extend(AudioBuf* buf, pfloat x);

typedef struct {
    size_t  length;
    pfloat  stepsize;
    pfloat  leakage;
    pfloat  eps;
    pfloat* w;
} LMSFilter;

LMSFilter* lms_new(size_t length, pfloat stepsize, pfloat leakage);
void       lms_destory(LMSFilter* self);
void       lms_set_w(LMSFilter* self, pfloat* w);
void       lms_update(LMSFilter* self, pfloat* xbuf, pfloat e);
void       lms_train(LMSFilter* self, pfloat* xs, pfloat* ys, size_t length);
pfloat     lms_predict(LMSFilter* self, pfloat* xbuf);

typedef struct {
    size_t   length;
    pfloat   alpha; // memory-fading coefficient
    pfloat   r;     // measurement noise
    pfloat   q;     // process noise
    pfloat*  k;     // Kalman gain
    pfloat*  Px;    // temp
    pfloat*  w;     // Filter estimate
    pfloat** P;     // Filter covariance estimate
} RLSFilter;

RLSFilter* rls_new(size_t length, pfloat alpha, pfloat Pinit);
void       rls_destory(RLSFilter* self);
void       rls_set_w(RLSFilter* self, pfloat* w);
void       rls_update(RLSFilter* self, pfloat* xbuf, pfloat e);
void       rls_train(RLSFilter* self, pfloat* xs, pfloat* ys, size_t length);
pfloat     rls_predict(RLSFilter* self, pfloat* xbuf);

AudioBuf* create_sweep(pfloat duration, uint sr, pfloat amp, pfloat postsilence);

#endif /* __PIMP_H__ */