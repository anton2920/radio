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
#include <libsoup/soup.h>

#include "radio_http.h"


void radio_server_radio_cb(SoupServer *server,
                           SoupMessage *msg,
                           const char *path,
                           GHashTable *query,
                           SoupClientContext *client,
                           gpointer user_data)
{
    GSocketAddress *remote_sockaddr;

    g_assert_nonnull(server);
    g_assert_nonnull(msg);
    g_assert_nonnull(path);
    /* 'query' can be NULL */
    g_assert_nonnull(client);
    g_assert_nonnull(user_data);

    remote_sockaddr = soup_client_context_get_remote_address(client);
    g_assert_nonnull(remote_sockaddr);

    g_message("Accepted connection from %s:%d\n",
              g_inet_address_to_string(g_inet_socket_address_get_address(remote_sockaddr)),
              g_inet_socket_address_get_port(remote_sockaddr));


}
