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

void test_fft_even(void) {
    rfft_plan plan = make_rfft_plan(N);

    double x[N] = {1, 0, 0, 0};
    double complex Ytrue[N/2+1] = {1, 1, 1}; // expected

    // same memory for y and Y
    double complex Y[N / 2 + 1];
    double* y = (double*)Y;

    // work in y
    memcpy(y, x, N * sizeof(*x));
    rfft(plan, N, y, Y);
    TEST_ARRAY_WITHIN(1e-8, Ytrue, Y, N/2 + 1);
    // for (size_t i = 0; i < N/2+1; i++)
    // {
    //     printf("%.3f+%.3fi ", creal(Y[i]), cimag(Y[i]));
    // }
    // printf("\n");


    // now test backwards
    irfft(plan, N, Y, y);
    TEST_ARRAY_WITHIN(1e-8, x, y, N);
    // for (size_t i = 0; i < N; i++)
    // {
    //     printf("%.2f ", y[i]);
    // }
    // printf("\n");
}

void test_fft_odd(void) {
    rfft_plan plan = make_rfft_plan(Nodd);

    double x[Nodd] = {1, 0, 0, 0, 0};
    double complex Ytrue[Nodd/2+1] = {1, 1, 1}; // expected

    // same memory for y and Y
    double complex Y[Nodd / 2 + 1];
    double* y = (double*)Y;

    // work in y
    memcpy(y, x, Nodd * sizeof(*x));
    rfft(plan, Nodd, y, Y);
    TEST_ARRAY_WITHIN(1e-8, Ytrue, Y, Nodd/2 + 1);
    // for (size_t i = 0; i < Nodd/2+1; i++)
    // {
    //     printf("%.3f+%.3fi ", creal(Y[i]), cimag(Y[i]));
    // }
    // printf("\n");

    // now test backwards
    irfft(plan, Nodd, Y, y);
    TEST_ARRAY_WITHIN(1e-8, x, y, Nodd);
    // for (size_t i = 0; i < Nodd; i++)
    // {
    //     printf("%.2f ", y[i]);
    // }
    // printf("\n");
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_fft_even);
    RUN_TEST(test_fft_odd);
    return UNITY_END();
}