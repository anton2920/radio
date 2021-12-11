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
#include <libsoup/soup.h>

#include "radio_http.h"


int main()
{
    GError *error = NULL;
    GMainLoop *main_loop;
    GSocketAddress *sockaddr;
    SoupServer *radio_server;

    /* Creating main loop object */
    main_loop = g_main_loop_new(NULL, FALSE);
    g_assert_nonnull(main_loop);

    /* Creating HTTP radio server object */
    radio_server = soup_server_new("", NULL);
    g_assert_nonnull(radio_server);

    sockaddr = (GSocketAddress *) g_network_address_new("31.132.166.156", 1500);
    g_assert_nonnull(sockaddr);

    soup_server_add_handler(radio_server, "/radio", radio_server_radio_cb, NULL, NULL);

    if (!soup_server_listen(radio_server, sockaddr, 0, &error)) {
        g_assert_nonnull(error);
        g_printerr("soup_server_listen() failed: %s\n", error->message);
        exit(EXIT_FAILURE);
    }

    /* Main loop for asynchronous operation */
    g_main_loop_run(main_loop);

    return EXIT_SUCCESS;
}
