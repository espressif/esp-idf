// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t esp_err_t;

/* Definitions for error constants. */

#define ESP_OK          0
#define ESP_FAIL        -1

#define ESP_ERR_NO_MEM          0x101
#define ESP_ERR_INVALID_ARG     0x102
#define ESP_ERR_INVALID_STATE   0x103
#define ESP_ERR_INVALID_SIZE    0x104
#define ESP_ERR_NOT_FOUND       0x105
#define ESP_ERR_NOT_SUPPORTED   0x106
#define ESP_ERR_TIMEOUT         0x107
#define ESP_ERR_INVALID_RESPONSE    0x108
#define ESP_ERR_INVALID_CRC     0x109

#define ESP_ERR_WIFI_BASE       0x3000 /*!< Starting number of WiFi error codes */

void _esp_error_check_failed(esp_err_t rc, const char *file, int line, const char *function, const char *expression) __attribute__((noreturn));

#ifndef __ASSERT_FUNC
/* This won't happen on IDF, which defines __ASSERT_FUNC in assert.h, but it does happen when building on the host which
   uses /usr/include/assert.h or equivalent.
*/
#ifdef __ASSERT_FUNCTION
#define __ASSERT_FUNC __ASSERT_FUNCTION /* used in glibc assert.h */
#else
#define __ASSERT_FUNC "??"
#endif
#endif

/**
 * Macro which can be used to check the error code,
 * and terminate the program in case the code is not ESP_OK.
 * Prints the error code, error location, and the failed statement to serial output.
 *
 * Disabled if assertions are disabled.
 */
#ifdef NDEBUG
#define ESP_ERROR_CHECK(x) do {                                         \
        esp_err_t rc = (x);                                             \
        (void) sizeof(rc);                                              \
    } while(0);
#else
#define ESP_ERROR_CHECK(x) do {                                         \
        esp_err_t rc = (x);                                             \
        if (rc != ESP_OK) {                                             \
            _esp_error_check_failed(rc, __FILE__, __LINE__,             \
                                    __ASSERT_FUNC, #x);                 \
        }                                                               \
    } while(0);
#endif

#ifdef __cplusplus
}
#endif
