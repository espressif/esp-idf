/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This is a STUB FILE HEADER used when compiling ESP-IDF to run tests on the host system.
 * The header file used normally for ESP-IDF has the same name but is located elsewhere.
 */
#pragma once

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int _lock_t;

void _lock_acquire(_lock_t *lock);
void _lock_close(_lock_t *lock);
void _lock_init(_lock_t *lock);
void _lock_release(_lock_t *lock);

#ifdef __cplusplus
}
#endif
