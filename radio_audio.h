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


#ifndef RADIO_AUDIO_H
#define RADIO_AUDIO_H

#define RADIO_AUDIO_ERROR radio_audio_error_quark()


snd_pcm_t *radio_open_audio_device(const gchar *device_name, GError **error);


typedef enum {
    RADIO_AUDIO_ERROR_FAILED,
    RADIO_AUDIO_ERROR_OPEN
} radio_audio_error;

#endif // RADIO_AUDIO_H
