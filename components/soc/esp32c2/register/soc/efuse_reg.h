/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#include "soc/efuse_defs.h"
#ifdef __cplusplus
extern "C" {
#endif

/** EFUSE_PGM_DATA0_REG register
 *  Register 0 that stores data to be programmed.
 */
#define EFUSE_PGM_DATA0_REG (DR_REG_EFUSE_BASE + 0x0)
/** EFUSE_PGM_DATA_0 : R/W; bitpos: [31:0]; default: 0;
 *  The content of the 0th 32-bit data to be programmed.
 */
#define EFUSE_PGM_DATA_0    0xFFFFFFFFU
#define EFUSE_PGM_DATA_0_M  (EFUSE_PGM_DATA_0_V << EFUSE_PGM_DATA_0_S)
#define EFUSE_PGM_DATA_0_V  0xFFFFFFFFU
#define EFUSE_PGM_DATA_0_S  0

/** EFUSE_PGM_DATA1_REG register
 *  Register 1 that stores data to be programmed.
 */
#define EFUSE_PGM_DATA1_REG (DR_REG_EFUSE_BASE + 0x4)
/** EFUSE_PGM_DATA_1 : R/W; bitpos: [31:0]; default: 0;
 *  The content of the 1st 32-bit data to be programmed.
 */
#define EFUSE_PGM_DATA_1    0xFFFFFFFFU
#define EFUSE_PGM_DATA_1_M  (EFUSE_PGM_DATA_1_V << EFUSE_PGM_DATA_1_S)
#define EFUSE_PGM_DATA_1_V  0xFFFFFFFFU
#define EFUSE_PGM_DATA_1_S  0

/** EFUSE_PGM_DATA2_REG register
 *  Register 2 that stores data to be programmed.
 */
#define EFUSE_PGM_DATA2_REG (DR_REG_EFUSE_BASE + 0x8)
/** EFUSE_PGM_DATA_2 : R/W; bitpos: [31:0]; default: 0;
 *  The content of the 2nd 32-bit data to be programmed.
 */
#define EFUSE_PGM_DATA_2    0xFFFFFFFFU
#define EFUSE_PGM_DATA_2_M  (EFUSE_PGM_DATA_2_V << EFUSE_PGM_DATA_2_S)
#define EFUSE_PGM_DATA_2_V  0xFFFFFFFFU
#define EFUSE_PGM_DATA_2_S  0

/** EFUSE_PGM_DATA3_REG register
 *  Register 3 that stores data to be programmed.
 */
#define EFUSE_PGM_DATA3_REG (DR_REG_EFUSE_BASE + 0xc)
/** EFUSE_PGM_DATA_3 : R/W; bitpos: [31:0]; default: 0;
 *  The content of the 3rd 32-bit data to be programmed.
 */
#define EFUSE_PGM_DATA_3    0xFFFFFFFFU
#define EFUSE_PGM_DATA_3_M  (EFUSE_PGM_DATA_3_V << EFUSE_PGM_DATA_3_S)
#define EFUSE_PGM_DATA_3_V  0xFFFFFFFFU
#define EFUSE_PGM_DATA_3_S  0

/** EFUSE_PGM_DATA4_REG register
 *  Register 4 that stores data to be programmed.
 */
#define EFUSE_PGM_DATA4_REG (DR_REG_EFUSE_BASE + 0x10)
/** EFUSE_PGM_DATA_4 : R/W; bitpos: [31:0]; default: 0;
 *  The content of the 4th 32-bit data to be programmed.
 */
#define EFUSE_PGM_DATA_4    0xFFFFFFFFU
#define EFUSE_PGM_DATA_4_M  (EFUSE_PGM_DATA_4_V << EFUSE_PGM_DATA_4_S)
#define EFUSE_PGM_DATA_4_V  0xFFFFFFFFU
#define EFUSE_PGM_DATA_4_S  0

/** EFUSE_PGM_DATA5_REG register
 *  Register 5 that stores data to be programmed.
 */
#define EFUSE_PGM_DATA5_REG (DR_REG_EFUSE_BASE + 0x14)
/** EFUSE_PGM_DATA_5 : R/W; bitpos: [31:0]; default: 0;
 *  The content of the 5th 32-bit data to be programmed.
 */
#define EFUSE_PGM_DATA_5    0xFFFFFFFFU
#define EFUSE_PGM_DATA_5_M  (EFUSE_PGM_DATA_5_V << EFUSE_PGM_DATA_5_S)
#define EFUSE_PGM_DATA_5_V  0xFFFFFFFFU
#define EFUSE_PGM_DATA_5_S  0

/** EFUSE_PGM_DATA6_REG register
 *  Register 6 that stores data to be programmed.
 */
#define EFUSE_PGM_DATA6_REG (DR_REG_EFUSE_BASE + 0x18)
/** EFUSE_PGM_DATA_6 : R/W; bitpos: [31:0]; default: 0;
 *  The content of the 6th 32-bit data to be programmed.
 */
#define EFUSE_PGM_DATA_6    0xFFFFFFFFU
#define EFUSE_PGM_DATA_6_M  (EFUSE_PGM_DATA_6_V << EFUSE_PGM_DATA_6_S)
#define EFUSE_PGM_DATA_6_V  0xFFFFFFFFU
#define EFUSE_PGM_DATA_6_S  0

/** EFUSE_PGM_DATA7_REG register
 *  Register 7 that stores data to be programmed.
 */
#define EFUSE_PGM_DATA7_REG (DR_REG_EFUSE_BASE + 0x1c)
/** EFUSE_PGM_DATA_7 : R/W; bitpos: [31:0]; default: 0;
 *  The content of the 7th 32-bit data to be programmed.
 */
#define EFUSE_PGM_DATA_7    0xFFFFFFFFU
#define EFUSE_PGM_DATA_7_M  (EFUSE_PGM_DATA_7_V << EFUSE_PGM_DATA_7_S)
#define EFUSE_PGM_DATA_7_V  0xFFFFFFFFU
#define EFUSE_PGM_DATA_7_S  0

/** EFUSE_PGM_CHECK_VALUE0_REG register
 *  Register 0 that stores the RS code to be programmed.
 */
#define EFUSE_PGM_CHECK_VALUE0_REG (DR_REG_EFUSE_BASE + 0x20)
/** EFUSE_PGM_RS_DATA_0 : R/W; bitpos: [31:0]; default: 0;
 *  The content of the 0th 32-bit RS code to be programmed.
 */
#define EFUSE_PGM_RS_DATA_0    0xFFFFFFFFU
#define EFUSE_PGM_RS_DATA_0_M  (EFUSE_PGM_RS_DATA_0_V << EFUSE_PGM_RS_DATA_0_S)
#define EFUSE_PGM_RS_DATA_0_V  0xFFFFFFFFU
#define EFUSE_PGM_RS_DATA_0_S  0

/** EFUSE_PGM_CHECK_VALUE1_REG register
 *  Register 1 that stores the RS code to be programmed.
 */
#define EFUSE_PGM_CHECK_VALUE1_REG (DR_REG_EFUSE_BASE + 0x24)
/** EFUSE_PGM_RS_DATA_1 : R/W; bitpos: [31:0]; default: 0;
 *  The content of the 1st 32-bit RS code to be programmed.
 */
#define EFUSE_PGM_RS_DATA_1    0xFFFFFFFFU
#define EFUSE_PGM_RS_DATA_1_M  (EFUSE_PGM_RS_DATA_1_V << EFUSE_PGM_RS_DATA_1_S)
#define EFUSE_PGM_RS_DATA_1_V  0xFFFFFFFFU
#define EFUSE_PGM_RS_DATA_1_S  0

/** EFUSE_PGM_CHECK_VALUE2_REG register
 *  Register 2 that stores the RS code to be programmed.
 */
#define EFUSE_PGM_CHECK_VALUE2_REG (DR_REG_EFUSE_BASE + 0x28)
/** EFUSE_PGM_RS_DATA_2 : R/W; bitpos: [31:0]; default: 0;
 *  The content of the 2nd 32-bit RS code to be programmed.
 */
#define EFUSE_PGM_RS_DATA_2    0xFFFFFFFFU
#define EFUSE_PGM_RS_DATA_2_M  (EFUSE_PGM_RS_DATA_2_V << EFUSE_PGM_RS_DATA_2_S)
#define EFUSE_PGM_RS_DATA_2_V  0xFFFFFFFFU
#define EFUSE_PGM_RS_DATA_2_S  0

/** EFUSE_RD_WR_DIS_REG register
 *  BLOCK0 data register 0.
 */
#define EFUSE_RD_WR_DIS_REG (DR_REG_EFUSE_BASE + 0x2c)
/** EFUSE_WR_DIS : RO; bitpos: [7:0]; default: 0;
 *  Disable programming of individual eFuses.
 */
#define EFUSE_WR_DIS    0x000000FFU
#define EFUSE_WR_DIS_M  (EFUSE_WR_DIS_V << EFUSE_WR_DIS_S)
#define EFUSE_WR_DIS_V  0x000000FFU
#define EFUSE_WR_DIS_S  0
/** EFUSE_RESERVED_0_8 : RW; bitpos: [31:8]; default: 0; */
#define EFUSE_RESERVED_0_8    0x00FFFFFFU
#define EFUSE_RESERVED_0_8_M  (EFUSE_RESERVED_0_8_V << EFUSE_RESERVED_0_8_S)
#define EFUSE_RESERVED_0_8_V  0x00FFFFFFU
#define EFUSE_RESERVED_0_8_S  8

/** EFUSE_RD_REPEAT_DATA0_REG register
 *  BLOCK0 data register 1.
 */
#define EFUSE_RD_REPEAT_DATA0_REG (DR_REG_EFUSE_BASE + 0x30)
/** EFUSE_RD_DIS : RO; bitpos: [1:0]; default: 0;
 *  The bit be set to disable software read high/low 128-bit of BLK3.
 */
#define EFUSE_RD_DIS    0x00000003U
#define EFUSE_RD_DIS_M  (EFUSE_RD_DIS_V << EFUSE_RD_DIS_S)
#define EFUSE_RD_DIS_V  0x00000003U
#define EFUSE_RD_DIS_S  0
/** EFUSE_WDT_DELAY_SEL : RO; bitpos: [3:2]; default: 0;
 *  Selects RTC watchdog timeout threshold, in unit of slow clock cycle. 0: 40000. 1:
 *  80000. 2: 160000. 3:320000.
 */
#define EFUSE_WDT_DELAY_SEL    0x00000003U
#define EFUSE_WDT_DELAY_SEL_M  (EFUSE_WDT_DELAY_SEL_V << EFUSE_WDT_DELAY_SEL_S)
#define EFUSE_WDT_DELAY_SEL_V  0x00000003U
#define EFUSE_WDT_DELAY_SEL_S  2
/** EFUSE_DIS_PAD_JTAG : RO; bitpos: [4]; default: 0;
 *  Set this bit to disable pad jtag.
 */
#define EFUSE_DIS_PAD_JTAG    (BIT(4))
#define EFUSE_DIS_PAD_JTAG_M  (EFUSE_DIS_PAD_JTAG_V << EFUSE_DIS_PAD_JTAG_S)
#define EFUSE_DIS_PAD_JTAG_V  0x00000001U
#define EFUSE_DIS_PAD_JTAG_S  4
/** EFUSE_DIS_DOWNLOAD_ICACHE : RO; bitpos: [5]; default: 0;
 *  The bit be set to disable icache in download mode.
 */
#define EFUSE_DIS_DOWNLOAD_ICACHE    (BIT(5))
#define EFUSE_DIS_DOWNLOAD_ICACHE_M  (EFUSE_DIS_DOWNLOAD_ICACHE_V << EFUSE_DIS_DOWNLOAD_ICACHE_S)
#define EFUSE_DIS_DOWNLOAD_ICACHE_V  0x00000001U
#define EFUSE_DIS_DOWNLOAD_ICACHE_S  5
/** EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT : RO; bitpos: [6]; default: 0;
 *  The bit be set to disable manual encryption.
 */
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT    (BIT(6))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_M  (EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_V << EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_S)
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_V  0x00000001U
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_S  6
/** EFUSE_SPI_BOOT_CRYPT_CNT : RO; bitpos: [9:7]; default: 0;
 *  These bits be set to enable SPI boot encrypt/decrypt. Odd number of 1: enable. even
 *  number of 1: disable.
 */
#define EFUSE_SPI_BOOT_CRYPT_CNT    0x00000007U
#define EFUSE_SPI_BOOT_CRYPT_CNT_M  (EFUSE_SPI_BOOT_CRYPT_CNT_V << EFUSE_SPI_BOOT_CRYPT_CNT_S)
#define EFUSE_SPI_BOOT_CRYPT_CNT_V  0x00000007U
#define EFUSE_SPI_BOOT_CRYPT_CNT_S  7
/** EFUSE_XTS_KEY_LENGTH_256 : RO; bitpos: [10]; default: 0;
 *  The bit be set means XTS_AES use the whole 256-bit efuse data in BLOCK3. Otherwise,
 *  XTS_AES use 128-bit eFuse data in BLOCK3.
 */
#define EFUSE_XTS_KEY_LENGTH_256    (BIT(10))
#define EFUSE_XTS_KEY_LENGTH_256_M  (EFUSE_XTS_KEY_LENGTH_256_V << EFUSE_XTS_KEY_LENGTH_256_S)
#define EFUSE_XTS_KEY_LENGTH_256_V  0x00000001U
#define EFUSE_XTS_KEY_LENGTH_256_S  10
/** EFUSE_UART_PRINT_CONTROL : RO; bitpos: [12:11]; default: 0;
 *  Set this bit to disable usb printing.
 */
#define EFUSE_UART_PRINT_CONTROL    0x00000003U
#define EFUSE_UART_PRINT_CONTROL_M  (EFUSE_UART_PRINT_CONTROL_V << EFUSE_UART_PRINT_CONTROL_S)
#define EFUSE_UART_PRINT_CONTROL_V  0x00000003U
#define EFUSE_UART_PRINT_CONTROL_S  11
/** EFUSE_FORCE_SEND_RESUME : RO; bitpos: [13]; default: 0;
 *  Set this bit to force ROM code to send a resume command during SPI boot.
 */
#define EFUSE_FORCE_SEND_RESUME    (BIT(13))
#define EFUSE_FORCE_SEND_RESUME_M  (EFUSE_FORCE_SEND_RESUME_V << EFUSE_FORCE_SEND_RESUME_S)
#define EFUSE_FORCE_SEND_RESUME_V  0x00000001U
#define EFUSE_FORCE_SEND_RESUME_S  13
/** EFUSE_DIS_DOWNLOAD_MODE : RO; bitpos: [14]; default: 0;
 *  Set this bit to disable download mode (boot_mode[3:0] = 0, 1, 2, 4, 5, 6, 7).
 */
#define EFUSE_DIS_DOWNLOAD_MODE    (BIT(14))
#define EFUSE_DIS_DOWNLOAD_MODE_M  (EFUSE_DIS_DOWNLOAD_MODE_V << EFUSE_DIS_DOWNLOAD_MODE_S)
#define EFUSE_DIS_DOWNLOAD_MODE_V  0x00000001U
#define EFUSE_DIS_DOWNLOAD_MODE_S  14
/** EFUSE_DIS_DIRECT_BOOT : RO; bitpos: [15]; default: 0;
 *  This bit set means disable direct_boot mode.
 */
#define EFUSE_DIS_DIRECT_BOOT    (BIT(15))
#define EFUSE_DIS_DIRECT_BOOT_M  (EFUSE_DIS_DIRECT_BOOT_V << EFUSE_DIS_DIRECT_BOOT_S)
#define EFUSE_DIS_DIRECT_BOOT_V  0x00000001U
#define EFUSE_DIS_DIRECT_BOOT_S  15
/** EFUSE_ENABLE_SECURITY_DOWNLOAD : RO; bitpos: [16]; default: 0;
 *  Set this bit to enable secure UART download mode.
 */
#define EFUSE_ENABLE_SECURITY_DOWNLOAD    (BIT(16))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_M  (EFUSE_ENABLE_SECURITY_DOWNLOAD_V << EFUSE_ENABLE_SECURITY_DOWNLOAD_S)
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_V  0x00000001U
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_S  16
/** EFUSE_FLASH_TPUW : RO; bitpos: [20:17]; default: 0;
 *  Configures flash waiting time after power-up, in unit of ms. If the value is less
 *  than 15, the waiting time is the configurable value.  Otherwise, the waiting time
 *  is twice the configurable value.
 */
#define EFUSE_FLASH_TPUW    0x0000000FU
#define EFUSE_FLASH_TPUW_M  (EFUSE_FLASH_TPUW_V << EFUSE_FLASH_TPUW_S)
#define EFUSE_FLASH_TPUW_V  0x0000000FU
#define EFUSE_FLASH_TPUW_S  17
/** EFUSE_SECURE_BOOT_EN : RO; bitpos: [21]; default: 0;
 *  The bit be set to enable secure boot.
 */
#define EFUSE_SECURE_BOOT_EN    (BIT(21))
#define EFUSE_SECURE_BOOT_EN_M  (EFUSE_SECURE_BOOT_EN_V << EFUSE_SECURE_BOOT_EN_S)
#define EFUSE_SECURE_BOOT_EN_V  0x00000001U
#define EFUSE_SECURE_BOOT_EN_S  21
/** EFUSE_SECURE_VERSION : R; bitpos: [25:22]; default: 0;
 *  Secure version for anti-rollback
 */
#define EFUSE_SECURE_VERSION    0x0000000FU
#define EFUSE_SECURE_VERSION_M  (EFUSE_SECURE_VERSION_V << EFUSE_SECURE_VERSION_S)
#define EFUSE_SECURE_VERSION_V  0x0000000FU
#define EFUSE_SECURE_VERSION_S  22
/** EFUSE_CUSTOM_MAC_USED : R; bitpos: [26]; default: 0;
 *  True if MAC_CUSTOM is burned
 */
#define EFUSE_CUSTOM_MAC_USED    (BIT(26))
#define EFUSE_CUSTOM_MAC_USED_M  (EFUSE_CUSTOM_MAC_USED_V << EFUSE_CUSTOM_MAC_USED_S)
#define EFUSE_CUSTOM_MAC_USED_V  0x00000001U
#define EFUSE_CUSTOM_MAC_USED_S  26
/** EFUSE_DISABLE_WAFER_VERSION_MAJOR : R; bitpos: [27]; default: 0;
 *  Disables check of wafer version major
 */
#define EFUSE_DISABLE_WAFER_VERSION_MAJOR    (BIT(27))
#define EFUSE_DISABLE_WAFER_VERSION_MAJOR_M  (EFUSE_DISABLE_WAFER_VERSION_MAJOR_V << EFUSE_DISABLE_WAFER_VERSION_MAJOR_S)
#define EFUSE_DISABLE_WAFER_VERSION_MAJOR_V  0x00000001U
#define EFUSE_DISABLE_WAFER_VERSION_MAJOR_S  27
/** EFUSE_DISABLE_BLK_VERSION_MAJOR : R; bitpos: [28]; default: 0;
 *  Disables check of blk version major
 */
#define EFUSE_DISABLE_BLK_VERSION_MAJOR    (BIT(28))
#define EFUSE_DISABLE_BLK_VERSION_MAJOR_M  (EFUSE_DISABLE_BLK_VERSION_MAJOR_V << EFUSE_DISABLE_BLK_VERSION_MAJOR_S)
#define EFUSE_DISABLE_BLK_VERSION_MAJOR_V  0x00000001U
#define EFUSE_DISABLE_BLK_VERSION_MAJOR_S  28
/** EFUSE_RESERVED_0_61 : R; bitpos: [31:29]; default: 0;
 *  reserved
 */
#define EFUSE_RESERVED_0_61    0x00000007U
#define EFUSE_RESERVED_0_61_M  (EFUSE_RESERVED_0_61_V << EFUSE_RESERVED_0_61_S)
#define EFUSE_RESERVED_0_61_V  0x00000007U
#define EFUSE_RESERVED_0_61_S  29

/** EFUSE_RD_BLK1_DATA0_REG register
 *  BLOCK1 data register 0.
 */
#define EFUSE_RD_BLK1_DATA0_REG (DR_REG_EFUSE_BASE + 0x34)
/** EFUSE_CUSTOM_MAC : R; bitpos: [31:0]; default: 0;
 *  Custom MAC address
 */
#define EFUSE_CUSTOM_MAC    0xFFFFFFFFU
#define EFUSE_CUSTOM_MAC_M  (EFUSE_CUSTOM_MAC_V << EFUSE_CUSTOM_MAC_S)
#define EFUSE_CUSTOM_MAC_V  0xFFFFFFFFU
#define EFUSE_CUSTOM_MAC_S  0

/** EFUSE_RD_BLK1_DATA1_REG register
 *  BLOCK1 data register 1.
 */
#define EFUSE_RD_BLK1_DATA1_REG (DR_REG_EFUSE_BASE + 0x38)
/** EFUSE_CUSTOM_MAC_1 : R; bitpos: [15:0]; default: 0;
 *  Custom MAC address
 */
#define EFUSE_CUSTOM_MAC_1    0x0000FFFFU
#define EFUSE_CUSTOM_MAC_1_M  (EFUSE_CUSTOM_MAC_1_V << EFUSE_CUSTOM_MAC_1_S)
#define EFUSE_CUSTOM_MAC_1_V  0x0000FFFFU
#define EFUSE_CUSTOM_MAC_1_S  0
/** EFUSE_RESERVED_1_48 : R; bitpos: [31:16]; default: 0;
 *  reserved
 */
#define EFUSE_RESERVED_1_48    0x0000FFFFU
#define EFUSE_RESERVED_1_48_M  (EFUSE_RESERVED_1_48_V << EFUSE_RESERVED_1_48_S)
#define EFUSE_RESERVED_1_48_V  0x0000FFFFU
#define EFUSE_RESERVED_1_48_S  16

/** EFUSE_RD_BLK1_DATA2_REG register
 *  BLOCK1 data register 2.
 */
#define EFUSE_RD_BLK1_DATA2_REG (DR_REG_EFUSE_BASE + 0x3c)
/** EFUSE_SYSTEM_DATA2 : RO; bitpos: [23:0]; default: 0;
 *  Stores the bits [64:87] of system data.
 */
#define EFUSE_SYSTEM_DATA2    0x00FFFFFFU
#define EFUSE_SYSTEM_DATA2_M  (EFUSE_SYSTEM_DATA2_V << EFUSE_SYSTEM_DATA2_S)
#define EFUSE_SYSTEM_DATA2_V  0x00FFFFFFU
#define EFUSE_SYSTEM_DATA2_S  0

/** EFUSE_RD_BLK2_DATA0_REG register
 *  Register 0 of BLOCK2.
 */
#define EFUSE_RD_BLK2_DATA0_REG (DR_REG_EFUSE_BASE + 0x40)
/** EFUSE_MAC : R; bitpos: [31:0]; default: 0;
 *  MAC address
 */
#define EFUSE_MAC    0xFFFFFFFFU
#define EFUSE_MAC_M  (EFUSE_MAC_V << EFUSE_MAC_S)
#define EFUSE_MAC_V  0xFFFFFFFFU
#define EFUSE_MAC_S  0

/** EFUSE_RD_BLK2_DATA1_REG register
 *  Register 1 of BLOCK2.
 */
#define EFUSE_RD_BLK2_DATA1_REG (DR_REG_EFUSE_BASE + 0x44)
/** EFUSE_MAC_1 : R; bitpos: [15:0]; default: 0;
 *  MAC address
 */
#define EFUSE_MAC_1    0x0000FFFFU
#define EFUSE_MAC_1_M  (EFUSE_MAC_1_V << EFUSE_MAC_1_S)
#define EFUSE_MAC_1_V  0x0000FFFFU
#define EFUSE_MAC_1_S  0
/** EFUSE_WAFER_VERSION_MINOR : R; bitpos: [19:16]; default: 0;
 *  WAFER_VERSION_MINOR
 */
#define EFUSE_WAFER_VERSION_MINOR    0x0000000FU
#define EFUSE_WAFER_VERSION_MINOR_M  (EFUSE_WAFER_VERSION_MINOR_V << EFUSE_WAFER_VERSION_MINOR_S)
#define EFUSE_WAFER_VERSION_MINOR_V  0x0000000FU
#define EFUSE_WAFER_VERSION_MINOR_S  16
/** EFUSE_WAFER_VERSION_MAJOR : R; bitpos: [21:20]; default: 0;
 *  WAFER_VERSION_MAJOR
 */
#define EFUSE_WAFER_VERSION_MAJOR    0x00000003U
#define EFUSE_WAFER_VERSION_MAJOR_M  (EFUSE_WAFER_VERSION_MAJOR_V << EFUSE_WAFER_VERSION_MAJOR_S)
#define EFUSE_WAFER_VERSION_MAJOR_V  0x00000003U
#define EFUSE_WAFER_VERSION_MAJOR_S  20
/** EFUSE_PKG_VERSION : R; bitpos: [24:22]; default: 0;
 *  EFUSE_PKG_VERSION
 */
#define EFUSE_PKG_VERSION    0x00000007U
#define EFUSE_PKG_VERSION_M  (EFUSE_PKG_VERSION_V << EFUSE_PKG_VERSION_S)
#define EFUSE_PKG_VERSION_V  0x00000007U
#define EFUSE_PKG_VERSION_S  22
/** EFUSE_BLK_VERSION_MINOR : R; bitpos: [27:25]; default: 0;
 *  Minor version of BLOCK2
 */
#define EFUSE_BLK_VERSION_MINOR    0x00000007U
#define EFUSE_BLK_VERSION_MINOR_M  (EFUSE_BLK_VERSION_MINOR_V << EFUSE_BLK_VERSION_MINOR_S)
#define EFUSE_BLK_VERSION_MINOR_V  0x00000007U
#define EFUSE_BLK_VERSION_MINOR_S  25
/** EFUSE_BLK_VERSION_MAJOR : R; bitpos: [29:28]; default: 0;
 *  Major version of BLOCK2
 */
#define EFUSE_BLK_VERSION_MAJOR    0x00000003U
#define EFUSE_BLK_VERSION_MAJOR_M  (EFUSE_BLK_VERSION_MAJOR_V << EFUSE_BLK_VERSION_MAJOR_S)
#define EFUSE_BLK_VERSION_MAJOR_V  0x00000003U
#define EFUSE_BLK_VERSION_MAJOR_S  28
/** EFUSE_OCODE : R; bitpos: [31:30]; default: 0;
 *  OCode
 */
#define EFUSE_OCODE    0x00000003U
#define EFUSE_OCODE_M  (EFUSE_OCODE_V << EFUSE_OCODE_S)
#define EFUSE_OCODE_V  0x00000003U
#define EFUSE_OCODE_S  30

/** EFUSE_RD_BLK2_DATA2_REG register
 *  Register 2 of BLOCK2.
 */
#define EFUSE_RD_BLK2_DATA2_REG (DR_REG_EFUSE_BASE + 0x48)
/** EFUSE_OCODE_1 : R; bitpos: [4:0]; default: 0;
 *  OCode
 */
#define EFUSE_OCODE_1    0x0000001FU
#define EFUSE_OCODE_1_M  (EFUSE_OCODE_1_V << EFUSE_OCODE_1_S)
#define EFUSE_OCODE_1_V  0x0000001FU
#define EFUSE_OCODE_1_S  0
/** EFUSE_TEMP_CALIB : R; bitpos: [13:5]; default: 0;
 *  Temperature calibration data
 */
#define EFUSE_TEMP_CALIB    0x000001FFU
#define EFUSE_TEMP_CALIB_M  (EFUSE_TEMP_CALIB_V << EFUSE_TEMP_CALIB_S)
#define EFUSE_TEMP_CALIB_V  0x000001FFU
#define EFUSE_TEMP_CALIB_S  5
/** EFUSE_ADC1_INIT_CODE_ATTEN0 : R; bitpos: [21:14]; default: 0;
 *  ADC1 init code at atten0
 */
#define EFUSE_ADC1_INIT_CODE_ATTEN0    0x000000FFU
#define EFUSE_ADC1_INIT_CODE_ATTEN0_M  (EFUSE_ADC1_INIT_CODE_ATTEN0_V << EFUSE_ADC1_INIT_CODE_ATTEN0_S)
#define EFUSE_ADC1_INIT_CODE_ATTEN0_V  0x000000FFU
#define EFUSE_ADC1_INIT_CODE_ATTEN0_S  14
/** EFUSE_ADC1_INIT_CODE_ATTEN3 : R; bitpos: [26:22]; default: 0;
 *  ADC1 init code at atten3
 */
#define EFUSE_ADC1_INIT_CODE_ATTEN3    0x0000001FU
#define EFUSE_ADC1_INIT_CODE_ATTEN3_M  (EFUSE_ADC1_INIT_CODE_ATTEN3_V << EFUSE_ADC1_INIT_CODE_ATTEN3_S)
#define EFUSE_ADC1_INIT_CODE_ATTEN3_V  0x0000001FU
#define EFUSE_ADC1_INIT_CODE_ATTEN3_S  22
/** EFUSE_ADC1_CAL_VOL_ATTEN0 : R; bitpos: [31:27]; default: 0;
 *  ADC1 calibration voltage at atten0
 */
#define EFUSE_ADC1_CAL_VOL_ATTEN0    0x0000001FU
#define EFUSE_ADC1_CAL_VOL_ATTEN0_M  (EFUSE_ADC1_CAL_VOL_ATTEN0_V << EFUSE_ADC1_CAL_VOL_ATTEN0_S)
#define EFUSE_ADC1_CAL_VOL_ATTEN0_V  0x0000001FU
#define EFUSE_ADC1_CAL_VOL_ATTEN0_S  27

/** EFUSE_RD_BLK2_DATA3_REG register
 *  Register 3 of BLOCK2.
 */
#define EFUSE_RD_BLK2_DATA3_REG (DR_REG_EFUSE_BASE + 0x4c)
/** EFUSE_ADC1_CAL_VOL_ATTEN0_1 : R; bitpos: [2:0]; default: 0;
 *  ADC1 calibration voltage at atten0
 */
#define EFUSE_ADC1_CAL_VOL_ATTEN0_1    0x00000007U
#define EFUSE_ADC1_CAL_VOL_ATTEN0_1_M  (EFUSE_ADC1_CAL_VOL_ATTEN0_1_V << EFUSE_ADC1_CAL_VOL_ATTEN0_1_S)
#define EFUSE_ADC1_CAL_VOL_ATTEN0_1_V  0x00000007U
#define EFUSE_ADC1_CAL_VOL_ATTEN0_1_S  0
/** EFUSE_ADC1_CAL_VOL_ATTEN3 : R; bitpos: [8:3]; default: 0;
 *  ADC1 calibration voltage at atten3
 */
#define EFUSE_ADC1_CAL_VOL_ATTEN3    0x0000003FU
#define EFUSE_ADC1_CAL_VOL_ATTEN3_M  (EFUSE_ADC1_CAL_VOL_ATTEN3_V << EFUSE_ADC1_CAL_VOL_ATTEN3_S)
#define EFUSE_ADC1_CAL_VOL_ATTEN3_V  0x0000003FU
#define EFUSE_ADC1_CAL_VOL_ATTEN3_S  3
/** EFUSE_DIG_DBIAS_HVT : R; bitpos: [13:9]; default: 0;
 *  BLOCK2 digital dbias when hvt
 */
#define EFUSE_DIG_DBIAS_HVT    0x0000001FU
#define EFUSE_DIG_DBIAS_HVT_M  (EFUSE_DIG_DBIAS_HVT_V << EFUSE_DIG_DBIAS_HVT_S)
#define EFUSE_DIG_DBIAS_HVT_V  0x0000001FU
#define EFUSE_DIG_DBIAS_HVT_S  9
/** EFUSE_DIG_LDO_SLP_DBIAS2 : R; bitpos: [20:14]; default: 0;
 *  BLOCK2 DIG_LDO_DBG0_DBIAS2
 */
#define EFUSE_DIG_LDO_SLP_DBIAS2    0x0000007FU
#define EFUSE_DIG_LDO_SLP_DBIAS2_M  (EFUSE_DIG_LDO_SLP_DBIAS2_V << EFUSE_DIG_LDO_SLP_DBIAS2_S)
#define EFUSE_DIG_LDO_SLP_DBIAS2_V  0x0000007FU
#define EFUSE_DIG_LDO_SLP_DBIAS2_S  14
/** EFUSE_DIG_LDO_SLP_DBIAS26 : R; bitpos: [28:21]; default: 0;
 *  BLOCK2 DIG_LDO_DBG0_DBIAS26
 */
#define EFUSE_DIG_LDO_SLP_DBIAS26    0x000000FFU
#define EFUSE_DIG_LDO_SLP_DBIAS26_M  (EFUSE_DIG_LDO_SLP_DBIAS26_V << EFUSE_DIG_LDO_SLP_DBIAS26_S)
#define EFUSE_DIG_LDO_SLP_DBIAS26_V  0x000000FFU
#define EFUSE_DIG_LDO_SLP_DBIAS26_S  21
/** EFUSE_DIG_LDO_ACT_DBIAS26 : R; bitpos: [31:29]; default: 0;
 *  BLOCK2 DIG_LDO_ACT_DBIAS26
 */
#define EFUSE_DIG_LDO_ACT_DBIAS26    0x00000007U
#define EFUSE_DIG_LDO_ACT_DBIAS26_M  (EFUSE_DIG_LDO_ACT_DBIAS26_V << EFUSE_DIG_LDO_ACT_DBIAS26_S)
#define EFUSE_DIG_LDO_ACT_DBIAS26_V  0x00000007U
#define EFUSE_DIG_LDO_ACT_DBIAS26_S  29

/** EFUSE_RD_BLK2_DATA4_REG register
 *  Register 4 of BLOCK2.
 */
#define EFUSE_RD_BLK2_DATA4_REG (DR_REG_EFUSE_BASE + 0x50)
/** EFUSE_DIG_LDO_ACT_DBIAS26_1 : R; bitpos: [2:0]; default: 0;
 *  BLOCK2 DIG_LDO_ACT_DBIAS26
 */
#define EFUSE_DIG_LDO_ACT_DBIAS26_1    0x00000007U
#define EFUSE_DIG_LDO_ACT_DBIAS26_1_M  (EFUSE_DIG_LDO_ACT_DBIAS26_1_V << EFUSE_DIG_LDO_ACT_DBIAS26_1_S)
#define EFUSE_DIG_LDO_ACT_DBIAS26_1_V  0x00000007U
#define EFUSE_DIG_LDO_ACT_DBIAS26_1_S  0
/** EFUSE_DIG_LDO_ACT_STEPD10 : R; bitpos: [6:3]; default: 0;
 *  BLOCK2 DIG_LDO_ACT_STEPD10
 */
#define EFUSE_DIG_LDO_ACT_STEPD10    0x0000000FU
#define EFUSE_DIG_LDO_ACT_STEPD10_M  (EFUSE_DIG_LDO_ACT_STEPD10_V << EFUSE_DIG_LDO_ACT_STEPD10_S)
#define EFUSE_DIG_LDO_ACT_STEPD10_V  0x0000000FU
#define EFUSE_DIG_LDO_ACT_STEPD10_S  3
/** EFUSE_RTC_LDO_SLP_DBIAS13 : R; bitpos: [13:7]; default: 0;
 *  BLOCK2 DIG_LDO_SLP_DBIAS13
 */
#define EFUSE_RTC_LDO_SLP_DBIAS13    0x0000007FU
#define EFUSE_RTC_LDO_SLP_DBIAS13_M  (EFUSE_RTC_LDO_SLP_DBIAS13_V << EFUSE_RTC_LDO_SLP_DBIAS13_S)
#define EFUSE_RTC_LDO_SLP_DBIAS13_V  0x0000007FU
#define EFUSE_RTC_LDO_SLP_DBIAS13_S  7
/** EFUSE_RTC_LDO_SLP_DBIAS29 : R; bitpos: [22:14]; default: 0;
 *  BLOCK2 DIG_LDO_SLP_DBIAS29
 */
#define EFUSE_RTC_LDO_SLP_DBIAS29    0x000001FFU
#define EFUSE_RTC_LDO_SLP_DBIAS29_M  (EFUSE_RTC_LDO_SLP_DBIAS29_V << EFUSE_RTC_LDO_SLP_DBIAS29_S)
#define EFUSE_RTC_LDO_SLP_DBIAS29_V  0x000001FFU
#define EFUSE_RTC_LDO_SLP_DBIAS29_S  14
/** EFUSE_RTC_LDO_SLP_DBIAS31 : R; bitpos: [28:23]; default: 0;
 *  BLOCK2 DIG_LDO_SLP_DBIAS31
 */
#define EFUSE_RTC_LDO_SLP_DBIAS31    0x0000003FU
#define EFUSE_RTC_LDO_SLP_DBIAS31_M  (EFUSE_RTC_LDO_SLP_DBIAS31_V << EFUSE_RTC_LDO_SLP_DBIAS31_S)
#define EFUSE_RTC_LDO_SLP_DBIAS31_V  0x0000003FU
#define EFUSE_RTC_LDO_SLP_DBIAS31_S  23
/** EFUSE_RTC_LDO_ACT_DBIAS31 : R; bitpos: [31:29]; default: 0;
 *  BLOCK2 DIG_LDO_ACT_DBIAS31
 */
#define EFUSE_RTC_LDO_ACT_DBIAS31    0x00000007U
#define EFUSE_RTC_LDO_ACT_DBIAS31_M  (EFUSE_RTC_LDO_ACT_DBIAS31_V << EFUSE_RTC_LDO_ACT_DBIAS31_S)
#define EFUSE_RTC_LDO_ACT_DBIAS31_V  0x00000007U
#define EFUSE_RTC_LDO_ACT_DBIAS31_S  29

/** EFUSE_RD_BLK2_DATA5_REG register
 *  Register 5 of BLOCK2.
 */
#define EFUSE_RD_BLK2_DATA5_REG (DR_REG_EFUSE_BASE + 0x54)
/** EFUSE_RTC_LDO_ACT_DBIAS31_1 : R; bitpos: [2:0]; default: 0;
 *  BLOCK2 DIG_LDO_ACT_DBIAS31
 */
#define EFUSE_RTC_LDO_ACT_DBIAS31_1    0x00000007U
#define EFUSE_RTC_LDO_ACT_DBIAS31_1_M  (EFUSE_RTC_LDO_ACT_DBIAS31_1_V << EFUSE_RTC_LDO_ACT_DBIAS31_1_S)
#define EFUSE_RTC_LDO_ACT_DBIAS31_1_V  0x00000007U
#define EFUSE_RTC_LDO_ACT_DBIAS31_1_S  0
/** EFUSE_RTC_LDO_ACT_DBIAS13 : R; bitpos: [10:3]; default: 0;
 *  BLOCK2 DIG_LDO_ACT_DBIAS13
 */
#define EFUSE_RTC_LDO_ACT_DBIAS13    0x000000FFU
#define EFUSE_RTC_LDO_ACT_DBIAS13_M  (EFUSE_RTC_LDO_ACT_DBIAS13_V << EFUSE_RTC_LDO_ACT_DBIAS13_S)
#define EFUSE_RTC_LDO_ACT_DBIAS13_V  0x000000FFU
#define EFUSE_RTC_LDO_ACT_DBIAS13_S  3
/** EFUSE_RESERVED_2_171 : R; bitpos: [31:11]; default: 0;
 *  reserved
 */
#define EFUSE_RESERVED_2_171    0x001FFFFFU
#define EFUSE_RESERVED_2_171_M  (EFUSE_RESERVED_2_171_V << EFUSE_RESERVED_2_171_S)
#define EFUSE_RESERVED_2_171_V  0x001FFFFFU
#define EFUSE_RESERVED_2_171_S  11

/** EFUSE_RD_BLK2_DATA6_REG register
 *  Register 6 of BLOCK2.
 */
#define EFUSE_RD_BLK2_DATA6_REG (DR_REG_EFUSE_BASE + 0x58)
/** EFUSE_ADC_CALIBRATION_3 : RO; bitpos: [10:0]; default: 0;
 *  Store the bit [86:96] of ADC calibration data.
 */
#define EFUSE_ADC_CALIBRATION_3    0x000007FFU
#define EFUSE_ADC_CALIBRATION_3_M  (EFUSE_ADC_CALIBRATION_3_V << EFUSE_ADC_CALIBRATION_3_S)
#define EFUSE_ADC_CALIBRATION_3_V  0x000007FFU
#define EFUSE_ADC_CALIBRATION_3_S  0
/** EFUSE_BLK2_RESERVED_DATA_0 : RO; bitpos: [31:11]; default: 0;
 *  Store the bit [0:20] of block2 reserved data.
 */
#define EFUSE_BLK2_RESERVED_DATA_0    0x001FFFFFU
#define EFUSE_BLK2_RESERVED_DATA_0_M  (EFUSE_BLK2_RESERVED_DATA_0_V << EFUSE_BLK2_RESERVED_DATA_0_S)
#define EFUSE_BLK2_RESERVED_DATA_0_V  0x001FFFFFU
#define EFUSE_BLK2_RESERVED_DATA_0_S  11

/** EFUSE_RD_BLK2_DATA7_REG register
 *  Register 7 of BLOCK2.
 */
#define EFUSE_RD_BLK2_DATA7_REG (DR_REG_EFUSE_BASE + 0x5c)
/** EFUSE_BLK2_RESERVED_DATA_1 : RO; bitpos: [31:0]; default: 0;
 *  Store the bit [21:52] of block2 reserved data.
 */
#define EFUSE_BLK2_RESERVED_DATA_1    0xFFFFFFFFU
#define EFUSE_BLK2_RESERVED_DATA_1_M  (EFUSE_BLK2_RESERVED_DATA_1_V << EFUSE_BLK2_RESERVED_DATA_1_S)
#define EFUSE_BLK2_RESERVED_DATA_1_V  0xFFFFFFFFU
#define EFUSE_BLK2_RESERVED_DATA_1_S  0

/** EFUSE_RD_BLK3_DATA0_REG register
 *  Register 0 of BLOCK3.
 */
#define EFUSE_RD_BLK3_DATA0_REG (DR_REG_EFUSE_BASE + 0x60)
/** EFUSE_BLK3_DATA0 : RO; bitpos: [31:0]; default: 0;
 *  Store the first 32-bit of Block3.
 */
#define EFUSE_BLK3_DATA0    0xFFFFFFFFU
#define EFUSE_BLK3_DATA0_M  (EFUSE_BLK3_DATA0_V << EFUSE_BLK3_DATA0_S)
#define EFUSE_BLK3_DATA0_V  0xFFFFFFFFU
#define EFUSE_BLK3_DATA0_S  0

/** EFUSE_RD_BLK3_DATA1_REG register
 *  Register 1 of BLOCK3.
 */
#define EFUSE_RD_BLK3_DATA1_REG (DR_REG_EFUSE_BASE + 0x64)
/** EFUSE_BLK3_DATA1 : RO; bitpos: [31:0]; default: 0;
 *  Store the second 32-bit of Block3.
 */
#define EFUSE_BLK3_DATA1    0xFFFFFFFFU
#define EFUSE_BLK3_DATA1_M  (EFUSE_BLK3_DATA1_V << EFUSE_BLK3_DATA1_S)
#define EFUSE_BLK3_DATA1_V  0xFFFFFFFFU
#define EFUSE_BLK3_DATA1_S  0

/** EFUSE_RD_BLK3_DATA2_REG register
 *  Register 2 of BLOCK3.
 */
#define EFUSE_RD_BLK3_DATA2_REG (DR_REG_EFUSE_BASE + 0x68)
/** EFUSE_BLK3_DATA2 : RO; bitpos: [31:0]; default: 0;
 *  Store the third 32-bit of Block3.
 */
#define EFUSE_BLK3_DATA2    0xFFFFFFFFU
#define EFUSE_BLK3_DATA2_M  (EFUSE_BLK3_DATA2_V << EFUSE_BLK3_DATA2_S)
#define EFUSE_BLK3_DATA2_V  0xFFFFFFFFU
#define EFUSE_BLK3_DATA2_S  0

/** EFUSE_RD_BLK3_DATA3_REG register
 *  Register 3 of BLOCK3.
 */
#define EFUSE_RD_BLK3_DATA3_REG (DR_REG_EFUSE_BASE + 0x6c)
/** EFUSE_BLK3_DATA3 : RO; bitpos: [31:0]; default: 0;
 *  Store the fourth 32-bit of Block3.
 */
#define EFUSE_BLK3_DATA3    0xFFFFFFFFU
#define EFUSE_BLK3_DATA3_M  (EFUSE_BLK3_DATA3_V << EFUSE_BLK3_DATA3_S)
#define EFUSE_BLK3_DATA3_V  0xFFFFFFFFU
#define EFUSE_BLK3_DATA3_S  0

/** EFUSE_RD_BLK3_DATA4_REG register
 *  Register 4 of BLOCK3.
 */
#define EFUSE_RD_BLK3_DATA4_REG (DR_REG_EFUSE_BASE + 0x70)
/** EFUSE_BLK3_DATA4 : RO; bitpos: [31:0]; default: 0;
 *  Store the fifth 32-bit of Block3.
 */
#define EFUSE_BLK3_DATA4    0xFFFFFFFFU
#define EFUSE_BLK3_DATA4_M  (EFUSE_BLK3_DATA4_V << EFUSE_BLK3_DATA4_S)
#define EFUSE_BLK3_DATA4_V  0xFFFFFFFFU
#define EFUSE_BLK3_DATA4_S  0

/** EFUSE_RD_BLK3_DATA5_REG register
 *  Register 5 of BLOCK3.
 */
#define EFUSE_RD_BLK3_DATA5_REG (DR_REG_EFUSE_BASE + 0x74)
/** EFUSE_BLK3_DATA5 : RO; bitpos: [31:0]; default: 0;
 *  Store the sixth 32-bit of Block3.
 */
#define EFUSE_BLK3_DATA5    0xFFFFFFFFU
#define EFUSE_BLK3_DATA5_M  (EFUSE_BLK3_DATA5_V << EFUSE_BLK3_DATA5_S)
#define EFUSE_BLK3_DATA5_V  0xFFFFFFFFU
#define EFUSE_BLK3_DATA5_S  0

/** EFUSE_RD_BLK3_DATA6_REG register
 *  Register 6 of BLOCK3.
 */
#define EFUSE_RD_BLK3_DATA6_REG (DR_REG_EFUSE_BASE + 0x78)
/** EFUSE_BLK3_DATA6 : RO; bitpos: [31:0]; default: 0;
 *  Store the seventh 32-bit of Block3.
 */
#define EFUSE_BLK3_DATA6    0xFFFFFFFFU
#define EFUSE_BLK3_DATA6_M  (EFUSE_BLK3_DATA6_V << EFUSE_BLK3_DATA6_S)
#define EFUSE_BLK3_DATA6_V  0xFFFFFFFFU
#define EFUSE_BLK3_DATA6_S  0

/** EFUSE_RD_BLK3_DATA7_REG register
 *  Register 7 of BLOCK3.
 */
#define EFUSE_RD_BLK3_DATA7_REG (DR_REG_EFUSE_BASE + 0x7c)
/** EFUSE_BLK3_DATA7 : RO; bitpos: [31:0]; default: 0;
 *  Store the eighth 32-bit of Block3.
 */
#define EFUSE_BLK3_DATA7    0xFFFFFFFFU
#define EFUSE_BLK3_DATA7_M  (EFUSE_BLK3_DATA7_V << EFUSE_BLK3_DATA7_S)
#define EFUSE_BLK3_DATA7_V  0xFFFFFFFFU
#define EFUSE_BLK3_DATA7_S  0

/** EFUSE_RD_REPEAT_ERR_REG register
 *  Programming error record register 0 of BLOCK0.
 */
#define EFUSE_RD_REPEAT_ERR_REG (DR_REG_EFUSE_BASE + 0x80)
/** EFUSE_RD_DIS_ERR : RO; bitpos: [1:0]; default: 0;
 *  If any bit in RD_DIS is 1, then it indicates a programming error.
 */
#define EFUSE_RD_DIS_ERR    0x00000003U
#define EFUSE_RD_DIS_ERR_M  (EFUSE_RD_DIS_ERR_V << EFUSE_RD_DIS_ERR_S)
#define EFUSE_RD_DIS_ERR_V  0x00000003U
#define EFUSE_RD_DIS_ERR_S  0
/** EFUSE_WDT_DELAY_SEL_ERR : RO; bitpos: [3:2]; default: 0;
 *  If any bit in WDT_DELAY_SEL is 1, then it indicates a programming error.
 */
#define EFUSE_WDT_DELAY_SEL_ERR    0x00000003U
#define EFUSE_WDT_DELAY_SEL_ERR_M  (EFUSE_WDT_DELAY_SEL_ERR_V << EFUSE_WDT_DELAY_SEL_ERR_S)
#define EFUSE_WDT_DELAY_SEL_ERR_V  0x00000003U
#define EFUSE_WDT_DELAY_SEL_ERR_S  2
/** EFUSE_DIS_PAD_JTAG_ERR : RO; bitpos: [4]; default: 0;
 *  If any bit in DIS_PAD_JTAG is 1, then it indicates a programming error.
 */
#define EFUSE_DIS_PAD_JTAG_ERR    (BIT(4))
#define EFUSE_DIS_PAD_JTAG_ERR_M  (EFUSE_DIS_PAD_JTAG_ERR_V << EFUSE_DIS_PAD_JTAG_ERR_S)
#define EFUSE_DIS_PAD_JTAG_ERR_V  0x00000001U
#define EFUSE_DIS_PAD_JTAG_ERR_S  4
/** EFUSE_DIS_DOWNLOAD_ICACHE_ERR : RO; bitpos: [5]; default: 0;
 *  If any bit in this filed is 1, then it indicates a programming error.
 */
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR    (BIT(5))
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR_M  (EFUSE_DIS_DOWNLOAD_ICACHE_ERR_V << EFUSE_DIS_DOWNLOAD_ICACHE_ERR_S)
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR_V  0x00000001U
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR_S  5
/** EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR : RO; bitpos: [6]; default: 0;
 *  If any bit in DIS_DOWNLOAD_MANUAL_ENCRYPT is 1, then it indicates a programming
 *  error.
 */
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR    (BIT(6))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_M  (EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_V << EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_S)
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_V  0x00000001U
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_S  6
/** EFUSE_SPI_BOOT_ENCRYPT_DECRYPT_CNT_ERR : RO; bitpos: [9:7]; default: 0;
 *  If any bit in SPI_BOOT_ENCRYPT_DECRYPT_CNT is 1, then it indicates a programming
 *  error.
 */
#define EFUSE_SPI_BOOT_ENCRYPT_DECRYPT_CNT_ERR    0x00000007U
#define EFUSE_SPI_BOOT_ENCRYPT_DECRYPT_CNT_ERR_M  (EFUSE_SPI_BOOT_ENCRYPT_DECRYPT_CNT_ERR_V << EFUSE_SPI_BOOT_ENCRYPT_DECRYPT_CNT_ERR_S)
#define EFUSE_SPI_BOOT_ENCRYPT_DECRYPT_CNT_ERR_V  0x00000007U
#define EFUSE_SPI_BOOT_ENCRYPT_DECRYPT_CNT_ERR_S  7
/** EFUSE_XTS_KEY_LENGTH_256_ERR : RO; bitpos: [10]; default: 0;
 *  If any bit in XTS_KEY_LENGTH_256 is 1, then it indicates a programming error.
 */
#define EFUSE_XTS_KEY_LENGTH_256_ERR    (BIT(10))
#define EFUSE_XTS_KEY_LENGTH_256_ERR_M  (EFUSE_XTS_KEY_LENGTH_256_ERR_V << EFUSE_XTS_KEY_LENGTH_256_ERR_S)
#define EFUSE_XTS_KEY_LENGTH_256_ERR_V  0x00000001U
#define EFUSE_XTS_KEY_LENGTH_256_ERR_S  10
/** EFUSE_UART_PRINT_CONTROL_ERR : RO; bitpos: [12:11]; default: 0;
 *  If any bit in UART_PRINT_CONTROL is 1, then it indicates a programming error.
 */
#define EFUSE_UART_PRINT_CONTROL_ERR    0x00000003U
#define EFUSE_UART_PRINT_CONTROL_ERR_M  (EFUSE_UART_PRINT_CONTROL_ERR_V << EFUSE_UART_PRINT_CONTROL_ERR_S)
#define EFUSE_UART_PRINT_CONTROL_ERR_V  0x00000003U
#define EFUSE_UART_PRINT_CONTROL_ERR_S  11
/** EFUSE_FORCE_SEND_RESUME_ERR : RO; bitpos: [13]; default: 0;
 *  If any bit in FORCE_SEND_RESUME is 1, then it indicates a programming error.
 */
#define EFUSE_FORCE_SEND_RESUME_ERR    (BIT(13))
#define EFUSE_FORCE_SEND_RESUME_ERR_M  (EFUSE_FORCE_SEND_RESUME_ERR_V << EFUSE_FORCE_SEND_RESUME_ERR_S)
#define EFUSE_FORCE_SEND_RESUME_ERR_V  0x00000001U
#define EFUSE_FORCE_SEND_RESUME_ERR_S  13
/** EFUSE_DIS_DOWNLOAD_MODE_ERR : RO; bitpos: [14]; default: 0;
 *  If any bit in this filed is 1, then it indicates a programming error.
 */
#define EFUSE_DIS_DOWNLOAD_MODE_ERR    (BIT(14))
#define EFUSE_DIS_DOWNLOAD_MODE_ERR_M  (EFUSE_DIS_DOWNLOAD_MODE_ERR_V << EFUSE_DIS_DOWNLOAD_MODE_ERR_S)
#define EFUSE_DIS_DOWNLOAD_MODE_ERR_V  0x00000001U
#define EFUSE_DIS_DOWNLOAD_MODE_ERR_S  14
/** EFUSE_DIS_DIRECT_BOOT_ERR : RO; bitpos: [15]; default: 0;
 *  If any bit in this filed is 1, then it indicates a programming error.
 */
#define EFUSE_DIS_DIRECT_BOOT_ERR    (BIT(15))
#define EFUSE_DIS_DIRECT_BOOT_ERR_M  (EFUSE_DIS_DIRECT_BOOT_ERR_V << EFUSE_DIS_DIRECT_BOOT_ERR_S)
#define EFUSE_DIS_DIRECT_BOOT_ERR_V  0x00000001U
#define EFUSE_DIS_DIRECT_BOOT_ERR_S  15
/** EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR : RO; bitpos: [16]; default: 0;
 *  If any bit in this filed is 1, then it indicates a programming error.
 */
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR    (BIT(16))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_M  (EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_V << EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_S)
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_V  0x00000001U
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_S  16
/** EFUSE_FLASH_TPUW_ERR : RO; bitpos: [20:17]; default: 0;
 *  If any bit in this filed is 1, then it indicates a programming error.
 */
#define EFUSE_FLASH_TPUW_ERR    0x0000000FU
#define EFUSE_FLASH_TPUW_ERR_M  (EFUSE_FLASH_TPUW_ERR_V << EFUSE_FLASH_TPUW_ERR_S)
#define EFUSE_FLASH_TPUW_ERR_V  0x0000000FU
#define EFUSE_FLASH_TPUW_ERR_S  17
/** EFUSE_SECURE_BOOT_EN_ERR : RO; bitpos: [21]; default: 0;
 *  If any bit in this filed is 1, then it indicates a programming error.
 */
#define EFUSE_SECURE_BOOT_EN_ERR    (BIT(21))
#define EFUSE_SECURE_BOOT_EN_ERR_M  (EFUSE_SECURE_BOOT_EN_ERR_V << EFUSE_SECURE_BOOT_EN_ERR_S)
#define EFUSE_SECURE_BOOT_EN_ERR_V  0x00000001U
#define EFUSE_SECURE_BOOT_EN_ERR_S  21
/** EFUSE_RPT4_RESERVED_ERR : RO; bitpos: [31:22]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED_ERR    0x000003FFU
#define EFUSE_RPT4_RESERVED_ERR_M  (EFUSE_RPT4_RESERVED_ERR_V << EFUSE_RPT4_RESERVED_ERR_S)
#define EFUSE_RPT4_RESERVED_ERR_V  0x000003FFU
#define EFUSE_RPT4_RESERVED_ERR_S  22

/** EFUSE_RD_RS_ERR_REG register
 *  Programming error record register 0 of BLOCK1-10.
 */
#define EFUSE_RD_RS_ERR_REG (DR_REG_EFUSE_BASE + 0x84)
/** EFUSE_BLK1_ERR_NUM : RO; bitpos: [2:0]; default: 0;
 *  The value of this signal means the number of error bytes in block1.
 */
#define EFUSE_BLK1_ERR_NUM    0x00000007U
#define EFUSE_BLK1_ERR_NUM_M  (EFUSE_BLK1_ERR_NUM_V << EFUSE_BLK1_ERR_NUM_S)
#define EFUSE_BLK1_ERR_NUM_V  0x00000007U
#define EFUSE_BLK1_ERR_NUM_S  0
/** EFUSE_BLK1_FAIL : RO; bitpos: [3]; default: 0;
 *  0: Means no failure and that the data of block1 is reliable 1: Means that
 *  programming user data failed and the number of error bytes is over 6.
 */
#define EFUSE_BLK1_FAIL    (BIT(3))
#define EFUSE_BLK1_FAIL_M  (EFUSE_BLK1_FAIL_V << EFUSE_BLK1_FAIL_S)
#define EFUSE_BLK1_FAIL_V  0x00000001U
#define EFUSE_BLK1_FAIL_S  3
/** EFUSE_BLK2_ERR_NUM : RO; bitpos: [6:4]; default: 0;
 *  The value of this signal means the number of error bytes in block2.
 */
#define EFUSE_BLK2_ERR_NUM    0x00000007U
#define EFUSE_BLK2_ERR_NUM_M  (EFUSE_BLK2_ERR_NUM_V << EFUSE_BLK2_ERR_NUM_S)
#define EFUSE_BLK2_ERR_NUM_V  0x00000007U
#define EFUSE_BLK2_ERR_NUM_S  4
/** EFUSE_BLK2_FAIL : RO; bitpos: [7]; default: 0;
 *  0: Means no failure and that the data of block2 is reliable 1: Means that
 *  programming user data failed and the number of error bytes is over 6.
 */
#define EFUSE_BLK2_FAIL    (BIT(7))
#define EFUSE_BLK2_FAIL_M  (EFUSE_BLK2_FAIL_V << EFUSE_BLK2_FAIL_S)
#define EFUSE_BLK2_FAIL_V  0x00000001U
#define EFUSE_BLK2_FAIL_S  7
/** EFUSE_BLK3_ERR_NUM : RO; bitpos: [10:8]; default: 0;
 *  The value of this signal means the number of error bytes in block3.
 */
#define EFUSE_BLK3_ERR_NUM    0x00000007U
#define EFUSE_BLK3_ERR_NUM_M  (EFUSE_BLK3_ERR_NUM_V << EFUSE_BLK3_ERR_NUM_S)
#define EFUSE_BLK3_ERR_NUM_V  0x00000007U
#define EFUSE_BLK3_ERR_NUM_S  8
/** EFUSE_BLK3_FAIL : RO; bitpos: [11]; default: 0;
 *  0: Means no failure and that the block3 data is reliable 1: Means that programming
 *  user data failed and the number of error bytes is over 6.
 */
#define EFUSE_BLK3_FAIL    (BIT(11))
#define EFUSE_BLK3_FAIL_M  (EFUSE_BLK3_FAIL_V << EFUSE_BLK3_FAIL_S)
#define EFUSE_BLK3_FAIL_V  0x00000001U
#define EFUSE_BLK3_FAIL_S  11

/** EFUSE_CLK_REG register
 *  eFuse clcok configuration register.
 */
#define EFUSE_CLK_REG (DR_REG_EFUSE_BASE + 0x88)
/** EFUSE_EFUSE_MEM_FORCE_PD : R/W; bitpos: [0]; default: 0;
 *  Set this bit to force eFuse SRAM into power-saving mode.
 */
#define EFUSE_EFUSE_MEM_FORCE_PD    (BIT(0))
#define EFUSE_EFUSE_MEM_FORCE_PD_M  (EFUSE_EFUSE_MEM_FORCE_PD_V << EFUSE_EFUSE_MEM_FORCE_PD_S)
#define EFUSE_EFUSE_MEM_FORCE_PD_V  0x00000001U
#define EFUSE_EFUSE_MEM_FORCE_PD_S  0
/** EFUSE_MEM_CLK_FORCE_ON : R/W; bitpos: [1]; default: 1;
 *  Set this bit and force to activate clock signal of eFuse SRAM.
 */
#define EFUSE_MEM_CLK_FORCE_ON    (BIT(1))
#define EFUSE_MEM_CLK_FORCE_ON_M  (EFUSE_MEM_CLK_FORCE_ON_V << EFUSE_MEM_CLK_FORCE_ON_S)
#define EFUSE_MEM_CLK_FORCE_ON_V  0x00000001U
#define EFUSE_MEM_CLK_FORCE_ON_S  1
/** EFUSE_EFUSE_MEM_FORCE_PU : R/W; bitpos: [2]; default: 0;
 *  Set this bit to force eFuse SRAM into working mode.
 */
#define EFUSE_EFUSE_MEM_FORCE_PU    (BIT(2))
#define EFUSE_EFUSE_MEM_FORCE_PU_M  (EFUSE_EFUSE_MEM_FORCE_PU_V << EFUSE_EFUSE_MEM_FORCE_PU_S)
#define EFUSE_EFUSE_MEM_FORCE_PU_V  0x00000001U
#define EFUSE_EFUSE_MEM_FORCE_PU_S  2
/** EFUSE_CLK_EN : R/W; bitpos: [16]; default: 0;
 *  Set this bit and force to enable clock signal of eFuse memory.
 */
#define EFUSE_CLK_EN    (BIT(16))
#define EFUSE_CLK_EN_M  (EFUSE_CLK_EN_V << EFUSE_CLK_EN_S)
#define EFUSE_CLK_EN_V  0x00000001U
#define EFUSE_CLK_EN_S  16

/** EFUSE_CONF_REG register
 *  eFuse operation mode configuration register
 */
#define EFUSE_CONF_REG (DR_REG_EFUSE_BASE + 0x8c)
/** EFUSE_OP_CODE : R/W; bitpos: [15:0]; default: 0;
 *  0x5A5A: Operate programming command 0x5AA5: Operate read command.
 */
#define EFUSE_OP_CODE    0x0000FFFFU
#define EFUSE_OP_CODE_M  (EFUSE_OP_CODE_V << EFUSE_OP_CODE_S)
#define EFUSE_OP_CODE_V  0x0000FFFFU
#define EFUSE_OP_CODE_S  0

/** EFUSE_STATUS_REG register
 *  eFuse status register.
 */
#define EFUSE_STATUS_REG (DR_REG_EFUSE_BASE + 0x90)
/** EFUSE_STATE : RO; bitpos: [3:0]; default: 0;
 *  Indicates the state of the eFuse state machine.
 */
#define EFUSE_STATE    0x0000000FU
#define EFUSE_STATE_M  (EFUSE_STATE_V << EFUSE_STATE_S)
#define EFUSE_STATE_V  0x0000000FU
#define EFUSE_STATE_S  0
/** EFUSE_OTP_LOAD_SW : RO; bitpos: [4]; default: 0;
 *  The value of OTP_LOAD_SW.
 */
#define EFUSE_OTP_LOAD_SW    (BIT(4))
#define EFUSE_OTP_LOAD_SW_M  (EFUSE_OTP_LOAD_SW_V << EFUSE_OTP_LOAD_SW_S)
#define EFUSE_OTP_LOAD_SW_V  0x00000001U
#define EFUSE_OTP_LOAD_SW_S  4
/** EFUSE_OTP_VDDQ_C_SYNC2 : RO; bitpos: [5]; default: 0;
 *  The value of OTP_VDDQ_C_SYNC2.
 */
#define EFUSE_OTP_VDDQ_C_SYNC2    (BIT(5))
#define EFUSE_OTP_VDDQ_C_SYNC2_M  (EFUSE_OTP_VDDQ_C_SYNC2_V << EFUSE_OTP_VDDQ_C_SYNC2_S)
#define EFUSE_OTP_VDDQ_C_SYNC2_V  0x00000001U
#define EFUSE_OTP_VDDQ_C_SYNC2_S  5
/** EFUSE_OTP_STROBE_SW : RO; bitpos: [6]; default: 0;
 *  The value of OTP_STROBE_SW.
 */
#define EFUSE_OTP_STROBE_SW    (BIT(6))
#define EFUSE_OTP_STROBE_SW_M  (EFUSE_OTP_STROBE_SW_V << EFUSE_OTP_STROBE_SW_S)
#define EFUSE_OTP_STROBE_SW_V  0x00000001U
#define EFUSE_OTP_STROBE_SW_S  6
/** EFUSE_OTP_CSB_SW : RO; bitpos: [7]; default: 0;
 *  The value of OTP_CSB_SW.
 */
#define EFUSE_OTP_CSB_SW    (BIT(7))
#define EFUSE_OTP_CSB_SW_M  (EFUSE_OTP_CSB_SW_V << EFUSE_OTP_CSB_SW_S)
#define EFUSE_OTP_CSB_SW_V  0x00000001U
#define EFUSE_OTP_CSB_SW_S  7
/** EFUSE_OTP_PGENB_SW : RO; bitpos: [8]; default: 0;
 *  The value of OTP_PGENB_SW.
 */
#define EFUSE_OTP_PGENB_SW    (BIT(8))
#define EFUSE_OTP_PGENB_SW_M  (EFUSE_OTP_PGENB_SW_V << EFUSE_OTP_PGENB_SW_S)
#define EFUSE_OTP_PGENB_SW_V  0x00000001U
#define EFUSE_OTP_PGENB_SW_S  8
/** EFUSE_OTP_VDDQ_IS_SW : RO; bitpos: [9]; default: 0;
 *  The value of OTP_VDDQ_IS_SW.
 */
#define EFUSE_OTP_VDDQ_IS_SW    (BIT(9))
#define EFUSE_OTP_VDDQ_IS_SW_M  (EFUSE_OTP_VDDQ_IS_SW_V << EFUSE_OTP_VDDQ_IS_SW_S)
#define EFUSE_OTP_VDDQ_IS_SW_V  0x00000001U
#define EFUSE_OTP_VDDQ_IS_SW_S  9
/** EFUSE_BLK0_VALID_BIT_CNT : RO; bitpos: [15:10]; default: 0;
 *  Record the number of bit '1' in BLOCK0.
 */
#define EFUSE_BLK0_VALID_BIT_CNT    0x0000003FU
#define EFUSE_BLK0_VALID_BIT_CNT_M  (EFUSE_BLK0_VALID_BIT_CNT_V << EFUSE_BLK0_VALID_BIT_CNT_S)
#define EFUSE_BLK0_VALID_BIT_CNT_V  0x0000003FU
#define EFUSE_BLK0_VALID_BIT_CNT_S  10

/** EFUSE_CMD_REG register
 *  eFuse command register.
 */
#define EFUSE_CMD_REG (DR_REG_EFUSE_BASE + 0x94)
/** EFUSE_READ_CMD : R/W/SC; bitpos: [0]; default: 0;
 *  Set this bit to send read command.
 */
#define EFUSE_READ_CMD    (BIT(0))
#define EFUSE_READ_CMD_M  (EFUSE_READ_CMD_V << EFUSE_READ_CMD_S)
#define EFUSE_READ_CMD_V  0x00000001U
#define EFUSE_READ_CMD_S  0
/** EFUSE_PGM_CMD : R/W/SC; bitpos: [1]; default: 0;
 *  Set this bit to send programming command.
 */
#define EFUSE_PGM_CMD    (BIT(1))
#define EFUSE_PGM_CMD_M  (EFUSE_PGM_CMD_V << EFUSE_PGM_CMD_S)
#define EFUSE_PGM_CMD_V  0x00000001U
#define EFUSE_PGM_CMD_S  1
/** EFUSE_BLK_NUM : R/W; bitpos: [3:2]; default: 0;
 *  The serial number of the block to be programmed. Value 0-3 corresponds to block
 *  number 0-3, respectively.
 */
#define EFUSE_BLK_NUM    0x00000003U
#define EFUSE_BLK_NUM_M  (EFUSE_BLK_NUM_V << EFUSE_BLK_NUM_S)
#define EFUSE_BLK_NUM_V  0x00000003U
#define EFUSE_BLK_NUM_S  2

/** EFUSE_INT_RAW_REG register
 *  eFuse raw interrupt register.
 */
#define EFUSE_INT_RAW_REG (DR_REG_EFUSE_BASE + 0x98)
/** EFUSE_READ_DONE_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw bit signal for read_done interrupt.
 */
#define EFUSE_READ_DONE_INT_RAW    (BIT(0))
#define EFUSE_READ_DONE_INT_RAW_M  (EFUSE_READ_DONE_INT_RAW_V << EFUSE_READ_DONE_INT_RAW_S)
#define EFUSE_READ_DONE_INT_RAW_V  0x00000001U
#define EFUSE_READ_DONE_INT_RAW_S  0
/** EFUSE_PGM_DONE_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw bit signal for pgm_done interrupt.
 */
#define EFUSE_PGM_DONE_INT_RAW    (BIT(1))
#define EFUSE_PGM_DONE_INT_RAW_M  (EFUSE_PGM_DONE_INT_RAW_V << EFUSE_PGM_DONE_INT_RAW_S)
#define EFUSE_PGM_DONE_INT_RAW_V  0x00000001U
#define EFUSE_PGM_DONE_INT_RAW_S  1

/** EFUSE_INT_ST_REG register
 *  eFuse interrupt status register.
 */
#define EFUSE_INT_ST_REG (DR_REG_EFUSE_BASE + 0x9c)
/** EFUSE_READ_DONE_INT_ST : RO; bitpos: [0]; default: 0;
 *  The status signal for read_done interrupt.
 */
#define EFUSE_READ_DONE_INT_ST    (BIT(0))
#define EFUSE_READ_DONE_INT_ST_M  (EFUSE_READ_DONE_INT_ST_V << EFUSE_READ_DONE_INT_ST_S)
#define EFUSE_READ_DONE_INT_ST_V  0x00000001U
#define EFUSE_READ_DONE_INT_ST_S  0
/** EFUSE_PGM_DONE_INT_ST : RO; bitpos: [1]; default: 0;
 *  The status signal for pgm_done interrupt.
 */
#define EFUSE_PGM_DONE_INT_ST    (BIT(1))
#define EFUSE_PGM_DONE_INT_ST_M  (EFUSE_PGM_DONE_INT_ST_V << EFUSE_PGM_DONE_INT_ST_S)
#define EFUSE_PGM_DONE_INT_ST_V  0x00000001U
#define EFUSE_PGM_DONE_INT_ST_S  1

/** EFUSE_INT_ENA_REG register
 *  eFuse interrupt enable register.
 */
#define EFUSE_INT_ENA_REG (DR_REG_EFUSE_BASE + 0x100)
/** EFUSE_READ_DONE_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The enable signal for read_done interrupt.
 */
#define EFUSE_READ_DONE_INT_ENA    (BIT(0))
#define EFUSE_READ_DONE_INT_ENA_M  (EFUSE_READ_DONE_INT_ENA_V << EFUSE_READ_DONE_INT_ENA_S)
#define EFUSE_READ_DONE_INT_ENA_V  0x00000001U
#define EFUSE_READ_DONE_INT_ENA_S  0
/** EFUSE_PGM_DONE_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The enable signal for pgm_done interrupt.
 */
#define EFUSE_PGM_DONE_INT_ENA    (BIT(1))
#define EFUSE_PGM_DONE_INT_ENA_M  (EFUSE_PGM_DONE_INT_ENA_V << EFUSE_PGM_DONE_INT_ENA_S)
#define EFUSE_PGM_DONE_INT_ENA_V  0x00000001U
#define EFUSE_PGM_DONE_INT_ENA_S  1

/** EFUSE_INT_CLR_REG register
 *  eFuse interrupt clear register.
 */
#define EFUSE_INT_CLR_REG (DR_REG_EFUSE_BASE + 0x104)
/** EFUSE_READ_DONE_INT_CLR : WT; bitpos: [0]; default: 0;
 *  The clear signal for read_done interrupt.
 */
#define EFUSE_READ_DONE_INT_CLR    (BIT(0))
#define EFUSE_READ_DONE_INT_CLR_M  (EFUSE_READ_DONE_INT_CLR_V << EFUSE_READ_DONE_INT_CLR_S)
#define EFUSE_READ_DONE_INT_CLR_V  0x00000001U
#define EFUSE_READ_DONE_INT_CLR_S  0
/** EFUSE_PGM_DONE_INT_CLR : WT; bitpos: [1]; default: 0;
 *  The clear signal for pgm_done interrupt.
 */
#define EFUSE_PGM_DONE_INT_CLR    (BIT(1))
#define EFUSE_PGM_DONE_INT_CLR_M  (EFUSE_PGM_DONE_INT_CLR_V << EFUSE_PGM_DONE_INT_CLR_S)
#define EFUSE_PGM_DONE_INT_CLR_V  0x00000001U
#define EFUSE_PGM_DONE_INT_CLR_S  1

/** EFUSE_DAC_CONF_REG register
 *  Controls the eFuse programming voltage.
 */
#define EFUSE_DAC_CONF_REG (DR_REG_EFUSE_BASE + 0x108)
/** EFUSE_DAC_CLK_DIV : R/W; bitpos: [7:0]; default: 28;
 *  Controls the division factor of the rising clock of the programming voltage.
 */
#define EFUSE_DAC_CLK_DIV    0x000000FFU
#define EFUSE_DAC_CLK_DIV_M  (EFUSE_DAC_CLK_DIV_V << EFUSE_DAC_CLK_DIV_S)
#define EFUSE_DAC_CLK_DIV_V  0x000000FFU
#define EFUSE_DAC_CLK_DIV_S  0
/** EFUSE_DAC_CLK_PAD_SEL : R/W; bitpos: [8]; default: 0;
 *  Don't care.
 */
#define EFUSE_DAC_CLK_PAD_SEL    (BIT(8))
#define EFUSE_DAC_CLK_PAD_SEL_M  (EFUSE_DAC_CLK_PAD_SEL_V << EFUSE_DAC_CLK_PAD_SEL_S)
#define EFUSE_DAC_CLK_PAD_SEL_V  0x00000001U
#define EFUSE_DAC_CLK_PAD_SEL_S  8
/** EFUSE_DAC_NUM : R/W; bitpos: [16:9]; default: 255;
 *  Controls the rising period of the programming voltage.
 */
#define EFUSE_DAC_NUM    0x000000FFU
#define EFUSE_DAC_NUM_M  (EFUSE_DAC_NUM_V << EFUSE_DAC_NUM_S)
#define EFUSE_DAC_NUM_V  0x000000FFU
#define EFUSE_DAC_NUM_S  9
/** EFUSE_OE_CLR : R/W; bitpos: [17]; default: 0;
 *  Reduces the power supply of the programming voltage.
 */
#define EFUSE_OE_CLR    (BIT(17))
#define EFUSE_OE_CLR_M  (EFUSE_OE_CLR_V << EFUSE_OE_CLR_S)
#define EFUSE_OE_CLR_V  0x00000001U
#define EFUSE_OE_CLR_S  17

/** EFUSE_RD_TIM_CONF_REG register
 *  Configures read timing parameters.
 */
#define EFUSE_RD_TIM_CONF_REG (DR_REG_EFUSE_BASE + 0x10c)
/** EFUSE_THR_A : R/W; bitpos: [7:0]; default: 1;
 *  Configures hold time for efuse read.
 */
#define EFUSE_THR_A    0x000000FFU
#define EFUSE_THR_A_M  (EFUSE_THR_A_V << EFUSE_THR_A_S)
#define EFUSE_THR_A_V  0x000000FFU
#define EFUSE_THR_A_S  0
/** EFUSE_TRD : R/W; bitpos: [15:8]; default: 2;
 *  Configures pulse time for efuse read.
 */
#define EFUSE_TRD    0x000000FFU
#define EFUSE_TRD_M  (EFUSE_TRD_V << EFUSE_TRD_S)
#define EFUSE_TRD_V  0x000000FFU
#define EFUSE_TRD_S  8
/** EFUSE_TSUR_A : R/W; bitpos: [23:16]; default: 1;
 *  Configures setup time for efuse read.
 */
#define EFUSE_TSUR_A    0x000000FFU
#define EFUSE_TSUR_A_M  (EFUSE_TSUR_A_V << EFUSE_TSUR_A_S)
#define EFUSE_TSUR_A_V  0x000000FFU
#define EFUSE_TSUR_A_S  16
/** EFUSE_READ_INIT_NUM : R/W; bitpos: [31:24]; default: 18;
 *  Configures the initial read time of eFuse.
 */
#define EFUSE_READ_INIT_NUM    0x000000FFU
#define EFUSE_READ_INIT_NUM_M  (EFUSE_READ_INIT_NUM_V << EFUSE_READ_INIT_NUM_S)
#define EFUSE_READ_INIT_NUM_V  0x000000FFU
#define EFUSE_READ_INIT_NUM_S  24

/** EFUSE_WR_TIM_CONF0_REG register
 *  Configurarion register 0 of eFuse programming timing parameters.
 */
#define EFUSE_WR_TIM_CONF0_REG (DR_REG_EFUSE_BASE + 0x110)
/** EFUSE_THP_A : R/W; bitpos: [7:0]; default: 1;
 *  Configures hold time for efuse program.
 */
#define EFUSE_THP_A    0x000000FFU
#define EFUSE_THP_A_M  (EFUSE_THP_A_V << EFUSE_THP_A_S)
#define EFUSE_THP_A_V  0x000000FFU
#define EFUSE_THP_A_S  0
/** EFUSE_TPGM_INACTIVE : R/W; bitpos: [15:8]; default: 1;
 *  Configures pulse time for burning '0' bit.
 */
#define EFUSE_TPGM_INACTIVE    0x000000FFU
#define EFUSE_TPGM_INACTIVE_M  (EFUSE_TPGM_INACTIVE_V << EFUSE_TPGM_INACTIVE_S)
#define EFUSE_TPGM_INACTIVE_V  0x000000FFU
#define EFUSE_TPGM_INACTIVE_S  8
/** EFUSE_TPGM : R/W; bitpos: [31:16]; default: 200;
 *  Configures pulse time for burning '1' bit.
 */
#define EFUSE_TPGM    0x0000FFFFU
#define EFUSE_TPGM_M  (EFUSE_TPGM_V << EFUSE_TPGM_S)
#define EFUSE_TPGM_V  0x0000FFFFU
#define EFUSE_TPGM_S  16

/** EFUSE_WR_TIM_CONF1_REG register
 *  Configurarion register 1 of eFuse programming timing parameters.
 */
#define EFUSE_WR_TIM_CONF1_REG (DR_REG_EFUSE_BASE + 0x114)
/** EFUSE_TSUP_A : R/W; bitpos: [7:0]; default: 1;
 *  Configures setup time for efuse program.
 */
#define EFUSE_TSUP_A    0x000000FFU
#define EFUSE_TSUP_A_M  (EFUSE_TSUP_A_V << EFUSE_TSUP_A_S)
#define EFUSE_TSUP_A_V  0x000000FFU
#define EFUSE_TSUP_A_S  0
/** EFUSE_PWR_ON_NUM : R/W; bitpos: [23:8]; default: 12288;
 *  Configures the power up time for VDDQ.
 */
#define EFUSE_PWR_ON_NUM    0x0000FFFFU
#define EFUSE_PWR_ON_NUM_M  (EFUSE_PWR_ON_NUM_V << EFUSE_PWR_ON_NUM_S)
#define EFUSE_PWR_ON_NUM_V  0x0000FFFFU
#define EFUSE_PWR_ON_NUM_S  8

/** EFUSE_WR_TIM_CONF2_REG register
 *  Configurarion register 2 of eFuse programming timing parameters.
 */
#define EFUSE_WR_TIM_CONF2_REG (DR_REG_EFUSE_BASE + 0x118)
/** EFUSE_PWR_OFF_NUM : R/W; bitpos: [15:0]; default: 400;
 *  Configures the power outage time for VDDQ.
 */
#define EFUSE_PWR_OFF_NUM    0x0000FFFFU
#define EFUSE_PWR_OFF_NUM_M  (EFUSE_PWR_OFF_NUM_V << EFUSE_PWR_OFF_NUM_S)
#define EFUSE_PWR_OFF_NUM_V  0x0000FFFFU
#define EFUSE_PWR_OFF_NUM_S  0

/** EFUSE_DATE_REG register
 *  eFuse version register.
 */
#define EFUSE_DATE_REG (DR_REG_EFUSE_BASE + 0x1fc)
/** EFUSE_DATE : R/W; bitpos: [27:0]; default: 34636176;
 *  Stores eFuse version.
 */
#define EFUSE_DATE    0x0FFFFFFFU
#define EFUSE_DATE_M  (EFUSE_DATE_V << EFUSE_DATE_S)
#define EFUSE_DATE_V  0x0FFFFFFFU
#define EFUSE_DATE_S  0

#ifdef __cplusplus
}
#endif
