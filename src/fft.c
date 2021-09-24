#include <string.h>
#include <assert.h>

#include "pimp.h"
#include "fft.h"

#if defined(PIMP_WITH_POCKETFFT)
// can only be used with DTYPE=float

#include "pocketfft.h"

void rfft(rfft_plan plan, size_t n, pfloat src[n], pcomplex dest[n / 2 + 1]) {
    assert(rfft_length(plan) == n);

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

void irfft(rfft_plan plan, size_t n, pcomplex src[n / 2 + 1], pfloat dest[n]) {
    assert(rfft_length(plan) == n);

    // cimag(src[0]) is assumed 0, as is cimag(src[n/2]) if n is even
    memmove(dest + 1, src + 1, (n - 1) * sizeof(*dest));
    dest[0] = creal(src[0]); // dest[0] is real DC
    rfft_backward(plan, dest, 1.0 / n);
}

#elif defined(PIMP_FFT_NE10)
# error "NE10 FFT not implemented!\n";
#endif
