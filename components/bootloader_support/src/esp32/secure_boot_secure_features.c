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
#ifdef CONFIG_SECURE_BOOT_V1_ENABLED
    ESP_LOGI(TAG, "Read & write protecting new key...");
    esp_efuse_write_field_bit(ESP_EFUSE_RD_DIS_BLK2);
    esp_efuse_write_field_bit(ESP_EFUSE_WR_DIS_BLK2);


    esp_efuse_write_field_bit(ESP_EFUSE_ABS_DONE_0);

#ifndef CONFIG_SECURE_BOOT_ALLOW_JTAG
    ESP_LOGI(TAG, "Disable JTAG...");
    esp_efuse_write_field_bit(ESP_EFUSE_DISABLE_JTAG);
#else
    ESP_LOGW(TAG, "Not disabling JTAG - SECURITY COMPROMISED");
#endif

#ifndef CONFIG_SECURE_BOOT_ALLOW_ROM_BASIC
    ESP_LOGI(TAG, "Disable ROM BASIC interpreter fallback...");
    esp_efuse_write_field_bit(ESP_EFUSE_CONSOLE_DEBUG_DISABLE);
#else
    ESP_LOGW(TAG, "Not disabling ROM BASIC fallback - SECURITY COMPROMISED");
#endif
#endif // CONFIG_SECURE_BOOT_V1_ENABLED



#ifdef CONFIG_SECURE_BOOT_V2_ENABLED
    esp_err_t err;
    err = esp_efuse_write_field_bit(ESP_EFUSE_ABS_DONE_1);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Blowing secure boot efuse...failed.");
        return err;
    }

#ifndef CONFIG_SECURE_BOOT_ALLOW_JTAG
    ESP_LOGI(TAG, "Disable JTAG...");
    err = esp_efuse_write_field_bit(ESP_EFUSE_DISABLE_JTAG);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Disable JTAG...failed.");
        return err;
    }
#else
    ESP_LOGW(TAG, "Not disabling JTAG - SECURITY COMPROMISED");
#endif

#ifndef CONFIG_SECURE_BOOT_ALLOW_ROM_BASIC
    ESP_LOGI(TAG, "Disable ROM BASIC interpreter fallback...");
    err = esp_efuse_write_field_bit(ESP_EFUSE_CONSOLE_DEBUG_DISABLE);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Disable ROM BASIC interpreter fallback...failed.");
        return err;
    }
#else
    ESP_LOGW(TAG, "Not disabling ROM BASIC fallback - SECURITY COMPROMISED");
#endif

#ifdef CONFIG_SECURE_DISABLE_ROM_DL_MODE
    ESP_LOGI(TAG, "Disable ROM Download mode...");
    err = esp_efuse_disable_rom_download_mode();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not disable ROM Download mode...");
        return err;
    }
#else
    ESP_LOGW(TAG, "UART ROM Download mode kept enabled - SECURITY COMPROMISED");
#endif

#ifndef CONFIG_SECURE_BOOT_V2_ALLOW_EFUSE_RD_DIS
    bool rd_dis_now = true;
#ifdef CONFIG_SECURE_FLASH_ENC_ENABLED
    /* If flash encryption is not enabled yet then don't read-disable efuses yet, do it later in the boot
       when Flash Encryption is being enabled */
    rd_dis_now = esp_flash_encryption_enabled();
#endif
    if (rd_dis_now) {
        ESP_LOGI(TAG, "Prevent read disabling of additional efuses...");
        err = esp_efuse_write_field_bit(ESP_EFUSE_WR_DIS_EFUSE_RD_DISABLE);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Prevent read disabling of additional efuses...failed.");
            return err;
        }
    }
#else
    ESP_LOGW(TAG, "Allowing read disabling of additional efuses - SECURITY COMPROMISED");
#endif
#endif // CONFIG_SECURE_BOOT_V2_ENABLED


    return ESP_OK;
}
