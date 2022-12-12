/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_efuse.h"
#include <assert.h>
#include "esp_efuse_table.h"

// md5_digest_table ceedae45d1a885ced865a05eeca7d7ee
// This file was generated from the file esp_efuse_table.csv. DO NOT CHANGE THIS FILE MANUALLY.
// If you want to change some fields, you need to change esp_efuse_table.csv file
// then run `efuse_common_table` or `efuse_custom_table` command it will generate this file.
// To show efuse_table run the command 'show_efuse_table'.

static const esp_efuse_desc_t WR_DIS[] = {
    {EFUSE_BLK0, 0, 8}, 	 // Write protection,
};

static const esp_efuse_desc_t WR_DIS_KEY0_RD_DIS[] = {
    {EFUSE_BLK0, 0, 1}, 	 // Write protection for KEY0_RD_DIS,
};

static const esp_efuse_desc_t WR_DIS_GROUP_1[] = {
    {EFUSE_BLK0, 1, 1}, 	 // Write protection for WDT_DELAY DIS_PAD_JTAG DIS_DOWNLOAD_ICACHE,
};

static const esp_efuse_desc_t WR_DIS_GROUP_2[] = {
    {EFUSE_BLK0, 2, 1}, 	 // Write protection for DOWNLOAD_DIS_MANUAL_ENCRYPT SPI_BOOT_CRYPT_CNT XTS_KEY_LENGTH_256 SECURE_BOOT_EN,
};

static const esp_efuse_desc_t WR_DIS_SPI_BOOT_CRYPT_CNT[] = {
    {EFUSE_BLK0, 2, 1}, 	 // Write protection for DOWNLOAD_DIS_MANUAL_ENCRYPT [SPI_BOOT_CRYPT_CNT] XTS_KEY_LENGTH_256 SECURE_BOOT_EN,
};

static const esp_efuse_desc_t WR_DIS_GROUP_3[] = {
    {EFUSE_BLK0, 3, 1}, 	 // Write protection for UART_PRINT_CONTROL FORCE_SEND_RESUME DIS_DOWNLOAD_MODE DIS_DIRECT_BOOT ENABLE_SECURITY_DOWNLOAD FLASH_TPUW,
};

static const esp_efuse_desc_t WR_DIS_BLK0_RESERVED[] = {
    {EFUSE_BLK0, 4, 1}, 	 // Write protection for BLK0_RESERVED,
};

static const esp_efuse_desc_t WR_DIS_SYS_DATA_PART0[] = {
    {EFUSE_BLK0, 5, 1}, 	 // Write protection for EFUSE_BLK1.  SYS_DATA_PART0,
};

static const esp_efuse_desc_t WR_DIS_SYS_DATA_PART1[] = {
    {EFUSE_BLK0, 6, 1}, 	 // Write protection for EFUSE_BLK2.  SYS_DATA_PART2,
};

static const esp_efuse_desc_t WR_DIS_KEY0[] = {
    {EFUSE_BLK0, 7, 1}, 	 // Write protection for EFUSE_BLK3.  whole KEY0,
};

static const esp_efuse_desc_t RD_DIS[] = {
    {EFUSE_BLK0, 32, 2}, 	 // Read protection,
};

static const esp_efuse_desc_t RD_DIS_KEY0[] = {
    {EFUSE_BLK0, 32, 2}, 	 // Read protection for EFUSE_BLK3.  KEY0,
};

static const esp_efuse_desc_t RD_DIS_KEY0_LOW[] = {
    {EFUSE_BLK0, 32, 1}, 	 // Read protection for EFUSE_BLK3.  KEY0 lower 128-bit key,
};

static const esp_efuse_desc_t RD_DIS_KEY0_HI[] = {
    {EFUSE_BLK0, 33, 1}, 	 // Read protection for EFUSE_BLK3.  KEY0 higher 128-bit key,
};

static const esp_efuse_desc_t WDT_DELAY_SEL[] = {
    {EFUSE_BLK0, 34, 2}, 	 // RTC WDT timeout threshold,
};

static const esp_efuse_desc_t DIS_PAD_JTAG[] = {
    {EFUSE_BLK0, 36, 1}, 	 // Hardware Disable JTAG permanently,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_ICACHE[] = {
    {EFUSE_BLK0, 37, 1}, 	 // Disable ICache in Download mode,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    {EFUSE_BLK0, 38, 1}, 	 // Disable flash encryption in Download boot mode,
};

static const esp_efuse_desc_t SPI_BOOT_CRYPT_CNT[] = {
    {EFUSE_BLK0, 39, 3}, 	 // Enable SPI boot encrypt/decrypt. Odd number: enable; even number: disable,
};

static const esp_efuse_desc_t XTS_KEY_LENGTH_256[] = {
    {EFUSE_BLK0, 42, 1}, 	 // Select XTS_AES key length. 1: 256-bit of whole block3; 0: Lower 128-bit of block3,
};

static const esp_efuse_desc_t UART_PRINT_CONTROL[] = {
    {EFUSE_BLK0, 43, 2}, 	 // Set UART boot message output mode. 00: Force print; 01: Low-level print controlled by GPIO 8; 10: High-level print controlled by GPIO 8; 11: Print force disabled,
};

static const esp_efuse_desc_t FORCE_SEND_RESUME[] = {
    {EFUSE_BLK0, 45, 1}, 	 // Force ROM code to send an SPI flash resume command during SPI boot,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 46, 1}, 	 // Disable all download boot modes,
};

static const esp_efuse_desc_t DIS_DIRECT_BOOT[] = {
    {EFUSE_BLK0, 47, 1}, 	 // Disable direct_boot mode,
};

static const esp_efuse_desc_t ENABLE_SECURITY_DOWNLOAD[] = {
    {EFUSE_BLK0, 48, 1}, 	 // Enable secure UART download mode,
};

static const esp_efuse_desc_t FLASH_TPUW[] = {
    {EFUSE_BLK0, 49, 4}, 	 // Configure flash startup delay after SoC being powered up (the unit is ms/2). When the value is 15 delay will be 7.5 ms,
};

static const esp_efuse_desc_t SECURE_BOOT_EN[] = {
    {EFUSE_BLK0, 53, 1}, 	 // Enable secure boot,
};

static const esp_efuse_desc_t SECURE_VERSION[] = {
    {EFUSE_BLK0, 54, 4}, 	 // Secure version for anti-rollback,
};

static const esp_efuse_desc_t ENABLE_CUSTOM_MAC[] = {
    {EFUSE_BLK0, 58, 1}, 	 // True if MAC_CUSTOM is burned,
};

static const esp_efuse_desc_t DISABLE_WAFER_VERSION_MAJOR[] = {
    {EFUSE_BLK0, 59, 1}, 	 // Disables check of wafer version major,
};

static const esp_efuse_desc_t DISABLE_BLK_VERSION_MAJOR[] = {
    {EFUSE_BLK0, 60, 1}, 	 // Disables check of blk version major,
};

static const esp_efuse_desc_t USER_DATA[] = {
    {EFUSE_BLK1, 0, 88}, 	 // User data block,
};

static const esp_efuse_desc_t USER_DATA_MAC_CUSTOM[] = {
    {EFUSE_BLK1, 0, 48}, 	 // Custom MAC addr,
};

static const esp_efuse_desc_t MAC_FACTORY[] = {
    {EFUSE_BLK2, 40, 8}, 	 // Factory MAC addr [0],
    {EFUSE_BLK2, 32, 8}, 	 // Factory MAC addr [1],
    {EFUSE_BLK2, 24, 8}, 	 // Factory MAC addr [2],
    {EFUSE_BLK2, 16, 8}, 	 // Factory MAC addr [3],
    {EFUSE_BLK2, 8, 8}, 	 // Factory MAC addr [4],
    {EFUSE_BLK2, 0, 8}, 	 // Factory MAC addr [5],
};

static const esp_efuse_desc_t WAFER_VERSION_MINOR[] = {
    {EFUSE_BLK2, 48, 4}, 	 // WAFER_VERSION_MINOR,
};

static const esp_efuse_desc_t WAFER_VERSION_MAJOR[] = {
    {EFUSE_BLK2, 52, 2}, 	 // WAFER_VERSION_MAJOR,
};

static const esp_efuse_desc_t PKG_VERSION[] = {
    {EFUSE_BLK2, 54, 3}, 	 // EFUSE_PKG_VERSION,
};

static const esp_efuse_desc_t BLK_VERSION_MINOR[] = {
    {EFUSE_BLK2, 57, 3}, 	 // BLK_VERSION_MINOR,
};

static const esp_efuse_desc_t BLK_VERSION_MAJOR[] = {
    {EFUSE_BLK2, 60, 2}, 	 // BLK_VERSION_MAJOR,
};

static const esp_efuse_desc_t KEY0[] = {
    {EFUSE_BLK3, 0, 256}, 	 // [256bit FE key] or [128bit FE key and 128key SB key] or [user data],
};

static const esp_efuse_desc_t KEY0_FE_256BIT[] = {
    {EFUSE_BLK3, 0, 256}, 	 // [256bit FE key],
};

static const esp_efuse_desc_t KEY0_FE_128BIT[] = {
    {EFUSE_BLK3, 0, 128}, 	 // [128bit FE key],
};

static const esp_efuse_desc_t KEY0_SB_128BIT[] = {
    {EFUSE_BLK3, 128, 128}, 	 // [128bit SB key],
};

static const esp_efuse_desc_t OCODE[] = {
    {EFUSE_BLK2, 62, 7}, 	 // OCode,
};

static const esp_efuse_desc_t TEMP_CALIB[] = {
    {EFUSE_BLK2, 69, 9}, 	 // Temperature calibration data,
};

static const esp_efuse_desc_t ADC1_INIT_CODE_ATTEN0[] = {
    {EFUSE_BLK2, 78, 8}, 	 // ADC1 init code at atten0,
};

static const esp_efuse_desc_t ADC1_INIT_CODE_ATTEN3[] = {
    {EFUSE_BLK2, 86, 5}, 	 // ADC1 init code at atten3,
};

static const esp_efuse_desc_t ADC1_CAL_VOL_ATTEN0[] = {
    {EFUSE_BLK2, 91, 8}, 	 // ADC1 calibration voltage at atten0,
};

static const esp_efuse_desc_t ADC1_CAL_VOL_ATTEN3[] = {
    {EFUSE_BLK2, 99, 6}, 	 // ADC1 calibration voltage at atten3,
};

static const esp_efuse_desc_t DIG_DBIAS_HVT[] = {
    {EFUSE_BLK2, 105, 5}, 	 // BLOCK2 digital dbias when hvt,
};

static const esp_efuse_desc_t DIG_LDO_SLP_DBIAS2[] = {
    {EFUSE_BLK2, 110, 7}, 	 // BLOCK2 DIG_LDO_DBG0_DBIAS2,
};

static const esp_efuse_desc_t DIG_LDO_SLP_DBIAS26[] = {
    {EFUSE_BLK2, 117, 8}, 	 // BLOCK2 DIG_LDO_DBG0_DBIAS26,
};

static const esp_efuse_desc_t DIG_LDO_ACT_DBIAS26[] = {
    {EFUSE_BLK2, 125, 6}, 	 // BLOCK2 DIG_LDO_ACT_DBIAS26,
};

static const esp_efuse_desc_t DIG_LDO_ACT_STEPD10[] = {
    {EFUSE_BLK2, 131, 4}, 	 // BLOCK2 DIG_LDO_ACT_STEPD10,
};

static const esp_efuse_desc_t RTC_LDO_SLP_DBIAS13[] = {
    {EFUSE_BLK2, 135, 7}, 	 // BLOCK2 DIG_LDO_SLP_DBIAS13,
};

static const esp_efuse_desc_t RTC_LDO_SLP_DBIAS29[] = {
    {EFUSE_BLK2, 142, 9}, 	 // BLOCK2 DIG_LDO_SLP_DBIAS29,
};

static const esp_efuse_desc_t RTC_LDO_SLP_DBIAS31[] = {
    {EFUSE_BLK2, 151, 6}, 	 // BLOCK2 DIG_LDO_SLP_DBIAS31,
};

static const esp_efuse_desc_t RTC_LDO_ACT_DBIAS31[] = {
    {EFUSE_BLK2, 157, 6}, 	 // BLOCK2 DIG_LDO_ACT_DBIAS31,
};

static const esp_efuse_desc_t RTC_LDO_ACT_DBIAS13[] = {
    {EFUSE_BLK2, 163, 8}, 	 // BLOCK2 DIG_LDO_ACT_DBIAS13,
};





const esp_efuse_desc_t* ESP_EFUSE_WR_DIS[] = {
    &WR_DIS[0],    		// Write protection
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY0_RD_DIS[] = {
    &WR_DIS_KEY0_RD_DIS[0],    		// Write protection for KEY0_RD_DIS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_GROUP_1[] = {
    &WR_DIS_GROUP_1[0],    		// Write protection for WDT_DELAY DIS_PAD_JTAG DIS_DOWNLOAD_ICACHE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_GROUP_2[] = {
    &WR_DIS_GROUP_2[0],    		// Write protection for DOWNLOAD_DIS_MANUAL_ENCRYPT SPI_BOOT_CRYPT_CNT XTS_KEY_LENGTH_256 SECURE_BOOT_EN
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_BOOT_CRYPT_CNT[] = {
    &WR_DIS_SPI_BOOT_CRYPT_CNT[0],    		// Write protection for DOWNLOAD_DIS_MANUAL_ENCRYPT [SPI_BOOT_CRYPT_CNT] XTS_KEY_LENGTH_256 SECURE_BOOT_EN
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_GROUP_3[] = {
    &WR_DIS_GROUP_3[0],    		// Write protection for UART_PRINT_CONTROL FORCE_SEND_RESUME DIS_DOWNLOAD_MODE DIS_DIRECT_BOOT ENABLE_SECURITY_DOWNLOAD FLASH_TPUW
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK0_RESERVED[] = {
    &WR_DIS_BLK0_RESERVED[0],    		// Write protection for BLK0_RESERVED
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SYS_DATA_PART0[] = {
    &WR_DIS_SYS_DATA_PART0[0],    		// Write protection for EFUSE_BLK1.  SYS_DATA_PART0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SYS_DATA_PART1[] = {
    &WR_DIS_SYS_DATA_PART1[0],    		// Write protection for EFUSE_BLK2.  SYS_DATA_PART2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY0[] = {
    &WR_DIS_KEY0[0],    		// Write protection for EFUSE_BLK3.  whole KEY0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS[] = {
    &RD_DIS[0],    		// Read protection
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY0[] = {
    &RD_DIS_KEY0[0],    		// Read protection for EFUSE_BLK3.  KEY0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY0_LOW[] = {
    &RD_DIS_KEY0_LOW[0],    		// Read protection for EFUSE_BLK3.  KEY0 lower 128-bit key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY0_HI[] = {
    &RD_DIS_KEY0_HI[0],    		// Read protection for EFUSE_BLK3.  KEY0 higher 128-bit key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WDT_DELAY_SEL[] = {
    &WDT_DELAY_SEL[0],    		// RTC WDT timeout threshold
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_PAD_JTAG[] = {
    &DIS_PAD_JTAG[0],    		// Hardware Disable JTAG permanently
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_ICACHE[] = {
    &DIS_DOWNLOAD_ICACHE[0],    		// Disable ICache in Download mode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    &DIS_DOWNLOAD_MANUAL_ENCRYPT[0],    		// Disable flash encryption in Download boot mode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_BOOT_CRYPT_CNT[] = {
    &SPI_BOOT_CRYPT_CNT[0],    		// Enable SPI boot encrypt/decrypt. Odd number: enable; even number: disable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_XTS_KEY_LENGTH_256[] = {
    &XTS_KEY_LENGTH_256[0],    		// Select XTS_AES key length. 1: 256-bit of whole block3; 0: Lower 128-bit of block3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_UART_PRINT_CONTROL[] = {
    &UART_PRINT_CONTROL[0],    		// Set UART boot message output mode. 00: Force print; 01: Low-level print controlled by GPIO 8; 10: High-level print controlled by GPIO 8; 11: Print force disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FORCE_SEND_RESUME[] = {
    &FORCE_SEND_RESUME[0],    		// Force ROM code to send an SPI flash resume command during SPI boot
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MODE[] = {
    &DIS_DOWNLOAD_MODE[0],    		// Disable all download boot modes
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DIRECT_BOOT[] = {
    &DIS_DIRECT_BOOT[0],    		// Disable direct_boot mode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ENABLE_SECURITY_DOWNLOAD[] = {
    &ENABLE_SECURITY_DOWNLOAD[0],    		// Enable secure UART download mode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_TPUW[] = {
    &FLASH_TPUW[0],    		// Configure flash startup delay after SoC being powered up (the unit is ms/2). When the value is 15 delay will be 7.5 ms
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_EN[] = {
    &SECURE_BOOT_EN[0],    		// Enable secure boot
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_VERSION[] = {
    &SECURE_VERSION[0],    		// Secure version for anti-rollback
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ENABLE_CUSTOM_MAC[] = {
    &ENABLE_CUSTOM_MAC[0],    		// True if MAC_CUSTOM is burned
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DISABLE_WAFER_VERSION_MAJOR[] = {
    &DISABLE_WAFER_VERSION_MAJOR[0],    		// Disables check of wafer version major
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DISABLE_BLK_VERSION_MAJOR[] = {
    &DISABLE_BLK_VERSION_MAJOR[0],    		// Disables check of blk version major
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USER_DATA[] = {
    &USER_DATA[0],    		// User data block
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USER_DATA_MAC_CUSTOM[] = {
    &USER_DATA_MAC_CUSTOM[0],    		// Custom MAC addr
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_MAC_FACTORY[] = {
    &MAC_FACTORY[0],    		// Factory MAC addr [0]
    &MAC_FACTORY[1],    		// Factory MAC addr [1]
    &MAC_FACTORY[2],    		// Factory MAC addr [2]
    &MAC_FACTORY[3],    		// Factory MAC addr [3]
    &MAC_FACTORY[4],    		// Factory MAC addr [4]
    &MAC_FACTORY[5],    		// Factory MAC addr [5]
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WAFER_VERSION_MINOR[] = {
    &WAFER_VERSION_MINOR[0],    		// WAFER_VERSION_MINOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WAFER_VERSION_MAJOR[] = {
    &WAFER_VERSION_MAJOR[0],    		// WAFER_VERSION_MAJOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PKG_VERSION[] = {
    &PKG_VERSION[0],    		// EFUSE_PKG_VERSION
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_BLK_VERSION_MINOR[] = {
    &BLK_VERSION_MINOR[0],    		// BLK_VERSION_MINOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_BLK_VERSION_MAJOR[] = {
    &BLK_VERSION_MAJOR[0],    		// BLK_VERSION_MAJOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY0[] = {
    &KEY0[0],    		// [256bit FE key] or [128bit FE key and 128key SB key] or [user data]
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY0_FE_256BIT[] = {
    &KEY0_FE_256BIT[0],    		// [256bit FE key]
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY0_FE_128BIT[] = {
    &KEY0_FE_128BIT[0],    		// [128bit FE key]
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY0_SB_128BIT[] = {
    &KEY0_SB_128BIT[0],    		// [128bit SB key]
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_OCODE[] = {
    &OCODE[0],    		// OCode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_TEMP_CALIB[] = {
    &TEMP_CALIB[0],    		// Temperature calibration data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_INIT_CODE_ATTEN0[] = {
    &ADC1_INIT_CODE_ATTEN0[0],    		// ADC1 init code at atten0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_INIT_CODE_ATTEN3[] = {
    &ADC1_INIT_CODE_ATTEN3[0],    		// ADC1 init code at atten3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CAL_VOL_ATTEN0[] = {
    &ADC1_CAL_VOL_ATTEN0[0],    		// ADC1 calibration voltage at atten0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CAL_VOL_ATTEN3[] = {
    &ADC1_CAL_VOL_ATTEN3[0],    		// ADC1 calibration voltage at atten3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIG_DBIAS_HVT[] = {
    &DIG_DBIAS_HVT[0],    		// BLOCK2 digital dbias when hvt
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIG_LDO_SLP_DBIAS2[] = {
    &DIG_LDO_SLP_DBIAS2[0],    		// BLOCK2 DIG_LDO_DBG0_DBIAS2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIG_LDO_SLP_DBIAS26[] = {
    &DIG_LDO_SLP_DBIAS26[0],    		// BLOCK2 DIG_LDO_DBG0_DBIAS26
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIG_LDO_ACT_DBIAS26[] = {
    &DIG_LDO_ACT_DBIAS26[0],    		// BLOCK2 DIG_LDO_ACT_DBIAS26
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIG_LDO_ACT_STEPD10[] = {
    &DIG_LDO_ACT_STEPD10[0],    		// BLOCK2 DIG_LDO_ACT_STEPD10
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RTC_LDO_SLP_DBIAS13[] = {
    &RTC_LDO_SLP_DBIAS13[0],    		// BLOCK2 DIG_LDO_SLP_DBIAS13
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RTC_LDO_SLP_DBIAS29[] = {
    &RTC_LDO_SLP_DBIAS29[0],    		// BLOCK2 DIG_LDO_SLP_DBIAS29
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RTC_LDO_SLP_DBIAS31[] = {
    &RTC_LDO_SLP_DBIAS31[0],    		// BLOCK2 DIG_LDO_SLP_DBIAS31
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RTC_LDO_ACT_DBIAS31[] = {
    &RTC_LDO_ACT_DBIAS31[0],    		// BLOCK2 DIG_LDO_ACT_DBIAS31
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RTC_LDO_ACT_DBIAS13[] = {
    &RTC_LDO_ACT_DBIAS13[0],    		// BLOCK2 DIG_LDO_ACT_DBIAS13
    NULL
};
