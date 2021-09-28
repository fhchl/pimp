#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "pimp.h"

#if defined(PIMP_WITH_POCKETFFT)

#if !PIMP_USE_DOUBLE
# error "pocketfft can only be used with doubles (set PIMP_USE_DOUBLES)"
#endif

#include "pocketfft.h"

void rfft(rfft_plan plan, pfloat* src, pcomplex* dest) {
    size_t n = rfft_length(plan);
    // use dest as workspace for floats
    pfloat* dest_float = (pfloat *)dest;
    memmove(dest_float, src, n*sizeof *src);

    rfft_forward(plan, dest_float, 1.0);

    // dest_float[1]...dest_float[n-1] are complex pairs, move them one to the right
    memmove(dest + 1, dest_float + 1, (n - 1) * sizeof(*dest_float));
    // dest_float[0] is real DC
    dest[0] = dest_float[0];
    // if n is even, then last coef at nyquist is real
    if (n % 2 == 0)
        dest[n / 2] = creal(dest[n / 2]);
}

void irfft(rfft_plan plan, pcomplex* src, pfloat* dest) {
    size_t n = rfft_length(plan);
    // cimag(src[0]) is assumed 0, as is cimag(src[n/2]) if n is even
    memmove(dest + 1, src + 1, (n - 1) * sizeof(*dest));
    dest[0] = creal(src[0]); // dest[0] is real DC
    rfft_backward(plan, dest, 1.0 / n);
}

#elif defined(PIMP_WITH_NE10)

#include "NE10.h"

#if PIMP_USE_DOUBLE
#error "Ne10 can only be used with floats (don't set PIMP_USE_DOUBLE)"
#endif

int ne_10_is_initialized = 0;

int is_power_of_two(size_t n) {
    return (n != 0) && ((n & (n - 1)) == 0);
}

rfft_plan make_rfft_plan(size_t n) {
    assert(is_power_of_two(n));
    if (!ne_10_is_initialized) {
        if (ne10_init() != NE10_OK)
        {
            fprintf(stderr, "Failed to initialise Ne10.\n");
            return 0;
        }
        ne_10_is_initialized = 1;
    }
    return ne10_fft_alloc_r2c_float32((ne10_int32_t)n);
}

void destroy_rfft_plan(rfft_plan plan) {
    ne10_fft_destroy_r2c_float32(plan);
}

void rfft(rfft_plan plan, pfloat* src, pcomplex* dest) {
    (*ne10_fft_r2c_1d_float32)(
        (ne10_fft_cpx_float32_t*)dest, src, plan);
}

void irfft(rfft_plan plan, pcomplex* src, pfloat* dest) {;
    ne10_fft_c2r_1d_float32(dest, (ne10_fft_cpx_float32_t*)src, plan);
}

size_t rfft_length(rfft_plan plan) {
    fprintf(stderr, "not implemented yet");
    return 1;
    //return plan->nfft;
}

#endif
