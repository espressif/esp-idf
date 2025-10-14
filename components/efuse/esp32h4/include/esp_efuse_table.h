/*
 * SPDX-FileCopyrightText: 2017-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_efuse.h"

// md5_digest_table 39c442690c2273d557b5bb0db99fbe04
// This file was generated from the file esp_efuse_table.csv. DO NOT CHANGE THIS FILE MANUALLY.
// If you want to change some fields, you need to change esp_efuse_table.csv file
// then run `efuse_common_table` or `efuse_custom_table` command it will generate this file.
// To show efuse_table run the command 'show_efuse_table'.


extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RD_DIS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KM_DISABLE_DEPLOY_MODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KM_RND_SWITCH_CYCLE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KM_DEPLOY_ONLY_ONCE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FORCE_USE_KEY_MANAGER_KEY[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FORCE_DISABLE_SW_INIT_KEY[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KM_XTS_KEY_LENGTH_256[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_LOCK_KM_KEY[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_USB_JTAG[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_FORCE_DOWNLOAD[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_DOWNLOAD_MSPI_DIS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_TWAI[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_JTAG_SEL_ENABLE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_PAD_JTAG[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DOWNLOAD_MANUAL_ENCRYPT[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_BOOT_CRYPT_CNT[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE0[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE1[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE2[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_PURPOSE_0[];
#define ESP_EFUSE_WR_DIS_KEY0_PURPOSE ESP_EFUSE_WR_DIS_KEY_PURPOSE_0
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_PURPOSE_1[];
#define ESP_EFUSE_WR_DIS_KEY1_PURPOSE ESP_EFUSE_WR_DIS_KEY_PURPOSE_1
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_PURPOSE_2[];
#define ESP_EFUSE_WR_DIS_KEY2_PURPOSE ESP_EFUSE_WR_DIS_KEY_PURPOSE_2
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_PURPOSE_3[];
#define ESP_EFUSE_WR_DIS_KEY3_PURPOSE ESP_EFUSE_WR_DIS_KEY_PURPOSE_3
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_PURPOSE_4[];
#define ESP_EFUSE_WR_DIS_KEY4_PURPOSE ESP_EFUSE_WR_DIS_KEY_PURPOSE_4
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY_PURPOSE_5[];
#define ESP_EFUSE_WR_DIS_KEY5_PURPOSE ESP_EFUSE_WR_DIS_KEY_PURPOSE_5
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SEC_DPA_LEVEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_XTS_DPA_PSEUDO_LEVEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_XTS_DPA_CLK_ENABLE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ECC_FORCE_CONST_TIME[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_EN[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_AGGRESSIVE_REVOKE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_TPUW[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DOWNLOAD_MODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_DIRECT_BOOT[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_USB_SERIAL_JTAG_ROM_PRINT[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ENABLE_SECURITY_DOWNLOAD[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_UART_PRINT_CONTROL[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FORCE_SEND_RESUME[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_HUK_GEN_STATE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK1[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_MAC[];
#define ESP_EFUSE_WR_DIS_MAC_FACTORY ESP_EFUSE_WR_DIS_MAC
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_MAC_EXT[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PVT_LIMIT[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PVT_CELL_SELECT[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PVT_PUMP_LIMIT[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PUMP_DRV[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_WDT_DELAY_SEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_HYS_EN_PAD[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_PVT_GLITCH_CHARGE_RESET[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_VDD_SPI_LDO_ADJUST[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_LDO_POWER_SEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SYS_DATA_PART1[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_USR_DATA[];
#define ESP_EFUSE_WR_DIS_USER_DATA ESP_EFUSE_WR_DIS_BLOCK_USR_DATA
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_CUSTOM_MAC[];
#define ESP_EFUSE_WR_DIS_MAC_CUSTOM ESP_EFUSE_WR_DIS_CUSTOM_MAC
#define ESP_EFUSE_WR_DIS_USER_DATA_MAC_CUSTOM ESP_EFUSE_WR_DIS_CUSTOM_MAC
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY0[];
#define ESP_EFUSE_WR_DIS_KEY0 ESP_EFUSE_WR_DIS_BLOCK_KEY0
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY1[];
#define ESP_EFUSE_WR_DIS_KEY1 ESP_EFUSE_WR_DIS_BLOCK_KEY1
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY2[];
#define ESP_EFUSE_WR_DIS_KEY2 ESP_EFUSE_WR_DIS_BLOCK_KEY2
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY3[];
#define ESP_EFUSE_WR_DIS_KEY3 ESP_EFUSE_WR_DIS_BLOCK_KEY3
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY4[];
#define ESP_EFUSE_WR_DIS_KEY4 ESP_EFUSE_WR_DIS_BLOCK_KEY4
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_KEY5[];
#define ESP_EFUSE_WR_DIS_KEY5 ESP_EFUSE_WR_DIS_BLOCK_KEY5
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLOCK_SYS_DATA2[];
#define ESP_EFUSE_WR_DIS_SYS_DATA_PART2 ESP_EFUSE_WR_DIS_BLOCK_SYS_DATA2
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_USB_EXCHG_PINS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SOFT_DIS_JTAG[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_KEY0[];
#define ESP_EFUSE_RD_DIS_KEY0 ESP_EFUSE_RD_DIS_BLOCK_KEY0
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_KEY1[];
#define ESP_EFUSE_RD_DIS_KEY1 ESP_EFUSE_RD_DIS_BLOCK_KEY1
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_KEY2[];
#define ESP_EFUSE_RD_DIS_KEY2 ESP_EFUSE_RD_DIS_BLOCK_KEY2
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_KEY3[];
#define ESP_EFUSE_RD_DIS_KEY3 ESP_EFUSE_RD_DIS_BLOCK_KEY3
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_KEY4[];
#define ESP_EFUSE_RD_DIS_KEY4 ESP_EFUSE_RD_DIS_BLOCK_KEY4
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_KEY5[];
#define ESP_EFUSE_RD_DIS_KEY5 ESP_EFUSE_RD_DIS_BLOCK_KEY5
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLOCK_SYS_DATA2[];
#define ESP_EFUSE_RD_DIS_SYS_DATA_PART2 ESP_EFUSE_RD_DIS_BLOCK_SYS_DATA2
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_JTAG[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_FORCE_DOWNLOAD[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_DOWNLOAD_MSPI_DIS[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_TWAI[];
extern const esp_efuse_desc_t* ESP_EFUSE_JTAG_SEL_ENABLE[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_PAD_JTAG[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT[];
extern const esp_efuse_desc_t* ESP_EFUSE_PVT_GLITCH_EN[];
extern const esp_efuse_desc_t* ESP_EFUSE_PVT_GLITCH_MODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_CORE1[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_BOOT_CRYPT_CNT[];
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_KEY_REVOKE0[];
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_KEY_REVOKE1[];
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_KEY_REVOKE2[];
extern const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_0[];
#define ESP_EFUSE_KEY0_PURPOSE ESP_EFUSE_KEY_PURPOSE_0
extern const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_1[];
#define ESP_EFUSE_KEY1_PURPOSE ESP_EFUSE_KEY_PURPOSE_1
extern const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_2[];
#define ESP_EFUSE_KEY2_PURPOSE ESP_EFUSE_KEY_PURPOSE_2
extern const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_3[];
#define ESP_EFUSE_KEY3_PURPOSE ESP_EFUSE_KEY_PURPOSE_3
extern const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_4[];
#define ESP_EFUSE_KEY4_PURPOSE ESP_EFUSE_KEY_PURPOSE_4
extern const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_5[];
#define ESP_EFUSE_KEY5_PURPOSE ESP_EFUSE_KEY_PURPOSE_5
extern const esp_efuse_desc_t* ESP_EFUSE_SEC_DPA_LEVEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_XTS_DPA_PSEUDO_LEVEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_XTS_DPA_CLK_ENABLE[];
extern const esp_efuse_desc_t* ESP_EFUSE_ECC_FORCE_CONST_TIME[];
extern const esp_efuse_desc_t* ESP_EFUSE_ECDSA_P384_ENABLE[];
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_EN[];
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE[];
extern const esp_efuse_desc_t* ESP_EFUSE_KM_DISABLE_DEPLOY_MODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_KM_RND_SWITCH_CYCLE[];
extern const esp_efuse_desc_t* ESP_EFUSE_KM_DEPLOY_ONLY_ONCE[];
extern const esp_efuse_desc_t* ESP_EFUSE_FORCE_USE_KEY_MANAGER_KEY[];
extern const esp_efuse_desc_t* ESP_EFUSE_FORCE_DISABLE_SW_INIT_KEY[];
extern const esp_efuse_desc_t* ESP_EFUSE_KM_XTS_KEY_LENGTH_256[];
extern const esp_efuse_desc_t* ESP_EFUSE_LOCK_KM_KEY[];
extern const esp_efuse_desc_t* ESP_EFUSE_FLASH_TPUW[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_DIRECT_BOOT[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_ENABLE_SECURITY_DOWNLOAD[];
extern const esp_efuse_desc_t* ESP_EFUSE_UART_PRINT_CONTROL[];
extern const esp_efuse_desc_t* ESP_EFUSE_FORCE_SEND_RESUME[];
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_HUK_GEN_STATE[];
extern const esp_efuse_desc_t* ESP_EFUSE_FLASH_LDO_EFUSE_SEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_USB_EXCHG_PINS[];
extern const esp_efuse_desc_t* ESP_EFUSE_USB_OTG_FS_EXCHG_PINS[];
extern const esp_efuse_desc_t* ESP_EFUSE_USB_PHY_SEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_SOFT_DIS_JTAG[];
extern const esp_efuse_desc_t* ESP_EFUSE_IO_LDO_ADJUST[];
extern const esp_efuse_desc_t* ESP_EFUSE_IO_LDO_1P8[];
extern const esp_efuse_desc_t* ESP_EFUSE_DCDC_CCM_EN[];
extern const esp_efuse_desc_t* ESP_EFUSE_MAC[];
#define ESP_EFUSE_MAC_FACTORY ESP_EFUSE_MAC
extern const esp_efuse_desc_t* ESP_EFUSE_MAC_EXT[];
extern const esp_efuse_desc_t* ESP_EFUSE_PVT_LIMIT[];
extern const esp_efuse_desc_t* ESP_EFUSE_PVT_CELL_SELECT[];
extern const esp_efuse_desc_t* ESP_EFUSE_PVT_PUMP_LIMIT[];
extern const esp_efuse_desc_t* ESP_EFUSE_PUMP_DRV[];
extern const esp_efuse_desc_t* ESP_EFUSE_WDT_DELAY_SEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_HYS_EN_PAD[];
extern const esp_efuse_desc_t* ESP_EFUSE_PVT_GLITCH_CHARGE_RESET[];
extern const esp_efuse_desc_t* ESP_EFUSE_VDD_SPI_LDO_ADJUST[];
extern const esp_efuse_desc_t* ESP_EFUSE_FLASH_LDO_POWER_SEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_USER_DATA[];
#define ESP_EFUSE_BLOCK_USR_DATA ESP_EFUSE_USER_DATA
extern const esp_efuse_desc_t* ESP_EFUSE_USER_DATA_MAC_CUSTOM[];
#define ESP_EFUSE_MAC_CUSTOM ESP_EFUSE_USER_DATA_MAC_CUSTOM
#define ESP_EFUSE_CUSTOM_MAC ESP_EFUSE_USER_DATA_MAC_CUSTOM
extern const esp_efuse_desc_t* ESP_EFUSE_KEY0[];
#define ESP_EFUSE_BLOCK_KEY0 ESP_EFUSE_KEY0
extern const esp_efuse_desc_t* ESP_EFUSE_KEY1[];
#define ESP_EFUSE_BLOCK_KEY1 ESP_EFUSE_KEY1
extern const esp_efuse_desc_t* ESP_EFUSE_KEY2[];
#define ESP_EFUSE_BLOCK_KEY2 ESP_EFUSE_KEY2
extern const esp_efuse_desc_t* ESP_EFUSE_KEY3[];
#define ESP_EFUSE_BLOCK_KEY3 ESP_EFUSE_KEY3
extern const esp_efuse_desc_t* ESP_EFUSE_KEY4[];
#define ESP_EFUSE_BLOCK_KEY4 ESP_EFUSE_KEY4
extern const esp_efuse_desc_t* ESP_EFUSE_KEY5[];
#define ESP_EFUSE_BLOCK_KEY5 ESP_EFUSE_KEY5
extern const esp_efuse_desc_t* ESP_EFUSE_SYS_DATA_PART2[];
#define ESP_EFUSE_BLOCK_SYS_DATA2 ESP_EFUSE_SYS_DATA_PART2

#ifdef __cplusplus
}
#endif
