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

    /* CRYPT_CONFIG determines which bits of the AES block key are XORed
       with bits from the flash address, to provide the key tweak.

       CRYPT_CONFIG == 0 is effectively AES ECB mode (NOT SUPPORTED)

       For now this is hardcoded to XOR all 256 bits of the key.

       If you need to override it, you can pre-burn this efuse to the
       desired value and then write-protect it, in which case this
       operation does nothing. Please note this is not recommended!
    */
    ESP_LOGI(TAG, "Setting CRYPT_CONFIG efuse to 0xF");
    uint32_t crypt_config = 0;
    esp_efuse_read_field_blob(ESP_EFUSE_ENCRYPT_CONFIG, &crypt_config, 4);
    if (crypt_config == 0) {
        crypt_config = EFUSE_FLASH_CRYPT_CONFIG;
        esp_efuse_write_field_blob(ESP_EFUSE_ENCRYPT_CONFIG, &crypt_config, 4);
    } else if (crypt_config != EFUSE_FLASH_CRYPT_CONFIG) {
        ESP_LOGE(TAG, "EFUSE_ENCRYPT_CONFIG should be set 0xF but it is 0x%x", crypt_config);
    }

#ifndef CONFIG_SECURE_FLASH_UART_BOOTLOADER_ALLOW_ENC
    ESP_LOGI(TAG, "Disable UART bootloader encryption...");
    esp_efuse_write_field_bit(ESP_EFUSE_DISABLE_DL_ENCRYPT);
#else
    ESP_LOGW(TAG, "Not disabling UART bootloader encryption");
#endif

#ifndef CONFIG_SECURE_FLASH_UART_BOOTLOADER_ALLOW_DEC
    ESP_LOGI(TAG, "Disable UART bootloader decryption...");
    esp_efuse_write_field_bit(ESP_EFUSE_DISABLE_DL_DECRYPT);
#else
    ESP_LOGW(TAG, "Not disabling UART bootloader decryption - SECURITY COMPROMISED");
#endif

#ifndef CONFIG_SECURE_FLASH_UART_BOOTLOADER_ALLOW_CACHE
    ESP_LOGI(TAG, "Disable UART bootloader MMU cache...");
    esp_efuse_write_field_bit(ESP_EFUSE_DISABLE_DL_CACHE);
#else
    ESP_LOGW(TAG, "Not disabling UART bootloader MMU cache - SECURITY COMPROMISED");
#endif

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

#if defined(CONFIG_SECURE_BOOT_V2_ENABLED) && !defined(CONFIG_SECURE_BOOT_V2_ALLOW_EFUSE_RD_DIS)
    // This bit is set when enabling Secure Boot V2, but we can't enable it until this later point in the first boot
    // otherwise the Flash Encryption key cannot be read protected
    esp_efuse_write_field_bit(ESP_EFUSE_WR_DIS_EFUSE_RD_DISABLE);
#endif

#ifndef CONFIG_SECURE_FLASH_SKIP_WRITE_PROTECTION_CACHE
    // Set write-protection for DIS_ICACHE to prevent bricking chip in case it will be set accidentally.
    // esp32 has DIS_ICACHE. Write-protection bit = 3.
    // List of eFuses with the same write protection bit:
    // MAC, MAC_CRC, DISABLE_APP_CPU, DISABLE_BT, DIS_CACHE, VOL_LEVEL_HP_INV.
    esp_efuse_write_field_bit(ESP_EFUSE_WR_DIS_DIS_CACHE);
#endif

    return ESP_OK;
}
