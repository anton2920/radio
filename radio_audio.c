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
