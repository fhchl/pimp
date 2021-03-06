#include <string.h>
#include <assert.h>

#include "pimp.h"
#include "wav.h"

/*
* Declarations
*/

typedef struct {
    uint    samplerate;
    size_t  len;
    pfloat* data;
} AudioBuf;

AudioBuf* audiobuf_new(uint samplerate, size_t len, pfloat data[len]);
AudioBuf* audiobuf_from_wav(char* path);
void      audiobuf_destroy(AudioBuf* buf);
void      audiobuf_left_extend(AudioBuf* buf, pfloat x);

/*
* Definitions
*/

void check_error() {
    WAV_CONST WavErr* err = wav_err();
    if (err->code > 0) {
        printf("error: %s\n", err->message);
        exit(EXIT_FAILURE);
    } else
        wav_err_clear();
}

AudioBuf* audiobuf_new(uint samplerate, size_t len, pfloat data[len]) {
    AudioBuf* buf   = malloc(sizeof *buf);
    buf->data       = data;
    buf->samplerate = samplerate;
    buf->len        = len;
    return buf;
}

AudioBuf* audiobuf_from_wav(char* path) {
    WavFile* fp = wav_open(path, WAV_OPEN_READ);
    check_error();

    if (wav_get_format(fp) != WAV_FORMAT_IEEE_FLOAT) {
        printf("error: wrong format %d", wav_get_format(fp));
        exit(EXIT_FAILURE);
    }

    size_t len        = wav_get_length(fp);
    uint   samplerate = wav_get_sample_rate(fp);
    float* data_float = malloc(len * sizeof *data_float);
    size_t count      = wav_read(fp, data_float, len);
    if (count != len)
        check_error();

    pfloat* data = malloc(len * sizeof *data);
    for (size_t i = 0; i < len; i++)
        data[i] = (pfloat)data_float[i];

    wav_close(fp);
    free(data_float);

    return audiobuf_new(samplerate, len, data);
}

void audiobuf_destroy(AudioBuf* buf) {
    if (buf) {
        free(buf->data);
        free(buf);
    }
}

void audiobuf_left_extend(AudioBuf* buf, pfloat x) {
    memmove(&buf->data[1], &buf->data[0], (buf->len - 1) * sizeof *(buf->data));
    buf->data[0] = x;
}