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
#include <glib-unix.h>
#include <libsoup/soup.h>
#include <pulse/simple.h>

#include "radio_http.h"


#define RADIO_SERVER_PORT    1500


gboolean exit_sighandler(gpointer _data)
{
    GMainLoop *main_loop;

    g_assert_nonnull(_data);

    main_loop = (GMainLoop *) _data;

    g_main_loop_quit(main_loop);
    return TRUE;
}


int main()
{
    g_autoptr(SoupServer) radio_server;
    g_autoptr(GMainLoop) main_loop;
    GError *error = NULL;

    /* TODO: add CLI options */

    /* Creating main loop object */
    main_loop = g_main_loop_new(NULL, FALSE);
    g_assert_nonnull(main_loop);

#if defined(__unix__)
    /* Installing Unix signal handlers to ensure clean
     * shutdown even if for example the user presses Ctrl+C
     */
    g_unix_signal_add(SIGINT, exit_sighandler, main_loop);
    g_unix_signal_add(SIGTERM, exit_sighandler, main_loop);
#endif

    /* Creating HTTP radio server object */
    radio_server = soup_server_new(NULL, NULL);
    g_assert_nonnull(radio_server);

    soup_server_add_handler(radio_server, RADIO_ENDPOINT_PATH, radio_server_radio_cb, NULL, NULL);

    if (!soup_server_listen_all(radio_server, RADIO_SERVER_PORT, 0, &error)) {
        g_assert_nonnull(error);
        g_printerr("soup_server_listen() failed: %s\n", error->message);
        exit(EXIT_FAILURE);
    }

    g_message("Listening on *:%d\n", RADIO_SERVER_PORT);

    /* Main loop for asynchronous operation */
    g_main_loop_run(main_loop);

    return EXIT_SUCCESS;
}
