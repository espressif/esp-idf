/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

struct mbedtls_rom_cond_waiter;

typedef struct mbedtls_platform_mutex_t {
    SemaphoreHandle_t mutex;
    /* is_valid is 0 after a failed init or a free, and nonzero after a
     * successful init. This field is not considered part of the public
     * API of Mbed TLS and may change without notice. */
    char is_valid;
} mbedtls_platform_mutex_t;

typedef struct mbedtls_platform_condition_variable_t {
    /* Protects the waiter list below. */
    SemaphoreHandle_t mutex;
    /* Intrusive list of threads currently blocked on this condition variable. */
    struct mbedtls_rom_cond_waiter *waiters;
    char is_valid;
} mbedtls_platform_condition_variable_t;
