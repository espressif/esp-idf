/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

typedef struct mbedtls_threading_mutex_t {
    SemaphoreHandle_t mutex;
    /* is_valid is 0 after a failed init or a free, and nonzero after a
     * successful init. This field is not considered part of the public
     * API of Mbed TLS and may change without notice. */
    char is_valid;
} mbedtls_threading_mutex_t;

extern void mbedtls_threading_set_alt(void (*mutex_init)(mbedtls_threading_mutex_t *),
                                      void (*mutex_free)(mbedtls_threading_mutex_t *),
                                      int (*mutex_lock)(mbedtls_threading_mutex_t *),
                                      int (*mutex_unlock)(mbedtls_threading_mutex_t *));
