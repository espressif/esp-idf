/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_efuse.h"
#include <assert.h>
#include "esp_efuse_table.h"

// md5_digest_table c56ed98dde7a08c8f70d57a01faba96a
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

static const esp_efuse_desc_t WR_DIS_XTS_KEY_LENGTH_256[] = {
    {EFUSE_BLK0, 1, 1}, 	 // [] wr_dis of XTS_KEY_LENGTH_256,
};

static const esp_efuse_desc_t WR_DIS_LOCK_KM_KEY[] = {
    {EFUSE_BLK0, 1, 1}, 	 // [] wr_dis of LOCK_KM_KEY,
};

static const esp_efuse_desc_t WR_DIS_KM_DISABLE_DEPLOY_MODE[] = {
    {EFUSE_BLK0, 1, 1}, 	 // [] wr_dis of KM_DISABLE_DEPLOY_MODE,
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

static const esp_efuse_desc_t WR_DIS_WDT_DELAY_SEL[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of WDT_DELAY_SEL,
};

static const esp_efuse_desc_t WR_DIS_HYS_EN_PAD[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of HYS_EN_PAD,
};

static const esp_efuse_desc_t WR_DIS_PXA0_TIEH_SEL_0[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of PXA0_TIEH_SEL_0,
};

static const esp_efuse_desc_t WR_DIS_PXA0_TIEH_SEL_1[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of PXA0_TIEH_SEL_1,
};

static const esp_efuse_desc_t WR_DIS_PXA0_TIEH_SEL_2[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of PXA0_TIEH_SEL_2,
};

static const esp_efuse_desc_t WR_DIS_PXA0_TIEH_SEL_3[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of PXA0_TIEH_SEL_3,
};

static const esp_efuse_desc_t WR_DIS_DIS_WDT[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of DIS_WDT,
};

static const esp_efuse_desc_t WR_DIS_DIS_SWD[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of DIS_SWD,
};

static const esp_efuse_desc_t WR_DIS_HP_PWR_SRC_SEL[] = {
    {EFUSE_BLK0, 3, 1}, 	 // [] wr_dis of HP_PWR_SRC_SEL,
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

static const esp_efuse_desc_t WR_DIS_CRYPT_DPA_ENABLE[] = {
    {EFUSE_BLK0, 14, 1}, 	 // [] wr_dis of CRYPT_DPA_ENABLE,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_EN[] = {
    {EFUSE_BLK0, 15, 1}, 	 // [] wr_dis of SECURE_BOOT_EN,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_AGGRESSIVE_REVOKE[] = {
    {EFUSE_BLK0, 16, 1}, 	 // [] wr_dis of SECURE_BOOT_AGGRESSIVE_REVOKE,
};

static const esp_efuse_desc_t WR_DIS_ECDSA_ENABLE_SOFT_K[] = {
    {EFUSE_BLK0, 17, 1}, 	 // [] wr_dis of ECDSA_ENABLE_SOFT_K,
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

static const esp_efuse_desc_t WR_DIS_DIS_USB_OTG_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] wr_dis of DIS_USB_OTG_DOWNLOAD_MODE,
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

static const esp_efuse_desc_t WR_DIS_KM_HUK_GEN_STATE[] = {
    {EFUSE_BLK0, 19, 1}, 	 // [] wr_dis of KM_HUK_GEN_STATE,
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

static const esp_efuse_desc_t WR_DIS_PSRAM_CAP[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of PSRAM_CAP,
};

static const esp_efuse_desc_t WR_DIS_TEMP[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of TEMP,
};

static const esp_efuse_desc_t WR_DIS_PSRAM_VENDOR[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of PSRAM_VENDOR,
};

static const esp_efuse_desc_t WR_DIS_PKG_VERSION[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of PKG_VERSION,
};

static const esp_efuse_desc_t WR_DIS_SYS_DATA_PART1[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of BLOCK2,
};

static const esp_efuse_desc_t WR_DIS_LDO_VO1_DREF[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of LDO_VO1_DREF,
};

static const esp_efuse_desc_t WR_DIS_LDO_VO2_DREF[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of LDO_VO2_DREF,
};

static const esp_efuse_desc_t WR_DIS_LDO_VO1_MUL[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of LDO_VO1_MUL,
};

static const esp_efuse_desc_t WR_DIS_LDO_VO2_MUL[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of LDO_VO2_MUL,
};

static const esp_efuse_desc_t WR_DIS_LDO_VO3_K[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of LDO_VO3_K,
};

static const esp_efuse_desc_t WR_DIS_LDO_VO3_VOS[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of LDO_VO3_VOS,
};

static const esp_efuse_desc_t WR_DIS_LDO_VO3_C[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of LDO_VO3_C,
};

static const esp_efuse_desc_t WR_DIS_LDO_VO4_K[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of LDO_VO4_K,
};

static const esp_efuse_desc_t WR_DIS_LDO_VO4_VOS[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of LDO_VO4_VOS,
};

static const esp_efuse_desc_t WR_DIS_LDO_VO4_C[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of LDO_VO4_C,
};

static const esp_efuse_desc_t WR_DIS_ACTIVE_HP_DBIAS[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of ACTIVE_HP_DBIAS,
};

static const esp_efuse_desc_t WR_DIS_ACTIVE_LP_DBIAS[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of ACTIVE_LP_DBIAS,
};

static const esp_efuse_desc_t WR_DIS_LSLP_HP_DBIAS[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of LSLP_HP_DBIAS,
};

static const esp_efuse_desc_t WR_DIS_DSLP_DBG[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of DSLP_DBG,
};

static const esp_efuse_desc_t WR_DIS_DSLP_LP_DBIAS[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of DSLP_LP_DBIAS,
};

static const esp_efuse_desc_t WR_DIS_LP_DCDC_DBIAS_VOL_GAP[] = {
    {EFUSE_BLK0, 20, 1}, 	 // [] wr_dis of LP_DCDC_DBIAS_VOL_GAP,
};

static const esp_efuse_desc_t WR_DIS_OPTIONAL_UNIQUE_ID[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of OPTIONAL_UNIQUE_ID,
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

static const esp_efuse_desc_t WR_DIS_ADC2_AVE_INITCODE_ATTEN0[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC2_AVE_INITCODE_ATTEN0,
};

static const esp_efuse_desc_t WR_DIS_ADC2_AVE_INITCODE_ATTEN1[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC2_AVE_INITCODE_ATTEN1,
};

static const esp_efuse_desc_t WR_DIS_ADC2_AVE_INITCODE_ATTEN2[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC2_AVE_INITCODE_ATTEN2,
};

static const esp_efuse_desc_t WR_DIS_ADC2_AVE_INITCODE_ATTEN3[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of ADC2_AVE_INITCODE_ATTEN3,
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

static const esp_efuse_desc_t WR_DIS_ADC2_HI_DOUT_ATTEN0[] = {
    {EFUSE_BLK0, 29, 1}, 	 // [] wr_dis of ADC2_HI_DOUT_ATTEN0,
};

static const esp_efuse_desc_t WR_DIS_ADC2_HI_DOUT_ATTEN1[] = {
    {EFUSE_BLK0, 29, 1}, 	 // [] wr_dis of ADC2_HI_DOUT_ATTEN1,
};

static const esp_efuse_desc_t WR_DIS_ADC2_HI_DOUT_ATTEN2[] = {
    {EFUSE_BLK0, 29, 1}, 	 // [] wr_dis of ADC2_HI_DOUT_ATTEN2,
};

static const esp_efuse_desc_t WR_DIS_ADC2_HI_DOUT_ATTEN3[] = {
    {EFUSE_BLK0, 29, 1}, 	 // [] wr_dis of ADC2_HI_DOUT_ATTEN3,
};

static const esp_efuse_desc_t WR_DIS_ADC1_CH0_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 29, 1}, 	 // [] wr_dis of ADC1_CH0_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t WR_DIS_ADC1_CH1_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 29, 1}, 	 // [] wr_dis of ADC1_CH1_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t WR_DIS_ADC1_CH2_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 29, 1}, 	 // [] wr_dis of ADC1_CH2_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t WR_DIS_ADC1_CH3_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 29, 1}, 	 // [] wr_dis of ADC1_CH3_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t WR_DIS_ADC1_CH4_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 29, 1}, 	 // [] wr_dis of ADC1_CH4_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t WR_DIS_ADC1_CH5_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 29, 1}, 	 // [] wr_dis of ADC1_CH5_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t WR_DIS_ADC1_CH6_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 29, 1}, 	 // [] wr_dis of ADC1_CH6_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t WR_DIS_ADC1_CH7_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 29, 1}, 	 // [] wr_dis of ADC1_CH7_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t WR_DIS_ADC2_CH0_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 29, 1}, 	 // [] wr_dis of ADC2_CH0_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t WR_DIS_ADC2_CH1_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 29, 1}, 	 // [] wr_dis of ADC2_CH1_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t WR_DIS_ADC2_CH2_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 29, 1}, 	 // [] wr_dis of ADC2_CH2_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t WR_DIS_ADC2_CH3_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 29, 1}, 	 // [] wr_dis of ADC2_CH3_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t WR_DIS_ADC2_CH4_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 29, 1}, 	 // [] wr_dis of ADC2_CH4_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t WR_DIS_ADC2_CH5_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 29, 1}, 	 // [] wr_dis of ADC2_CH5_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t WR_DIS_TEMPERATURE_SENSOR[] = {
    {EFUSE_BLK0, 29, 1}, 	 // [] wr_dis of TEMPERATURE_SENSOR,
};

static const esp_efuse_desc_t WR_DIS_USB_DEVICE_EXCHG_PINS[] = {
    {EFUSE_BLK0, 30, 1}, 	 // [] wr_dis of USB_DEVICE_EXCHG_PINS,
};

static const esp_efuse_desc_t WR_DIS_USB_OTG11_EXCHG_PINS[] = {
    {EFUSE_BLK0, 30, 1}, 	 // [] wr_dis of USB_OTG11_EXCHG_PINS,
};

static const esp_efuse_desc_t WR_DIS_USB_PHY_SEL[] = {
    {EFUSE_BLK0, 30, 1}, 	 // [] wr_dis of USB_PHY_SEL,
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

static const esp_efuse_desc_t RD_DIS_ADC2_HI_DOUT_ATTEN0[] = {
    {EFUSE_BLK0, 38, 1}, 	 // [] rd_dis of ADC2_HI_DOUT_ATTEN0,
};

static const esp_efuse_desc_t RD_DIS_ADC2_HI_DOUT_ATTEN1[] = {
    {EFUSE_BLK0, 38, 1}, 	 // [] rd_dis of ADC2_HI_DOUT_ATTEN1,
};

static const esp_efuse_desc_t RD_DIS_ADC2_HI_DOUT_ATTEN2[] = {
    {EFUSE_BLK0, 38, 1}, 	 // [] rd_dis of ADC2_HI_DOUT_ATTEN2,
};

static const esp_efuse_desc_t RD_DIS_ADC2_HI_DOUT_ATTEN3[] = {
    {EFUSE_BLK0, 38, 1}, 	 // [] rd_dis of ADC2_HI_DOUT_ATTEN3,
};

static const esp_efuse_desc_t RD_DIS_ADC1_CH0_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 38, 1}, 	 // [] rd_dis of ADC1_CH0_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t RD_DIS_ADC1_CH1_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 38, 1}, 	 // [] rd_dis of ADC1_CH1_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t RD_DIS_ADC1_CH2_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 38, 1}, 	 // [] rd_dis of ADC1_CH2_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t RD_DIS_ADC1_CH3_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 38, 1}, 	 // [] rd_dis of ADC1_CH3_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t RD_DIS_ADC1_CH4_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 38, 1}, 	 // [] rd_dis of ADC1_CH4_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t RD_DIS_ADC1_CH5_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 38, 1}, 	 // [] rd_dis of ADC1_CH5_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t RD_DIS_ADC1_CH6_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 38, 1}, 	 // [] rd_dis of ADC1_CH6_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t RD_DIS_ADC1_CH7_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 38, 1}, 	 // [] rd_dis of ADC1_CH7_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t RD_DIS_ADC2_CH0_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 38, 1}, 	 // [] rd_dis of ADC2_CH0_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t RD_DIS_ADC2_CH1_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 38, 1}, 	 // [] rd_dis of ADC2_CH1_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t RD_DIS_ADC2_CH2_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 38, 1}, 	 // [] rd_dis of ADC2_CH2_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t RD_DIS_ADC2_CH3_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 38, 1}, 	 // [] rd_dis of ADC2_CH3_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t RD_DIS_ADC2_CH4_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 38, 1}, 	 // [] rd_dis of ADC2_CH4_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t RD_DIS_ADC2_CH5_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK0, 38, 1}, 	 // [] rd_dis of ADC2_CH5_ATTEN0_INITCODE_DIFF,
};

static const esp_efuse_desc_t RD_DIS_TEMPERATURE_SENSOR[] = {
    {EFUSE_BLK0, 38, 1}, 	 // [] rd_dis of TEMPERATURE_SENSOR,
};

static const esp_efuse_desc_t USB_DEVICE_EXCHG_PINS[] = {
    {EFUSE_BLK0, 39, 1}, 	 // [] Enable usb device exchange pins of D+ and D-,
};

static const esp_efuse_desc_t USB_OTG11_EXCHG_PINS[] = {
    {EFUSE_BLK0, 40, 1}, 	 // [] Enable usb otg11 exchange pins of D+ and D-,
};

static const esp_efuse_desc_t DIS_USB_JTAG[] = {
    {EFUSE_BLK0, 41, 1}, 	 // [] Represents whether the function of usb switch to jtag is disabled or enabled. 1: disabled. 0: enabled,
};

static const esp_efuse_desc_t POWERGLITCH_EN[] = {
    {EFUSE_BLK0, 42, 1}, 	 // [] Represents whether power glitch function is enabled. 1: enabled. 0: disabled,
};

static const esp_efuse_desc_t DIS_FORCE_DOWNLOAD[] = {
    {EFUSE_BLK0, 44, 1}, 	 // [] Represents whether the function that forces chip into download mode is disabled or enabled. 1: disabled. 0: enabled,
};

static const esp_efuse_desc_t SPI_DOWNLOAD_MSPI_DIS[] = {
    {EFUSE_BLK0, 45, 1}, 	 // [] Set this bit to disable accessing MSPI flash/MSPI ram by SYS AXI matrix during boot_mode_download,
};

static const esp_efuse_desc_t DIS_TWAI[] = {
    {EFUSE_BLK0, 46, 1}, 	 // [] Represents whether TWAI function is disabled or enabled. 1: disabled. 0: enabled,
};

static const esp_efuse_desc_t JTAG_SEL_ENABLE[] = {
    {EFUSE_BLK0, 47, 1}, 	 // [] Represents whether the selection between usb_to_jtag and pad_to_jtag through strapping gpio15 when both EFUSE_DIS_PAD_JTAG and EFUSE_DIS_USB_JTAG are equal to 0 is enabled or disabled. 1: enabled. 0: disabled,
};

static const esp_efuse_desc_t SOFT_DIS_JTAG[] = {
    {EFUSE_BLK0, 48, 3}, 	 // [] Represents whether JTAG is disabled in soft way. Odd number: disabled. Even number: enabled,
};

static const esp_efuse_desc_t DIS_PAD_JTAG[] = {
    {EFUSE_BLK0, 51, 1}, 	 // [] Represents whether JTAG is disabled in the hard way(permanently). 1: disabled. 0: enabled,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    {EFUSE_BLK0, 52, 1}, 	 // [] Represents whether flash encrypt function is disabled or enabled(except in SPI boot mode). 1: disabled. 0: enabled,
};

static const esp_efuse_desc_t USB_PHY_SEL[] = {
    {EFUSE_BLK0, 57, 1}, 	 // [] TBD,
};

static const esp_efuse_desc_t KM_HUK_GEN_STATE[] = {
    {EFUSE_BLK0, 58, 9}, 	 // [] Set this bit to control validation of HUK generate mode. Odd of 1 is invalid; even of 1 is valid,
};

static const esp_efuse_desc_t KM_RND_SWITCH_CYCLE[] = {
    {EFUSE_BLK0, 67, 2}, 	 // [] Set bits to control key manager random number switch cycle. 0: control by register. 1: 8 km clk cycles. 2: 16 km cycles. 3: 32 km cycles,
};

static const esp_efuse_desc_t KM_DEPLOY_ONLY_ONCE[] = {
    {EFUSE_BLK0, 69, 4}, 	 // [] Set each bit to control whether corresponding key can only be deployed once. 1 is true; 0 is false. Bit0: ecdsa. Bit1: xts. Bit2: hmac. Bit3: ds,
};

static const esp_efuse_desc_t FORCE_USE_KEY_MANAGER_KEY[] = {
    {EFUSE_BLK0, 73, 4}, 	 // [] Set each bit to control whether corresponding key must come from key manager.. 1 is true; 0 is false. Bit0: ecdsa. Bit1: xts. Bit2: hmac. Bit3: ds,
};

static const esp_efuse_desc_t FORCE_DISABLE_SW_INIT_KEY[] = {
    {EFUSE_BLK0, 77, 1}, 	 // [] Set this bit to disable software written init key; and force use efuse_init_key,
};

static const esp_efuse_desc_t XTS_KEY_LENGTH_256[] = {
    {EFUSE_BLK0, 78, 1}, 	 // [] Set this bit to configure flash encryption use xts-128 key; else use xts-256 key,
};

static const esp_efuse_desc_t WDT_DELAY_SEL[] = {
    {EFUSE_BLK0, 80, 2}, 	 // [] Represents whether RTC watchdog timeout threshold is selected at startup. 1: selected. 0: not selected,
};

static const esp_efuse_desc_t SPI_BOOT_CRYPT_CNT[] = {
    {EFUSE_BLK0, 82, 3}, 	 // [] Enables flash encryption when 1 or 3 bits are set and disables otherwise {0: "Disable"; 1: "Enable"; 3: "Disable"; 7: "Enable"},
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
    {EFUSE_BLK0, 88, 4}, 	 // [KEY0_PURPOSE] Represents the purpose of Key0,
};

static const esp_efuse_desc_t KEY_PURPOSE_1[] = {
    {EFUSE_BLK0, 92, 4}, 	 // [KEY1_PURPOSE] Represents the purpose of Key1,
};

static const esp_efuse_desc_t KEY_PURPOSE_2[] = {
    {EFUSE_BLK0, 96, 4}, 	 // [KEY2_PURPOSE] Represents the purpose of Key2,
};

static const esp_efuse_desc_t KEY_PURPOSE_3[] = {
    {EFUSE_BLK0, 100, 4}, 	 // [KEY3_PURPOSE] Represents the purpose of Key3,
};

static const esp_efuse_desc_t KEY_PURPOSE_4[] = {
    {EFUSE_BLK0, 104, 4}, 	 // [KEY4_PURPOSE] Represents the purpose of Key4,
};

static const esp_efuse_desc_t KEY_PURPOSE_5[] = {
    {EFUSE_BLK0, 108, 4}, 	 // [KEY5_PURPOSE] Represents the purpose of Key5,
};

static const esp_efuse_desc_t SEC_DPA_LEVEL[] = {
    {EFUSE_BLK0, 112, 2}, 	 // [] Represents the spa secure level by configuring the clock random divide mode,
};

static const esp_efuse_desc_t ECDSA_ENABLE_SOFT_K[] = {
    {EFUSE_BLK0, 114, 1}, 	 // [] Represents whether hardware random number k is forced used in ESDCA. 1: force used. 0: not force used,
};

static const esp_efuse_desc_t CRYPT_DPA_ENABLE[] = {
    {EFUSE_BLK0, 115, 1}, 	 // [] Represents whether anti-dpa attack is enabled. 1:enabled. 0: disabled,
};

static const esp_efuse_desc_t SECURE_BOOT_EN[] = {
    {EFUSE_BLK0, 116, 1}, 	 // [] Represents whether secure boot is enabled or disabled. 1: enabled. 0: disabled,
};

static const esp_efuse_desc_t SECURE_BOOT_AGGRESSIVE_REVOKE[] = {
    {EFUSE_BLK0, 117, 1}, 	 // [] Represents whether revoking aggressive secure boot is enabled or disabled. 1: enabled. 0: disabled,
};

static const esp_efuse_desc_t FLASH_TYPE[] = {
    {EFUSE_BLK0, 119, 1}, 	 // [] The type of interfaced flash. 0: four data lines; 1: eight data lines,
};

static const esp_efuse_desc_t FLASH_PAGE_SIZE[] = {
    {EFUSE_BLK0, 120, 2}, 	 // [] Set flash page size,
};

static const esp_efuse_desc_t FLASH_ECC_EN[] = {
    {EFUSE_BLK0, 122, 1}, 	 // [] Set this bit to enable ecc for flash boot,
};

static const esp_efuse_desc_t DIS_USB_OTG_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 123, 1}, 	 // [] Set this bit to disable download via USB-OTG,
};

static const esp_efuse_desc_t FLASH_TPUW[] = {
    {EFUSE_BLK0, 124, 4}, 	 // [] Represents the flash waiting time after power-up; in unit of ms. When the value less than 15; the waiting time is the programmed value. Otherwise; the waiting time is 2 times the programmed value,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 128, 1}, 	 // [] Represents whether Download mode is disabled or enabled. 1: disabled. 0: enabled,
};

static const esp_efuse_desc_t DIS_DIRECT_BOOT[] = {
    {EFUSE_BLK0, 129, 1}, 	 // [] Represents whether direct boot mode is disabled or enabled. 1: disabled. 0: enabled,
};

static const esp_efuse_desc_t DIS_USB_SERIAL_JTAG_ROM_PRINT[] = {
    {EFUSE_BLK0, 130, 1}, 	 // [] Represents whether print from USB-Serial-JTAG is disabled or enabled. 1: disabled. 0: enabled,
};

static const esp_efuse_desc_t LOCK_KM_KEY[] = {
    {EFUSE_BLK0, 131, 1}, 	 // [] TBD,
};

static const esp_efuse_desc_t DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 132, 1}, 	 // [] Represents whether the USB-Serial-JTAG download function is disabled or enabled. 1: disabled. 0: enabled,
};

static const esp_efuse_desc_t ENABLE_SECURITY_DOWNLOAD[] = {
    {EFUSE_BLK0, 133, 1}, 	 // [] Represents whether security download is enabled or disabled. 1: enabled. 0: disabled,
};

static const esp_efuse_desc_t UART_PRINT_CONTROL[] = {
    {EFUSE_BLK0, 134, 2}, 	 // [] Represents the type of UART printing. 00: force enable printing. 01: enable printing when GPIO8 is reset at low level. 10: enable printing when GPIO8 is reset at high level. 11: force disable printing,
};

static const esp_efuse_desc_t FORCE_SEND_RESUME[] = {
    {EFUSE_BLK0, 136, 1}, 	 // [] Represents whether ROM code is forced to send a resume command during SPI boot. 1: forced. 0:not forced,
};

static const esp_efuse_desc_t SECURE_VERSION[] = {
    {EFUSE_BLK0, 137, 16}, 	 // [] Represents the version used by ESP-IDF anti-rollback feature,
};

static const esp_efuse_desc_t SECURE_BOOT_DISABLE_FAST_WAKE[] = {
    {EFUSE_BLK0, 153, 1}, 	 // [] Represents whether FAST VERIFY ON WAKE is disabled or enabled when Secure Boot is enabled. 1: disabled. 0: enabled,
};

static const esp_efuse_desc_t HYS_EN_PAD[] = {
    {EFUSE_BLK0, 154, 1}, 	 // [] Represents whether the hysteresis function of corresponding PAD is enabled. 1: enabled. 0:disabled,
};

static const esp_efuse_desc_t DCDC_VSET[] = {
    {EFUSE_BLK0, 155, 5}, 	 // [] Set the dcdc voltage default,
};

static const esp_efuse_desc_t PXA0_TIEH_SEL_0[] = {
    {EFUSE_BLK0, 160, 2}, 	 // [] TBD,
};

static const esp_efuse_desc_t PXA0_TIEH_SEL_1[] = {
    {EFUSE_BLK0, 162, 2}, 	 // [] TBD,
};

static const esp_efuse_desc_t PXA0_TIEH_SEL_2[] = {
    {EFUSE_BLK0, 164, 2}, 	 // [] TBD,
};

static const esp_efuse_desc_t PXA0_TIEH_SEL_3[] = {
    {EFUSE_BLK0, 166, 2}, 	 // [] TBD,
};

static const esp_efuse_desc_t KM_DISABLE_DEPLOY_MODE[] = {
    {EFUSE_BLK0, 168, 4}, 	 // [] TBD,
};

static const esp_efuse_desc_t HP_PWR_SRC_SEL[] = {
    {EFUSE_BLK0, 178, 1}, 	 // [] HP system power source select. 0:LDO. 1: DCDC,
};

static const esp_efuse_desc_t DCDC_VSET_EN[] = {
    {EFUSE_BLK0, 179, 1}, 	 // [] Select dcdc vset use efuse_dcdc_vset,
};

static const esp_efuse_desc_t DIS_WDT[] = {
    {EFUSE_BLK0, 180, 1}, 	 // [] Set this bit to disable watch dog,
};

static const esp_efuse_desc_t DIS_SWD[] = {
    {EFUSE_BLK0, 181, 1}, 	 // [] Set this bit to disable super-watchdog,
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

static const esp_efuse_desc_t PSRAM_CAP[] = {
    {EFUSE_BLK1, 77, 3}, 	 // [] PSRAM capacity,
};

static const esp_efuse_desc_t TEMP[] = {
    {EFUSE_BLK1, 80, 2}, 	 // [] Operating temperature of the ESP chip,
};

static const esp_efuse_desc_t PSRAM_VENDOR[] = {
    {EFUSE_BLK1, 82, 2}, 	 // [] PSRAM vendor,
};

static const esp_efuse_desc_t PKG_VERSION[] = {
    {EFUSE_BLK1, 84, 3}, 	 // [] Package version,
};

static const esp_efuse_desc_t LDO_VO1_DREF[] = {
    {EFUSE_BLK1, 88, 4}, 	 // [] Output VO1 parameter,
};

static const esp_efuse_desc_t LDO_VO2_DREF[] = {
    {EFUSE_BLK1, 92, 4}, 	 // [] Output VO2 parameter,
};

static const esp_efuse_desc_t LDO_VO1_MUL[] = {
    {EFUSE_BLK1, 96, 3}, 	 // [] Output VO1 parameter,
};

static const esp_efuse_desc_t LDO_VO2_MUL[] = {
    {EFUSE_BLK1, 99, 3}, 	 // [] Output VO2 parameter,
};

static const esp_efuse_desc_t LDO_VO3_K[] = {
    {EFUSE_BLK1, 102, 8}, 	 // [] Output VO3 calibration parameter,
};

static const esp_efuse_desc_t LDO_VO3_VOS[] = {
    {EFUSE_BLK1, 110, 6}, 	 // [] Output VO3 calibration parameter,
};

static const esp_efuse_desc_t LDO_VO3_C[] = {
    {EFUSE_BLK1, 116, 6}, 	 // [] Output VO3 calibration parameter,
};

static const esp_efuse_desc_t LDO_VO4_K[] = {
    {EFUSE_BLK1, 122, 8}, 	 // [] Output VO4 calibration parameter,
};

static const esp_efuse_desc_t LDO_VO4_VOS[] = {
    {EFUSE_BLK1, 130, 6}, 	 // [] Output VO4 calibration parameter,
};

static const esp_efuse_desc_t LDO_VO4_C[] = {
    {EFUSE_BLK1, 136, 6}, 	 // [] Output VO4 calibration parameter,
};

static const esp_efuse_desc_t ACTIVE_HP_DBIAS[] = {
    {EFUSE_BLK1, 144, 4}, 	 // [] Active HP DBIAS of fixed voltage,
};

static const esp_efuse_desc_t ACTIVE_LP_DBIAS[] = {
    {EFUSE_BLK1, 148, 4}, 	 // [] Active LP DBIAS of fixed voltage,
};

static const esp_efuse_desc_t LSLP_HP_DBIAS[] = {
    {EFUSE_BLK1, 152, 4}, 	 // [] LSLP HP DBIAS of fixed voltage,
};

static const esp_efuse_desc_t DSLP_DBG[] = {
    {EFUSE_BLK1, 156, 4}, 	 // [] DSLP BDG of fixed voltage,
};

static const esp_efuse_desc_t DSLP_LP_DBIAS[] = {
    {EFUSE_BLK1, 160, 5}, 	 // [] DSLP LP DBIAS of fixed voltage,
};

static const esp_efuse_desc_t LP_DCDC_DBIAS_VOL_GAP[] = {
    {EFUSE_BLK1, 165, 5}, 	 // [] DBIAS gap between LP and DCDC,
};

static const esp_efuse_desc_t OPTIONAL_UNIQUE_ID[] = {
    {EFUSE_BLK2, 0, 128}, 	 // [] Optional unique 128-bit ID,
};

static const esp_efuse_desc_t ADC1_AVE_INITCODE_ATTEN0[] = {
    {EFUSE_BLK2, 128, 10}, 	 // [] Average initcode of ADC1 atten0,
};

static const esp_efuse_desc_t ADC1_AVE_INITCODE_ATTEN1[] = {
    {EFUSE_BLK2, 138, 10}, 	 // [] Average initcode of ADC1 atten1,
};

static const esp_efuse_desc_t ADC1_AVE_INITCODE_ATTEN2[] = {
    {EFUSE_BLK2, 148, 10}, 	 // [] Average initcode of ADC1 atten2,
};

static const esp_efuse_desc_t ADC1_AVE_INITCODE_ATTEN3[] = {
    {EFUSE_BLK2, 158, 10}, 	 // [] Average initcode of ADC1 atten3,
};

static const esp_efuse_desc_t ADC2_AVE_INITCODE_ATTEN0[] = {
    {EFUSE_BLK2, 168, 10}, 	 // [] Average initcode of ADC2 atten0,
};

static const esp_efuse_desc_t ADC2_AVE_INITCODE_ATTEN1[] = {
    {EFUSE_BLK2, 178, 10}, 	 // [] Average initcode of ADC2 atten1,
};

static const esp_efuse_desc_t ADC2_AVE_INITCODE_ATTEN2[] = {
    {EFUSE_BLK2, 188, 10}, 	 // [] Average initcode of ADC2 atten2,
};

static const esp_efuse_desc_t ADC2_AVE_INITCODE_ATTEN3[] = {
    {EFUSE_BLK2, 198, 10}, 	 // [] Average initcode of ADC2 atten3,
};

static const esp_efuse_desc_t ADC1_HI_DOUT_ATTEN0[] = {
    {EFUSE_BLK2, 208, 10}, 	 // [] HI_DOUT of ADC1 atten0,
};

static const esp_efuse_desc_t ADC1_HI_DOUT_ATTEN1[] = {
    {EFUSE_BLK2, 218, 10}, 	 // [] HI_DOUT of ADC1 atten1,
};

static const esp_efuse_desc_t ADC1_HI_DOUT_ATTEN2[] = {
    {EFUSE_BLK2, 228, 10}, 	 // [] HI_DOUT of ADC1 atten2,
};

static const esp_efuse_desc_t ADC1_HI_DOUT_ATTEN3[] = {
    {EFUSE_BLK2, 238, 10}, 	 // [] HI_DOUT of ADC1 atten3,
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

static const esp_efuse_desc_t ADC2_HI_DOUT_ATTEN0[] = {
    {EFUSE_BLK10, 0, 10}, 	 // [] HI_DOUT of ADC2 atten0,
};

static const esp_efuse_desc_t ADC2_HI_DOUT_ATTEN1[] = {
    {EFUSE_BLK10, 10, 10}, 	 // [] HI_DOUT of ADC2 atten1,
};

static const esp_efuse_desc_t ADC2_HI_DOUT_ATTEN2[] = {
    {EFUSE_BLK10, 20, 10}, 	 // [] HI_DOUT of ADC2 atten2,
};

static const esp_efuse_desc_t ADC2_HI_DOUT_ATTEN3[] = {
    {EFUSE_BLK10, 30, 10}, 	 // [] HI_DOUT of ADC2 atten3,
};

static const esp_efuse_desc_t ADC1_CH0_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK10, 40, 4}, 	 // [] Gap between ADC1_ch0 and average initcode,
};

static const esp_efuse_desc_t ADC1_CH1_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK10, 44, 4}, 	 // [] Gap between ADC1_ch1 and average initcode,
};

static const esp_efuse_desc_t ADC1_CH2_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK10, 48, 4}, 	 // [] Gap between ADC1_ch2 and average initcode,
};

static const esp_efuse_desc_t ADC1_CH3_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK10, 52, 4}, 	 // [] Gap between ADC1_ch3 and average initcode,
};

static const esp_efuse_desc_t ADC1_CH4_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK10, 56, 4}, 	 // [] Gap between ADC1_ch4 and average initcode,
};

static const esp_efuse_desc_t ADC1_CH5_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK10, 60, 4}, 	 // [] Gap between ADC1_ch5 and average initcode,
};

static const esp_efuse_desc_t ADC1_CH6_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK10, 64, 4}, 	 // [] Gap between ADC1_ch6 and average initcode,
};

static const esp_efuse_desc_t ADC1_CH7_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK10, 68, 4}, 	 // [] Gap between ADC1_ch7 and average initcode,
};

static const esp_efuse_desc_t ADC2_CH0_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK10, 72, 4}, 	 // [] Gap between ADC2_ch0 and average initcode,
};

static const esp_efuse_desc_t ADC2_CH1_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK10, 76, 4}, 	 // [] Gap between ADC2_ch1 and average initcode,
};

static const esp_efuse_desc_t ADC2_CH2_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK10, 80, 4}, 	 // [] Gap between ADC2_ch2 and average initcode,
};

static const esp_efuse_desc_t ADC2_CH3_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK10, 84, 4}, 	 // [] Gap between ADC2_ch3 and average initcode,
};

static const esp_efuse_desc_t ADC2_CH4_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK10, 88, 4}, 	 // [] Gap between ADC2_ch4 and average initcode,
};

static const esp_efuse_desc_t ADC2_CH5_ATTEN0_INITCODE_DIFF[] = {
    {EFUSE_BLK10, 92, 4}, 	 // [] Gap between ADC2_ch5 and average initcode,
};

static const esp_efuse_desc_t TEMPERATURE_SENSOR[] = {
    {EFUSE_BLK10, 96, 9}, 	 // [] Temperature calibration data,
};





const esp_efuse_desc_t* ESP_EFUSE_WR_DIS[] = {
    &WR_DIS[0],    		// [] Disable programming of individual eFuses
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RD_DIS[] = {
    &WR_DIS_RD_DIS[0],    		// [] wr_dis of RD_DIS
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_XTS_KEY_LENGTH_256[] = {
    &WR_DIS_XTS_KEY_LENGTH_256[0],    		// [] wr_dis of XTS_KEY_LENGTH_256
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LOCK_KM_KEY[] = {
    &WR_DIS_LOCK_KM_KEY[0],    		// [] wr_dis of LOCK_KM_KEY
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KM_DISABLE_DEPLOY_MODE[] = {
    &WR_DIS_KM_DISABLE_DEPLOY_MODE[0],    		// [] wr_dis of KM_DISABLE_DEPLOY_MODE
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_WDT_DELAY_SEL[] = {
    &WR_DIS_WDT_DELAY_SEL[0],    		// [] wr_dis of WDT_DELAY_SEL
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_HYS_EN_PAD[] = {
    &WR_DIS_HYS_EN_PAD[0],    		// [] wr_dis of HYS_EN_PAD
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PXA0_TIEH_SEL_0[] = {
    &WR_DIS_PXA0_TIEH_SEL_0[0],    		// [] wr_dis of PXA0_TIEH_SEL_0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PXA0_TIEH_SEL_1[] = {
    &WR_DIS_PXA0_TIEH_SEL_1[0],    		// [] wr_dis of PXA0_TIEH_SEL_1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PXA0_TIEH_SEL_2[] = {
    &WR_DIS_PXA0_TIEH_SEL_2[0],    		// [] wr_dis of PXA0_TIEH_SEL_2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PXA0_TIEH_SEL_3[] = {
    &WR_DIS_PXA0_TIEH_SEL_3[0],    		// [] wr_dis of PXA0_TIEH_SEL_3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_WDT[] = {
    &WR_DIS_DIS_WDT[0],    		// [] wr_dis of DIS_WDT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_SWD[] = {
    &WR_DIS_DIS_SWD[0],    		// [] wr_dis of DIS_SWD
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_HP_PWR_SRC_SEL[] = {
    &WR_DIS_HP_PWR_SRC_SEL[0],    		// [] wr_dis of HP_PWR_SRC_SEL
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_CRYPT_DPA_ENABLE[] = {
    &WR_DIS_CRYPT_DPA_ENABLE[0],    		// [] wr_dis of CRYPT_DPA_ENABLE
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ECDSA_ENABLE_SOFT_K[] = {
    &WR_DIS_ECDSA_ENABLE_SOFT_K[0],    		// [] wr_dis of ECDSA_ENABLE_SOFT_K
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_USB_OTG_DOWNLOAD_MODE[] = {
    &WR_DIS_DIS_USB_OTG_DOWNLOAD_MODE[0],    		// [] wr_dis of DIS_USB_OTG_DOWNLOAD_MODE
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KM_HUK_GEN_STATE[] = {
    &WR_DIS_KM_HUK_GEN_STATE[0],    		// [] wr_dis of KM_HUK_GEN_STATE
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PSRAM_CAP[] = {
    &WR_DIS_PSRAM_CAP[0],    		// [] wr_dis of PSRAM_CAP
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_TEMP[] = {
    &WR_DIS_TEMP[0],    		// [] wr_dis of TEMP
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PSRAM_VENDOR[] = {
    &WR_DIS_PSRAM_VENDOR[0],    		// [] wr_dis of PSRAM_VENDOR
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LDO_VO1_DREF[] = {
    &WR_DIS_LDO_VO1_DREF[0],    		// [] wr_dis of LDO_VO1_DREF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LDO_VO2_DREF[] = {
    &WR_DIS_LDO_VO2_DREF[0],    		// [] wr_dis of LDO_VO2_DREF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LDO_VO1_MUL[] = {
    &WR_DIS_LDO_VO1_MUL[0],    		// [] wr_dis of LDO_VO1_MUL
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LDO_VO2_MUL[] = {
    &WR_DIS_LDO_VO2_MUL[0],    		// [] wr_dis of LDO_VO2_MUL
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LDO_VO3_K[] = {
    &WR_DIS_LDO_VO3_K[0],    		// [] wr_dis of LDO_VO3_K
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LDO_VO3_VOS[] = {
    &WR_DIS_LDO_VO3_VOS[0],    		// [] wr_dis of LDO_VO3_VOS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LDO_VO3_C[] = {
    &WR_DIS_LDO_VO3_C[0],    		// [] wr_dis of LDO_VO3_C
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LDO_VO4_K[] = {
    &WR_DIS_LDO_VO4_K[0],    		// [] wr_dis of LDO_VO4_K
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LDO_VO4_VOS[] = {
    &WR_DIS_LDO_VO4_VOS[0],    		// [] wr_dis of LDO_VO4_VOS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LDO_VO4_C[] = {
    &WR_DIS_LDO_VO4_C[0],    		// [] wr_dis of LDO_VO4_C
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LSLP_HP_DBIAS[] = {
    &WR_DIS_LSLP_HP_DBIAS[0],    		// [] wr_dis of LSLP_HP_DBIAS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DSLP_DBG[] = {
    &WR_DIS_DSLP_DBG[0],    		// [] wr_dis of DSLP_DBG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DSLP_LP_DBIAS[] = {
    &WR_DIS_DSLP_LP_DBIAS[0],    		// [] wr_dis of DSLP_LP_DBIAS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LP_DCDC_DBIAS_VOL_GAP[] = {
    &WR_DIS_LP_DCDC_DBIAS_VOL_GAP[0],    		// [] wr_dis of LP_DCDC_DBIAS_VOL_GAP
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_OPTIONAL_UNIQUE_ID[] = {
    &WR_DIS_OPTIONAL_UNIQUE_ID[0],    		// [] wr_dis of OPTIONAL_UNIQUE_ID
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_AVE_INITCODE_ATTEN0[] = {
    &WR_DIS_ADC2_AVE_INITCODE_ATTEN0[0],    		// [] wr_dis of ADC2_AVE_INITCODE_ATTEN0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_AVE_INITCODE_ATTEN1[] = {
    &WR_DIS_ADC2_AVE_INITCODE_ATTEN1[0],    		// [] wr_dis of ADC2_AVE_INITCODE_ATTEN1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_AVE_INITCODE_ATTEN2[] = {
    &WR_DIS_ADC2_AVE_INITCODE_ATTEN2[0],    		// [] wr_dis of ADC2_AVE_INITCODE_ATTEN2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_AVE_INITCODE_ATTEN3[] = {
    &WR_DIS_ADC2_AVE_INITCODE_ATTEN3[0],    		// [] wr_dis of ADC2_AVE_INITCODE_ATTEN3
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_HI_DOUT_ATTEN0[] = {
    &WR_DIS_ADC2_HI_DOUT_ATTEN0[0],    		// [] wr_dis of ADC2_HI_DOUT_ATTEN0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_HI_DOUT_ATTEN1[] = {
    &WR_DIS_ADC2_HI_DOUT_ATTEN1[0],    		// [] wr_dis of ADC2_HI_DOUT_ATTEN1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_HI_DOUT_ATTEN2[] = {
    &WR_DIS_ADC2_HI_DOUT_ATTEN2[0],    		// [] wr_dis of ADC2_HI_DOUT_ATTEN2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_HI_DOUT_ATTEN3[] = {
    &WR_DIS_ADC2_HI_DOUT_ATTEN3[0],    		// [] wr_dis of ADC2_HI_DOUT_ATTEN3
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_CH6_ATTEN0_INITCODE_DIFF[] = {
    &WR_DIS_ADC1_CH6_ATTEN0_INITCODE_DIFF[0],    		// [] wr_dis of ADC1_CH6_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC1_CH7_ATTEN0_INITCODE_DIFF[] = {
    &WR_DIS_ADC1_CH7_ATTEN0_INITCODE_DIFF[0],    		// [] wr_dis of ADC1_CH7_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_CH0_ATTEN0_INITCODE_DIFF[] = {
    &WR_DIS_ADC2_CH0_ATTEN0_INITCODE_DIFF[0],    		// [] wr_dis of ADC2_CH0_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_CH1_ATTEN0_INITCODE_DIFF[] = {
    &WR_DIS_ADC2_CH1_ATTEN0_INITCODE_DIFF[0],    		// [] wr_dis of ADC2_CH1_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_CH2_ATTEN0_INITCODE_DIFF[] = {
    &WR_DIS_ADC2_CH2_ATTEN0_INITCODE_DIFF[0],    		// [] wr_dis of ADC2_CH2_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_CH3_ATTEN0_INITCODE_DIFF[] = {
    &WR_DIS_ADC2_CH3_ATTEN0_INITCODE_DIFF[0],    		// [] wr_dis of ADC2_CH3_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_CH4_ATTEN0_INITCODE_DIFF[] = {
    &WR_DIS_ADC2_CH4_ATTEN0_INITCODE_DIFF[0],    		// [] wr_dis of ADC2_CH4_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ADC2_CH5_ATTEN0_INITCODE_DIFF[] = {
    &WR_DIS_ADC2_CH5_ATTEN0_INITCODE_DIFF[0],    		// [] wr_dis of ADC2_CH5_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_TEMPERATURE_SENSOR[] = {
    &WR_DIS_TEMPERATURE_SENSOR[0],    		// [] wr_dis of TEMPERATURE_SENSOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_USB_DEVICE_EXCHG_PINS[] = {
    &WR_DIS_USB_DEVICE_EXCHG_PINS[0],    		// [] wr_dis of USB_DEVICE_EXCHG_PINS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_USB_OTG11_EXCHG_PINS[] = {
    &WR_DIS_USB_OTG11_EXCHG_PINS[0],    		// [] wr_dis of USB_OTG11_EXCHG_PINS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_USB_PHY_SEL[] = {
    &WR_DIS_USB_PHY_SEL[0],    		// [] wr_dis of USB_PHY_SEL
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

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC2_HI_DOUT_ATTEN0[] = {
    &RD_DIS_ADC2_HI_DOUT_ATTEN0[0],    		// [] rd_dis of ADC2_HI_DOUT_ATTEN0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC2_HI_DOUT_ATTEN1[] = {
    &RD_DIS_ADC2_HI_DOUT_ATTEN1[0],    		// [] rd_dis of ADC2_HI_DOUT_ATTEN1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC2_HI_DOUT_ATTEN2[] = {
    &RD_DIS_ADC2_HI_DOUT_ATTEN2[0],    		// [] rd_dis of ADC2_HI_DOUT_ATTEN2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC2_HI_DOUT_ATTEN3[] = {
    &RD_DIS_ADC2_HI_DOUT_ATTEN3[0],    		// [] rd_dis of ADC2_HI_DOUT_ATTEN3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC1_CH0_ATTEN0_INITCODE_DIFF[] = {
    &RD_DIS_ADC1_CH0_ATTEN0_INITCODE_DIFF[0],    		// [] rd_dis of ADC1_CH0_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC1_CH1_ATTEN0_INITCODE_DIFF[] = {
    &RD_DIS_ADC1_CH1_ATTEN0_INITCODE_DIFF[0],    		// [] rd_dis of ADC1_CH1_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC1_CH2_ATTEN0_INITCODE_DIFF[] = {
    &RD_DIS_ADC1_CH2_ATTEN0_INITCODE_DIFF[0],    		// [] rd_dis of ADC1_CH2_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC1_CH3_ATTEN0_INITCODE_DIFF[] = {
    &RD_DIS_ADC1_CH3_ATTEN0_INITCODE_DIFF[0],    		// [] rd_dis of ADC1_CH3_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC1_CH4_ATTEN0_INITCODE_DIFF[] = {
    &RD_DIS_ADC1_CH4_ATTEN0_INITCODE_DIFF[0],    		// [] rd_dis of ADC1_CH4_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC1_CH5_ATTEN0_INITCODE_DIFF[] = {
    &RD_DIS_ADC1_CH5_ATTEN0_INITCODE_DIFF[0],    		// [] rd_dis of ADC1_CH5_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC1_CH6_ATTEN0_INITCODE_DIFF[] = {
    &RD_DIS_ADC1_CH6_ATTEN0_INITCODE_DIFF[0],    		// [] rd_dis of ADC1_CH6_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC1_CH7_ATTEN0_INITCODE_DIFF[] = {
    &RD_DIS_ADC1_CH7_ATTEN0_INITCODE_DIFF[0],    		// [] rd_dis of ADC1_CH7_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC2_CH0_ATTEN0_INITCODE_DIFF[] = {
    &RD_DIS_ADC2_CH0_ATTEN0_INITCODE_DIFF[0],    		// [] rd_dis of ADC2_CH0_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC2_CH1_ATTEN0_INITCODE_DIFF[] = {
    &RD_DIS_ADC2_CH1_ATTEN0_INITCODE_DIFF[0],    		// [] rd_dis of ADC2_CH1_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC2_CH2_ATTEN0_INITCODE_DIFF[] = {
    &RD_DIS_ADC2_CH2_ATTEN0_INITCODE_DIFF[0],    		// [] rd_dis of ADC2_CH2_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC2_CH3_ATTEN0_INITCODE_DIFF[] = {
    &RD_DIS_ADC2_CH3_ATTEN0_INITCODE_DIFF[0],    		// [] rd_dis of ADC2_CH3_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC2_CH4_ATTEN0_INITCODE_DIFF[] = {
    &RD_DIS_ADC2_CH4_ATTEN0_INITCODE_DIFF[0],    		// [] rd_dis of ADC2_CH4_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_ADC2_CH5_ATTEN0_INITCODE_DIFF[] = {
    &RD_DIS_ADC2_CH5_ATTEN0_INITCODE_DIFF[0],    		// [] rd_dis of ADC2_CH5_ATTEN0_INITCODE_DIFF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_TEMPERATURE_SENSOR[] = {
    &RD_DIS_TEMPERATURE_SENSOR[0],    		// [] rd_dis of TEMPERATURE_SENSOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USB_DEVICE_EXCHG_PINS[] = {
    &USB_DEVICE_EXCHG_PINS[0],    		// [] Enable usb device exchange pins of D+ and D-
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USB_OTG11_EXCHG_PINS[] = {
    &USB_OTG11_EXCHG_PINS[0],    		// [] Enable usb otg11 exchange pins of D+ and D-
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_JTAG[] = {
    &DIS_USB_JTAG[0],    		// [] Represents whether the function of usb switch to jtag is disabled or enabled. 1: disabled. 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_POWERGLITCH_EN[] = {
    &POWERGLITCH_EN[0],    		// [] Represents whether power glitch function is enabled. 1: enabled. 0: disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_FORCE_DOWNLOAD[] = {
    &DIS_FORCE_DOWNLOAD[0],    		// [] Represents whether the function that forces chip into download mode is disabled or enabled. 1: disabled. 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_DOWNLOAD_MSPI_DIS[] = {
    &SPI_DOWNLOAD_MSPI_DIS[0],    		// [] Set this bit to disable accessing MSPI flash/MSPI ram by SYS AXI matrix during boot_mode_download
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_TWAI[] = {
    &DIS_TWAI[0],    		// [] Represents whether TWAI function is disabled or enabled. 1: disabled. 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_JTAG_SEL_ENABLE[] = {
    &JTAG_SEL_ENABLE[0],    		// [] Represents whether the selection between usb_to_jtag and pad_to_jtag through strapping gpio15 when both EFUSE_DIS_PAD_JTAG and EFUSE_DIS_USB_JTAG are equal to 0 is enabled or disabled. 1: enabled. 0: disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SOFT_DIS_JTAG[] = {
    &SOFT_DIS_JTAG[0],    		// [] Represents whether JTAG is disabled in soft way. Odd number: disabled. Even number: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_PAD_JTAG[] = {
    &DIS_PAD_JTAG[0],    		// [] Represents whether JTAG is disabled in the hard way(permanently). 1: disabled. 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    &DIS_DOWNLOAD_MANUAL_ENCRYPT[0],    		// [] Represents whether flash encrypt function is disabled or enabled(except in SPI boot mode). 1: disabled. 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USB_PHY_SEL[] = {
    &USB_PHY_SEL[0],    		// [] TBD
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KM_HUK_GEN_STATE[] = {
    &KM_HUK_GEN_STATE[0],    		// [] Set this bit to control validation of HUK generate mode. Odd of 1 is invalid; even of 1 is valid
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KM_RND_SWITCH_CYCLE[] = {
    &KM_RND_SWITCH_CYCLE[0],    		// [] Set bits to control key manager random number switch cycle. 0: control by register. 1: 8 km clk cycles. 2: 16 km cycles. 3: 32 km cycles
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KM_DEPLOY_ONLY_ONCE[] = {
    &KM_DEPLOY_ONLY_ONCE[0],    		// [] Set each bit to control whether corresponding key can only be deployed once. 1 is true; 0 is false. Bit0: ecdsa. Bit1: xts. Bit2: hmac. Bit3: ds
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FORCE_USE_KEY_MANAGER_KEY[] = {
    &FORCE_USE_KEY_MANAGER_KEY[0],    		// [] Set each bit to control whether corresponding key must come from key manager.. 1 is true; 0 is false. Bit0: ecdsa. Bit1: xts. Bit2: hmac. Bit3: ds
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FORCE_DISABLE_SW_INIT_KEY[] = {
    &FORCE_DISABLE_SW_INIT_KEY[0],    		// [] Set this bit to disable software written init key; and force use efuse_init_key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_XTS_KEY_LENGTH_256[] = {
    &XTS_KEY_LENGTH_256[0],    		// [] Set this bit to configure flash encryption use xts-128 key; else use xts-256 key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WDT_DELAY_SEL[] = {
    &WDT_DELAY_SEL[0],    		// [] Represents whether RTC watchdog timeout threshold is selected at startup. 1: selected. 0: not selected
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

const esp_efuse_desc_t* ESP_EFUSE_ECDSA_ENABLE_SOFT_K[] = {
    &ECDSA_ENABLE_SOFT_K[0],    		// [] Represents whether hardware random number k is forced used in ESDCA. 1: force used. 0: not force used
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_CRYPT_DPA_ENABLE[] = {
    &CRYPT_DPA_ENABLE[0],    		// [] Represents whether anti-dpa attack is enabled. 1:enabled. 0: disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_EN[] = {
    &SECURE_BOOT_EN[0],    		// [] Represents whether secure boot is enabled or disabled. 1: enabled. 0: disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE[] = {
    &SECURE_BOOT_AGGRESSIVE_REVOKE[0],    		// [] Represents whether revoking aggressive secure boot is enabled or disabled. 1: enabled. 0: disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_TYPE[] = {
    &FLASH_TYPE[0],    		// [] The type of interfaced flash. 0: four data lines; 1: eight data lines
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_PAGE_SIZE[] = {
    &FLASH_PAGE_SIZE[0],    		// [] Set flash page size
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_ECC_EN[] = {
    &FLASH_ECC_EN[0],    		// [] Set this bit to enable ecc for flash boot
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_OTG_DOWNLOAD_MODE[] = {
    &DIS_USB_OTG_DOWNLOAD_MODE[0],    		// [] Set this bit to disable download via USB-OTG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_TPUW[] = {
    &FLASH_TPUW[0],    		// [] Represents the flash waiting time after power-up; in unit of ms. When the value less than 15; the waiting time is the programmed value. Otherwise; the waiting time is 2 times the programmed value
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MODE[] = {
    &DIS_DOWNLOAD_MODE[0],    		// [] Represents whether Download mode is disabled or enabled. 1: disabled. 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DIRECT_BOOT[] = {
    &DIS_DIRECT_BOOT[0],    		// [] Represents whether direct boot mode is disabled or enabled. 1: disabled. 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT[] = {
    &DIS_USB_SERIAL_JTAG_ROM_PRINT[0],    		// [] Represents whether print from USB-Serial-JTAG is disabled or enabled. 1: disabled. 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_LOCK_KM_KEY[] = {
    &LOCK_KM_KEY[0],    		// [] TBD
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[] = {
    &DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[0],    		// [] Represents whether the USB-Serial-JTAG download function is disabled or enabled. 1: disabled. 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ENABLE_SECURITY_DOWNLOAD[] = {
    &ENABLE_SECURITY_DOWNLOAD[0],    		// [] Represents whether security download is enabled or disabled. 1: enabled. 0: disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_UART_PRINT_CONTROL[] = {
    &UART_PRINT_CONTROL[0],    		// [] Represents the type of UART printing. 00: force enable printing. 01: enable printing when GPIO8 is reset at low level. 10: enable printing when GPIO8 is reset at high level. 11: force disable printing
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FORCE_SEND_RESUME[] = {
    &FORCE_SEND_RESUME[0],    		// [] Represents whether ROM code is forced to send a resume command during SPI boot. 1: forced. 0:not forced
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_VERSION[] = {
    &SECURE_VERSION[0],    		// [] Represents the version used by ESP-IDF anti-rollback feature
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE[] = {
    &SECURE_BOOT_DISABLE_FAST_WAKE[0],    		// [] Represents whether FAST VERIFY ON WAKE is disabled or enabled when Secure Boot is enabled. 1: disabled. 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_HYS_EN_PAD[] = {
    &HYS_EN_PAD[0],    		// [] Represents whether the hysteresis function of corresponding PAD is enabled. 1: enabled. 0:disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DCDC_VSET[] = {
    &DCDC_VSET[0],    		// [] Set the dcdc voltage default
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PXA0_TIEH_SEL_0[] = {
    &PXA0_TIEH_SEL_0[0],    		// [] TBD
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PXA0_TIEH_SEL_1[] = {
    &PXA0_TIEH_SEL_1[0],    		// [] TBD
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PXA0_TIEH_SEL_2[] = {
    &PXA0_TIEH_SEL_2[0],    		// [] TBD
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PXA0_TIEH_SEL_3[] = {
    &PXA0_TIEH_SEL_3[0],    		// [] TBD
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KM_DISABLE_DEPLOY_MODE[] = {
    &KM_DISABLE_DEPLOY_MODE[0],    		// [] TBD
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_HP_PWR_SRC_SEL[] = {
    &HP_PWR_SRC_SEL[0],    		// [] HP system power source select. 0:LDO. 1: DCDC
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DCDC_VSET_EN[] = {
    &DCDC_VSET_EN[0],    		// [] Select dcdc vset use efuse_dcdc_vset
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_WDT[] = {
    &DIS_WDT[0],    		// [] Set this bit to disable watch dog
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_SWD[] = {
    &DIS_SWD[0],    		// [] Set this bit to disable super-watchdog
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

const esp_efuse_desc_t* ESP_EFUSE_PSRAM_CAP[] = {
    &PSRAM_CAP[0],    		// [] PSRAM capacity
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_TEMP[] = {
    &TEMP[0],    		// [] Operating temperature of the ESP chip
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PSRAM_VENDOR[] = {
    &PSRAM_VENDOR[0],    		// [] PSRAM vendor
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PKG_VERSION[] = {
    &PKG_VERSION[0],    		// [] Package version
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_LDO_VO1_DREF[] = {
    &LDO_VO1_DREF[0],    		// [] Output VO1 parameter
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_LDO_VO2_DREF[] = {
    &LDO_VO2_DREF[0],    		// [] Output VO2 parameter
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_LDO_VO1_MUL[] = {
    &LDO_VO1_MUL[0],    		// [] Output VO1 parameter
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_LDO_VO2_MUL[] = {
    &LDO_VO2_MUL[0],    		// [] Output VO2 parameter
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_LDO_VO3_K[] = {
    &LDO_VO3_K[0],    		// [] Output VO3 calibration parameter
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_LDO_VO3_VOS[] = {
    &LDO_VO3_VOS[0],    		// [] Output VO3 calibration parameter
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_LDO_VO3_C[] = {
    &LDO_VO3_C[0],    		// [] Output VO3 calibration parameter
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_LDO_VO4_K[] = {
    &LDO_VO4_K[0],    		// [] Output VO4 calibration parameter
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_LDO_VO4_VOS[] = {
    &LDO_VO4_VOS[0],    		// [] Output VO4 calibration parameter
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_LDO_VO4_C[] = {
    &LDO_VO4_C[0],    		// [] Output VO4 calibration parameter
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

const esp_efuse_desc_t* ESP_EFUSE_LSLP_HP_DBIAS[] = {
    &LSLP_HP_DBIAS[0],    		// [] LSLP HP DBIAS of fixed voltage
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DSLP_DBG[] = {
    &DSLP_DBG[0],    		// [] DSLP BDG of fixed voltage
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DSLP_LP_DBIAS[] = {
    &DSLP_LP_DBIAS[0],    		// [] DSLP LP DBIAS of fixed voltage
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_LP_DCDC_DBIAS_VOL_GAP[] = {
    &LP_DCDC_DBIAS_VOL_GAP[0],    		// [] DBIAS gap between LP and DCDC
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_OPTIONAL_UNIQUE_ID[] = {
    &OPTIONAL_UNIQUE_ID[0],    		// [] Optional unique 128-bit ID
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_AVE_INITCODE_ATTEN0[] = {
    &ADC1_AVE_INITCODE_ATTEN0[0],    		// [] Average initcode of ADC1 atten0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_AVE_INITCODE_ATTEN1[] = {
    &ADC1_AVE_INITCODE_ATTEN1[0],    		// [] Average initcode of ADC1 atten1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_AVE_INITCODE_ATTEN2[] = {
    &ADC1_AVE_INITCODE_ATTEN2[0],    		// [] Average initcode of ADC1 atten2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_AVE_INITCODE_ATTEN3[] = {
    &ADC1_AVE_INITCODE_ATTEN3[0],    		// [] Average initcode of ADC1 atten3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_AVE_INITCODE_ATTEN0[] = {
    &ADC2_AVE_INITCODE_ATTEN0[0],    		// [] Average initcode of ADC2 atten0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_AVE_INITCODE_ATTEN1[] = {
    &ADC2_AVE_INITCODE_ATTEN1[0],    		// [] Average initcode of ADC2 atten1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_AVE_INITCODE_ATTEN2[] = {
    &ADC2_AVE_INITCODE_ATTEN2[0],    		// [] Average initcode of ADC2 atten2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_AVE_INITCODE_ATTEN3[] = {
    &ADC2_AVE_INITCODE_ATTEN3[0],    		// [] Average initcode of ADC2 atten3
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

const esp_efuse_desc_t* ESP_EFUSE_ADC2_HI_DOUT_ATTEN0[] = {
    &ADC2_HI_DOUT_ATTEN0[0],    		// [] HI_DOUT of ADC2 atten0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_HI_DOUT_ATTEN1[] = {
    &ADC2_HI_DOUT_ATTEN1[0],    		// [] HI_DOUT of ADC2 atten1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_HI_DOUT_ATTEN2[] = {
    &ADC2_HI_DOUT_ATTEN2[0],    		// [] HI_DOUT of ADC2 atten2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_HI_DOUT_ATTEN3[] = {
    &ADC2_HI_DOUT_ATTEN3[0],    		// [] HI_DOUT of ADC2 atten3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CH0_ATTEN0_INITCODE_DIFF[] = {
    &ADC1_CH0_ATTEN0_INITCODE_DIFF[0],    		// [] Gap between ADC1_ch0 and average initcode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CH1_ATTEN0_INITCODE_DIFF[] = {
    &ADC1_CH1_ATTEN0_INITCODE_DIFF[0],    		// [] Gap between ADC1_ch1 and average initcode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CH2_ATTEN0_INITCODE_DIFF[] = {
    &ADC1_CH2_ATTEN0_INITCODE_DIFF[0],    		// [] Gap between ADC1_ch2 and average initcode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CH3_ATTEN0_INITCODE_DIFF[] = {
    &ADC1_CH3_ATTEN0_INITCODE_DIFF[0],    		// [] Gap between ADC1_ch3 and average initcode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CH4_ATTEN0_INITCODE_DIFF[] = {
    &ADC1_CH4_ATTEN0_INITCODE_DIFF[0],    		// [] Gap between ADC1_ch4 and average initcode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CH5_ATTEN0_INITCODE_DIFF[] = {
    &ADC1_CH5_ATTEN0_INITCODE_DIFF[0],    		// [] Gap between ADC1_ch5 and average initcode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CH6_ATTEN0_INITCODE_DIFF[] = {
    &ADC1_CH6_ATTEN0_INITCODE_DIFF[0],    		// [] Gap between ADC1_ch6 and average initcode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CH7_ATTEN0_INITCODE_DIFF[] = {
    &ADC1_CH7_ATTEN0_INITCODE_DIFF[0],    		// [] Gap between ADC1_ch7 and average initcode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_CH0_ATTEN0_INITCODE_DIFF[] = {
    &ADC2_CH0_ATTEN0_INITCODE_DIFF[0],    		// [] Gap between ADC2_ch0 and average initcode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_CH1_ATTEN0_INITCODE_DIFF[] = {
    &ADC2_CH1_ATTEN0_INITCODE_DIFF[0],    		// [] Gap between ADC2_ch1 and average initcode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_CH2_ATTEN0_INITCODE_DIFF[] = {
    &ADC2_CH2_ATTEN0_INITCODE_DIFF[0],    		// [] Gap between ADC2_ch2 and average initcode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_CH3_ATTEN0_INITCODE_DIFF[] = {
    &ADC2_CH3_ATTEN0_INITCODE_DIFF[0],    		// [] Gap between ADC2_ch3 and average initcode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_CH4_ATTEN0_INITCODE_DIFF[] = {
    &ADC2_CH4_ATTEN0_INITCODE_DIFF[0],    		// [] Gap between ADC2_ch4 and average initcode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_CH5_ATTEN0_INITCODE_DIFF[] = {
    &ADC2_CH5_ATTEN0_INITCODE_DIFF[0],    		// [] Gap between ADC2_ch5 and average initcode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_TEMPERATURE_SENSOR[] = {
    &TEMPERATURE_SENSOR[0],    		// [] Temperature calibration data
    NULL
};
