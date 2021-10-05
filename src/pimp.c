#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>

#include "pimp.h"

#define EPS 1e-8

void left_extend(size_t len, pfloat buf[len], pfloat x) {
    memmove(&buf[1], &buf[0], (len - 1) * sizeof *buf);
    buf[0] = x;
}

void block_right_extend(size_t len, size_t blocklen, pfloat buf[len], pfloat x[blocklen]) {
    assert(len >= blocklen);
    memmove(&buf[0], &buf[blocklen], (len - blocklen) * sizeof *buf);
    memcpy(&buf[len - blocklen], &x[0], blocklen * sizeof *buf);
}

LMSFilter* lms_init(size_t len, pfloat stepsize, pfloat leakage) {
    assert((0 <= leakage) && (leakage <= 1));
    assert(0 <= stepsize);

    LMSFilter* lms = malloc(sizeof *lms);
    CHECK_ALLOC(lms);
    lms->w = calloc(len, sizeof *(lms->w));
    CHECK_ALLOC(lms->w);
    lms->len      = len;
    lms->stepsize = stepsize;
    lms->leakage  = leakage;
    lms->avg = 0.5;
    lms->Pavg = 1;
    return lms;
}

void lms_set_w(LMSFilter* self, pfloat w[self->len]) {
    for (size_t i = 0; i < self->len; i++)
        self->w[i] = w[i];
}

void lms_destroy(LMSFilter* self) {
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
    self->Pavg = self->avg * pow + (1 - self->avg) * self->Pavg;

    pfloat stepsize = self->stepsize / (self->Pavg + EPS);
    for (size_t i = 0; i < self->len; i++) {
        self->w[i] *= self->leakage;
        self->w[i] += stepsize * e * x[i];
    }
}

void lms_train(LMSFilter* self, size_t len, pfloat xs[len], pfloat ys[len]) {
    pfloat* xbuf = calloc(self->len, sizeof *xbuf);
    CHECK_ALLOC(xbuf);

    for (size_t i = 0; i < len; i++) {
        pfloat x = xs[i];
        pfloat y = ys[i];
        left_extend(self->len, xbuf, x);
        pfloat y_hat = lms_predict(self, xbuf);
        pfloat e     = y - y_hat;
        lms_update(self, xbuf, e);
    }

    free(xbuf);
}

RLSFilter* rls_init(size_t len, pfloat alpha, pfloat Pinit) {
    assert(alpha >= 1.0);
    assert(Pinit >= 0.);

    RLSFilter* rls;
    rls = malloc(sizeof *rls);
    CHECK_ALLOC(rls);
    rls->w = calloc(len, sizeof *(rls->w));
    CHECK_ALLOC(rls->w);
    rls->Px = malloc(len * sizeof *(rls->Px));
    CHECK_ALLOC(rls->Px);
    rls->k = malloc(len * sizeof *(rls->k));
    CHECK_ALLOC(rls->k);
    rls->P = malloc(len * sizeof *(rls->P));
    CHECK_ALLOC(rls->P);
    for (size_t i = 0; i < len; i++) {
        rls->P[i] = calloc(len, sizeof **(rls->P));
        CHECK_ALLOC(rls->P[i]);
    }

    rls->len   = len;
    rls->alpha = alpha;
    rls->q     = 0;
    rls->r     = 0;
    for (size_t i = 0; i < len; i++)
        rls->P[i][i] = Pinit;

    return rls;
}

void rls_destroy(RLSFilter* self) {
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
    size_t n = self->len;

    // P <- alpha^2 P + I q
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++)
            self->P[i][j] *= pow(self->alpha, 2);
        self->P[i][i] += self->q;
    }
    // Px <- P x  // TODO: BLIS
    mat_vec_mul(n, self->Px, self->P, x);
    // norm <- (x^T P x + I r)
    pfloat norm = self->r;
    for (size_t i = 0; i < n; i++)
        norm += self->Px[i] * x[i];
    // k <- P x (x^T P x + I r)^-1
    for (size_t i = 0; i < n; i++)
        self->k[i] = self->Px[i] / (norm + EPS);
    // w <- w + k e
    for (size_t i = 0; i < n; i++)
        self->w[i] += self->k[i] * e;
    // P <- P - k  x^T P  // TODO: BLIS
    for (size_t i = 0; i < n; i++)
        for (size_t j = 0; j < n; j++)
            self->P[i][j] -= self->k[i] * self->Px[j];
    // force symmetry: P = (P + P^T)/2
    for (size_t i = 0; i < n; i++)
        for (size_t j = 0; j < n; j++)
            self->P[i][j] = (self->P[i][j] + self->P[j][i]) / 2;
}

pfloat rls_predict(RLSFilter* self, pfloat x[self->len]) {
    pfloat y = 0;
    for (size_t i = 0; i < self->len; i++)
        y += self->w[i] * x[i];
    return y;
}

void rls_train(RLSFilter* self, size_t len, pfloat xs[len], pfloat ys[len]) {
    pfloat* xbuf = calloc(self->len, sizeof *xbuf);
    CHECK_ALLOC(xbuf);

    for (size_t i = 0; i < len; i++) {
        left_extend(self->len, xbuf, xs[i]);
        pfloat y_hat = rls_predict(self, xbuf);
        rls_update(self, xbuf, ys[i] - y_hat);
    }

    free(xbuf);
}

#if defined(PIMP_WITH_POCKETFFT) || defined(PIMP_WITH_NE10)

BlockLMSFilter* blms_init(size_t len, size_t blocklen, pfloat stepsize, pfloat leakage) {
    assert((0 <= leakage) && (leakage <= 1));
    assert(0 <= stepsize);
    assert(blocklen <= len);

    pfloat Pavg_init = 1;

    BlockLMSFilter* blms = malloc(sizeof *blms);
    CHECK_ALLOC(blms);
    blms->W = calloc(len + 1, sizeof *(blms->W));
    CHECK_ALLOC(blms->W);
    blms->plan = make_rfft_plan(2 * len);
    CHECK_ALLOC(blms->plan);
    blms->_Y = calloc((len + 1), sizeof *(blms->_Y));
    CHECK_ALLOC(blms->_Y);
    blms->_U = calloc((len + 1), sizeof *(blms->_U));
    CHECK_ALLOC(blms->_U);
    blms->Pavg = malloc((len + 1) * sizeof *(blms->Pavg));
    CHECK_ALLOC(blms->Pavg);
    for (size_t i = 0; i < len; i++) blms->Pavg[i] = Pavg_init;

    blms->len      = len;
    blms->blocklen = blocklen;
    blms->stepsize = stepsize;
    blms->leakage  = leakage;
    blms->avg      = 0.5;

    return blms;
}

void blms_destroy(BlockLMSFilter* self) {
    if (self) {
        destroy_rfft_plan(self->plan);
        free(self->W);
        free(self->_Y);
        free(self->_U);
        free(self);
    }
}

void blms_set_w(BlockLMSFilter* self, const pfloat w[self->len]) {
    size_t n = self->len;
    // work in space of self.W which has size 2*(n+1) * sizeof(pfloat)
    pfloat* _w = (pfloat*)(self->W);
    // first n taps are w
    memcpy(_w, w, n * sizeof *_w);
    // last n taps are 0
    memset(&_w[n], 0, n * sizeof *_w);
    rfft(self->plan, _w, self->W);
}

void blms_get_w(BlockLMSFilter* self, pfloat w[2 * self->len]) {
    irfft(self->plan, self->W, w);
}

void blms_predict(BlockLMSFilter* self, const pcomplex X[self->len + 1], pfloat y[self->blocklen]) {
    size_t n  = self->len;
    size_t bn = self->blocklen;

    // convolve in frequency domain
    for (size_t i = 0; i < n + 1; i++)
        self->_Y[i] = X[i] * self->W[i];
    // reuse temporary array _Y of size 2*(len+1) * sizeof(pfloat)
    pfloat* _y = (pfloat*)(self->_Y);
    irfft(self->plan, self->_Y, _y);

    // return only last block
    memcpy(y, &_y[2 * n - bn], bn * sizeof *y);
}

void blms_update(BlockLMSFilter* self, const pcomplex X[self->len + 1], pfloat e[self->len]) {

    // update PSD estimate
    for (size_t i = 0; i < self->len + 1; i++)
    {
        self->Pavg[i] = pow(cabs(X[i]),2)*self->avg + (1-self->avg)*self->Pavg[i];
    }

    // _0e = [0..., e...]
    pfloat* _0e = (pfloat*)self->_U;
    memset(_0e, 0, self->len * sizeof *_0e);
    memcpy(&_0e[self->len], e, self->len * sizeof *_0e);

    // E = DFT(_0e)
    pcomplex* U = self->_U;
    rfft(self->plan, _0e, U);

    // Newton step
    // W <- leakage * W + stepsize/(|X|^2 + eps) * conj(X) * E
    for (size_t i = 0; i < self->len + 1; i++) {
        // U = stepsize/(|X|^2 + eps) * conj(X) * E
        U[i] *= self->stepsize / (self->Pavg[i] + EPS) * conj(X[i]);
        //W <- leakage * W + U
        self->W[i] *= self->leakage;
        self->W[i] += U[i];
    }

    // project on causal solution by zeroing second half
    pfloat* w = (pfloat*)self->W;
    irfft(self->plan, self->W, w);
    memset(&w[self->len], 0, self->len * sizeof *w);
    rfft(self->plan, w, self->W);
}

void blms_train(BlockLMSFilter* self, size_t n, pfloat xs[n], pfloat ys[n]) {
    size_t len      = self->len;
    size_t blocklen = self->blocklen;

    pfloat*   x     = xs;
    pfloat*   y     = ys;
    pfloat*   xbuf  = calloc(2 * len, sizeof *xbuf);
    pcomplex* Xbuf  = calloc(len + 1, sizeof *Xbuf);
    pfloat*   y_hat = calloc(blocklen, sizeof *y_hat);
    pfloat*   e     = calloc(blocklen, sizeof *e);
    pfloat*   ebuf  = calloc(len, sizeof *ebuf);
    CHECK_ALLOC(xbuf);
    CHECK_ALLOC(Xbuf);
    CHECK_ALLOC(y_hat);
    CHECK_ALLOC(e);
    CHECK_ALLOC(ebuf);

    for (size_t block = 0; block < n / blocklen; block++) {
        block_right_extend(2 * len, blocklen, xbuf, x);
        rfft(self->plan, xbuf, Xbuf);
        blms_predict(self, Xbuf, y_hat);
        for (size_t i = 0; i < blocklen; i++)
            e[i] = y[i] - y_hat[i];
        block_right_extend(len, blocklen, ebuf, e);
        blms_update(self, Xbuf, ebuf);

        x += blocklen;
        y += blocklen;
    }

    free(xbuf);
    free(Xbuf);
    free(y_hat);
    free(e);
    free(ebuf);
}

#endif
