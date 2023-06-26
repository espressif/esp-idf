/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
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
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_DIRECT_BOOT);

#ifdef CONFIG_SECURE_ENABLE_SECURE_ROM_DL_MODE
    ESP_LOGI(TAG, "Enabling Security download mode...");
    esp_err_t err = esp_efuse_enable_rom_secure_download_mode();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not enable Security download mode...");
        return err;
    }
#elif CONFIG_SECURE_DISABLE_ROM_DL_MODE
    ESP_LOGI(TAG, "Disable ROM Download mode...");
    esp_err_t err = esp_efuse_disable_rom_download_mode();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not disable ROM Download mode...");
        return err;
    }
#else
    ESP_LOGW(TAG, "UART ROM Download mode kept enabled - SECURITY COMPROMISED");
#endif

#ifndef CONFIG_SECURE_BOOT_ALLOW_JTAG
    ESP_LOGI(TAG, "Disable hardware & software JTAG...");
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_PAD_JTAG);
#else
    ESP_LOGW(TAG, "Not disabling JTAG - SECURITY COMPROMISED");
#endif

    esp_efuse_write_field_bit(ESP_EFUSE_SECURE_BOOT_EN);

#ifndef CONFIG_SECURE_BOOT_V2_ALLOW_EFUSE_RD_DIS
    // Secure boot and Flash encryption share one eFuse key block so they can not be set separately.
    // CONFIG_SECURE_BOOT_FLASH_ENC_KEYS_BURN_TOGETHER option is used to burn SB and FE at the same time.
    // SB key is readable, the corresponding bit in RD_DIS is unset.
    //   We set write-protection for RD_DIS to ensure that the SB key is always readable.
    // FE key is read-protected, the corresponding bit in RD_DIS is set.
    ESP_LOGI(TAG, "Prevent read disabling of additional efuses...");
    esp_efuse_write_field_bit(ESP_EFUSE_WR_DIS_RD_DIS);
#else
    ESP_LOGW(TAG, "Allowing read disabling of additional efuses - SECURITY COMPROMISED");
#endif

    return ESP_OK;
}
