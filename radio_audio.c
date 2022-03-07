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


#include <alsa/asoundlib.h>
#include <glib.h>

#include "radio_audio.h"


static GQuark radio_audio_error_quark(void)
{
    return g_quark_from_static_string("radio-audio-error-quark");
}


snd_pcm_t *radio_open_audio_device(const gchar *device_name, GError **error)
{
    snd_pcm_t *audio_handle;
    gint err;

    g_assert_nonnull(device_name);

    if ((err = snd_pcm_open(&audio_handle, device_name, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        g_assert((error != NULL) || (*error == NULL));
        g_set_error(error, RADIO_AUDIO_ERROR, RADIO_AUDIO_ERROR_OPEN,
                    "failed to open audio device %s; %s", device_name,
                    snd_strerror(err));
        return NULL;
    }

    return audio_handle;
}
