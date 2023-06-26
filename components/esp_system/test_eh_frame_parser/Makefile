 # SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 #
 # SPDX-License-Identifier: Apache-2.0

CC=gcc
CFLAGS=-W -fasynchronous-unwind-tables -I. -I../port/include/x86/ -I../include/ -I../include/esp_private -std=c99 -g -m32
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
