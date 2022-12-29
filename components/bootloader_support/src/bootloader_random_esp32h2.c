/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "bootloader_random.h"
#include "esp_log.h"

static const char *TAG = "bootloader_random";
void bootloader_random_enable(void)
{
    // ESP32H2-TODO: IDF-6274
    ESP_EARLY_LOGW(TAG, "bootloader_random_enable() has not been implemented yet");
}

void bootloader_random_disable(void)
{
    // ESP32H2-TODO: IDF-6274
    ESP_EARLY_LOGW(TAG, "bootloader_random_disable() has not been implemented yet");
}
