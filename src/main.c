#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "wav.h"

#define SAMPLERATE 44100
#define DURATION 10
void generate_sine_wave(float *x, int sample_rate, int len)
{
    for (int i = 0; i < len; ++i) {
        x[i] = 0.5f * cosf(2 * 3.14159265358979323f * 440.0f * i / sample_rate);
    }
}

void check_error()
{
    WAV_CONST WavErr* err = wav_err();
    if (err->code > 0)
    {
        printf("error: %s\n", err->message);
        exit(1);
    }
    else wav_err_clear();
}

void check_format(WavFile *fp)
{
    if (wav_get_format(fp) != WAV_FORMAT_IEEE_FLOAT)
    {
        printf("wrong format %d", wav_get_format(fp));
        exit(1);
    }
}

typedef struct
{
    uint samplerate;
    uint length;
    float *data;
} AudioBuf;

AudioBuf* buf_from_wav(char* path)
{
    size_t count;
    size_t length;
    uint samplerate;
    float* data;
    WavFile *fp;

    fp = wav_open(path, WAV_OPEN_READ);
    check_error();
    check_format(fp);

    length = wav_get_length(fp);
    data = malloc(sizeof(float) * length);
    samplerate = wav_get_sample_rate(fp);

    count = wav_read(fp, data, length);
    if (count != length) check_error();

    AudioBuf* buf = malloc(sizeof(AudioBuf));
    buf->data = data;
    buf->samplerate = samplerate;
    buf->length = length;

    wav_close(fp);

    return buf;
}

void audiobuf_free(AudioBuf *buf)
{
    free(buf->data);
    free(buf);
}

int main(void)
{
    uint sr;
    AudioBuf *y, *x;

    x = buf_from_wav("../test/in.wav");
    y = buf_from_wav("../test/out.wav");

    audiobuf_free(x);
    audiobuf_free(y);
    return 0;
}
