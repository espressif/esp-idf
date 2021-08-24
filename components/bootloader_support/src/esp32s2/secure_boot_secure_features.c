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
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_BOOT_REMAP);
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_LEGACY_SPI_BOOT);

#ifdef CONFIG_SECURE_ENABLE_SECURE_ROM_DL_MODE
    ESP_LOGI(TAG, "Enabling Security download mode...");
    esp_efuse_write_field_bit(ESP_EFUSE_ENABLE_SECURITY_DOWNLOAD);
#else
    ESP_LOGW(TAG, "Not enabling Security download mode - SECURITY COMPROMISED");
#endif

#ifndef CONFIG_SECURE_BOOT_ALLOW_JTAG
    ESP_LOGI(TAG, "Disable hardware & software JTAG...");
    esp_efuse_write_field_bit(ESP_EFUSE_HARD_DIS_JTAG);
    esp_efuse_write_field_bit(ESP_EFUSE_SOFT_DIS_JTAG);
#else
    ESP_LOGW(TAG, "Not disabling JTAG - SECURITY COMPROMISED");
#endif

#ifdef CONFIG_SECURE_BOOT_ENABLE_AGGRESSIVE_KEY_REVOKE
    esp_efuse_write_field_bit(ESP_EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE);
#endif

    esp_efuse_write_field_bit(ESP_EFUSE_SECURE_BOOT_EN);

#ifndef CONFIG_SECURE_BOOT_V2_ALLOW_EFUSE_RD_DIS
    bool rd_dis_now = true;
#ifdef CONFIG_SECURE_FLASH_ENC_ENABLED
    /* If flash encryption is not enabled yet then don't read-disable efuses yet, do it later in the boot
       when Flash Encryption is being enabled */
    rd_dis_now = esp_flash_encryption_enabled();
#endif
    if (rd_dis_now) {
        ESP_LOGI(TAG, "Prevent read disabling of additional efuses...");
        esp_efuse_write_field_bit(ESP_EFUSE_WR_DIS_RD_DIS);
    }
#else
    ESP_LOGW(TAG, "Allowing read disabling of additional efuses - SECURITY COMPROMISED");
#endif

    return ESP_OK;
}
