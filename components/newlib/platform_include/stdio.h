/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include_next <stdio.h>
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_LIBC_PICOLIBC_NEWLIB_COMPATIBILITY
#include <stddef.h>
void flockfile(FILE *);
void funlockfile(FILE *);
FILE *open_memstream(char **, size_t *);
#endif

#ifdef __cplusplus
}
#endif
