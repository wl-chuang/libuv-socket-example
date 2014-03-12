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

#include <uv.h>


static uv_tcp_t      _socket;
static uv_connect_t  _connect;
//static uv_shutdown_t _shutdown;
static uv_timer_t    _pacemaker;

static char* _command = "hello world";


static void _on_write(uv_write_t* req, int status)
{
	free(req);
/*
	uv_stream_t* stream;

	stream = req->handle;

	uv_shutdown(&_shutdown, stream, NULL);
*/
}

static void _on_pace_timer(uv_timer_t* handle, int status)
{
	uv_buf_t buffer;
	uv_stream_t* stream;
	uv_write_t*  write_req;

	stream = handle->data;
	buffer = uv_buf_init(_command, strlen(_command));

	write_req = malloc(sizeof(*write_req));
	uv_write(write_req, stream, &buffer, 1, _on_write);
}

static void _on_connect(uv_connect_t* req, int status)
{
	uv_buf_t buffer;
	uv_stream_t* stream;
	uv_write_t*  write_req;

	stream = req->handle;
	buffer = uv_buf_init(_command, strlen(_command));

	write_req = malloc(sizeof(*write_req));
	uv_write(write_req, stream, &buffer, 1, _on_write);

	uv_timer_init(uv_default_loop(), &_pacemaker);
	_pacemaker.data = stream;
	uv_timer_start(&_pacemaker, _on_pace_timer, 2000, 2000);
}

int main(int argc, char** argv)
{
	uv_loop_t* loop;
	struct sockaddr_in dest;

	loop = uv_default_loop();

	//uv_timer_init(server->loop, &server->watchdog);
	//server->watchdog.data = server;
	//uv_timer_start(&server->watchdog, _on_watchdog_timer, 5000, 5000);

	uv_tcp_init(loop, &_socket);
	//_socket.data = server;

	dest = uv_ip4_addr("127.0.0.1", 7979);
	uv_tcp_connect(&_connect, &_socket, dest, _on_connect);

	uv_run(loop, UV_RUN_DEFAULT);

	return 0;
}
