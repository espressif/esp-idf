/*
 * SPDX-FileCopyrightText: 2018-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <sys/types.h>
#include "sdkconfig.h"

#if CONFIG_LIBC_PICOLIBC_NEWLIB_COMPATIBILITY
#include <sys/select.h>
#endif

#include_next <sys/unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

int truncate(const char *, off_t __length);
int gethostname(char *__name, size_t __len);
int getentropy(void *buffer, size_t length);

#if CONFIG_LIBC_PICOLIBC
/* Not provided by picolibc. The values only have to be unique within IDF,
 * sysconf() is implemented by esp_libc.
 */
#ifndef _SC_NPROCESSORS_CONF
#define _SC_NPROCESSORS_CONF 138
#endif
#ifndef _SC_NPROCESSORS_ONLN
#define _SC_NPROCESSORS_ONLN 139
#endif
#endif // CONFIG_LIBC_PICOLIBC

#ifdef __cplusplus
}
#endif
