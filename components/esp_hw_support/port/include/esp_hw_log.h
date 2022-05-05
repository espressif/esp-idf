/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "esp_rom_sys.h"

/**
 * @file esp_hw_log.h
 * @brief ESP HW Support logging functions
 *
 * This header file provides wrappers for logging functions for libraries which may or
 * may not use the ESP-IDF environment.
 */

#ifdef ESP_PLATFORM
#include "esp_log.h"
#define ESP_HW_LOGE(tag, fmt, ...) ESP_EARLY_LOGE(tag, fmt, ##__VA_ARGS__)
#define ESP_HW_LOGW(tag, fmt, ...) ESP_EARLY_LOGW(tag, fmt, ##__VA_ARGS__)
#define ESP_HW_LOGI(tag, fmt, ...) ESP_EARLY_LOGI(tag, fmt, ##__VA_ARGS__)
#define ESP_HW_LOGD(tag, fmt, ...) ESP_EARLY_LOGD(tag, fmt, ##__VA_ARGS__)
#define ESP_HW_LOGV(tag, fmt, ...) ESP_EARLY_LOGV(tag, fmt, ##__VA_ARGS__)

#else

#define ESP_HW_LOGE(tag, fmt, ...) esp_rom_printf("%s(err): " fmt, tag, ##__VA_ARGS__)
#define ESP_HW_LOGW(tag, fmt, ...) esp_rom_printf("%s(warn): " fmt, tag, ##__VA_ARGS__)
#define ESP_HW_LOGI(tag, fmt, ...) esp_rom_printf("%s(info): " fmt, tag, ##__VA_ARGS__)
#define ESP_HW_LOGD(tag, fmt, ...) esp_rom_printf("%s(dbg): " fmt, tag, ##__VA_ARGS__)
#define ESP_HW_LOGV(tag, fmt, ...) esp_rom_printf("%s: " fmt, tag, ##__VA_ARGS__)
#endif //ESP_PLATFORM
