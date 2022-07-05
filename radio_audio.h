/*
   HTTP radio server — free on-premise HTTP radio server only for good music
   Copyright © 2021-2022 @anton2920

   This file is part of HTTP radio server.

   HTTP radio server is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 2 of the License, or
   (at your option) any later version.
   HTTP radio server is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with HTTP radio server. If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef RADIO_AUDIO_H
#define RADIO_AUDIO_H


#include <pulse/simple.h>


#define RADIO_AUDIO_BUFFER_SIZE 1920
#define RADIO_AUDIO_ERROR radio_audio_error_quark()


typedef pa_simple radio_audio_handle_t;


/* See: http://soundfile.sapp.org/doc/WaveFormat/ */
typedef struct PA_GCC_PACKED radio_audio_wav_chunk_header_t
{
    /* RIFF header */
    struct PA_GCC_PACKED radio_audio_wav_riff_header_t {
        gchar   chunk_id[4];        /* 'RIFF' in ASCII */
        gint32  chunk_size;         /* 36 + 'subchunk2_size' */
        gchar   format[4];          /* 'WAVE' in ASCII */
    } riff;

    /* FMT subchunk */
    struct PA_GCC_PACKED radio_audio_wav_fmt_subchunk_header_t {
        gchar   subchunk1_id[4];    /* 'fmt ' in ASCII */
        gint32  subchunk1_size;     /* should be 16 */
        gint16  audio_format;       /* PCM == 1 */
        gint16  num_channels;       /* Mono == 1, Stereo == 2 */
        gint32  sample_rate;        /* 8000, 44100, etc. */
        gint32  byte_rate;          /* == 'sample_rate' * 'num_channels' * 'bits_per_sample'/8 */
        gint16  block_align;        /* == 'num_channels' * 'bits_per_sample/8 */
        gint16  bits_per_sample;    /* 8 bits == 8, 16 bits == 16, etc. */
    } fmt;

    /* Data subchunk */
    struct PA_GCC_PACKED radio_audio_wav_data_subchunk_header_t
    {
        gchar   subchunk2_id[4];    /* 'data' in ASCII */
        gint32  subchunk2_size;     /* number of bytes following this subchunk */
    } data;
} radio_audio_wav_chunk_header_t;


typedef enum {
    RADIO_AUDIO_ERROR_FAILED,
    RADIO_AUDIO_ERROR_OPEN,
    RADIO_AUDIO_ERROR_READ
} radio_audio_error;


radio_audio_handle_t *radio_audio_open_device(const gchar *device_name, GError **error);
gboolean radio_audio_read(radio_audio_handle_t *capture_device, gchar *buffer, gsize bufsize, GError **error);
void radio_audio_free(radio_audio_handle_t *capture_device);

#endif // RADIO_AUDIO_H
