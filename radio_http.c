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


#include <libsoup/soup.h>

#include "radio_audio.h"
#include "radio_http.h"


typedef struct radio_server_context_t
{
    radio_audio_handle_t    *capture_device;
    SoupClientContext       *client_ctx;
} radio_server_context_t;


static void radio_server_wrote_headers_cb(SoupMessage *_msg G_GNUC_UNUSED,
                                          gpointer user_data)
{
    radio_audio_wav_chunk_header_t wav_header;
    gchar buffer[RADIO_AUDIO_BUFFER_SIZE];
    radio_server_context_t ctx;
    GIOStream *http_stream;
    GOutputStream *ostream;
    GError *error = NULL;

    g_assert_nonnull(user_data);

    ctx = *((radio_server_context_t *) user_data);
    g_free(user_data); /* Context was 'g_new()'ed, should be 'g_free()'d */

    http_stream = soup_client_context_steal_connection(ctx.client_ctx);
    g_assert_nonnull(http_stream);

    ostream = g_io_stream_get_output_stream(http_stream);
    g_assert_nonnull(ostream);

    /* TODO: replace hardcoded constants */
    /* Filling 'RIFF' header */
    strcpy(wav_header.riff.chunk_id, "RIFF");
    wav_header.riff.chunk_size = 36 + RADIO_AUDIO_BUFFER_SIZE;
    strcpy(wav_header.riff.format, "WAVE");

    /* Filling 'fmt' subchunk */
    strcpy(wav_header.fmt.subchunk1_id, "fmt ");
    wav_header.fmt.subchunk1_size = 16;
    wav_header.fmt.audio_format = 1;
    wav_header.fmt.num_channels = 2;
    wav_header.fmt.sample_rate = 48000;
    wav_header.fmt.byte_rate = wav_header.fmt.sample_rate * wav_header.fmt.num_channels * 2;
    wav_header.fmt.block_align = (gint16) (wav_header.fmt.num_channels * 2);
    wav_header.fmt.bits_per_sample = 16;

    /* Filling 'data' subchunk */
    strcpy(wav_header.data.subchunk2_id, "data");
    wav_header.data.subchunk2_size = RADIO_AUDIO_BUFFER_SIZE;

    /* Writing header to client */
    if (g_output_stream_write(ostream,
                              &wav_header, sizeof(wav_header),
                              NULL, &error) < 0) {
        g_warning("g_output_stream_write() failed: %s", error->message);
        g_output_stream_close(ostream, NULL, NULL);
        g_object_unref(http_stream);
        radio_audio_free(ctx.capture_device);
        return;
    }

    while (TRUE) {
        /* Reading from playback device */
        if (radio_audio_read(ctx.capture_device, buffer, sizeof(buffer), &error) < 0) {
            g_warning("radio_audio_read() failed: %s", error->message);
            g_output_stream_close(ostream, NULL, NULL);
            g_object_unref(http_stream);
            radio_audio_free(ctx.capture_device);
            g_error_free(error);
            break;
        }

        /* Writing data to client */
        if (g_output_stream_write(ostream, buffer, sizeof(buffer), NULL, &error) < 0) {
            g_warning("g_output_stream_write() failed: %s", error->message);
            g_output_stream_close(ostream, NULL, NULL);
            g_object_unref(http_stream);
            radio_audio_free(ctx.capture_device);
            g_error_free(error);
            break;
        }
        /* Flushing stream */
        if (!g_output_stream_flush(ostream, NULL, &error)) {
            g_warning("g_output_stream_flush() failed: %s", error->message);
            g_output_stream_close(ostream, NULL, NULL);
            g_object_unref(http_stream);
            radio_audio_free(ctx.capture_device);
            g_error_free(error);
            break;
        }
    }
}


void radio_server_radio_cb(SoupServer *server,
                           SoupMessage *msg,
                           const char *path,
                           GHashTable *query G_GNUC_UNUSED,
                           SoupClientContext *client,
                           gpointer user_data G_GNUC_UNUSED)
{
    GSocketAddress *remote_sockaddr;
    radio_server_context_t *ctx;
    GError *error = NULL;
    gchar *addr_str;

    g_assert_nonnull(server);
    g_assert_nonnull(msg);
    g_assert_nonnull(path);
    /* 'query' can be NULL */
    g_assert_nonnull(client);
    /* 'user_data' can be NULL */

    remote_sockaddr = soup_client_context_get_remote_address(client);
    g_assert_nonnull(remote_sockaddr);

    addr_str = g_inet_address_to_string(g_inet_socket_address_get_address((GInetSocketAddress *) remote_sockaddr));
    g_assert_nonnull(addr_str);

    g_message("Accepted connection from %s:%d\n", addr_str,
              g_inet_socket_address_get_port((GInetSocketAddress *) remote_sockaddr));

    g_free(addr_str);

    if (msg->method != SOUP_METHOD_GET) {
        soup_message_set_status(msg, SOUP_STATUS_NOT_IMPLEMENTED);
        return;
    }

    if (strcmp(path, RADIO_ENDPOINT_PATH) != 0) {
        soup_message_set_status(msg, SOUP_STATUS_BAD_REQUEST);
        return;
    }

    /* Opening selected audio device */
    ctx = g_new(radio_server_context_t, 1);
    g_assert_nonnull(ctx);

    ctx->capture_device = radio_audio_open_device(NULL, &error);
    if (ctx->capture_device == NULL) {
        soup_message_set_status(msg, SOUP_STATUS_INTERNAL_SERVER_ERROR);
        g_warning("radio_audio_open_device() failed; %s; sending %d", error->message,
                  SOUP_STATUS_INTERNAL_SERVER_ERROR);
        return;
    }

    ctx->client_ctx = client;

    soup_message_set_status(msg, SOUP_STATUS_OK);
    soup_message_set_http_version(msg, SOUP_HTTP_1_0);
    soup_message_headers_set_content_type(msg->response_headers, "audio/wav", NULL);
    soup_message_headers_set_encoding(msg->response_headers, SOUP_ENCODING_EOF);

    g_signal_connect(G_OBJECT(msg), "wrote-headers", G_CALLBACK(radio_server_wrote_headers_cb), ctx);
    /* g_signal_connect(); */ /* TODO: handle "finished" */
}
