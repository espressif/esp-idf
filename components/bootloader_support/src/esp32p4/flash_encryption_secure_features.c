/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
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
#include "hal/key_mgr_ll.h"
#include "hal/mspi_timing_tuning_ll.h"

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
    esp_efuse_write_field_bit(ESP_EFUSE_SPI_DOWNLOAD_MSPI_DIS);
#else
    ESP_LOGW(TAG, "Not disabling UART bootloader cache - SECURITY COMPROMISED");
#endif

#ifndef CONFIG_SECURE_BOOT_ALLOW_JTAG
    ESP_LOGI(TAG, "Disable JTAG...");
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_PAD_JTAG);
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_USB_JTAG);
#else
    ESP_LOGW(TAG, "Not disabling JTAG - SECURITY COMPROMISED");
#endif

    esp_efuse_write_field_bit(ESP_EFUSE_DIS_DIRECT_BOOT);

#if defined(CONFIG_SECURE_BOOT_V2_ENABLED) && !defined(CONFIG_SECURE_BOOT_V2_ALLOW_EFUSE_RD_DIS)
    // This bit is set when enabling Secure Boot V2, but we can't enable it until this later point in the first boot
    // otherwise the Flash Encryption key cannot be read protected
    esp_efuse_write_field_bit(ESP_EFUSE_WR_DIS_RD_DIS);
#endif

    return ESP_OK;
}

esp_err_t esp_flash_encryption_enable_key_mgr(void)
{
    // Enable and reset key manager
    // To suppress build errors about spinlock's __DECLARE_RCC_ATOMIC_ENV
    int __DECLARE_RCC_ATOMIC_ENV __attribute__ ((unused));
    key_mgr_ll_enable_bus_clock(true);
    key_mgr_ll_enable_peripheral_clock(true);
    key_mgr_ll_reset_register();

    while (key_mgr_ll_get_state() != ESP_KEY_MGR_STATE_IDLE) {
    };

    // Force Key Manager to use eFuse key for XTS-AES operation
    key_mgr_ll_set_key_usage(ESP_KEY_MGR_XTS_AES_128_KEY, ESP_KEY_MGR_USE_EFUSE_KEY);
    _mspi_timing_ll_reset_mspi();

    return ESP_OK;
}
