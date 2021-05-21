/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This is a STUB FILE HEADER used when compiling ESP-IDF to run tests on the host system.
 * The header file used normally for ESP-IDF has the same name but is located elsewhere.
 */
#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#define vSemaphoreDelete( xSemaphore )
#define xSemaphoreCreateMutex()                     ((void*)(1))
#define xSemaphoreGive( xSemaphore )
#define xSemaphoreTake( xSemaphore, xBlockTime )    pdTRUE

typedef void* SemaphoreHandle_t;

#if defined(__cplusplus)
}
#endif
