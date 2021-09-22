#ifndef __PIMP_H__
#define __PIMP_H__

#include <complex.h>
#include <stdlib.h>

#ifndef DTYPE
# define DTYPE double
#endif

typedef DTYPE         pfloat;
typedef complex DTYPE pcomplex;

#include "fft.h"


void left_extend(size_t len, pfloat buf[len], pfloat x);

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

typedef struct {
    size_t    len;
    size_t    blocklen;
    pfloat    stepsize;
    pfloat    leakage;
    pfloat    eps;
    pcomplex* W;
    rfft_plan plan;
} BlockLMSFilter;

BlockLMSFilter* blms_init(size_t len, size_t blocklen, pfloat stepsize, pfloat leakage);
void            blms_destory(BlockLMSFilter* self);
void            blms_set_w(BlockLMSFilter* self, pfloat w[self->len]);
void            blms_update(BlockLMSFilter* self, pfloat x[self->len], pfloat e);
void            blms_train(BlockLMSFilter* self, size_t len, pfloat xs[len], pfloat ys[len]);
pfloat          blms_predict(BlockLMSFilter* self, pfloat x[self->len]);

#endif /* __PIMP_H__ */