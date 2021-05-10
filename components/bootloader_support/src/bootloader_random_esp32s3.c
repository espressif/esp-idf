/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "bootloader_random.h"
#include "esp_log.h"

static const char *TAG = "bootloader_random";

void bootloader_random_enable(void)
{
    ESP_LOGW(TAG, "RNG for ESP32-S3 not currently supported"); // IDF-1878
    // Don't forget to remove the following line
    // *libbootloader_support.a:bootloader_random*.*(.literal.bootloader_random_enable .text.bootloader_random_enable)
    // In the bootloader.ld when RNG support is ready for ESP32-S3
}

void bootloader_random_disable(void)
{
    ESP_LOGW(TAG, "RNG for ESP32-S3 not currently supported"); // IDF-1878
}
