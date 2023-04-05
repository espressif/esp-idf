/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_efuse.h"
#include <assert.h>
#include "esp_efuse_table.h"

// md5_digest_table 2e197b7b14eec62fa5bdf94c6d71e87a
// This file was generated from the file esp_efuse_table.csv. DO NOT CHANGE THIS FILE MANUALLY.
// If you want to change some fields, you need to change esp_efuse_table.csv file
// then run `efuse_common_table` or `efuse_custom_table` command it will generate this file.
// To show efuse_table run the command 'show_efuse_table'.

#define MAX_BLK_LEN CONFIG_EFUSE_MAX_BLK_LEN

// The last free bit in the block is counted over the entire file.
#define LAST_FREE_BIT_BLK1 MAX_BLK_LEN
#define LAST_FREE_BIT_BLK2 MAX_BLK_LEN
#define LAST_FREE_BIT_BLK3 192

_Static_assert(LAST_FREE_BIT_BLK1 <= MAX_BLK_LEN, "The eFuse table does not match the coding scheme. Edit the table and restart the efuse_common_table or efuse_custom_table command to regenerate the new files.");
_Static_assert(LAST_FREE_BIT_BLK2 <= MAX_BLK_LEN, "The eFuse table does not match the coding scheme. Edit the table and restart the efuse_common_table or efuse_custom_table command to regenerate the new files.");
_Static_assert(LAST_FREE_BIT_BLK3 <= MAX_BLK_LEN, "The eFuse table does not match the coding scheme. Edit the table and restart the efuse_common_table or efuse_custom_table command to regenerate the new files.");

static const esp_efuse_desc_t WR_DIS[] = {
    {EFUSE_BLK0, 0, 16}, 	 // [] Efuse write disable mask,
};

static const esp_efuse_desc_t WR_DIS_RD_DIS[] = {
    {EFUSE_BLK0, 0, 1}, 	 // [WR_DIS.EFUSE_RD_DISABLE] wr_dis of RD_DIS,
};

static const esp_efuse_desc_t WR_DIS_WR_DIS[] = {
    {EFUSE_BLK0, 1, 1}, 	 // [] wr_dis of WR_DIS,
};

static const esp_efuse_desc_t WR_DIS_FLASH_CRYPT_CNT[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of FLASH_CRYPT_CNT,
};

static const esp_efuse_desc_t WR_DIS_UART_DOWNLOAD_DIS[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of UART_DOWNLOAD_DIS,
};

static const esp_efuse_desc_t WR_DIS_MAC[] = {
    {EFUSE_BLK0, 3, 1}, 	 // [WR_DIS.MAC_FACTORY] wr_dis of MAC,
};

static const esp_efuse_desc_t WR_DIS_MAC_CRC[] = {
    {EFUSE_BLK0, 3, 1}, 	 // [WR_DIS.MAC_FACTORY_CRC] wr_dis of MAC_CRC,
};

static const esp_efuse_desc_t WR_DIS_DISABLE_APP_CPU[] = {
    {EFUSE_BLK0, 3, 1}, 	 // [WR_DIS.CHIP_VER_DIS_APP_CPU] wr_dis of DISABLE_APP_CPU,
};

static const esp_efuse_desc_t WR_DIS_DISABLE_BT[] = {
    {EFUSE_BLK0, 3, 1}, 	 // [WR_DIS.CHIP_VER_DIS_BT] wr_dis of DISABLE_BT,
};

static const esp_efuse_desc_t WR_DIS_DIS_CACHE[] = {
    {EFUSE_BLK0, 3, 1}, 	 // [WR_DIS.CHIP_VER_DIS_CACHE] wr_dis of DIS_CACHE,
};

static const esp_efuse_desc_t WR_DIS_VOL_LEVEL_HP_INV[] = {
    {EFUSE_BLK0, 3, 1}, 	 // [] wr_dis of VOL_LEVEL_HP_INV,
};

static const esp_efuse_desc_t WR_DIS_CLK8M_FREQ[] = {
    {EFUSE_BLK0, 4, 1}, 	 // [WR_DIS.CK8M_FREQ] wr_dis of CLK8M_FREQ,
};

static const esp_efuse_desc_t WR_DIS_ADC_VREF[] = {
    {EFUSE_BLK0, 4, 1}, 	 // [] wr_dis of ADC_VREF,
};

static const esp_efuse_desc_t WR_DIS_XPD_SDIO_REG[] = {
    {EFUSE_BLK0, 5, 1}, 	 // [] wr_dis of XPD_SDIO_REG,
};

static const esp_efuse_desc_t WR_DIS_XPD_SDIO_TIEH[] = {
    {EFUSE_BLK0, 5, 1}, 	 // [WR_DIS.SDIO_TIEH] wr_dis of XPD_SDIO_TIEH,
};

static const esp_efuse_desc_t WR_DIS_XPD_SDIO_FORCE[] = {
    {EFUSE_BLK0, 5, 1}, 	 // [WR_DIS.SDIO_FORCE] wr_dis of XPD_SDIO_FORCE,
};

static const esp_efuse_desc_t WR_DIS_SPI_PAD_CONFIG_CLK[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of SPI_PAD_CONFIG_CLK,
};

static const esp_efuse_desc_t WR_DIS_SPI_PAD_CONFIG_Q[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of SPI_PAD_CONFIG_Q,
};

static const esp_efuse_desc_t WR_DIS_SPI_PAD_CONFIG_D[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of SPI_PAD_CONFIG_D,
};

static const esp_efuse_desc_t WR_DIS_SPI_PAD_CONFIG_CS0[] = {
    {EFUSE_BLK0, 6, 1}, 	 // [] wr_dis of SPI_PAD_CONFIG_CS0,
};

static const esp_efuse_desc_t WR_DIS_BLOCK1[] = {
    {EFUSE_BLK0, 7, 1}, 	 // [WR_DIS.ENCRYPT_FLASH_KEY WR_DIS.BLK1] wr_dis of BLOCK1,
};

static const esp_efuse_desc_t WR_DIS_BLOCK2[] = {
    {EFUSE_BLK0, 8, 1}, 	 // [WR_DIS.SECURE_BOOT_KEY WR_DIS.BLK2] wr_dis of BLOCK2,
};

static const esp_efuse_desc_t WR_DIS_BLOCK3[] = {
    {EFUSE_BLK0, 9, 1}, 	 // [WR_DIS.BLK3] wr_dis of BLOCK3,
};

static const esp_efuse_desc_t WR_DIS_CUSTOM_MAC_CRC[] = {
    {EFUSE_BLK0, 9, 1}, 	 // [WR_DIS.MAC_CUSTOM_CRC] wr_dis of CUSTOM_MAC_CRC,
};

static const esp_efuse_desc_t WR_DIS_CUSTOM_MAC[] = {
    {EFUSE_BLK0, 9, 1}, 	 // [WR_DIS.MAC_CUSTOM] wr_dis of CUSTOM_MAC,
};

static const esp_efuse_desc_t WR_DIS_ADC1_TP_LOW[] = {
    {EFUSE_BLK0, 9, 1}, 	 // [] wr_dis of ADC1_TP_LOW,
};

static const esp_efuse_desc_t WR_DIS_ADC1_TP_HIGH[] = {
    {EFUSE_BLK0, 9, 1}, 	 // [] wr_dis of ADC1_TP_HIGH,
};

static const esp_efuse_desc_t WR_DIS_ADC2_TP_LOW[] = {
    {EFUSE_BLK0, 9, 1}, 	 // [] wr_dis of ADC2_TP_LOW,
};

static const esp_efuse_desc_t WR_DIS_ADC2_TP_HIGH[] = {
    {EFUSE_BLK0, 9, 1}, 	 // [] wr_dis of ADC2_TP_HIGH,
};

static const esp_efuse_desc_t WR_DIS_SECURE_VERSION[] = {
    {EFUSE_BLK0, 9, 1}, 	 // [] wr_dis of SECURE_VERSION,
};

static const esp_efuse_desc_t WR_DIS_MAC_VERSION[] = {
    {EFUSE_BLK0, 9, 1}, 	 // [WR_DIS.MAC_CUSTOM_VER] wr_dis of MAC_VERSION,
};

static const esp_efuse_desc_t WR_DIS_BLK3_PART_RESERVE[] = {
    {EFUSE_BLK0, 10, 1}, 	 // [] wr_dis of BLK3_PART_RESERVE,
};

static const esp_efuse_desc_t WR_DIS_FLASH_CRYPT_CONFIG[] = {
    {EFUSE_BLK0, 10, 1}, 	 // [WR_DIS.ENCRYPT_CONFIG] wr_dis of FLASH_CRYPT_CONFIG,
};

static const esp_efuse_desc_t WR_DIS_CODING_SCHEME[] = {
    {EFUSE_BLK0, 10, 1}, 	 // [] wr_dis of CODING_SCHEME,
};

static const esp_efuse_desc_t WR_DIS_KEY_STATUS[] = {
    {EFUSE_BLK0, 10, 1}, 	 // [] wr_dis of KEY_STATUS,
};

static const esp_efuse_desc_t WR_DIS_ABS_DONE_0[] = {
    {EFUSE_BLK0, 12, 1}, 	 // [] wr_dis of ABS_DONE_0,
};

static const esp_efuse_desc_t WR_DIS_ABS_DONE_1[] = {
    {EFUSE_BLK0, 13, 1}, 	 // [] wr_dis of ABS_DONE_1,
};

static const esp_efuse_desc_t WR_DIS_JTAG_DISABLE[] = {
    {EFUSE_BLK0, 14, 1}, 	 // [WR_DIS.DISABLE_JTAG] wr_dis of JTAG_DISABLE,
};

static const esp_efuse_desc_t WR_DIS_CONSOLE_DEBUG_DISABLE[] = {
    {EFUSE_BLK0, 15, 1}, 	 // [] wr_dis of CONSOLE_DEBUG_DISABLE,
};

static const esp_efuse_desc_t WR_DIS_DISABLE_DL_ENCRYPT[] = {
    {EFUSE_BLK0, 15, 1}, 	 // [] wr_dis of DISABLE_DL_ENCRYPT,
};

static const esp_efuse_desc_t WR_DIS_DISABLE_DL_DECRYPT[] = {
    {EFUSE_BLK0, 15, 1}, 	 // [] wr_dis of DISABLE_DL_DECRYPT,
};

static const esp_efuse_desc_t WR_DIS_DISABLE_DL_CACHE[] = {
    {EFUSE_BLK0, 15, 1}, 	 // [] wr_dis of DISABLE_DL_CACHE,
};

static const esp_efuse_desc_t RD_DIS[] = {
    {EFUSE_BLK0, 16, 4}, 	 // [] Disable reading from BlOCK1-3,
};

static const esp_efuse_desc_t RD_DIS_BLOCK1[] = {
    {EFUSE_BLK0, 16, 1}, 	 // [RD_DIS.ENCRYPT_FLASH_KEY RD_DIS.BLK1] rd_dis of BLOCK1,
};

static const esp_efuse_desc_t RD_DIS_BLOCK2[] = {
    {EFUSE_BLK0, 17, 1}, 	 // [RD_DIS.SECURE_BOOT_KEY RD_DIS.BLK2] rd_dis of BLOCK2,
};

static const esp_efuse_desc_t RD_DIS_BLOCK3[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [RD_DIS.BLK3] rd_dis of BLOCK3,
};

static const esp_efuse_desc_t RD_DIS_CUSTOM_MAC_CRC[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [RD_DIS.MAC_CUSTOM_CRC] rd_dis of CUSTOM_MAC_CRC,
};

static const esp_efuse_desc_t RD_DIS_CUSTOM_MAC[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [RD_DIS.MAC_CUSTOM] rd_dis of CUSTOM_MAC,
};

static const esp_efuse_desc_t RD_DIS_ADC1_TP_LOW[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] rd_dis of ADC1_TP_LOW,
};

static const esp_efuse_desc_t RD_DIS_ADC1_TP_HIGH[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] rd_dis of ADC1_TP_HIGH,
};

static const esp_efuse_desc_t RD_DIS_ADC2_TP_LOW[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] rd_dis of ADC2_TP_LOW,
};

static const esp_efuse_desc_t RD_DIS_ADC2_TP_HIGH[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] rd_dis of ADC2_TP_HIGH,
};

static const esp_efuse_desc_t RD_DIS_SECURE_VERSION[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] rd_dis of SECURE_VERSION,
};

static const esp_efuse_desc_t RD_DIS_MAC_VERSION[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [RD_DIS.MAC_CUSTOM_VER] rd_dis of MAC_VERSION,
};

static const esp_efuse_desc_t RD_DIS_BLK3_PART_RESERVE[] = {
    {EFUSE_BLK0, 19, 1}, 	 // [] rd_dis of BLK3_PART_RESERVE,
};

static const esp_efuse_desc_t RD_DIS_FLASH_CRYPT_CONFIG[] = {
    {EFUSE_BLK0, 19, 1}, 	 // [RD_DIS.ENCRYPT_CONFIG] rd_dis of FLASH_CRYPT_CONFIG,
};

static const esp_efuse_desc_t RD_DIS_CODING_SCHEME[] = {
    {EFUSE_BLK0, 19, 1}, 	 // [] rd_dis of CODING_SCHEME,
};

static const esp_efuse_desc_t RD_DIS_KEY_STATUS[] = {
    {EFUSE_BLK0, 19, 1}, 	 // [] rd_dis of KEY_STATUS,
};

static const esp_efuse_desc_t FLASH_CRYPT_CNT[] = {
    {EFUSE_BLK0, 20, 7}, 	 // [] Flash encryption is enabled if this field has an odd number of bits set,
};

static const esp_efuse_desc_t UART_DOWNLOAD_DIS[] = {
    {EFUSE_BLK0, 27, 1}, 	 // [] Disable UART download mode. Valid for ESP32 V3 and newer; only,
};

static const esp_efuse_desc_t MAC[] = {
    {EFUSE_BLK0, 72, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK0, 64, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK0, 56, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK0, 48, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK0, 40, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK0, 32, 8}, 	 // [MAC_FACTORY] MAC address,
};

static const esp_efuse_desc_t MAC_CRC[] = {
    {EFUSE_BLK0, 80, 8}, 	 // [MAC_FACTORY_CRC] CRC8 for MAC address,
};

static const esp_efuse_desc_t DISABLE_APP_CPU[] = {
    {EFUSE_BLK0, 96, 1}, 	 // [CHIP_VER_DIS_APP_CPU] Disables APP CPU,
};

static const esp_efuse_desc_t DISABLE_BT[] = {
    {EFUSE_BLK0, 97, 1}, 	 // [CHIP_VER_DIS_BT] Disables Bluetooth,
};

static const esp_efuse_desc_t CHIP_PACKAGE_4BIT[] = {
    {EFUSE_BLK0, 98, 1}, 	 // [CHIP_VER_PKG_4BIT] Chip package identifier #4bit,
};

static const esp_efuse_desc_t DIS_CACHE[] = {
    {EFUSE_BLK0, 99, 1}, 	 // [CHIP_VER_DIS_CACHE] Disables cache,
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_HD[] = {
    {EFUSE_BLK0, 100, 5}, 	 // [] read for SPI_pad_config_hd,
};

static const esp_efuse_desc_t CHIP_PACKAGE[] = {
    {EFUSE_BLK0, 105, 3}, 	 // [CHIP_VER_PKG] Chip package identifier,
};

static const esp_efuse_desc_t CHIP_CPU_FREQ_LOW[] = {
    {EFUSE_BLK0, 108, 1}, 	 // [] If set alongside EFUSE_RD_CHIP_CPU_FREQ_RATED; the ESP32's max CPU frequency is rated for 160MHz. 240MHz otherwise,
};

static const esp_efuse_desc_t CHIP_CPU_FREQ_RATED[] = {
    {EFUSE_BLK0, 109, 1}, 	 // [] If set; the ESP32's maximum CPU frequency has been rated,
};

static const esp_efuse_desc_t BLK3_PART_RESERVE[] = {
    {EFUSE_BLK0, 110, 1}, 	 // [] BLOCK3 partially served for ADC calibration data,
};

static const esp_efuse_desc_t CHIP_VER_REV1[] = {
    {EFUSE_BLK0, 111, 1}, 	 // [] bit is set to 1 for rev1 silicon,
};

static const esp_efuse_desc_t CLK8M_FREQ[] = {
    {EFUSE_BLK0, 128, 8}, 	 // [CK8M_FREQ] 8MHz clock freq override,
};

static const esp_efuse_desc_t ADC_VREF[] = {
    {EFUSE_BLK0, 136, 5}, 	 // [] True ADC reference voltage,
};

static const esp_efuse_desc_t XPD_SDIO_REG[] = {
    {EFUSE_BLK0, 142, 1}, 	 // [] read for XPD_SDIO_REG,
};

static const esp_efuse_desc_t XPD_SDIO_TIEH[] = {
    {EFUSE_BLK0, 143, 1}, 	 // [SDIO_TIEH] If XPD_SDIO_FORCE & XPD_SDIO_REG {1: "3.3V"; 0: "1.8V"},
};

static const esp_efuse_desc_t XPD_SDIO_FORCE[] = {
    {EFUSE_BLK0, 144, 1}, 	 // [SDIO_FORCE] Ignore MTDI pin (GPIO12) for VDD_SDIO on reset,
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_CLK[] = {
    {EFUSE_BLK0, 160, 5}, 	 // [] Override SD_CLK pad (GPIO6/SPICLK),
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_Q[] = {
    {EFUSE_BLK0, 165, 5}, 	 // [] Override SD_DATA_0 pad (GPIO7/SPIQ),
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_D[] = {
    {EFUSE_BLK0, 170, 5}, 	 // [] Override SD_DATA_1 pad (GPIO8/SPID),
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_CS0[] = {
    {EFUSE_BLK0, 175, 5}, 	 // [] Override SD_CMD pad (GPIO11/SPICS0),
};

static const esp_efuse_desc_t CHIP_VER_REV2[] = {
    {EFUSE_BLK0, 180, 1}, 	 // [],
};

static const esp_efuse_desc_t VOL_LEVEL_HP_INV[] = {
    {EFUSE_BLK0, 182, 2}, 	 // [] This field stores the voltage level for CPU to run at 240 MHz; or for flash/PSRAM to run at 80 MHz.0x0: level 7; 0x1: level 6; 0x2: level 5; 0x3: level 4. (RO),
};

static const esp_efuse_desc_t WAFER_VERSION_MINOR[] = {
    {EFUSE_BLK0, 184, 2}, 	 // [],
};

static const esp_efuse_desc_t FLASH_CRYPT_CONFIG[] = {
    {EFUSE_BLK0, 188, 4}, 	 // [ENCRYPT_CONFIG] Flash encryption config (key tweak bits),
};

static const esp_efuse_desc_t CODING_SCHEME[] = {
    {EFUSE_BLK0, 192, 2}, 	 // [] Efuse variable block length scheme {0: "NONE (BLK1-3 len=256 bits)"; 1: "3/4 (BLK1-3 len=192 bits)"; 2: "REPEAT (BLK1-3 len=128 bits) not supported"; 3: "NONE (BLK1-3 len=256 bits)"},
};

static const esp_efuse_desc_t CONSOLE_DEBUG_DISABLE[] = {
    {EFUSE_BLK0, 194, 1}, 	 // [] Disable ROM BASIC interpreter fallback,
};

static const esp_efuse_desc_t DISABLE_SDIO_HOST[] = {
    {EFUSE_BLK0, 195, 1}, 	 // [],
};

static const esp_efuse_desc_t ABS_DONE_0[] = {
    {EFUSE_BLK0, 196, 1}, 	 // [] Secure boot V1 is enabled for bootloader image,
};

static const esp_efuse_desc_t ABS_DONE_1[] = {
    {EFUSE_BLK0, 197, 1}, 	 // [] Secure boot V2 is enabled for bootloader image,
};

static const esp_efuse_desc_t JTAG_DISABLE[] = {
    {EFUSE_BLK0, 198, 1}, 	 // [DISABLE_JTAG] Disable JTAG,
};

static const esp_efuse_desc_t DISABLE_DL_ENCRYPT[] = {
    {EFUSE_BLK0, 199, 1}, 	 // [] Disable flash encryption in UART bootloader,
};

static const esp_efuse_desc_t DISABLE_DL_DECRYPT[] = {
    {EFUSE_BLK0, 200, 1}, 	 // [] Disable flash decryption in UART bootloader,
};

static const esp_efuse_desc_t DISABLE_DL_CACHE[] = {
    {EFUSE_BLK0, 201, 1}, 	 // [] Disable flash cache in UART bootloader,
};

static const esp_efuse_desc_t KEY_STATUS[] = {
    {EFUSE_BLK0, 202, 1}, 	 // [] Usage of efuse block 3 (reserved),
};

static const esp_efuse_desc_t BLOCK1[] = {
    {EFUSE_BLK1, 0, MAX_BLK_LEN}, 	 // [ENCRYPT_FLASH_KEY] Flash encryption key,
};

static const esp_efuse_desc_t BLOCK2[] = {
    {EFUSE_BLK2, 0, MAX_BLK_LEN}, 	 // [SECURE_BOOT_KEY] Security boot key,
};

static const esp_efuse_desc_t CUSTOM_MAC_CRC[] = {
    {EFUSE_BLK3, 0, 8}, 	 // [MAC_CUSTOM_CRC] CRC8 for custom MAC address,
};

static const esp_efuse_desc_t MAC_CUSTOM[] = {
    {EFUSE_BLK3, 8, 48}, 	 // [MAC_CUSTOM] Custom MAC address,
};

static const esp_efuse_desc_t ADC1_TP_LOW[] = {
    {EFUSE_BLK3, 96, 7}, 	 // [] ADC1 Two Point calibration low point. Only valid if EFUSE_RD_BLK3_PART_RESERVE,
};

static const esp_efuse_desc_t ADC1_TP_HIGH[] = {
    {EFUSE_BLK3, 103, 9}, 	 // [] ADC1 Two Point calibration high point. Only valid if EFUSE_RD_BLK3_PART_RESERVE,
};

static const esp_efuse_desc_t ADC2_TP_LOW[] = {
    {EFUSE_BLK3, 112, 7}, 	 // [] ADC2 Two Point calibration low point. Only valid if EFUSE_RD_BLK3_PART_RESERVE,
};

static const esp_efuse_desc_t ADC2_TP_HIGH[] = {
    {EFUSE_BLK3, 119, 9}, 	 // [] ADC2 Two Point calibration high point. Only valid if EFUSE_RD_BLK3_PART_RESERVE,
};

static const esp_efuse_desc_t SECURE_VERSION[] = {
    {EFUSE_BLK3, 128, 32}, 	 // [] Secure version for anti-rollback,
};

static const esp_efuse_desc_t MAC_VERSION[] = {
    {EFUSE_BLK3, 184, 8}, 	 // [MAC_CUSTOM_VER] Version of the MAC field {1: "Custom MAC in BLOCK3"},
};





const esp_efuse_desc_t* ESP_EFUSE_WR_DIS[] = {
    &WR_DIS[0],    		// [] Efuse write disable mask
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RD_DIS[] = {
    &WR_DIS_RD_DIS[0],    		// [WR_DIS.EFUSE_RD_DISABLE] wr_dis of RD_DIS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_WR_DIS[] = {
    &WR_DIS_WR_DIS[0],    		// [] wr_dis of WR_DIS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_CRYPT_CNT[] = {
    &WR_DIS_FLASH_CRYPT_CNT[0],    		// [] wr_dis of FLASH_CRYPT_CNT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_UART_DOWNLOAD_DIS[] = {
    &WR_DIS_UART_DOWNLOAD_DIS[0],    		// [] wr_dis of UART_DOWNLOAD_DIS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_MAC[] = {
    &WR_DIS_MAC[0],    		// [WR_DIS.MAC_FACTORY] wr_dis of MAC
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_MAC_CRC[] = {
    &WR_DIS_MAC_CRC[0],    		// [WR_DIS.MAC_FACTORY_CRC] wr_dis of MAC_CRC
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DISABLE_APP_CPU[] = {
    &WR_DIS_DISABLE_APP_CPU[0],    		// [WR_DIS.CHIP_VER_DIS_APP_CPU] wr_dis of DISABLE_APP_CPU
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DISABLE_BT[] = {
    &WR_DIS_DISABLE_BT[0],    		// [WR_DIS.CHIP_VER_DIS_BT] wr_dis of DISABLE_BT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_CACHE[] = {
    &WR_DIS_DIS_CACHE[0],    		// [WR_DIS.CHIP_VER_DIS_CACHE] wr_dis of DIS_CACHE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_VOL_LEVEL_HP_INV[] = {
    &WR_DIS_VOL_LEVEL_HP_INV[0],    		// [] wr_dis of VOL_LEVEL_HP_INV
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_CLK8M_FREQ[] = {
    &WR_DIS_CLK8M_FREQ[0],    		// [WR_DIS.CK8M_FREQ] wr_dis of CLK8M_FREQ
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC_VREF[] = {
    &WR_DIS_ADC_VREF[0],    		// [] wr_dis of ADC_VREF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_XPD_SDIO_REG[] = {
    &WR_DIS_XPD_SDIO_REG[0],    		// [] wr_dis of XPD_SDIO_REG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_XPD_SDIO_TIEH[] = {
    &WR_DIS_XPD_SDIO_TIEH[0],    		// [WR_DIS.SDIO_TIEH] wr_dis of XPD_SDIO_TIEH
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_XPD_SDIO_FORCE[] = {
    &WR_DIS_XPD_SDIO_FORCE[0],    		// [WR_DIS.SDIO_FORCE] wr_dis of XPD_SDIO_FORCE
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_PAD_CONFIG_CS0[] = {
    &WR_DIS_SPI_PAD_CONFIG_CS0[0],    		// [] wr_dis of SPI_PAD_CONFIG_CS0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK1[] = {
    &WR_DIS_BLOCK1[0],    		// [WR_DIS.ENCRYPT_FLASH_KEY WR_DIS.BLK1] wr_dis of BLOCK1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK2[] = {
    &WR_DIS_BLOCK2[0],    		// [WR_DIS.SECURE_BOOT_KEY WR_DIS.BLK2] wr_dis of BLOCK2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK3[] = {
    &WR_DIS_BLOCK3[0],    		// [WR_DIS.BLK3] wr_dis of BLOCK3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_CUSTOM_MAC_CRC[] = {
    &WR_DIS_CUSTOM_MAC_CRC[0],    		// [WR_DIS.MAC_CUSTOM_CRC] wr_dis of CUSTOM_MAC_CRC
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_CUSTOM_MAC[] = {
    &WR_DIS_CUSTOM_MAC[0],    		// [WR_DIS.MAC_CUSTOM] wr_dis of CUSTOM_MAC
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_TP_LOW[] = {
    &WR_DIS_ADC1_TP_LOW[0],    		// [] wr_dis of ADC1_TP_LOW
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_TP_HIGH[] = {
    &WR_DIS_ADC1_TP_HIGH[0],    		// [] wr_dis of ADC1_TP_HIGH
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_TP_LOW[] = {
    &WR_DIS_ADC2_TP_LOW[0],    		// [] wr_dis of ADC2_TP_LOW
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_TP_HIGH[] = {
    &WR_DIS_ADC2_TP_HIGH[0],    		// [] wr_dis of ADC2_TP_HIGH
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_VERSION[] = {
    &WR_DIS_SECURE_VERSION[0],    		// [] wr_dis of SECURE_VERSION
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_MAC_VERSION[] = {
    &WR_DIS_MAC_VERSION[0],    		// [WR_DIS.MAC_CUSTOM_VER] wr_dis of MAC_VERSION
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK3_PART_RESERVE[] = {
    &WR_DIS_BLK3_PART_RESERVE[0],    		// [] wr_dis of BLK3_PART_RESERVE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_CRYPT_CONFIG[] = {
    &WR_DIS_FLASH_CRYPT_CONFIG[0],    		// [WR_DIS.ENCRYPT_CONFIG] wr_dis of FLASH_CRYPT_CONFIG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_CODING_SCHEME[] = {
    &WR_DIS_CODING_SCHEME[0],    		// [] wr_dis of CODING_SCHEME
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_STATUS[] = {
    &WR_DIS_KEY_STATUS[0],    		// [] wr_dis of KEY_STATUS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ABS_DONE_0[] = {
    &WR_DIS_ABS_DONE_0[0],    		// [] wr_dis of ABS_DONE_0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ABS_DONE_1[] = {
    &WR_DIS_ABS_DONE_1[0],    		// [] wr_dis of ABS_DONE_1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_JTAG_DISABLE[] = {
    &WR_DIS_JTAG_DISABLE[0],    		// [WR_DIS.DISABLE_JTAG] wr_dis of JTAG_DISABLE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_CONSOLE_DEBUG_DISABLE[] = {
    &WR_DIS_CONSOLE_DEBUG_DISABLE[0],    		// [] wr_dis of CONSOLE_DEBUG_DISABLE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DISABLE_DL_ENCRYPT[] = {
    &WR_DIS_DISABLE_DL_ENCRYPT[0],    		// [] wr_dis of DISABLE_DL_ENCRYPT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DISABLE_DL_DECRYPT[] = {
    &WR_DIS_DISABLE_DL_DECRYPT[0],    		// [] wr_dis of DISABLE_DL_DECRYPT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DISABLE_DL_CACHE[] = {
    &WR_DIS_DISABLE_DL_CACHE[0],    		// [] wr_dis of DISABLE_DL_CACHE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS[] = {
    &RD_DIS[0],    		// [] Disable reading from BlOCK1-3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK1[] = {
    &RD_DIS_BLOCK1[0],    		// [RD_DIS.ENCRYPT_FLASH_KEY RD_DIS.BLK1] rd_dis of BLOCK1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK2[] = {
    &RD_DIS_BLOCK2[0],    		// [RD_DIS.SECURE_BOOT_KEY RD_DIS.BLK2] rd_dis of BLOCK2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK3[] = {
    &RD_DIS_BLOCK3[0],    		// [RD_DIS.BLK3] rd_dis of BLOCK3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_CUSTOM_MAC_CRC[] = {
    &RD_DIS_CUSTOM_MAC_CRC[0],    		// [RD_DIS.MAC_CUSTOM_CRC] rd_dis of CUSTOM_MAC_CRC
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_CUSTOM_MAC[] = {
    &RD_DIS_CUSTOM_MAC[0],    		// [RD_DIS.MAC_CUSTOM] rd_dis of CUSTOM_MAC
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC1_TP_LOW[] = {
    &RD_DIS_ADC1_TP_LOW[0],    		// [] rd_dis of ADC1_TP_LOW
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC1_TP_HIGH[] = {
    &RD_DIS_ADC1_TP_HIGH[0],    		// [] rd_dis of ADC1_TP_HIGH
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC2_TP_LOW[] = {
    &RD_DIS_ADC2_TP_LOW[0],    		// [] rd_dis of ADC2_TP_LOW
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC2_TP_HIGH[] = {
    &RD_DIS_ADC2_TP_HIGH[0],    		// [] rd_dis of ADC2_TP_HIGH
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_SECURE_VERSION[] = {
    &RD_DIS_SECURE_VERSION[0],    		// [] rd_dis of SECURE_VERSION
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_MAC_VERSION[] = {
    &RD_DIS_MAC_VERSION[0],    		// [RD_DIS.MAC_CUSTOM_VER] rd_dis of MAC_VERSION
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLK3_PART_RESERVE[] = {
    &RD_DIS_BLK3_PART_RESERVE[0],    		// [] rd_dis of BLK3_PART_RESERVE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_FLASH_CRYPT_CONFIG[] = {
    &RD_DIS_FLASH_CRYPT_CONFIG[0],    		// [RD_DIS.ENCRYPT_CONFIG] rd_dis of FLASH_CRYPT_CONFIG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_CODING_SCHEME[] = {
    &RD_DIS_CODING_SCHEME[0],    		// [] rd_dis of CODING_SCHEME
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY_STATUS[] = {
    &RD_DIS_KEY_STATUS[0],    		// [] rd_dis of KEY_STATUS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_CRYPT_CNT[] = {
    &FLASH_CRYPT_CNT[0],    		// [] Flash encryption is enabled if this field has an odd number of bits set
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_UART_DOWNLOAD_DIS[] = {
    &UART_DOWNLOAD_DIS[0],    		// [] Disable UART download mode. Valid for ESP32 V3 and newer; only
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

const esp_efuse_desc_t* ESP_EFUSE_MAC_CRC[] = {
    &MAC_CRC[0],    		// [MAC_FACTORY_CRC] CRC8 for MAC address
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DISABLE_APP_CPU[] = {
    &DISABLE_APP_CPU[0],    		// [CHIP_VER_DIS_APP_CPU] Disables APP CPU
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DISABLE_BT[] = {
    &DISABLE_BT[0],    		// [CHIP_VER_DIS_BT] Disables Bluetooth
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_CHIP_PACKAGE_4BIT[] = {
    &CHIP_PACKAGE_4BIT[0],    		// [CHIP_VER_PKG_4BIT] Chip package identifier #4bit
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_CACHE[] = {
    &DIS_CACHE[0],    		// [CHIP_VER_DIS_CACHE] Disables cache
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_HD[] = {
    &SPI_PAD_CONFIG_HD[0],    		// [] read for SPI_pad_config_hd
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_CHIP_PACKAGE[] = {
    &CHIP_PACKAGE[0],    		// [CHIP_VER_PKG] Chip package identifier
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_CHIP_CPU_FREQ_LOW[] = {
    &CHIP_CPU_FREQ_LOW[0],    		// [] If set alongside EFUSE_RD_CHIP_CPU_FREQ_RATED; the ESP32's max CPU frequency is rated for 160MHz. 240MHz otherwise
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_CHIP_CPU_FREQ_RATED[] = {
    &CHIP_CPU_FREQ_RATED[0],    		// [] If set; the ESP32's maximum CPU frequency has been rated
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_BLK3_PART_RESERVE[] = {
    &BLK3_PART_RESERVE[0],    		// [] BLOCK3 partially served for ADC calibration data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_CHIP_VER_REV1[] = {
    &CHIP_VER_REV1[0],    		// [] bit is set to 1 for rev1 silicon
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_CLK8M_FREQ[] = {
    &CLK8M_FREQ[0],    		// [CK8M_FREQ] 8MHz clock freq override
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC_VREF[] = {
    &ADC_VREF[0],    		// [] True ADC reference voltage
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_XPD_SDIO_REG[] = {
    &XPD_SDIO_REG[0],    		// [] read for XPD_SDIO_REG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_XPD_SDIO_TIEH[] = {
    &XPD_SDIO_TIEH[0],    		// [SDIO_TIEH] If XPD_SDIO_FORCE & XPD_SDIO_REG {1: "3.3V"; 0: "1.8V"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_XPD_SDIO_FORCE[] = {
    &XPD_SDIO_FORCE[0],    		// [SDIO_FORCE] Ignore MTDI pin (GPIO12) for VDD_SDIO on reset
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_CLK[] = {
    &SPI_PAD_CONFIG_CLK[0],    		// [] Override SD_CLK pad (GPIO6/SPICLK)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_Q[] = {
    &SPI_PAD_CONFIG_Q[0],    		// [] Override SD_DATA_0 pad (GPIO7/SPIQ)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_D[] = {
    &SPI_PAD_CONFIG_D[0],    		// [] Override SD_DATA_1 pad (GPIO8/SPID)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_CS0[] = {
    &SPI_PAD_CONFIG_CS0[0],    		// [] Override SD_CMD pad (GPIO11/SPICS0)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_CHIP_VER_REV2[] = {
    &CHIP_VER_REV2[0],    		// []
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_VOL_LEVEL_HP_INV[] = {
    &VOL_LEVEL_HP_INV[0],    		// [] This field stores the voltage level for CPU to run at 240 MHz; or for flash/PSRAM to run at 80 MHz.0x0: level 7; 0x1: level 6; 0x2: level 5; 0x3: level 4. (RO)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WAFER_VERSION_MINOR[] = {
    &WAFER_VERSION_MINOR[0],    		// []
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_CRYPT_CONFIG[] = {
    &FLASH_CRYPT_CONFIG[0],    		// [ENCRYPT_CONFIG] Flash encryption config (key tweak bits)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_CODING_SCHEME[] = {
    &CODING_SCHEME[0],    		// [] Efuse variable block length scheme {0: "NONE (BLK1-3 len=256 bits)"; 1: "3/4 (BLK1-3 len=192 bits)"; 2: "REPEAT (BLK1-3 len=128 bits) not supported"; 3: "NONE (BLK1-3 len=256 bits)"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_CONSOLE_DEBUG_DISABLE[] = {
    &CONSOLE_DEBUG_DISABLE[0],    		// [] Disable ROM BASIC interpreter fallback
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DISABLE_SDIO_HOST[] = {
    &DISABLE_SDIO_HOST[0],    		// []
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ABS_DONE_0[] = {
    &ABS_DONE_0[0],    		// [] Secure boot V1 is enabled for bootloader image
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ABS_DONE_1[] = {
    &ABS_DONE_1[0],    		// [] Secure boot V2 is enabled for bootloader image
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_JTAG_DISABLE[] = {
    &JTAG_DISABLE[0],    		// [DISABLE_JTAG] Disable JTAG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DISABLE_DL_ENCRYPT[] = {
    &DISABLE_DL_ENCRYPT[0],    		// [] Disable flash encryption in UART bootloader
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DISABLE_DL_DECRYPT[] = {
    &DISABLE_DL_DECRYPT[0],    		// [] Disable flash decryption in UART bootloader
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DISABLE_DL_CACHE[] = {
    &DISABLE_DL_CACHE[0],    		// [] Disable flash cache in UART bootloader
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_STATUS[] = {
    &KEY_STATUS[0],    		// [] Usage of efuse block 3 (reserved)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_BLOCK1[] = {
    &BLOCK1[0],    		// [ENCRYPT_FLASH_KEY] Flash encryption key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_BLOCK2[] = {
    &BLOCK2[0],    		// [SECURE_BOOT_KEY] Security boot key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_CUSTOM_MAC_CRC[] = {
    &CUSTOM_MAC_CRC[0],    		// [MAC_CUSTOM_CRC] CRC8 for custom MAC address
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_MAC_CUSTOM[] = {
    &MAC_CUSTOM[0],    		// [MAC_CUSTOM] Custom MAC address
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_TP_LOW[] = {
    &ADC1_TP_LOW[0],    		// [] ADC1 Two Point calibration low point. Only valid if EFUSE_RD_BLK3_PART_RESERVE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_TP_HIGH[] = {
    &ADC1_TP_HIGH[0],    		// [] ADC1 Two Point calibration high point. Only valid if EFUSE_RD_BLK3_PART_RESERVE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_TP_LOW[] = {
    &ADC2_TP_LOW[0],    		// [] ADC2 Two Point calibration low point. Only valid if EFUSE_RD_BLK3_PART_RESERVE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_TP_HIGH[] = {
    &ADC2_TP_HIGH[0],    		// [] ADC2 Two Point calibration high point. Only valid if EFUSE_RD_BLK3_PART_RESERVE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_VERSION[] = {
    &SECURE_VERSION[0],    		// [] Secure version for anti-rollback
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_MAC_VERSION[] = {
    &MAC_VERSION[0],    		// [MAC_CUSTOM_VER] Version of the MAC field {1: "Custom MAC in BLOCK3"}
    NULL
};
