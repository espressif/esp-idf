/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __FS_H__
#define __FS_H__

#include "lwip/opt.h"
#include "lwip/err.h"

/** Set this to 1 and provide the functions:
 * - "int fs_open_custom(struct fs_file *file, const char *name)"
 *    Called first for every opened file to allow opening files
 *    that are not included in fsdata(_custom).c
 * - "void fs_close_custom(struct fs_file *file)"
 *    Called to free resources allocated by fs_open_custom().
 */
#ifndef LWIP_HTTPD_CUSTOM_FILES
#define LWIP_HTTPD_CUSTOM_FILES       0
#endif

/** Set this to 1 to support fs_read() to dynamically read file data.
 * Without this (default=off), only one-block files are supported,
 * and the contents must be ready after fs_open().
 */
#ifndef LWIP_HTTPD_DYNAMIC_FILE_READ
#define LWIP_HTTPD_DYNAMIC_FILE_READ  0
#endif

/** Set this to 1 to include an application state argument per file
 * that is opened. This allows to keep a state per connection/file.
 */
#ifndef LWIP_HTTPD_FILE_STATE
#define LWIP_HTTPD_FILE_STATE         0
#endif

/** HTTPD_PRECALCULATED_CHECKSUM==1: include precompiled checksums for
 * predefined (MSS-sized) chunks of the files to prevent having to calculate
 * the checksums at runtime. */
#ifndef HTTPD_PRECALCULATED_CHECKSUM
#define HTTPD_PRECALCULATED_CHECKSUM  0
#endif

/** LWIP_HTTPD_FS_ASYNC_READ==1: support asynchronous read operations
 * (fs_read_async returns FS_READ_DELAYED and calls a callback when finished).
 */
#ifndef LWIP_HTTPD_FS_ASYNC_READ
#define LWIP_HTTPD_FS_ASYNC_READ      0
#endif

#define FS_READ_EOF     -1
#define FS_READ_DELAYED -2

#if HTTPD_PRECALCULATED_CHECKSUM
struct fsdata_chksum {
  u32_t offset;
  u16_t chksum;
  u16_t len;
};
#endif /* HTTPD_PRECALCULATED_CHECKSUM */

struct fs_file {
  const char *data;
  int len;
  int index;
  void *pextension;
#if HTTPD_PRECALCULATED_CHECKSUM
  const struct fsdata_chksum *chksum;
  u16_t chksum_count;
#endif /* HTTPD_PRECALCULATED_CHECKSUM */
  u8_t http_header_included;
#if LWIP_HTTPD_CUSTOM_FILES
  u8_t is_custom_file;
#endif /* LWIP_HTTPD_CUSTOM_FILES */
#if LWIP_HTTPD_FILE_STATE
  void *state;
#endif /* LWIP_HTTPD_FILE_STATE */
};

#if LWIP_HTTPD_FS_ASYNC_READ
typedef void (*fs_wait_cb)(void *arg);
#endif /* LWIP_HTTPD_FS_ASYNC_READ */

err_t fs_open(struct fs_file *file, const char *name);
void fs_close(struct fs_file *file);
#if LWIP_HTTPD_DYNAMIC_FILE_READ
#if LWIP_HTTPD_FS_ASYNC_READ
int fs_read_async(struct fs_file *file, char *buffer, int count, fs_wait_cb callback_fn, void *callback_arg);
#else /* LWIP_HTTPD_FS_ASYNC_READ */
int fs_read(struct fs_file *file, char *buffer, int count);
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
#endif /* LWIP_HTTPD_DYNAMIC_FILE_READ */
#if LWIP_HTTPD_FS_ASYNC_READ
int fs_is_file_ready(struct fs_file *file, fs_wait_cb callback_fn, void *callback_arg);
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
int fs_bytes_left(struct fs_file *file);

#if LWIP_HTTPD_FILE_STATE
/** This user-defined function is called when a file is opened. */
void *fs_state_init(struct fs_file *file, const char *name);
/** This user-defined function is called when a file is closed. */
void fs_state_free(struct fs_file *file, void *state);
#endif /* #if LWIP_HTTPD_FILE_STATE */

#endif /* __FS_H__ */
