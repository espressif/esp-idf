/*
 * SPDX-FileCopyrightText: 2017-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_efuse.h"
#include <assert.h>
#include "esp_efuse_table.h"

// md5_digest_table 0c453d200f282e320677c1ac46786658
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

static const esp_efuse_desc_t WR_DIS_KM_DISABLE_DEPLOY_MODE[] = {
    {EFUSE_BLK0, 1, 1}, 	 // [] wr_dis of KM_DISABLE_DEPLOY_MODE,
};

static const esp_efuse_desc_t WR_DIS_KM_RND_SWITCH_CYCLE[] = {
    {EFUSE_BLK0, 1, 1}, 	 // [] wr_dis of KM_RND_SWITCH_CYCLE,
};

static const esp_efuse_desc_t WR_DIS_KM_DEPLOY_ONLY_ONCE[] = {
    {EFUSE_BLK0, 1, 1}, 	 // [] wr_dis of KM_DEPLOY_ONLY_ONCE,
};

static const esp_efuse_desc_t WR_DIS_FORCE_USE_KEY_MANAGER_KEY[] = {
    {EFUSE_BLK0, 1, 1}, 	 // [] wr_dis of FORCE_USE_KEY_MANAGER_KEY,
};

static const esp_efuse_desc_t WR_DIS_FORCE_DISABLE_SW_INIT_KEY[] = {
    {EFUSE_BLK0, 1, 1}, 	 // [] wr_dis of FORCE_DISABLE_SW_INIT_KEY,
};

static const esp_efuse_desc_t WR_DIS_KM_XTS_KEY_LENGTH_256[] = {
    {EFUSE_BLK0, 1, 1}, 	 // [] wr_dis of KM_XTS_KEY_LENGTH_256,
};

static const esp_efuse_desc_t WR_DIS_LOCK_KM_KEY[] = {
    {EFUSE_BLK0, 1, 1}, 	 // [] wr_dis of LOCK_KM_KEY,
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

static const esp_efuse_desc_t WR_DIS_DIS_TWAI[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of DIS_TWAI,
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

static const esp_efuse_desc_t WR_DIS_XTS_DPA_PSEUDO_LEVEL[] = {
    {EFUSE_BLK0, 14, 1}, 	 // [] wr_dis of XTS_DPA_PSEUDO_LEVEL,
};

static const esp_efuse_desc_t WR_DIS_XTS_DPA_CLK_ENABLE[] = {
    {EFUSE_BLK0, 14, 1}, 	 // [] wr_dis of XTS_DPA_CLK_ENABLE,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_SHA384_EN[] = {
    {EFUSE_BLK0, 14, 1}, 	 // [] wr_dis of SECURE_BOOT_SHA384_EN,
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

static const esp_efuse_desc_t WR_DIS_XTAL_48M_SEL[] = {
    {EFUSE_BLK0, 17, 1}, 	 // [] wr_dis of XTAL_48M_SEL,
};

static const esp_efuse_desc_t WR_DIS_XTAL_48M_SEL_MODE[] = {
    {EFUSE_BLK0, 17, 1}, 	 // [] wr_dis of XTAL_48M_SEL_MODE,
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

static const esp_efuse_desc_t WR_DIS_HUK_GEN_STATE[] = {
    {EFUSE_BLK0, 19, 1}, 	 // [] wr_dis of HUK_GEN_STATE,
};

static const esp_efuse_desc_t WR_DIS_BLK1[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of BLOCK1,
};

static const esp_efuse_desc_t WR_DIS_MAC[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [WR_DIS.MAC_FACTORY] wr_dis of MAC,
};

static const esp_efuse_desc_t WR_DIS_MAC_EXT[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of MAC_EXT,
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

static const esp_efuse_desc_t WR_DIS_PA_TRIM_VERSION[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of PA_TRIM_VERSION,
};

static const esp_efuse_desc_t WR_DIS_TRIM_N_BIAS[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of TRIM_N_BIAS,
};

static const esp_efuse_desc_t WR_DIS_TRIM_P_BIAS[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of TRIM_P_BIAS,
};

static const esp_efuse_desc_t WR_DIS_SYS_DATA_PART1[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of BLOCK2,
};

static const esp_efuse_desc_t WR_DIS_ACTIVE_HP_DBIAS[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of ACTIVE_HP_DBIAS,
};

static const esp_efuse_desc_t WR_DIS_ACTIVE_LP_DBIAS[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of ACTIVE_LP_DBIAS,
};

static const esp_efuse_desc_t WR_DIS_LSLP_HP_DBG[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of LSLP_HP_DBG,
};

static const esp_efuse_desc_t WR_DIS_LSLP_HP_DBIAS[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of LSLP_HP_DBIAS,
};

static const esp_efuse_desc_t WR_DIS_DSLP_LP_DBG[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of DSLP_LP_DBG,
};

static const esp_efuse_desc_t WR_DIS_DSLP_LP_DBIAS[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of DSLP_LP_DBIAS,
};

static const esp_efuse_desc_t WR_DIS_LP_HP_DBIAS_VOL_GAP[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of LP_HP_DBIAS_VOL_GAP,
};

static const esp_efuse_desc_t WR_DIS_REF_CURR_CODE[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of REF_CURR_CODE,
};

static const esp_efuse_desc_t WR_DIS_RES_TUNE_CODE[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of RES_TUNE_CODE,
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

static const esp_efuse_desc_t WR_DIS_ADC1_AVE_INITCODE_ATTEN0[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_AVE_INITCODE_ATTEN0,
};

static const esp_efuse_desc_t WR_DIS_ADC1_AVE_INITCODE_ATTEN1[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_AVE_INITCODE_ATTEN1,
};

static const esp_efuse_desc_t WR_DIS_ADC1_AVE_INITCODE_ATTEN2[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_AVE_INITCODE_ATTEN2,
};

static const esp_efuse_desc_t WR_DIS_ADC1_AVE_INITCODE_ATTEN3[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_AVE_INITCODE_ATTEN3,
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

static const esp_efuse_desc_t WR_DIS_ADC1_CH4_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_CH4_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t WR_DIS_ADC1_CH5_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC1_CH5_ATTEN0_INITCODE_DIFF,
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

static const esp_efuse_desc_t BOOTLOADER_ANTI_ROLLBACK_SECURE_VERSION_HI[] = {
    {EFUSE_BLK0, 39, 1}, 	 // [] Represents the anti-rollback secure version of the 2nd stage bootloader used by the ROM bootloader (the high part of the field),
};

static const esp_efuse_desc_t DIS_ICACHE[] = {
    {EFUSE_BLK0, 40, 1}, 	 // [] Represents whether cache is disabled. 1: Disabled 0: Enabled,
};

static const esp_efuse_desc_t DIS_USB_JTAG[] = {
    {EFUSE_BLK0, 41, 1}, 	 // [] Represents whether the USB-to-JTAG function in USB Serial/JTAG is disabled. Note that \hyperref[fielddesc:EFUSEDISUSBJTAG]{EFUSE\_DIS\_USB\_JTAG} is available only when \hyperref[fielddesc:EFUSEDISUSBSERIALJTAG]{EFUSE\_DIS\_USB\_SERIAL\_JTAG} is configured to 0. For more information; please refer to Chapter \ref{mod:bootctrl} \textit{\nameref{mod:bootctrl}}.1: Disabled0: Enabled,
};

static const esp_efuse_desc_t BOOTLOADER_ANTI_ROLLBACK_EN[] = {
    {EFUSE_BLK0, 42, 1}, 	 // [] Represents whether the ani-rollback check for the 2nd stage bootloader is enabled.1: Enabled0: Disabled,
};

static const esp_efuse_desc_t DIS_FORCE_DOWNLOAD[] = {
    {EFUSE_BLK0, 44, 1}, 	 // [] Represents whether the function that forces chip into Download mode is disabled. 1: Disabled0: Enabled,
};

static const esp_efuse_desc_t SPI_DOWNLOAD_MSPI_DIS[] = {
    {EFUSE_BLK0, 45, 1}, 	 // [] Represents whether SPI0 controller during boot\_mode\_download is disabled.0: Enabled1: Disabled,
};

static const esp_efuse_desc_t DIS_TWAI[] = {
    {EFUSE_BLK0, 46, 1}, 	 // [] Represents whether TWAI$^®$ function is disabled.1: Disabled0: Enabled,
};

static const esp_efuse_desc_t JTAG_SEL_ENABLE[] = {
    {EFUSE_BLK0, 47, 1}, 	 // [] Represents whether the selection of a JTAG signal source through the strapping pin value is enabled when all of \hyperref[fielddesc:EFUSEDISPADJTAG]{EFUSE\_DIS\_PAD\_JTAG}; \hyperref[fielddesc:EFUSEDISUSBJTAG]{EFUSE\_DIS\_USB\_JTAG} and \hyperref[fielddesc:EFUSEDISUSBSERIALJTAG]{EFUSE\_DIS\_USB\_SERIAL\_JTAG} are configured to 0. For more information; please refer to Chapter \ref{mod:bootctrl} \textit{\nameref{mod:bootctrl}}.1: Enabled0: Disabled,
};

static const esp_efuse_desc_t SOFT_DIS_JTAG[] = {
    {EFUSE_BLK0, 48, 3}, 	 // [] Represents whether PAD JTAG is disabled in the soft way. It can be restarted via HMAC. Odd count of bits with a value of 1: DisabledEven count of bits with a value of 1: Enabled,
};

static const esp_efuse_desc_t DIS_PAD_JTAG[] = {
    {EFUSE_BLK0, 51, 1}, 	 // [] Represents whether PAD JTAG is disabled in the hard way (permanently).1: Disabled0: Enabled,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    {EFUSE_BLK0, 52, 1}, 	 // [] Represents whether flash encryption is disabled (except in SPI boot mode).1: Disabled0: Enabled,
};

static const esp_efuse_desc_t USB_EXCHG_PINS[] = {
    {EFUSE_BLK0, 57, 1}, 	 // [] Represents whether the D+ and D- pins is exchanged.1: Exchanged0: Not exchanged,
};

static const esp_efuse_desc_t VDD_SPI_AS_GPIO[] = {
    {EFUSE_BLK0, 58, 1}, 	 // [] Represents whether VDD SPI pin is functioned as GPIO.1: Functioned0: Not functioned,
};

static const esp_efuse_desc_t WDT_DELAY_SEL[] = {
    {EFUSE_BLK0, 59, 2}, 	 // [] Represents RTC watchdog timeout threshold.0: The originally configured STG0 threshold × 21: The originally configured STG0 threshold × 42: The originally configured STG0 threshold × 83: The originally configured STG0 threshold × 16,
};

static const esp_efuse_desc_t BOOTLOADER_ANTI_ROLLBACK_SECURE_VERSION_LO[] = {
    {EFUSE_BLK0, 61, 3}, 	 // [] Represents the anti-rollback secure version of the 2nd stage bootloader used by the ROM bootloader (the low part of the field),
};

static const esp_efuse_desc_t KM_DISABLE_DEPLOY_MODE[] = {
    {EFUSE_BLK0, 64, 4}, 	 // [] Represents whether the new key deployment of key manager is disabled. Bit0: Represents whether the new ECDSA key deployment is disabled0: Enabled1: DisabledBit1: Represents whether the new XTS-AES (flash and PSRAM) key deployment is disabled0: Enabled1: DisabledBit2: Represents whether the new HMAC key deployment is disabled0: Enabled1: DisabledBit3: Represents whether the new DS key deployment is disabled0: Enabled1: Disabled,
};

static const esp_efuse_desc_t KM_RND_SWITCH_CYCLE[] = {
    {EFUSE_BLK0, 68, 2}, 	 // [] Represents the cycle at which the Key Manager switches random numbers.0: Controlled by the \hyperref[fielddesc:KEYMNGRNDSWITCHCYCLE]{KEYMNG\_RND\_SWITCH\_CYCLE} register. For more information; please refer to Chapter \ref{mod:keymng} \textit{\nameref{mod:keymng}}1: 8 Key Manager clock cycles2: 16 Key Manager clock cycles3: 32 Key Manager clock cycles,
};

static const esp_efuse_desc_t KM_DEPLOY_ONLY_ONCE[] = {
    {EFUSE_BLK0, 70, 4}, 	 // [] Represents whether the corresponding key can be deployed only once.Bit0: Represents whether the ECDSA key can be deployed only once0: The key can be deployed multiple times1: The key can be deployed only onceBit1: Represents whether the XTS-AES (flash and PSRAM) key can be deployed only once0: The key can be deployed multiple times1: The key can be deployed only onceBit2: Represents whether the HMAC key can be deployed only once0: The key can be deployed multiple times1: The key can be deployed only onceBit3: Represents whether the DS key can be deployed only once0: The key can be deployed multiple times1: The key can be deployed only once,
};

static const esp_efuse_desc_t FORCE_USE_KEY_MANAGER_KEY[] = {
    {EFUSE_BLK0, 74, 4}, 	 // [] Represents whether the corresponding key must come from Key Manager. Bit0: Represents whether the ECDSA key must come from Key Manager.0: The key does not need to come from Key Manager1: The key must come from Key ManagerBit1: Represents whether the XTS-AES (flash and PSRAM) key must come from Key Manager.0: The key does not need to come from Key Manager1: The key must come from Key ManagerBit2: Represents whether the HMAC key must come from Key Manager.0: The key does not need to come from Key Manager1: The key must come from Key ManagerBit3: Represents whether the DS key must come from Key Manager.0: The key does not need to come from Key Manager1: The key must come from Key Manager,
};

static const esp_efuse_desc_t FORCE_DISABLE_SW_INIT_KEY[] = {
    {EFUSE_BLK0, 78, 1}, 	 // [] Represents whether to disable the use of the initialization key written by software and instead force use efuse\_init\_key.0: Enable1: Disable,
};

static const esp_efuse_desc_t BOOTLOADER_ANTI_ROLLBACK_UPDATE_IN_ROM[] = {
    {EFUSE_BLK0, 79, 1}, 	 // [] Represents whether the ani-rollback SECURE_VERSION will be updated from the ROM bootloader.1: Enable0: Disable,
};

static const esp_efuse_desc_t SPI_BOOT_CRYPT_CNT[] = {
    {EFUSE_BLK0, 80, 3}, 	 // [] Enables flash encryption when 1 or 3 bits are set and disables otherwise {0: "Disable"; 1: "Enable"; 3: "Disable"; 7: "Enable"},
};

static const esp_efuse_desc_t SECURE_BOOT_KEY_REVOKE0[] = {
    {EFUSE_BLK0, 83, 1}, 	 // [] Revoke 1st secure boot key,
};

static const esp_efuse_desc_t SECURE_BOOT_KEY_REVOKE1[] = {
    {EFUSE_BLK0, 84, 1}, 	 // [] Revoke 2nd secure boot key,
};

static const esp_efuse_desc_t SECURE_BOOT_KEY_REVOKE2[] = {
    {EFUSE_BLK0, 85, 1}, 	 // [] Revoke 3rd secure boot key,
};

static const esp_efuse_desc_t KEY_PURPOSE_0[] = {
    {EFUSE_BLK0, 86, 5}, 	 // [KEY0_PURPOSE] Represents the purpose of Key0. See Table \ref{tab:efuse-key-purpose},
};

static const esp_efuse_desc_t KEY_PURPOSE_1[] = {
    {EFUSE_BLK0, 91, 5}, 	 // [KEY1_PURPOSE] Represents the purpose of Key1. See Table \ref{tab:efuse-key-purpose},
};

static const esp_efuse_desc_t KEY_PURPOSE_2[] = {
    {EFUSE_BLK0, 96, 5}, 	 // [KEY2_PURPOSE] Represents the purpose of Key2. See Table \ref{tab:efuse-key-purpose},
};

static const esp_efuse_desc_t KEY_PURPOSE_3[] = {
    {EFUSE_BLK0, 101, 5}, 	 // [KEY3_PURPOSE] Represents the purpose of Key3. See Table \ref{tab:efuse-key-purpose},
};

static const esp_efuse_desc_t KEY_PURPOSE_4[] = {
    {EFUSE_BLK0, 106, 5}, 	 // [KEY4_PURPOSE] Represents the purpose of Key4. See Table \ref{tab:efuse-key-purpose},
};

static const esp_efuse_desc_t KEY_PURPOSE_5[] = {
    {EFUSE_BLK0, 111, 5}, 	 // [KEY5_PURPOSE] Represents the purpose of Key5. See Table \ref{tab:efuse-key-purpose},
};

static const esp_efuse_desc_t SEC_DPA_LEVEL[] = {
    {EFUSE_BLK0, 116, 2}, 	 // [] Represents the security level of anti-DPA attack. The level is adjusted by configuring the clock random frequency division mode.0: Security level is SEC\_DPA\_OFF1: Security level is SEC\_DPA\_LOW2: Security level is SEC\_DPA\_MIDDLE3: Security level is SEC\_DPA\_HIGHFor more information; please refer to Chapter \ref{mod:sysreg} \textit{\nameref{mod:sysreg}} > Section \ref{sec:sysreg-anti-dpa-attack-security-control} \textit{\nameref{sec:sysreg-anti-dpa-attack-security-control}}.,
};

static const esp_efuse_desc_t RECOVERY_BOOTLOADER_FLASH_SECTOR_HI[] = {
    {EFUSE_BLK0, 118, 3}, 	 // [] Represents the starting flash sector (flash sector size is 0x1000) of the recovery bootloader used by the ROM bootloader If the primary bootloader fails. 0 and 0xFFF - this feature is disabled. (The high part of the field),
};

static const esp_efuse_desc_t SECURE_BOOT_EN[] = {
    {EFUSE_BLK0, 121, 1}, 	 // [] Represents whether Secure Boot is enabled.1: Enabled0: Disabled,
};

static const esp_efuse_desc_t SECURE_BOOT_AGGRESSIVE_REVOKE[] = {
    {EFUSE_BLK0, 122, 1}, 	 // [] Represents whether aggressive revocation of Secure Boot is enabled.1: Enabled0: Disabled,
};

static const esp_efuse_desc_t KM_XTS_KEY_LENGTH_256[] = {
    {EFUSE_BLK0, 123, 1}, 	 // [] Represents which key flash encryption uses.0: XTS-AES-256 key1: XTS-AES-128 key,
};

static const esp_efuse_desc_t FLASH_TPUW[] = {
    {EFUSE_BLK0, 124, 4}, 	 // [] Represents the flash waiting time after power-up. Measurement unit: ms. When the value is less than 15; the waiting time is the programmed value. Otherwise; the waiting time is a fixed value; i.e. 30 ms,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 128, 1}, 	 // [] Represents whether Download mode is disable or enable. 1. Disable 0: Enable,
};

static const esp_efuse_desc_t DIS_DIRECT_BOOT[] = {
    {EFUSE_BLK0, 129, 1}, 	 // [] Represents whether direct boot mode is disabled or enabled. 1. Disable 0: Enable,
};

static const esp_efuse_desc_t DIS_USB_SERIAL_JTAG_ROM_PRINT[] = {
    {EFUSE_BLK0, 130, 1}, 	 // [] Represents whether print from USB-Serial-JTAG is disabled or enabled. 1. Disable 0: Enable,
};

static const esp_efuse_desc_t LOCK_KM_KEY[] = {
    {EFUSE_BLK0, 131, 1}, 	 // [] Represents whether the keys in the Key Manager are locked after deployment.0: Not locked1: Locked,
};

static const esp_efuse_desc_t DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 132, 1}, 	 // [] Represents whether the USB-Serial-JTAG download function is disabled or enabled. 1: Disable 0: Enable,
};

static const esp_efuse_desc_t ENABLE_SECURITY_DOWNLOAD[] = {
    {EFUSE_BLK0, 133, 1}, 	 // [] Represents whether security download is enabled. Only downloading into flash is supported. Reading/writing RAM or registers is not supported (i.e. stub download is not supported).1: Enabled0: Disabled,
};

static const esp_efuse_desc_t UART_PRINT_CONTROL[] = {
    {EFUSE_BLK0, 134, 2}, 	 // [] Set the default UARTboot message output mode {0: "Enable"; 1: "Enable when GPIO8 is low at reset"; 2: "Enable when GPIO8 is high at reset"; 3: "Disable"},
};

static const esp_efuse_desc_t FORCE_SEND_RESUME[] = {
    {EFUSE_BLK0, 136, 1}, 	 // [] Represents whether ROM code is forced to send a resume command during SPI boot.1: Forced. 0: Not forced.,
};

static const esp_efuse_desc_t SECURE_VERSION[] = {
    {EFUSE_BLK0, 137, 9}, 	 // [] Represents the app secure version used by ESP-IDF anti-rollback feature,
};

static const esp_efuse_desc_t SECURE_BOOT_DISABLE_FAST_WAKE[] = {
    {EFUSE_BLK0, 153, 1}, 	 // [] Represents whether FAST VERIFY ON WAKE is disabled when Secure Boot is enabled.1: Disabled0: Enabled,
};

static const esp_efuse_desc_t HYS_EN_PAD[] = {
    {EFUSE_BLK0, 154, 1}, 	 // [] Represents whether the hysteresis function of PAD0 – PAD27 is enabled.1: Enabled0: Disabled,
};

static const esp_efuse_desc_t XTS_DPA_PSEUDO_LEVEL[] = {
    {EFUSE_BLK0, 155, 2}, 	 // [] Represents the pseudo round level of XTS-AES anti-DPA attack.0: Disabled1: Low2: Moderate3: High,
};

static const esp_efuse_desc_t XTS_DPA_CLK_ENABLE[] = {
    {EFUSE_BLK0, 157, 1}, 	 // [] Represents whether XTS-AES anti-DPA attack clock is enabled.0: Disable1: Enabled,
};

static const esp_efuse_desc_t SECURE_BOOT_SHA384_EN[] = {
    {EFUSE_BLK0, 159, 1}, 	 // [] Represents if the chip supports Secure Boot using SHA-384,
};

static const esp_efuse_desc_t HUK_GEN_STATE[] = {
    {EFUSE_BLK0, 160, 9}, 	 // [] Represents whether the HUK generate mode is valid.Odd count of bits with a value of 1: InvalidEven count of bits with a value of 1: Valid,
};

static const esp_efuse_desc_t XTAL_48M_SEL[] = {
    {EFUSE_BLK0, 169, 3}, 	 // [] Represents whether XTAL frequency is 48MHz or not. If not; 40MHz XTAL will be used. If this field contains Odd number bit 1: Enable 48MHz XTAL\ Even number bit 1: Enable 40MHz XTAL,
};

static const esp_efuse_desc_t XTAL_48M_SEL_MODE[] = {
    {EFUSE_BLK0, 172, 1}, 	 // [] Represents what determines the XTAL frequency in \textbf{Joint Download Boot} mode.  For more information; please refer to Chapter \ref{mod:bootctrl} \textit{\nameref{mod:bootctrl}}.0: Strapping PAD state1: \hyperref[fielddesc:EFUSEXTAL48MSEL]{EFUSE\_XTAL\_48M\_SEL} in eFuse,
};

static const esp_efuse_desc_t ECC_FORCE_CONST_TIME[] = {
    {EFUSE_BLK0, 173, 1}, 	 // [] Represents whether to force ECC to use constant-time mode for point multiplication calculation. 0: Not force1: Force,
};

static const esp_efuse_desc_t RECOVERY_BOOTLOADER_FLASH_SECTOR_LO[] = {
    {EFUSE_BLK0, 174, 9}, 	 // [] Represents the starting flash sector (flash sector size is 0x1000) of the recovery bootloader used by the ROM bootloader If the primary bootloader fails. 0 and 0xFFF - this feature is disabled. (The low part of the field),
};

static const esp_efuse_desc_t MAC[] = {
    {EFUSE_BLK1, 40, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK1, 32, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK1, 24, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK1, 16, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK1, 8, 8}, 	 // [MAC_FACTORY] MAC address,
    {EFUSE_BLK1, 0, 8}, 	 // [MAC_FACTORY] MAC address,
};

static const esp_efuse_desc_t MAC_EXT[] = {
    {EFUSE_BLK1, 48, 16}, 	 // [] Represents the extended bits of MAC address,
};

static const esp_efuse_desc_t WAFER_VERSION_MINOR[] = {
    {EFUSE_BLK1, 64, 4}, 	 // [] Minor chip version,
};

static const esp_efuse_desc_t WAFER_VERSION_MAJOR[] = {
    {EFUSE_BLK1, 68, 2}, 	 // [] Minor chip version,
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
    {EFUSE_BLK1, 83, 3}, 	 // [] Psram capacity,
};

static const esp_efuse_desc_t PSRAM_VENDOR[] = {
    {EFUSE_BLK1, 86, 2}, 	 // [] Psram vendor,
};

static const esp_efuse_desc_t TEMP[] = {
    {EFUSE_BLK1, 88, 2}, 	 // [] Temp (die embedded inside),
};

static const esp_efuse_desc_t PKG_VERSION[] = {
    {EFUSE_BLK1, 90, 3}, 	 // [] Package version,
};

static const esp_efuse_desc_t PA_TRIM_VERSION[] = {
    {EFUSE_BLK1, 93, 3}, 	 // [] PADC CAL PA trim version,
};

static const esp_efuse_desc_t TRIM_N_BIAS[] = {
    {EFUSE_BLK1, 96, 5}, 	 // [] PADC CAL N bias,
};

static const esp_efuse_desc_t TRIM_P_BIAS[] = {
    {EFUSE_BLK1, 101, 5}, 	 // [] PADC CAL P bias,
};

static const esp_efuse_desc_t ACTIVE_HP_DBIAS[] = {
    {EFUSE_BLK1, 106, 4}, 	 // [] Active HP DBIAS of fixed voltage,
};

static const esp_efuse_desc_t ACTIVE_LP_DBIAS[] = {
    {EFUSE_BLK1, 110, 4}, 	 // [] Active LP DBIAS of fixed voltage,
};

static const esp_efuse_desc_t LSLP_HP_DBG[] = {
    {EFUSE_BLK1, 114, 2}, 	 // [] LSLP HP DBG of fixed voltage,
};

static const esp_efuse_desc_t LSLP_HP_DBIAS[] = {
    {EFUSE_BLK1, 116, 4}, 	 // [] LSLP HP DBIAS of fixed voltage,
};

static const esp_efuse_desc_t DSLP_LP_DBG[] = {
    {EFUSE_BLK1, 120, 4}, 	 // [] DSLP LP DBG of fixed voltage,
};

static const esp_efuse_desc_t DSLP_LP_DBIAS[] = {
    {EFUSE_BLK1, 124, 5}, 	 // [] DSLP LP DBIAS of fixed voltage,
};

static const esp_efuse_desc_t LP_HP_DBIAS_VOL_GAP[] = {
    {EFUSE_BLK1, 129, 5}, 	 // [] DBIAS gap between LP and HP,
};

static const esp_efuse_desc_t REF_CURR_CODE[] = {
    {EFUSE_BLK1, 134, 4}, 	 // [] REF PADC Calibration Curr,
};

static const esp_efuse_desc_t RES_TUNE_CODE[] = {
    {EFUSE_BLK1, 138, 5}, 	 // [] RES PADC Calibration Tune,
};

static const esp_efuse_desc_t OPTIONAL_UNIQUE_ID[] = {
    {EFUSE_BLK2, 0, 128}, 	 // [] Optional unique 128-bit ID,
};

static const esp_efuse_desc_t TEMPERATURE_SENSOR[] = {
    {EFUSE_BLK2, 128, 9}, 	 // [] Temperature calibration data,
};

static const esp_efuse_desc_t OCODE[] = {
    {EFUSE_BLK2, 137, 8}, 	 // [] ADC OCode,
};

static const esp_efuse_desc_t ADC1_AVE_INITCODE_ATTEN0[] = {
    {EFUSE_BLK2, 145, 10}, 	 // [] Average initcode of ADC1 atten0,
};

static const esp_efuse_desc_t ADC1_AVE_INITCODE_ATTEN1[] = {
    {EFUSE_BLK2, 155, 10}, 	 // [] Average initcode of ADC1 atten0,
};

static const esp_efuse_desc_t ADC1_AVE_INITCODE_ATTEN2[] = {
    {EFUSE_BLK2, 165, 10}, 	 // [] Average initcode of ADC1 atten0,
};

static const esp_efuse_desc_t ADC1_AVE_INITCODE_ATTEN3[] = {
    {EFUSE_BLK2, 175, 10}, 	 // [] Average initcode of ADC1 atten0,
};

static const esp_efuse_desc_t ADC1_HI_DOUT_ATTEN0[] = {
    {EFUSE_BLK2, 185, 10}, 	 // [] HI DOUT of ADC1 atten0,
};

static const esp_efuse_desc_t ADC1_HI_DOUT_ATTEN1[] = {
    {EFUSE_BLK2, 195, 10}, 	 // [] HI DOUT of ADC1 atten1,
};

static const esp_efuse_desc_t ADC1_HI_DOUT_ATTEN2[] = {
    {EFUSE_BLK2, 205, 10}, 	 // [] HI DOUT of ADC1 atten2,
};

static const esp_efuse_desc_t ADC1_HI_DOUT_ATTEN3[] = {
    {EFUSE_BLK2, 215, 10}, 	 // [] HI DOUT of ADC1 atten3,
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

static const esp_efuse_desc_t ADC1_CH4_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK2, 241, 4}, 	 // [] Gap between ADC1 CH4 and average initcode,
};

static const esp_efuse_desc_t ADC1_CH5_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK2, 245, 4}, 	 // [] Gap between ADC1 CH5 and average initcode,
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KM_DISABLE_DEPLOY_MODE[] = {
    &WR_DIS_KM_DISABLE_DEPLOY_MODE[0],    		// [] wr_dis of KM_DISABLE_DEPLOY_MODE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KM_RND_SWITCH_CYCLE[] = {
    &WR_DIS_KM_RND_SWITCH_CYCLE[0],    		// [] wr_dis of KM_RND_SWITCH_CYCLE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KM_DEPLOY_ONLY_ONCE[] = {
    &WR_DIS_KM_DEPLOY_ONLY_ONCE[0],    		// [] wr_dis of KM_DEPLOY_ONLY_ONCE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FORCE_USE_KEY_MANAGER_KEY[] = {
    &WR_DIS_FORCE_USE_KEY_MANAGER_KEY[0],    		// [] wr_dis of FORCE_USE_KEY_MANAGER_KEY
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FORCE_DISABLE_SW_INIT_KEY[] = {
    &WR_DIS_FORCE_DISABLE_SW_INIT_KEY[0],    		// [] wr_dis of FORCE_DISABLE_SW_INIT_KEY
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KM_XTS_KEY_LENGTH_256[] = {
    &WR_DIS_KM_XTS_KEY_LENGTH_256[0],    		// [] wr_dis of KM_XTS_KEY_LENGTH_256
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LOCK_KM_KEY[] = {
    &WR_DIS_LOCK_KM_KEY[0],    		// [] wr_dis of LOCK_KM_KEY
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_TWAI[] = {
    &WR_DIS_DIS_TWAI[0],    		// [] wr_dis of DIS_TWAI
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_XTS_DPA_PSEUDO_LEVEL[] = {
    &WR_DIS_XTS_DPA_PSEUDO_LEVEL[0],    		// [] wr_dis of XTS_DPA_PSEUDO_LEVEL
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_XTS_DPA_CLK_ENABLE[] = {
    &WR_DIS_XTS_DPA_CLK_ENABLE[0],    		// [] wr_dis of XTS_DPA_CLK_ENABLE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_SHA384_EN[] = {
    &WR_DIS_SECURE_BOOT_SHA384_EN[0],    		// [] wr_dis of SECURE_BOOT_SHA384_EN
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_XTAL_48M_SEL[] = {
    &WR_DIS_XTAL_48M_SEL[0],    		// [] wr_dis of XTAL_48M_SEL
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_XTAL_48M_SEL_MODE[] = {
    &WR_DIS_XTAL_48M_SEL_MODE[0],    		// [] wr_dis of XTAL_48M_SEL_MODE
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_HUK_GEN_STATE[] = {
    &WR_DIS_HUK_GEN_STATE[0],    		// [] wr_dis of HUK_GEN_STATE
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_MAC_EXT[] = {
    &WR_DIS_MAC_EXT[0],    		// [] wr_dis of MAC_EXT
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PA_TRIM_VERSION[] = {
    &WR_DIS_PA_TRIM_VERSION[0],    		// [] wr_dis of PA_TRIM_VERSION
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_TRIM_N_BIAS[] = {
    &WR_DIS_TRIM_N_BIAS[0],    		// [] wr_dis of TRIM_N_BIAS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_TRIM_P_BIAS[] = {
    &WR_DIS_TRIM_P_BIAS[0],    		// [] wr_dis of TRIM_P_BIAS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SYS_DATA_PART1[] = {
    &WR_DIS_SYS_DATA_PART1[0],    		// [] wr_dis of BLOCK2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ACTIVE_HP_DBIAS[] = {
    &WR_DIS_ACTIVE_HP_DBIAS[0],    		// [] wr_dis of ACTIVE_HP_DBIAS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ACTIVE_LP_DBIAS[] = {
    &WR_DIS_ACTIVE_LP_DBIAS[0],    		// [] wr_dis of ACTIVE_LP_DBIAS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LSLP_HP_DBG[] = {
    &WR_DIS_LSLP_HP_DBG[0],    		// [] wr_dis of LSLP_HP_DBG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LSLP_HP_DBIAS[] = {
    &WR_DIS_LSLP_HP_DBIAS[0],    		// [] wr_dis of LSLP_HP_DBIAS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DSLP_LP_DBG[] = {
    &WR_DIS_DSLP_LP_DBG[0],    		// [] wr_dis of DSLP_LP_DBG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DSLP_LP_DBIAS[] = {
    &WR_DIS_DSLP_LP_DBIAS[0],    		// [] wr_dis of DSLP_LP_DBIAS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LP_HP_DBIAS_VOL_GAP[] = {
    &WR_DIS_LP_HP_DBIAS_VOL_GAP[0],    		// [] wr_dis of LP_HP_DBIAS_VOL_GAP
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_REF_CURR_CODE[] = {
    &WR_DIS_REF_CURR_CODE[0],    		// [] wr_dis of REF_CURR_CODE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RES_TUNE_CODE[] = {
    &WR_DIS_RES_TUNE_CODE[0],    		// [] wr_dis of RES_TUNE_CODE
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_AVE_INITCODE_ATTEN0[] = {
    &WR_DIS_ADC1_AVE_INITCODE_ATTEN0[0],    		// [] wr_dis of ADC1_AVE_INITCODE_ATTEN0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_AVE_INITCODE_ATTEN1[] = {
    &WR_DIS_ADC1_AVE_INITCODE_ATTEN1[0],    		// [] wr_dis of ADC1_AVE_INITCODE_ATTEN1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_AVE_INITCODE_ATTEN2[] = {
    &WR_DIS_ADC1_AVE_INITCODE_ATTEN2[0],    		// [] wr_dis of ADC1_AVE_INITCODE_ATTEN2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_AVE_INITCODE_ATTEN3[] = {
    &WR_DIS_ADC1_AVE_INITCODE_ATTEN3[0],    		// [] wr_dis of ADC1_AVE_INITCODE_ATTEN3
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_CH4_ATTEN0_INITCODE_DIFF[] = {
    &WR_DIS_ADC1_CH4_ATTEN0_INITCODE_DIFF[0],    		// [] wr_dis of ADC1_CH4_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_CH5_ATTEN0_INITCODE_DIFF[] = {
    &WR_DIS_ADC1_CH5_ATTEN0_INITCODE_DIFF[0],    		// [] wr_dis of ADC1_CH5_ATTEN0_INITCODE_DIFF
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

const esp_efuse_desc_t* ESP_EFUSE_BOOTLOADER_ANTI_ROLLBACK_SECURE_VERSION_HI[] = {
    &BOOTLOADER_ANTI_ROLLBACK_SECURE_VERSION_HI[0],    		// [] Represents the anti-rollback secure version of the 2nd stage bootloader used by the ROM bootloader (the high part of the field)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_ICACHE[] = {
    &DIS_ICACHE[0],    		// [] Represents whether cache is disabled. 1: Disabled 0: Enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_JTAG[] = {
    &DIS_USB_JTAG[0],    		// [] Represents whether the USB-to-JTAG function in USB Serial/JTAG is disabled. Note that \hyperref[fielddesc:EFUSEDISUSBJTAG]{EFUSE\_DIS\_USB\_JTAG} is available only when \hyperref[fielddesc:EFUSEDISUSBSERIALJTAG]{EFUSE\_DIS\_USB\_SERIAL\_JTAG} is configured to 0. For more information; please refer to Chapter \ref{mod:bootctrl} \textit{\nameref{mod:bootctrl}}.1: Disabled0: Enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_BOOTLOADER_ANTI_ROLLBACK_EN[] = {
    &BOOTLOADER_ANTI_ROLLBACK_EN[0],    		// [] Represents whether the ani-rollback check for the 2nd stage bootloader is enabled.1: Enabled0: Disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_FORCE_DOWNLOAD[] = {
    &DIS_FORCE_DOWNLOAD[0],    		// [] Represents whether the function that forces chip into Download mode is disabled. 1: Disabled0: Enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_DOWNLOAD_MSPI_DIS[] = {
    &SPI_DOWNLOAD_MSPI_DIS[0],    		// [] Represents whether SPI0 controller during boot\_mode\_download is disabled.0: Enabled1: Disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_TWAI[] = {
    &DIS_TWAI[0],    		// [] Represents whether TWAI$^®$ function is disabled.1: Disabled0: Enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_JTAG_SEL_ENABLE[] = {
    &JTAG_SEL_ENABLE[0],    		// [] Represents whether the selection of a JTAG signal source through the strapping pin value is enabled when all of \hyperref[fielddesc:EFUSEDISPADJTAG]{EFUSE\_DIS\_PAD\_JTAG}; \hyperref[fielddesc:EFUSEDISUSBJTAG]{EFUSE\_DIS\_USB\_JTAG} and \hyperref[fielddesc:EFUSEDISUSBSERIALJTAG]{EFUSE\_DIS\_USB\_SERIAL\_JTAG} are configured to 0. For more information; please refer to Chapter \ref{mod:bootctrl} \textit{\nameref{mod:bootctrl}}.1: Enabled0: Disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SOFT_DIS_JTAG[] = {
    &SOFT_DIS_JTAG[0],    		// [] Represents whether PAD JTAG is disabled in the soft way. It can be restarted via HMAC. Odd count of bits with a value of 1: DisabledEven count of bits with a value of 1: Enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_PAD_JTAG[] = {
    &DIS_PAD_JTAG[0],    		// [] Represents whether PAD JTAG is disabled in the hard way (permanently).1: Disabled0: Enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    &DIS_DOWNLOAD_MANUAL_ENCRYPT[0],    		// [] Represents whether flash encryption is disabled (except in SPI boot mode).1: Disabled0: Enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USB_EXCHG_PINS[] = {
    &USB_EXCHG_PINS[0],    		// [] Represents whether the D+ and D- pins is exchanged.1: Exchanged0: Not exchanged
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_VDD_SPI_AS_GPIO[] = {
    &VDD_SPI_AS_GPIO[0],    		// [] Represents whether VDD SPI pin is functioned as GPIO.1: Functioned0: Not functioned
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WDT_DELAY_SEL[] = {
    &WDT_DELAY_SEL[0],    		// [] Represents RTC watchdog timeout threshold.0: The originally configured STG0 threshold × 21: The originally configured STG0 threshold × 42: The originally configured STG0 threshold × 83: The originally configured STG0 threshold × 16
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_BOOTLOADER_ANTI_ROLLBACK_SECURE_VERSION_LO[] = {
    &BOOTLOADER_ANTI_ROLLBACK_SECURE_VERSION_LO[0],    		// [] Represents the anti-rollback secure version of the 2nd stage bootloader used by the ROM bootloader (the low part of the field)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KM_DISABLE_DEPLOY_MODE[] = {
    &KM_DISABLE_DEPLOY_MODE[0],    		// [] Represents whether the new key deployment of key manager is disabled. Bit0: Represents whether the new ECDSA key deployment is disabled0: Enabled1: DisabledBit1: Represents whether the new XTS-AES (flash and PSRAM) key deployment is disabled0: Enabled1: DisabledBit2: Represents whether the new HMAC key deployment is disabled0: Enabled1: DisabledBit3: Represents whether the new DS key deployment is disabled0: Enabled1: Disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KM_RND_SWITCH_CYCLE[] = {
    &KM_RND_SWITCH_CYCLE[0],    		// [] Represents the cycle at which the Key Manager switches random numbers.0: Controlled by the \hyperref[fielddesc:KEYMNGRNDSWITCHCYCLE]{KEYMNG\_RND\_SWITCH\_CYCLE} register. For more information; please refer to Chapter \ref{mod:keymng} \textit{\nameref{mod:keymng}}1: 8 Key Manager clock cycles2: 16 Key Manager clock cycles3: 32 Key Manager clock cycles
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KM_DEPLOY_ONLY_ONCE[] = {
    &KM_DEPLOY_ONLY_ONCE[0],    		// [] Represents whether the corresponding key can be deployed only once.Bit0: Represents whether the ECDSA key can be deployed only once0: The key can be deployed multiple times1: The key can be deployed only onceBit1: Represents whether the XTS-AES (flash and PSRAM) key can be deployed only once0: The key can be deployed multiple times1: The key can be deployed only onceBit2: Represents whether the HMAC key can be deployed only once0: The key can be deployed multiple times1: The key can be deployed only onceBit3: Represents whether the DS key can be deployed only once0: The key can be deployed multiple times1: The key can be deployed only once
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FORCE_USE_KEY_MANAGER_KEY[] = {
    &FORCE_USE_KEY_MANAGER_KEY[0],    		// [] Represents whether the corresponding key must come from Key Manager. Bit0: Represents whether the ECDSA key must come from Key Manager.0: The key does not need to come from Key Manager1: The key must come from Key ManagerBit1: Represents whether the XTS-AES (flash and PSRAM) key must come from Key Manager.0: The key does not need to come from Key Manager1: The key must come from Key ManagerBit2: Represents whether the HMAC key must come from Key Manager.0: The key does not need to come from Key Manager1: The key must come from Key ManagerBit3: Represents whether the DS key must come from Key Manager.0: The key does not need to come from Key Manager1: The key must come from Key Manager
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FORCE_DISABLE_SW_INIT_KEY[] = {
    &FORCE_DISABLE_SW_INIT_KEY[0],    		// [] Represents whether to disable the use of the initialization key written by software and instead force use efuse\_init\_key.0: Enable1: Disable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_BOOTLOADER_ANTI_ROLLBACK_UPDATE_IN_ROM[] = {
    &BOOTLOADER_ANTI_ROLLBACK_UPDATE_IN_ROM[0],    		// [] Represents whether the ani-rollback SECURE_VERSION will be updated from the ROM bootloader.1: Enable0: Disable
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
    &KEY_PURPOSE_0[0],    		// [KEY0_PURPOSE] Represents the purpose of Key0. See Table \ref{tab:efuse-key-purpose}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_1[] = {
    &KEY_PURPOSE_1[0],    		// [KEY1_PURPOSE] Represents the purpose of Key1. See Table \ref{tab:efuse-key-purpose}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_2[] = {
    &KEY_PURPOSE_2[0],    		// [KEY2_PURPOSE] Represents the purpose of Key2. See Table \ref{tab:efuse-key-purpose}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_3[] = {
    &KEY_PURPOSE_3[0],    		// [KEY3_PURPOSE] Represents the purpose of Key3. See Table \ref{tab:efuse-key-purpose}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_4[] = {
    &KEY_PURPOSE_4[0],    		// [KEY4_PURPOSE] Represents the purpose of Key4. See Table \ref{tab:efuse-key-purpose}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_5[] = {
    &KEY_PURPOSE_5[0],    		// [KEY5_PURPOSE] Represents the purpose of Key5. See Table \ref{tab:efuse-key-purpose}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SEC_DPA_LEVEL[] = {
    &SEC_DPA_LEVEL[0],    		// [] Represents the security level of anti-DPA attack. The level is adjusted by configuring the clock random frequency division mode.0: Security level is SEC\_DPA\_OFF1: Security level is SEC\_DPA\_LOW2: Security level is SEC\_DPA\_MIDDLE3: Security level is SEC\_DPA\_HIGHFor more information; please refer to Chapter \ref{mod:sysreg} \textit{\nameref{mod:sysreg}} > Section \ref{sec:sysreg-anti-dpa-attack-security-control} \textit{\nameref{sec:sysreg-anti-dpa-attack-security-control}}.
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RECOVERY_BOOTLOADER_FLASH_SECTOR_HI[] = {
    &RECOVERY_BOOTLOADER_FLASH_SECTOR_HI[0],    		// [] Represents the starting flash sector (flash sector size is 0x1000) of the recovery bootloader used by the ROM bootloader If the primary bootloader fails. 0 and 0xFFF - this feature is disabled. (The high part of the field)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_EN[] = {
    &SECURE_BOOT_EN[0],    		// [] Represents whether Secure Boot is enabled.1: Enabled0: Disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE[] = {
    &SECURE_BOOT_AGGRESSIVE_REVOKE[0],    		// [] Represents whether aggressive revocation of Secure Boot is enabled.1: Enabled0: Disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KM_XTS_KEY_LENGTH_256[] = {
    &KM_XTS_KEY_LENGTH_256[0],    		// [] Represents which key flash encryption uses.0: XTS-AES-256 key1: XTS-AES-128 key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_TPUW[] = {
    &FLASH_TPUW[0],    		// [] Represents the flash waiting time after power-up. Measurement unit: ms. When the value is less than 15; the waiting time is the programmed value. Otherwise; the waiting time is a fixed value; i.e. 30 ms
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MODE[] = {
    &DIS_DOWNLOAD_MODE[0],    		// [] Represents whether Download mode is disable or enable. 1. Disable 0: Enable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DIRECT_BOOT[] = {
    &DIS_DIRECT_BOOT[0],    		// [] Represents whether direct boot mode is disabled or enabled. 1. Disable 0: Enable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT[] = {
    &DIS_USB_SERIAL_JTAG_ROM_PRINT[0],    		// [] Represents whether print from USB-Serial-JTAG is disabled or enabled. 1. Disable 0: Enable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_LOCK_KM_KEY[] = {
    &LOCK_KM_KEY[0],    		// [] Represents whether the keys in the Key Manager are locked after deployment.0: Not locked1: Locked
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[] = {
    &DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[0],    		// [] Represents whether the USB-Serial-JTAG download function is disabled or enabled. 1: Disable 0: Enable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ENABLE_SECURITY_DOWNLOAD[] = {
    &ENABLE_SECURITY_DOWNLOAD[0],    		// [] Represents whether security download is enabled. Only downloading into flash is supported. Reading/writing RAM or registers is not supported (i.e. stub download is not supported).1: Enabled0: Disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_UART_PRINT_CONTROL[] = {
    &UART_PRINT_CONTROL[0],    		// [] Set the default UARTboot message output mode {0: "Enable"; 1: "Enable when GPIO8 is low at reset"; 2: "Enable when GPIO8 is high at reset"; 3: "Disable"}
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FORCE_SEND_RESUME[] = {
    &FORCE_SEND_RESUME[0],    		// [] Represents whether ROM code is forced to send a resume command during SPI boot.1: Forced. 0: Not forced.
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_VERSION[] = {
    &SECURE_VERSION[0],    		// [] Represents the app secure version used by ESP-IDF anti-rollback feature
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE[] = {
    &SECURE_BOOT_DISABLE_FAST_WAKE[0],    		// [] Represents whether FAST VERIFY ON WAKE is disabled when Secure Boot is enabled.1: Disabled0: Enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_HYS_EN_PAD[] = {
    &HYS_EN_PAD[0],    		// [] Represents whether the hysteresis function of PAD0 – PAD27 is enabled.1: Enabled0: Disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_XTS_DPA_PSEUDO_LEVEL[] = {
    &XTS_DPA_PSEUDO_LEVEL[0],    		// [] Represents the pseudo round level of XTS-AES anti-DPA attack.0: Disabled1: Low2: Moderate3: High
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_XTS_DPA_CLK_ENABLE[] = {
    &XTS_DPA_CLK_ENABLE[0],    		// [] Represents whether XTS-AES anti-DPA attack clock is enabled.0: Disable1: Enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_SHA384_EN[] = {
    &SECURE_BOOT_SHA384_EN[0],    		// [] Represents if the chip supports Secure Boot using SHA-384
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_HUK_GEN_STATE[] = {
    &HUK_GEN_STATE[0],    		// [] Represents whether the HUK generate mode is valid.Odd count of bits with a value of 1: InvalidEven count of bits with a value of 1: Valid
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_XTAL_48M_SEL[] = {
    &XTAL_48M_SEL[0],    		// [] Represents whether XTAL frequency is 48MHz or not. If not; 40MHz XTAL will be used. If this field contains Odd number bit 1: Enable 48MHz XTAL\ Even number bit 1: Enable 40MHz XTAL
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_XTAL_48M_SEL_MODE[] = {
    &XTAL_48M_SEL_MODE[0],    		// [] Represents what determines the XTAL frequency in \textbf{Joint Download Boot} mode.  For more information; please refer to Chapter \ref{mod:bootctrl} \textit{\nameref{mod:bootctrl}}.0: Strapping PAD state1: \hyperref[fielddesc:EFUSEXTAL48MSEL]{EFUSE\_XTAL\_48M\_SEL} in eFuse
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ECC_FORCE_CONST_TIME[] = {
    &ECC_FORCE_CONST_TIME[0],    		// [] Represents whether to force ECC to use constant-time mode for point multiplication calculation. 0: Not force1: Force
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RECOVERY_BOOTLOADER_FLASH_SECTOR_LO[] = {
    &RECOVERY_BOOTLOADER_FLASH_SECTOR_LO[0],    		// [] Represents the starting flash sector (flash sector size is 0x1000) of the recovery bootloader used by the ROM bootloader If the primary bootloader fails. 0 and 0xFFF - this feature is disabled. (The low part of the field)
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

const esp_efuse_desc_t* ESP_EFUSE_MAC_EXT[] = {
    &MAC_EXT[0],    		// [] Represents the extended bits of MAC address
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WAFER_VERSION_MINOR[] = {
    &WAFER_VERSION_MINOR[0],    		// [] Minor chip version
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WAFER_VERSION_MAJOR[] = {
    &WAFER_VERSION_MAJOR[0],    		// [] Minor chip version
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
    &PSRAM_CAP[0],    		// [] Psram capacity
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PSRAM_VENDOR[] = {
    &PSRAM_VENDOR[0],    		// [] Psram vendor
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_TEMP[] = {
    &TEMP[0],    		// [] Temp (die embedded inside)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PKG_VERSION[] = {
    &PKG_VERSION[0],    		// [] Package version
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PA_TRIM_VERSION[] = {
    &PA_TRIM_VERSION[0],    		// [] PADC CAL PA trim version
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_TRIM_N_BIAS[] = {
    &TRIM_N_BIAS[0],    		// [] PADC CAL N bias
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_TRIM_P_BIAS[] = {
    &TRIM_P_BIAS[0],    		// [] PADC CAL P bias
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ACTIVE_HP_DBIAS[] = {
    &ACTIVE_HP_DBIAS[0],    		// [] Active HP DBIAS of fixed voltage
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ACTIVE_LP_DBIAS[] = {
    &ACTIVE_LP_DBIAS[0],    		// [] Active LP DBIAS of fixed voltage
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_LSLP_HP_DBG[] = {
    &LSLP_HP_DBG[0],    		// [] LSLP HP DBG of fixed voltage
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_LSLP_HP_DBIAS[] = {
    &LSLP_HP_DBIAS[0],    		// [] LSLP HP DBIAS of fixed voltage
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DSLP_LP_DBG[] = {
    &DSLP_LP_DBG[0],    		// [] DSLP LP DBG of fixed voltage
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DSLP_LP_DBIAS[] = {
    &DSLP_LP_DBIAS[0],    		// [] DSLP LP DBIAS of fixed voltage
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_LP_HP_DBIAS_VOL_GAP[] = {
    &LP_HP_DBIAS_VOL_GAP[0],    		// [] DBIAS gap between LP and HP
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_REF_CURR_CODE[] = {
    &REF_CURR_CODE[0],    		// [] REF PADC Calibration Curr
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RES_TUNE_CODE[] = {
    &RES_TUNE_CODE[0],    		// [] RES PADC Calibration Tune
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
    &OCODE[0],    		// [] ADC OCode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_AVE_INITCODE_ATTEN0[] = {
    &ADC1_AVE_INITCODE_ATTEN0[0],    		// [] Average initcode of ADC1 atten0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_AVE_INITCODE_ATTEN1[] = {
    &ADC1_AVE_INITCODE_ATTEN1[0],    		// [] Average initcode of ADC1 atten0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_AVE_INITCODE_ATTEN2[] = {
    &ADC1_AVE_INITCODE_ATTEN2[0],    		// [] Average initcode of ADC1 atten0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_AVE_INITCODE_ATTEN3[] = {
    &ADC1_AVE_INITCODE_ATTEN3[0],    		// [] Average initcode of ADC1 atten0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_HI_DOUT_ATTEN0[] = {
    &ADC1_HI_DOUT_ATTEN0[0],    		// [] HI DOUT of ADC1 atten0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_HI_DOUT_ATTEN1[] = {
    &ADC1_HI_DOUT_ATTEN1[0],    		// [] HI DOUT of ADC1 atten1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_HI_DOUT_ATTEN2[] = {
    &ADC1_HI_DOUT_ATTEN2[0],    		// [] HI DOUT of ADC1 atten2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_HI_DOUT_ATTEN3[] = {
    &ADC1_HI_DOUT_ATTEN3[0],    		// [] HI DOUT of ADC1 atten3
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

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CH4_ATTEN0_INITCODE_DIFF[] = {
    &ADC1_CH4_ATTEN0_INITCODE_DIFF[0],    		// [] Gap between ADC1 CH4 and average initcode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CH5_ATTEN0_INITCODE_DIFF[] = {
    &ADC1_CH5_ATTEN0_INITCODE_DIFF[0],    		// [] Gap between ADC1 CH5 and average initcode
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
