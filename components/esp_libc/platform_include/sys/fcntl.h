/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"

#include_next <sys/fcntl.h>

#if CONFIG_LIBC_PICOLIBC_NEWLIB_COMPATIBILITY
#include <time.h>
#include <sys/time.h>
#endif
