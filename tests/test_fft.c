#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <string.h>

#include "tests.h"
#include "pimp.h"

void setUp(void) {}
void tearDown(void) {}

#define N 4
#define Nodd 5

struct data {
    int len;
    pfloat in[10];
    pcomplex out[10];
};

void run(struct data* tests, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        struct data t = tests[i];
        rfft_plan plan = make_rfft_plan(t.len);
        pcomplex* out = malloc((t.len/2 + 1) * sizeof *out);
        pfloat* in = malloc(t.len * sizeof *in);

        rfft(plan, t.in, out);
        PRINT_ARRAY(t.out, t.len/2+1);
        PRINT_ARRAY(out, t.len/2+1);
        TEST_ARRAY_WITHIN(1e-7, t.out, out, t.len/2 + 1);

        irfft(plan, out, in);
        PRINT_ARRAY(t.in, t.len);
        PRINT_ARRAY(in, t.len);
        TEST_ARRAY_WITHIN(1e-7, t.in, in, t.len);
    }
}

void test_even_length() {
    struct data tests[] = {
        {
            .len = 4,
            .in = {1, 0, 0, 0},
            .out = {1, 1, 1}
        },
        {
            .len = 4,
            .in = {0, 1, 0, 0},
            .out = {1, -I, -1}
        },
        {
            .len = 4,
            .in = {0, 0, 1, 0},
            .out = {1, -1, 1}
        },
        {
            .len = 4,
            .in = {0, 0, 0, 1},
            .out = {1, I, -1}
        },
    };

    run(tests, 4);
}

void test_odd_length() {
    struct data tests[] = {
        {
            .len = 3,
            .in = {1, 0, 0},
            .out = {1, 1, 1}
        },
        {
            .len = 3,
            .in = {0, 1, 0},
            .out = {1, -0.5-0.8660254*I}
        },
        {
            .len = 3,
            .in = {0, 0, 1},
            .out = {1, -0.5+0.8660254*I}
        },
    };

    run(tests, 3);
}

void test_fft_even(void) {
    rfft_plan plan = make_rfft_plan(N);

    pfloat x[N] = {1, 0, 0, 0};
    pcomplex Ytrue[N/2+1] = {1, 1, 1}; // expected

    // same memory for y and Y
    pcomplex Y[N / 2 + 1];
    pfloat* y = (pfloat*)Y;

    // work in y
    memcpy(y, x, N * sizeof(*x));
    rfft(plan, y, Y);
    TEST_ARRAY_WITHIN(1e-8, Ytrue, Y, N/2 + 1);
    // for (size_t i = 0; i < N/2+1; i++)
    // {
    //     printf("%.3f+%.3fi ", creal(Y[i]), cimag(Y[i]));
    // }
    // printf("\n");


    // now test backwards
    irfft(plan, Y, y);
    TEST_ARRAY_WITHIN(1e-8, x, y, N);
    // for (size_t i = 0; i < N; i++)
    // {
    //     printf("%.2f ", y[i]);
    // }
    // printf("\n");

    destroy_rfft_plan(plan);
}

void test_fft_even_dynamic(void) {
    rfft_plan plan = make_rfft_plan(N);

    pfloat* x = calloc(N, sizeof(*x));
    x[0] = 1;
    pcomplex* Ytrue = calloc(N, sizeof(*Ytrue));
    Ytrue[0] = Ytrue[1] = Ytrue[2] = 1;

    // same memory for y and Y
    pcomplex* Y = malloc((N / 2 + 1) * sizeof *Y);
    pfloat* y = (pfloat*)Y;

    // work in y
    memcpy(y, x, N * sizeof(*x));
    rfft(plan, y, Y);
    // PRINT_ARRAY(Y, N/2 + 1);
    // PRINT_ARRAY(Ytrue, N/2 + 1);
    TEST_ARRAY_WITHIN(1e-8, Ytrue, Y, N/2 + 1);


    // now test backwards
    irfft(plan, Y, y);
    // PRINT_ARRAY(y, N);
    TEST_ARRAY_WITHIN(1e-8, x, y, N);

    destroy_rfft_plan(plan);
    free(x);
    free(Ytrue);
    free(Y);
}

void test_fft_odd(void) {
    rfft_plan plan = make_rfft_plan(Nodd);

    pfloat x[Nodd] = {1, 0, 0, 0, 0};
    pcomplex Ytrue[Nodd/2+1] = {1, 1, 1}; // expected

    // same memory for y and Y
    pcomplex Y[Nodd / 2 + 1];
    pfloat* y = (pfloat*)Y;

    // work in y
    memcpy(y, x, Nodd * sizeof(*x));
    rfft(plan, y, Y);
    TEST_ARRAY_WITHIN(1e-8, Ytrue, Y, Nodd/2 + 1);
    // for (size_t i = 0; i < Nodd/2+1; i++)
    // {
    //     printf("%.3f+%.3fi ", creal(Y[i]), cimag(Y[i]));
    // }
    // printf("\n");

    // now test backwards
    irfft(plan, Y, y);
    TEST_ARRAY_WITHIN(1e-8, x, y, Nodd);
    // for (size_t i = 0; i < Nodd; i++)
    // {
    //     printf("%.2f ", y[i]);
    // }
    // printf("\n");
    destroy_rfft_plan(plan);
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_even_length);
    RUN_TEST(test_fft_even_dynamic);
    RUN_TEST(test_fft_even);

    #if PIMP_WITH_NE10 == 0
    // ne10 fft only supports even fft sizes
    RUN_TEST(test_odd_length);
    RUN_TEST(test_fft_odd);
    #endif

    return UNITY_END();
}