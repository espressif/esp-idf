/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define EFUSE_WRITE_OP_CODE 0x5a5a
#define EFUSE_READ_OP_CODE 0x5aa5

/* Write disable bits */
#define EFUSE_WR_DIS_RD_DIS (1<<0) /*< disable writing read disable reg */
#define EFUSE_WR_DIS_WR_DIS (1<<1) /*< disable writing write disable reg */
#define EFUSE_WR_DIS_FLASH_CRYPT_CNT (1<<2)
#define EFUSE_WR_DIS_MAC_SPI_CONFIG_HD (1<<3) /*< disable writing MAC & SPI config hd efuses */
#define EFUSE_WR_DIS_XPD_SDIO (1<<5) /*< disable writing SDIO config efuses */
#define EFUSE_WR_DIS_SPI_PAD_CONFIG (1<<6) /*< disable writing SPI_PAD_CONFIG efuses */
#define EFUSE_WR_DIS_BLK1 (1<<7) /*< disable writing BLK1 efuses */
#define EFUSE_WR_DIS_BLK2 (1<<8) /*< disable writing BLK2 efuses */
#define EFUSE_WR_DIS_BLK3 (1<<9) /*< disable writing BLK3 efuses */
#define EFUSE_WR_DIS_FLASH_CRYPT_CODING_SCHEME (1<<10) /*< disable writing FLASH_CRYPT_CONFIG and CODING_SCHEME efuses */
#define EFUSE_WR_DIS_ABS_DONE_0 (1<<12) /*< disable writing ABS_DONE_0 efuse */
#define EFUSE_WR_DIS_ABS_DONE_1 (1<<13) /*< disable writing ABS_DONE_1 efuse */
#define EFUSE_WR_DIS_JTAG_DISABLE (1<<14) /*< disable writing JTAG_DISABLE efuse */
#define EFUSE_WR_DIS_CONSOLE_DL_DISABLE (1<<15) /*< disable writing CONSOLE_DEBUG_DISABLE, DISABLE_DL_ENCRYPT, DISABLE_DL_DECRYPT and DISABLE_DL_CACHE efuses */

/* Read disable bits for efuse blocks 1-3 */
#define EFUSE_RD_DIS_BLK1 (1<<16)
#define EFUSE_RD_DIS_BLK2 (1<<17)
#define EFUSE_RD_DIS_BLK3 (1<<18)

#define EFUSE_RD_CHIP_VER_PKG_ESP32D0WDQ6  0
#define EFUSE_RD_CHIP_VER_PKG_ESP32D0WDQ5  1
#define EFUSE_RD_CHIP_VER_PKG_ESP32D2WDQ5  2
#define EFUSE_RD_CHIP_VER_PKG_ESP32PICOD2  4 /* Deprecated: this chip was never mass produced  */
#define EFUSE_RD_CHIP_VER_PKG_ESP32U4WDH   4
#define EFUSE_RD_CHIP_VER_PKG_ESP32PICOD4  5
#define EFUSE_RD_CHIP_VER_PKG_ESP32PICOV302  6
#define EFUSE_RD_CHIP_VER_PKG_ESP32D0WDR2V3  7

#define EFUSE_CODING_SCHEME_VAL_NONE 0x0
#define EFUSE_CODING_SCHEME_VAL_34 0x1
#define EFUSE_CODING_SCHEME_VAL_REPEAT 0x2

#ifdef __cplusplus
}
#endif
