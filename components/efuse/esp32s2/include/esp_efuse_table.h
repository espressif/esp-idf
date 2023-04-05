/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_efuse.h"

// md5_digest_table 42c79ddff54c8f03645a832a69f60af2
// This file was generated from the file esp_efuse_table.csv. DO NOT CHANGE THIS FILE MANUALLY.
// If you want to change some fields, you need to change esp_efuse_table.csv file
// then run `efuse_common_table` or `efuse_custom_table` command it will generate this file.
// To show efuse_table run the command 'show_efuse_table'.


extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RD_DIS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_ICACHE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DCACHE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DOWNLOAD_ICACHE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DOWNLOAD_DCACHE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_FORCE_DOWNLOAD[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_USB[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_TWAI[];
#define ESP_EFUSE_WR_DIS_DIS_CAN ESP_EFUSE_WR_DIS_DIS_TWAI
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_BOOT_REMAP[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SOFT_DIS_JTAG[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_HARD_DIS_JTAG[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DOWNLOAD_MANUAL_ENCRYPT[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_VDD_SPI_XPD[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_VDD_SPI_TIEH[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_VDD_SPI_FORCE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_WDT_DELAY_SEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_BOOT_CRYPT_CNT[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE0[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE1[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE2[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_PURPOSE_0[];
#define ESP_EFUSE_WR_DIS_KEY0_PURPOSE ESP_EFUSE_WR_DIS_KEY_PURPOSE_0
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_PURPOSE_1[];
#define ESP_EFUSE_WR_DIS_KEY1_PURPOSE ESP_EFUSE_WR_DIS_KEY_PURPOSE_1
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_PURPOSE_2[];
#define ESP_EFUSE_WR_DIS_KEY2_PURPOSE ESP_EFUSE_WR_DIS_KEY_PURPOSE_2
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_PURPOSE_3[];
#define ESP_EFUSE_WR_DIS_KEY3_PURPOSE ESP_EFUSE_WR_DIS_KEY_PURPOSE_3
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_PURPOSE_4[];
#define ESP_EFUSE_WR_DIS_KEY4_PURPOSE ESP_EFUSE_WR_DIS_KEY_PURPOSE_4
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_PURPOSE_5[];
#define ESP_EFUSE_WR_DIS_KEY5_PURPOSE ESP_EFUSE_WR_DIS_KEY_PURPOSE_5
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_EN[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_AGGRESSIVE_REVOKE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_TPUW[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DOWNLOAD_MODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_LEGACY_SPI_BOOT[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_UART_PRINT_CHANNEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_USB_DOWNLOAD_MODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ENABLE_SECURITY_DOWNLOAD[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_UART_PRINT_CONTROL[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PIN_POWER_SELECTION[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_TYPE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FORCE_SEND_RESUME[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK1[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_MAC[];
#define ESP_EFUSE_WR_DIS_MAC_FACTORY ESP_EFUSE_WR_DIS_MAC
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_CLK[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_Q[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_D[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_CS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_HD[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_WP[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_DQS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_D4[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_D5[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_D6[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_D7[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_WAFER_VERSION_MAJOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_WAFER_VERSION_MINOR_HI[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK_VERSION_MAJOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PSRAM_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PKG_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_WAFER_VERSION_MINOR_LO[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SYS_DATA_PART1[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_OPTIONAL_UNIQUE_ID[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC_CALIB[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK_VERSION_MINOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_TEMP_CALIB[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTCCALIB_V1IDX_A10H[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTCCALIB_V1IDX_A11H[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTCCALIB_V1IDX_A12H[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTCCALIB_V1IDX_A13H[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTCCALIB_V1IDX_A20H[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTCCALIB_V1IDX_A21H[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTCCALIB_V1IDX_A22H[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTCCALIB_V1IDX_A23H[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTCCALIB_V1IDX_A10L[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTCCALIB_V1IDX_A11L[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTCCALIB_V1IDX_A12L[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTCCALIB_V1IDX_A13L[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTCCALIB_V1IDX_A20L[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTCCALIB_V1IDX_A21L[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTCCALIB_V1IDX_A22L[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTCCALIB_V1IDX_A23L[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_USR_DATA[];
#define ESP_EFUSE_WR_DIS_USER_DATA ESP_EFUSE_WR_DIS_BLOCK_USR_DATA
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_CUSTOM_MAC[];
#define ESP_EFUSE_WR_DIS_MAC_CUSTOM ESP_EFUSE_WR_DIS_CUSTOM_MAC
#define ESP_EFUSE_WR_DIS_USER_DATA_MAC_CUSTOM ESP_EFUSE_WR_DIS_CUSTOM_MAC
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY0[];
#define ESP_EFUSE_WR_DIS_KEY0 ESP_EFUSE_WR_DIS_BLOCK_KEY0
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY1[];
#define ESP_EFUSE_WR_DIS_KEY1 ESP_EFUSE_WR_DIS_BLOCK_KEY1
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY2[];
#define ESP_EFUSE_WR_DIS_KEY2 ESP_EFUSE_WR_DIS_BLOCK_KEY2
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY3[];
#define ESP_EFUSE_WR_DIS_KEY3 ESP_EFUSE_WR_DIS_BLOCK_KEY3
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY4[];
#define ESP_EFUSE_WR_DIS_KEY4 ESP_EFUSE_WR_DIS_BLOCK_KEY4
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY5[];
#define ESP_EFUSE_WR_DIS_KEY5 ESP_EFUSE_WR_DIS_BLOCK_KEY5
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_SYS_DATA2[];
#define ESP_EFUSE_WR_DIS_SYS_DATA_PART2 ESP_EFUSE_WR_DIS_BLOCK_SYS_DATA2
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_USB_EXCHG_PINS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_USB_EXT_PHY_ENABLE[];
#define ESP_EFUSE_WR_DIS_EXT_PHY_ENABLE ESP_EFUSE_WR_DIS_USB_EXT_PHY_ENABLE
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_USB_FORCE_NOPERSIST[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK0_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_KEY0[];
#define ESP_EFUSE_RD_DIS_KEY0 ESP_EFUSE_RD_DIS_BLOCK_KEY0
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_KEY1[];
#define ESP_EFUSE_RD_DIS_KEY1 ESP_EFUSE_RD_DIS_BLOCK_KEY1
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_KEY2[];
#define ESP_EFUSE_RD_DIS_KEY2 ESP_EFUSE_RD_DIS_BLOCK_KEY2
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_KEY3[];
#define ESP_EFUSE_RD_DIS_KEY3 ESP_EFUSE_RD_DIS_BLOCK_KEY3
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_KEY4[];
#define ESP_EFUSE_RD_DIS_KEY4 ESP_EFUSE_RD_DIS_BLOCK_KEY4
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_KEY5[];
#define ESP_EFUSE_RD_DIS_KEY5 ESP_EFUSE_RD_DIS_BLOCK_KEY5
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_SYS_DATA2[];
#define ESP_EFUSE_RD_DIS_SYS_DATA_PART2 ESP_EFUSE_RD_DIS_BLOCK_SYS_DATA2
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_ICACHE[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_DCACHE[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_ICACHE[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_DCACHE[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_FORCE_DOWNLOAD[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_USB[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_TWAI[];
#define ESP_EFUSE_DIS_CAN ESP_EFUSE_DIS_TWAI
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_BOOT_REMAP[];
extern const esp_efuse_desc_t* ESP_EFUSE_SOFT_DIS_JTAG[];
extern const esp_efuse_desc_t* ESP_EFUSE_HARD_DIS_JTAG[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT[];
extern const esp_efuse_desc_t* ESP_EFUSE_USB_EXCHG_PINS[];
extern const esp_efuse_desc_t* ESP_EFUSE_USB_EXT_PHY_ENABLE[];
#define ESP_EFUSE_EXT_PHY_ENABLE ESP_EFUSE_USB_EXT_PHY_ENABLE
extern const esp_efuse_desc_t* ESP_EFUSE_USB_FORCE_NOPERSIST[];
extern const esp_efuse_desc_t* ESP_EFUSE_BLOCK0_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_VDD_SPI_XPD[];
extern const esp_efuse_desc_t* ESP_EFUSE_VDD_SPI_TIEH[];
extern const esp_efuse_desc_t* ESP_EFUSE_VDD_SPI_FORCE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WDT_DELAY_SEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_BOOT_CRYPT_CNT[];
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_KEY_REVOKE0[];
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_KEY_REVOKE1[];
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_KEY_REVOKE2[];
extern const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_0[];
#define ESP_EFUSE_KEY0_PURPOSE ESP_EFUSE_KEY_PURPOSE_0
extern const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_1[];
#define ESP_EFUSE_KEY1_PURPOSE ESP_EFUSE_KEY_PURPOSE_1
extern const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_2[];
#define ESP_EFUSE_KEY2_PURPOSE ESP_EFUSE_KEY_PURPOSE_2
extern const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_3[];
#define ESP_EFUSE_KEY3_PURPOSE ESP_EFUSE_KEY_PURPOSE_3
extern const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_4[];
#define ESP_EFUSE_KEY4_PURPOSE ESP_EFUSE_KEY_PURPOSE_4
extern const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_5[];
#define ESP_EFUSE_KEY5_PURPOSE ESP_EFUSE_KEY_PURPOSE_5
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_EN[];
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE[];
extern const esp_efuse_desc_t* ESP_EFUSE_FLASH_TPUW[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_LEGACY_SPI_BOOT[];
extern const esp_efuse_desc_t* ESP_EFUSE_UART_PRINT_CHANNEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_DOWNLOAD_MODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_ENABLE_SECURITY_DOWNLOAD[];
extern const esp_efuse_desc_t* ESP_EFUSE_UART_PRINT_CONTROL[];
extern const esp_efuse_desc_t* ESP_EFUSE_PIN_POWER_SELECTION[];
extern const esp_efuse_desc_t* ESP_EFUSE_FLASH_TYPE[];
extern const esp_efuse_desc_t* ESP_EFUSE_FORCE_SEND_RESUME[];
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_DISABLE_WAFER_VERSION_MAJOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_DISABLE_BLK_VERSION_MAJOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_MAC[];
#define ESP_EFUSE_MAC_FACTORY ESP_EFUSE_MAC
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_CLK[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_Q[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_D[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_CS[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_HD[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_WP[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_DQS[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_D4[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_D5[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_D6[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_D7[];
extern const esp_efuse_desc_t* ESP_EFUSE_WAFER_VERSION_MAJOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_WAFER_VERSION_MINOR_HI[];
extern const esp_efuse_desc_t* ESP_EFUSE_FLASH_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_BLK_VERSION_MAJOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_PSRAM_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_PKG_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_WAFER_VERSION_MINOR_LO[];
extern const esp_efuse_desc_t* ESP_EFUSE_OPTIONAL_UNIQUE_ID[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC_CALIB[];
extern const esp_efuse_desc_t* ESP_EFUSE_BLK_VERSION_MINOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_TEMP_CALIB[];
extern const esp_efuse_desc_t* ESP_EFUSE_RTCCALIB_V1IDX_A10H[];
extern const esp_efuse_desc_t* ESP_EFUSE_RTCCALIB_V1IDX_A11H[];
extern const esp_efuse_desc_t* ESP_EFUSE_RTCCALIB_V1IDX_A12H[];
extern const esp_efuse_desc_t* ESP_EFUSE_RTCCALIB_V1IDX_A13H[];
extern const esp_efuse_desc_t* ESP_EFUSE_RTCCALIB_V1IDX_A20H[];
extern const esp_efuse_desc_t* ESP_EFUSE_RTCCALIB_V1IDX_A21H[];
extern const esp_efuse_desc_t* ESP_EFUSE_RTCCALIB_V1IDX_A22H[];
extern const esp_efuse_desc_t* ESP_EFUSE_RTCCALIB_V1IDX_A23H[];
extern const esp_efuse_desc_t* ESP_EFUSE_RTCCALIB_V1IDX_A10L[];
extern const esp_efuse_desc_t* ESP_EFUSE_RTCCALIB_V1IDX_A11L[];
extern const esp_efuse_desc_t* ESP_EFUSE_RTCCALIB_V1IDX_A12L[];
extern const esp_efuse_desc_t* ESP_EFUSE_RTCCALIB_V1IDX_A13L[];
extern const esp_efuse_desc_t* ESP_EFUSE_RTCCALIB_V1IDX_A20L[];
extern const esp_efuse_desc_t* ESP_EFUSE_RTCCALIB_V1IDX_A21L[];
extern const esp_efuse_desc_t* ESP_EFUSE_RTCCALIB_V1IDX_A22L[];
extern const esp_efuse_desc_t* ESP_EFUSE_RTCCALIB_V1IDX_A23L[];
extern const esp_efuse_desc_t* ESP_EFUSE_USER_DATA[];
#define ESP_EFUSE_BLOCK_USR_DATA ESP_EFUSE_USER_DATA
extern const esp_efuse_desc_t* ESP_EFUSE_USER_DATA_MAC_CUSTOM[];
#define ESP_EFUSE_MAC_CUSTOM ESP_EFUSE_USER_DATA_MAC_CUSTOM
#define ESP_EFUSE_CUSTOM_MAC ESP_EFUSE_USER_DATA_MAC_CUSTOM
extern const esp_efuse_desc_t* ESP_EFUSE_KEY0[];
#define ESP_EFUSE_BLOCK_KEY0 ESP_EFUSE_KEY0
extern const esp_efuse_desc_t* ESP_EFUSE_KEY1[];
#define ESP_EFUSE_BLOCK_KEY1 ESP_EFUSE_KEY1
extern const esp_efuse_desc_t* ESP_EFUSE_KEY2[];
#define ESP_EFUSE_BLOCK_KEY2 ESP_EFUSE_KEY2
extern const esp_efuse_desc_t* ESP_EFUSE_KEY3[];
#define ESP_EFUSE_BLOCK_KEY3 ESP_EFUSE_KEY3
extern const esp_efuse_desc_t* ESP_EFUSE_KEY4[];
#define ESP_EFUSE_BLOCK_KEY4 ESP_EFUSE_KEY4
extern const esp_efuse_desc_t* ESP_EFUSE_KEY5[];
#define ESP_EFUSE_BLOCK_KEY5 ESP_EFUSE_KEY5
extern const esp_efuse_desc_t* ESP_EFUSE_SYS_DATA_PART2[];
#define ESP_EFUSE_BLOCK_SYS_DATA2 ESP_EFUSE_SYS_DATA_PART2

#ifdef __cplusplus
}
#endif
