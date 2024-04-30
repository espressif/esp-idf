/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Log level
 */
typedef enum {
    ESP_LOG_NONE    = 0,    /*!< No log output */
    ESP_LOG_ERROR   = 1,    /*!< Critical errors, software module can not recover on its own */
    ESP_LOG_WARN    = 2,    /*!< Error conditions from which recovery measures have been taken */
    ESP_LOG_INFO    = 3,    /*!< Information messages which describe normal flow of events */
    ESP_LOG_DEBUG   = 4,    /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
    ESP_LOG_VERBOSE = 5,    /*!< Bigger chunks of debugging information, or frequent messages which can potentially flood the output. */
    ESP_LOG_MAX     = 6,    /*!< Number of levels supported */
} esp_log_level_t;

/** @cond */

#ifndef LOG_LOCAL_LEVEL
#ifndef BOOTLOADER_BUILD
#define LOG_LOCAL_LEVEL  CONFIG_LOG_MAXIMUM_LEVEL
#else
#define LOG_LOCAL_LEVEL  CONFIG_BOOTLOADER_LOG_LEVEL
#endif
#endif

/** @endcond */

#ifdef __cplusplus
}
#endif
