/*
 * Copyright (c) 2013, William W.L. Chuang
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Pyraemon Studio nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"


int client_initialize(struct client* client, struct server* server)
{
	memset(client, 0, sizeof(*client));

	client->server = server;

	uv_tcp_init(server->loop, &client->socket);
	client->socket.data = client;

	return 0;
}

void client_destroy(struct client* client)
{
	if (client == NULL) return;

	if (client->command != NULL) free(client->command);
}

static uv_buf_t _on_alloc_buffer(uv_handle_t* handle, size_t suggested_size)
{
	struct client* client;

	client = handle->data;

	memset(client->buffer, 0, sizeof(client->buffer));

	return uv_buf_init(client->buffer, sizeof(client->buffer));
}

static void _on_disconnect(uv_handle_t* handle)
{
	// TODO: This is a dummy example

	struct client* client = handle->data;

	client_destroy(client);
	free(client);
}

static void _on_read(uv_stream_t* stream, ssize_t nread, uv_buf_t buf)
{
	struct client* client;

	client = stream->data;

	if (nread == 0) {
		return;
	}
	if (nread < 0) {
		fprintf(stderr, "The client disconnected\n");
		server_detach(client->server, client);
		client_disconnect(client, _on_disconnect);
		return;
	}

	fprintf(stderr, "Client read: %s\n", buf.base);

	if (client->command == NULL) {
		client->command = strdup(buf.base);
		//memcpy(client->command, buf.base, nread);
		fprintf(stderr, "The client command: %s\n", client->command);
	}

	if (strncmp("pace", buf.base, 4) == 0) {
		client->pace++;
	}
}

void client_read(struct client* client)
{
	uv_read_start((uv_stream_t*)&client->socket, _on_alloc_buffer, _on_read);
}

void client_disconnect(struct client* client, uv_close_cb callback)
{
	uv_close((uv_handle_t*)&client->socket, callback);
}
