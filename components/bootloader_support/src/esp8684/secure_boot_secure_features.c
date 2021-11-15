/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
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

static __attribute__((unused)) const char *TAG = "secure_boot";

esp_err_t esp_secure_boot_enable_secure_features(void)
{
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_LEGACY_SPI_BOOT);

#ifdef CONFIG_SECURE_ENABLE_SECURE_ROM_DL_MODE
    ESP_LOGI(TAG, "Enabling Security download mode...");
    esp_efuse_write_field_bit(ESP_EFUSE_ENABLE_SECURITY_DOWNLOAD);
#else
    ESP_LOGW(TAG, "Not enabling Security download mode - SECURITY COMPROMISED");
#endif

#ifndef CONFIG_SECURE_BOOT_ALLOW_JTAG
    ESP_LOGI(TAG, "Disable hardware & software JTAG...");
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_PAD_JTAG);
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_USB_JTAG);
    esp_efuse_write_field_bit(ESP_EFUSE_SOFT_DIS_JTAG);
#else
    ESP_LOGW(TAG, "Not disabling JTAG - SECURITY COMPROMISED");
#endif

#ifdef CONFIG_SECURE_BOOT_ENABLE_AGGRESSIVE_KEY_REVOKE
    esp_efuse_write_field_bit(ESP_EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE);
#endif

    esp_efuse_write_field_bit(ESP_EFUSE_SECURE_BOOT_EN);

    return ESP_OK;
}
