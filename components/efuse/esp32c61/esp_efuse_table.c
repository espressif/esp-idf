/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_efuse.h"
#include <assert.h>
#include "esp_efuse_table.h"

// md5_digest_table af9aaa79feb0970d90f35360a5113f03
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

static const esp_efuse_desc_t WR_DIS_DIS_USB_JTAG[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of DIS_USB_JTAG,
};

static const esp_efuse_desc_t WR_DIS_DIS_FORCE_DOWNLOAD[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of DIS_FORCE_DOWNLOAD,
};

static const esp_efuse_desc_t WR_DIS_SPI_DOWNLOAD_MSPI_DIS[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of SPI_DOWNLOAD_MSPI_DIS,
};

static const esp_efuse_desc_t WR_DIS_JTAG_SEL_ENABLE[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of JTAG_SEL_ENABLE,
};

static const esp_efuse_desc_t WR_DIS_DIS_PAD_JTAG[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of DIS_PAD_JTAG,
};

static const esp_efuse_desc_t WR_DIS_DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of DIS_DOWNLOAD_MANUAL_ENCRYPT,
};

static const esp_efuse_desc_t WR_DIS_HYS_EN_PAD[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of HYS_EN_PAD,
};

static const esp_efuse_desc_t WR_DIS_DIS_WIFI6[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of DIS_WIFI6,
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

static const esp_efuse_desc_t WR_DIS_SEC_DPA_LEVEL[] = {
    {EFUSE_BLK0, 14, 1}, 	 // [] wr_dis of SEC_DPA_LEVEL,
};

static const esp_efuse_desc_t WR_DIS_XTS_DPA_CLK_ENABLE[] = {
    {EFUSE_BLK0, 14, 1}, 	 // [] wr_dis of XTS_DPA_CLK_ENABLE,
};

static const esp_efuse_desc_t WR_DIS_XTS_DPA_PSEUDO_LEVEL[] = {
    {EFUSE_BLK0, 14, 1}, 	 // [] wr_dis of XTS_DPA_PSEUDO_LEVEL,
};

static const esp_efuse_desc_t WR_DIS_ECDSA_DISABLE_P192[] = {
    {EFUSE_BLK0, 14, 1}, 	 // [] wr_dis of ECDSA_DISABLE_P192,
};

static const esp_efuse_desc_t WR_DIS_ECC_FORCE_CONST_TIME[] = {
    {EFUSE_BLK0, 14, 1}, 	 // [] wr_dis of ECC_FORCE_CONST_TIME,
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
    {EFUSE_BLK0, 18, 1}, 	 // [] wr_dis of DIS_DIRECT_BOOT,
};

static const esp_efuse_desc_t WR_DIS_DIS_USB_SERIAL_JTAG_ROM_PRINT[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] wr_dis of DIS_USB_SERIAL_JTAG_ROM_PRINT,
};

static const esp_efuse_desc_t WR_DIS_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] wr_dis of DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE,
};

static const esp_efuse_desc_t WR_DIS_ENABLE_SECURITY_DOWNLOAD[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] wr_dis of ENABLE_SECURITY_DOWNLOAD,
};

static const esp_efuse_desc_t WR_DIS_UART_PRINT_CONTROL[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] wr_dis of UART_PRINT_CONTROL,
};

static const esp_efuse_desc_t WR_DIS_FORCE_SEND_RESUME[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] wr_dis of FORCE_SEND_RESUME,
};

static const esp_efuse_desc_t WR_DIS_SECURE_VERSION[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] wr_dis of SECURE_VERSION,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_DISABLE_FAST_WAKE[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] wr_dis of SECURE_BOOT_DISABLE_FAST_WAKE,
};

static const esp_efuse_desc_t WR_DIS_BLK1[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of BLOCK1,
};

static const esp_efuse_desc_t WR_DIS_MAC[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [WR_DIS.MAC_FACTORY] wr_dis of MAC,
};

static const esp_efuse_desc_t WR_DIS_WAFER_VERSION_MINOR[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of WAFER_VERSION_MINOR,
};

static const esp_efuse_desc_t WR_DIS_WAFER_VERSION_MAJOR[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of WAFER_VERSION_MAJOR,
};

static const esp_efuse_desc_t WR_DIS_DISABLE_WAFER_VERSION_MAJOR[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of DISABLE_WAFER_VERSION_MAJOR,
};

static const esp_efuse_desc_t WR_DIS_DISABLE_BLK_VERSION_MAJOR[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of DISABLE_BLK_VERSION_MAJOR,
};

static const esp_efuse_desc_t WR_DIS_BLK_VERSION_MINOR[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of BLK_VERSION_MINOR,
};

static const esp_efuse_desc_t WR_DIS_BLK_VERSION_MAJOR[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of BLK_VERSION_MAJOR,
};

static const esp_efuse_desc_t WR_DIS_FLASH_CAP[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of FLASH_CAP,
};

static const esp_efuse_desc_t WR_DIS_FLASH_VENDOR[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of FLASH_VENDOR,
};

static const esp_efuse_desc_t WR_DIS_PSRAM_CAP[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of PSRAM_CAP,
};

static const esp_efuse_desc_t WR_DIS_PSRAM_VENDOR[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of PSRAM_VENDOR,
};

static const esp_efuse_desc_t WR_DIS_TEMP[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of TEMP,
};

static const esp_efuse_desc_t WR_DIS_PKG_VERSION[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of PKG_VERSION,
};

static const esp_efuse_desc_t WR_DIS_SYS_DATA_PART1[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of BLOCK2,
};

static const esp_efuse_desc_t WR_DIS_OPTIONAL_UNIQUE_ID[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of OPTIONAL_UNIQUE_ID,
};

static const esp_efuse_desc_t WR_DIS_TEMPERATURE_SENSOR[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of TEMPERATURE_SENSOR,
};

static const esp_efuse_desc_t WR_DIS_OCODE[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of OCODE,
};

static const esp_efuse_desc_t WR_DIS_ADC1_AVE_INIT_CODE_ATTEN0[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_AVE_INIT_CODE_ATTEN0,
};

static const esp_efuse_desc_t WR_DIS_ADC1_AVE_INIT_CODE_ATTEN1[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_AVE_INIT_CODE_ATTEN1,
};

static const esp_efuse_desc_t WR_DIS_ADC1_AVE_INIT_CODE_ATTEN2[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_AVE_INIT_CODE_ATTEN2,
};

static const esp_efuse_desc_t WR_DIS_ADC1_AVE_INIT_CODE_ATTEN3[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_AVE_INIT_CODE_ATTEN3,
};

static const esp_efuse_desc_t WR_DIS_ADC1_HI_DOUT_ATTEN0[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_HI_DOUT_ATTEN0,
};

static const esp_efuse_desc_t WR_DIS_ADC1_HI_DOUT_ATTEN1[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_HI_DOUT_ATTEN1,
};

static const esp_efuse_desc_t WR_DIS_ADC1_HI_DOUT_ATTEN2[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_HI_DOUT_ATTEN2,
};

static const esp_efuse_desc_t WR_DIS_ADC1_HI_DOUT_ATTEN3[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_HI_DOUT_ATTEN3,
};

static const esp_efuse_desc_t WR_DIS_ADC1_CH0_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_CH0_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t WR_DIS_ADC1_CH1_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_CH1_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t WR_DIS_ADC1_CH2_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_CH2_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t WR_DIS_ADC1_CH3_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_CH3_ATTEN0_INITCODE_DIFF,
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

static const esp_efuse_desc_t WR_DIS_VDD_SPI_AS_GPIO[] = {
    {EFUSE_BLK0, 30, 1}, 	 // [] wr_dis of VDD_SPI_AS_GPIO,
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
    {EFUSE_BLK0, 39, 1}, 	 // [] Represents whether icache is disabled or enabled.\\ 1: disabled\\ 0: enabled,
};

static const esp_efuse_desc_t DIS_USB_JTAG[] = {
    {EFUSE_BLK0, 40, 1}, 	 // [] Represents whether the function of usb switch to jtag is disabled or enabled.\\ 1: disabled\\ 0: enabled,
};

static const esp_efuse_desc_t DIS_FORCE_DOWNLOAD[] = {
    {EFUSE_BLK0, 42, 1}, 	 // [] Represents whether the function that forces chip into download mode is disabled or enabled.\\ 1: disabled\\ 0: enabled,
};

static const esp_efuse_desc_t SPI_DOWNLOAD_MSPI_DIS[] = {
    {EFUSE_BLK0, 43, 1}, 	 // [] Represents whether SPI0 controller during boot_mode_download is disabled or enabled.\\ 1: disabled\\ 0: enabled,
};

static const esp_efuse_desc_t JTAG_SEL_ENABLE[] = {
    {EFUSE_BLK0, 44, 1}, 	 // [] Represents whether the selection between usb_to_jtag and pad_to_jtag through strapping gpio15 when both EFUSE_DIS_PAD_JTAG and EFUSE_DIS_USB_JTAG are equal to 0 is enabled or disabled.\\ 1: enabled\\ 0: disabled,
};

static const esp_efuse_desc_t DIS_PAD_JTAG[] = {
    {EFUSE_BLK0, 45, 1}, 	 // [] Represents whether JTAG is disabled in the hard way(permanently).\\ 1: disabled\\ 0: enabled,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    {EFUSE_BLK0, 46, 1}, 	 // [] Represents whether flash encrypt function is disabled or enabled(except in SPI boot mode).\\ 1: disabled\\ 0: enabled,
};

static const esp_efuse_desc_t USB_EXCHG_PINS[] = {
    {EFUSE_BLK0, 51, 1}, 	 // [] Represents whether the D+ and D- pins is exchanged.\\ 1: exchanged\\ 0: not exchanged,
};

static const esp_efuse_desc_t VDD_SPI_AS_GPIO[] = {
    {EFUSE_BLK0, 52, 1}, 	 // [] Represents whether vdd spi pin is functioned as gpio.\\ 1: functioned\\ 0: not functioned,
};

static const esp_efuse_desc_t WDT_DELAY_SEL[] = {
    {EFUSE_BLK0, 53, 2}, 	 // [] Represents the threshold level of the RTC watchdog STG0 timeout.\\ 0: Original threshold configuration value of STG0 *2 \\1: Original threshold configuration value of STG0 *4 \\2: Original threshold configuration value of STG0 *8 \\3: Original threshold configuration value of STG0 *16,
};

static const esp_efuse_desc_t SPI_BOOT_CRYPT_CNT[] = {
    {EFUSE_BLK0, 55, 3}, 	 // [] Enables flash encryption when 1 or 3 bits are set and disables otherwise {0: "Disable"; 1: "Enable"; 3: "Disable"; 7: "Enable"},
};

static const esp_efuse_desc_t SECURE_BOOT_KEY_REVOKE0[] = {
    {EFUSE_BLK0, 58, 1}, 	 // [] Revoke 1st secure boot key,
};

static const esp_efuse_desc_t SECURE_BOOT_KEY_REVOKE1[] = {
    {EFUSE_BLK0, 59, 1}, 	 // [] Revoke 2nd secure boot key,
};

static const esp_efuse_desc_t SECURE_BOOT_KEY_REVOKE2[] = {
    {EFUSE_BLK0, 60, 1}, 	 // [] Revoke 3rd secure boot key,
};

static const esp_efuse_desc_t KEY_PURPOSE_0[] = {
    {EFUSE_BLK0, 64, 4}, 	 // [KEY0_PURPOSE] Represents the purpose of Key0,
};

static const esp_efuse_desc_t KEY_PURPOSE_1[] = {
    {EFUSE_BLK0, 68, 4}, 	 // [KEY1_PURPOSE] Represents the purpose of Key1,
};

static const esp_efuse_desc_t KEY_PURPOSE_2[] = {
    {EFUSE_BLK0, 72, 4}, 	 // [KEY2_PURPOSE] Represents the purpose of Key2,
};

static const esp_efuse_desc_t KEY_PURPOSE_3[] = {
    {EFUSE_BLK0, 76, 4}, 	 // [KEY3_PURPOSE] Represents the purpose of Key3,
};

static const esp_efuse_desc_t KEY_PURPOSE_4[] = {
    {EFUSE_BLK0, 80, 4}, 	 // [KEY4_PURPOSE] Represents the purpose of Key4,
};

static const esp_efuse_desc_t KEY_PURPOSE_5[] = {
    {EFUSE_BLK0, 84, 4}, 	 // [KEY5_PURPOSE] Represents the purpose of Key5,
};

static const esp_efuse_desc_t SEC_DPA_LEVEL[] = {
    {EFUSE_BLK0, 88, 2}, 	 // [] Represents the spa secure level by configuring the clock random divide mode,
};

static const esp_efuse_desc_t SECURE_BOOT_EN[] = {
    {EFUSE_BLK0, 90, 1}, 	 // [] Represents whether secure boot is enabled or disabled.\\ 1: enabled\\ 0: disabled,
};

static const esp_efuse_desc_t SECURE_BOOT_AGGRESSIVE_REVOKE[] = {
    {EFUSE_BLK0, 91, 1}, 	 // [] Represents whether revoking aggressive secure boot is enabled or disabled.\\ 1: enabled.\\ 0: disabled,
};

static const esp_efuse_desc_t FLASH_TPUW[] = {
    {EFUSE_BLK0, 92, 4}, 	 // [] Represents the flash waiting time after power-up; in unit of ms. When the value less than 15; the waiting time is programmed value. Otherwise; the waiting time is 2 times the programmed value,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 96, 1}, 	 // [] Represents whether Download mode is disable or enable.\\ 1. Disable\\ 0: Enable,
};

static const esp_efuse_desc_t DIS_DIRECT_BOOT[] = {
    {EFUSE_BLK0, 97, 1}, 	 // [] Represents whether direct boot mode is disabled or enabled.\\ 1. Disable\\ 0: Enable,
};

static const esp_efuse_desc_t DIS_USB_SERIAL_JTAG_ROM_PRINT[] = {
    {EFUSE_BLK0, 98, 1}, 	 // [] Represents whether print from USB-Serial-JTAG is disabled or enabled.\\ 1. Disable\\ 0: Enable,
};

static const esp_efuse_desc_t DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 99, 1}, 	 // [] Represents whether the USB-Serial-JTAG download function is disabled or enabled.\\ 1: Disable\\ 0: Enable,
};

static const esp_efuse_desc_t ENABLE_SECURITY_DOWNLOAD[] = {
    {EFUSE_BLK0, 100, 1}, 	 // [] Represents whether security download is enabled or disabled.\\ 1: Enable\\ 0: Disable,
};

static const esp_efuse_desc_t UART_PRINT_CONTROL[] = {
    {EFUSE_BLK0, 101, 2}, 	 // [] Represents the types of UART printing,
};

static const esp_efuse_desc_t FORCE_SEND_RESUME[] = {
    {EFUSE_BLK0, 103, 1}, 	 // [] Represents whether ROM code is forced to send a resume command during SPI boot,
};

static const esp_efuse_desc_t SECURE_VERSION[] = {
    {EFUSE_BLK0, 104, 16}, 	 // [] Represents the version used by ESP-IDF anti-rollback feature,
};

static const esp_efuse_desc_t SECURE_BOOT_DISABLE_FAST_WAKE[] = {
    {EFUSE_BLK0, 120, 1}, 	 // [] Represents whether FAST_VERIFY_ON_WAKE is disable or enable when Secure Boot is enable,
};

static const esp_efuse_desc_t HYS_EN_PAD[] = {
    {EFUSE_BLK0, 121, 1}, 	 // [] Represents whether the hysteresis function of corresponding PAD is enabled.\\ 1: enabled\\ 0:disabled,
};

static const esp_efuse_desc_t XTS_DPA_CLK_ENABLE[] = {
    {EFUSE_BLK0, 122, 1}, 	 // [] Represents whether anti-dpa attack clock function is enabled.\\ 1. Enable\\ 0: Disable,
};

static const esp_efuse_desc_t XTS_DPA_PSEUDO_LEVEL[] = {
    {EFUSE_BLK0, 123, 2}, 	 // [] Represents the anti-dpa attack pseudo function level.\\ 3:High\\ 2: Moderate\\ 1: Low\\ 0: Decided by register configuration,
};

static const esp_efuse_desc_t DIS_WIFI6[] = {
    {EFUSE_BLK0, 125, 1}, 	 // [] Represents whether the WiFi 6 feature is enable or disable.\\ 1: WiFi 6 is disable\\ 0: WiFi 6 is enabled.,
};

static const esp_efuse_desc_t ECDSA_DISABLE_P192[] = {
    {EFUSE_BLK0, 126, 1}, 	 // [] Represents whether to disable P192 curve in ECDSA.\\ 1: Disabled.\\ 0: Not disable,
};

static const esp_efuse_desc_t ECC_FORCE_CONST_TIME[] = {
    {EFUSE_BLK0, 127, 1}, 	 // [] Represents whether to force ecc to use const-time calculation mode. \\ 1: Enable. \\ 0: Disable,
};

static const esp_efuse_desc_t MAC[] = {
    {EFUSE_BLK1, 40, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK1, 32, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK1, 24, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK1, 16, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK1, 8, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK1, 0, 8}, 	 // [MAC_FACTORY] MAC address,
};

static const esp_efuse_desc_t WAFER_VERSION_MINOR[] = {
    {EFUSE_BLK1, 64, 4}, 	 // [] Minor chip version,
};

static const esp_efuse_desc_t WAFER_VERSION_MAJOR[] = {
    {EFUSE_BLK1, 68, 2}, 	 // [] Major chip version,
};

static const esp_efuse_desc_t DISABLE_WAFER_VERSION_MAJOR[] = {
    {EFUSE_BLK1, 70, 1}, 	 // [] Disables check of wafer version major,
};

static const esp_efuse_desc_t DISABLE_BLK_VERSION_MAJOR[] = {
    {EFUSE_BLK1, 71, 1}, 	 // [] Disables check of blk version major,
};

static const esp_efuse_desc_t BLK_VERSION_MINOR[] = {
    {EFUSE_BLK1, 72, 3}, 	 // [] BLK_VERSION_MINOR of BLOCK2,
};

static const esp_efuse_desc_t BLK_VERSION_MAJOR[] = {
    {EFUSE_BLK1, 75, 2}, 	 // [] BLK_VERSION_MAJOR of BLOCK2,
};

static const esp_efuse_desc_t FLASH_CAP[] = {
    {EFUSE_BLK1, 77, 3}, 	 // [] Flash capacity,
};

static const esp_efuse_desc_t FLASH_VENDOR[] = {
    {EFUSE_BLK1, 80, 3}, 	 // [] Flash vendor,
};

static const esp_efuse_desc_t PSRAM_CAP[] = {
    {EFUSE_BLK1, 83, 3}, 	 // [] PSRAM capacity,
};

static const esp_efuse_desc_t PSRAM_VENDOR[] = {
    {EFUSE_BLK1, 86, 2}, 	 // [] PSRAM vendor,
};

static const esp_efuse_desc_t TEMP[] = {
    {EFUSE_BLK1, 88, 2}, 	 // [] Temperature,
};

static const esp_efuse_desc_t PKG_VERSION[] = {
    {EFUSE_BLK1, 90, 3}, 	 // [] Package version,
};

static const esp_efuse_desc_t OPTIONAL_UNIQUE_ID[] = {
    {EFUSE_BLK2, 0, 128}, 	 // [] Optional unique 128-bit ID,
};

static const esp_efuse_desc_t TEMPERATURE_SENSOR[] = {
    {EFUSE_BLK2, 128, 9}, 	 // [] Temperature calibration data,
};

static const esp_efuse_desc_t OCODE[] = {
    {EFUSE_BLK2, 137, 8}, 	 // [] ADC OCode calibration,
};

static const esp_efuse_desc_t ADC1_AVE_INIT_CODE_ATTEN0[] = {
    {EFUSE_BLK2, 145, 10}, 	 // [] Average initcode of ADC1 atten0,
};

static const esp_efuse_desc_t ADC1_AVE_INIT_CODE_ATTEN1[] = {
    {EFUSE_BLK2, 155, 10}, 	 // [] Average initcode of ADC1 atten1,
};

static const esp_efuse_desc_t ADC1_AVE_INIT_CODE_ATTEN2[] = {
    {EFUSE_BLK2, 165, 10}, 	 // [] Average initcode of ADC1 atten2,
};

static const esp_efuse_desc_t ADC1_AVE_INIT_CODE_ATTEN3[] = {
    {EFUSE_BLK2, 175, 10}, 	 // [] Average initcode of ADC1 atten3,
};

static const esp_efuse_desc_t ADC1_HI_DOUT_ATTEN0[] = {
    {EFUSE_BLK2, 185, 10}, 	 // [] HI_DOUT of ADC1 atten0,
};

static const esp_efuse_desc_t ADC1_HI_DOUT_ATTEN1[] = {
    {EFUSE_BLK2, 195, 10}, 	 // [] HI_DOUT of ADC1 atten1,
};

static const esp_efuse_desc_t ADC1_HI_DOUT_ATTEN2[] = {
    {EFUSE_BLK2, 205, 10}, 	 // [] HI_DOUT of ADC1 atten2,
};

static const esp_efuse_desc_t ADC1_HI_DOUT_ATTEN3[] = {
    {EFUSE_BLK2, 215, 10}, 	 // [] HI_DOUT of ADC1 atten3,
};

static const esp_efuse_desc_t ADC1_CH0_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK2, 225, 4}, 	 // [] Gap between ADC1 CH0 and average initcode,
};

static const esp_efuse_desc_t ADC1_CH1_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK2, 229, 4}, 	 // [] Gap between ADC1 CH1 and average initcode,
};

static const esp_efuse_desc_t ADC1_CH2_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK2, 233, 4}, 	 // [] Gap between ADC1 CH2 and average initcode,
};

static const esp_efuse_desc_t ADC1_CH3_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK2, 237, 4}, 	 // [] Gap between ADC1 CH3 and average initcode,
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_USB_JTAG[] = {
    &WR_DIS_DIS_USB_JTAG[0],    		// [] wr_dis of DIS_USB_JTAG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_FORCE_DOWNLOAD[] = {
    &WR_DIS_DIS_FORCE_DOWNLOAD[0],    		// [] wr_dis of DIS_FORCE_DOWNLOAD
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_DOWNLOAD_MSPI_DIS[] = {
    &WR_DIS_SPI_DOWNLOAD_MSPI_DIS[0],    		// [] wr_dis of SPI_DOWNLOAD_MSPI_DIS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_JTAG_SEL_ENABLE[] = {
    &WR_DIS_JTAG_SEL_ENABLE[0],    		// [] wr_dis of JTAG_SEL_ENABLE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_PAD_JTAG[] = {
    &WR_DIS_DIS_PAD_JTAG[0],    		// [] wr_dis of DIS_PAD_JTAG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    &WR_DIS_DIS_DOWNLOAD_MANUAL_ENCRYPT[0],    		// [] wr_dis of DIS_DOWNLOAD_MANUAL_ENCRYPT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_HYS_EN_PAD[] = {
    &WR_DIS_HYS_EN_PAD[0],    		// [] wr_dis of HYS_EN_PAD
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_WIFI6[] = {
    &WR_DIS_DIS_WIFI6[0],    		// [] wr_dis of DIS_WIFI6
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SEC_DPA_LEVEL[] = {
    &WR_DIS_SEC_DPA_LEVEL[0],    		// [] wr_dis of SEC_DPA_LEVEL
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_XTS_DPA_CLK_ENABLE[] = {
    &WR_DIS_XTS_DPA_CLK_ENABLE[0],    		// [] wr_dis of XTS_DPA_CLK_ENABLE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_XTS_DPA_PSEUDO_LEVEL[] = {
    &WR_DIS_XTS_DPA_PSEUDO_LEVEL[0],    		// [] wr_dis of XTS_DPA_PSEUDO_LEVEL
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ECDSA_DISABLE_P192[] = {
    &WR_DIS_ECDSA_DISABLE_P192[0],    		// [] wr_dis of ECDSA_DISABLE_P192
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ECC_FORCE_CONST_TIME[] = {
    &WR_DIS_ECC_FORCE_CONST_TIME[0],    		// [] wr_dis of ECC_FORCE_CONST_TIME
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
    &WR_DIS_DIS_DIRECT_BOOT[0],    		// [] wr_dis of DIS_DIRECT_BOOT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_USB_SERIAL_JTAG_ROM_PRINT[] = {
    &WR_DIS_DIS_USB_SERIAL_JTAG_ROM_PRINT[0],    		// [] wr_dis of DIS_USB_SERIAL_JTAG_ROM_PRINT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[] = {
    &WR_DIS_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[0],    		// [] wr_dis of DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FORCE_SEND_RESUME[] = {
    &WR_DIS_FORCE_SEND_RESUME[0],    		// [] wr_dis of FORCE_SEND_RESUME
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_VERSION[] = {
    &WR_DIS_SECURE_VERSION[0],    		// [] wr_dis of SECURE_VERSION
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_DISABLE_FAST_WAKE[] = {
    &WR_DIS_SECURE_BOOT_DISABLE_FAST_WAKE[0],    		// [] wr_dis of SECURE_BOOT_DISABLE_FAST_WAKE
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_WAFER_VERSION_MINOR[] = {
    &WR_DIS_WAFER_VERSION_MINOR[0],    		// [] wr_dis of WAFER_VERSION_MINOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_WAFER_VERSION_MAJOR[] = {
    &WR_DIS_WAFER_VERSION_MAJOR[0],    		// [] wr_dis of WAFER_VERSION_MAJOR
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK_VERSION_MINOR[] = {
    &WR_DIS_BLK_VERSION_MINOR[0],    		// [] wr_dis of BLK_VERSION_MINOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK_VERSION_MAJOR[] = {
    &WR_DIS_BLK_VERSION_MAJOR[0],    		// [] wr_dis of BLK_VERSION_MAJOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_CAP[] = {
    &WR_DIS_FLASH_CAP[0],    		// [] wr_dis of FLASH_CAP
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PSRAM_VENDOR[] = {
    &WR_DIS_PSRAM_VENDOR[0],    		// [] wr_dis of PSRAM_VENDOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_TEMP[] = {
    &WR_DIS_TEMP[0],    		// [] wr_dis of TEMP
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PKG_VERSION[] = {
    &WR_DIS_PKG_VERSION[0],    		// [] wr_dis of PKG_VERSION
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_TEMPERATURE_SENSOR[] = {
    &WR_DIS_TEMPERATURE_SENSOR[0],    		// [] wr_dis of TEMPERATURE_SENSOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_OCODE[] = {
    &WR_DIS_OCODE[0],    		// [] wr_dis of OCODE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_AVE_INIT_CODE_ATTEN0[] = {
    &WR_DIS_ADC1_AVE_INIT_CODE_ATTEN0[0],    		// [] wr_dis of ADC1_AVE_INIT_CODE_ATTEN0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_AVE_INIT_CODE_ATTEN1[] = {
    &WR_DIS_ADC1_AVE_INIT_CODE_ATTEN1[0],    		// [] wr_dis of ADC1_AVE_INIT_CODE_ATTEN1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_AVE_INIT_CODE_ATTEN2[] = {
    &WR_DIS_ADC1_AVE_INIT_CODE_ATTEN2[0],    		// [] wr_dis of ADC1_AVE_INIT_CODE_ATTEN2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_AVE_INIT_CODE_ATTEN3[] = {
    &WR_DIS_ADC1_AVE_INIT_CODE_ATTEN3[0],    		// [] wr_dis of ADC1_AVE_INIT_CODE_ATTEN3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_HI_DOUT_ATTEN0[] = {
    &WR_DIS_ADC1_HI_DOUT_ATTEN0[0],    		// [] wr_dis of ADC1_HI_DOUT_ATTEN0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_HI_DOUT_ATTEN1[] = {
    &WR_DIS_ADC1_HI_DOUT_ATTEN1[0],    		// [] wr_dis of ADC1_HI_DOUT_ATTEN1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_HI_DOUT_ATTEN2[] = {
    &WR_DIS_ADC1_HI_DOUT_ATTEN2[0],    		// [] wr_dis of ADC1_HI_DOUT_ATTEN2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_HI_DOUT_ATTEN3[] = {
    &WR_DIS_ADC1_HI_DOUT_ATTEN3[0],    		// [] wr_dis of ADC1_HI_DOUT_ATTEN3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_CH0_ATTEN0_INITCODE_DIFF[] = {
    &WR_DIS_ADC1_CH0_ATTEN0_INITCODE_DIFF[0],    		// [] wr_dis of ADC1_CH0_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_CH1_ATTEN0_INITCODE_DIFF[] = {
    &WR_DIS_ADC1_CH1_ATTEN0_INITCODE_DIFF[0],    		// [] wr_dis of ADC1_CH1_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_CH2_ATTEN0_INITCODE_DIFF[] = {
    &WR_DIS_ADC1_CH2_ATTEN0_INITCODE_DIFF[0],    		// [] wr_dis of ADC1_CH2_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_CH3_ATTEN0_INITCODE_DIFF[] = {
    &WR_DIS_ADC1_CH3_ATTEN0_INITCODE_DIFF[0],    		// [] wr_dis of ADC1_CH3_ATTEN0_INITCODE_DIFF
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_VDD_SPI_AS_GPIO[] = {
    &WR_DIS_VDD_SPI_AS_GPIO[0],    		// [] wr_dis of VDD_SPI_AS_GPIO
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
    &DIS_ICACHE[0],    		// [] Represents whether icache is disabled or enabled.\\ 1: disabled\\ 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_JTAG[] = {
    &DIS_USB_JTAG[0],    		// [] Represents whether the function of usb switch to jtag is disabled or enabled.\\ 1: disabled\\ 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_FORCE_DOWNLOAD[] = {
    &DIS_FORCE_DOWNLOAD[0],    		// [] Represents whether the function that forces chip into download mode is disabled or enabled.\\ 1: disabled\\ 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_DOWNLOAD_MSPI_DIS[] = {
    &SPI_DOWNLOAD_MSPI_DIS[0],    		// [] Represents whether SPI0 controller during boot_mode_download is disabled or enabled.\\ 1: disabled\\ 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_JTAG_SEL_ENABLE[] = {
    &JTAG_SEL_ENABLE[0],    		// [] Represents whether the selection between usb_to_jtag and pad_to_jtag through strapping gpio15 when both EFUSE_DIS_PAD_JTAG and EFUSE_DIS_USB_JTAG are equal to 0 is enabled or disabled.\\ 1: enabled\\ 0: disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_PAD_JTAG[] = {
    &DIS_PAD_JTAG[0],    		// [] Represents whether JTAG is disabled in the hard way(permanently).\\ 1: disabled\\ 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    &DIS_DOWNLOAD_MANUAL_ENCRYPT[0],    		// [] Represents whether flash encrypt function is disabled or enabled(except in SPI boot mode).\\ 1: disabled\\ 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USB_EXCHG_PINS[] = {
    &USB_EXCHG_PINS[0],    		// [] Represents whether the D+ and D- pins is exchanged.\\ 1: exchanged\\ 0: not exchanged
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_VDD_SPI_AS_GPIO[] = {
    &VDD_SPI_AS_GPIO[0],    		// [] Represents whether vdd spi pin is functioned as gpio.\\ 1: functioned\\ 0: not functioned
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WDT_DELAY_SEL[] = {
    &WDT_DELAY_SEL[0],    		// [] Represents the threshold level of the RTC watchdog STG0 timeout.\\ 0: Original threshold configuration value of STG0 *2 \\1: Original threshold configuration value of STG0 *4 \\2: Original threshold configuration value of STG0 *8 \\3: Original threshold configuration value of STG0 *16
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_BOOT_CRYPT_CNT[] = {
    &SPI_BOOT_CRYPT_CNT[0],    		// [] Enables flash encryption when 1 or 3 bits are set and disables otherwise {0: "Disable"; 1: "Enable"; 3: "Disable"; 7: "Enable"}
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
    &KEY_PURPOSE_0[0],    		// [KEY0_PURPOSE] Represents the purpose of Key0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_1[] = {
    &KEY_PURPOSE_1[0],    		// [KEY1_PURPOSE] Represents the purpose of Key1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_2[] = {
    &KEY_PURPOSE_2[0],    		// [KEY2_PURPOSE] Represents the purpose of Key2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_3[] = {
    &KEY_PURPOSE_3[0],    		// [KEY3_PURPOSE] Represents the purpose of Key3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_4[] = {
    &KEY_PURPOSE_4[0],    		// [KEY4_PURPOSE] Represents the purpose of Key4
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_5[] = {
    &KEY_PURPOSE_5[0],    		// [KEY5_PURPOSE] Represents the purpose of Key5
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SEC_DPA_LEVEL[] = {
    &SEC_DPA_LEVEL[0],    		// [] Represents the spa secure level by configuring the clock random divide mode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_EN[] = {
    &SECURE_BOOT_EN[0],    		// [] Represents whether secure boot is enabled or disabled.\\ 1: enabled\\ 0: disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE[] = {
    &SECURE_BOOT_AGGRESSIVE_REVOKE[0],    		// [] Represents whether revoking aggressive secure boot is enabled or disabled.\\ 1: enabled.\\ 0: disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_TPUW[] = {
    &FLASH_TPUW[0],    		// [] Represents the flash waiting time after power-up; in unit of ms. When the value less than 15; the waiting time is programmed value. Otherwise; the waiting time is 2 times the programmed value
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MODE[] = {
    &DIS_DOWNLOAD_MODE[0],    		// [] Represents whether Download mode is disable or enable.\\ 1. Disable\\ 0: Enable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DIRECT_BOOT[] = {
    &DIS_DIRECT_BOOT[0],    		// [] Represents whether direct boot mode is disabled or enabled.\\ 1. Disable\\ 0: Enable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT[] = {
    &DIS_USB_SERIAL_JTAG_ROM_PRINT[0],    		// [] Represents whether print from USB-Serial-JTAG is disabled or enabled.\\ 1. Disable\\ 0: Enable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[] = {
    &DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[0],    		// [] Represents whether the USB-Serial-JTAG download function is disabled or enabled.\\ 1: Disable\\ 0: Enable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ENABLE_SECURITY_DOWNLOAD[] = {
    &ENABLE_SECURITY_DOWNLOAD[0],    		// [] Represents whether security download is enabled or disabled.\\ 1: Enable\\ 0: Disable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_UART_PRINT_CONTROL[] = {
    &UART_PRINT_CONTROL[0],    		// [] Represents the types of UART printing
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FORCE_SEND_RESUME[] = {
    &FORCE_SEND_RESUME[0],    		// [] Represents whether ROM code is forced to send a resume command during SPI boot
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_VERSION[] = {
    &SECURE_VERSION[0],    		// [] Represents the version used by ESP-IDF anti-rollback feature
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE[] = {
    &SECURE_BOOT_DISABLE_FAST_WAKE[0],    		// [] Represents whether FAST_VERIFY_ON_WAKE is disable or enable when Secure Boot is enable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_HYS_EN_PAD[] = {
    &HYS_EN_PAD[0],    		// [] Represents whether the hysteresis function of corresponding PAD is enabled.\\ 1: enabled\\ 0:disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_XTS_DPA_CLK_ENABLE[] = {
    &XTS_DPA_CLK_ENABLE[0],    		// [] Represents whether anti-dpa attack clock function is enabled.\\ 1. Enable\\ 0: Disable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_XTS_DPA_PSEUDO_LEVEL[] = {
    &XTS_DPA_PSEUDO_LEVEL[0],    		// [] Represents the anti-dpa attack pseudo function level.\\ 3:High\\ 2: Moderate\\ 1: Low\\ 0: Decided by register configuration
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_WIFI6[] = {
    &DIS_WIFI6[0],    		// [] Represents whether the WiFi 6 feature is enable or disable.\\ 1: WiFi 6 is disable\\ 0: WiFi 6 is enabled.
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ECDSA_DISABLE_P192[] = {
    &ECDSA_DISABLE_P192[0],    		// [] Represents whether to disable P192 curve in ECDSA.\\ 1: Disabled.\\ 0: Not disable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ECC_FORCE_CONST_TIME[] = {
    &ECC_FORCE_CONST_TIME[0],    		// [] Represents whether to force ecc to use const-time calculation mode. \\ 1: Enable. \\ 0: Disable
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
    &WAFER_VERSION_MINOR[0],    		// [] Minor chip version
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WAFER_VERSION_MAJOR[] = {
    &WAFER_VERSION_MAJOR[0],    		// [] Major chip version
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

const esp_efuse_desc_t* ESP_EFUSE_BLK_VERSION_MINOR[] = {
    &BLK_VERSION_MINOR[0],    		// [] BLK_VERSION_MINOR of BLOCK2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_BLK_VERSION_MAJOR[] = {
    &BLK_VERSION_MAJOR[0],    		// [] BLK_VERSION_MAJOR of BLOCK2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_CAP[] = {
    &FLASH_CAP[0],    		// [] Flash capacity
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_VENDOR[] = {
    &FLASH_VENDOR[0],    		// [] Flash vendor
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PSRAM_CAP[] = {
    &PSRAM_CAP[0],    		// [] PSRAM capacity
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PSRAM_VENDOR[] = {
    &PSRAM_VENDOR[0],    		// [] PSRAM vendor
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_TEMP[] = {
    &TEMP[0],    		// [] Temperature
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PKG_VERSION[] = {
    &PKG_VERSION[0],    		// [] Package version
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_OPTIONAL_UNIQUE_ID[] = {
    &OPTIONAL_UNIQUE_ID[0],    		// [] Optional unique 128-bit ID
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_TEMPERATURE_SENSOR[] = {
    &TEMPERATURE_SENSOR[0],    		// [] Temperature calibration data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_OCODE[] = {
    &OCODE[0],    		// [] ADC OCode calibration
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_AVE_INIT_CODE_ATTEN0[] = {
    &ADC1_AVE_INIT_CODE_ATTEN0[0],    		// [] Average initcode of ADC1 atten0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_AVE_INIT_CODE_ATTEN1[] = {
    &ADC1_AVE_INIT_CODE_ATTEN1[0],    		// [] Average initcode of ADC1 atten1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_AVE_INIT_CODE_ATTEN2[] = {
    &ADC1_AVE_INIT_CODE_ATTEN2[0],    		// [] Average initcode of ADC1 atten2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_AVE_INIT_CODE_ATTEN3[] = {
    &ADC1_AVE_INIT_CODE_ATTEN3[0],    		// [] Average initcode of ADC1 atten3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_HI_DOUT_ATTEN0[] = {
    &ADC1_HI_DOUT_ATTEN0[0],    		// [] HI_DOUT of ADC1 atten0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_HI_DOUT_ATTEN1[] = {
    &ADC1_HI_DOUT_ATTEN1[0],    		// [] HI_DOUT of ADC1 atten1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_HI_DOUT_ATTEN2[] = {
    &ADC1_HI_DOUT_ATTEN2[0],    		// [] HI_DOUT of ADC1 atten2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_HI_DOUT_ATTEN3[] = {
    &ADC1_HI_DOUT_ATTEN3[0],    		// [] HI_DOUT of ADC1 atten3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CH0_ATTEN0_INITCODE_DIFF[] = {
    &ADC1_CH0_ATTEN0_INITCODE_DIFF[0],    		// [] Gap between ADC1 CH0 and average initcode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CH1_ATTEN0_INITCODE_DIFF[] = {
    &ADC1_CH1_ATTEN0_INITCODE_DIFF[0],    		// [] Gap between ADC1 CH1 and average initcode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CH2_ATTEN0_INITCODE_DIFF[] = {
    &ADC1_CH2_ATTEN0_INITCODE_DIFF[0],    		// [] Gap between ADC1 CH2 and average initcode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CH3_ATTEN0_INITCODE_DIFF[] = {
    &ADC1_CH3_ATTEN0_INITCODE_DIFF[0],    		// [] Gap between ADC1 CH3 and average initcode
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
