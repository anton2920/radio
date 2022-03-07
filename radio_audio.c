/*
   HTTP radio server — free on-premise HTTP radio server only for good music
   Copyright © 2021 @anton2920

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


#include <glib.h>
#include <pulse/error.h>

#include "radio_audio.h"


#define RADIO_APPLICATION_NAME  "radio"
#define RADIO_STREAM_NAME       "radio-record-stream"


static GQuark radio_audio_error_quark(void)
{
    return g_quark_from_static_string("radio-audio-error-quark");
}


radio_audio_handle_t *radio_audio_open_device(const gchar *device_name, GError **error)
{
    const pa_sample_spec audio_handle_specs = {
        .format = PA_SAMPLE_S16LE,
        .rate = 48000,
        .channels = 2
    };
    pa_simple *audio_handle;
    gint err_code;

    /* 'device_name' can be NULL */

    if ((audio_handle = pa_simple_new(NULL, RADIO_APPLICATION_NAME, PA_STREAM_RECORD,
                                      device_name, RADIO_STREAM_NAME, &audio_handle_specs,
                                      NULL, NULL, &err_code)) == 0) {
        g_assert((error != NULL) || (*error == NULL));
        g_set_error(error, RADIO_AUDIO_ERROR, RADIO_AUDIO_ERROR_OPEN,
                    "failed to open audio device %s; %s", device_name,
                    pa_strerror(err_code));
        return NULL;
    }

    return audio_handle;
}


gboolean radio_audio_read(radio_audio_handle_t *capture_device, gchar *buffer, gsize bufsize, GError **error)
{
    gssize nbytes;
    gint err_code;

    if ((nbytes = pa_simple_read(capture_device, buffer, bufsize, &err_code)) < 0) {
        g_assert((error != NULL) || (*error == NULL));
        g_set_error(error, RADIO_AUDIO_ERROR, RADIO_AUDIO_ERROR_READ,
                    "failed to read from audio device; %s",
                    pa_strerror(err_code));
    }

    return nbytes ? FALSE : TRUE;
}


void radio_audio_free(radio_audio_handle_t *capture_device)
{
    g_assert_nonnull(capture_device);

    pa_simple_free(capture_device);
}
