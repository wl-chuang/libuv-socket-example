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
#include <assert.h>

#include <unistd.h>

#include "core.h"


int server_initialize(uv_loop_t* loop, struct server* server)
{
	memset(server, 0, sizeof(*server));

	server->loop = loop;
	TAILQ_INIT(&server->clients);

	return 0;
}

void server_destroy(struct server* server)
{
	assert(TAILQ_FIRST(&server->clients));
}

static void _on_server_socket_close(uv_handle_t* handle)
{
	fprintf(stderr, "Server socket closed\n");
}

static void _on_client_disconnect(uv_handle_t* handle)
{
	struct client* client = handle->data;

	client_destroy(client);
	free(client);
}

static void _on_signal(uv_signal_t *handle, int signum)
{
	struct client* client = NULL;
	struct server* server = handle->data;

	fprintf(stderr, "Signal received: %d\n", signum);

	uv_signal_stop(&server->sighup);
	uv_signal_stop(&server->sigint);
	uv_signal_stop(&server->sigkill);
	uv_signal_stop(&server->sigterm);
	uv_close((uv_handle_t*)&server->watchdog, NULL);
	uv_close((uv_handle_t*)&server->socket, _on_server_socket_close);

	while ((client = TAILQ_FIRST(&server->clients)) != NULL) {
		TAILQ_REMOVE(&server->clients, client, link);
		client_disconnect(client, _on_client_disconnect);
	}
}

static void _on_connect(uv_stream_t* stream, int status)
{
	struct server* server;
	struct client* client = NULL;

	if (status == -1) return;

	server = stream->data;

	client = malloc(sizeof(*client));
	if (client == NULL) return;

	client_initialize(client, server);

	if (uv_accept(stream, (uv_stream_t*)&client->socket) != 0) {
		uv_close((uv_handle_t*)&client->socket, NULL);
		client_destroy(client);
		free(client);
		return;
	}

	client_read(client);
	TAILQ_INSERT_TAIL(&server->clients, client, link);
}

static void _on_watchdog_timer(uv_timer_t* handle, int status)
{
	struct client* client;
	struct server* server;

	server = handle->data;

	client = TAILQ_FIRST(&server->clients);
	while (client != NULL) {
		if (client->pace == 0) {
			fprintf(stderr, "The client(%s) goes down\n", client->command);
/*
			struct client* dead = client;
			client = TAILQ_NEXT(client, link);

			TAILQ_REMOVE(&server->clients, dead, link);

			// TODO:
			// Bring up the client
			fprintf(stderr, "The client(%s) goes down\n", dead->command);

			continue;
*/
		}

		client->pace = 0;
		client = TAILQ_NEXT(client, link);
	}
/*
	static unsigned int _COUNTER = 0;
	static char* _ROLLER_LIST = "|/-\\";

	fprintf(stderr, "\rThe watch dog is running... %c", _ROLLER_LIST[_COUNTER & 0x3]);
	_COUNTER++;
*/
}

int server_start(struct server* server)
{
	struct sockaddr_in addr;

	uv_signal_init(server->loop, &server->sighup);
	server->sighup.data = server;
	uv_signal_start(&server->sighup, _on_signal, SIGHUP);

	uv_signal_init(server->loop, &server->sigint);
	server->sigint.data = server;
	uv_signal_start(&server->sigint, _on_signal, SIGINT);

	uv_signal_init(server->loop, &server->sigkill);
	server->sigkill.data = server;
	uv_signal_start(&server->sigkill, _on_signal, SIGTERM);

	uv_signal_init(server->loop, &server->sigterm);
	server->sigterm.data = server;
	uv_signal_start(&server->sigterm, _on_signal, SIGTERM);

	uv_timer_init(server->loop, &server->watchdog);
	server->watchdog.data = server;
	uv_timer_start(&server->watchdog, _on_watchdog_timer, 5000, 5000);

	uv_tcp_init(server->loop, &server->socket);
	server->socket.data = server;

	addr = uv_ip4_addr("127.0.0.1", 7979);
	uv_tcp_bind(&server->socket, addr);

	uv_listen((uv_stream_t*)&server->socket, 32, _on_connect);

	return 0;
}

void server_detach(struct server* server, struct client* client)
{
	TAILQ_REMOVE(&client->server->clients, client, link);
}
