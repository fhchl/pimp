#ifndef __PIMP_FFT_H__
#define __PIMP_FFT_H__

#include "pimp.h"

struct rfft_plan_i;
typedef struct rfft_plan_i* rfft_plan;

rfft_plan make_rfft_plan(size_t n);
void      destroy_rfft_plan(rfft_plan plan);
void      rfft(rfft_plan plan, size_t n, pfloat src[n], pcomplex dest[n / 2 + 1]);
void      irfft(rfft_plan plan, size_t n, pcomplex src[n / 2 + 1], pfloat dest[n]);
size_t    rfft_length(rfft_plan plan);

#endif /* __PIMP_FFT_H__ */