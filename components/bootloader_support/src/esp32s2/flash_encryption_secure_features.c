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

static __attribute__((unused)) const char *TAG = "flash_encrypt";

esp_err_t esp_flash_encryption_enable_secure_features(void)
{
#ifndef CONFIG_SECURE_FLASH_UART_BOOTLOADER_ALLOW_ENC
    ESP_LOGI(TAG, "Disable UART bootloader encryption...");
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT);
#else
    ESP_LOGW(TAG, "Not disabling UART bootloader encryption");
#endif

#ifndef CONFIG_SECURE_FLASH_UART_BOOTLOADER_ALLOW_CACHE
    ESP_LOGI(TAG, "Disable UART bootloader cache...");
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_DOWNLOAD_DCACHE);
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_DOWNLOAD_ICACHE);
#else
    ESP_LOGW(TAG, "Not disabling UART bootloader cache - SECURITY COMPROMISED");
#endif

#ifndef CONFIG_SECURE_BOOT_ALLOW_JTAG
    ESP_LOGI(TAG, "Disable JTAG...");
    esp_efuse_write_field_bit(ESP_EFUSE_HARD_DIS_JTAG);
#else
    ESP_LOGW(TAG, "Not disabling JTAG - SECURITY COMPROMISED");
#endif

    esp_efuse_write_field_bit(ESP_EFUSE_DIS_BOOT_REMAP);
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_LEGACY_SPI_BOOT);

#if defined(CONFIG_SECURE_BOOT_V2_ENABLED) && !defined(CONFIG_SECURE_BOOT_V2_ALLOW_EFUSE_RD_DIS)
    // This bit is set when enabling Secure Boot V2, but we can't enable it until this later point in the first boot
    // otherwise the Flash Encryption key cannot be read protected
    esp_efuse_write_field_bit(ESP_EFUSE_WR_DIS_RD_DIS);
#endif

#ifndef CONFIG_SECURE_FLASH_SKIP_WRITE_PROTECTION_CACHE
    // Set write-protection for DIS_ICACHE and DIS_DCACHE to prevent bricking chip in case it will be set accidentally.
    // esp32s2 has DIS_ICACHE and DIS_DCACHE. Write-protection bit = 2 for both.
    // List of eFuses with the same write protection bit:
    // DIS_ICACHE, DIS_DCACHE, DIS_DOWNLOAD_ICACHE, DIS_DOWNLOAD_DCACHE,
    // DIS_FORCE_DOWNLOAD, DIS_USB, DIS_TWAI, DIS_BOOT_REMAP, SOFT_DIS_JTAG,
    // HARD_DIS_JTAG, DIS_DOWNLOAD_MANUAL_ENCRYPT.
    esp_efuse_write_field_bit(ESP_EFUSE_WR_DIS_DIS_ICACHE);
#endif

    return ESP_OK;
}
