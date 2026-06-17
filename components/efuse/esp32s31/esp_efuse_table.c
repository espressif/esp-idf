/*
 * SPDX-FileCopyrightText: 2017-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_efuse.h"
#include <assert.h>
#include "esp_efuse_table.h"

// md5_digest_table 1cdb8731d5e8193ceb48139f9db95994
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

static const esp_efuse_desc_t WR_DIS_KM_DISABLE_DEPLOY_MODE[] = {
    {EFUSE_BLK0, 1, 1}, 	 // [] wr_dis of KM_DISABLE_DEPLOY_MODE,
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

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_DISABLE_FAST_WAKE[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of SECURE_BOOT_DISABLE_FAST_WAKE,
};

static const esp_efuse_desc_t WR_DIS_DIS_WDT[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of DIS_WDT,
};

static const esp_efuse_desc_t WR_DIS_DIS_SWD[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of DIS_SWD,
};

static const esp_efuse_desc_t WR_DIS_PMU_FLASH_POWER_SEL[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of PMU_FLASH_POWER_SEL,
};

static const esp_efuse_desc_t WR_DIS_PMU_FLASH_POWER_SEL_EN[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of PMU_FLASH_POWER_SEL_EN,
};

static const esp_efuse_desc_t WR_DIS_POWER_GLITCH_EN[] = {
    {EFUSE_BLK0, 2, 1}, 	 // [] wr_dis of POWER_GLITCH_EN,
};

static const esp_efuse_desc_t WR_DIS_PVT_0_GLITCH_EN[] = {
    {EFUSE_BLK0, 3, 1}, 	 // [] wr_dis of PVT_0_GLITCH_EN,
};

static const esp_efuse_desc_t WR_DIS_PVT_0_GLITCH_MODE[] = {
    {EFUSE_BLK0, 3, 1}, 	 // [] wr_dis of PVT_0_GLITCH_MODE,
};

static const esp_efuse_desc_t WR_DIS_PVT_1_GLITCH_EN[] = {
    {EFUSE_BLK0, 3, 1}, 	 // [] wr_dis of PVT_1_GLITCH_EN,
};

static const esp_efuse_desc_t WR_DIS_PVT_1_GLITCH_MODE[] = {
    {EFUSE_BLK0, 3, 1}, 	 // [] wr_dis of PVT_1_GLITCH_MODE,
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

static const esp_efuse_desc_t WR_DIS_DIS_SM_CRYPT[] = {
    {EFUSE_BLK0, 14, 1}, 	 // [] wr_dis of DIS_SM_CRYPT,
};

static const esp_efuse_desc_t WR_DIS_ECC_FORCE_CONST_TIME[] = {
    {EFUSE_BLK0, 14, 1}, 	 // [] wr_dis of ECC_FORCE_CONST_TIME,
};

static const esp_efuse_desc_t WR_DIS_ECDSA_DISABLE_SOFT_K[] = {
    {EFUSE_BLK0, 14, 1}, 	 // [] wr_dis of ECDSA_DISABLE_SOFT_K,
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

static const esp_efuse_desc_t WR_DIS_RMA_NONCE_ENA[] = {
    {EFUSE_BLK0, 14, 1}, 	 // [] wr_dis of RMA_NONCE_ENA,
};

static const esp_efuse_desc_t WR_DIS_RMA_CHIP_INFO_SOURCE[] = {
    {EFUSE_BLK0, 14, 1}, 	 // [] wr_dis of RMA_CHIP_INFO_SOURCE,
};

static const esp_efuse_desc_t WR_DIS_RMA_DISABLE_FAST_VEF[] = {
    {EFUSE_BLK0, 14, 1}, 	 // [] wr_dis of RMA_DISABLE_FAST_VEF,
};

static const esp_efuse_desc_t WR_DIS_ENA_XTS_SHADOW[] = {
    {EFUSE_BLK0, 14, 1}, 	 // [] wr_dis of ENA_XTS_SHADOW,
};

static const esp_efuse_desc_t WR_DIS_ENA_SPI_BOOT_CRYPT_SCRAMBLER[] = {
    {EFUSE_BLK0, 14, 1}, 	 // [] wr_dis of ENA_SPI_BOOT_CRYPT_SCRAMBLER,
};

static const esp_efuse_desc_t WR_DIS_RE_ENABLE_JTAG_SOURCE[] = {
    {EFUSE_BLK0, 14, 1}, 	 // [] wr_dis of RE_ENABLE_JTAG_SOURCE,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_EN[] = {
    {EFUSE_BLK0, 15, 1}, 	 // [] wr_dis of SECURE_BOOT_EN,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_AGGRESSIVE_REVOKE[] = {
    {EFUSE_BLK0, 16, 1}, 	 // [] wr_dis of SECURE_BOOT_AGGRESSIVE_REVOKE,
};

static const esp_efuse_desc_t WR_DIS_FLASH_TYPE[] = {
    {EFUSE_BLK0, 18, 1}, 	 // [] wr_dis of FLASH_TYPE,
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

static const esp_efuse_desc_t WR_DIS_OPTIONAL_UNIQUE_ID[] = {
    {EFUSE_BLK0, 21, 1}, 	 // [] wr_dis of OPTIONAL_UNIQUE_ID,
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

static const esp_efuse_desc_t WR_DIS_BLOCK_SYS_DATA2[] = {
    {EFUSE_BLK0, 28, 1}, 	 // [WR_DIS.SYS_DATA_PART2] wr_dis of BLOCK_SYS_DATA2,
};

static const esp_efuse_desc_t WR_DIS_USB_DEVICE_EXCHG_PINS[] = {
    {EFUSE_BLK0, 28, 1}, 	 // [] wr_dis of USB_DEVICE_EXCHG_PINS,
};

static const esp_efuse_desc_t WR_DIS_SOFT_DIS_JTAG[] = {
    {EFUSE_BLK0, 31, 1}, 	 // [] wr_dis of SOFT_DIS_JTAG,
};

static const esp_efuse_desc_t RD_DIS[] = {
    {EFUSE_BLK0, 32, 7}, 	 // [] Disable reading from BlOCK4-9,
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

static const esp_efuse_desc_t DIS_USB_JTAG[] = {
    {EFUSE_BLK0, 42, 1}, 	 // [] Represents whether the function of usb switch to jtag is disabled or enabled. 1: disabled 0: enabled,
};

static const esp_efuse_desc_t DIS_FORCE_DOWNLOAD[] = {
    {EFUSE_BLK0, 44, 1}, 	 // [] Represents whether the function that forces chip into download mode is disabled or enabled. 1: disabled 0: enabled,
};

static const esp_efuse_desc_t SPI_DOWNLOAD_MSPI_DIS[] = {
    {EFUSE_BLK0, 45, 1}, 	 // [] Represents whether SPI0 controller during boot_mode_download is disabled or enabled. 1: disabled 0: enabled,
};

static const esp_efuse_desc_t DIS_TWAI[] = {
    {EFUSE_BLK0, 46, 1}, 	 // [] Represents whether TWAI function is disabled or enabled. 1: disabled 0: enabled,
};

static const esp_efuse_desc_t JTAG_SEL_ENABLE[] = {
    {EFUSE_BLK0, 47, 1}, 	 // [] Represents whether the selection between usb_to_jtag and pad_to_jtag through strapping gpio15 when both EFUSE_DIS_PAD_JTAG and EFUSE_DIS_USB_JTAG are equal to 0 is enabled or disabled. 1: enabled 0: disabled,
};

static const esp_efuse_desc_t SOFT_DIS_JTAG[] = {
    {EFUSE_BLK0, 48, 3}, 	 // [] Represents whether JTAG is disabled in soft way. Odd number: disabled Even number: enabled,
};

static const esp_efuse_desc_t DIS_PAD_JTAG[] = {
    {EFUSE_BLK0, 51, 1}, 	 // [] Represents whether JTAG is disabled in the hard way(permanently). 1: disabled 0: enabled,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    {EFUSE_BLK0, 52, 1}, 	 // [] Represents whether flash encrypt function is disabled or enabled(except in SPI boot mode). 1: disabled 0: enabled,
};

static const esp_efuse_desc_t HUK_GEN_STATE[] = {
    {EFUSE_BLK0, 55, 5}, 	 // [] Represents the control of validation of HUK generate mode. Odd of 1 is invalid; even of 1 is valid,
};

static const esp_efuse_desc_t KM_RND_SWITCH_CYCLE[] = {
    {EFUSE_BLK0, 64, 1}, 	 // [] Represents the control of key manager random number switch cycle. 0: control by register. 1: 8 km clk cycles. 2: 16 km cycles. 3: 32 km cycles,
};

static const esp_efuse_desc_t KM_DISABLE_DEPLOY_MODE[] = {
    {EFUSE_BLK0, 66, 5}, 	 // [] Represents whether the deploy mode of key manager is disable or not.  1: disabled  0: enabled. bit 0: ecsda; bit 1: flash & spi boot srambler; bit2: hmac & aes; bit3: ds & rma nonce; bit4: psram,
};

static const esp_efuse_desc_t KM_DEPLOY_ONLY_ONCE[] = {
    {EFUSE_BLK0, 71, 5}, 	 // [] Represents whether corresponding key can only be deployed once. 1 is true; 0 is false.  0: ecsda 1: flash & spi boot srambler 2: hmac & aes 3: ds & rma nonce 4: psram,
};

static const esp_efuse_desc_t FORCE_USE_KEY_MANAGER_KEY[] = {
    {EFUSE_BLK0, 76, 5}, 	 // [] Represents whether corresponding key must come from key manager. 1 is true; 0 is false. 0: ecsda 1: flash 2: reserved 3: reserved 4: psram,
};

static const esp_efuse_desc_t FORCE_DISABLE_SW_INIT_KEY[] = {
    {EFUSE_BLK0, 81, 1}, 	 // [] Represents whether to disable software written init key; and force use efuse_init_key,
};

static const esp_efuse_desc_t KM_XTS_KEY_LENGTH_256[] = {
    {EFUSE_BLK0, 82, 1}, 	 // [] Represents whether to configure flash encryption use xts-128 key. else use xts-256 key.  0: 128-bit key  1: 256-bit key,
};

static const esp_efuse_desc_t WDT_DELAY_SEL[] = {
    {EFUSE_BLK0, 83, 1}, 	 // [] Represents the threshold level of the RTC watchdog STG0 timeout.0: Original threshold configuration value of STG0 *2 1: Original threshold configuration value of STG0 *4 2: Original threshold configuration value of STG0 *8 3: Original threshold configuration value of STG0 *16,
};

static const esp_efuse_desc_t DIS_SM_CRYPT[] = {
    {EFUSE_BLK0, 84, 1}, 	 // [] Represents whether to disable all the SM crypto functions; including SM2; SM3. 1: disabled 0: enabled,
};

static const esp_efuse_desc_t SPI_BOOT_CRYPT_CNT[] = {
    {EFUSE_BLK0, 85, 3}, 	 // [] Enables flash encryption when 1 or 3 bits are set and disables otherwise {0: "Disable"; 1: "Enable"; 3: "Disable"; 7: "Enable"},
};

static const esp_efuse_desc_t SECURE_BOOT_KEY_REVOKE0[] = {
    {EFUSE_BLK0, 88, 1}, 	 // [] Revoke 1st secure boot key,
};

static const esp_efuse_desc_t SECURE_BOOT_KEY_REVOKE1[] = {
    {EFUSE_BLK0, 89, 1}, 	 // [] Revoke 2nd secure boot key,
};

static const esp_efuse_desc_t SECURE_BOOT_KEY_REVOKE2[] = {
    {EFUSE_BLK0, 90, 1}, 	 // [] Revoke 3rd secure boot key,
};

static const esp_efuse_desc_t KEY_PURPOSE_0[] = {
    {EFUSE_BLK0, 96, 5}, 	 // [KEY0_PURPOSE] Represents the purpose of Key0,
};

static const esp_efuse_desc_t KEY_PURPOSE_1[] = {
    {EFUSE_BLK0, 101, 5}, 	 // [KEY1_PURPOSE] Represents the purpose of Key1,
};

static const esp_efuse_desc_t KEY_PURPOSE_2[] = {
    {EFUSE_BLK0, 106, 5}, 	 // [KEY2_PURPOSE] Represents the purpose of Key2,
};

static const esp_efuse_desc_t KEY_PURPOSE_3[] = {
    {EFUSE_BLK0, 111, 5}, 	 // [KEY3_PURPOSE] Represents the purpose of Key3,
};

static const esp_efuse_desc_t KEY_PURPOSE_4[] = {
    {EFUSE_BLK0, 116, 5}, 	 // [KEY4_PURPOSE] Represents the purpose of Key4,
};

static const esp_efuse_desc_t ECC_FORCE_CONST_TIME[] = {
    {EFUSE_BLK0, 121, 1}, 	 // [] Represents whether permanently turn on ECC const-time mode.  1: turn on 0: turn off,
};

static const esp_efuse_desc_t ECDSA_DISABLE_SOFT_K[] = {
    {EFUSE_BLK0, 122, 1}, 	 // [] Represents whether permanently turn off ECDSA software set KEY. 1: turn off 0: turn on,
};

static const esp_efuse_desc_t SEC_DPA_LEVEL[] = {
    {EFUSE_BLK0, 123, 2}, 	 // [] Represents the spa secure level by configuring the clock random divide mode,
};

static const esp_efuse_desc_t XTS_DPA_CLK_ENABLE[] = {
    {EFUSE_BLK0, 125, 1}, 	 // [] Represents whether to enable xts clock anti-dpa attack function.0: Disabled. 1: Enabled,
};

static const esp_efuse_desc_t XTS_DPA_PSEUDO_LEVEL[] = {
    {EFUSE_BLK0, 128, 2}, 	 // [] Represents the control of the xts pseudo-round anti-dpa attack function. 0: controlled by register. 1-3: the higher the value is; the more pseudo-rounds are inserted to the xts-aes calculation,
};

static const esp_efuse_desc_t SECURE_BOOT_EN[] = {
    {EFUSE_BLK0, 130, 1}, 	 // [] Represents whether secure boot is enabled or disabled. 1: enabled 0: disabled,
};

static const esp_efuse_desc_t SECURE_BOOT_AGGRESSIVE_REVOKE[] = {
    {EFUSE_BLK0, 131, 1}, 	 // [] Represents whether revoking aggressive secure boot is enabled or disabled. 1: enabled. 0: disabled,
};

static const esp_efuse_desc_t FLASH_TYPE[] = {
    {EFUSE_BLK0, 133, 1}, 	 // [] flash type: 0: nor flash; 1: nand flash,
};

static const esp_efuse_desc_t DIS_USB_OTG_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 137, 1}, 	 // [] Set this bit to disable download via USB-OTG,
};

static const esp_efuse_desc_t FLASH_TPUW[] = {
    {EFUSE_BLK0, 140, 4}, 	 // [] Represents the flash waiting time after power-up; in unit of ms. When the value less than 15; the waiting time is the programmed value. Otherwise; the waiting time is 2 times the programmed value,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 144, 1}, 	 // [] Represents whether Download mode is disabled or enabled. 1: disabled 0: enabled,
};

static const esp_efuse_desc_t DIS_DIRECT_BOOT[] = {
    {EFUSE_BLK0, 145, 1}, 	 // [] Represents whether direct boot mode is disabled or enabled. 1: disabled 0: enabled,
};

static const esp_efuse_desc_t DIS_USB_SERIAL_JTAG_ROM_PRINT[] = {
    {EFUSE_BLK0, 146, 1}, 	 // [] Represents whether print from USB-Serial-JTAG is disabled or enabled. 1: disabled 0: enabled,
};

static const esp_efuse_desc_t LOCK_KM_KEY[] = {
    {EFUSE_BLK0, 147, 1}, 	 // [] Represetns whether to lock the efuse xts key. 1. Lock 0: Unlock,
};

static const esp_efuse_desc_t DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 148, 1}, 	 // [] Represents whether the USB-Serial-JTAG download function is disabled or enabled. 1: Disable 0: Enable,
};

static const esp_efuse_desc_t ENABLE_SECURITY_DOWNLOAD[] = {
    {EFUSE_BLK0, 149, 1}, 	 // [] Represents whether security download is enabled or disabled. 1: enabled 0: disabled,
};

static const esp_efuse_desc_t UART_PRINT_CONTROL[] = {
    {EFUSE_BLK0, 150, 2}, 	 // [] Represents the type of UART printing. 00: force enable printing 01: enable printing when GPIO8 is reset at low level 10: enable printing when GPIO8 is reset at high level 11: force disable printing,
};

static const esp_efuse_desc_t FORCE_SEND_RESUME[] = {
    {EFUSE_BLK0, 152, 1}, 	 // [] Represents whether ROM code is forced to send a resume command during SPI boot. 1: forced 0:not forced,
};

static const esp_efuse_desc_t SECURE_VERSION[] = {
    {EFUSE_BLK0, 160, 16}, 	 // [] Represents the version used by ESP-IDF anti-rollback feature,
};

static const esp_efuse_desc_t SECURE_BOOT_DISABLE_FAST_WAKE[] = {
    {EFUSE_BLK0, 176, 1}, 	 // [] Represents whether FAST VERIFY ON WAKE is disabled or enabled when Secure Boot is enabled. 1: disabled 0: enabled,
};

static const esp_efuse_desc_t HYS_EN_PAD[] = {
    {EFUSE_BLK0, 177, 1}, 	 // [] Represents whether the hysteresis function of corresponding PAD is enabled. 1: enabled 0:disabled,
};

static const esp_efuse_desc_t DCDC_VSET_EN[] = {
    {EFUSE_BLK0, 194, 1}, 	 // [] Select dcdc vset use efuse_dcdc_vset,
};

static const esp_efuse_desc_t DIS_WDT[] = {
    {EFUSE_BLK0, 195, 1}, 	 // [] Set this bit to disable watch dog,
};

static const esp_efuse_desc_t DIS_SWD[] = {
    {EFUSE_BLK0, 196, 1}, 	 // [] Set this bit to disable super-watchdog,
};

static const esp_efuse_desc_t SECURE_BOOT_SHA384_EN[] = {
    {EFUSE_BLK0, 203, 1}, 	 // [] Represents whether secure boot using SHA-384 is enabled. 0: Disable 1: Enable,
};

static const esp_efuse_desc_t BOOTLOADER_ANTI_ROLLBACK_SECURE_VERSION[] = {
    {EFUSE_BLK0, 204, 4}, 	 // [] Represents the anti-rollback secure version of the 2nd stage bootloader used by the ROM bootloader,
};

static const esp_efuse_desc_t BOOTLOADER_ANTI_ROLLBACK_EN[] = {
    {EFUSE_BLK0, 208, 1}, 	 // [] Represents whether the ani-rollback check for the 2nd stage bootloader is enabled.1: Enabled0: Disabled,
};

static const esp_efuse_desc_t BOOTLOADER_ANTI_ROLLBACK_UPDATE_IN_ROM[] = {
    {EFUSE_BLK0, 209, 1}, 	 // [] Represents whether the ani-rollback SECURE_VERSION will be updated from the ROM bootloader.1: Enable0: Disable,
};

static const esp_efuse_desc_t RECOVERY_BOOTLOADER_FLASH_SECTOR[] = {
    {EFUSE_BLK0, 210, 12}, 	 // [] Represents the starting flash sector (flash sector size is 0x1000) of the recovery bootloader used by the ROM bootloader If the primary bootloader fails. 0 and 0xFFF - this feature is disabled,
};

static const esp_efuse_desc_t RMA_ENA[] = {
    {EFUSE_BLK0, 222, 2}, 	 // [] Represents whether rma function is supported in download mode. 2'b01/2'b10: enabled2'b00/2'b11: disabled,
};

static const esp_efuse_desc_t RMA_SESSION_COUNTER[] = {
    {EFUSE_BLK0, 224, 3}, 	 // [] Represents the number of times the RMA session has been entered,
};

static const esp_efuse_desc_t RMA_NONCE_ENA[] = {
    {EFUSE_BLK0, 227, 2}, 	 // [] Represents whether random number NONCE is used in RMA and whether the KM module is used to generate the NONCE. 2'bx0: No NONCE 2'b1x: Use KM generate NONCE.,
};

static const esp_efuse_desc_t RMA_CHIP_INFO_SOURCE[] = {
    {EFUSE_BLK0, 229, 1}, 	 // [] Represents whether HUK_info is selected as the source for calculating CHIP_info in RMA.1: use HUK_info 0: use UNIQ_id,
};

static const esp_efuse_desc_t RMA_DISABLE_FAST_VEF[] = {
    {EFUSE_BLK0, 230, 1}, 	 // [] Represents whether disable FAST_VEF in RMA session.1: disable0: enable,
};

static const esp_efuse_desc_t PVT_0_GLITCH_EN[] = {
    {EFUSE_BLK0, 231, 1}, 	 // [] Represents whether to enable PVT power glitch monitor function.1:Enable. 0:Disable,
};

static const esp_efuse_desc_t PVT_0_GLITCH_MODE[] = {
    {EFUSE_BLK0, 232, 2}, 	 // [] Use to configure glitch mode,
};

static const esp_efuse_desc_t PVT_1_GLITCH_EN[] = {
    {EFUSE_BLK0, 234, 1}, 	 // [] Represents whether to enable PVT power glitch monitor function.1:Enable. 0:Disable,
};

static const esp_efuse_desc_t PVT_1_GLITCH_MODE[] = {
    {EFUSE_BLK0, 235, 2}, 	 // [] Use to configure glitch mode,
};

static const esp_efuse_desc_t PMU_FLASH_POWER_SEL[] = {
    {EFUSE_BLK0, 237, 1}, 	 // [] FLASH power select. 1'b1: use 3.3V1'b0: use 1.8V,
};

static const esp_efuse_desc_t PMU_FLASH_POWER_SEL_EN[] = {
    {EFUSE_BLK0, 238, 1}, 	 // [] FLASH power select enable signal. 1'b1 : validates EFUSE_PMU_FLASH_POWER_SEL 1'b0: invalidates EFUSE_PMU_FLASH_POWER_SEL,
};

static const esp_efuse_desc_t POWER_GLITCH_EN[] = {
    {EFUSE_BLK0, 239, 4}, 	 // [] set these bit enable power glitch enable,
};

static const esp_efuse_desc_t ENA_XTS_SHADOW[] = {
    {EFUSE_BLK0, 243, 1}, 	 // [] Represents whether to enable XTS-AES shadow core countermeasure against fault injection attacks.  0: Disabled 1: Enabled,
};

static const esp_efuse_desc_t ENA_SPI_BOOT_CRYPT_SCRAMBLER[] = {
    {EFUSE_BLK0, 244, 1}, 	 // [] Represents whether to enable ciphertext scrambler for external memory .  0: Disabled 1: Enabled,
};

static const esp_efuse_desc_t RE_ENABLE_JTAG_SOURCE[] = {
    {EFUSE_BLK0, 245, 1}, 	 // [] Represents which Crypto peripheral is selected for re-enabling JTAG.  0: RMA 1: HMAC,
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
    {EFUSE_BLK1, 56, 8}, 	 // [] Stores the extended bits of MAC address,
    {EFUSE_BLK1, 48, 8}, 	 // [] Stores the extended bits of MAC address,
};

static const esp_efuse_desc_t WAFER_VERSION_MINOR[] = {
    {EFUSE_BLK1, 114, 4}, 	 // [] Minor chip version,
};

static const esp_efuse_desc_t WAFER_VERSION_MAJOR[] = {
    {EFUSE_BLK1, 118, 2}, 	 // [] Major chip version,
};

static const esp_efuse_desc_t DISABLE_WAFER_VERSION_MAJOR[] = {
    {EFUSE_BLK1, 120, 1}, 	 // [] Disables check of wafer version major,
};

static const esp_efuse_desc_t DISABLE_BLK_VERSION_MAJOR[] = {
    {EFUSE_BLK1, 121, 1}, 	 // [] Disables check of blk version major,
};

static const esp_efuse_desc_t BLK_VERSION_MINOR[] = {
    {EFUSE_BLK1, 122, 3}, 	 // [] BLK_VERSION_MINOR of BLOCK2,
};

static const esp_efuse_desc_t BLK_VERSION_MAJOR[] = {
    {EFUSE_BLK1, 125, 2}, 	 // [] BLK_VERSION_MAJOR of BLOCK2,
};

static const esp_efuse_desc_t PSRAM_CAP[] = {
    {EFUSE_BLK1, 127, 3}, 	 // [] Psram capacity,
};

static const esp_efuse_desc_t TEMP[] = {
    {EFUSE_BLK1, 130, 2}, 	 // [] Maximum ambient temperature that ESP Chip can work properly,
};

static const esp_efuse_desc_t PSRAM_VENDOR[] = {
    {EFUSE_BLK1, 132, 2}, 	 // [] Psram vendor,
};

static const esp_efuse_desc_t PKG_VERSION[] = {
    {EFUSE_BLK1, 134, 2}, 	 // [] Package version,
};

static const esp_efuse_desc_t OPTIONAL_UNIQUE_ID[] = {
    {EFUSE_BLK2, 0, 128}, 	 // [] Optional unique 128-bit ID,
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

static const esp_efuse_desc_t SYS_DATA_PART2[] = {
    {EFUSE_BLK9, 0, 32}, 	 // [BLOCK_SYS_DATA2] System data part 2 (reserved),
};

static const esp_efuse_desc_t USB_DEVICE_EXCHG_PINS[] = {
    {EFUSE_BLK9, 192, 1}, 	 // [] Represents whether enable usb device exchange pins of D+ and D- or not.  1: enabled 0: disabled,
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KM_DISABLE_DEPLOY_MODE[] = {
    &WR_DIS_KM_DISABLE_DEPLOY_MODE[0],    		// [] wr_dis of KM_DISABLE_DEPLOY_MODE
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_DISABLE_FAST_WAKE[] = {
    &WR_DIS_SECURE_BOOT_DISABLE_FAST_WAKE[0],    		// [] wr_dis of SECURE_BOOT_DISABLE_FAST_WAKE
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PMU_FLASH_POWER_SEL[] = {
    &WR_DIS_PMU_FLASH_POWER_SEL[0],    		// [] wr_dis of PMU_FLASH_POWER_SEL
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PMU_FLASH_POWER_SEL_EN[] = {
    &WR_DIS_PMU_FLASH_POWER_SEL_EN[0],    		// [] wr_dis of PMU_FLASH_POWER_SEL_EN
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_POWER_GLITCH_EN[] = {
    &WR_DIS_POWER_GLITCH_EN[0],    		// [] wr_dis of POWER_GLITCH_EN
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PVT_0_GLITCH_EN[] = {
    &WR_DIS_PVT_0_GLITCH_EN[0],    		// [] wr_dis of PVT_0_GLITCH_EN
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PVT_0_GLITCH_MODE[] = {
    &WR_DIS_PVT_0_GLITCH_MODE[0],    		// [] wr_dis of PVT_0_GLITCH_MODE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PVT_1_GLITCH_EN[] = {
    &WR_DIS_PVT_1_GLITCH_EN[0],    		// [] wr_dis of PVT_1_GLITCH_EN
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PVT_1_GLITCH_MODE[] = {
    &WR_DIS_PVT_1_GLITCH_MODE[0],    		// [] wr_dis of PVT_1_GLITCH_MODE
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_SM_CRYPT[] = {
    &WR_DIS_DIS_SM_CRYPT[0],    		// [] wr_dis of DIS_SM_CRYPT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ECC_FORCE_CONST_TIME[] = {
    &WR_DIS_ECC_FORCE_CONST_TIME[0],    		// [] wr_dis of ECC_FORCE_CONST_TIME
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ECDSA_DISABLE_SOFT_K[] = {
    &WR_DIS_ECDSA_DISABLE_SOFT_K[0],    		// [] wr_dis of ECDSA_DISABLE_SOFT_K
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RMA_NONCE_ENA[] = {
    &WR_DIS_RMA_NONCE_ENA[0],    		// [] wr_dis of RMA_NONCE_ENA
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RMA_CHIP_INFO_SOURCE[] = {
    &WR_DIS_RMA_CHIP_INFO_SOURCE[0],    		// [] wr_dis of RMA_CHIP_INFO_SOURCE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RMA_DISABLE_FAST_VEF[] = {
    &WR_DIS_RMA_DISABLE_FAST_VEF[0],    		// [] wr_dis of RMA_DISABLE_FAST_VEF
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ENA_XTS_SHADOW[] = {
    &WR_DIS_ENA_XTS_SHADOW[0],    		// [] wr_dis of ENA_XTS_SHADOW
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ENA_SPI_BOOT_CRYPT_SCRAMBLER[] = {
    &WR_DIS_ENA_SPI_BOOT_CRYPT_SCRAMBLER[0],    		// [] wr_dis of ENA_SPI_BOOT_CRYPT_SCRAMBLER
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RE_ENABLE_JTAG_SOURCE[] = {
    &WR_DIS_RE_ENABLE_JTAG_SOURCE[0],    		// [] wr_dis of RE_ENABLE_JTAG_SOURCE
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_TYPE[] = {
    &WR_DIS_FLASH_TYPE[0],    		// [] wr_dis of FLASH_TYPE
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_OPTIONAL_UNIQUE_ID[] = {
    &WR_DIS_OPTIONAL_UNIQUE_ID[0],    		// [] wr_dis of OPTIONAL_UNIQUE_ID
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

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_SYS_DATA2[] = {
    &WR_DIS_BLOCK_SYS_DATA2[0],    		// [WR_DIS.SYS_DATA_PART2] wr_dis of BLOCK_SYS_DATA2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_USB_DEVICE_EXCHG_PINS[] = {
    &WR_DIS_USB_DEVICE_EXCHG_PINS[0],    		// [] wr_dis of USB_DEVICE_EXCHG_PINS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SOFT_DIS_JTAG[] = {
    &WR_DIS_SOFT_DIS_JTAG[0],    		// [] wr_dis of SOFT_DIS_JTAG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS[] = {
    &RD_DIS[0],    		// [] Disable reading from BlOCK4-9
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

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_JTAG[] = {
    &DIS_USB_JTAG[0],    		// [] Represents whether the function of usb switch to jtag is disabled or enabled. 1: disabled 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_FORCE_DOWNLOAD[] = {
    &DIS_FORCE_DOWNLOAD[0],    		// [] Represents whether the function that forces chip into download mode is disabled or enabled. 1: disabled 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_DOWNLOAD_MSPI_DIS[] = {
    &SPI_DOWNLOAD_MSPI_DIS[0],    		// [] Represents whether SPI0 controller during boot_mode_download is disabled or enabled. 1: disabled 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_TWAI[] = {
    &DIS_TWAI[0],    		// [] Represents whether TWAI function is disabled or enabled. 1: disabled 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_JTAG_SEL_ENABLE[] = {
    &JTAG_SEL_ENABLE[0],    		// [] Represents whether the selection between usb_to_jtag and pad_to_jtag through strapping gpio15 when both EFUSE_DIS_PAD_JTAG and EFUSE_DIS_USB_JTAG are equal to 0 is enabled or disabled. 1: enabled 0: disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SOFT_DIS_JTAG[] = {
    &SOFT_DIS_JTAG[0],    		// [] Represents whether JTAG is disabled in soft way. Odd number: disabled Even number: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_PAD_JTAG[] = {
    &DIS_PAD_JTAG[0],    		// [] Represents whether JTAG is disabled in the hard way(permanently). 1: disabled 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    &DIS_DOWNLOAD_MANUAL_ENCRYPT[0],    		// [] Represents whether flash encrypt function is disabled or enabled(except in SPI boot mode). 1: disabled 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_HUK_GEN_STATE[] = {
    &HUK_GEN_STATE[0],    		// [] Represents the control of validation of HUK generate mode. Odd of 1 is invalid; even of 1 is valid
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KM_RND_SWITCH_CYCLE[] = {
    &KM_RND_SWITCH_CYCLE[0],    		// [] Represents the control of key manager random number switch cycle. 0: control by register. 1: 8 km clk cycles. 2: 16 km cycles. 3: 32 km cycles
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KM_DISABLE_DEPLOY_MODE[] = {
    &KM_DISABLE_DEPLOY_MODE[0],    		// [] Represents whether the deploy mode of key manager is disable or not.  1: disabled  0: enabled. bit 0: ecsda; bit 1: flash & spi boot srambler; bit2: hmac & aes; bit3: ds & rma nonce; bit4: psram
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KM_DEPLOY_ONLY_ONCE[] = {
    &KM_DEPLOY_ONLY_ONCE[0],    		// [] Represents whether corresponding key can only be deployed once. 1 is true; 0 is false.  0: ecsda 1: flash & spi boot srambler 2: hmac & aes 3: ds & rma nonce 4: psram
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FORCE_USE_KEY_MANAGER_KEY[] = {
    &FORCE_USE_KEY_MANAGER_KEY[0],    		// [] Represents whether corresponding key must come from key manager. 1 is true; 0 is false. 0: ecsda 1: flash 2: reserved 3: reserved 4: psram
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FORCE_DISABLE_SW_INIT_KEY[] = {
    &FORCE_DISABLE_SW_INIT_KEY[0],    		// [] Represents whether to disable software written init key; and force use efuse_init_key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KM_XTS_KEY_LENGTH_256[] = {
    &KM_XTS_KEY_LENGTH_256[0],    		// [] Represents whether to configure flash encryption use xts-128 key. else use xts-256 key.  0: 128-bit key  1: 256-bit key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WDT_DELAY_SEL[] = {
    &WDT_DELAY_SEL[0],    		// [] Represents the threshold level of the RTC watchdog STG0 timeout.0: Original threshold configuration value of STG0 *2 1: Original threshold configuration value of STG0 *4 2: Original threshold configuration value of STG0 *8 3: Original threshold configuration value of STG0 *16
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_SM_CRYPT[] = {
    &DIS_SM_CRYPT[0],    		// [] Represents whether to disable all the SM crypto functions; including SM2; SM3. 1: disabled 0: enabled
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

const esp_efuse_desc_t* ESP_EFUSE_ECC_FORCE_CONST_TIME[] = {
    &ECC_FORCE_CONST_TIME[0],    		// [] Represents whether permanently turn on ECC const-time mode.  1: turn on 0: turn off
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ECDSA_DISABLE_SOFT_K[] = {
    &ECDSA_DISABLE_SOFT_K[0],    		// [] Represents whether permanently turn off ECDSA software set KEY. 1: turn off 0: turn on
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SEC_DPA_LEVEL[] = {
    &SEC_DPA_LEVEL[0],    		// [] Represents the spa secure level by configuring the clock random divide mode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_XTS_DPA_CLK_ENABLE[] = {
    &XTS_DPA_CLK_ENABLE[0],    		// [] Represents whether to enable xts clock anti-dpa attack function.0: Disabled. 1: Enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_XTS_DPA_PSEUDO_LEVEL[] = {
    &XTS_DPA_PSEUDO_LEVEL[0],    		// [] Represents the control of the xts pseudo-round anti-dpa attack function. 0: controlled by register. 1-3: the higher the value is; the more pseudo-rounds are inserted to the xts-aes calculation
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_EN[] = {
    &SECURE_BOOT_EN[0],    		// [] Represents whether secure boot is enabled or disabled. 1: enabled 0: disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE[] = {
    &SECURE_BOOT_AGGRESSIVE_REVOKE[0],    		// [] Represents whether revoking aggressive secure boot is enabled or disabled. 1: enabled. 0: disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_TYPE[] = {
    &FLASH_TYPE[0],    		// [] flash type: 0: nor flash; 1: nand flash
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
    &DIS_DOWNLOAD_MODE[0],    		// [] Represents whether Download mode is disabled or enabled. 1: disabled 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DIRECT_BOOT[] = {
    &DIS_DIRECT_BOOT[0],    		// [] Represents whether direct boot mode is disabled or enabled. 1: disabled 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT[] = {
    &DIS_USB_SERIAL_JTAG_ROM_PRINT[0],    		// [] Represents whether print from USB-Serial-JTAG is disabled or enabled. 1: disabled 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_LOCK_KM_KEY[] = {
    &LOCK_KM_KEY[0],    		// [] Represetns whether to lock the efuse xts key. 1. Lock 0: Unlock
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[] = {
    &DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[0],    		// [] Represents whether the USB-Serial-JTAG download function is disabled or enabled. 1: Disable 0: Enable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ENABLE_SECURITY_DOWNLOAD[] = {
    &ENABLE_SECURITY_DOWNLOAD[0],    		// [] Represents whether security download is enabled or disabled. 1: enabled 0: disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_UART_PRINT_CONTROL[] = {
    &UART_PRINT_CONTROL[0],    		// [] Represents the type of UART printing. 00: force enable printing 01: enable printing when GPIO8 is reset at low level 10: enable printing when GPIO8 is reset at high level 11: force disable printing
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FORCE_SEND_RESUME[] = {
    &FORCE_SEND_RESUME[0],    		// [] Represents whether ROM code is forced to send a resume command during SPI boot. 1: forced 0:not forced
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_VERSION[] = {
    &SECURE_VERSION[0],    		// [] Represents the version used by ESP-IDF anti-rollback feature
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE[] = {
    &SECURE_BOOT_DISABLE_FAST_WAKE[0],    		// [] Represents whether FAST VERIFY ON WAKE is disabled or enabled when Secure Boot is enabled. 1: disabled 0: enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_HYS_EN_PAD[] = {
    &HYS_EN_PAD[0],    		// [] Represents whether the hysteresis function of corresponding PAD is enabled. 1: enabled 0:disabled
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

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_SHA384_EN[] = {
    &SECURE_BOOT_SHA384_EN[0],    		// [] Represents whether secure boot using SHA-384 is enabled. 0: Disable 1: Enable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_BOOTLOADER_ANTI_ROLLBACK_SECURE_VERSION[] = {
    &BOOTLOADER_ANTI_ROLLBACK_SECURE_VERSION[0],    		// [] Represents the anti-rollback secure version of the 2nd stage bootloader used by the ROM bootloader
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_BOOTLOADER_ANTI_ROLLBACK_EN[] = {
    &BOOTLOADER_ANTI_ROLLBACK_EN[0],    		// [] Represents whether the ani-rollback check for the 2nd stage bootloader is enabled.1: Enabled0: Disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_BOOTLOADER_ANTI_ROLLBACK_UPDATE_IN_ROM[] = {
    &BOOTLOADER_ANTI_ROLLBACK_UPDATE_IN_ROM[0],    		// [] Represents whether the ani-rollback SECURE_VERSION will be updated from the ROM bootloader.1: Enable0: Disable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RECOVERY_BOOTLOADER_FLASH_SECTOR[] = {
    &RECOVERY_BOOTLOADER_FLASH_SECTOR[0],    		// [] Represents the starting flash sector (flash sector size is 0x1000) of the recovery bootloader used by the ROM bootloader If the primary bootloader fails. 0 and 0xFFF - this feature is disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RMA_ENA[] = {
    &RMA_ENA[0],    		// [] Represents whether rma function is supported in download mode. 2'b01/2'b10: enabled2'b00/2'b11: disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RMA_SESSION_COUNTER[] = {
    &RMA_SESSION_COUNTER[0],    		// [] Represents the number of times the RMA session has been entered
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RMA_NONCE_ENA[] = {
    &RMA_NONCE_ENA[0],    		// [] Represents whether random number NONCE is used in RMA and whether the KM module is used to generate the NONCE. 2'bx0: No NONCE 2'b1x: Use KM generate NONCE.
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RMA_CHIP_INFO_SOURCE[] = {
    &RMA_CHIP_INFO_SOURCE[0],    		// [] Represents whether HUK_info is selected as the source for calculating CHIP_info in RMA.1: use HUK_info 0: use UNIQ_id
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RMA_DISABLE_FAST_VEF[] = {
    &RMA_DISABLE_FAST_VEF[0],    		// [] Represents whether disable FAST_VEF in RMA session.1: disable0: enable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PVT_0_GLITCH_EN[] = {
    &PVT_0_GLITCH_EN[0],    		// [] Represents whether to enable PVT power glitch monitor function.1:Enable. 0:Disable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PVT_0_GLITCH_MODE[] = {
    &PVT_0_GLITCH_MODE[0],    		// [] Use to configure glitch mode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PVT_1_GLITCH_EN[] = {
    &PVT_1_GLITCH_EN[0],    		// [] Represents whether to enable PVT power glitch monitor function.1:Enable. 0:Disable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PVT_1_GLITCH_MODE[] = {
    &PVT_1_GLITCH_MODE[0],    		// [] Use to configure glitch mode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PMU_FLASH_POWER_SEL[] = {
    &PMU_FLASH_POWER_SEL[0],    		// [] FLASH power select. 1'b1: use 3.3V1'b0: use 1.8V
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PMU_FLASH_POWER_SEL_EN[] = {
    &PMU_FLASH_POWER_SEL_EN[0],    		// [] FLASH power select enable signal. 1'b1 : validates EFUSE_PMU_FLASH_POWER_SEL 1'b0: invalidates EFUSE_PMU_FLASH_POWER_SEL
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_POWER_GLITCH_EN[] = {
    &POWER_GLITCH_EN[0],    		// [] set these bit enable power glitch enable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ENA_XTS_SHADOW[] = {
    &ENA_XTS_SHADOW[0],    		// [] Represents whether to enable XTS-AES shadow core countermeasure against fault injection attacks.  0: Disabled 1: Enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ENA_SPI_BOOT_CRYPT_SCRAMBLER[] = {
    &ENA_SPI_BOOT_CRYPT_SCRAMBLER[0],    		// [] Represents whether to enable ciphertext scrambler for external memory .  0: Disabled 1: Enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RE_ENABLE_JTAG_SOURCE[] = {
    &RE_ENABLE_JTAG_SOURCE[0],    		// [] Represents which Crypto peripheral is selected for re-enabling JTAG.  0: RMA 1: HMAC
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
    &MAC_EXT[0],    		// [] Stores the extended bits of MAC address
    &MAC_EXT[1],    		// [] Stores the extended bits of MAC address
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
    &PSRAM_CAP[0],    		// [] Psram capacity
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_TEMP[] = {
    &TEMP[0],    		// [] Maximum ambient temperature that ESP Chip can work properly
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PSRAM_VENDOR[] = {
    &PSRAM_VENDOR[0],    		// [] Psram vendor
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

const esp_efuse_desc_t* ESP_EFUSE_SYS_DATA_PART2[] = {
    &SYS_DATA_PART2[0],    		// [BLOCK_SYS_DATA2] System data part 2 (reserved)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USB_DEVICE_EXCHG_PINS[] = {
    &USB_DEVICE_EXCHG_PINS[0],    		// [] Represents whether enable usb device exchange pins of D+ and D- or not.  1: enabled 0: disabled
    NULL
};
