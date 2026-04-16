/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include_next <sys/cdefs.h>
#include "sdkconfig.h"

#if CONFIG_LIBC_PICOLIBC_NEWLIB_COMPATIBILITY
#include <stdint.h>
#include <stddef.h>
#endif

#if CONFIG_LIBC_PICOLIBC // TODO IDF-15569
#undef __noreturn
#if __has_attribute(__noreturn__)
#define __noreturn __attribute__((__noreturn__))
#else
#define __noreturn
#endif
#else
#define __noreturn
#endif
