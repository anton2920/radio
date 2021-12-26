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
#include <libsoup/soup.h>

#include "radio_audio.h"
#include "radio_http.h"


#define RADIO_SERVER_ADDRESS "10.0.0.2"
#define RADIO_SERVER_PORT    1500


int main()
{
    snd_pcm_t *capture_handle;
    GSocketAddress *sockaddr;
    SoupServer *radio_server;
    GError *error = NULL;
    GMainLoop *main_loop;
    GInetAddress *inaddr;

    /* TODO: add CLI options */

    /* Opening selected (TODO) audio device */
    capture_handle = radio_open_audio_device("hw:0", &error);
    if (capture_handle == NULL) {
        g_assert_nonnull(error);
        g_printerr("radio_open_audio_device() failed: %s\n", error->message);
        exit(EXIT_FAILURE);
    }

    /* Creating main loop object */
    main_loop = g_main_loop_new(NULL, FALSE);
    g_assert_nonnull(main_loop);

    /* Creating HTTP radio server object */
    radio_server = soup_server_new(NULL, NULL);
    g_assert_nonnull(radio_server);

    inaddr = g_inet_address_new_from_string(RADIO_SERVER_ADDRESS);
    g_assert_nonnull(inaddr);

    sockaddr = g_inet_socket_address_new(inaddr, RADIO_SERVER_PORT);
    g_assert_nonnull(sockaddr);
    g_object_unref(inaddr);

    soup_server_add_handler(radio_server, "/radio", radio_server_radio_cb, capture_handle, NULL);

    if (!soup_server_listen(radio_server, sockaddr, 0, &error)) {
        g_assert_nonnull(error);
        g_printerr("soup_server_listen() failed: %s\n", error->message);
        g_object_unref(sockaddr);
        g_object_unref(radio_server);
        g_object_unref(main_loop);
        exit(EXIT_FAILURE);
    }
    g_object_unref(sockaddr);

    g_message("Listening on %s:%d\n", RADIO_SERVER_ADDRESS, RADIO_SERVER_PORT);

    /* Main loop for asynchronous operation */
    g_main_loop_run(main_loop);

    g_object_unref(radio_server);
    g_object_unref(main_loop);

    return EXIT_SUCCESS;
}
