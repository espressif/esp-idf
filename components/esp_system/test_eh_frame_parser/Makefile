#
# Copyright 2020 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

CC=gcc
CFLAGS=-W -fasynchronous-unwind-tables -I. -I../include/ -std=c99 -g -DCONFIG_ESP_SYSTEM_USE_EH_FRAME -m32
LDFLAGS=-Wl,--eh-frame-hdr -m32 -g -Tlinker.ld -no-pie
OBJECTS=objs/eh_frame_parser.o objs/main.o
HEADERS=eh_frame_parser_impl.h
BIN=eh_frame_test

.PHONY: all clean createdir

all: $(OBJECTS)
	$(CC) -o $(BIN) $^ $(LDFLAGS)

objs/main.o:main.c
	@mkdir -p objs
	$(CC) -o $@ -c $^ $(CFLAGS)

objs/eh_frame_parser.o: ../eh_frame_parser.c
	@mkdir -p objs
	$(CC) -o $@ -c $^ $(CFLAGS)

clean:
	rm -rf objs $(BIN)
