/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include "sdkconfig.h"

#include_next <signal.h>

#ifdef __cplusplus
extern "C" {
#endif

/* TODO GCC-441: remove this file */
#if CONFIG_LIBC_PICOLIBC
#if __POSIX_VISIBLE >= 199506
int pthread_sigmask(int, const sigset_t *, sigset_t *);
#endif
#endif

#ifdef __cplusplus
}
#endif
