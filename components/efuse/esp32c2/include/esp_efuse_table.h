/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_efuse.h"

// md5_digest_table 439495cbc35dc68d7566e05ac3dbb248
// This file was generated from the file esp_efuse_table.csv. DO NOT CHANGE THIS FILE MANUALLY.
// If you want to change some fields, you need to change esp_efuse_table.csv file
// then run `efuse_common_table` or `efuse_custom_table` command it will generate this file.
// To show efuse_table run the command 'show_efuse_table'.


extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RD_DIS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_WDT_DELAY_SEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_PAD_JTAG[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DOWNLOAD_ICACHE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DOWNLOAD_MANUAL_ENCRYPT[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_BOOT_CRYPT_CNT[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_XTS_KEY_LENGTH_256[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_EN[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_UART_PRINT_CONTROL[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FORCE_SEND_RESUME[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DOWNLOAD_MODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DIRECT_BOOT[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ENABLE_SECURITY_DOWNLOAD[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_TPUW[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_CUSTOM_MAC_USED[];
#define ESP_EFUSE_WR_DIS_ENABLE_CUSTOM_MAC ESP_EFUSE_WR_DIS_CUSTOM_MAC_USED
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DISABLE_WAFER_VERSION_MAJOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DISABLE_BLK_VERSION_MAJOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_CUSTOM_MAC[];
#define ESP_EFUSE_WR_DIS_MAC_CUSTOM ESP_EFUSE_WR_DIS_CUSTOM_MAC
#define ESP_EFUSE_WR_DIS_USER_DATA_MAC_CUSTOM ESP_EFUSE_WR_DIS_CUSTOM_MAC
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_MAC[];
#define ESP_EFUSE_WR_DIS_MAC_FACTORY ESP_EFUSE_WR_DIS_MAC
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_WAFER_VERSION_MINOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_WAFER_VERSION_MAJOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PKG_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK_VERSION_MINOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK_VERSION_MAJOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_OCODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_TEMP_CALIB[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_INIT_CODE_ATTEN0[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_INIT_CODE_ATTEN3[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_CAL_VOL_ATTEN0[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_CAL_VOL_ATTEN3[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIG_DBIAS_HVT[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIG_LDO_SLP_DBIAS2[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIG_LDO_SLP_DBIAS26[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIG_LDO_ACT_DBIAS26[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIG_LDO_ACT_STEPD10[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTC_LDO_SLP_DBIAS13[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTC_LDO_SLP_DBIAS29[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTC_LDO_SLP_DBIAS31[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTC_LDO_ACT_DBIAS31[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTC_LDO_ACT_DBIAS13[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC_CALIBRATION_3[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY0[];
#define ESP_EFUSE_WR_DIS_KEY0 ESP_EFUSE_WR_DIS_BLOCK_KEY0
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY0[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY0_LOW[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY0_HI[];
extern const esp_efuse_desc_t* ESP_EFUSE_WDT_DELAY_SEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_PAD_JTAG[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_ICACHE[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_BOOT_CRYPT_CNT[];
extern const esp_efuse_desc_t* ESP_EFUSE_XTS_KEY_LENGTH_256[];
extern const esp_efuse_desc_t* ESP_EFUSE_UART_PRINT_CONTROL[];
extern const esp_efuse_desc_t* ESP_EFUSE_FORCE_SEND_RESUME[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_DIRECT_BOOT[];
extern const esp_efuse_desc_t* ESP_EFUSE_ENABLE_SECURITY_DOWNLOAD[];
extern const esp_efuse_desc_t* ESP_EFUSE_FLASH_TPUW[];
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_EN[];
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_CUSTOM_MAC_USED[];
#define ESP_EFUSE_ENABLE_CUSTOM_MAC ESP_EFUSE_CUSTOM_MAC_USED
extern const esp_efuse_desc_t* ESP_EFUSE_DISABLE_WAFER_VERSION_MAJOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_DISABLE_BLK_VERSION_MAJOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_USER_DATA[];
extern const esp_efuse_desc_t* ESP_EFUSE_USER_DATA_MAC_CUSTOM[];
#define ESP_EFUSE_MAC_CUSTOM ESP_EFUSE_USER_DATA_MAC_CUSTOM
#define ESP_EFUSE_CUSTOM_MAC ESP_EFUSE_USER_DATA_MAC_CUSTOM
extern const esp_efuse_desc_t* ESP_EFUSE_MAC[];
#define ESP_EFUSE_MAC_FACTORY ESP_EFUSE_MAC
extern const esp_efuse_desc_t* ESP_EFUSE_WAFER_VERSION_MINOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_WAFER_VERSION_MAJOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_PKG_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_BLK_VERSION_MINOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_BLK_VERSION_MAJOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_OCODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_TEMP_CALIB[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_INIT_CODE_ATTEN0[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_INIT_CODE_ATTEN3[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_CAL_VOL_ATTEN0[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_CAL_VOL_ATTEN3[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIG_DBIAS_HVT[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIG_LDO_SLP_DBIAS2[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIG_LDO_SLP_DBIAS26[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIG_LDO_ACT_DBIAS26[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIG_LDO_ACT_STEPD10[];
extern const esp_efuse_desc_t* ESP_EFUSE_RTC_LDO_SLP_DBIAS13[];
extern const esp_efuse_desc_t* ESP_EFUSE_RTC_LDO_SLP_DBIAS29[];
extern const esp_efuse_desc_t* ESP_EFUSE_RTC_LDO_SLP_DBIAS31[];
extern const esp_efuse_desc_t* ESP_EFUSE_RTC_LDO_ACT_DBIAS31[];
extern const esp_efuse_desc_t* ESP_EFUSE_RTC_LDO_ACT_DBIAS13[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC_CALIBRATION_3[];
extern const esp_efuse_desc_t* ESP_EFUSE_KEY0[];
#define ESP_EFUSE_BLOCK_KEY0 ESP_EFUSE_KEY0
extern const esp_efuse_desc_t* ESP_EFUSE_KEY0_FE_256BIT[];
extern const esp_efuse_desc_t* ESP_EFUSE_KEY0_FE_128BIT[];
extern const esp_efuse_desc_t* ESP_EFUSE_KEY0_SB_128BIT[];

#ifdef __cplusplus
}
#endif
