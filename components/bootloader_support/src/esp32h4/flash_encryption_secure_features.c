/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <strings.h>
#include "esp_flash_encrypt.h"
#include "esp_secure_boot.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_log.h"
#include "sdkconfig.h"

//TODO: [ESP32H4] IDF-12261

static __attribute__((unused)) const char *TAG = "flash_encrypt";

esp_err_t esp_flash_encryption_enable_secure_features(void)
{
    abort();
    return ESP_OK;
}
