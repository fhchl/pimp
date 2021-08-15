#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "pimp.h"

int main(void)
{
    uint sr;
    AudioBuf *y, *x;
    LMSFilter lms;

    x = audiobuf_from_wav("../test/in.wav");
    y = audiobuf_from_wav("../test/out.wav");
    if (x->samplerate != y->samplerate)
    {
        printf("samplerate missmatch: %d, %d", x->samplerate, y->samplerate);
        exit(EXIT_FAILURE);
    }
    else if (x->length != y->length)
    {
        printf("length missmatch");
        exit(EXIT_FAILURE);
    }

    audiobuf_destroy(x);
    audiobuf_destroy(y);

    return EXIT_SUCCESS;
}
