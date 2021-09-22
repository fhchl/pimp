#include <string.h>

#include "pimp.h"
#include "fft.h"

#if defined(PIMP_WITH_POCKETFFT)

#if DTYPE == float
# error "pocketfft can only used with DTYPE=double\n";
#endif

#include "pocketfft.h"

void rfft(rfft_plan plan, size_t n, pfloat src[n], pcomplex dest[n / 2 + 1]) {
    rfft_forward(plan, src, 1.0);
    // src[1]...src[n-1] are complex pairs, move them one to the right
    memcpy(dest + 1, src + 1, (n - 1) * sizeof(pfloat));
    // src[0] is real DC
    dest[0] = src[0];
    // if n is even, then last coef at nyquist is real
    if (n % 2 == 0)
        dest[n / 2] = creal(dest[n / 2]);
}

void irfft(rfft_plan plan, size_t n, pcomplex src[n / 2 + 1], pfloat dest[n]) {
    memcpy(dest + 1, src + 1, (n - 1) * sizeof(pfloat));
    dest[0] = creal(src[0]); // dest[0] is real DC
    rfft_backward(plan, dest, 1.0 / n);
}

#elif defined(PIMP_FFT_NE10)
# error "NE10 FFT not implemented!\n";
#endif
