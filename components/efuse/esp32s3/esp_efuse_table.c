/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_efuse.h"
#include <assert.h>
#include "esp_efuse_table.h"

// md5_digest_table e0674ff40a1e124670c6eecf33410e76
// This file was generated from the file esp_efuse_table.csv. DO NOT CHANGE THIS FILE MANUALLY.
// If you want to change some fields, you need to change esp_efuse_table.csv file
// then run `efuse_common_table` or `efuse_custom_table` command it will generate this file.
// To show efuse_table run the command 'show_efuse_table'.

static const esp_efuse_desc_t WR_DIS[] = {
    {EFUSE_BLK0, 0, 32}, 	 // [] Disable programming of individual eFuses,
};

static const esp_efuse_desc_t WR_DIS_RD_DIS[] = {
    {EFUSE_BLK0, 0, 1}, 	 // [] wr_dis of RD_DIS,
};

static const esp_efuse_desc_t WR_DIS_DIS_ICACHE[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of DIS_ICACHE,
};

static const esp_efuse_desc_t WR_DIS_DIS_DCACHE[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of DIS_DCACHE,
};

static const esp_efuse_desc_t WR_DIS_DIS_DOWNLOAD_ICACHE[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of DIS_DOWNLOAD_ICACHE,
};

static const esp_efuse_desc_t WR_DIS_DIS_DOWNLOAD_DCACHE[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of DIS_DOWNLOAD_DCACHE,
};

static const esp_efuse_desc_t WR_DIS_DIS_FORCE_DOWNLOAD[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of DIS_FORCE_DOWNLOAD,
};

static const esp_efuse_desc_t WR_DIS_DIS_USB_OTG[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [WR_DIS.DIS_USB] wr_dis of DIS_USB_OTG,
};

static const esp_efuse_desc_t WR_DIS_DIS_TWAI[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [WR_DIS.DIS_CAN] wr_dis of DIS_TWAI,
};

static const esp_efuse_desc_t WR_DIS_DIS_APP_CPU[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of DIS_APP_CPU,
};

static const esp_efuse_desc_t WR_DIS_DIS_PAD_JTAG[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [WR_DIS.HARD_DIS_JTAG] wr_dis of DIS_PAD_JTAG,
};

static const esp_efuse_desc_t WR_DIS_DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of DIS_DOWNLOAD_MANUAL_ENCRYPT,
};

static const esp_efuse_desc_t WR_DIS_DIS_USB_JTAG[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of DIS_USB_JTAG,
};

static const esp_efuse_desc_t WR_DIS_DIS_USB_SERIAL_JTAG[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [WR_DIS.DIS_USB_DEVICE] wr_dis of DIS_USB_SERIAL_JTAG,
};

static const esp_efuse_desc_t WR_DIS_STRAP_JTAG_SEL[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of STRAP_JTAG_SEL,
};

static const esp_efuse_desc_t WR_DIS_USB_PHY_SEL[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of USB_PHY_SEL,
};

static const esp_efuse_desc_t WR_DIS_VDD_SPI_XPD[] = {
    {EFUSE_BLK0, 3, 1}, 	 // [] wr_dis of VDD_SPI_XPD,
};

static const esp_efuse_desc_t WR_DIS_VDD_SPI_TIEH[] = {
    {EFUSE_BLK0, 3, 1}, 	 // [] wr_dis of VDD_SPI_TIEH,
};

static const esp_efuse_desc_t WR_DIS_VDD_SPI_FORCE[] = {
    {EFUSE_BLK0, 3, 1}, 	 // [] wr_dis of VDD_SPI_FORCE,
};

static const esp_efuse_desc_t WR_DIS_WDT_DELAY_SEL[] = {
    {EFUSE_BLK0, 3, 1}, 	 // [] wr_dis of WDT_DELAY_SEL,
};

static const esp_efuse_desc_t WR_DIS_SPI_BOOT_CRYPT_CNT[] = {
    {EFUSE_BLK0, 4, 1}, 	 // [] wr_dis of SPI_BOOT_CRYPT_CNT,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_KEY_REVOKE0[] = {
    {EFUSE_BLK0, 5, 1}, 	 // [] wr_dis of SECURE_BOOT_KEY_REVOKE0,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_KEY_REVOKE1[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of SECURE_BOOT_KEY_REVOKE1,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_KEY_REVOKE2[] = {
    {EFUSE_BLK0, 7, 1}, 	 // [] wr_dis of SECURE_BOOT_KEY_REVOKE2,
};

static const esp_efuse_desc_t WR_DIS_KEY_PURPOSE_0[] = {
    {EFUSE_BLK0, 8, 1}, 	 // [WR_DIS.KEY0_PURPOSE] wr_dis of KEY_PURPOSE_0,
};

static const esp_efuse_desc_t WR_DIS_KEY_PURPOSE_1[] = {
    {EFUSE_BLK0, 9, 1}, 	 // [WR_DIS.KEY1_PURPOSE] wr_dis of KEY_PURPOSE_1,
};

static const esp_efuse_desc_t WR_DIS_KEY_PURPOSE_2[] = {
    {EFUSE_BLK0, 10, 1}, 	 // [WR_DIS.KEY2_PURPOSE] wr_dis of KEY_PURPOSE_2,
};

static const esp_efuse_desc_t WR_DIS_KEY_PURPOSE_3[] = {
    {EFUSE_BLK0, 11, 1}, 	 // [WR_DIS.KEY3_PURPOSE] wr_dis of KEY_PURPOSE_3,
};

static const esp_efuse_desc_t WR_DIS_KEY_PURPOSE_4[] = {
    {EFUSE_BLK0, 12, 1}, 	 // [WR_DIS.KEY4_PURPOSE] wr_dis of KEY_PURPOSE_4,
};

static const esp_efuse_desc_t WR_DIS_KEY_PURPOSE_5[] = {
    {EFUSE_BLK0, 13, 1}, 	 // [WR_DIS.KEY5_PURPOSE] wr_dis of KEY_PURPOSE_5,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_EN[] = {
    {EFUSE_BLK0, 15, 1}, 	 // [] wr_dis of SECURE_BOOT_EN,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_AGGRESSIVE_REVOKE[] = {
    {EFUSE_BLK0, 16, 1}, 	 // [] wr_dis of SECURE_BOOT_AGGRESSIVE_REVOKE,
};

static const esp_efuse_desc_t WR_DIS_FLASH_TPUW[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] wr_dis of FLASH_TPUW,
};

static const esp_efuse_desc_t WR_DIS_DIS_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] wr_dis of DIS_DOWNLOAD_MODE,
};

static const esp_efuse_desc_t WR_DIS_DIS_DIRECT_BOOT[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [WR_DIS.DIS_LEGACY_SPI_BOOT] wr_dis of DIS_DIRECT_BOOT,
};

static const esp_efuse_desc_t WR_DIS_DIS_USB_SERIAL_JTAG_ROM_PRINT[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [WR_DIS.UART_PRINT_CHANNEL] wr_dis of DIS_USB_SERIAL_JTAG_ROM_PRINT,
};

static const esp_efuse_desc_t WR_DIS_FLASH_ECC_MODE[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] wr_dis of FLASH_ECC_MODE,
};

static const esp_efuse_desc_t WR_DIS_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [WR_DIS.DIS_USB_DOWNLOAD_MODE] wr_dis of DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE,
};

static const esp_efuse_desc_t WR_DIS_ENABLE_SECURITY_DOWNLOAD[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] wr_dis of ENABLE_SECURITY_DOWNLOAD,
};

static const esp_efuse_desc_t WR_DIS_UART_PRINT_CONTROL[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] wr_dis of UART_PRINT_CONTROL,
};

static const esp_efuse_desc_t WR_DIS_PIN_POWER_SELECTION[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] wr_dis of PIN_POWER_SELECTION,
};

static const esp_efuse_desc_t WR_DIS_FLASH_TYPE[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] wr_dis of FLASH_TYPE,
};

static const esp_efuse_desc_t WR_DIS_FLASH_PAGE_SIZE[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] wr_dis of FLASH_PAGE_SIZE,
};

static const esp_efuse_desc_t WR_DIS_FLASH_ECC_EN[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] wr_dis of FLASH_ECC_EN,
};

static const esp_efuse_desc_t WR_DIS_FORCE_SEND_RESUME[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] wr_dis of FORCE_SEND_RESUME,
};

static const esp_efuse_desc_t WR_DIS_SECURE_VERSION[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] wr_dis of SECURE_VERSION,
};

static const esp_efuse_desc_t WR_DIS_DIS_USB_OTG_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 19, 1}, 	 // [] wr_dis of DIS_USB_OTG_DOWNLOAD_MODE,
};

static const esp_efuse_desc_t WR_DIS_DISABLE_WAFER_VERSION_MAJOR[] = {
    {EFUSE_BLK0, 19, 1}, 	 // [] wr_dis of DISABLE_WAFER_VERSION_MAJOR,
};

static const esp_efuse_desc_t WR_DIS_DISABLE_BLK_VERSION_MAJOR[] = {
    {EFUSE_BLK0, 19, 1}, 	 // [] wr_dis of DISABLE_BLK_VERSION_MAJOR,
};

static const esp_efuse_desc_t WR_DIS_BLK1[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of BLOCK1,
};

static const esp_efuse_desc_t WR_DIS_MAC[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [WR_DIS.MAC_FACTORY] wr_dis of MAC,
};

static const esp_efuse_desc_t WR_DIS_SPI_PAD_CONFIG_CLK[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of SPI_PAD_CONFIG_CLK,
};

static const esp_efuse_desc_t WR_DIS_SPI_PAD_CONFIG_Q[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of SPI_PAD_CONFIG_Q,
};

static const esp_efuse_desc_t WR_DIS_SPI_PAD_CONFIG_D[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of SPI_PAD_CONFIG_D,
};

static const esp_efuse_desc_t WR_DIS_SPI_PAD_CONFIG_CS[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of SPI_PAD_CONFIG_CS,
};

static const esp_efuse_desc_t WR_DIS_SPI_PAD_CONFIG_HD[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of SPI_PAD_CONFIG_HD,
};

static const esp_efuse_desc_t WR_DIS_SPI_PAD_CONFIG_WP[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of SPI_PAD_CONFIG_WP,
};

static const esp_efuse_desc_t WR_DIS_SPI_PAD_CONFIG_DQS[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of SPI_PAD_CONFIG_DQS,
};

static const esp_efuse_desc_t WR_DIS_SPI_PAD_CONFIG_D4[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of SPI_PAD_CONFIG_D4,
};

static const esp_efuse_desc_t WR_DIS_SPI_PAD_CONFIG_D5[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of SPI_PAD_CONFIG_D5,
};

static const esp_efuse_desc_t WR_DIS_SPI_PAD_CONFIG_D6[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of SPI_PAD_CONFIG_D6,
};

static const esp_efuse_desc_t WR_DIS_SPI_PAD_CONFIG_D7[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of SPI_PAD_CONFIG_D7,
};

static const esp_efuse_desc_t WR_DIS_WAFER_VERSION_MINOR_LO[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of WAFER_VERSION_MINOR_LO,
};

static const esp_efuse_desc_t WR_DIS_PKG_VERSION[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of PKG_VERSION,
};

static const esp_efuse_desc_t WR_DIS_BLK_VERSION_MINOR[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of BLK_VERSION_MINOR,
};

static const esp_efuse_desc_t WR_DIS_FLASH_CAP[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of FLASH_CAP,
};

static const esp_efuse_desc_t WR_DIS_FLASH_TEMP[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of FLASH_TEMP,
};

static const esp_efuse_desc_t WR_DIS_FLASH_VENDOR[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of FLASH_VENDOR,
};

static const esp_efuse_desc_t WR_DIS_PSRAM_CAP[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of PSRAM_CAP,
};

static const esp_efuse_desc_t WR_DIS_PSRAM_TEMP[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of PSRAM_TEMP,
};

static const esp_efuse_desc_t WR_DIS_PSRAM_VENDOR[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of PSRAM_VENDOR,
};

static const esp_efuse_desc_t WR_DIS_K_RTC_LDO[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of K_RTC_LDO,
};

static const esp_efuse_desc_t WR_DIS_K_DIG_LDO[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of K_DIG_LDO,
};

static const esp_efuse_desc_t WR_DIS_V_RTC_DBIAS20[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of V_RTC_DBIAS20,
};

static const esp_efuse_desc_t WR_DIS_V_DIG_DBIAS20[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of V_DIG_DBIAS20,
};

static const esp_efuse_desc_t WR_DIS_DIG_DBIAS_HVT[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of DIG_DBIAS_HVT,
};

static const esp_efuse_desc_t WR_DIS_WAFER_VERSION_MINOR_HI[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of WAFER_VERSION_MINOR_HI,
};

static const esp_efuse_desc_t WR_DIS_WAFER_VERSION_MAJOR[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of WAFER_VERSION_MAJOR,
};

static const esp_efuse_desc_t WR_DIS_ADC2_CAL_VOL_ATTEN3[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of ADC2_CAL_VOL_ATTEN3,
};

static const esp_efuse_desc_t WR_DIS_SYS_DATA_PART1[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of BLOCK2,
};

static const esp_efuse_desc_t WR_DIS_OPTIONAL_UNIQUE_ID[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of OPTIONAL_UNIQUE_ID,
};

static const esp_efuse_desc_t WR_DIS_BLK_VERSION_MAJOR[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of BLK_VERSION_MAJOR,
};

static const esp_efuse_desc_t WR_DIS_TEMP_CALIB[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of TEMP_CALIB,
};

static const esp_efuse_desc_t WR_DIS_OCODE[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of OCODE,
};

static const esp_efuse_desc_t WR_DIS_ADC1_INIT_CODE_ATTEN0[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_INIT_CODE_ATTEN0,
};

static const esp_efuse_desc_t WR_DIS_ADC1_INIT_CODE_ATTEN1[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_INIT_CODE_ATTEN1,
};

static const esp_efuse_desc_t WR_DIS_ADC1_INIT_CODE_ATTEN2[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_INIT_CODE_ATTEN2,
};

static const esp_efuse_desc_t WR_DIS_ADC1_INIT_CODE_ATTEN3[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_INIT_CODE_ATTEN3,
};

static const esp_efuse_desc_t WR_DIS_ADC2_INIT_CODE_ATTEN0[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC2_INIT_CODE_ATTEN0,
};

static const esp_efuse_desc_t WR_DIS_ADC2_INIT_CODE_ATTEN1[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC2_INIT_CODE_ATTEN1,
};

static const esp_efuse_desc_t WR_DIS_ADC2_INIT_CODE_ATTEN2[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC2_INIT_CODE_ATTEN2,
};

static const esp_efuse_desc_t WR_DIS_ADC2_INIT_CODE_ATTEN3[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC2_INIT_CODE_ATTEN3,
};

static const esp_efuse_desc_t WR_DIS_ADC1_CAL_VOL_ATTEN0[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_CAL_VOL_ATTEN0,
};

static const esp_efuse_desc_t WR_DIS_ADC1_CAL_VOL_ATTEN1[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_CAL_VOL_ATTEN1,
};

static const esp_efuse_desc_t WR_DIS_ADC1_CAL_VOL_ATTEN2[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_CAL_VOL_ATTEN2,
};

static const esp_efuse_desc_t WR_DIS_ADC1_CAL_VOL_ATTEN3[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_CAL_VOL_ATTEN3,
};

static const esp_efuse_desc_t WR_DIS_ADC2_CAL_VOL_ATTEN0[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC2_CAL_VOL_ATTEN0,
};

static const esp_efuse_desc_t WR_DIS_ADC2_CAL_VOL_ATTEN1[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC2_CAL_VOL_ATTEN1,
};

static const esp_efuse_desc_t WR_DIS_ADC2_CAL_VOL_ATTEN2[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC2_CAL_VOL_ATTEN2,
};

static const esp_efuse_desc_t WR_DIS_BLOCK_USR_DATA[] = {
    {EFUSE_BLK0, 22, 1}, 	 // [WR_DIS.USER_DATA] wr_dis of BLOCK_USR_DATA,
};

static const esp_efuse_desc_t WR_DIS_CUSTOM_MAC[] = {
    {EFUSE_BLK0, 22, 1}, 	 // [WR_DIS.MAC_CUSTOM WR_DIS.USER_DATA_MAC_CUSTOM] wr_dis of CUSTOM_MAC,
};

static const esp_efuse_desc_t WR_DIS_BLOCK_KEY0[] = {
    {EFUSE_BLK0, 23, 1}, 	 // [WR_DIS.KEY0] wr_dis of BLOCK_KEY0,
};

static const esp_efuse_desc_t WR_DIS_BLOCK_KEY1[] = {
    {EFUSE_BLK0, 24, 1}, 	 // [WR_DIS.KEY1] wr_dis of BLOCK_KEY1,
};

static const esp_efuse_desc_t WR_DIS_BLOCK_KEY2[] = {
    {EFUSE_BLK0, 25, 1}, 	 // [WR_DIS.KEY2] wr_dis of BLOCK_KEY2,
};

static const esp_efuse_desc_t WR_DIS_BLOCK_KEY3[] = {
    {EFUSE_BLK0, 26, 1}, 	 // [WR_DIS.KEY3] wr_dis of BLOCK_KEY3,
};

static const esp_efuse_desc_t WR_DIS_BLOCK_KEY4[] = {
    {EFUSE_BLK0, 27, 1}, 	 // [WR_DIS.KEY4] wr_dis of BLOCK_KEY4,
};

static const esp_efuse_desc_t WR_DIS_BLOCK_KEY5[] = {
    {EFUSE_BLK0, 28, 1}, 	 // [WR_DIS.KEY5] wr_dis of BLOCK_KEY5,
};

static const esp_efuse_desc_t WR_DIS_BLOCK_SYS_DATA2[] = {
    {EFUSE_BLK0, 29, 1}, 	 // [WR_DIS.SYS_DATA_PART2] wr_dis of BLOCK_SYS_DATA2,
};

static const esp_efuse_desc_t WR_DIS_USB_EXCHG_PINS[] = {
    {EFUSE_BLK0, 30, 1}, 	 // [] wr_dis of USB_EXCHG_PINS,
};

static const esp_efuse_desc_t WR_DIS_USB_EXT_PHY_ENABLE[] = {
    {EFUSE_BLK0, 30, 1}, 	 // [WR_DIS.EXT_PHY_ENABLE] wr_dis of USB_EXT_PHY_ENABLE,
};

static const esp_efuse_desc_t WR_DIS_SOFT_DIS_JTAG[] = {
    {EFUSE_BLK0, 31, 1}, 	 // [] wr_dis of SOFT_DIS_JTAG,
};

static const esp_efuse_desc_t RD_DIS[] = {
    {EFUSE_BLK0, 32, 7}, 	 // [] Disable reading from BlOCK4-10,
};

static const esp_efuse_desc_t RD_DIS_BLOCK_KEY0[] = {
    {EFUSE_BLK0, 32, 1}, 	 // [RD_DIS.KEY0] rd_dis of BLOCK_KEY0,
};

static const esp_efuse_desc_t RD_DIS_BLOCK_KEY1[] = {
    {EFUSE_BLK0, 33, 1}, 	 // [RD_DIS.KEY1] rd_dis of BLOCK_KEY1,
};

static const esp_efuse_desc_t RD_DIS_BLOCK_KEY2[] = {
    {EFUSE_BLK0, 34, 1}, 	 // [RD_DIS.KEY2] rd_dis of BLOCK_KEY2,
};

static const esp_efuse_desc_t RD_DIS_BLOCK_KEY3[] = {
    {EFUSE_BLK0, 35, 1}, 	 // [RD_DIS.KEY3] rd_dis of BLOCK_KEY3,
};

static const esp_efuse_desc_t RD_DIS_BLOCK_KEY4[] = {
    {EFUSE_BLK0, 36, 1}, 	 // [RD_DIS.KEY4] rd_dis of BLOCK_KEY4,
};

static const esp_efuse_desc_t RD_DIS_BLOCK_KEY5[] = {
    {EFUSE_BLK0, 37, 1}, 	 // [RD_DIS.KEY5] rd_dis of BLOCK_KEY5,
};

static const esp_efuse_desc_t RD_DIS_BLOCK_SYS_DATA2[] = {
    {EFUSE_BLK0, 38, 1}, 	 // [RD_DIS.SYS_DATA_PART2] rd_dis of BLOCK_SYS_DATA2,
};

static const esp_efuse_desc_t DIS_ICACHE[] = {
    {EFUSE_BLK0, 40, 1}, 	 // [] Set this bit to disable Icache,
};

static const esp_efuse_desc_t DIS_DCACHE[] = {
    {EFUSE_BLK0, 41, 1}, 	 // [] Set this bit to disable Dcache,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_ICACHE[] = {
    {EFUSE_BLK0, 42, 1}, 	 // [] Set this bit to disable Icache in download mode (boot_mode[3:0] is 0; 1; 2; 3; 6; 7),
};

static const esp_efuse_desc_t DIS_DOWNLOAD_DCACHE[] = {
    {EFUSE_BLK0, 43, 1}, 	 // [] Set this bit to disable Dcache in download mode ( boot_mode[3:0] is 0; 1; 2; 3; 6; 7),
};

static const esp_efuse_desc_t DIS_FORCE_DOWNLOAD[] = {
    {EFUSE_BLK0, 44, 1}, 	 // [] Set this bit to disable the function that forces chip into download mode,
};

static const esp_efuse_desc_t DIS_USB_OTG[] = {
    {EFUSE_BLK0, 45, 1}, 	 // [DIS_USB] Set this bit to disable USB function,
};

static const esp_efuse_desc_t DIS_TWAI[] = {
    {EFUSE_BLK0, 46, 1}, 	 // [DIS_CAN] Set this bit to disable CAN function,
};

static const esp_efuse_desc_t DIS_APP_CPU[] = {
    {EFUSE_BLK0, 47, 1}, 	 // [] Disable app cpu,
};

static const esp_efuse_desc_t SOFT_DIS_JTAG[] = {
    {EFUSE_BLK0, 48, 3}, 	 // [] Set these bits to disable JTAG in the soft way (odd number 1 means disable ). JTAG can be enabled in HMAC module,
};

static const esp_efuse_desc_t DIS_PAD_JTAG[] = {
    {EFUSE_BLK0, 51, 1}, 	 // [HARD_DIS_JTAG] Set this bit to disable JTAG in the hard way. JTAG is disabled permanently,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    {EFUSE_BLK0, 52, 1}, 	 // [] Set this bit to disable flash encryption when in download boot modes,
};

static const esp_efuse_desc_t USB_EXCHG_PINS[] = {
    {EFUSE_BLK0, 57, 1}, 	 // [] Set this bit to exchange USB D+ and D- pins,
};

static const esp_efuse_desc_t USB_EXT_PHY_ENABLE[] = {
    {EFUSE_BLK0, 58, 1}, 	 // [EXT_PHY_ENABLE] Set this bit to enable external PHY,
};

static const esp_efuse_desc_t VDD_SPI_XPD[] = {
    {EFUSE_BLK0, 68, 1}, 	 // [] SPI regulator power up signal,
};

static const esp_efuse_desc_t VDD_SPI_TIEH[] = {
    {EFUSE_BLK0, 69, 1}, 	 // [] If VDD_SPI_FORCE is 1; determines VDD_SPI voltage {0: "VDD_SPI connects to 1.8 V LDO"; 1: "VDD_SPI connects to VDD3P3_RTC_IO"},
};

static const esp_efuse_desc_t VDD_SPI_FORCE[] = {
    {EFUSE_BLK0, 70, 1}, 	 // [] Set this bit and force to use the configuration of eFuse to configure VDD_SPI,
};

static const esp_efuse_desc_t WDT_DELAY_SEL[] = {
    {EFUSE_BLK0, 80, 2}, 	 // [] RTC watchdog timeout threshold; in unit of slow clock cycle {0: "40000"; 1: "80000"; 2: "160000"; 3: "320000"},
};

static const esp_efuse_desc_t SPI_BOOT_CRYPT_CNT[] = {
    {EFUSE_BLK0, 82, 3}, 	 // [] Enables flash encryption when 1 or 3 bits are set and disabled otherwise {0: "Disable"; 1: "Enable"; 3: "Disable"; 7: "Enable"},
};

static const esp_efuse_desc_t SECURE_BOOT_KEY_REVOKE0[] = {
    {EFUSE_BLK0, 85, 1}, 	 // [] Revoke 1st secure boot key,
};

static const esp_efuse_desc_t SECURE_BOOT_KEY_REVOKE1[] = {
    {EFUSE_BLK0, 86, 1}, 	 // [] Revoke 2nd secure boot key,
};

static const esp_efuse_desc_t SECURE_BOOT_KEY_REVOKE2[] = {
    {EFUSE_BLK0, 87, 1}, 	 // [] Revoke 3rd secure boot key,
};

static const esp_efuse_desc_t KEY_PURPOSE_0[] = {
    {EFUSE_BLK0, 88, 4}, 	 // [KEY0_PURPOSE] Purpose of Key0,
};

static const esp_efuse_desc_t KEY_PURPOSE_1[] = {
    {EFUSE_BLK0, 92, 4}, 	 // [KEY1_PURPOSE] Purpose of Key1,
};

static const esp_efuse_desc_t KEY_PURPOSE_2[] = {
    {EFUSE_BLK0, 96, 4}, 	 // [KEY2_PURPOSE] Purpose of Key2,
};

static const esp_efuse_desc_t KEY_PURPOSE_3[] = {
    {EFUSE_BLK0, 100, 4}, 	 // [KEY3_PURPOSE] Purpose of Key3,
};

static const esp_efuse_desc_t KEY_PURPOSE_4[] = {
    {EFUSE_BLK0, 104, 4}, 	 // [KEY4_PURPOSE] Purpose of Key4,
};

static const esp_efuse_desc_t KEY_PURPOSE_5[] = {
    {EFUSE_BLK0, 108, 4}, 	 // [KEY5_PURPOSE] Purpose of Key5,
};

static const esp_efuse_desc_t SECURE_BOOT_EN[] = {
    {EFUSE_BLK0, 116, 1}, 	 // [] Set this bit to enable secure boot,
};

static const esp_efuse_desc_t SECURE_BOOT_AGGRESSIVE_REVOKE[] = {
    {EFUSE_BLK0, 117, 1}, 	 // [] Set this bit to enable revoking aggressive secure boot,
};

static const esp_efuse_desc_t DIS_USB_JTAG[] = {
    {EFUSE_BLK0, 118, 1}, 	 // [] Set this bit to disable function of usb switch to jtag in module of usb device,
};

static const esp_efuse_desc_t DIS_USB_SERIAL_JTAG[] = {
    {EFUSE_BLK0, 119, 1}, 	 // [DIS_USB_DEVICE] Set this bit to disable usb device,
};

static const esp_efuse_desc_t STRAP_JTAG_SEL[] = {
    {EFUSE_BLK0, 120, 1}, 	 // [] Set this bit to enable selection between usb_to_jtag and pad_to_jtag through strapping gpio10 when both reg_dis_usb_jtag and reg_dis_pad_jtag are equal to 0,
};

static const esp_efuse_desc_t USB_PHY_SEL[] = {
    {EFUSE_BLK0, 121, 1}, 	 // [] This bit is used to switch internal PHY and external PHY for USB OTG and USB Device {0: "internal PHY is assigned to USB Device while external PHY is assigned to USB OTG"; 1: "internal PHY is assigned to USB OTG while external PHY is assigned to USB Device"},
};

static const esp_efuse_desc_t FLASH_TPUW[] = {
    {EFUSE_BLK0, 124, 4}, 	 // [] Configures flash waiting time after power-up; in unit of ms. If the value is less than 15; the waiting time is the configurable value.  Otherwise; the waiting time is twice the configurable value,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 128, 1}, 	 // [] Set this bit to disable download mode (boot_mode[3:0] = 0; 1; 2; 3; 6; 7),
};

static const esp_efuse_desc_t DIS_DIRECT_BOOT[] = {
    {EFUSE_BLK0, 129, 1}, 	 // [DIS_LEGACY_SPI_BOOT] Disable direct boot mode,
};

static const esp_efuse_desc_t DIS_USB_SERIAL_JTAG_ROM_PRINT[] = {
    {EFUSE_BLK0, 130, 1}, 	 // [UART_PRINT_CHANNEL] USB printing {0: "Enable"; 1: "Disable"},
};

static const esp_efuse_desc_t FLASH_ECC_MODE[] = {
    {EFUSE_BLK0, 131, 1}, 	 // [] Flash ECC mode in ROM {0: "16to18 byte"; 1: "16to17 byte"},
};

static const esp_efuse_desc_t DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 132, 1}, 	 // [DIS_USB_DOWNLOAD_MODE] Set this bit to disable UART download mode through USB,
};

static const esp_efuse_desc_t ENABLE_SECURITY_DOWNLOAD[] = {
    {EFUSE_BLK0, 133, 1}, 	 // [] Set this bit to enable secure UART download mode,
};

static const esp_efuse_desc_t UART_PRINT_CONTROL[] = {
    {EFUSE_BLK0, 134, 2}, 	 // [] Set the default UART boot message output mode {0: "Enable"; 1: "Enable when GPIO46 is low at reset"; 2: "Enable when GPIO46 is high at reset"; 3: "Disable"},
};

static const esp_efuse_desc_t PIN_POWER_SELECTION[] = {
    {EFUSE_BLK0, 136, 1}, 	 // [] Set default power supply for GPIO33-GPIO37; set when SPI flash is initialized {0: "VDD3P3_CPU"; 1: "VDD_SPI"},
};

static const esp_efuse_desc_t FLASH_TYPE[] = {
    {EFUSE_BLK0, 137, 1}, 	 // [] SPI flash type {0: "4 data lines"; 1: "8 data lines"},
};

static const esp_efuse_desc_t FLASH_PAGE_SIZE[] = {
    {EFUSE_BLK0, 138, 2}, 	 // [] Set Flash page size,
};

static const esp_efuse_desc_t FLASH_ECC_EN[] = {
    {EFUSE_BLK0, 140, 1}, 	 // [] Set 1 to enable ECC for flash boot,
};

static const esp_efuse_desc_t FORCE_SEND_RESUME[] = {
    {EFUSE_BLK0, 141, 1}, 	 // [] Set this bit to force ROM code to send a resume command during SPI boot,
};

static const esp_efuse_desc_t SECURE_VERSION[] = {
    {EFUSE_BLK0, 142, 16}, 	 // [] Secure version (used by ESP-IDF anti-rollback feature),
};

static const esp_efuse_desc_t DIS_USB_OTG_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 159, 1}, 	 // [] Set this bit to disable download through USB-OTG,
};

static const esp_efuse_desc_t DISABLE_WAFER_VERSION_MAJOR[] = {
    {EFUSE_BLK0, 160, 1}, 	 // [] Disables check of wafer version major,
};

static const esp_efuse_desc_t DISABLE_BLK_VERSION_MAJOR[] = {
    {EFUSE_BLK0, 161, 1}, 	 // [] Disables check of blk version major,
};

static const esp_efuse_desc_t MAC[] = {
    {EFUSE_BLK1, 40, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK1, 32, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK1, 24, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK1, 16, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK1, 8, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK1, 0, 8}, 	 // [MAC_FACTORY] MAC address,
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_CLK[] = {
    {EFUSE_BLK1, 48, 6}, 	 // [] SPI_PAD_configure CLK,
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_Q[] = {
    {EFUSE_BLK1, 54, 6}, 	 // [] SPI_PAD_configure Q(D1),
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_D[] = {
    {EFUSE_BLK1, 60, 6}, 	 // [] SPI_PAD_configure D(D0),
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_CS[] = {
    {EFUSE_BLK1, 66, 6}, 	 // [] SPI_PAD_configure CS,
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_HD[] = {
    {EFUSE_BLK1, 72, 6}, 	 // [] SPI_PAD_configure HD(D3),
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_WP[] = {
    {EFUSE_BLK1, 78, 6}, 	 // [] SPI_PAD_configure WP(D2),
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_DQS[] = {
    {EFUSE_BLK1, 84, 6}, 	 // [] SPI_PAD_configure DQS,
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_D4[] = {
    {EFUSE_BLK1, 90, 6}, 	 // [] SPI_PAD_configure D4,
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_D5[] = {
    {EFUSE_BLK1, 96, 6}, 	 // [] SPI_PAD_configure D5,
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_D6[] = {
    {EFUSE_BLK1, 102, 6}, 	 // [] SPI_PAD_configure D6,
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_D7[] = {
    {EFUSE_BLK1, 108, 6}, 	 // [] SPI_PAD_configure D7,
};

static const esp_efuse_desc_t WAFER_VERSION_MINOR_LO[] = {
    {EFUSE_BLK1, 114, 3}, 	 // [] WAFER_VERSION_MINOR least significant bits,
};

static const esp_efuse_desc_t PKG_VERSION[] = {
    {EFUSE_BLK1, 117, 3}, 	 // [] Package version,
};

static const esp_efuse_desc_t BLK_VERSION_MINOR[] = {
    {EFUSE_BLK1, 120, 3}, 	 // [] BLK_VERSION_MINOR,
};

static const esp_efuse_desc_t FLASH_CAP[] = {
    {EFUSE_BLK1, 123, 3}, 	 // [] Flash capacity {0: "None"; 1: "8M"; 2: "4M"},
};

static const esp_efuse_desc_t FLASH_TEMP[] = {
    {EFUSE_BLK1, 126, 2}, 	 // [] Flash temperature {0: "None"; 1: "105C"; 2: "85C"},
};

static const esp_efuse_desc_t FLASH_VENDOR[] = {
    {EFUSE_BLK1, 128, 3}, 	 // [] Flash vendor {0: "None"; 1: "XMC"; 2: "GD"; 3: "FM"; 4: "TT"; 5: "BY"},
};

static const esp_efuse_desc_t PSRAM_CAP[] = {
    {EFUSE_BLK1, 131, 2}, 	 // [] PSRAM capacity {0: "None"; 1: "8M"; 2: "2M"},
};

static const esp_efuse_desc_t PSRAM_TEMP[] = {
    {EFUSE_BLK1, 133, 2}, 	 // [] PSRAM temperature {0: "None"; 1: "105C"; 2: "85C"},
};

static const esp_efuse_desc_t PSRAM_VENDOR[] = {
    {EFUSE_BLK1, 135, 2}, 	 // [] PSRAM vendor {0: "None"; 1: "AP_3v3"; 2: "AP_1v8"},
};

static const esp_efuse_desc_t K_RTC_LDO[] = {
    {EFUSE_BLK1, 141, 7}, 	 // [] BLOCK1 K_RTC_LDO,
};

static const esp_efuse_desc_t K_DIG_LDO[] = {
    {EFUSE_BLK1, 148, 7}, 	 // [] BLOCK1 K_DIG_LDO,
};

static const esp_efuse_desc_t V_RTC_DBIAS20[] = {
    {EFUSE_BLK1, 155, 8}, 	 // [] BLOCK1 voltage of rtc dbias20,
};

static const esp_efuse_desc_t V_DIG_DBIAS20[] = {
    {EFUSE_BLK1, 163, 8}, 	 // [] BLOCK1 voltage of digital dbias20,
};

static const esp_efuse_desc_t DIG_DBIAS_HVT[] = {
    {EFUSE_BLK1, 171, 5}, 	 // [] BLOCK1 digital dbias when hvt,
};

static const esp_efuse_desc_t WAFER_VERSION_MINOR_HI[] = {
    {EFUSE_BLK1, 183, 1}, 	 // [] WAFER_VERSION_MINOR most significant bit,
};

static const esp_efuse_desc_t WAFER_VERSION_MAJOR[] = {
    {EFUSE_BLK1, 184, 2}, 	 // [] WAFER_VERSION_MAJOR,
};

static const esp_efuse_desc_t ADC2_CAL_VOL_ATTEN3[] = {
    {EFUSE_BLK1, 186, 6}, 	 // [] ADC2 calibration voltage at atten3,
};

static const esp_efuse_desc_t OPTIONAL_UNIQUE_ID[] = {
    {EFUSE_BLK2, 0, 128}, 	 // [] Optional unique 128-bit ID,
};

static const esp_efuse_desc_t BLK_VERSION_MAJOR[] = {
    {EFUSE_BLK2, 128, 2}, 	 // [] BLK_VERSION_MAJOR of BLOCK2 {0: "No calib"; 1: "ADC calib V1"},
};

static const esp_efuse_desc_t TEMP_CALIB[] = {
    {EFUSE_BLK2, 132, 9}, 	 // [] Temperature calibration data,
};

static const esp_efuse_desc_t OCODE[] = {
    {EFUSE_BLK2, 141, 8}, 	 // [] ADC OCode,
};

static const esp_efuse_desc_t ADC1_INIT_CODE_ATTEN0[] = {
    {EFUSE_BLK2, 149, 8}, 	 // [] ADC1 init code at atten0,
};

static const esp_efuse_desc_t ADC1_INIT_CODE_ATTEN1[] = {
    {EFUSE_BLK2, 157, 6}, 	 // [] ADC1 init code at atten1,
};

static const esp_efuse_desc_t ADC1_INIT_CODE_ATTEN2[] = {
    {EFUSE_BLK2, 163, 6}, 	 // [] ADC1 init code at atten2,
};

static const esp_efuse_desc_t ADC1_INIT_CODE_ATTEN3[] = {
    {EFUSE_BLK2, 169, 6}, 	 // [] ADC1 init code at atten3,
};

static const esp_efuse_desc_t ADC2_INIT_CODE_ATTEN0[] = {
    {EFUSE_BLK2, 175, 8}, 	 // [] ADC2 init code at atten0,
};

static const esp_efuse_desc_t ADC2_INIT_CODE_ATTEN1[] = {
    {EFUSE_BLK2, 183, 6}, 	 // [] ADC2 init code at atten1,
};

static const esp_efuse_desc_t ADC2_INIT_CODE_ATTEN2[] = {
    {EFUSE_BLK2, 189, 6}, 	 // [] ADC2 init code at atten2,
};

static const esp_efuse_desc_t ADC2_INIT_CODE_ATTEN3[] = {
    {EFUSE_BLK2, 195, 6}, 	 // [] ADC2 init code at atten3,
};

static const esp_efuse_desc_t ADC1_CAL_VOL_ATTEN0[] = {
    {EFUSE_BLK2, 201, 8}, 	 // [] ADC1 calibration voltage at atten0,
};

static const esp_efuse_desc_t ADC1_CAL_VOL_ATTEN1[] = {
    {EFUSE_BLK2, 209, 8}, 	 // [] ADC1 calibration voltage at atten1,
};

static const esp_efuse_desc_t ADC1_CAL_VOL_ATTEN2[] = {
    {EFUSE_BLK2, 217, 8}, 	 // [] ADC1 calibration voltage at atten2,
};

static const esp_efuse_desc_t ADC1_CAL_VOL_ATTEN3[] = {
    {EFUSE_BLK2, 225, 8}, 	 // [] ADC1 calibration voltage at atten3,
};

static const esp_efuse_desc_t ADC2_CAL_VOL_ATTEN0[] = {
    {EFUSE_BLK2, 233, 8}, 	 // [] ADC2 calibration voltage at atten0,
};

static const esp_efuse_desc_t ADC2_CAL_VOL_ATTEN1[] = {
    {EFUSE_BLK2, 241, 7}, 	 // [] ADC2 calibration voltage at atten1,
};

static const esp_efuse_desc_t ADC2_CAL_VOL_ATTEN2[] = {
    {EFUSE_BLK2, 248, 7}, 	 // [] ADC2 calibration voltage at atten2,
};

static const esp_efuse_desc_t USER_DATA[] = {
    {EFUSE_BLK3, 0, 256}, 	 // [BLOCK_USR_DATA] User data,
};

static const esp_efuse_desc_t USER_DATA_MAC_CUSTOM[] = {
    {EFUSE_BLK3, 200, 48}, 	 // [MAC_CUSTOM CUSTOM_MAC] Custom MAC,
};

static const esp_efuse_desc_t KEY0[] = {
    {EFUSE_BLK4, 0, 256}, 	 // [BLOCK_KEY0] Key0 or user data,
};

static const esp_efuse_desc_t KEY1[] = {
    {EFUSE_BLK5, 0, 256}, 	 // [BLOCK_KEY1] Key1 or user data,
};

static const esp_efuse_desc_t KEY2[] = {
    {EFUSE_BLK6, 0, 256}, 	 // [BLOCK_KEY2] Key2 or user data,
};

static const esp_efuse_desc_t KEY3[] = {
    {EFUSE_BLK7, 0, 256}, 	 // [BLOCK_KEY3] Key3 or user data,
};

static const esp_efuse_desc_t KEY4[] = {
    {EFUSE_BLK8, 0, 256}, 	 // [BLOCK_KEY4] Key4 or user data,
};

static const esp_efuse_desc_t KEY5[] = {
    {EFUSE_BLK9, 0, 256}, 	 // [BLOCK_KEY5] Key5 or user data,
};

static const esp_efuse_desc_t SYS_DATA_PART2[] = {
    {EFUSE_BLK10, 0, 256}, 	 // [BLOCK_SYS_DATA2] System data part 2 (reserved),
};





const esp_efuse_desc_t* ESP_EFUSE_WR_DIS[] = {
    &WR_DIS[0],    		// [] Disable programming of individual eFuses
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RD_DIS[] = {
    &WR_DIS_RD_DIS[0],    		// [] wr_dis of RD_DIS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_ICACHE[] = {
    &WR_DIS_DIS_ICACHE[0],    		// [] wr_dis of DIS_ICACHE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DCACHE[] = {
    &WR_DIS_DIS_DCACHE[0],    		// [] wr_dis of DIS_DCACHE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DOWNLOAD_ICACHE[] = {
    &WR_DIS_DIS_DOWNLOAD_ICACHE[0],    		// [] wr_dis of DIS_DOWNLOAD_ICACHE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DOWNLOAD_DCACHE[] = {
    &WR_DIS_DIS_DOWNLOAD_DCACHE[0],    		// [] wr_dis of DIS_DOWNLOAD_DCACHE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_FORCE_DOWNLOAD[] = {
    &WR_DIS_DIS_FORCE_DOWNLOAD[0],    		// [] wr_dis of DIS_FORCE_DOWNLOAD
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_USB_OTG[] = {
    &WR_DIS_DIS_USB_OTG[0],    		// [WR_DIS.DIS_USB] wr_dis of DIS_USB_OTG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_TWAI[] = {
    &WR_DIS_DIS_TWAI[0],    		// [WR_DIS.DIS_CAN] wr_dis of DIS_TWAI
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_APP_CPU[] = {
    &WR_DIS_DIS_APP_CPU[0],    		// [] wr_dis of DIS_APP_CPU
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_PAD_JTAG[] = {
    &WR_DIS_DIS_PAD_JTAG[0],    		// [WR_DIS.HARD_DIS_JTAG] wr_dis of DIS_PAD_JTAG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    &WR_DIS_DIS_DOWNLOAD_MANUAL_ENCRYPT[0],    		// [] wr_dis of DIS_DOWNLOAD_MANUAL_ENCRYPT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_USB_JTAG[] = {
    &WR_DIS_DIS_USB_JTAG[0],    		// [] wr_dis of DIS_USB_JTAG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_USB_SERIAL_JTAG[] = {
    &WR_DIS_DIS_USB_SERIAL_JTAG[0],    		// [WR_DIS.DIS_USB_DEVICE] wr_dis of DIS_USB_SERIAL_JTAG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_STRAP_JTAG_SEL[] = {
    &WR_DIS_STRAP_JTAG_SEL[0],    		// [] wr_dis of STRAP_JTAG_SEL
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_USB_PHY_SEL[] = {
    &WR_DIS_USB_PHY_SEL[0],    		// [] wr_dis of USB_PHY_SEL
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_VDD_SPI_XPD[] = {
    &WR_DIS_VDD_SPI_XPD[0],    		// [] wr_dis of VDD_SPI_XPD
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_VDD_SPI_TIEH[] = {
    &WR_DIS_VDD_SPI_TIEH[0],    		// [] wr_dis of VDD_SPI_TIEH
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_VDD_SPI_FORCE[] = {
    &WR_DIS_VDD_SPI_FORCE[0],    		// [] wr_dis of VDD_SPI_FORCE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_WDT_DELAY_SEL[] = {
    &WR_DIS_WDT_DELAY_SEL[0],    		// [] wr_dis of WDT_DELAY_SEL
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_BOOT_CRYPT_CNT[] = {
    &WR_DIS_SPI_BOOT_CRYPT_CNT[0],    		// [] wr_dis of SPI_BOOT_CRYPT_CNT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE0[] = {
    &WR_DIS_SECURE_BOOT_KEY_REVOKE0[0],    		// [] wr_dis of SECURE_BOOT_KEY_REVOKE0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE1[] = {
    &WR_DIS_SECURE_BOOT_KEY_REVOKE1[0],    		// [] wr_dis of SECURE_BOOT_KEY_REVOKE1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE2[] = {
    &WR_DIS_SECURE_BOOT_KEY_REVOKE2[0],    		// [] wr_dis of SECURE_BOOT_KEY_REVOKE2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_PURPOSE_0[] = {
    &WR_DIS_KEY_PURPOSE_0[0],    		// [WR_DIS.KEY0_PURPOSE] wr_dis of KEY_PURPOSE_0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_PURPOSE_1[] = {
    &WR_DIS_KEY_PURPOSE_1[0],    		// [WR_DIS.KEY1_PURPOSE] wr_dis of KEY_PURPOSE_1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_PURPOSE_2[] = {
    &WR_DIS_KEY_PURPOSE_2[0],    		// [WR_DIS.KEY2_PURPOSE] wr_dis of KEY_PURPOSE_2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_PURPOSE_3[] = {
    &WR_DIS_KEY_PURPOSE_3[0],    		// [WR_DIS.KEY3_PURPOSE] wr_dis of KEY_PURPOSE_3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_PURPOSE_4[] = {
    &WR_DIS_KEY_PURPOSE_4[0],    		// [WR_DIS.KEY4_PURPOSE] wr_dis of KEY_PURPOSE_4
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_PURPOSE_5[] = {
    &WR_DIS_KEY_PURPOSE_5[0],    		// [WR_DIS.KEY5_PURPOSE] wr_dis of KEY_PURPOSE_5
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_EN[] = {
    &WR_DIS_SECURE_BOOT_EN[0],    		// [] wr_dis of SECURE_BOOT_EN
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_AGGRESSIVE_REVOKE[] = {
    &WR_DIS_SECURE_BOOT_AGGRESSIVE_REVOKE[0],    		// [] wr_dis of SECURE_BOOT_AGGRESSIVE_REVOKE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_TPUW[] = {
    &WR_DIS_FLASH_TPUW[0],    		// [] wr_dis of FLASH_TPUW
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DOWNLOAD_MODE[] = {
    &WR_DIS_DIS_DOWNLOAD_MODE[0],    		// [] wr_dis of DIS_DOWNLOAD_MODE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DIRECT_BOOT[] = {
    &WR_DIS_DIS_DIRECT_BOOT[0],    		// [WR_DIS.DIS_LEGACY_SPI_BOOT] wr_dis of DIS_DIRECT_BOOT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_USB_SERIAL_JTAG_ROM_PRINT[] = {
    &WR_DIS_DIS_USB_SERIAL_JTAG_ROM_PRINT[0],    		// [WR_DIS.UART_PRINT_CHANNEL] wr_dis of DIS_USB_SERIAL_JTAG_ROM_PRINT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_ECC_MODE[] = {
    &WR_DIS_FLASH_ECC_MODE[0],    		// [] wr_dis of FLASH_ECC_MODE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[] = {
    &WR_DIS_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[0],    		// [WR_DIS.DIS_USB_DOWNLOAD_MODE] wr_dis of DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ENABLE_SECURITY_DOWNLOAD[] = {
    &WR_DIS_ENABLE_SECURITY_DOWNLOAD[0],    		// [] wr_dis of ENABLE_SECURITY_DOWNLOAD
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_UART_PRINT_CONTROL[] = {
    &WR_DIS_UART_PRINT_CONTROL[0],    		// [] wr_dis of UART_PRINT_CONTROL
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PIN_POWER_SELECTION[] = {
    &WR_DIS_PIN_POWER_SELECTION[0],    		// [] wr_dis of PIN_POWER_SELECTION
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_TYPE[] = {
    &WR_DIS_FLASH_TYPE[0],    		// [] wr_dis of FLASH_TYPE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_PAGE_SIZE[] = {
    &WR_DIS_FLASH_PAGE_SIZE[0],    		// [] wr_dis of FLASH_PAGE_SIZE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_ECC_EN[] = {
    &WR_DIS_FLASH_ECC_EN[0],    		// [] wr_dis of FLASH_ECC_EN
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FORCE_SEND_RESUME[] = {
    &WR_DIS_FORCE_SEND_RESUME[0],    		// [] wr_dis of FORCE_SEND_RESUME
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_VERSION[] = {
    &WR_DIS_SECURE_VERSION[0],    		// [] wr_dis of SECURE_VERSION
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_USB_OTG_DOWNLOAD_MODE[] = {
    &WR_DIS_DIS_USB_OTG_DOWNLOAD_MODE[0],    		// [] wr_dis of DIS_USB_OTG_DOWNLOAD_MODE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DISABLE_WAFER_VERSION_MAJOR[] = {
    &WR_DIS_DISABLE_WAFER_VERSION_MAJOR[0],    		// [] wr_dis of DISABLE_WAFER_VERSION_MAJOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DISABLE_BLK_VERSION_MAJOR[] = {
    &WR_DIS_DISABLE_BLK_VERSION_MAJOR[0],    		// [] wr_dis of DISABLE_BLK_VERSION_MAJOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK1[] = {
    &WR_DIS_BLK1[0],    		// [] wr_dis of BLOCK1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_MAC[] = {
    &WR_DIS_MAC[0],    		// [WR_DIS.MAC_FACTORY] wr_dis of MAC
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_CLK[] = {
    &WR_DIS_SPI_PAD_CONFIG_CLK[0],    		// [] wr_dis of SPI_PAD_CONFIG_CLK
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_Q[] = {
    &WR_DIS_SPI_PAD_CONFIG_Q[0],    		// [] wr_dis of SPI_PAD_CONFIG_Q
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_D[] = {
    &WR_DIS_SPI_PAD_CONFIG_D[0],    		// [] wr_dis of SPI_PAD_CONFIG_D
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_CS[] = {
    &WR_DIS_SPI_PAD_CONFIG_CS[0],    		// [] wr_dis of SPI_PAD_CONFIG_CS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_HD[] = {
    &WR_DIS_SPI_PAD_CONFIG_HD[0],    		// [] wr_dis of SPI_PAD_CONFIG_HD
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_WP[] = {
    &WR_DIS_SPI_PAD_CONFIG_WP[0],    		// [] wr_dis of SPI_PAD_CONFIG_WP
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_DQS[] = {
    &WR_DIS_SPI_PAD_CONFIG_DQS[0],    		// [] wr_dis of SPI_PAD_CONFIG_DQS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_D4[] = {
    &WR_DIS_SPI_PAD_CONFIG_D4[0],    		// [] wr_dis of SPI_PAD_CONFIG_D4
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_D5[] = {
    &WR_DIS_SPI_PAD_CONFIG_D5[0],    		// [] wr_dis of SPI_PAD_CONFIG_D5
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_D6[] = {
    &WR_DIS_SPI_PAD_CONFIG_D6[0],    		// [] wr_dis of SPI_PAD_CONFIG_D6
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_D7[] = {
    &WR_DIS_SPI_PAD_CONFIG_D7[0],    		// [] wr_dis of SPI_PAD_CONFIG_D7
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_WAFER_VERSION_MINOR_LO[] = {
    &WR_DIS_WAFER_VERSION_MINOR_LO[0],    		// [] wr_dis of WAFER_VERSION_MINOR_LO
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PKG_VERSION[] = {
    &WR_DIS_PKG_VERSION[0],    		// [] wr_dis of PKG_VERSION
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK_VERSION_MINOR[] = {
    &WR_DIS_BLK_VERSION_MINOR[0],    		// [] wr_dis of BLK_VERSION_MINOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_CAP[] = {
    &WR_DIS_FLASH_CAP[0],    		// [] wr_dis of FLASH_CAP
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_TEMP[] = {
    &WR_DIS_FLASH_TEMP[0],    		// [] wr_dis of FLASH_TEMP
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_VENDOR[] = {
    &WR_DIS_FLASH_VENDOR[0],    		// [] wr_dis of FLASH_VENDOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PSRAM_CAP[] = {
    &WR_DIS_PSRAM_CAP[0],    		// [] wr_dis of PSRAM_CAP
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PSRAM_TEMP[] = {
    &WR_DIS_PSRAM_TEMP[0],    		// [] wr_dis of PSRAM_TEMP
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PSRAM_VENDOR[] = {
    &WR_DIS_PSRAM_VENDOR[0],    		// [] wr_dis of PSRAM_VENDOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_K_RTC_LDO[] = {
    &WR_DIS_K_RTC_LDO[0],    		// [] wr_dis of K_RTC_LDO
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_K_DIG_LDO[] = {
    &WR_DIS_K_DIG_LDO[0],    		// [] wr_dis of K_DIG_LDO
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_V_RTC_DBIAS20[] = {
    &WR_DIS_V_RTC_DBIAS20[0],    		// [] wr_dis of V_RTC_DBIAS20
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_V_DIG_DBIAS20[] = {
    &WR_DIS_V_DIG_DBIAS20[0],    		// [] wr_dis of V_DIG_DBIAS20
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIG_DBIAS_HVT[] = {
    &WR_DIS_DIG_DBIAS_HVT[0],    		// [] wr_dis of DIG_DBIAS_HVT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_WAFER_VERSION_MINOR_HI[] = {
    &WR_DIS_WAFER_VERSION_MINOR_HI[0],    		// [] wr_dis of WAFER_VERSION_MINOR_HI
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_WAFER_VERSION_MAJOR[] = {
    &WR_DIS_WAFER_VERSION_MAJOR[0],    		// [] wr_dis of WAFER_VERSION_MAJOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_CAL_VOL_ATTEN3[] = {
    &WR_DIS_ADC2_CAL_VOL_ATTEN3[0],    		// [] wr_dis of ADC2_CAL_VOL_ATTEN3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SYS_DATA_PART1[] = {
    &WR_DIS_SYS_DATA_PART1[0],    		// [] wr_dis of BLOCK2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_OPTIONAL_UNIQUE_ID[] = {
    &WR_DIS_OPTIONAL_UNIQUE_ID[0],    		// [] wr_dis of OPTIONAL_UNIQUE_ID
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK_VERSION_MAJOR[] = {
    &WR_DIS_BLK_VERSION_MAJOR[0],    		// [] wr_dis of BLK_VERSION_MAJOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_TEMP_CALIB[] = {
    &WR_DIS_TEMP_CALIB[0],    		// [] wr_dis of TEMP_CALIB
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_OCODE[] = {
    &WR_DIS_OCODE[0],    		// [] wr_dis of OCODE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_INIT_CODE_ATTEN0[] = {
    &WR_DIS_ADC1_INIT_CODE_ATTEN0[0],    		// [] wr_dis of ADC1_INIT_CODE_ATTEN0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_INIT_CODE_ATTEN1[] = {
    &WR_DIS_ADC1_INIT_CODE_ATTEN1[0],    		// [] wr_dis of ADC1_INIT_CODE_ATTEN1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_INIT_CODE_ATTEN2[] = {
    &WR_DIS_ADC1_INIT_CODE_ATTEN2[0],    		// [] wr_dis of ADC1_INIT_CODE_ATTEN2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_INIT_CODE_ATTEN3[] = {
    &WR_DIS_ADC1_INIT_CODE_ATTEN3[0],    		// [] wr_dis of ADC1_INIT_CODE_ATTEN3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_INIT_CODE_ATTEN0[] = {
    &WR_DIS_ADC2_INIT_CODE_ATTEN0[0],    		// [] wr_dis of ADC2_INIT_CODE_ATTEN0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_INIT_CODE_ATTEN1[] = {
    &WR_DIS_ADC2_INIT_CODE_ATTEN1[0],    		// [] wr_dis of ADC2_INIT_CODE_ATTEN1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_INIT_CODE_ATTEN2[] = {
    &WR_DIS_ADC2_INIT_CODE_ATTEN2[0],    		// [] wr_dis of ADC2_INIT_CODE_ATTEN2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_INIT_CODE_ATTEN3[] = {
    &WR_DIS_ADC2_INIT_CODE_ATTEN3[0],    		// [] wr_dis of ADC2_INIT_CODE_ATTEN3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_CAL_VOL_ATTEN0[] = {
    &WR_DIS_ADC1_CAL_VOL_ATTEN0[0],    		// [] wr_dis of ADC1_CAL_VOL_ATTEN0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_CAL_VOL_ATTEN1[] = {
    &WR_DIS_ADC1_CAL_VOL_ATTEN1[0],    		// [] wr_dis of ADC1_CAL_VOL_ATTEN1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_CAL_VOL_ATTEN2[] = {
    &WR_DIS_ADC1_CAL_VOL_ATTEN2[0],    		// [] wr_dis of ADC1_CAL_VOL_ATTEN2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_CAL_VOL_ATTEN3[] = {
    &WR_DIS_ADC1_CAL_VOL_ATTEN3[0],    		// [] wr_dis of ADC1_CAL_VOL_ATTEN3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_CAL_VOL_ATTEN0[] = {
    &WR_DIS_ADC2_CAL_VOL_ATTEN0[0],    		// [] wr_dis of ADC2_CAL_VOL_ATTEN0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_CAL_VOL_ATTEN1[] = {
    &WR_DIS_ADC2_CAL_VOL_ATTEN1[0],    		// [] wr_dis of ADC2_CAL_VOL_ATTEN1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_CAL_VOL_ATTEN2[] = {
    &WR_DIS_ADC2_CAL_VOL_ATTEN2[0],    		// [] wr_dis of ADC2_CAL_VOL_ATTEN2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_USR_DATA[] = {
    &WR_DIS_BLOCK_USR_DATA[0],    		// [WR_DIS.USER_DATA] wr_dis of BLOCK_USR_DATA
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_CUSTOM_MAC[] = {
    &WR_DIS_CUSTOM_MAC[0],    		// [WR_DIS.MAC_CUSTOM WR_DIS.USER_DATA_MAC_CUSTOM] wr_dis of CUSTOM_MAC
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY0[] = {
    &WR_DIS_BLOCK_KEY0[0],    		// [WR_DIS.KEY0] wr_dis of BLOCK_KEY0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY1[] = {
    &WR_DIS_BLOCK_KEY1[0],    		// [WR_DIS.KEY1] wr_dis of BLOCK_KEY1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY2[] = {
    &WR_DIS_BLOCK_KEY2[0],    		// [WR_DIS.KEY2] wr_dis of BLOCK_KEY2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY3[] = {
    &WR_DIS_BLOCK_KEY3[0],    		// [WR_DIS.KEY3] wr_dis of BLOCK_KEY3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY4[] = {
    &WR_DIS_BLOCK_KEY4[0],    		// [WR_DIS.KEY4] wr_dis of BLOCK_KEY4
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY5[] = {
    &WR_DIS_BLOCK_KEY5[0],    		// [WR_DIS.KEY5] wr_dis of BLOCK_KEY5
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_SYS_DATA2[] = {
    &WR_DIS_BLOCK_SYS_DATA2[0],    		// [WR_DIS.SYS_DATA_PART2] wr_dis of BLOCK_SYS_DATA2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_USB_EXCHG_PINS[] = {
    &WR_DIS_USB_EXCHG_PINS[0],    		// [] wr_dis of USB_EXCHG_PINS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_USB_EXT_PHY_ENABLE[] = {
    &WR_DIS_USB_EXT_PHY_ENABLE[0],    		// [WR_DIS.EXT_PHY_ENABLE] wr_dis of USB_EXT_PHY_ENABLE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SOFT_DIS_JTAG[] = {
    &WR_DIS_SOFT_DIS_JTAG[0],    		// [] wr_dis of SOFT_DIS_JTAG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS[] = {
    &RD_DIS[0],    		// [] Disable reading from BlOCK4-10
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_KEY0[] = {
    &RD_DIS_BLOCK_KEY0[0],    		// [RD_DIS.KEY0] rd_dis of BLOCK_KEY0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_KEY1[] = {
    &RD_DIS_BLOCK_KEY1[0],    		// [RD_DIS.KEY1] rd_dis of BLOCK_KEY1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_KEY2[] = {
    &RD_DIS_BLOCK_KEY2[0],    		// [RD_DIS.KEY2] rd_dis of BLOCK_KEY2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_KEY3[] = {
    &RD_DIS_BLOCK_KEY3[0],    		// [RD_DIS.KEY3] rd_dis of BLOCK_KEY3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_KEY4[] = {
    &RD_DIS_BLOCK_KEY4[0],    		// [RD_DIS.KEY4] rd_dis of BLOCK_KEY4
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_KEY5[] = {
    &RD_DIS_BLOCK_KEY5[0],    		// [RD_DIS.KEY5] rd_dis of BLOCK_KEY5
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_SYS_DATA2[] = {
    &RD_DIS_BLOCK_SYS_DATA2[0],    		// [RD_DIS.SYS_DATA_PART2] rd_dis of BLOCK_SYS_DATA2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_ICACHE[] = {
    &DIS_ICACHE[0],    		// [] Set this bit to disable Icache
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DCACHE[] = {
    &DIS_DCACHE[0],    		// [] Set this bit to disable Dcache
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_ICACHE[] = {
    &DIS_DOWNLOAD_ICACHE[0],    		// [] Set this bit to disable Icache in download mode (boot_mode[3:0] is 0; 1; 2; 3; 6; 7)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_DCACHE[] = {
    &DIS_DOWNLOAD_DCACHE[0],    		// [] Set this bit to disable Dcache in download mode ( boot_mode[3:0] is 0; 1; 2; 3; 6; 7)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_FORCE_DOWNLOAD[] = {
    &DIS_FORCE_DOWNLOAD[0],    		// [] Set this bit to disable the function that forces chip into download mode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_OTG[] = {
    &DIS_USB_OTG[0],    		// [DIS_USB] Set this bit to disable USB function
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_TWAI[] = {
    &DIS_TWAI[0],    		// [DIS_CAN] Set this bit to disable CAN function
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_APP_CPU[] = {
    &DIS_APP_CPU[0],    		// [] Disable app cpu
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SOFT_DIS_JTAG[] = {
    &SOFT_DIS_JTAG[0],    		// [] Set these bits to disable JTAG in the soft way (odd number 1 means disable ). JTAG can be enabled in HMAC module
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_PAD_JTAG[] = {
    &DIS_PAD_JTAG[0],    		// [HARD_DIS_JTAG] Set this bit to disable JTAG in the hard way. JTAG is disabled permanently
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    &DIS_DOWNLOAD_MANUAL_ENCRYPT[0],    		// [] Set this bit to disable flash encryption when in download boot modes
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USB_EXCHG_PINS[] = {
    &USB_EXCHG_PINS[0],    		// [] Set this bit to exchange USB D+ and D- pins
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USB_EXT_PHY_ENABLE[] = {
    &USB_EXT_PHY_ENABLE[0],    		// [EXT_PHY_ENABLE] Set this bit to enable external PHY
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_VDD_SPI_XPD[] = {
    &VDD_SPI_XPD[0],    		// [] SPI regulator power up signal
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_VDD_SPI_TIEH[] = {
    &VDD_SPI_TIEH[0],    		// [] If VDD_SPI_FORCE is 1; determines VDD_SPI voltage {0: "VDD_SPI connects to 1.8 V LDO"; 1: "VDD_SPI connects to VDD3P3_RTC_IO"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_VDD_SPI_FORCE[] = {
    &VDD_SPI_FORCE[0],    		// [] Set this bit and force to use the configuration of eFuse to configure VDD_SPI
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WDT_DELAY_SEL[] = {
    &WDT_DELAY_SEL[0],    		// [] RTC watchdog timeout threshold; in unit of slow clock cycle {0: "40000"; 1: "80000"; 2: "160000"; 3: "320000"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_BOOT_CRYPT_CNT[] = {
    &SPI_BOOT_CRYPT_CNT[0],    		// [] Enables flash encryption when 1 or 3 bits are set and disabled otherwise {0: "Disable"; 1: "Enable"; 3: "Disable"; 7: "Enable"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_KEY_REVOKE0[] = {
    &SECURE_BOOT_KEY_REVOKE0[0],    		// [] Revoke 1st secure boot key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_KEY_REVOKE1[] = {
    &SECURE_BOOT_KEY_REVOKE1[0],    		// [] Revoke 2nd secure boot key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_KEY_REVOKE2[] = {
    &SECURE_BOOT_KEY_REVOKE2[0],    		// [] Revoke 3rd secure boot key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_0[] = {
    &KEY_PURPOSE_0[0],    		// [KEY0_PURPOSE] Purpose of Key0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_1[] = {
    &KEY_PURPOSE_1[0],    		// [KEY1_PURPOSE] Purpose of Key1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_2[] = {
    &KEY_PURPOSE_2[0],    		// [KEY2_PURPOSE] Purpose of Key2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_3[] = {
    &KEY_PURPOSE_3[0],    		// [KEY3_PURPOSE] Purpose of Key3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_4[] = {
    &KEY_PURPOSE_4[0],    		// [KEY4_PURPOSE] Purpose of Key4
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_5[] = {
    &KEY_PURPOSE_5[0],    		// [KEY5_PURPOSE] Purpose of Key5
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_EN[] = {
    &SECURE_BOOT_EN[0],    		// [] Set this bit to enable secure boot
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE[] = {
    &SECURE_BOOT_AGGRESSIVE_REVOKE[0],    		// [] Set this bit to enable revoking aggressive secure boot
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_JTAG[] = {
    &DIS_USB_JTAG[0],    		// [] Set this bit to disable function of usb switch to jtag in module of usb device
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_SERIAL_JTAG[] = {
    &DIS_USB_SERIAL_JTAG[0],    		// [DIS_USB_DEVICE] Set this bit to disable usb device
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_STRAP_JTAG_SEL[] = {
    &STRAP_JTAG_SEL[0],    		// [] Set this bit to enable selection between usb_to_jtag and pad_to_jtag through strapping gpio10 when both reg_dis_usb_jtag and reg_dis_pad_jtag are equal to 0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USB_PHY_SEL[] = {
    &USB_PHY_SEL[0],    		// [] This bit is used to switch internal PHY and external PHY for USB OTG and USB Device {0: "internal PHY is assigned to USB Device while external PHY is assigned to USB OTG"; 1: "internal PHY is assigned to USB OTG while external PHY is assigned to USB Device"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_TPUW[] = {
    &FLASH_TPUW[0],    		// [] Configures flash waiting time after power-up; in unit of ms. If the value is less than 15; the waiting time is the configurable value.  Otherwise; the waiting time is twice the configurable value
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MODE[] = {
    &DIS_DOWNLOAD_MODE[0],    		// [] Set this bit to disable download mode (boot_mode[3:0] = 0; 1; 2; 3; 6; 7)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DIRECT_BOOT[] = {
    &DIS_DIRECT_BOOT[0],    		// [DIS_LEGACY_SPI_BOOT] Disable direct boot mode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT[] = {
    &DIS_USB_SERIAL_JTAG_ROM_PRINT[0],    		// [UART_PRINT_CHANNEL] USB printing {0: "Enable"; 1: "Disable"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_ECC_MODE[] = {
    &FLASH_ECC_MODE[0],    		// [] Flash ECC mode in ROM {0: "16to18 byte"; 1: "16to17 byte"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[] = {
    &DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[0],    		// [DIS_USB_DOWNLOAD_MODE] Set this bit to disable UART download mode through USB
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ENABLE_SECURITY_DOWNLOAD[] = {
    &ENABLE_SECURITY_DOWNLOAD[0],    		// [] Set this bit to enable secure UART download mode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_UART_PRINT_CONTROL[] = {
    &UART_PRINT_CONTROL[0],    		// [] Set the default UART boot message output mode {0: "Enable"; 1: "Enable when GPIO46 is low at reset"; 2: "Enable when GPIO46 is high at reset"; 3: "Disable"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PIN_POWER_SELECTION[] = {
    &PIN_POWER_SELECTION[0],    		// [] Set default power supply for GPIO33-GPIO37; set when SPI flash is initialized {0: "VDD3P3_CPU"; 1: "VDD_SPI"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_TYPE[] = {
    &FLASH_TYPE[0],    		// [] SPI flash type {0: "4 data lines"; 1: "8 data lines"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_PAGE_SIZE[] = {
    &FLASH_PAGE_SIZE[0],    		// [] Set Flash page size
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_ECC_EN[] = {
    &FLASH_ECC_EN[0],    		// [] Set 1 to enable ECC for flash boot
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FORCE_SEND_RESUME[] = {
    &FORCE_SEND_RESUME[0],    		// [] Set this bit to force ROM code to send a resume command during SPI boot
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_VERSION[] = {
    &SECURE_VERSION[0],    		// [] Secure version (used by ESP-IDF anti-rollback feature)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_OTG_DOWNLOAD_MODE[] = {
    &DIS_USB_OTG_DOWNLOAD_MODE[0],    		// [] Set this bit to disable download through USB-OTG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DISABLE_WAFER_VERSION_MAJOR[] = {
    &DISABLE_WAFER_VERSION_MAJOR[0],    		// [] Disables check of wafer version major
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DISABLE_BLK_VERSION_MAJOR[] = {
    &DISABLE_BLK_VERSION_MAJOR[0],    		// [] Disables check of blk version major
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_MAC[] = {
    &MAC[0],    		// [MAC_FACTORY] MAC address
    &MAC[1],    		// [MAC_FACTORY] MAC address
    &MAC[2],    		// [MAC_FACTORY] MAC address
    &MAC[3],    		// [MAC_FACTORY] MAC address
    &MAC[4],    		// [MAC_FACTORY] MAC address
    &MAC[5],    		// [MAC_FACTORY] MAC address
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_CLK[] = {
    &SPI_PAD_CONFIG_CLK[0],    		// [] SPI_PAD_configure CLK
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_Q[] = {
    &SPI_PAD_CONFIG_Q[0],    		// [] SPI_PAD_configure Q(D1)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_D[] = {
    &SPI_PAD_CONFIG_D[0],    		// [] SPI_PAD_configure D(D0)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_CS[] = {
    &SPI_PAD_CONFIG_CS[0],    		// [] SPI_PAD_configure CS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_HD[] = {
    &SPI_PAD_CONFIG_HD[0],    		// [] SPI_PAD_configure HD(D3)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_WP[] = {
    &SPI_PAD_CONFIG_WP[0],    		// [] SPI_PAD_configure WP(D2)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_DQS[] = {
    &SPI_PAD_CONFIG_DQS[0],    		// [] SPI_PAD_configure DQS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_D4[] = {
    &SPI_PAD_CONFIG_D4[0],    		// [] SPI_PAD_configure D4
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_D5[] = {
    &SPI_PAD_CONFIG_D5[0],    		// [] SPI_PAD_configure D5
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_D6[] = {
    &SPI_PAD_CONFIG_D6[0],    		// [] SPI_PAD_configure D6
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_D7[] = {
    &SPI_PAD_CONFIG_D7[0],    		// [] SPI_PAD_configure D7
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WAFER_VERSION_MINOR_LO[] = {
    &WAFER_VERSION_MINOR_LO[0],    		// [] WAFER_VERSION_MINOR least significant bits
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PKG_VERSION[] = {
    &PKG_VERSION[0],    		// [] Package version
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_BLK_VERSION_MINOR[] = {
    &BLK_VERSION_MINOR[0],    		// [] BLK_VERSION_MINOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_CAP[] = {
    &FLASH_CAP[0],    		// [] Flash capacity {0: "None"; 1: "8M"; 2: "4M"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_TEMP[] = {
    &FLASH_TEMP[0],    		// [] Flash temperature {0: "None"; 1: "105C"; 2: "85C"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_VENDOR[] = {
    &FLASH_VENDOR[0],    		// [] Flash vendor {0: "None"; 1: "XMC"; 2: "GD"; 3: "FM"; 4: "TT"; 5: "BY"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PSRAM_CAP[] = {
    &PSRAM_CAP[0],    		// [] PSRAM capacity {0: "None"; 1: "8M"; 2: "2M"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PSRAM_TEMP[] = {
    &PSRAM_TEMP[0],    		// [] PSRAM temperature {0: "None"; 1: "105C"; 2: "85C"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PSRAM_VENDOR[] = {
    &PSRAM_VENDOR[0],    		// [] PSRAM vendor {0: "None"; 1: "AP_3v3"; 2: "AP_1v8"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_K_RTC_LDO[] = {
    &K_RTC_LDO[0],    		// [] BLOCK1 K_RTC_LDO
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_K_DIG_LDO[] = {
    &K_DIG_LDO[0],    		// [] BLOCK1 K_DIG_LDO
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_V_RTC_DBIAS20[] = {
    &V_RTC_DBIAS20[0],    		// [] BLOCK1 voltage of rtc dbias20
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_V_DIG_DBIAS20[] = {
    &V_DIG_DBIAS20[0],    		// [] BLOCK1 voltage of digital dbias20
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIG_DBIAS_HVT[] = {
    &DIG_DBIAS_HVT[0],    		// [] BLOCK1 digital dbias when hvt
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WAFER_VERSION_MINOR_HI[] = {
    &WAFER_VERSION_MINOR_HI[0],    		// [] WAFER_VERSION_MINOR most significant bit
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WAFER_VERSION_MAJOR[] = {
    &WAFER_VERSION_MAJOR[0],    		// [] WAFER_VERSION_MAJOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_CAL_VOL_ATTEN3[] = {
    &ADC2_CAL_VOL_ATTEN3[0],    		// [] ADC2 calibration voltage at atten3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_OPTIONAL_UNIQUE_ID[] = {
    &OPTIONAL_UNIQUE_ID[0],    		// [] Optional unique 128-bit ID
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_BLK_VERSION_MAJOR[] = {
    &BLK_VERSION_MAJOR[0],    		// [] BLK_VERSION_MAJOR of BLOCK2 {0: "No calib"; 1: "ADC calib V1"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_TEMP_CALIB[] = {
    &TEMP_CALIB[0],    		// [] Temperature calibration data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_OCODE[] = {
    &OCODE[0],    		// [] ADC OCode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_INIT_CODE_ATTEN0[] = {
    &ADC1_INIT_CODE_ATTEN0[0],    		// [] ADC1 init code at atten0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_INIT_CODE_ATTEN1[] = {
    &ADC1_INIT_CODE_ATTEN1[0],    		// [] ADC1 init code at atten1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_INIT_CODE_ATTEN2[] = {
    &ADC1_INIT_CODE_ATTEN2[0],    		// [] ADC1 init code at atten2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_INIT_CODE_ATTEN3[] = {
    &ADC1_INIT_CODE_ATTEN3[0],    		// [] ADC1 init code at atten3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_INIT_CODE_ATTEN0[] = {
    &ADC2_INIT_CODE_ATTEN0[0],    		// [] ADC2 init code at atten0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_INIT_CODE_ATTEN1[] = {
    &ADC2_INIT_CODE_ATTEN1[0],    		// [] ADC2 init code at atten1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_INIT_CODE_ATTEN2[] = {
    &ADC2_INIT_CODE_ATTEN2[0],    		// [] ADC2 init code at atten2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_INIT_CODE_ATTEN3[] = {
    &ADC2_INIT_CODE_ATTEN3[0],    		// [] ADC2 init code at atten3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CAL_VOL_ATTEN0[] = {
    &ADC1_CAL_VOL_ATTEN0[0],    		// [] ADC1 calibration voltage at atten0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CAL_VOL_ATTEN1[] = {
    &ADC1_CAL_VOL_ATTEN1[0],    		// [] ADC1 calibration voltage at atten1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CAL_VOL_ATTEN2[] = {
    &ADC1_CAL_VOL_ATTEN2[0],    		// [] ADC1 calibration voltage at atten2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CAL_VOL_ATTEN3[] = {
    &ADC1_CAL_VOL_ATTEN3[0],    		// [] ADC1 calibration voltage at atten3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_CAL_VOL_ATTEN0[] = {
    &ADC2_CAL_VOL_ATTEN0[0],    		// [] ADC2 calibration voltage at atten0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_CAL_VOL_ATTEN1[] = {
    &ADC2_CAL_VOL_ATTEN1[0],    		// [] ADC2 calibration voltage at atten1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_CAL_VOL_ATTEN2[] = {
    &ADC2_CAL_VOL_ATTEN2[0],    		// [] ADC2 calibration voltage at atten2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USER_DATA[] = {
    &USER_DATA[0],    		// [BLOCK_USR_DATA] User data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USER_DATA_MAC_CUSTOM[] = {
    &USER_DATA_MAC_CUSTOM[0],    		// [MAC_CUSTOM CUSTOM_MAC] Custom MAC
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY0[] = {
    &KEY0[0],    		// [BLOCK_KEY0] Key0 or user data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY1[] = {
    &KEY1[0],    		// [BLOCK_KEY1] Key1 or user data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY2[] = {
    &KEY2[0],    		// [BLOCK_KEY2] Key2 or user data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY3[] = {
    &KEY3[0],    		// [BLOCK_KEY3] Key3 or user data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY4[] = {
    &KEY4[0],    		// [BLOCK_KEY4] Key4 or user data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY5[] = {
    &KEY5[0],    		// [BLOCK_KEY5] Key5 or user data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SYS_DATA_PART2[] = {
    &SYS_DATA_PART2[0],    		// [BLOCK_SYS_DATA2] System data part 2 (reserved)
    NULL
};
