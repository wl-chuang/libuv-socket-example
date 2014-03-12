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

#ifndef _THE_UV_SOCKET_EXAMPLE_CORE_H_INCLUDED
#define _THE_UV_SOCKET_EXAMPLE_CORE_H_INCLUDED


#include <uv.h>
#include "queue.h"


struct server;


struct client
{
	//uv_loop_t* loop;
	struct server* server;
	uv_tcp_t   socket;

	TAILQ_ENTRY(client) link;

	char* command;
	int   pace;

	char  buffer[256];
	//int   bufferLength;
	//int   dataRead;
};

TAILQ_HEAD(client_list, client);

struct server
{
	uv_loop_t*  loop;

	uv_tcp_t    socket;
	uv_timer_t  watchdog;

	uv_signal_t sighup;
	uv_signal_t sigint;
	uv_signal_t sigkill;
	uv_signal_t sigterm;

	struct client_list clients;
};


int  server_initialize(uv_loop_t* loop, struct server* server);
void server_destroy(struct server* server);
int server_start(struct server* server);
void server_detach(struct server* server, struct client* client);

int  client_initialize(struct client* client, struct server* server);
void client_destroy(struct client* client);
void client_read(struct client* client);
void client_disconnect(struct client* client, uv_close_cb callback);
//void client_close(struct client* client);


#endif /* _THE_UV_SOCKET_EXAMPLE_CORE_H_INCLUDED */
