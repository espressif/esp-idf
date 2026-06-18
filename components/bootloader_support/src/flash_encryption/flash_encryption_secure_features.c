/*
 * SPDX-FileCopyrightText: 2017-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Unified flash-encryption secure-features implementation shared by all targets
 * except ESP32 (which has a distinct eFuse family and is handled in
 * src/esp32/flash_encryption_secure_features.c). All per-target differences are
 * expressed through SOC_EFUSE_* capabilities rather than per-target files.
 */

#include <stdint.h>
#include <strings.h>
#include "esp_flash_encrypt.h"
#include "esp_secure_boot.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_log.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"
#include "hal/spi_flash_encrypted_ll.h"
#if SOC_KEY_MANAGER_FE_KEY_DEPLOY
#include "esp_crypto_periph_clk.h"
#include "esp_key_mgr.h"
#include "hal/key_mgr_hal.h"
#include "hal/key_mgr_ll.h"
#endif

ESP_LOG_ATTR_TAG(TAG, "flash_encrypt");

esp_err_t esp_flash_encryption_enable_secure_features(void)
{
#ifndef CONFIG_SECURE_FLASH_UART_BOOTLOADER_ALLOW_ENC
    ESP_LOGI(TAG, "Disable UART bootloader encryption...");
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT);
#else
    ESP_LOGW(TAG, "Not disabling UART bootloader encryption");
#endif

#if SOC_EFUSE_DIS_DOWNLOAD_MSPI || SOC_EFUSE_DIS_DOWNLOAD_ICACHE || SOC_EFUSE_DIS_DOWNLOAD_DCACHE
#ifndef CONFIG_SECURE_FLASH_UART_BOOTLOADER_ALLOW_CACHE
    ESP_LOGI(TAG, "Disable UART bootloader cache...");
#if SOC_EFUSE_DIS_DOWNLOAD_MSPI
    esp_efuse_write_field_bit(ESP_EFUSE_SPI_DOWNLOAD_MSPI_DIS);
#endif
#if SOC_EFUSE_DIS_DOWNLOAD_DCACHE
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_DOWNLOAD_DCACHE);
#endif
#if SOC_EFUSE_DIS_DOWNLOAD_ICACHE
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_DOWNLOAD_ICACHE);
#endif
#else
    ESP_LOGW(TAG, "Not disabling UART bootloader cache - SECURITY COMPROMISED");
#endif
#endif // SOC_EFUSE_DIS_DOWNLOAD_*

#ifndef CONFIG_SECURE_BOOT_ALLOW_JTAG
    ESP_LOGI(TAG, "Disable JTAG...");
#if SOC_EFUSE_HARD_DIS_JTAG
    esp_efuse_write_field_bit(ESP_EFUSE_HARD_DIS_JTAG);
#endif
#if SOC_EFUSE_DIS_PAD_JTAG
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_PAD_JTAG);
#endif
#if SOC_EFUSE_DIS_USB_JTAG
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_USB_JTAG);
#endif
#else
    ESP_LOGW(TAG, "Not disabling JTAG - SECURITY COMPROMISED");
#endif

#if SOC_EFUSE_DIS_BOOT_REMAP
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_BOOT_REMAP);
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_LEGACY_SPI_BOOT);
#else
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_DIRECT_BOOT);
#endif

#if SOC_FLASH_ENCRYPTION_XTS_AES_SUPPORT_PSEUDO_ROUND
#if CONFIG_SECURE_FLASH_PSEUDO_ROUND_FUNC
    if (spi_flash_encrypt_ll_is_pseudo_rounds_function_supported()) {
        ESP_LOGI(TAG, "Enable XTS-AES pseudo rounds function...");
        uint8_t xts_pseudo_level = CONFIG_SECURE_FLASH_PSEUDO_ROUND_FUNC_STRENGTH;
        esp_efuse_write_field_blob(ESP_EFUSE_XTS_DPA_PSEUDO_LEVEL, &xts_pseudo_level, ESP_EFUSE_XTS_DPA_PSEUDO_LEVEL[0]->bit_count);
    }
#endif
#endif

    // ESP32-C2 uses a single shared key block and write-protects RD_DIS in its
    // secure-boot path instead, so it is intentionally skipped here.
#if defined(CONFIG_SECURE_BOOT_V2_ENABLED) && !defined(CONFIG_SECURE_BOOT_V2_ALLOW_EFUSE_RD_DIS) && !defined(CONFIG_SECURE_BOOT_FLASH_ENC_KEYS_BURN_TOGETHER)
    // This bit is set when enabling Secure Boot V2, but we can't enable it until this later point in the first boot
    // otherwise the Flash Encryption key cannot be read protected
    esp_efuse_write_field_bit(ESP_EFUSE_WR_DIS_RD_DIS);
#endif

#if SOC_EFUSE_DIS_ICACHE
#ifndef CONFIG_SECURE_FLASH_SKIP_WRITE_PROTECTION_CACHE
    // Set write-protection for DIS_ICACHE (and DIS_DCACHE on targets that have it) to
    // prevent bricking chip in case it will be set accidentally. App and bootloader use
    // cache, so disabling it makes the chip useless for IDF. On every target below the
    // write-protection bit is 2, so burning it also write-protects every other eFuse
    // sharing that bit. Lists derived from components/efuse/<target>/esp_efuse_table.csv:
    //
    // esp32c3:  DIS_ICACHE, DIS_USB_JTAG, DIS_DOWNLOAD_ICACHE, DIS_USB_SERIAL_JTAG,
    //           DIS_FORCE_DOWNLOAD, DIS_TWAI, JTAG_SEL_ENABLE, DIS_PAD_JTAG,
    //           DIS_DOWNLOAD_MANUAL_ENCRYPT.
    // esp32c5:  DIS_ICACHE, DIS_USB_JTAG, DIS_FORCE_DOWNLOAD, SPI_DOWNLOAD_MSPI_DIS,
    //           DIS_TWAI, JTAG_SEL_ENABLE, DIS_PAD_JTAG, DIS_DOWNLOAD_MANUAL_ENCRYPT,
    //           HYS_EN_PAD.
    // esp32c6:  SWAP_UART_SDIO_EN, DIS_ICACHE, DIS_USB_JTAG, DIS_DOWNLOAD_ICACHE,
    //           DIS_USB_SERIAL_JTAG, DIS_FORCE_DOWNLOAD, DIS_TWAI, JTAG_SEL_ENABLE,
    //           DIS_PAD_JTAG, DIS_DOWNLOAD_MANUAL_ENCRYPT.
    // esp32c61: DIS_ICACHE, DIS_USB_JTAG, DIS_FORCE_DOWNLOAD, SPI_DOWNLOAD_MSPI_DIS,
    //           JTAG_SEL_ENABLE, DIS_PAD_JTAG, DIS_DOWNLOAD_MANUAL_ENCRYPT, HYS_EN_PAD,
    //           DIS_WIFI6.
    // esp32h2:  DIS_ICACHE, DIS_USB_JTAG, POWERGLITCH_EN, DIS_FORCE_DOWNLOAD,
    //           SPI_DOWNLOAD_MSPI_DIS, DIS_TWAI, JTAG_SEL_ENABLE, DIS_PAD_JTAG,
    //           DIS_DOWNLOAD_MANUAL_ENCRYPT, POWERGLITCH_EN1.
    // esp32h21: PVT_GLITCH_EN, DIS_ICACHE, DIS_USB_JTAG, POWERGLITCH_EN,
    //           DIS_FORCE_DOWNLOAD, SPI_DOWNLOAD_MSPI_DIS, DIS_TWAI, JTAG_SEL_ENABLE,
    //           DIS_PAD_JTAG, DIS_DOWNLOAD_MANUAL_ENCRYPT, POWERGLITCH_EN1.
    // esp32s2:  DIS_ICACHE, DIS_DCACHE, DIS_DOWNLOAD_ICACHE, DIS_DOWNLOAD_DCACHE,
    //           DIS_FORCE_DOWNLOAD, DIS_USB, DIS_TWAI, DIS_BOOT_REMAP, SOFT_DIS_JTAG,
    //           HARD_DIS_JTAG, DIS_DOWNLOAD_MANUAL_ENCRYPT.
    // esp32s3:  DIS_ICACHE, DIS_DCACHE, DIS_DOWNLOAD_ICACHE, DIS_DOWNLOAD_DCACHE,
    //           DIS_FORCE_DOWNLOAD, DIS_USB_OTG, DIS_TWAI, DIS_APP_CPU, DIS_PAD_JTAG,
    //           DIS_DOWNLOAD_MANUAL_ENCRYPT, DIS_USB_JTAG, DIS_USB_SERIAL_JTAG,
    //           STRAP_JTAG_SEL, USB_PHY_SEL.
    esp_efuse_write_field_bit(ESP_EFUSE_WR_DIS_DIS_ICACHE);
#endif
#endif // SOC_EFUSE_DIS_ICACHE

    return ESP_OK;
}

#if SOC_KEY_MANAGER_FE_KEY_DEPLOY
esp_err_t esp_flash_encryption_use_efuse_key(void)
{
    esp_crypto_key_mgr_enable_periph_clk(true);

    // Force Key Manager to use eFuse key for XTS-AES operation
    key_mgr_hal_set_key_usage(ESP_KEY_MGR_FLASH_XTS_AES_KEY, ESP_KEY_MGR_USE_EFUSE_KEY);

    return ESP_OK;
}
#endif // SOC_KEY_MANAGER_FE_KEY_DEPLOY
