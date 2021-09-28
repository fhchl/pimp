#include <string.h>
#include <assert.h>

#include "pimp.h"

#if PIMP_WITH_POCKETFFT

#if !PIMP_USE_DOUBLE
# error "pocketfft can only be used with doubles (set PIMP_USE_DOUBLES)"
#endif

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

#elif PIMP_FFT_NE10

#include "NE10.h"

#if PIMP_USE_DOUBLE
#error "Ne10 can only be used with floats (don't set PIMP_USE_DOUBLE)"
#endif

// Initialise Ne10, using hardware auto-detection to set library function pointers
    if (ne10_init() != NE10_OK)
    {
        fprintf(stderr, "Failed to initialise Ne10.\n");
        return 1;
    }

    // Prepare the complex-to-complex single precision floating point FFT configuration
    // structure for inputs of length `SAMPLES`. (You need only generate this once for a
    // particular input size.)
    cfg = ne10_fft_alloc_c2c_float32(SAMPLES);

    // Generate test input values (with both real and imaginary components)
    for (int i = 0; i < SAMPLES; i++)
    {
        src[i].r = (ne10_float32_t)rand() / RAND_MAX * 50.0f;
        src[i].i = (ne10_float32_t)rand() / RAND_MAX * 50.0f;
    }

    // Perform the FFT (for an IFFT, the last parameter should be `1`)
    ne10_fft_c2c_1d_float32(dst, src, cfg, 0);

    // Display the results
    for (int i = 0; i < SAMPLES; i++)
    {
        printf( "IN[%2d]: %10.4f + %10.4fi\t", i, src[i].r, src[i].i);
        printf("OUT[%2d]: %10.4f + %10.4fi\n", i, dst[i].r, dst[i].i);
    }

    // Free the allocated configuration structure
    ne10_fft_destroy_c2c_float32(cfg);


#endif
