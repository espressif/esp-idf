/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_efuse.h"
#include <assert.h>
#include "esp_efuse_table.h"

// md5_digest_table af57e8a6a405ebf239cc552f713c91d0
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

static const esp_efuse_desc_t WAFER_VERSION[] = {
    {EFUSE_BLK2, 48, 3}, 	 // EFUSE_WAFER_VERSION,
};

static const esp_efuse_desc_t PKG_VERSION[] = {
    {EFUSE_BLK2, 51, 3}, 	 // EFUSE_PKG_VERSION,
};

static const esp_efuse_desc_t BLOCK2_VERSION[] = {
    {EFUSE_BLK2, 54, 3}, 	 // EFUSE_BLOCK2_VERSION,
};

static const esp_efuse_desc_t RF_REF_I_BIAS_CONFIG[] = {
    {EFUSE_BLK2, 57, 4}, 	 // EFUSE_RF_REF_I_BIAS_CONFIG,
};

static const esp_efuse_desc_t LDO_VOL_BIAS_CONFIG_LOW[] = {
    {EFUSE_BLK2, 61, 3}, 	 // EFUSE_LDO_VOL_BIAS_CONFIG_LOW,
};

static const esp_efuse_desc_t LDO_VOL_BIAS_CONFIG_HIGH[] = {
    {EFUSE_BLK2, 64, 27}, 	 // EFUSE_LDO_VOL_BIAS_CONFIG_HIGH,
};

static const esp_efuse_desc_t PVT_LOW[] = {
    {EFUSE_BLK2, 91, 5}, 	 // EFUSE_PVT_LOW,
};

static const esp_efuse_desc_t PVT_HIGH[] = {
    {EFUSE_BLK2, 96, 10}, 	 // EFUSE_PVT_HIGH,
};

static const esp_efuse_desc_t ADC_CALIBRATION_0[] = {
    {EFUSE_BLK2, 106, 22}, 	 // EFUSE_ADC_CALIBRATION_0,
};

static const esp_efuse_desc_t ADC_CALIBRATION_1[] = {
    {EFUSE_BLK2, 128, 32}, 	 // EFUSE_ADC_CALIBRATION_1,
};

static const esp_efuse_desc_t ADC_CALIBRATION_2[] = {
    {EFUSE_BLK2, 160, 32}, 	 // EFUSE_ADC_CALIBRATION_2,
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

const esp_efuse_desc_t* ESP_EFUSE_WAFER_VERSION[] = {
    &WAFER_VERSION[0],    		// EFUSE_WAFER_VERSION
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PKG_VERSION[] = {
    &PKG_VERSION[0],    		// EFUSE_PKG_VERSION
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_BLOCK2_VERSION[] = {
    &BLOCK2_VERSION[0],    		// EFUSE_BLOCK2_VERSION
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RF_REF_I_BIAS_CONFIG[] = {
    &RF_REF_I_BIAS_CONFIG[0],    		// EFUSE_RF_REF_I_BIAS_CONFIG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_LDO_VOL_BIAS_CONFIG_LOW[] = {
    &LDO_VOL_BIAS_CONFIG_LOW[0],    		// EFUSE_LDO_VOL_BIAS_CONFIG_LOW
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_LDO_VOL_BIAS_CONFIG_HIGH[] = {
    &LDO_VOL_BIAS_CONFIG_HIGH[0],    		// EFUSE_LDO_VOL_BIAS_CONFIG_HIGH
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PVT_LOW[] = {
    &PVT_LOW[0],    		// EFUSE_PVT_LOW
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PVT_HIGH[] = {
    &PVT_HIGH[0],    		// EFUSE_PVT_HIGH
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC_CALIBRATION_0[] = {
    &ADC_CALIBRATION_0[0],    		// EFUSE_ADC_CALIBRATION_0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC_CALIBRATION_1[] = {
    &ADC_CALIBRATION_1[0],    		// EFUSE_ADC_CALIBRATION_1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC_CALIBRATION_2[] = {
    &ADC_CALIBRATION_2[0],    		// EFUSE_ADC_CALIBRATION_2
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
