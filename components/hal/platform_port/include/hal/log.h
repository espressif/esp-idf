/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * When compiling a G0 application, `log` component is not available, thus its headers (`esp_log.h`) and Kconfig macros
 * are not available either.
 * In that case, we have to define the LOG macros to use ROM functions, which are part of G0 layer.
 */
#if __has_include("esp_log.h")

#include "esp_log.h"

#define HAL_LOGE(...)    ESP_LOGE(__VA_ARGS__)
#define HAL_LOGW(...)    ESP_LOGW(__VA_ARGS__)
#define HAL_LOGI(...)    ESP_LOGI(__VA_ARGS__)
#define HAL_LOGD(...)    ESP_LOGD(__VA_ARGS__)
#define HAL_LOGV(...)    ESP_LOGV(__VA_ARGS__)

#define HAL_EARLY_LOGE(...)    ESP_EARLY_LOGE(__VA_ARGS__)
#define HAL_EARLY_LOGW(...)    ESP_EARLY_LOGW(__VA_ARGS__)
#define HAL_EARLY_LOGI(...)    ESP_EARLY_LOGI(__VA_ARGS__)
#define HAL_EARLY_LOGD(...)    ESP_EARLY_LOGD(__VA_ARGS__)
#define HAL_EARLY_LOGV(...)    ESP_EARLY_LOGV(__VA_ARGS__)

#else // __has_include("esp_log.h")

#include "esp_rom_sys.h"

#define HAL_LOG_NONE    0
#define HAL_LOG_ERROR   1
#define HAL_LOG_WARN    2
#define HAL_LOG_INFO    3
#define HAL_LOG_DEBUG   4
#define HAL_LOG_VERBOSE 5


#if CONFIG_HAL_LOG_LEVEL >= HAL_LOG_ERROR
    #define HAL_LOGE(tag, fmt, ...) esp_rom_printf("%s(err): " fmt, tag, ##__VA_ARGS__)
#else
    #define HAL_LOGE(tag, fmt, ...)
#endif

#if CONFIG_HAL_LOG_LEVEL >= HAL_LOG_WARN
    #define HAL_LOGW(tag, fmt, ...) esp_rom_printf("%s(warn): " fmt, tag, ##__VA_ARGS__)
#else
    #define HAL_LOGW(tag, fmt, ...)
#endif

#if CONFIG_HAL_LOG_LEVEL >= HAL_LOG_INFO
    #define HAL_LOGI(tag, fmt, ...) esp_rom_printf("%s(info): " fmt, tag, ##__VA_ARGS__)
#else
    #define HAL_LOGI(tag, fmt, ...)
#endif

#if CONFIG_HAL_LOG_LEVEL >= HAL_LOG_DEBUG
    #define HAL_LOGD(tag, fmt, ...) esp_rom_printf("%s(dbg): " fmt, tag, ##__VA_ARGS__)
#else
    #define HAL_LOGD(tag, fmt, ...)
#endif

#if CONFIG_HAL_LOG_LEVEL >= HAL_LOG_VERBOSE
    #define HAL_LOGV(tag, fmt, ...) esp_rom_printf("%s: " fmt, tag, ##__VA_ARGS__)
#else
    #define HAL_LOGV(tag, fmt, ...)
#endif

#define HAL_EARLY_LOGE(...)    HAL_LOGE(__VA_ARGS__)
#define HAL_EARLY_LOGW(...)    HAL_LOGW(__VA_ARGS__)
#define HAL_EARLY_LOGI(...)    HAL_LOGI(__VA_ARGS__)
#define HAL_EARLY_LOGD(...)    HAL_LOGD(__VA_ARGS__)
#define HAL_EARLY_LOGV(...)    HAL_LOGV(__VA_ARGS__)

#endif // __has_include("esp_log.h")
