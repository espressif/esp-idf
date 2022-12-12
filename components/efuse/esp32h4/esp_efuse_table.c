/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_efuse.h"
#include <assert.h>
#include "esp_efuse_table.h"

// md5_digest_table b9e60ac2d8c534764d7bee10063617aa
// This file was generated from the file esp_efuse_table.csv. DO NOT CHANGE THIS FILE MANUALLY.
// If you want to change some fields, you need to change esp_efuse_table.csv file
// then run `efuse_common_table` or `efuse_custom_table` command it will generate this file.
// To show efuse_table run the command 'show_efuse_table'.

static const esp_efuse_desc_t WR_DIS[] = {
    {EFUSE_BLK0, 0, 32}, 	 // Write protection,
};

static const esp_efuse_desc_t WR_DIS_RD_DIS[] = {
    {EFUSE_BLK0, 0, 1}, 	 // Write protection for RD_DIS_KEY0 RD_DIS_KEY1 RD_DIS_KEY2 RD_DIS_KEY3 RD_DIS_KEY4 RD_DIS_KEY5 RD_DIS_SYS_DATA_PART2,
};

static const esp_efuse_desc_t WR_DIS_GROUP_1[] = {
    {EFUSE_BLK0, 2, 1}, 	 // Write protection for DIS_ICACHE DIS_DOWNLOAD_ICACHE DIS_FORCE_DOWNLOAD DIS_USB DIS_TWAI SOFT_DIS_JTAG DIS_DOWNLOAD_MANUAL_ENCRYPT,
};

static const esp_efuse_desc_t WR_DIS_GROUP_2[] = {
    {EFUSE_BLK0, 3, 1}, 	 // Write protection for WDT_DELAY_SEL,
};

static const esp_efuse_desc_t WR_DIS_SPI_BOOT_CRYPT_CNT[] = {
    {EFUSE_BLK0, 4, 1}, 	 // Write protection for SPI_BOOT_CRYPT_CNT,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_KEY_REVOKE0[] = {
    {EFUSE_BLK0, 5, 1}, 	 // Write protection for SECURE_BOOT_KEY_REVOKE0,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_KEY_REVOKE1[] = {
    {EFUSE_BLK0, 6, 1}, 	 // Write protection for SECURE_BOOT_KEY_REVOKE1,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_KEY_REVOKE2[] = {
    {EFUSE_BLK0, 7, 1}, 	 // Write protection for SECURE_BOOT_KEY_REVOKE2,
};

static const esp_efuse_desc_t WR_DIS_KEY0_PURPOSE[] = {
    {EFUSE_BLK0, 8, 1}, 	 // Write protection for key_purpose. KEY0,
};

static const esp_efuse_desc_t WR_DIS_KEY1_PURPOSE[] = {
    {EFUSE_BLK0, 9, 1}, 	 // Write protection for key_purpose. KEY1,
};

static const esp_efuse_desc_t WR_DIS_KEY2_PURPOSE[] = {
    {EFUSE_BLK0, 10, 1}, 	 // Write protection for key_purpose. KEY2,
};

static const esp_efuse_desc_t WR_DIS_KEY3_PURPOSE[] = {
    {EFUSE_BLK0, 11, 1}, 	 // Write protection for key_purpose. KEY3,
};

static const esp_efuse_desc_t WR_DIS_KEY4_PURPOSE[] = {
    {EFUSE_BLK0, 12, 1}, 	 // Write protection for key_purpose. KEY4,
};

static const esp_efuse_desc_t WR_DIS_KEY5_PURPOSE[] = {
    {EFUSE_BLK0, 13, 1}, 	 // Write protection for key_purpose. KEY5,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_EN[] = {
    {EFUSE_BLK0, 15, 1}, 	 // Write protection for SECURE_BOOT_EN,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_AGGRESSIVE_REVOKE[] = {
    {EFUSE_BLK0, 16, 1}, 	 // Write protection for SECURE_BOOT_AGGRESSIVE_REVOKE,
};

static const esp_efuse_desc_t WR_DIS_GROUP_3[] = {
    {EFUSE_BLK0, 18, 1}, 	 // Write protection for FLASH_TPUW DIS_DOWNLOAD_MODE DIS_DIRECT_BOOT DIS_USB_SERIAL_JTAG_ROM_PRINT DIS_TINY_BASIC DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE ENABLE_SECURITY_DOWNLOAD UART_PRINT_CONTROL PIN_POWER_SELECTION FLASH_TYPE FORCE_SEND_RESUME SECURE_VERSION,
};

static const esp_efuse_desc_t WR_DIS_BLK1[] = {
    {EFUSE_BLK0, 20, 1}, 	 // Write protection for EFUSE_BLK1.  MAC_SPI_8M_SYS,
};

static const esp_efuse_desc_t WR_DIS_SYS_DATA_PART1[] = {
    {EFUSE_BLK0, 21, 1}, 	 // Write protection for EFUSE_BLK2.  SYS_DATA_PART1,
};

static const esp_efuse_desc_t WR_DIS_USER_DATA[] = {
    {EFUSE_BLK0, 22, 1}, 	 // Write protection for EFUSE_BLK3.  USER_DATA,
};

static const esp_efuse_desc_t WR_DIS_KEY0[] = {
    {EFUSE_BLK0, 23, 1}, 	 // Write protection for EFUSE_BLK4.  KEY0,
};

static const esp_efuse_desc_t WR_DIS_KEY1[] = {
    {EFUSE_BLK0, 24, 1}, 	 // Write protection for EFUSE_BLK5.  KEY1,
};

static const esp_efuse_desc_t WR_DIS_KEY2[] = {
    {EFUSE_BLK0, 25, 1}, 	 // Write protection for EFUSE_BLK6.  KEY2,
};

static const esp_efuse_desc_t WR_DIS_KEY3[] = {
    {EFUSE_BLK0, 26, 1}, 	 // Write protection for EFUSE_BLK7.  KEY3,
};

static const esp_efuse_desc_t WR_DIS_KEY4[] = {
    {EFUSE_BLK0, 27, 1}, 	 // Write protection for EFUSE_BLK8.  KEY4,
};

static const esp_efuse_desc_t WR_DIS_KEY5[] = {
    {EFUSE_BLK0, 28, 1}, 	 // Write protection for EFUSE_BLK9.  KEY5,
};

static const esp_efuse_desc_t WR_DIS_SYS_DATA_PART2[] = {
    {EFUSE_BLK0, 29, 1}, 	 // Write protection for EFUSE_BLK10. SYS_DATA_PART2,
};

static const esp_efuse_desc_t RD_DIS[] = {
    {EFUSE_BLK0, 32, 7}, 	 // Read protection,
};

static const esp_efuse_desc_t RD_DIS_KEY0[] = {
    {EFUSE_BLK0, 32, 1}, 	 // Read protection for EFUSE_BLK4.  KEY0,
};

static const esp_efuse_desc_t RD_DIS_KEY1[] = {
    {EFUSE_BLK0, 33, 1}, 	 // Read protection for EFUSE_BLK5.  KEY1,
};

static const esp_efuse_desc_t RD_DIS_KEY2[] = {
    {EFUSE_BLK0, 34, 1}, 	 // Read protection for EFUSE_BLK6.  KEY2,
};

static const esp_efuse_desc_t RD_DIS_KEY3[] = {
    {EFUSE_BLK0, 35, 1}, 	 // Read protection for EFUSE_BLK7.  KEY3,
};

static const esp_efuse_desc_t RD_DIS_KEY4[] = {
    {EFUSE_BLK0, 36, 1}, 	 // Read protection for EFUSE_BLK8.  KEY4,
};

static const esp_efuse_desc_t RD_DIS_KEY5[] = {
    {EFUSE_BLK0, 37, 1}, 	 // Read protection for EFUSE_BLK9.  KEY5,
};

static const esp_efuse_desc_t RD_DIS_SYS_DATA_PART2[] = {
    {EFUSE_BLK0, 38, 1}, 	 // Read protection for EFUSE_BLK10. SYS_DATA_PART2,
};

static const esp_efuse_desc_t DIS_ICACHE[] = {
    {EFUSE_BLK0, 40, 1}, 	 // Disable Icache,
};

static const esp_efuse_desc_t DIS_USB_JTAG[] = {
    {EFUSE_BLK0, 41, 1}, 	 // Disable USB JTAG,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_ICACHE[] = {
    {EFUSE_BLK0, 42, 1}, 	 // Disable Icache in download mode,
};

static const esp_efuse_desc_t DIS_USB_DEVICE[] = {
    {EFUSE_BLK0, 43, 1}, 	 // Disable USB_DEVICE,
};

static const esp_efuse_desc_t DIS_FORCE_DOWNLOAD[] = {
    {EFUSE_BLK0, 44, 1}, 	 // Disable force chip go to download mode function,
};

static const esp_efuse_desc_t DIS_TWAI[] = {
    {EFUSE_BLK0, 46, 1}, 	 // Disable TWAI function,
};

static const esp_efuse_desc_t JTAG_SEL_ENABLE[] = {
    {EFUSE_BLK0, 47, 1}, 	 // Set this bit to enable selection between usb_to_jtag and pad_to_jtag through strapping gpio10 when both reg_dis_usb_jtag and reg_dis_pad_jtag are equal to 0.,
};

static const esp_efuse_desc_t SOFT_DIS_JTAG[] = {
    {EFUSE_BLK0, 48, 3}, 	 // Set these bits to disable JTAG in the soft way (odd number 1 means disable). JTAG can be enabled in HMAC module.,
};

static const esp_efuse_desc_t DIS_PAD_JTAG[] = {
    {EFUSE_BLK0, 51, 1}, 	 // Disable JTAG in the hard way. JTAG is disabled permanently.,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    {EFUSE_BLK0, 52, 1}, 	 // Disable flash encryption when in download boot modes.,
};

static const esp_efuse_desc_t USB_DREFH[] = {
    {EFUSE_BLK0, 53, 2}, 	 // Controls single-end input threshold vrefh 1.76 V to 2 V with step of 80 mV stored in eFuse.,
};

static const esp_efuse_desc_t USB_DREFL[] = {
    {EFUSE_BLK0, 55, 2}, 	 // Controls single-end input threshold vrefl 0.8 V to 1.04 V with step of 80 mV stored in eFuse.,
};

static const esp_efuse_desc_t USB_EXCHG_PINS[] = {
    {EFUSE_BLK0, 57, 1}, 	 // Exchange D+ D- pins,
};

static const esp_efuse_desc_t VDD_SPI_AS_GPIO[] = {
    {EFUSE_BLK0, 58, 1}, 	 // Set this bit to vdd spi pin function as gpio,
};

static const esp_efuse_desc_t BTLC_GPIO_ENABLE[] = {
    {EFUSE_BLK0, 59, 2}, 	 // Enable btlc gpio,
};

static const esp_efuse_desc_t POWERGLITCH_EN[] = {
    {EFUSE_BLK0, 61, 1}, 	 // Set this bit to enable power glitch function,
};

static const esp_efuse_desc_t POWER_GLITCH_DSENSE[] = {
    {EFUSE_BLK0, 62, 2}, 	 // Sample delay configuration of power glitch,
};

static const esp_efuse_desc_t WDT_DELAY_SEL[] = {
    {EFUSE_BLK0, 80, 2}, 	 // Select RTC WDT time out threshold,
};

static const esp_efuse_desc_t SPI_BOOT_CRYPT_CNT[] = {
    {EFUSE_BLK0, 82, 3}, 	 // SPI boot encrypt decrypt enable. odd number 1 enable. even number 1 disable,
};

static const esp_efuse_desc_t SECURE_BOOT_KEY_REVOKE0[] = {
    {EFUSE_BLK0, 85, 1}, 	 // Enable revoke first secure boot key,
};

static const esp_efuse_desc_t SECURE_BOOT_KEY_REVOKE1[] = {
    {EFUSE_BLK0, 86, 1}, 	 // Enable revoke second secure boot key,
};

static const esp_efuse_desc_t SECURE_BOOT_KEY_REVOKE2[] = {
    {EFUSE_BLK0, 87, 1}, 	 // Enable revoke third secure boot key,
};

static const esp_efuse_desc_t KEY_PURPOSE_0[] = {
    {EFUSE_BLK0, 88, 4}, 	 // Key0 purpose,
};

static const esp_efuse_desc_t KEY_PURPOSE_1[] = {
    {EFUSE_BLK0, 92, 4}, 	 // Key1 purpose,
};

static const esp_efuse_desc_t KEY_PURPOSE_2[] = {
    {EFUSE_BLK0, 96, 4}, 	 // Key2 purpose,
};

static const esp_efuse_desc_t KEY_PURPOSE_3[] = {
    {EFUSE_BLK0, 100, 4}, 	 // Key3 purpose,
};

static const esp_efuse_desc_t KEY_PURPOSE_4[] = {
    {EFUSE_BLK0, 104, 4}, 	 // Key4 purpose,
};

static const esp_efuse_desc_t KEY_PURPOSE_5[] = {
    {EFUSE_BLK0, 108, 4}, 	 // Key5 purpose,
};

static const esp_efuse_desc_t SECURE_BOOT_EN[] = {
    {EFUSE_BLK0, 116, 1}, 	 // Secure boot enable,
};

static const esp_efuse_desc_t SECURE_BOOT_AGGRESSIVE_REVOKE[] = {
    {EFUSE_BLK0, 117, 1}, 	 // Enable aggressive secure boot revoke,
};

static const esp_efuse_desc_t FLASH_TPUW[] = {
    {EFUSE_BLK0, 124, 4}, 	 // Flash wait time after power up. (unit is ms). When value is 15. the time is 30 ms,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 128, 1}, 	 // Disble download mode include boot_mode[3:0] is 0 1 2 3 6 7,
};

static const esp_efuse_desc_t DIS_DIRECT_BOOT[] = {
    {EFUSE_BLK0, 129, 1}, 	 // Disable direct boot mode,
};

static const esp_efuse_desc_t DIS_USB_SERIAL_JTAG_ROM_PRINT[] = {
    {EFUSE_BLK0, 130, 1}, 	 // Disable usb serial jtag print during rom boot,
};

static const esp_efuse_desc_t DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 132, 1}, 	 // Disable download through USB-Serial-JTAG,
};

static const esp_efuse_desc_t ENABLE_SECURITY_DOWNLOAD[] = {
    {EFUSE_BLK0, 133, 1}, 	 // Enable security download mode,
};

static const esp_efuse_desc_t UART_PRINT_CONTROL[] = {
    {EFUSE_BLK0, 134, 2}, 	 // b00:force print. b01:control by GPIO8 - low level print. b10:control by GPIO8 - high level print. b11:force disable print.,
};

static const esp_efuse_desc_t FORCE_SEND_RESUME[] = {
    {EFUSE_BLK0, 141, 1}, 	 // Force ROM code to send a resume command during SPI boot,
};

static const esp_efuse_desc_t SECURE_VERSION[] = {
    {EFUSE_BLK0, 142, 16}, 	 // Secure version for anti-rollback,
};

static const esp_efuse_desc_t BOOT_DISABLE_FAST_WAKE[] = {
    {EFUSE_BLK0, 158, 1}, 	 // Fast verify on wake option in ROM for Secure Boot,
};

static const esp_efuse_desc_t MAC_FACTORY[] = {
    {EFUSE_BLK1, 40, 8}, 	 // Factory MAC addr [0],
    {EFUSE_BLK1, 32, 8}, 	 // Factory MAC addr [1],
    {EFUSE_BLK1, 24, 8}, 	 // Factory MAC addr [2],
    {EFUSE_BLK1, 16, 8}, 	 // Factory MAC addr [3],
    {EFUSE_BLK1, 8, 8}, 	 // Factory MAC addr [4],
    {EFUSE_BLK1, 0, 8}, 	 // Factory MAC addr [5],
};

static const esp_efuse_desc_t MAC_EXT[] = {
    {EFUSE_BLK1, 123, 8}, 	 // Extend MAC addr [0],
    {EFUSE_BLK1, 131, 8}, 	 // Extend MAC addr [1],
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_CLK[] = {
    {EFUSE_BLK1, 48, 6}, 	 // SPI_PAD_configure CLK,
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_Q_D1[] = {
    {EFUSE_BLK1, 54, 6}, 	 // SPI_PAD_configure Q(D1),
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_D_D0[] = {
    {EFUSE_BLK1, 60, 6}, 	 // SPI_PAD_configure D(D0),
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_CS[] = {
    {EFUSE_BLK1, 66, 6}, 	 // SPI_PAD_configure CS,
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_HD_D3[] = {
    {EFUSE_BLK1, 72, 6}, 	 // SPI_PAD_configure HD(D3),
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_WP_D2[] = {
    {EFUSE_BLK1, 78, 6}, 	 // SPI_PAD_configure WP(D2),
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_DQS[] = {
    {EFUSE_BLK1, 84, 6}, 	 // SPI_PAD_configure DQS,
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_D4[] = {
    {EFUSE_BLK1, 90, 6}, 	 // SPI_PAD_configure D4,
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_D5[] = {
    {EFUSE_BLK1, 96, 6}, 	 // SPI_PAD_configure D5,
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_D6[] = {
    {EFUSE_BLK1, 102, 6}, 	 // SPI_PAD_configure D6,
};

static const esp_efuse_desc_t SPI_PAD_CONFIG_D7[] = {
    {EFUSE_BLK1, 108, 6}, 	 // SPI_PAD_configure D7,
};

static const esp_efuse_desc_t WAFER_VERSION[] = {
    {EFUSE_BLK1, 114, 3}, 	 // WAFER version,
};

static const esp_efuse_desc_t PKG_VERSION[] = {
    {EFUSE_BLK1, 117, 3}, 	 // Package version 0:ESP32H4,
};

static const esp_efuse_desc_t BLOCK1_VERSION[] = {
    {EFUSE_BLK1, 120, 3}, 	 // BLOCK1 efuse version,
};

static const esp_efuse_desc_t OPTIONAL_UNIQUE_ID[] = {
    {EFUSE_BLK2, 0, 128}, 	 // Optional unique 128-bit ID,
};

static const esp_efuse_desc_t BLOCK2_VERSION[] = {
    {EFUSE_BLK2, 128, 3}, 	 // Version of BLOCK2,
};

static const esp_efuse_desc_t TEMP_CALIB[] = {
    {EFUSE_BLK2, 131, 9}, 	 // Temperature calibration data,
};

static const esp_efuse_desc_t OCODE[] = {
    {EFUSE_BLK2, 140, 8}, 	 // ADC OCode,
};

static const esp_efuse_desc_t ADC1_INIT_CODE_ATTEN0[] = {
    {EFUSE_BLK2, 148, 10}, 	 // ADC1 init code at atten0,
};

static const esp_efuse_desc_t ADC1_INIT_CODE_ATTEN1[] = {
    {EFUSE_BLK2, 158, 10}, 	 // ADC1 init code at atten1,
};

static const esp_efuse_desc_t ADC1_INIT_CODE_ATTEN2[] = {
    {EFUSE_BLK2, 168, 10}, 	 // ADC1 init code at atten2,
};

static const esp_efuse_desc_t ADC1_INIT_CODE_ATTEN3[] = {
    {EFUSE_BLK2, 178, 10}, 	 // ADC1 init code at atten3,
};

static const esp_efuse_desc_t ADC1_CAL_VOL_ATTEN0[] = {
    {EFUSE_BLK2, 188, 10}, 	 // ADC1 calibration voltage at atten0,
};

static const esp_efuse_desc_t ADC1_CAL_VOL_ATTEN1[] = {
    {EFUSE_BLK2, 198, 10}, 	 // ADC1 calibration voltage at atten1,
};

static const esp_efuse_desc_t ADC1_CAL_VOL_ATTEN2[] = {
    {EFUSE_BLK2, 208, 10}, 	 // ADC1 calibration voltage at atten2,
};

static const esp_efuse_desc_t ADC1_CAL_VOL_ATTEN3[] = {
    {EFUSE_BLK2, 218, 10}, 	 // ADC1 calibration voltage at atten3,
};

static const esp_efuse_desc_t USER_DATA[] = {
    {EFUSE_BLK3, 0, 256}, 	 // User data,
};

static const esp_efuse_desc_t USER_DATA_MAC_CUSTOM[] = {
    {EFUSE_BLK3, 200, 48}, 	 // Custom MAC,
};

static const esp_efuse_desc_t KEY0[] = {
    {EFUSE_BLK4, 0, 256}, 	 // Key0 or user data,
};

static const esp_efuse_desc_t KEY1[] = {
    {EFUSE_BLK5, 0, 256}, 	 // Key1 or user data,
};

static const esp_efuse_desc_t KEY2[] = {
    {EFUSE_BLK6, 0, 256}, 	 // Key2 or user data,
};

static const esp_efuse_desc_t KEY3[] = {
    {EFUSE_BLK7, 0, 256}, 	 // Key3 or user data,
};

static const esp_efuse_desc_t KEY4[] = {
    {EFUSE_BLK8, 0, 256}, 	 // Key4 or user data,
};

static const esp_efuse_desc_t KEY5[] = {
    {EFUSE_BLK9, 0, 256}, 	 // Key5 or user data,
};

static const esp_efuse_desc_t SYS_DATA_PART2[] = {
    {EFUSE_BLK10, 0, 256}, 	 // System configuration,
};





const esp_efuse_desc_t* ESP_EFUSE_WR_DIS[] = {
    &WR_DIS[0],    		// Write protection
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RD_DIS[] = {
    &WR_DIS_RD_DIS[0],    		// Write protection for RD_DIS_KEY0 RD_DIS_KEY1 RD_DIS_KEY2 RD_DIS_KEY3 RD_DIS_KEY4 RD_DIS_KEY5 RD_DIS_SYS_DATA_PART2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_GROUP_1[] = {
    &WR_DIS_GROUP_1[0],    		// Write protection for DIS_ICACHE DIS_DOWNLOAD_ICACHE DIS_FORCE_DOWNLOAD DIS_USB DIS_TWAI SOFT_DIS_JTAG DIS_DOWNLOAD_MANUAL_ENCRYPT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_GROUP_2[] = {
    &WR_DIS_GROUP_2[0],    		// Write protection for WDT_DELAY_SEL
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_BOOT_CRYPT_CNT[] = {
    &WR_DIS_SPI_BOOT_CRYPT_CNT[0],    		// Write protection for SPI_BOOT_CRYPT_CNT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE0[] = {
    &WR_DIS_SECURE_BOOT_KEY_REVOKE0[0],    		// Write protection for SECURE_BOOT_KEY_REVOKE0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE1[] = {
    &WR_DIS_SECURE_BOOT_KEY_REVOKE1[0],    		// Write protection for SECURE_BOOT_KEY_REVOKE1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE2[] = {
    &WR_DIS_SECURE_BOOT_KEY_REVOKE2[0],    		// Write protection for SECURE_BOOT_KEY_REVOKE2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY0_PURPOSE[] = {
    &WR_DIS_KEY0_PURPOSE[0],    		// Write protection for key_purpose. KEY0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY1_PURPOSE[] = {
    &WR_DIS_KEY1_PURPOSE[0],    		// Write protection for key_purpose. KEY1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY2_PURPOSE[] = {
    &WR_DIS_KEY2_PURPOSE[0],    		// Write protection for key_purpose. KEY2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY3_PURPOSE[] = {
    &WR_DIS_KEY3_PURPOSE[0],    		// Write protection for key_purpose. KEY3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY4_PURPOSE[] = {
    &WR_DIS_KEY4_PURPOSE[0],    		// Write protection for key_purpose. KEY4
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY5_PURPOSE[] = {
    &WR_DIS_KEY5_PURPOSE[0],    		// Write protection for key_purpose. KEY5
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_EN[] = {
    &WR_DIS_SECURE_BOOT_EN[0],    		// Write protection for SECURE_BOOT_EN
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_AGGRESSIVE_REVOKE[] = {
    &WR_DIS_SECURE_BOOT_AGGRESSIVE_REVOKE[0],    		// Write protection for SECURE_BOOT_AGGRESSIVE_REVOKE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_GROUP_3[] = {
    &WR_DIS_GROUP_3[0],    		// Write protection for FLASH_TPUW DIS_DOWNLOAD_MODE DIS_DIRECT_BOOT DIS_USB_SERIAL_JTAG_ROM_PRINT DIS_TINY_BASIC DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE ENABLE_SECURITY_DOWNLOAD UART_PRINT_CONTROL PIN_POWER_SELECTION FLASH_TYPE FORCE_SEND_RESUME SECURE_VERSION
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK1[] = {
    &WR_DIS_BLK1[0],    		// Write protection for EFUSE_BLK1.  MAC_SPI_8M_SYS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SYS_DATA_PART1[] = {
    &WR_DIS_SYS_DATA_PART1[0],    		// Write protection for EFUSE_BLK2.  SYS_DATA_PART1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_USER_DATA[] = {
    &WR_DIS_USER_DATA[0],    		// Write protection for EFUSE_BLK3.  USER_DATA
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY0[] = {
    &WR_DIS_KEY0[0],    		// Write protection for EFUSE_BLK4.  KEY0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY1[] = {
    &WR_DIS_KEY1[0],    		// Write protection for EFUSE_BLK5.  KEY1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY2[] = {
    &WR_DIS_KEY2[0],    		// Write protection for EFUSE_BLK6.  KEY2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY3[] = {
    &WR_DIS_KEY3[0],    		// Write protection for EFUSE_BLK7.  KEY3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY4[] = {
    &WR_DIS_KEY4[0],    		// Write protection for EFUSE_BLK8.  KEY4
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY5[] = {
    &WR_DIS_KEY5[0],    		// Write protection for EFUSE_BLK9.  KEY5
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SYS_DATA_PART2[] = {
    &WR_DIS_SYS_DATA_PART2[0],    		// Write protection for EFUSE_BLK10. SYS_DATA_PART2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS[] = {
    &RD_DIS[0],    		// Read protection
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY0[] = {
    &RD_DIS_KEY0[0],    		// Read protection for EFUSE_BLK4.  KEY0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY1[] = {
    &RD_DIS_KEY1[0],    		// Read protection for EFUSE_BLK5.  KEY1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY2[] = {
    &RD_DIS_KEY2[0],    		// Read protection for EFUSE_BLK6.  KEY2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY3[] = {
    &RD_DIS_KEY3[0],    		// Read protection for EFUSE_BLK7.  KEY3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY4[] = {
    &RD_DIS_KEY4[0],    		// Read protection for EFUSE_BLK8.  KEY4
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY5[] = {
    &RD_DIS_KEY5[0],    		// Read protection for EFUSE_BLK9.  KEY5
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_SYS_DATA_PART2[] = {
    &RD_DIS_SYS_DATA_PART2[0],    		// Read protection for EFUSE_BLK10. SYS_DATA_PART2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_ICACHE[] = {
    &DIS_ICACHE[0],    		// Disable Icache
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_JTAG[] = {
    &DIS_USB_JTAG[0],    		// Disable USB JTAG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_ICACHE[] = {
    &DIS_DOWNLOAD_ICACHE[0],    		// Disable Icache in download mode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_DEVICE[] = {
    &DIS_USB_DEVICE[0],    		// Disable USB_DEVICE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_FORCE_DOWNLOAD[] = {
    &DIS_FORCE_DOWNLOAD[0],    		// Disable force chip go to download mode function
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_TWAI[] = {
    &DIS_TWAI[0],    		// Disable TWAI function
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_JTAG_SEL_ENABLE[] = {
    &JTAG_SEL_ENABLE[0],    		// Set this bit to enable selection between usb_to_jtag and pad_to_jtag through strapping gpio10 when both reg_dis_usb_jtag and reg_dis_pad_jtag are equal to 0.
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SOFT_DIS_JTAG[] = {
    &SOFT_DIS_JTAG[0],    		// Set these bits to disable JTAG in the soft way (odd number 1 means disable). JTAG can be enabled in HMAC module.
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_PAD_JTAG[] = {
    &DIS_PAD_JTAG[0],    		// Disable JTAG in the hard way. JTAG is disabled permanently.
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    &DIS_DOWNLOAD_MANUAL_ENCRYPT[0],    		// Disable flash encryption when in download boot modes.
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USB_DREFH[] = {
    &USB_DREFH[0],    		// Controls single-end input threshold vrefh 1.76 V to 2 V with step of 80 mV stored in eFuse.
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USB_DREFL[] = {
    &USB_DREFL[0],    		// Controls single-end input threshold vrefl 0.8 V to 1.04 V with step of 80 mV stored in eFuse.
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USB_EXCHG_PINS[] = {
    &USB_EXCHG_PINS[0],    		// Exchange D+ D- pins
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_VDD_SPI_AS_GPIO[] = {
    &VDD_SPI_AS_GPIO[0],    		// Set this bit to vdd spi pin function as gpio
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_BTLC_GPIO_ENABLE[] = {
    &BTLC_GPIO_ENABLE[0],    		// Enable btlc gpio
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_POWERGLITCH_EN[] = {
    &POWERGLITCH_EN[0],    		// Set this bit to enable power glitch function
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_POWER_GLITCH_DSENSE[] = {
    &POWER_GLITCH_DSENSE[0],    		// Sample delay configuration of power glitch
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WDT_DELAY_SEL[] = {
    &WDT_DELAY_SEL[0],    		// Select RTC WDT time out threshold
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_BOOT_CRYPT_CNT[] = {
    &SPI_BOOT_CRYPT_CNT[0],    		// SPI boot encrypt decrypt enable. odd number 1 enable. even number 1 disable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_KEY_REVOKE0[] = {
    &SECURE_BOOT_KEY_REVOKE0[0],    		// Enable revoke first secure boot key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_KEY_REVOKE1[] = {
    &SECURE_BOOT_KEY_REVOKE1[0],    		// Enable revoke second secure boot key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_KEY_REVOKE2[] = {
    &SECURE_BOOT_KEY_REVOKE2[0],    		// Enable revoke third secure boot key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_0[] = {
    &KEY_PURPOSE_0[0],    		// Key0 purpose
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_1[] = {
    &KEY_PURPOSE_1[0],    		// Key1 purpose
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_2[] = {
    &KEY_PURPOSE_2[0],    		// Key2 purpose
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_3[] = {
    &KEY_PURPOSE_3[0],    		// Key3 purpose
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_4[] = {
    &KEY_PURPOSE_4[0],    		// Key4 purpose
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_5[] = {
    &KEY_PURPOSE_5[0],    		// Key5 purpose
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_EN[] = {
    &SECURE_BOOT_EN[0],    		// Secure boot enable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE[] = {
    &SECURE_BOOT_AGGRESSIVE_REVOKE[0],    		// Enable aggressive secure boot revoke
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_TPUW[] = {
    &FLASH_TPUW[0],    		// Flash wait time after power up. (unit is ms). When value is 15. the time is 30 ms
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MODE[] = {
    &DIS_DOWNLOAD_MODE[0],    		// Disble download mode include boot_mode[3:0] is 0 1 2 3 6 7
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DIRECT_BOOT[] = {
    &DIS_DIRECT_BOOT[0],    		// Disable direct boot mode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT[] = {
    &DIS_USB_SERIAL_JTAG_ROM_PRINT[0],    		// Disable usb serial jtag print during rom boot
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[] = {
    &DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[0],    		// Disable download through USB-Serial-JTAG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ENABLE_SECURITY_DOWNLOAD[] = {
    &ENABLE_SECURITY_DOWNLOAD[0],    		// Enable security download mode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_UART_PRINT_CONTROL[] = {
    &UART_PRINT_CONTROL[0],    		// b00:force print. b01:control by GPIO8 - low level print. b10:control by GPIO8 - high level print. b11:force disable print.
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FORCE_SEND_RESUME[] = {
    &FORCE_SEND_RESUME[0],    		// Force ROM code to send a resume command during SPI boot
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_VERSION[] = {
    &SECURE_VERSION[0],    		// Secure version for anti-rollback
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_BOOT_DISABLE_FAST_WAKE[] = {
    &BOOT_DISABLE_FAST_WAKE[0],    		// Fast verify on wake option in ROM for Secure Boot
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

const esp_efuse_desc_t* ESP_EFUSE_MAC_EXT[] = {
    &MAC_EXT[0],    		// Extend MAC addr [0]
    &MAC_EXT[1],    		// Extend MAC addr [1]
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_CLK[] = {
    &SPI_PAD_CONFIG_CLK[0],    		// SPI_PAD_configure CLK
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_Q_D1[] = {
    &SPI_PAD_CONFIG_Q_D1[0],    		// SPI_PAD_configure Q(D1)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_D_D0[] = {
    &SPI_PAD_CONFIG_D_D0[0],    		// SPI_PAD_configure D(D0)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_CS[] = {
    &SPI_PAD_CONFIG_CS[0],    		// SPI_PAD_configure CS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_HD_D3[] = {
    &SPI_PAD_CONFIG_HD_D3[0],    		// SPI_PAD_configure HD(D3)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_WP_D2[] = {
    &SPI_PAD_CONFIG_WP_D2[0],    		// SPI_PAD_configure WP(D2)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_DQS[] = {
    &SPI_PAD_CONFIG_DQS[0],    		// SPI_PAD_configure DQS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_D4[] = {
    &SPI_PAD_CONFIG_D4[0],    		// SPI_PAD_configure D4
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_D5[] = {
    &SPI_PAD_CONFIG_D5[0],    		// SPI_PAD_configure D5
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_D6[] = {
    &SPI_PAD_CONFIG_D6[0],    		// SPI_PAD_configure D6
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_D7[] = {
    &SPI_PAD_CONFIG_D7[0],    		// SPI_PAD_configure D7
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WAFER_VERSION[] = {
    &WAFER_VERSION[0],    		// WAFER version
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PKG_VERSION[] = {
    &PKG_VERSION[0],    		// Package version 0:ESP32H4
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_BLOCK1_VERSION[] = {
    &BLOCK1_VERSION[0],    		// BLOCK1 efuse version
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_OPTIONAL_UNIQUE_ID[] = {
    &OPTIONAL_UNIQUE_ID[0],    		// Optional unique 128-bit ID
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_BLOCK2_VERSION[] = {
    &BLOCK2_VERSION[0],    		// Version of BLOCK2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_TEMP_CALIB[] = {
    &TEMP_CALIB[0],    		// Temperature calibration data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_OCODE[] = {
    &OCODE[0],    		// ADC OCode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_INIT_CODE_ATTEN0[] = {
    &ADC1_INIT_CODE_ATTEN0[0],    		// ADC1 init code at atten0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_INIT_CODE_ATTEN1[] = {
    &ADC1_INIT_CODE_ATTEN1[0],    		// ADC1 init code at atten1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_INIT_CODE_ATTEN2[] = {
    &ADC1_INIT_CODE_ATTEN2[0],    		// ADC1 init code at atten2
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

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CAL_VOL_ATTEN1[] = {
    &ADC1_CAL_VOL_ATTEN1[0],    		// ADC1 calibration voltage at atten1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CAL_VOL_ATTEN2[] = {
    &ADC1_CAL_VOL_ATTEN2[0],    		// ADC1 calibration voltage at atten2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CAL_VOL_ATTEN3[] = {
    &ADC1_CAL_VOL_ATTEN3[0],    		// ADC1 calibration voltage at atten3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USER_DATA[] = {
    &USER_DATA[0],    		// User data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USER_DATA_MAC_CUSTOM[] = {
    &USER_DATA_MAC_CUSTOM[0],    		// Custom MAC
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY0[] = {
    &KEY0[0],    		// Key0 or user data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY1[] = {
    &KEY1[0],    		// Key1 or user data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY2[] = {
    &KEY2[0],    		// Key2 or user data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY3[] = {
    &KEY3[0],    		// Key3 or user data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY4[] = {
    &KEY4[0],    		// Key4 or user data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY5[] = {
    &KEY5[0],    		// Key5 or user data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SYS_DATA_PART2[] = {
    &SYS_DATA_PART2[0],    		// System configuration
    NULL
};
