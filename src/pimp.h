#ifndef __PIMP_H__
#define __PIMP_H__

#include <assert.h>
#include <complex.h>
#include <stdlib.h>

#define CHECK_ALLOC(errcode) assert(errcode)

#if PIMP_USE_DOUBLE
#define DTYPE double
#else
#define DTYPE float
#endif

typedef DTYPE         pfloat;
typedef _Complex DTYPE pcomplex;

void left_extend(size_t len, pfloat buf[len], pfloat x);
void block_right_extend(size_t len, size_t blocklen, pfloat buf[len], const pfloat x[blocklen]);

typedef struct {
    size_t  len;
    pfloat  stepsize;
    pfloat  leakage;
    pfloat* w;
    pfloat  avg;
    pfloat  Pavg;
} LMSFilter;

LMSFilter* lms_init(size_t len, pfloat stepsize, pfloat leakage);
void       lms_destroy(LMSFilter* self);
void       lms_set_w(LMSFilter* self, const pfloat w[self->len]);
void       lms_update(LMSFilter* self, const pfloat x[self->len], pfloat e);
pfloat     lms_predict(LMSFilter* self, const pfloat x[self->len]);
void       lms_train(LMSFilter* self, size_t n, const pfloat x[n], const pfloat y[n]);

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
void       rls_destroy(RLSFilter* self);
void       rls_set_w(RLSFilter* self, const pfloat w[self->len]);
void       rls_update(RLSFilter* self, const pfloat x[self->len], pfloat e);
pfloat     rls_predict(RLSFilter* self, const pfloat x[self->len]);
void       rls_train(RLSFilter* self, size_t n, const pfloat x[n], const pfloat y[n]);

#if PIMP_WITH_POCKETFFT || PIMP_WITH_NE10

#if PIMP_WITH_NE10
#include "NE10.h"
typedef ne10_fft_r2c_cfg_float32_t rfft_plan;
#elif PIMP_WITH_POCKETFFT
struct rfft_plan_i;
typedef struct rfft_plan_i* rfft_plan;
#endif

rfft_plan make_rfft_plan(size_t n);
void      destroy_rfft_plan(rfft_plan plan);
size_t    rfft_length(rfft_plan plan);
// apply real to complex dft to `src` of length `n` and save in `dest` of length `n/2 +1`
void      rfft(rfft_plan plan, pfloat* src, pcomplex* dest);
// apply complex to real idft to `src` of length `n/2+1` and save in `dest` of `n`
void      irfft(rfft_plan plan, pcomplex* src, pfloat* dest);

typedef struct {
    size_t    len;
    size_t    blocklen;
    pfloat    stepsize;
    pfloat    leakage;
    pcomplex* W;
    pfloat*   Pavg;
    pfloat    avg; // PSD averaging time constant
    rfft_plan plan;
    pcomplex* _Y; // temp array in prediction
    pcomplex* _U; // temp array in update
} BlockLMSFilter;

BlockLMSFilter* blms_init(size_t len, size_t blocklen, pfloat stepsize, pfloat leakage);
void            blms_destroy(BlockLMSFilter* self);
void            blms_set_w(BlockLMSFilter* self, const pfloat w[self->len]);
void            blms_get_w(BlockLMSFilter* self, pfloat w[2*self->len]);
void            blms_update(BlockLMSFilter* self, const pcomplex X[self->len + 1], pfloat e[self->blocklen]);
void            blms_predict(BlockLMSFilter* self, const pcomplex X[self->len + 1], pfloat y[self->blocklen]);
void            blms_train(BlockLMSFilter* self, size_t n, const pfloat x[n], const pfloat y[n]);

#endif

#endif /* __PIMP_H__ */