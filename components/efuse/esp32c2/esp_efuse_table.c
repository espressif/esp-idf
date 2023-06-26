/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_efuse.h"
#include <assert.h>
#include "esp_efuse_table.h"

// md5_digest_table 439495cbc35dc68d7566e05ac3dbb248
// This file was generated from the file esp_efuse_table.csv. DO NOT CHANGE THIS FILE MANUALLY.
// If you want to change some fields, you need to change esp_efuse_table.csv file
// then run `efuse_common_table` or `efuse_custom_table` command it will generate this file.
// To show efuse_table run the command 'show_efuse_table'.

static const esp_efuse_desc_t WR_DIS[] = {
    {EFUSE_BLK0, 0, 8}, 	 // [] Disable programming of individual eFuses,
};

static const esp_efuse_desc_t WR_DIS_RD_DIS[] = {
    {EFUSE_BLK0, 0, 1}, 	 // [] wr_dis of RD_DIS,
};

static const esp_efuse_desc_t WR_DIS_WDT_DELAY_SEL[] = {
    {EFUSE_BLK0, 1, 1}, 	 // [] wr_dis of WDT_DELAY_SEL,
};

static const esp_efuse_desc_t WR_DIS_DIS_PAD_JTAG[] = {
    {EFUSE_BLK0, 1, 1}, 	 // [] wr_dis of DIS_PAD_JTAG,
};

static const esp_efuse_desc_t WR_DIS_DIS_DOWNLOAD_ICACHE[] = {
    {EFUSE_BLK0, 1, 1}, 	 // [] wr_dis of DIS_DOWNLOAD_ICACHE,
};

static const esp_efuse_desc_t WR_DIS_DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of DIS_DOWNLOAD_MANUAL_ENCRYPT,
};

static const esp_efuse_desc_t WR_DIS_SPI_BOOT_CRYPT_CNT[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of SPI_BOOT_CRYPT_CNT,
};

static const esp_efuse_desc_t WR_DIS_XTS_KEY_LENGTH_256[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of XTS_KEY_LENGTH_256,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_EN[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of SECURE_BOOT_EN,
};

static const esp_efuse_desc_t WR_DIS_UART_PRINT_CONTROL[] = {
    {EFUSE_BLK0, 3, 1}, 	 // [] wr_dis of UART_PRINT_CONTROL,
};

static const esp_efuse_desc_t WR_DIS_FORCE_SEND_RESUME[] = {
    {EFUSE_BLK0, 3, 1}, 	 // [] wr_dis of FORCE_SEND_RESUME,
};

static const esp_efuse_desc_t WR_DIS_DIS_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 3, 1}, 	 // [] wr_dis of DIS_DOWNLOAD_MODE,
};

static const esp_efuse_desc_t WR_DIS_DIS_DIRECT_BOOT[] = {
    {EFUSE_BLK0, 3, 1}, 	 // [] wr_dis of DIS_DIRECT_BOOT,
};

static const esp_efuse_desc_t WR_DIS_ENABLE_SECURITY_DOWNLOAD[] = {
    {EFUSE_BLK0, 3, 1}, 	 // [] wr_dis of ENABLE_SECURITY_DOWNLOAD,
};

static const esp_efuse_desc_t WR_DIS_FLASH_TPUW[] = {
    {EFUSE_BLK0, 3, 1}, 	 // [] wr_dis of FLASH_TPUW,
};

static const esp_efuse_desc_t WR_DIS_SECURE_VERSION[] = {
    {EFUSE_BLK0, 4, 1}, 	 // [] wr_dis of SECURE_VERSION,
};

static const esp_efuse_desc_t WR_DIS_CUSTOM_MAC_USED[] = {
    {EFUSE_BLK0, 4, 1}, 	 // [WR_DIS.ENABLE_CUSTOM_MAC] wr_dis of CUSTOM_MAC_USED,
};

static const esp_efuse_desc_t WR_DIS_DISABLE_WAFER_VERSION_MAJOR[] = {
    {EFUSE_BLK0, 4, 1}, 	 // [] wr_dis of DISABLE_WAFER_VERSION_MAJOR,
};

static const esp_efuse_desc_t WR_DIS_DISABLE_BLK_VERSION_MAJOR[] = {
    {EFUSE_BLK0, 4, 1}, 	 // [] wr_dis of DISABLE_BLK_VERSION_MAJOR,
};

static const esp_efuse_desc_t WR_DIS_CUSTOM_MAC[] = {
    {EFUSE_BLK0, 5, 1}, 	 // [WR_DIS.MAC_CUSTOM WR_DIS.USER_DATA_MAC_CUSTOM] wr_dis of CUSTOM_MAC,
};

static const esp_efuse_desc_t WR_DIS_MAC[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [WR_DIS.MAC_FACTORY] wr_dis of MAC,
};

static const esp_efuse_desc_t WR_DIS_WAFER_VERSION_MINOR[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of WAFER_VERSION_MINOR,
};

static const esp_efuse_desc_t WR_DIS_WAFER_VERSION_MAJOR[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of WAFER_VERSION_MAJOR,
};

static const esp_efuse_desc_t WR_DIS_PKG_VERSION[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of PKG_VERSION,
};

static const esp_efuse_desc_t WR_DIS_BLK_VERSION_MINOR[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of BLK_VERSION_MINOR,
};

static const esp_efuse_desc_t WR_DIS_BLK_VERSION_MAJOR[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of BLK_VERSION_MAJOR,
};

static const esp_efuse_desc_t WR_DIS_OCODE[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of OCODE,
};

static const esp_efuse_desc_t WR_DIS_TEMP_CALIB[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of TEMP_CALIB,
};

static const esp_efuse_desc_t WR_DIS_ADC1_INIT_CODE_ATTEN0[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of ADC1_INIT_CODE_ATTEN0,
};

static const esp_efuse_desc_t WR_DIS_ADC1_INIT_CODE_ATTEN3[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of ADC1_INIT_CODE_ATTEN3,
};

static const esp_efuse_desc_t WR_DIS_ADC1_CAL_VOL_ATTEN0[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of ADC1_CAL_VOL_ATTEN0,
};

static const esp_efuse_desc_t WR_DIS_ADC1_CAL_VOL_ATTEN3[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of ADC1_CAL_VOL_ATTEN3,
};

static const esp_efuse_desc_t WR_DIS_DIG_DBIAS_HVT[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of DIG_DBIAS_HVT,
};

static const esp_efuse_desc_t WR_DIS_DIG_LDO_SLP_DBIAS2[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of DIG_LDO_SLP_DBIAS2,
};

static const esp_efuse_desc_t WR_DIS_DIG_LDO_SLP_DBIAS26[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of DIG_LDO_SLP_DBIAS26,
};

static const esp_efuse_desc_t WR_DIS_DIG_LDO_ACT_DBIAS26[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of DIG_LDO_ACT_DBIAS26,
};

static const esp_efuse_desc_t WR_DIS_DIG_LDO_ACT_STEPD10[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of DIG_LDO_ACT_STEPD10,
};

static const esp_efuse_desc_t WR_DIS_RTC_LDO_SLP_DBIAS13[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of RTC_LDO_SLP_DBIAS13,
};

static const esp_efuse_desc_t WR_DIS_RTC_LDO_SLP_DBIAS29[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of RTC_LDO_SLP_DBIAS29,
};

static const esp_efuse_desc_t WR_DIS_RTC_LDO_SLP_DBIAS31[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of RTC_LDO_SLP_DBIAS31,
};

static const esp_efuse_desc_t WR_DIS_RTC_LDO_ACT_DBIAS31[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of RTC_LDO_ACT_DBIAS31,
};

static const esp_efuse_desc_t WR_DIS_RTC_LDO_ACT_DBIAS13[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of RTC_LDO_ACT_DBIAS13,
};

static const esp_efuse_desc_t WR_DIS_ADC_CALIBRATION_3[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of ADC_CALIBRATION_3,
};

static const esp_efuse_desc_t WR_DIS_BLOCK_KEY0[] = {
    {EFUSE_BLK0, 7, 1}, 	 // [WR_DIS.KEY0] wr_dis of BLOCK_KEY0,
};

static const esp_efuse_desc_t RD_DIS[] = {
    {EFUSE_BLK0, 32, 2}, 	 // [] Disable reading from BlOCK3,
};

static const esp_efuse_desc_t RD_DIS_KEY0[] = {
    {EFUSE_BLK0, 32, 2}, 	 // [] Read protection for EFUSE_BLK3. KEY0,
};

static const esp_efuse_desc_t RD_DIS_KEY0_LOW[] = {
    {EFUSE_BLK0, 32, 1}, 	 // [] Read protection for EFUSE_BLK3. KEY0 lower 128-bit key,
};

static const esp_efuse_desc_t RD_DIS_KEY0_HI[] = {
    {EFUSE_BLK0, 33, 1}, 	 // [] Read protection for EFUSE_BLK3. KEY0 higher 128-bit key,
};

static const esp_efuse_desc_t WDT_DELAY_SEL[] = {
    {EFUSE_BLK0, 34, 2}, 	 // [] RTC watchdog timeout threshold; in unit of slow clock cycle {0: "40000"; 1: "80000"; 2: "160000"; 3: "320000"},
};

static const esp_efuse_desc_t DIS_PAD_JTAG[] = {
    {EFUSE_BLK0, 36, 1}, 	 // [] Set this bit to disable pad jtag,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_ICACHE[] = {
    {EFUSE_BLK0, 37, 1}, 	 // [] The bit be set to disable icache in download mode,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    {EFUSE_BLK0, 38, 1}, 	 // [] The bit be set to disable manual encryption,
};

static const esp_efuse_desc_t SPI_BOOT_CRYPT_CNT[] = {
    {EFUSE_BLK0, 39, 3}, 	 // [] Enables flash encryption when 1 or 3 bits are set and disables otherwise {0: "Disable"; 1: "Enable"; 3: "Disable"; 7: "Enable"},
};

static const esp_efuse_desc_t XTS_KEY_LENGTH_256[] = {
    {EFUSE_BLK0, 42, 1}, 	 // [] Flash encryption key length {0: "128 bits key"; 1: "256 bits key"},
};

static const esp_efuse_desc_t UART_PRINT_CONTROL[] = {
    {EFUSE_BLK0, 43, 2}, 	 // [] Set the default UARTboot message output mode {0: "Enable"; 1: "Enable when GPIO8 is low at reset"; 2: "Enable when GPIO8 is high at reset"; 3: "Disable"},
};

static const esp_efuse_desc_t FORCE_SEND_RESUME[] = {
    {EFUSE_BLK0, 45, 1}, 	 // [] Set this bit to force ROM code to send a resume command during SPI boot,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 46, 1}, 	 // [] Set this bit to disable download mode (boot_mode[3:0] = 0; 1; 2; 4; 5; 6; 7),
};

static const esp_efuse_desc_t DIS_DIRECT_BOOT[] = {
    {EFUSE_BLK0, 47, 1}, 	 // [] This bit set means disable direct_boot mode,
};

static const esp_efuse_desc_t ENABLE_SECURITY_DOWNLOAD[] = {
    {EFUSE_BLK0, 48, 1}, 	 // [] Set this bit to enable secure UART download mode,
};

static const esp_efuse_desc_t FLASH_TPUW[] = {
    {EFUSE_BLK0, 49, 4}, 	 // [] Configures flash waiting time after power-up; in unit of ms. If the value is less than 15; the waiting time is the configurable value.  Otherwise; the waiting time is twice the configurable value,
};

static const esp_efuse_desc_t SECURE_BOOT_EN[] = {
    {EFUSE_BLK0, 53, 1}, 	 // [] The bit be set to enable secure boot,
};

static const esp_efuse_desc_t SECURE_VERSION[] = {
    {EFUSE_BLK0, 54, 4}, 	 // [] Secure version for anti-rollback,
};

static const esp_efuse_desc_t CUSTOM_MAC_USED[] = {
    {EFUSE_BLK0, 58, 1}, 	 // [ENABLE_CUSTOM_MAC] True if MAC_CUSTOM is burned,
};

static const esp_efuse_desc_t DISABLE_WAFER_VERSION_MAJOR[] = {
    {EFUSE_BLK0, 59, 1}, 	 // [] Disables check of wafer version major,
};

static const esp_efuse_desc_t DISABLE_BLK_VERSION_MAJOR[] = {
    {EFUSE_BLK0, 60, 1}, 	 // [] Disables check of blk version major,
};

static const esp_efuse_desc_t USER_DATA[] = {
    {EFUSE_BLK1, 0, 88}, 	 // [] User data block,
};

static const esp_efuse_desc_t USER_DATA_MAC_CUSTOM[] = {
    {EFUSE_BLK1, 0, 48}, 	 // [MAC_CUSTOM CUSTOM_MAC] Custom MAC address,
};

static const esp_efuse_desc_t MAC[] = {
    {EFUSE_BLK2, 40, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK2, 32, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK2, 24, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK2, 16, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK2, 8, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK2, 0, 8}, 	 // [MAC_FACTORY] MAC address,
};

static const esp_efuse_desc_t WAFER_VERSION_MINOR[] = {
    {EFUSE_BLK2, 48, 4}, 	 // [] WAFER_VERSION_MINOR,
};

static const esp_efuse_desc_t WAFER_VERSION_MAJOR[] = {
    {EFUSE_BLK2, 52, 2}, 	 // [] WAFER_VERSION_MAJOR,
};

static const esp_efuse_desc_t PKG_VERSION[] = {
    {EFUSE_BLK2, 54, 3}, 	 // [] EFUSE_PKG_VERSION,
};

static const esp_efuse_desc_t BLK_VERSION_MINOR[] = {
    {EFUSE_BLK2, 57, 3}, 	 // [] Minor version of BLOCK2 {0: "No calib"; 1: "With calib"},
};

static const esp_efuse_desc_t BLK_VERSION_MAJOR[] = {
    {EFUSE_BLK2, 60, 2}, 	 // [] Major version of BLOCK2,
};

static const esp_efuse_desc_t OCODE[] = {
    {EFUSE_BLK2, 62, 7}, 	 // [] OCode,
};

static const esp_efuse_desc_t TEMP_CALIB[] = {
    {EFUSE_BLK2, 69, 9}, 	 // [] Temperature calibration data,
};

static const esp_efuse_desc_t ADC1_INIT_CODE_ATTEN0[] = {
    {EFUSE_BLK2, 78, 8}, 	 // [] ADC1 init code at atten0,
};

static const esp_efuse_desc_t ADC1_INIT_CODE_ATTEN3[] = {
    {EFUSE_BLK2, 86, 5}, 	 // [] ADC1 init code at atten3,
};

static const esp_efuse_desc_t ADC1_CAL_VOL_ATTEN0[] = {
    {EFUSE_BLK2, 91, 8}, 	 // [] ADC1 calibration voltage at atten0,
};

static const esp_efuse_desc_t ADC1_CAL_VOL_ATTEN3[] = {
    {EFUSE_BLK2, 99, 6}, 	 // [] ADC1 calibration voltage at atten3,
};

static const esp_efuse_desc_t DIG_DBIAS_HVT[] = {
    {EFUSE_BLK2, 105, 5}, 	 // [] BLOCK2 digital dbias when hvt,
};

static const esp_efuse_desc_t DIG_LDO_SLP_DBIAS2[] = {
    {EFUSE_BLK2, 110, 7}, 	 // [] BLOCK2 DIG_LDO_DBG0_DBIAS2,
};

static const esp_efuse_desc_t DIG_LDO_SLP_DBIAS26[] = {
    {EFUSE_BLK2, 117, 8}, 	 // [] BLOCK2 DIG_LDO_DBG0_DBIAS26,
};

static const esp_efuse_desc_t DIG_LDO_ACT_DBIAS26[] = {
    {EFUSE_BLK2, 125, 6}, 	 // [] BLOCK2 DIG_LDO_ACT_DBIAS26,
};

static const esp_efuse_desc_t DIG_LDO_ACT_STEPD10[] = {
    {EFUSE_BLK2, 131, 4}, 	 // [] BLOCK2 DIG_LDO_ACT_STEPD10,
};

static const esp_efuse_desc_t RTC_LDO_SLP_DBIAS13[] = {
    {EFUSE_BLK2, 135, 7}, 	 // [] BLOCK2 DIG_LDO_SLP_DBIAS13,
};

static const esp_efuse_desc_t RTC_LDO_SLP_DBIAS29[] = {
    {EFUSE_BLK2, 142, 9}, 	 // [] BLOCK2 DIG_LDO_SLP_DBIAS29,
};

static const esp_efuse_desc_t RTC_LDO_SLP_DBIAS31[] = {
    {EFUSE_BLK2, 151, 6}, 	 // [] BLOCK2 DIG_LDO_SLP_DBIAS31,
};

static const esp_efuse_desc_t RTC_LDO_ACT_DBIAS31[] = {
    {EFUSE_BLK2, 157, 6}, 	 // [] BLOCK2 DIG_LDO_ACT_DBIAS31,
};

static const esp_efuse_desc_t RTC_LDO_ACT_DBIAS13[] = {
    {EFUSE_BLK2, 163, 8}, 	 // [] BLOCK2 DIG_LDO_ACT_DBIAS13,
};

static const esp_efuse_desc_t ADC_CALIBRATION_3[] = {
    {EFUSE_BLK2, 192, 11}, 	 // [] Store the bit [86:96] of ADC calibration data,
};

static const esp_efuse_desc_t KEY0[] = {
    {EFUSE_BLK3, 0, 256}, 	 // [BLOCK_KEY0] BLOCK_BLOCK_KEY0 - 256-bits. 256-bit key of Flash Encryption,
};

static const esp_efuse_desc_t KEY0_FE_256BIT[] = {
    {EFUSE_BLK3, 0, 256}, 	 // [] 256bit FE key,
};

static const esp_efuse_desc_t KEY0_FE_128BIT[] = {
    {EFUSE_BLK3, 0, 128}, 	 // [] 128bit FE key,
};

static const esp_efuse_desc_t KEY0_SB_128BIT[] = {
    {EFUSE_BLK3, 128, 128}, 	 // [] 128bit SB key,
};





const esp_efuse_desc_t* ESP_EFUSE_WR_DIS[] = {
    &WR_DIS[0],    		// [] Disable programming of individual eFuses
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RD_DIS[] = {
    &WR_DIS_RD_DIS[0],    		// [] wr_dis of RD_DIS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_WDT_DELAY_SEL[] = {
    &WR_DIS_WDT_DELAY_SEL[0],    		// [] wr_dis of WDT_DELAY_SEL
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_PAD_JTAG[] = {
    &WR_DIS_DIS_PAD_JTAG[0],    		// [] wr_dis of DIS_PAD_JTAG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DOWNLOAD_ICACHE[] = {
    &WR_DIS_DIS_DOWNLOAD_ICACHE[0],    		// [] wr_dis of DIS_DOWNLOAD_ICACHE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    &WR_DIS_DIS_DOWNLOAD_MANUAL_ENCRYPT[0],    		// [] wr_dis of DIS_DOWNLOAD_MANUAL_ENCRYPT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_BOOT_CRYPT_CNT[] = {
    &WR_DIS_SPI_BOOT_CRYPT_CNT[0],    		// [] wr_dis of SPI_BOOT_CRYPT_CNT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_XTS_KEY_LENGTH_256[] = {
    &WR_DIS_XTS_KEY_LENGTH_256[0],    		// [] wr_dis of XTS_KEY_LENGTH_256
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_EN[] = {
    &WR_DIS_SECURE_BOOT_EN[0],    		// [] wr_dis of SECURE_BOOT_EN
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_UART_PRINT_CONTROL[] = {
    &WR_DIS_UART_PRINT_CONTROL[0],    		// [] wr_dis of UART_PRINT_CONTROL
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FORCE_SEND_RESUME[] = {
    &WR_DIS_FORCE_SEND_RESUME[0],    		// [] wr_dis of FORCE_SEND_RESUME
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DOWNLOAD_MODE[] = {
    &WR_DIS_DIS_DOWNLOAD_MODE[0],    		// [] wr_dis of DIS_DOWNLOAD_MODE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DIRECT_BOOT[] = {
    &WR_DIS_DIS_DIRECT_BOOT[0],    		// [] wr_dis of DIS_DIRECT_BOOT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ENABLE_SECURITY_DOWNLOAD[] = {
    &WR_DIS_ENABLE_SECURITY_DOWNLOAD[0],    		// [] wr_dis of ENABLE_SECURITY_DOWNLOAD
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_TPUW[] = {
    &WR_DIS_FLASH_TPUW[0],    		// [] wr_dis of FLASH_TPUW
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_VERSION[] = {
    &WR_DIS_SECURE_VERSION[0],    		// [] wr_dis of SECURE_VERSION
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_CUSTOM_MAC_USED[] = {
    &WR_DIS_CUSTOM_MAC_USED[0],    		// [WR_DIS.ENABLE_CUSTOM_MAC] wr_dis of CUSTOM_MAC_USED
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_CUSTOM_MAC[] = {
    &WR_DIS_CUSTOM_MAC[0],    		// [WR_DIS.MAC_CUSTOM WR_DIS.USER_DATA_MAC_CUSTOM] wr_dis of CUSTOM_MAC
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_MAC[] = {
    &WR_DIS_MAC[0],    		// [WR_DIS.MAC_FACTORY] wr_dis of MAC
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_WAFER_VERSION_MINOR[] = {
    &WR_DIS_WAFER_VERSION_MINOR[0],    		// [] wr_dis of WAFER_VERSION_MINOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_WAFER_VERSION_MAJOR[] = {
    &WR_DIS_WAFER_VERSION_MAJOR[0],    		// [] wr_dis of WAFER_VERSION_MAJOR
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK_VERSION_MAJOR[] = {
    &WR_DIS_BLK_VERSION_MAJOR[0],    		// [] wr_dis of BLK_VERSION_MAJOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_OCODE[] = {
    &WR_DIS_OCODE[0],    		// [] wr_dis of OCODE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_TEMP_CALIB[] = {
    &WR_DIS_TEMP_CALIB[0],    		// [] wr_dis of TEMP_CALIB
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_INIT_CODE_ATTEN0[] = {
    &WR_DIS_ADC1_INIT_CODE_ATTEN0[0],    		// [] wr_dis of ADC1_INIT_CODE_ATTEN0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_INIT_CODE_ATTEN3[] = {
    &WR_DIS_ADC1_INIT_CODE_ATTEN3[0],    		// [] wr_dis of ADC1_INIT_CODE_ATTEN3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_CAL_VOL_ATTEN0[] = {
    &WR_DIS_ADC1_CAL_VOL_ATTEN0[0],    		// [] wr_dis of ADC1_CAL_VOL_ATTEN0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_CAL_VOL_ATTEN3[] = {
    &WR_DIS_ADC1_CAL_VOL_ATTEN3[0],    		// [] wr_dis of ADC1_CAL_VOL_ATTEN3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIG_DBIAS_HVT[] = {
    &WR_DIS_DIG_DBIAS_HVT[0],    		// [] wr_dis of DIG_DBIAS_HVT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIG_LDO_SLP_DBIAS2[] = {
    &WR_DIS_DIG_LDO_SLP_DBIAS2[0],    		// [] wr_dis of DIG_LDO_SLP_DBIAS2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIG_LDO_SLP_DBIAS26[] = {
    &WR_DIS_DIG_LDO_SLP_DBIAS26[0],    		// [] wr_dis of DIG_LDO_SLP_DBIAS26
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIG_LDO_ACT_DBIAS26[] = {
    &WR_DIS_DIG_LDO_ACT_DBIAS26[0],    		// [] wr_dis of DIG_LDO_ACT_DBIAS26
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIG_LDO_ACT_STEPD10[] = {
    &WR_DIS_DIG_LDO_ACT_STEPD10[0],    		// [] wr_dis of DIG_LDO_ACT_STEPD10
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTC_LDO_SLP_DBIAS13[] = {
    &WR_DIS_RTC_LDO_SLP_DBIAS13[0],    		// [] wr_dis of RTC_LDO_SLP_DBIAS13
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTC_LDO_SLP_DBIAS29[] = {
    &WR_DIS_RTC_LDO_SLP_DBIAS29[0],    		// [] wr_dis of RTC_LDO_SLP_DBIAS29
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTC_LDO_SLP_DBIAS31[] = {
    &WR_DIS_RTC_LDO_SLP_DBIAS31[0],    		// [] wr_dis of RTC_LDO_SLP_DBIAS31
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTC_LDO_ACT_DBIAS31[] = {
    &WR_DIS_RTC_LDO_ACT_DBIAS31[0],    		// [] wr_dis of RTC_LDO_ACT_DBIAS31
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RTC_LDO_ACT_DBIAS13[] = {
    &WR_DIS_RTC_LDO_ACT_DBIAS13[0],    		// [] wr_dis of RTC_LDO_ACT_DBIAS13
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC_CALIBRATION_3[] = {
    &WR_DIS_ADC_CALIBRATION_3[0],    		// [] wr_dis of ADC_CALIBRATION_3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY0[] = {
    &WR_DIS_BLOCK_KEY0[0],    		// [WR_DIS.KEY0] wr_dis of BLOCK_KEY0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS[] = {
    &RD_DIS[0],    		// [] Disable reading from BlOCK3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY0[] = {
    &RD_DIS_KEY0[0],    		// [] Read protection for EFUSE_BLK3. KEY0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY0_LOW[] = {
    &RD_DIS_KEY0_LOW[0],    		// [] Read protection for EFUSE_BLK3. KEY0 lower 128-bit key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY0_HI[] = {
    &RD_DIS_KEY0_HI[0],    		// [] Read protection for EFUSE_BLK3. KEY0 higher 128-bit key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WDT_DELAY_SEL[] = {
    &WDT_DELAY_SEL[0],    		// [] RTC watchdog timeout threshold; in unit of slow clock cycle {0: "40000"; 1: "80000"; 2: "160000"; 3: "320000"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_PAD_JTAG[] = {
    &DIS_PAD_JTAG[0],    		// [] Set this bit to disable pad jtag
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_ICACHE[] = {
    &DIS_DOWNLOAD_ICACHE[0],    		// [] The bit be set to disable icache in download mode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    &DIS_DOWNLOAD_MANUAL_ENCRYPT[0],    		// [] The bit be set to disable manual encryption
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_BOOT_CRYPT_CNT[] = {
    &SPI_BOOT_CRYPT_CNT[0],    		// [] Enables flash encryption when 1 or 3 bits are set and disables otherwise {0: "Disable"; 1: "Enable"; 3: "Disable"; 7: "Enable"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_XTS_KEY_LENGTH_256[] = {
    &XTS_KEY_LENGTH_256[0],    		// [] Flash encryption key length {0: "128 bits key"; 1: "256 bits key"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_UART_PRINT_CONTROL[] = {
    &UART_PRINT_CONTROL[0],    		// [] Set the default UARTboot message output mode {0: "Enable"; 1: "Enable when GPIO8 is low at reset"; 2: "Enable when GPIO8 is high at reset"; 3: "Disable"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FORCE_SEND_RESUME[] = {
    &FORCE_SEND_RESUME[0],    		// [] Set this bit to force ROM code to send a resume command during SPI boot
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MODE[] = {
    &DIS_DOWNLOAD_MODE[0],    		// [] Set this bit to disable download mode (boot_mode[3:0] = 0; 1; 2; 4; 5; 6; 7)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DIRECT_BOOT[] = {
    &DIS_DIRECT_BOOT[0],    		// [] This bit set means disable direct_boot mode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ENABLE_SECURITY_DOWNLOAD[] = {
    &ENABLE_SECURITY_DOWNLOAD[0],    		// [] Set this bit to enable secure UART download mode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_TPUW[] = {
    &FLASH_TPUW[0],    		// [] Configures flash waiting time after power-up; in unit of ms. If the value is less than 15; the waiting time is the configurable value.  Otherwise; the waiting time is twice the configurable value
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_EN[] = {
    &SECURE_BOOT_EN[0],    		// [] The bit be set to enable secure boot
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_VERSION[] = {
    &SECURE_VERSION[0],    		// [] Secure version for anti-rollback
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_CUSTOM_MAC_USED[] = {
    &CUSTOM_MAC_USED[0],    		// [ENABLE_CUSTOM_MAC] True if MAC_CUSTOM is burned
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

const esp_efuse_desc_t* ESP_EFUSE_USER_DATA[] = {
    &USER_DATA[0],    		// [] User data block
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USER_DATA_MAC_CUSTOM[] = {
    &USER_DATA_MAC_CUSTOM[0],    		// [MAC_CUSTOM CUSTOM_MAC] Custom MAC address
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

const esp_efuse_desc_t* ESP_EFUSE_WAFER_VERSION_MINOR[] = {
    &WAFER_VERSION_MINOR[0],    		// [] WAFER_VERSION_MINOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WAFER_VERSION_MAJOR[] = {
    &WAFER_VERSION_MAJOR[0],    		// [] WAFER_VERSION_MAJOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PKG_VERSION[] = {
    &PKG_VERSION[0],    		// [] EFUSE_PKG_VERSION
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_BLK_VERSION_MINOR[] = {
    &BLK_VERSION_MINOR[0],    		// [] Minor version of BLOCK2 {0: "No calib"; 1: "With calib"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_BLK_VERSION_MAJOR[] = {
    &BLK_VERSION_MAJOR[0],    		// [] Major version of BLOCK2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_OCODE[] = {
    &OCODE[0],    		// [] OCode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_TEMP_CALIB[] = {
    &TEMP_CALIB[0],    		// [] Temperature calibration data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_INIT_CODE_ATTEN0[] = {
    &ADC1_INIT_CODE_ATTEN0[0],    		// [] ADC1 init code at atten0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_INIT_CODE_ATTEN3[] = {
    &ADC1_INIT_CODE_ATTEN3[0],    		// [] ADC1 init code at atten3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CAL_VOL_ATTEN0[] = {
    &ADC1_CAL_VOL_ATTEN0[0],    		// [] ADC1 calibration voltage at atten0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CAL_VOL_ATTEN3[] = {
    &ADC1_CAL_VOL_ATTEN3[0],    		// [] ADC1 calibration voltage at atten3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIG_DBIAS_HVT[] = {
    &DIG_DBIAS_HVT[0],    		// [] BLOCK2 digital dbias when hvt
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIG_LDO_SLP_DBIAS2[] = {
    &DIG_LDO_SLP_DBIAS2[0],    		// [] BLOCK2 DIG_LDO_DBG0_DBIAS2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIG_LDO_SLP_DBIAS26[] = {
    &DIG_LDO_SLP_DBIAS26[0],    		// [] BLOCK2 DIG_LDO_DBG0_DBIAS26
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIG_LDO_ACT_DBIAS26[] = {
    &DIG_LDO_ACT_DBIAS26[0],    		// [] BLOCK2 DIG_LDO_ACT_DBIAS26
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIG_LDO_ACT_STEPD10[] = {
    &DIG_LDO_ACT_STEPD10[0],    		// [] BLOCK2 DIG_LDO_ACT_STEPD10
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RTC_LDO_SLP_DBIAS13[] = {
    &RTC_LDO_SLP_DBIAS13[0],    		// [] BLOCK2 DIG_LDO_SLP_DBIAS13
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RTC_LDO_SLP_DBIAS29[] = {
    &RTC_LDO_SLP_DBIAS29[0],    		// [] BLOCK2 DIG_LDO_SLP_DBIAS29
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RTC_LDO_SLP_DBIAS31[] = {
    &RTC_LDO_SLP_DBIAS31[0],    		// [] BLOCK2 DIG_LDO_SLP_DBIAS31
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RTC_LDO_ACT_DBIAS31[] = {
    &RTC_LDO_ACT_DBIAS31[0],    		// [] BLOCK2 DIG_LDO_ACT_DBIAS31
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RTC_LDO_ACT_DBIAS13[] = {
    &RTC_LDO_ACT_DBIAS13[0],    		// [] BLOCK2 DIG_LDO_ACT_DBIAS13
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC_CALIBRATION_3[] = {
    &ADC_CALIBRATION_3[0],    		// [] Store the bit [86:96] of ADC calibration data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY0[] = {
    &KEY0[0],    		// [BLOCK_KEY0] BLOCK_BLOCK_KEY0 - 256-bits. 256-bit key of Flash Encryption
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY0_FE_256BIT[] = {
    &KEY0_FE_256BIT[0],    		// [] 256bit FE key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY0_FE_128BIT[] = {
    &KEY0_FE_128BIT[0],    		// [] 128bit FE key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY0_SB_128BIT[] = {
    &KEY0_SB_128BIT[0],    		// [] 128bit SB key
    NULL
};
