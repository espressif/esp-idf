/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

#include_next <stdio.h>

#if CONFIG_LIBC_PICOLIBC
FILE *open_memstream(char **, size_t *);
#endif
