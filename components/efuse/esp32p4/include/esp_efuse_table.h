/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_efuse.h"

// md5_digest_table c56ed98dde7a08c8f70d57a01faba96a
// This file was generated from the file esp_efuse_table.csv. DO NOT CHANGE THIS FILE MANUALLY.
// If you want to change some fields, you need to change esp_efuse_table.csv file
// then run `efuse_common_table` or `efuse_custom_table` command it will generate this file.
// To show efuse_table run the command 'show_efuse_table'.


extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RD_DIS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KM_RND_SWITCH_CYCLE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KM_DEPLOY_ONLY_ONCE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FORCE_USE_KEY_MANAGER_KEY[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FORCE_DISABLE_SW_INIT_KEY[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_XTS_KEY_LENGTH_256[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LOCK_KM_KEY[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KM_DISABLE_DEPLOY_MODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_USB_JTAG[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_FORCE_DOWNLOAD[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_DOWNLOAD_MSPI_DIS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_TWAI[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_JTAG_SEL_ENABLE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_PAD_JTAG[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DOWNLOAD_MANUAL_ENCRYPT[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_WDT_DELAY_SEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_HYS_EN_PAD[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PXA0_TIEH_SEL_0[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PXA0_TIEH_SEL_1[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PXA0_TIEH_SEL_2[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PXA0_TIEH_SEL_3[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_WDT[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_SWD[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_HP_PWR_SRC_SEL[];
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
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SEC_DPA_LEVEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_CRYPT_DPA_ENABLE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_EN[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_AGGRESSIVE_REVOKE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ECDSA_ENABLE_SOFT_K[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_TYPE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_PAGE_SIZE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_ECC_EN[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_USB_OTG_DOWNLOAD_MODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_TPUW[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DOWNLOAD_MODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DIRECT_BOOT[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_USB_SERIAL_JTAG_ROM_PRINT[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ENABLE_SECURITY_DOWNLOAD[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_UART_PRINT_CONTROL[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FORCE_SEND_RESUME[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_DISABLE_FAST_WAKE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KM_HUK_GEN_STATE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK1[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_MAC[];
#define ESP_EFUSE_WR_DIS_MAC_FACTORY ESP_EFUSE_WR_DIS_MAC
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_WAFER_VERSION_MINOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_WAFER_VERSION_MAJOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DISABLE_WAFER_VERSION_MAJOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DISABLE_BLK_VERSION_MAJOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK_VERSION_MINOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK_VERSION_MAJOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PSRAM_CAP[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_TEMP[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PSRAM_VENDOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PKG_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SYS_DATA_PART1[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LDO_VO1_DREF[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LDO_VO2_DREF[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LDO_VO1_MUL[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LDO_VO2_MUL[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LDO_VO3_K[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LDO_VO3_VOS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LDO_VO3_C[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LDO_VO4_K[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LDO_VO4_VOS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LDO_VO4_C[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ACTIVE_HP_DBIAS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ACTIVE_LP_DBIAS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LSLP_HP_DBIAS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DSLP_DBG[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DSLP_LP_DBIAS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LP_DCDC_DBIAS_VOL_GAP[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_OPTIONAL_UNIQUE_ID[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_AVE_INITCODE_ATTEN0[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_AVE_INITCODE_ATTEN1[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_AVE_INITCODE_ATTEN2[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_AVE_INITCODE_ATTEN3[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_AVE_INITCODE_ATTEN0[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_AVE_INITCODE_ATTEN1[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_AVE_INITCODE_ATTEN2[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_AVE_INITCODE_ATTEN3[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_HI_DOUT_ATTEN0[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_HI_DOUT_ATTEN1[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_HI_DOUT_ATTEN2[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_HI_DOUT_ATTEN3[];
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
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_HI_DOUT_ATTEN0[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_HI_DOUT_ATTEN1[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_HI_DOUT_ATTEN2[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_HI_DOUT_ATTEN3[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_CH0_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_CH1_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_CH2_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_CH3_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_CH4_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_CH5_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_CH6_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_CH7_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_CH0_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_CH1_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_CH2_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_CH3_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_CH4_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_CH5_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_TEMPERATURE_SENSOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_USB_DEVICE_EXCHG_PINS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_USB_OTG11_EXCHG_PINS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_USB_PHY_SEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SOFT_DIS_JTAG[];
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
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC2_HI_DOUT_ATTEN0[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC2_HI_DOUT_ATTEN1[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC2_HI_DOUT_ATTEN2[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC2_HI_DOUT_ATTEN3[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC1_CH0_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC1_CH1_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC1_CH2_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC1_CH3_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC1_CH4_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC1_CH5_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC1_CH6_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC1_CH7_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC2_CH0_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC2_CH1_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC2_CH2_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC2_CH3_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC2_CH4_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC2_CH5_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_TEMPERATURE_SENSOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_USB_DEVICE_EXCHG_PINS[];
extern const esp_efuse_desc_t* ESP_EFUSE_USB_OTG11_EXCHG_PINS[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_JTAG[];
extern const esp_efuse_desc_t* ESP_EFUSE_POWERGLITCH_EN[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_FORCE_DOWNLOAD[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_DOWNLOAD_MSPI_DIS[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_TWAI[];
extern const esp_efuse_desc_t* ESP_EFUSE_JTAG_SEL_ENABLE[];
extern const esp_efuse_desc_t* ESP_EFUSE_SOFT_DIS_JTAG[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_PAD_JTAG[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT[];
extern const esp_efuse_desc_t* ESP_EFUSE_USB_PHY_SEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_KM_HUK_GEN_STATE[];
extern const esp_efuse_desc_t* ESP_EFUSE_KM_RND_SWITCH_CYCLE[];
extern const esp_efuse_desc_t* ESP_EFUSE_KM_DEPLOY_ONLY_ONCE[];
extern const esp_efuse_desc_t* ESP_EFUSE_FORCE_USE_KEY_MANAGER_KEY[];
extern const esp_efuse_desc_t* ESP_EFUSE_FORCE_DISABLE_SW_INIT_KEY[];
extern const esp_efuse_desc_t* ESP_EFUSE_XTS_KEY_LENGTH_256[];
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
extern const esp_efuse_desc_t* ESP_EFUSE_SEC_DPA_LEVEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_ECDSA_ENABLE_SOFT_K[];
extern const esp_efuse_desc_t* ESP_EFUSE_CRYPT_DPA_ENABLE[];
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_EN[];
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE[];
extern const esp_efuse_desc_t* ESP_EFUSE_FLASH_TYPE[];
extern const esp_efuse_desc_t* ESP_EFUSE_FLASH_PAGE_SIZE[];
extern const esp_efuse_desc_t* ESP_EFUSE_FLASH_ECC_EN[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_OTG_DOWNLOAD_MODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_FLASH_TPUW[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_DIRECT_BOOT[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT[];
extern const esp_efuse_desc_t* ESP_EFUSE_LOCK_KM_KEY[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_ENABLE_SECURITY_DOWNLOAD[];
extern const esp_efuse_desc_t* ESP_EFUSE_UART_PRINT_CONTROL[];
extern const esp_efuse_desc_t* ESP_EFUSE_FORCE_SEND_RESUME[];
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE[];
extern const esp_efuse_desc_t* ESP_EFUSE_HYS_EN_PAD[];
extern const esp_efuse_desc_t* ESP_EFUSE_DCDC_VSET[];
extern const esp_efuse_desc_t* ESP_EFUSE_PXA0_TIEH_SEL_0[];
extern const esp_efuse_desc_t* ESP_EFUSE_PXA0_TIEH_SEL_1[];
extern const esp_efuse_desc_t* ESP_EFUSE_PXA0_TIEH_SEL_2[];
extern const esp_efuse_desc_t* ESP_EFUSE_PXA0_TIEH_SEL_3[];
extern const esp_efuse_desc_t* ESP_EFUSE_KM_DISABLE_DEPLOY_MODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_HP_PWR_SRC_SEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_DCDC_VSET_EN[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_WDT[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_SWD[];
extern const esp_efuse_desc_t* ESP_EFUSE_MAC[];
#define ESP_EFUSE_MAC_FACTORY ESP_EFUSE_MAC
extern const esp_efuse_desc_t* ESP_EFUSE_WAFER_VERSION_MINOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_WAFER_VERSION_MAJOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_DISABLE_WAFER_VERSION_MAJOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_DISABLE_BLK_VERSION_MAJOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_BLK_VERSION_MINOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_BLK_VERSION_MAJOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_PSRAM_CAP[];
extern const esp_efuse_desc_t* ESP_EFUSE_TEMP[];
extern const esp_efuse_desc_t* ESP_EFUSE_PSRAM_VENDOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_PKG_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_LDO_VO1_DREF[];
extern const esp_efuse_desc_t* ESP_EFUSE_LDO_VO2_DREF[];
extern const esp_efuse_desc_t* ESP_EFUSE_LDO_VO1_MUL[];
extern const esp_efuse_desc_t* ESP_EFUSE_LDO_VO2_MUL[];
extern const esp_efuse_desc_t* ESP_EFUSE_LDO_VO3_K[];
extern const esp_efuse_desc_t* ESP_EFUSE_LDO_VO3_VOS[];
extern const esp_efuse_desc_t* ESP_EFUSE_LDO_VO3_C[];
extern const esp_efuse_desc_t* ESP_EFUSE_LDO_VO4_K[];
extern const esp_efuse_desc_t* ESP_EFUSE_LDO_VO4_VOS[];
extern const esp_efuse_desc_t* ESP_EFUSE_LDO_VO4_C[];
extern const esp_efuse_desc_t* ESP_EFUSE_ACTIVE_HP_DBIAS[];
extern const esp_efuse_desc_t* ESP_EFUSE_ACTIVE_LP_DBIAS[];
extern const esp_efuse_desc_t* ESP_EFUSE_LSLP_HP_DBIAS[];
extern const esp_efuse_desc_t* ESP_EFUSE_DSLP_DBG[];
extern const esp_efuse_desc_t* ESP_EFUSE_DSLP_LP_DBIAS[];
extern const esp_efuse_desc_t* ESP_EFUSE_LP_DCDC_DBIAS_VOL_GAP[];
extern const esp_efuse_desc_t* ESP_EFUSE_OPTIONAL_UNIQUE_ID[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_AVE_INITCODE_ATTEN0[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_AVE_INITCODE_ATTEN1[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_AVE_INITCODE_ATTEN2[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_AVE_INITCODE_ATTEN3[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC2_AVE_INITCODE_ATTEN0[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC2_AVE_INITCODE_ATTEN1[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC2_AVE_INITCODE_ATTEN2[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC2_AVE_INITCODE_ATTEN3[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_HI_DOUT_ATTEN0[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_HI_DOUT_ATTEN1[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_HI_DOUT_ATTEN2[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_HI_DOUT_ATTEN3[];
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
extern const esp_efuse_desc_t* ESP_EFUSE_ADC2_HI_DOUT_ATTEN0[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC2_HI_DOUT_ATTEN1[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC2_HI_DOUT_ATTEN2[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC2_HI_DOUT_ATTEN3[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_CH0_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_CH1_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_CH2_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_CH3_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_CH4_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_CH5_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_CH6_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_CH7_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC2_CH0_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC2_CH1_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC2_CH2_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC2_CH3_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC2_CH4_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC2_CH5_ATTEN0_INITCODE_DIFF[];
extern const esp_efuse_desc_t* ESP_EFUSE_TEMPERATURE_SENSOR[];

#ifdef __cplusplus
}
#endif
