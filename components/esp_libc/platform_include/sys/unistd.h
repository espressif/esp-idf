/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <sys/types.h>

#include_next <sys/unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

int truncate(const char *, off_t __length);
int gethostname(char *__name, size_t __len);
int getentropy(void *buffer, size_t length);

#ifdef __cplusplus
}
#endif
