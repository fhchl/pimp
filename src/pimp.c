#include <assert.h>
#include <stdlib.h>
#include <tgmath.h>
#include <stdio.h>
#include <string.h>

#include "pimp.h"

#define CHECK_ALLOC(errcode) assert(errcode)

void left_extend(size_t len, pfloat buf[len], pfloat x) {
    memmove(&buf[1], &buf[0], (len - 1) * sizeof *buf);
    buf[0] = x;
}

LMSFilter* lms_init(size_t len, pfloat stepsize, pfloat leakage) {
    LMSFilter* lms = malloc(sizeof *lms);
    CHECK_ALLOC(lms);
    lms->w = calloc(len, sizeof *(lms->w));
    CHECK_ALLOC(lms->w);
    lms->len      = len;
    lms->stepsize = stepsize;
    lms->eps      = 1e-8;
    lms->leakage  = 1;
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
    pfloat *xbuf = calloc(self->len, sizeof *xbuf);
    CHECK_ALLOC(xbuf);

    pfloat y_hat, x, y, e;
    for (size_t i = 0; i < len; i++) {
        x = xs[i];
        y = ys[i];
        left_extend(self->len, xbuf, x);
        y_hat = lms_predict(self, xbuf);
        e     = y - y_hat;
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
    rls->w         = calloc(len, sizeof *(rls->w));
    CHECK_ALLOC(rls->w);
    rls->Px        = malloc(len * sizeof *(rls->Px));
    CHECK_ALLOC(rls->Px);
    rls->k         = malloc(len * sizeof *(rls->k));
    CHECK_ALLOC(rls->k);
    rls->P         = malloc(len * sizeof *(rls->P));
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
    pfloat *xbuf = calloc(self->len, sizeof *xbuf);
    CHECK_ALLOC(xbuf);

    pfloat y_hat, x, y, e;
    for (size_t i = 0; i < len; i++) {
        x = xs[i];
        y = ys[i];
        left_extend(self->len, xbuf, x);
        y_hat = rls_predict(self, xbuf);
        e     = y - y_hat;
        rls_update(self, xbuf, e);
    }

    free(xbuf);
}