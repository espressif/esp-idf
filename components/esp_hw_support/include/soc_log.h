/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "esp_rom_sys.h"

/**
 * @file soc_log.h
 * @brief SOC library logging functions
 *
 * To make SOC library compatible with environments which don't use ESP-IDF,
 * this header file provides wrappers for logging functions.
 */

#ifdef ESP_PLATFORM
#include "esp_log.h"
#define SOC_LOGE(tag, fmt, ...) ESP_EARLY_LOGE(tag, fmt, ##__VA_ARGS__)
#define SOC_LOGW(tag, fmt, ...) ESP_EARLY_LOGW(tag, fmt, ##__VA_ARGS__)
#define SOC_LOGI(tag, fmt, ...) ESP_EARLY_LOGI(tag, fmt, ##__VA_ARGS__)
#define SOC_LOGD(tag, fmt, ...) ESP_EARLY_LOGD(tag, fmt, ##__VA_ARGS__)
#define SOC_LOGV(tag, fmt, ...) ESP_EARLY_LOGV(tag, fmt, ##__VA_ARGS__)

#else

#define SOC_LOGE(tag, fmt, ...) esp_rom_printf("%s(err): " fmt, tag, ##__VA_ARGS__)
#define SOC_LOGW(tag, fmt, ...) esp_rom_printf("%s(warn): " fmt, tag, ##__VA_ARGS__)
#define SOC_LOGI(tag, fmt, ...) esp_rom_printf("%s(info): " fmt, tag, ##__VA_ARGS__)
#define SOC_LOGD(tag, fmt, ...) esp_rom_printf("%s(dbg): " fmt, tag, ##__VA_ARGS__)
#define SOC_LOGV(tag, fmt, ...) esp_rom_printf("%s: " fmt, tag, ##__VA_ARGS__)
#endif //ESP_PLATFORM
