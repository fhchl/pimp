#include <stdlib.h>
#include <stdio.h>
#include "wav.h"

typedef double lms_t;

typedef struct
{
    uint samplerate;
    size_t length;
    double *data;
} AudioBuf;
AudioBuf *audiobuf_new(uint samplerate, size_t length, double *data);
AudioBuf *audiobuf_from_wav(char *path);
void audiobuf_destroy(AudioBuf *buf);
void audiobuf_left_extend(AudioBuf *buf, lms_t x);

typedef struct
{
    size_t length;
    lms_t stepsize;
    lms_t leakage;
    lms_t *w;
} LMSFilter;
LMSFilter *lms_new(size_t length, lms_t stepsize, lms_t leakage);
void lms_destory(LMSFilter *self);
void lms_set_w(LMSFilter* self, lms_t* w);
void lms_update(LMSFilter *self, lms_t* xbuf, lms_t e);
lms_t lms_predict(LMSFilter *self, lms_t *xbuf);