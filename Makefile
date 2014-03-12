# Copyright (c) 2013, William W.L. Chuang
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of Pyraemon Studio nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

export LIBUV_VERSION = libuv-0.10.25

all: lib-uv socket-server socket-client

output-dir:
	@if [ ! -d "output" ]; then mkdir output; fi

output-clean:
	rm -rf output

lib-uv: output-dir
	@if [ ! -d "$(LIBUV_VERSION)" ]; then \
		tar -zxvf $(LIBUV_VERSION).tar.gz; \
	fi
	make -C $(LIBUV_VERSION) builddir_name=../output

lib-uv-clean:
	@rm -rf $(LIBUV_VERSION)
	@rm -rf output

socket-server:
	make -C server all

socket-server-clean:
	make -C server clean

socket-client:
	make -C client all

socket-client-clean:
	make -C client clean

clean:  lib-uv-clean \
		socket-server-clean socket-client-clean

.PHONY: output-dir output-clean all clean \
		lib-uv lib-uv-clean \
		socket-server socket-server-clean \
		socket-client socket-client-clean
