/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_efuse.h"

// md5_digest_table 2e197b7b14eec62fa5bdf94c6d71e87a
// This file was generated from the file esp_efuse_table.csv. DO NOT CHANGE THIS FILE MANUALLY.
// If you want to change some fields, you need to change esp_efuse_table.csv file
// then run `efuse_common_table` or `efuse_custom_table` command it will generate this file.
// To show efuse_table run the command 'show_efuse_table'.


extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RD_DIS[];
#define ESP_EFUSE_WR_DIS_EFUSE_RD_DISABLE ESP_EFUSE_WR_DIS_RD_DIS
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_WR_DIS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_CRYPT_CNT[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_UART_DOWNLOAD_DIS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_MAC[];
#define ESP_EFUSE_WR_DIS_MAC_FACTORY ESP_EFUSE_WR_DIS_MAC
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_MAC_CRC[];
#define ESP_EFUSE_WR_DIS_MAC_FACTORY_CRC ESP_EFUSE_WR_DIS_MAC_CRC
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DISABLE_APP_CPU[];
#define ESP_EFUSE_WR_DIS_CHIP_VER_DIS_APP_CPU ESP_EFUSE_WR_DIS_DISABLE_APP_CPU
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DISABLE_BT[];
#define ESP_EFUSE_WR_DIS_CHIP_VER_DIS_BT ESP_EFUSE_WR_DIS_DISABLE_BT
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_CACHE[];
#define ESP_EFUSE_WR_DIS_CHIP_VER_DIS_CACHE ESP_EFUSE_WR_DIS_DIS_CACHE
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_VOL_LEVEL_HP_INV[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_CLK8M_FREQ[];
#define ESP_EFUSE_WR_DIS_CK8M_FREQ ESP_EFUSE_WR_DIS_CLK8M_FREQ
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC_VREF[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_XPD_SDIO_REG[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_XPD_SDIO_TIEH[];
#define ESP_EFUSE_WR_DIS_SDIO_TIEH ESP_EFUSE_WR_DIS_XPD_SDIO_TIEH
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_XPD_SDIO_FORCE[];
#define ESP_EFUSE_WR_DIS_SDIO_FORCE ESP_EFUSE_WR_DIS_XPD_SDIO_FORCE
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_CLK[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_Q[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_D[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_CS0[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK1[];
#define ESP_EFUSE_WR_DIS_ENCRYPT_FLASH_KEY ESP_EFUSE_WR_DIS_BLOCK1
#define ESP_EFUSE_WR_DIS_BLK1 ESP_EFUSE_WR_DIS_BLOCK1
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK2[];
#define ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY ESP_EFUSE_WR_DIS_BLOCK2
#define ESP_EFUSE_WR_DIS_BLK2 ESP_EFUSE_WR_DIS_BLOCK2
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK3[];
#define ESP_EFUSE_WR_DIS_BLK3 ESP_EFUSE_WR_DIS_BLOCK3
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_CUSTOM_MAC_CRC[];
#define ESP_EFUSE_WR_DIS_MAC_CUSTOM_CRC ESP_EFUSE_WR_DIS_CUSTOM_MAC_CRC
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_CUSTOM_MAC[];
#define ESP_EFUSE_WR_DIS_MAC_CUSTOM ESP_EFUSE_WR_DIS_CUSTOM_MAC
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_TP_LOW[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_TP_HIGH[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_TP_LOW[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_TP_HIGH[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_MAC_VERSION[];
#define ESP_EFUSE_WR_DIS_MAC_CUSTOM_VER ESP_EFUSE_WR_DIS_MAC_VERSION
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK3_PART_RESERVE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_CRYPT_CONFIG[];
#define ESP_EFUSE_WR_DIS_ENCRYPT_CONFIG ESP_EFUSE_WR_DIS_FLASH_CRYPT_CONFIG
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_CODING_SCHEME[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_STATUS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ABS_DONE_0[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ABS_DONE_1[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_JTAG_DISABLE[];
#define ESP_EFUSE_WR_DIS_DISABLE_JTAG ESP_EFUSE_WR_DIS_JTAG_DISABLE
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_CONSOLE_DEBUG_DISABLE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DISABLE_DL_ENCRYPT[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DISABLE_DL_DECRYPT[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DISABLE_DL_CACHE[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK1[];
#define ESP_EFUSE_RD_DIS_ENCRYPT_FLASH_KEY ESP_EFUSE_RD_DIS_BLOCK1
#define ESP_EFUSE_RD_DIS_BLK1 ESP_EFUSE_RD_DIS_BLOCK1
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK2[];
#define ESP_EFUSE_RD_DIS_SECURE_BOOT_KEY ESP_EFUSE_RD_DIS_BLOCK2
#define ESP_EFUSE_RD_DIS_BLK2 ESP_EFUSE_RD_DIS_BLOCK2
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK3[];
#define ESP_EFUSE_RD_DIS_BLK3 ESP_EFUSE_RD_DIS_BLOCK3
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_CUSTOM_MAC_CRC[];
#define ESP_EFUSE_RD_DIS_MAC_CUSTOM_CRC ESP_EFUSE_RD_DIS_CUSTOM_MAC_CRC
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_CUSTOM_MAC[];
#define ESP_EFUSE_RD_DIS_MAC_CUSTOM ESP_EFUSE_RD_DIS_CUSTOM_MAC
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC1_TP_LOW[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC1_TP_HIGH[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC2_TP_LOW[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC2_TP_HIGH[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_SECURE_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_MAC_VERSION[];
#define ESP_EFUSE_RD_DIS_MAC_CUSTOM_VER ESP_EFUSE_RD_DIS_MAC_VERSION
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLK3_PART_RESERVE[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_FLASH_CRYPT_CONFIG[];
#define ESP_EFUSE_RD_DIS_ENCRYPT_CONFIG ESP_EFUSE_RD_DIS_FLASH_CRYPT_CONFIG
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_CODING_SCHEME[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY_STATUS[];
extern const esp_efuse_desc_t* ESP_EFUSE_FLASH_CRYPT_CNT[];
extern const esp_efuse_desc_t* ESP_EFUSE_UART_DOWNLOAD_DIS[];
extern const esp_efuse_desc_t* ESP_EFUSE_MAC[];
#define ESP_EFUSE_MAC_FACTORY ESP_EFUSE_MAC
extern const esp_efuse_desc_t* ESP_EFUSE_MAC_CRC[];
#define ESP_EFUSE_MAC_FACTORY_CRC ESP_EFUSE_MAC_CRC
extern const esp_efuse_desc_t* ESP_EFUSE_DISABLE_APP_CPU[];
#define ESP_EFUSE_CHIP_VER_DIS_APP_CPU ESP_EFUSE_DISABLE_APP_CPU
extern const esp_efuse_desc_t* ESP_EFUSE_DISABLE_BT[];
#define ESP_EFUSE_CHIP_VER_DIS_BT ESP_EFUSE_DISABLE_BT
extern const esp_efuse_desc_t* ESP_EFUSE_CHIP_PACKAGE_4BIT[];
#define ESP_EFUSE_CHIP_VER_PKG_4BIT ESP_EFUSE_CHIP_PACKAGE_4BIT
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_CACHE[];
#define ESP_EFUSE_CHIP_VER_DIS_CACHE ESP_EFUSE_DIS_CACHE
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_HD[];
extern const esp_efuse_desc_t* ESP_EFUSE_CHIP_PACKAGE[];
#define ESP_EFUSE_CHIP_VER_PKG ESP_EFUSE_CHIP_PACKAGE
extern const esp_efuse_desc_t* ESP_EFUSE_CHIP_CPU_FREQ_LOW[];
extern const esp_efuse_desc_t* ESP_EFUSE_CHIP_CPU_FREQ_RATED[];
extern const esp_efuse_desc_t* ESP_EFUSE_BLK3_PART_RESERVE[];
extern const esp_efuse_desc_t* ESP_EFUSE_CHIP_VER_REV1[];
extern const esp_efuse_desc_t* ESP_EFUSE_CLK8M_FREQ[];
#define ESP_EFUSE_CK8M_FREQ ESP_EFUSE_CLK8M_FREQ
extern const esp_efuse_desc_t* ESP_EFUSE_ADC_VREF[];
extern const esp_efuse_desc_t* ESP_EFUSE_XPD_SDIO_REG[];
extern const esp_efuse_desc_t* ESP_EFUSE_XPD_SDIO_TIEH[];
#define ESP_EFUSE_SDIO_TIEH ESP_EFUSE_XPD_SDIO_TIEH
extern const esp_efuse_desc_t* ESP_EFUSE_XPD_SDIO_FORCE[];
#define ESP_EFUSE_SDIO_FORCE ESP_EFUSE_XPD_SDIO_FORCE
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_CLK[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_Q[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_D[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_CS0[];
extern const esp_efuse_desc_t* ESP_EFUSE_CHIP_VER_REV2[];
extern const esp_efuse_desc_t* ESP_EFUSE_VOL_LEVEL_HP_INV[];
extern const esp_efuse_desc_t* ESP_EFUSE_WAFER_VERSION_MINOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_FLASH_CRYPT_CONFIG[];
#define ESP_EFUSE_ENCRYPT_CONFIG ESP_EFUSE_FLASH_CRYPT_CONFIG
extern const esp_efuse_desc_t* ESP_EFUSE_CODING_SCHEME[];
extern const esp_efuse_desc_t* ESP_EFUSE_CONSOLE_DEBUG_DISABLE[];
extern const esp_efuse_desc_t* ESP_EFUSE_DISABLE_SDIO_HOST[];
extern const esp_efuse_desc_t* ESP_EFUSE_ABS_DONE_0[];
extern const esp_efuse_desc_t* ESP_EFUSE_ABS_DONE_1[];
extern const esp_efuse_desc_t* ESP_EFUSE_JTAG_DISABLE[];
#define ESP_EFUSE_DISABLE_JTAG ESP_EFUSE_JTAG_DISABLE
extern const esp_efuse_desc_t* ESP_EFUSE_DISABLE_DL_ENCRYPT[];
extern const esp_efuse_desc_t* ESP_EFUSE_DISABLE_DL_DECRYPT[];
extern const esp_efuse_desc_t* ESP_EFUSE_DISABLE_DL_CACHE[];
extern const esp_efuse_desc_t* ESP_EFUSE_KEY_STATUS[];
extern const esp_efuse_desc_t* ESP_EFUSE_BLOCK1[];
#define ESP_EFUSE_ENCRYPT_FLASH_KEY ESP_EFUSE_BLOCK1
extern const esp_efuse_desc_t* ESP_EFUSE_BLOCK2[];
#define ESP_EFUSE_SECURE_BOOT_KEY ESP_EFUSE_BLOCK2
extern const esp_efuse_desc_t* ESP_EFUSE_CUSTOM_MAC_CRC[];
#define ESP_EFUSE_MAC_CUSTOM_CRC ESP_EFUSE_CUSTOM_MAC_CRC
extern const esp_efuse_desc_t* ESP_EFUSE_MAC_CUSTOM[];
#define ESP_EFUSE_MAC_CUSTOM ESP_EFUSE_MAC_CUSTOM
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_TP_LOW[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_TP_HIGH[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC2_TP_LOW[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC2_TP_HIGH[];
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_MAC_VERSION[];
#define ESP_EFUSE_MAC_CUSTOM_VER ESP_EFUSE_MAC_VERSION

#ifdef __cplusplus
}
#endif
