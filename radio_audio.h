#ifndef RADIO_AUDIO_H
#define RADIO_AUDIO_H

#define RADIO_AUDIO_ERROR radio_audio_error_quark()


snd_pcm_t *radio_open_audio_device(const gchar *device_name, GError **error);


typedef enum {
    RADIO_AUDIO_ERROR_FAILED,
    RADIO_AUDIO_ERROR_OPEN
} radio_audio_error;

#endif // RADIO_AUDIO_H
