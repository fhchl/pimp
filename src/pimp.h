#ifndef __PIMP_H__
#define __PIMP_H__

#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include "wav.h"

typedef double complex pcomplex;
typedef double pfloat;

typedef struct {
    uint   samplerate;
    size_t length;
    pfloat *data;
} AudioBuf;

AudioBuf *audiobuf_new(uint samplerate, size_t length, pfloat *data);
AudioBuf *audiobuf_from_wav(char *path);
void audiobuf_destroy(AudioBuf *buf);
void audiobuf_left_extend(AudioBuf *buf, pfloat x);


typedef struct {
    size_t length;
    pfloat stepsize;
    pfloat leakage;
    pfloat eps;
    pfloat *w;
} LMSFilter;

LMSFilter *lms_new(size_t length, pfloat stepsize, pfloat leakage);
void   lms_destory(LMSFilter *self);
void   lms_set_w(LMSFilter* self, pfloat* w);
void   lms_update(LMSFilter *self, pfloat* xbuf, pfloat e);
void   lms_train(LMSFilter *self, pfloat* xs, pfloat* ys, size_t length);
pfloat lms_predict(LMSFilter *self, pfloat* xbuf);


typedef struct {
    size_t   length;
    pfloat   stepsize;
    pfloat   leakage;
    pfloat   eps;
    pcomplex *W;
} BlockLMSFilter;

BlockLMSFilter *blms_new(size_t length, pfloat stepsize, pfloat leakage);
void   blms_destory(BlockLMSFilter *self);
void   blms_set_w(BlockLMSFilter* self, pfloat* w);
void   blms_update(BlockLMSFilter *self, pcomplex* Xbuf, pfloat e);
pfloat blms_predict(BlockLMSFilter *self, pcomplex* Xbuf);
void   blms_train(BlockLMSFilter *self, pfloat* xs, pfloat* ys, size_t length);

#endif /* __PIMP_H__ */