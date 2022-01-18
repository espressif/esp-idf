/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_efuse.h"
#include <assert.h>
#include "esp_efuse_table.h"

// md5_digest_table 3bf086fa10d850cbaeccbd70fcba1c2f
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
    {EFUSE_BLK0, 1, 1}, 	 // Write protection for WDT_DELAY DIS_PAD_JTAG,
};

static const esp_efuse_desc_t WR_DIS_GROUP_2[] = {
    {EFUSE_BLK0, 2, 1}, 	 // Write protection for DOWNLOAD_DIS_MANUAL_ENCRYPT SPI_BOOT_ENCRYPT_DECRYPT_CNT XTS_KEY_LENGTH_256,
};

static const esp_efuse_desc_t WR_DIS_GROUP_3[] = {
    {EFUSE_BLK0, 3, 1}, 	 // Write protection for UART_PRINT_CONTROL FORCE_SEND_RESUME DIS_DOWNLOAD_MODE DIS_DIRECT_BOOT ENABLE_SECURITY_DOWNLOAD FLASH_TPUW SECURE_BOOT_EN,
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
    {EFUSE_BLK0, 7, 1}, 	 // Write protection for EFUSE_BLK3.  KEY0,
};

static const esp_efuse_desc_t RD_DIS[] = {
    {EFUSE_BLK0, 8, 2}, 	 // Read protection,
};

static const esp_efuse_desc_t RD_DIS_KEY0[] = {
    {EFUSE_BLK0, 32, 2}, 	 // Read protection for EFUSE_BLK3.  KEY0,
};

static const esp_efuse_desc_t WDT_DELAY_SEL[] = {
    {EFUSE_BLK0, 34, 2}, 	 // /* TODO: Need Description*/,
};

static const esp_efuse_desc_t DIS_PAD_JTAG[] = {
    {EFUSE_BLK0, 36, 1}, 	 // /* TODO: Need Description*/,
};

static const esp_efuse_desc_t EFUSE_DIS_DOWNLOAD_ICACHE[] = {
    {EFUSE_BLK0, 37, 1}, 	 // /* TODO: Need Description*/,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    {EFUSE_BLK0, 38, 1}, 	 // /* TODO: Need Description*/,
};

static const esp_efuse_desc_t SPI_BOOT_ENCRYPT_DECRYPT_CNT[] = {
    {EFUSE_BLK0, 39, 3}, 	 // /* TODO: Need Description*/,
};

static const esp_efuse_desc_t XTS_KEY_LENGTH_256[] = {
    {EFUSE_BLK0, 42, 1}, 	 // /* TODO: Need Description*/,
};

static const esp_efuse_desc_t UART_PRINT_CONTROL[] = {
    {EFUSE_BLK0, 43, 2}, 	 // /* TODO: Need Description*/,
};

static const esp_efuse_desc_t FORCE_SEND_RESUME[] = {
    {EFUSE_BLK0, 45, 1}, 	 // /* TODO: Need Description*/,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 46, 1}, 	 // /* TODO: Need Description*/,
};

static const esp_efuse_desc_t DIS_DIRECT_BOOT[] = {
    {EFUSE_BLK0, 47, 1}, 	 // /* TODO: Need Description*/,
};

static const esp_efuse_desc_t ENABLE_SECURITY_DOWNLOAD[] = {
    {EFUSE_BLK0, 48, 1}, 	 // /* TODO: Need Description*/,
};

static const esp_efuse_desc_t FLASH_TPUW[] = {
    {EFUSE_BLK0, 49, 4}, 	 // /* TODO: Need Description*/,
};

static const esp_efuse_desc_t SECURE_BOOT_EN[] = {
    {EFUSE_BLK0, 53, 1}, 	 // /* TODO: Need Description*/,
};

static const esp_efuse_desc_t SYSTEM_DATA0[] = {
    {EFUSE_BLK1, 0, 32}, 	 // EFUSE_SYSTEM_DATA0,
};

static const esp_efuse_desc_t SYSTEM_DATA1[] = {
    {EFUSE_BLK1, 32, 32}, 	 // EFUSE_SYSTEM_DATA1,
};

static const esp_efuse_desc_t SYSTEM_DATA2[] = {
    {EFUSE_BLK1, 64, 23}, 	 // EFUSE_SYSTEM_DATA2,
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
    {EFUSE_BLK3, 0, 256}, 	 // Key0 or user data,
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
    &WR_DIS_GROUP_1[0],    		// Write protection for WDT_DELAY DIS_PAD_JTAG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_GROUP_2[] = {
    &WR_DIS_GROUP_2[0],    		// Write protection for DOWNLOAD_DIS_MANUAL_ENCRYPT SPI_BOOT_ENCRYPT_DECRYPT_CNT XTS_KEY_LENGTH_256
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_GROUP_3[] = {
    &WR_DIS_GROUP_3[0],    		// Write protection for UART_PRINT_CONTROL FORCE_SEND_RESUME DIS_DOWNLOAD_MODE DIS_DIRECT_BOOT ENABLE_SECURITY_DOWNLOAD FLASH_TPUW SECURE_BOOT_EN
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
    &WR_DIS_KEY0[0],    		// Write protection for EFUSE_BLK3.  KEY0
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

const esp_efuse_desc_t* ESP_EFUSE_WDT_DELAY_SEL[] = {
    &WDT_DELAY_SEL[0],    		// /* TODO: Need Description*/
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_PAD_JTAG[] = {
    &DIS_PAD_JTAG[0],    		// /* TODO: Need Description*/
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_EFUSE_DIS_DOWNLOAD_ICACHE[] = {
    &EFUSE_DIS_DOWNLOAD_ICACHE[0],    		// /* TODO: Need Description*/
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    &DIS_DOWNLOAD_MANUAL_ENCRYPT[0],    		// /* TODO: Need Description*/
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_BOOT_ENCRYPT_DECRYPT_CNT[] = {
    &SPI_BOOT_ENCRYPT_DECRYPT_CNT[0],    		// /* TODO: Need Description*/
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_XTS_KEY_LENGTH_256[] = {
    &XTS_KEY_LENGTH_256[0],    		// /* TODO: Need Description*/
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_UART_PRINT_CONTROL[] = {
    &UART_PRINT_CONTROL[0],    		// /* TODO: Need Description*/
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FORCE_SEND_RESUME[] = {
    &FORCE_SEND_RESUME[0],    		// /* TODO: Need Description*/
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MODE[] = {
    &DIS_DOWNLOAD_MODE[0],    		// /* TODO: Need Description*/
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DIRECT_BOOT[] = {
    &DIS_DIRECT_BOOT[0],    		// /* TODO: Need Description*/
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ENABLE_SECURITY_DOWNLOAD[] = {
    &ENABLE_SECURITY_DOWNLOAD[0],    		// /* TODO: Need Description*/
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_TPUW[] = {
    &FLASH_TPUW[0],    		// /* TODO: Need Description*/
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_EN[] = {
    &SECURE_BOOT_EN[0],    		// /* TODO: Need Description*/
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SYSTEM_DATA0[] = {
    &SYSTEM_DATA0[0],    		// EFUSE_SYSTEM_DATA0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SYSTEM_DATA1[] = {
    &SYSTEM_DATA1[0],    		// EFUSE_SYSTEM_DATA1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SYSTEM_DATA2[] = {
    &SYSTEM_DATA2[0],    		// EFUSE_SYSTEM_DATA2
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
    &KEY0[0],    		// Key0 or user data
    NULL
};
