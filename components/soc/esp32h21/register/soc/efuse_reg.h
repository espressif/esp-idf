/**
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#include "soc/efuse_defs.h"
#ifdef __cplusplus
extern "C" {
#endif

/** EFUSE_PGM_DATA0_REG register
 *  Represents pgm_data0
 */
#define EFUSE_PGM_DATA0_REG (DR_REG_EFUSE_BASE + 0x0)
/** EFUSE_PGM_DATA_0 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 0th 32-bit data to be programmed.
 */
#define EFUSE_PGM_DATA_0    0xFFFFFFFFU
#define EFUSE_PGM_DATA_0_M  (EFUSE_PGM_DATA_0_V << EFUSE_PGM_DATA_0_S)
#define EFUSE_PGM_DATA_0_V  0xFFFFFFFFU
#define EFUSE_PGM_DATA_0_S  0

/** EFUSE_PGM_DATA1_REG register
 *  Represents pgm_data1
 */
#define EFUSE_PGM_DATA1_REG (DR_REG_EFUSE_BASE + 0x4)
/** EFUSE_PGM_DATA_1 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 1th 32-bit data to be programmed.
 */
#define EFUSE_PGM_DATA_1    0xFFFFFFFFU
#define EFUSE_PGM_DATA_1_M  (EFUSE_PGM_DATA_1_V << EFUSE_PGM_DATA_1_S)
#define EFUSE_PGM_DATA_1_V  0xFFFFFFFFU
#define EFUSE_PGM_DATA_1_S  0

/** EFUSE_PGM_DATA2_REG register
 *  Represents pgm_data2
 */
#define EFUSE_PGM_DATA2_REG (DR_REG_EFUSE_BASE + 0x8)
/** EFUSE_PGM_DATA_2 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 2th 32-bit data to be programmed.
 */
#define EFUSE_PGM_DATA_2    0xFFFFFFFFU
#define EFUSE_PGM_DATA_2_M  (EFUSE_PGM_DATA_2_V << EFUSE_PGM_DATA_2_S)
#define EFUSE_PGM_DATA_2_V  0xFFFFFFFFU
#define EFUSE_PGM_DATA_2_S  0

/** EFUSE_PGM_DATA3_REG register
 *  Represents pgm_data3
 */
#define EFUSE_PGM_DATA3_REG (DR_REG_EFUSE_BASE + 0xc)
/** EFUSE_PGM_DATA_3 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 3th 32-bit data to be programmed.
 */
#define EFUSE_PGM_DATA_3    0xFFFFFFFFU
#define EFUSE_PGM_DATA_3_M  (EFUSE_PGM_DATA_3_V << EFUSE_PGM_DATA_3_S)
#define EFUSE_PGM_DATA_3_V  0xFFFFFFFFU
#define EFUSE_PGM_DATA_3_S  0

/** EFUSE_PGM_DATA4_REG register
 *  Represents pgm_data4
 */
#define EFUSE_PGM_DATA4_REG (DR_REG_EFUSE_BASE + 0x10)
/** EFUSE_PGM_DATA_4 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 4th 32-bit data to be programmed.
 */
#define EFUSE_PGM_DATA_4    0xFFFFFFFFU
#define EFUSE_PGM_DATA_4_M  (EFUSE_PGM_DATA_4_V << EFUSE_PGM_DATA_4_S)
#define EFUSE_PGM_DATA_4_V  0xFFFFFFFFU
#define EFUSE_PGM_DATA_4_S  0

/** EFUSE_PGM_DATA5_REG register
 *  Represents pgm_data5
 */
#define EFUSE_PGM_DATA5_REG (DR_REG_EFUSE_BASE + 0x14)
/** EFUSE_PGM_DATA_5 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 5th 32-bit data to be programmed.
 */
#define EFUSE_PGM_DATA_5    0xFFFFFFFFU
#define EFUSE_PGM_DATA_5_M  (EFUSE_PGM_DATA_5_V << EFUSE_PGM_DATA_5_S)
#define EFUSE_PGM_DATA_5_V  0xFFFFFFFFU
#define EFUSE_PGM_DATA_5_S  0

/** EFUSE_PGM_DATA6_REG register
 *  Represents pgm_data6
 */
#define EFUSE_PGM_DATA6_REG (DR_REG_EFUSE_BASE + 0x18)
/** EFUSE_PGM_DATA_6 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 6th 32-bit data to be programmed.
 */
#define EFUSE_PGM_DATA_6    0xFFFFFFFFU
#define EFUSE_PGM_DATA_6_M  (EFUSE_PGM_DATA_6_V << EFUSE_PGM_DATA_6_S)
#define EFUSE_PGM_DATA_6_V  0xFFFFFFFFU
#define EFUSE_PGM_DATA_6_S  0

/** EFUSE_PGM_DATA7_REG register
 *  Represents pgm_data7
 */
#define EFUSE_PGM_DATA7_REG (DR_REG_EFUSE_BASE + 0x1c)
/** EFUSE_PGM_DATA_7 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 7th 32-bit data to be programmed.
 */
#define EFUSE_PGM_DATA_7    0xFFFFFFFFU
#define EFUSE_PGM_DATA_7_M  (EFUSE_PGM_DATA_7_V << EFUSE_PGM_DATA_7_S)
#define EFUSE_PGM_DATA_7_V  0xFFFFFFFFU
#define EFUSE_PGM_DATA_7_S  0

/** EFUSE_PGM_CHECK_VALUE0_REG register
 *  Represents pgm_check_value0
 */
#define EFUSE_PGM_CHECK_VALUE0_REG (DR_REG_EFUSE_BASE + 0x20)
/** EFUSE_PGM_RS_DATA_0 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 0th RS code to be programmed.
 */
#define EFUSE_PGM_RS_DATA_0    0xFFFFFFFFU
#define EFUSE_PGM_RS_DATA_0_M  (EFUSE_PGM_RS_DATA_0_V << EFUSE_PGM_RS_DATA_0_S)
#define EFUSE_PGM_RS_DATA_0_V  0xFFFFFFFFU
#define EFUSE_PGM_RS_DATA_0_S  0

/** EFUSE_PGM_CHECK_VALUE1_REG register
 *  Represents pgm_check_value1
 */
#define EFUSE_PGM_CHECK_VALUE1_REG (DR_REG_EFUSE_BASE + 0x24)
/** EFUSE_PGM_RS_DATA_1 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 1th RS code to be programmed.
 */
#define EFUSE_PGM_RS_DATA_1    0xFFFFFFFFU
#define EFUSE_PGM_RS_DATA_1_M  (EFUSE_PGM_RS_DATA_1_V << EFUSE_PGM_RS_DATA_1_S)
#define EFUSE_PGM_RS_DATA_1_V  0xFFFFFFFFU
#define EFUSE_PGM_RS_DATA_1_S  0

/** EFUSE_PGM_CHECK_VALUE2_REG register
 *  Represents pgm_check_value2
 */
#define EFUSE_PGM_CHECK_VALUE2_REG (DR_REG_EFUSE_BASE + 0x28)
/** EFUSE_PGM_RS_DATA_2 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 2th RS code to be programmed.
 */
#define EFUSE_PGM_RS_DATA_2    0xFFFFFFFFU
#define EFUSE_PGM_RS_DATA_2_M  (EFUSE_PGM_RS_DATA_2_V << EFUSE_PGM_RS_DATA_2_S)
#define EFUSE_PGM_RS_DATA_2_V  0xFFFFFFFFU
#define EFUSE_PGM_RS_DATA_2_S  0

/** EFUSE_RD_WR_DIS_REG register
 *  Represents rd_wr_dis
 */
#define EFUSE_RD_WR_DIS_REG (DR_REG_EFUSE_BASE + 0x2c)
/** EFUSE_WR_DIS : RO; bitpos: [31:0]; default: 0;
 *  Represents whether programming of individual eFuse memory bit is disabled or
 *  enabled.
 *  1: Disabled
 *  0: Enabled
 */
#define EFUSE_WR_DIS    0xFFFFFFFFU
#define EFUSE_WR_DIS_M  (EFUSE_WR_DIS_V << EFUSE_WR_DIS_S)
#define EFUSE_WR_DIS_V  0xFFFFFFFFU
#define EFUSE_WR_DIS_S  0

/** EFUSE_RD_REPEAT_DATA0_REG register
 *  Represents rd_repeat_data
 */
#define EFUSE_RD_REPEAT_DATA0_REG (DR_REG_EFUSE_BASE + 0x30)
/** EFUSE_RD_DIS : RO; bitpos: [6:0]; default: 0;
 *  Represents whether reading of individual eFuse block(block4~block10) is disabled or
 *  enabled.
 *  1: Disabled
 *  0: Enabled
 */
#define EFUSE_RD_DIS    0x0000007FU
#define EFUSE_RD_DIS_M  (EFUSE_RD_DIS_V << EFUSE_RD_DIS_S)
#define EFUSE_RD_DIS_V  0x0000007FU
#define EFUSE_RD_DIS_S  0
/** EFUSE_PVT_GLITCH_EN : RO; bitpos: [7]; default: 0;
 *  Represents whether to enable PVT power glitch monitor function.
 *  1: Enable.
 *  0: Disable
 */
#define EFUSE_PVT_GLITCH_EN    (BIT(7))
#define EFUSE_PVT_GLITCH_EN_M  (EFUSE_PVT_GLITCH_EN_V << EFUSE_PVT_GLITCH_EN_S)
#define EFUSE_PVT_GLITCH_EN_V  0x00000001U
#define EFUSE_PVT_GLITCH_EN_S  7
/** EFUSE_DIS_ICACHE : RO; bitpos: [8]; default: 0;
 *  Represents whether icache is disabled or enabled.
 *  1: Disabled
 *  0: Enabled
 */
#define EFUSE_DIS_ICACHE    (BIT(8))
#define EFUSE_DIS_ICACHE_M  (EFUSE_DIS_ICACHE_V << EFUSE_DIS_ICACHE_S)
#define EFUSE_DIS_ICACHE_V  0x00000001U
#define EFUSE_DIS_ICACHE_S  8
/** EFUSE_DIS_USB_JTAG : RO; bitpos: [9]; default: 0;
 *  Represents whether the USB-to-JTAG function in USB Serial/JTAG is disabled.
 *  1: Disabled
 *  0: Enabled
 */
#define EFUSE_DIS_USB_JTAG    (BIT(9))
#define EFUSE_DIS_USB_JTAG_M  (EFUSE_DIS_USB_JTAG_V << EFUSE_DIS_USB_JTAG_S)
#define EFUSE_DIS_USB_JTAG_V  0x00000001U
#define EFUSE_DIS_USB_JTAG_S  9
/** EFUSE_POWERGLITCH_EN : RO; bitpos: [10]; default: 0;
 *  Represents whether to enable power glitch function.
 */
#define EFUSE_POWERGLITCH_EN    (BIT(10))
#define EFUSE_POWERGLITCH_EN_M  (EFUSE_POWERGLITCH_EN_V << EFUSE_POWERGLITCH_EN_S)
#define EFUSE_POWERGLITCH_EN_V  0x00000001U
#define EFUSE_POWERGLITCH_EN_S  10
/** EFUSE_DIS_FORCE_DOWNLOAD : RO; bitpos: [12]; default: 0;
 *  Represents whether the function that forces chip into Download mode is disabled.
 *  1: Disabled
 *  0: Enabled
 */
#define EFUSE_DIS_FORCE_DOWNLOAD    (BIT(12))
#define EFUSE_DIS_FORCE_DOWNLOAD_M  (EFUSE_DIS_FORCE_DOWNLOAD_V << EFUSE_DIS_FORCE_DOWNLOAD_S)
#define EFUSE_DIS_FORCE_DOWNLOAD_V  0x00000001U
#define EFUSE_DIS_FORCE_DOWNLOAD_S  12
/** EFUSE_SPI_DOWNLOAD_MSPI_DIS : RO; bitpos: [13]; default: 0;
 *  Represents accessing MSPI flash/MSPI RAM by SYS AXI matrix is disabled during
 *  boot_mode_download.
 *  1: Disabled
 *  0: Enabled
 */
#define EFUSE_SPI_DOWNLOAD_MSPI_DIS    (BIT(13))
#define EFUSE_SPI_DOWNLOAD_MSPI_DIS_M  (EFUSE_SPI_DOWNLOAD_MSPI_DIS_V << EFUSE_SPI_DOWNLOAD_MSPI_DIS_S)
#define EFUSE_SPI_DOWNLOAD_MSPI_DIS_V  0x00000001U
#define EFUSE_SPI_DOWNLOAD_MSPI_DIS_S  13
/** EFUSE_DIS_TWAI : RO; bitpos: [14]; default: 0;
 *  Represents whether TWAI function is disabled.
 *  1: Disabled
 *  0: Enabled
 */
#define EFUSE_DIS_TWAI    (BIT(14))
#define EFUSE_DIS_TWAI_M  (EFUSE_DIS_TWAI_V << EFUSE_DIS_TWAI_S)
#define EFUSE_DIS_TWAI_V  0x00000001U
#define EFUSE_DIS_TWAI_S  14
/** EFUSE_JTAG_SEL_ENABLE : RO; bitpos: [15]; default: 0;
 *  Represents whether the selection of a JTAG signal source through the strapping pin
 *  value is enabled when both EFUSE_DIS_PAD_JTAG and EFUSE_DIS_USB_JTAG are configured
 *  to 0. For more information, please refer to Chapter Placeholder.
 *  1: Enabled
 *  0: Disabled
 */
#define EFUSE_JTAG_SEL_ENABLE    (BIT(15))
#define EFUSE_JTAG_SEL_ENABLE_M  (EFUSE_JTAG_SEL_ENABLE_V << EFUSE_JTAG_SEL_ENABLE_S)
#define EFUSE_JTAG_SEL_ENABLE_V  0x00000001U
#define EFUSE_JTAG_SEL_ENABLE_S  15
/** EFUSE_SOFT_DIS_JTAG : RO; bitpos: [18:16]; default: 0;
 *  Represents whether PAD JTAG is disabled in the soft way. It can be restarted via
 *  HMAC.
 *  Odd count of bits with a value of 1: Disabled
 *  Even count of bits with a value of 1: Enabled
 */
#define EFUSE_SOFT_DIS_JTAG    0x00000007U
#define EFUSE_SOFT_DIS_JTAG_M  (EFUSE_SOFT_DIS_JTAG_V << EFUSE_SOFT_DIS_JTAG_S)
#define EFUSE_SOFT_DIS_JTAG_V  0x00000007U
#define EFUSE_SOFT_DIS_JTAG_S  16
/** EFUSE_DIS_PAD_JTAG : RO; bitpos: [19]; default: 0;
 *  Represents whether PAD JTAG is disabled in the hard way (permanently).
 *  1: Disabled
 *  0: Enabled
 */
#define EFUSE_DIS_PAD_JTAG    (BIT(19))
#define EFUSE_DIS_PAD_JTAG_M  (EFUSE_DIS_PAD_JTAG_V << EFUSE_DIS_PAD_JTAG_S)
#define EFUSE_DIS_PAD_JTAG_V  0x00000001U
#define EFUSE_DIS_PAD_JTAG_S  19
/** EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT : RO; bitpos: [20]; default: 0;
 *  Represents whether flash encryption is disabled (except in SPI boot mode).
 *  1: Disabled
 *  0: Enabled
 */
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT    (BIT(20))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_M  (EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_V << EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_S)
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_V  0x00000001U
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_S  20
/** EFUSE_USB_EXCHG_PINS : RO; bitpos: [25]; default: 0;
 *  Represents whether the D+ and D- pins is exchanged.
 *  1: Exchanged
 *  0: Not exchanged
 */
#define EFUSE_USB_EXCHG_PINS    (BIT(25))
#define EFUSE_USB_EXCHG_PINS_M  (EFUSE_USB_EXCHG_PINS_V << EFUSE_USB_EXCHG_PINS_S)
#define EFUSE_USB_EXCHG_PINS_V  0x00000001U
#define EFUSE_USB_EXCHG_PINS_S  25
/** EFUSE_VDD_SPI_AS_GPIO : RO; bitpos: [26]; default: 0;
 *  Represents whether vdd spi pin is functioned as gpio.
 *  1: Functioned
 *  0: Not functioned
 */
#define EFUSE_VDD_SPI_AS_GPIO    (BIT(26))
#define EFUSE_VDD_SPI_AS_GPIO_M  (EFUSE_VDD_SPI_AS_GPIO_V << EFUSE_VDD_SPI_AS_GPIO_S)
#define EFUSE_VDD_SPI_AS_GPIO_V  0x00000001U
#define EFUSE_VDD_SPI_AS_GPIO_S  26
/** EFUSE_ECDSA_CURVE_MODE : RO; bitpos: [28:27]; default: 0;
 *  Represents the configuration of the curve of ECDSA calculation.
 *  0: Only enable P256
 *  1: Only enable P192
 *  2: Both enable P256 and P192
 *  3: Only enable P256
 */
#define EFUSE_ECDSA_CURVE_MODE    0x00000003U
#define EFUSE_ECDSA_CURVE_MODE_M  (EFUSE_ECDSA_CURVE_MODE_V << EFUSE_ECDSA_CURVE_MODE_S)
#define EFUSE_ECDSA_CURVE_MODE_V  0x00000003U
#define EFUSE_ECDSA_CURVE_MODE_S  27
/** EFUSE_ECC_FORCE_CONST_TIME : RO; bitpos: [29]; default: 0;
 *  Represents whether to permanently turn on ECC const-time mode.
 *  0: Disabled
 *  1: Enabled
 */
#define EFUSE_ECC_FORCE_CONST_TIME    (BIT(29))
#define EFUSE_ECC_FORCE_CONST_TIME_M  (EFUSE_ECC_FORCE_CONST_TIME_V << EFUSE_ECC_FORCE_CONST_TIME_S)
#define EFUSE_ECC_FORCE_CONST_TIME_V  0x00000001U
#define EFUSE_ECC_FORCE_CONST_TIME_S  29
/** EFUSE_XTS_DPA_PSEUDO_LEVEL : RO; bitpos: [31:30]; default: 0;
 *  Represents control method of xts pseudo-round anti-dpa attack function.
 *  0: Controlled by register
 *  1-3: The higher the value is, the more pseudo-rounds are inserted to the xts-aes
 *  calculation.
 */
#define EFUSE_XTS_DPA_PSEUDO_LEVEL    0x00000003U
#define EFUSE_XTS_DPA_PSEUDO_LEVEL_M  (EFUSE_XTS_DPA_PSEUDO_LEVEL_V << EFUSE_XTS_DPA_PSEUDO_LEVEL_S)
#define EFUSE_XTS_DPA_PSEUDO_LEVEL_V  0x00000003U
#define EFUSE_XTS_DPA_PSEUDO_LEVEL_S  30

/** EFUSE_RD_REPEAT_DATA1_REG register
 *  Represents rd_repeat_data
 */
#define EFUSE_RD_REPEAT_DATA1_REG (DR_REG_EFUSE_BASE + 0x34)
/** EFUSE_IO_LDO_ADJUST : RO; bitpos: [7:0]; default: 0;
 *  Represents configuration of IO LDO mode and voltage.
 */
#define EFUSE_IO_LDO_ADJUST    0x000000FFU
#define EFUSE_IO_LDO_ADJUST_M  (EFUSE_IO_LDO_ADJUST_V << EFUSE_IO_LDO_ADJUST_S)
#define EFUSE_IO_LDO_ADJUST_V  0x000000FFU
#define EFUSE_IO_LDO_ADJUST_S  0
/** EFUSE_VDD_SPI_LDO_ADJUST : RO; bitpos: [15:8]; default: 0;
 *  Represents configuration of FLASH LDO mode and voltage.
 */
#define EFUSE_VDD_SPI_LDO_ADJUST    0x000000FFU
#define EFUSE_VDD_SPI_LDO_ADJUST_M  (EFUSE_VDD_SPI_LDO_ADJUST_V << EFUSE_VDD_SPI_LDO_ADJUST_S)
#define EFUSE_VDD_SPI_LDO_ADJUST_V  0x000000FFU
#define EFUSE_VDD_SPI_LDO_ADJUST_S  8
/** EFUSE_WDT_DELAY_SEL : RO; bitpos: [17:16]; default: 0;
 *  Represents RTC watchdog timeout threshold.
 *  0：The originally configured STG0 threshold × 2
 *  1：The originally configured STG0 threshold × 4
 *  2：The originally configured STG0 threshold × 8
 *  3：The originally configured STG0 threshold × 16
 */
#define EFUSE_WDT_DELAY_SEL    0x00000003U
#define EFUSE_WDT_DELAY_SEL_M  (EFUSE_WDT_DELAY_SEL_V << EFUSE_WDT_DELAY_SEL_S)
#define EFUSE_WDT_DELAY_SEL_V  0x00000003U
#define EFUSE_WDT_DELAY_SEL_S  16
/** EFUSE_SPI_BOOT_CRYPT_CNT : RO; bitpos: [20:18]; default: 0;
 *  Represents whether SPI boot encryption/decryption is enabled.
 *  Odd count of bits with a value of 1: Enabled
 *  Even count of bits with a value of 1: Disabled
 */
#define EFUSE_SPI_BOOT_CRYPT_CNT    0x00000007U
#define EFUSE_SPI_BOOT_CRYPT_CNT_M  (EFUSE_SPI_BOOT_CRYPT_CNT_V << EFUSE_SPI_BOOT_CRYPT_CNT_S)
#define EFUSE_SPI_BOOT_CRYPT_CNT_V  0x00000007U
#define EFUSE_SPI_BOOT_CRYPT_CNT_S  18
/** EFUSE_SECURE_BOOT_KEY_REVOKE0 : RO; bitpos: [21]; default: 0;
 *  Represents whether revoking Secure Boot key 0 is enabled.
 *  1: Enabled
 *  0: Disabled
 */
#define EFUSE_SECURE_BOOT_KEY_REVOKE0    (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_M  (EFUSE_SECURE_BOOT_KEY_REVOKE0_V << EFUSE_SECURE_BOOT_KEY_REVOKE0_S)
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_V  0x00000001U
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_S  21
/** EFUSE_SECURE_BOOT_KEY_REVOKE1 : RO; bitpos: [22]; default: 0;
 *  Represents whether revoking Secure Boot key 1 is enabled.
 *  1: Enabled
 *  0: Disabled
 */
#define EFUSE_SECURE_BOOT_KEY_REVOKE1    (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_M  (EFUSE_SECURE_BOOT_KEY_REVOKE1_V << EFUSE_SECURE_BOOT_KEY_REVOKE1_S)
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_V  0x00000001U
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_S  22
/** EFUSE_SECURE_BOOT_KEY_REVOKE2 : RO; bitpos: [23]; default: 0;
 *  Represents whether revoking Secure Boot key 2 is enabled.
 *  1: Enabled
 *  0: Disabled
 */
#define EFUSE_SECURE_BOOT_KEY_REVOKE2    (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_M  (EFUSE_SECURE_BOOT_KEY_REVOKE2_V << EFUSE_SECURE_BOOT_KEY_REVOKE2_S)
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_V  0x00000001U
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_S  23
/** EFUSE_KEY_PURPOSE_0 : RO; bitpos: [27:24]; default: 0;
 *  Represents the purpose of Key0. See Table tab:efuse-key-purpose.
 */
#define EFUSE_KEY_PURPOSE_0    0x0000000FU
#define EFUSE_KEY_PURPOSE_0_M  (EFUSE_KEY_PURPOSE_0_V << EFUSE_KEY_PURPOSE_0_S)
#define EFUSE_KEY_PURPOSE_0_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_0_S  24
/** EFUSE_KEY_PURPOSE_1 : RO; bitpos: [31:28]; default: 0;
 *  Represents the purpose of Key1. See Table tab:efuse-key-purpose.
 */
#define EFUSE_KEY_PURPOSE_1    0x0000000FU
#define EFUSE_KEY_PURPOSE_1_M  (EFUSE_KEY_PURPOSE_1_V << EFUSE_KEY_PURPOSE_1_S)
#define EFUSE_KEY_PURPOSE_1_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_1_S  28

/** EFUSE_RD_REPEAT_DATA2_REG register
 *  Represents rd_repeat_data
 */
#define EFUSE_RD_REPEAT_DATA2_REG (DR_REG_EFUSE_BASE + 0x38)
/** EFUSE_KEY_PURPOSE_2 : RO; bitpos: [3:0]; default: 0;
 *  Represents the purpose of Key2. See Table tab:efuse-key-purpose.
 */
#define EFUSE_KEY_PURPOSE_2    0x0000000FU
#define EFUSE_KEY_PURPOSE_2_M  (EFUSE_KEY_PURPOSE_2_V << EFUSE_KEY_PURPOSE_2_S)
#define EFUSE_KEY_PURPOSE_2_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_2_S  0
/** EFUSE_KEY_PURPOSE_3 : RO; bitpos: [7:4]; default: 0;
 *  Represents the purpose of Key3. See Table tab:efuse-key-purpose.
 */
#define EFUSE_KEY_PURPOSE_3    0x0000000FU
#define EFUSE_KEY_PURPOSE_3_M  (EFUSE_KEY_PURPOSE_3_V << EFUSE_KEY_PURPOSE_3_S)
#define EFUSE_KEY_PURPOSE_3_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_3_S  4
/** EFUSE_KEY_PURPOSE_4 : RO; bitpos: [11:8]; default: 0;
 *  Represents the purpose of Key4. See Table tab:efuse-key-purpose.
 */
#define EFUSE_KEY_PURPOSE_4    0x0000000FU
#define EFUSE_KEY_PURPOSE_4_M  (EFUSE_KEY_PURPOSE_4_V << EFUSE_KEY_PURPOSE_4_S)
#define EFUSE_KEY_PURPOSE_4_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_4_S  8
/** EFUSE_KEY_PURPOSE_5 : RO; bitpos: [15:12]; default: 0;
 *  Represents the purpose of Key5. See Table tab:efuse-key-purpose.
 */
#define EFUSE_KEY_PURPOSE_5    0x0000000FU
#define EFUSE_KEY_PURPOSE_5_M  (EFUSE_KEY_PURPOSE_5_V << EFUSE_KEY_PURPOSE_5_S)
#define EFUSE_KEY_PURPOSE_5_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_5_S  12
/** EFUSE_SEC_DPA_LEVEL : RO; bitpos: [17:16]; default: 0;
 *  Represents the security level of anti-DPA attack. The level is adjusted by
 *  configuring the clock random frequency division mode.
 *  0: Security level is SEC_DPA_OFF
 *  1: Security level is SEC_DPA_LOW
 *  2: Security level is SEC_DPA_MIDDLE
 *  3: Security level is SEC_DPA_HIGH
 *  For more information, please refer to Chapter mod:sysreg > Section
 *  sec:sysreg-anti-dpa-attack-security-control.
 */
#define EFUSE_SEC_DPA_LEVEL    0x00000003U
#define EFUSE_SEC_DPA_LEVEL_M  (EFUSE_SEC_DPA_LEVEL_V << EFUSE_SEC_DPA_LEVEL_S)
#define EFUSE_SEC_DPA_LEVEL_V  0x00000003U
#define EFUSE_SEC_DPA_LEVEL_S  16
/** EFUSE_IO_LDO_1P8 : RO; bitpos: [18]; default: 0;
 *  Represents select IO LDO voltage to 1.8V or 3.3V.
 *  1: 1.8V
 *  0: 3.3V
 */
#define EFUSE_IO_LDO_1P8    (BIT(18))
#define EFUSE_IO_LDO_1P8_M  (EFUSE_IO_LDO_1P8_V << EFUSE_IO_LDO_1P8_S)
#define EFUSE_IO_LDO_1P8_V  0x00000001U
#define EFUSE_IO_LDO_1P8_S  18
/** EFUSE_CRYPT_DPA_ENABLE : RO; bitpos: [19]; default: 0;
 *  Represents whether defense against DPA attack is enabled.
 *  1: Enabled
 *  0: Disabled
 */
#define EFUSE_CRYPT_DPA_ENABLE    (BIT(19))
#define EFUSE_CRYPT_DPA_ENABLE_M  (EFUSE_CRYPT_DPA_ENABLE_V << EFUSE_CRYPT_DPA_ENABLE_S)
#define EFUSE_CRYPT_DPA_ENABLE_V  0x00000001U
#define EFUSE_CRYPT_DPA_ENABLE_S  19
/** EFUSE_SECURE_BOOT_EN : RO; bitpos: [20]; default: 0;
 *  Represents whether Secure Boot is enabled.
 *  1: Enabled
 *  0: Disabled
 */
#define EFUSE_SECURE_BOOT_EN    (BIT(20))
#define EFUSE_SECURE_BOOT_EN_M  (EFUSE_SECURE_BOOT_EN_V << EFUSE_SECURE_BOOT_EN_S)
#define EFUSE_SECURE_BOOT_EN_V  0x00000001U
#define EFUSE_SECURE_BOOT_EN_S  20
/** EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE : RO; bitpos: [21]; default: 0;
 *  Represents whether aggressive revocation of Secure Boot is enabled.
 *  1: Enabled
 *  0: Disabled
 */
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE    (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_M  (EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_V << EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_S)
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_V  0x00000001U
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_S  21
/** EFUSE_POWERGLITCH_EN1 : RO; bitpos: [26:22]; default: 0;
 *  Represents whether to enable power glitch function when chip power on.
 */
#define EFUSE_POWERGLITCH_EN1    0x0000001FU
#define EFUSE_POWERGLITCH_EN1_M  (EFUSE_POWERGLITCH_EN1_V << EFUSE_POWERGLITCH_EN1_S)
#define EFUSE_POWERGLITCH_EN1_V  0x0000001FU
#define EFUSE_POWERGLITCH_EN1_S  22
/** EFUSE_DCDC_CCM_EN : RO; bitpos: [27]; default: 0;
 *  Represents whether change DCDC to CCM mode.
 */
#define EFUSE_DCDC_CCM_EN    (BIT(27))
#define EFUSE_DCDC_CCM_EN_M  (EFUSE_DCDC_CCM_EN_V << EFUSE_DCDC_CCM_EN_S)
#define EFUSE_DCDC_CCM_EN_V  0x00000001U
#define EFUSE_DCDC_CCM_EN_S  27
/** EFUSE_FLASH_TPUW : RO; bitpos: [31:28]; default: 0;
 *  Represents the flash waiting time after power-up. Measurement unit: ms.
 *  When the value is less than 15, the waiting time is the programmed value.
 *  Otherwise, the waiting time is a fixed value, i.e. 30 ms.
 */
#define EFUSE_FLASH_TPUW    0x0000000FU
#define EFUSE_FLASH_TPUW_M  (EFUSE_FLASH_TPUW_V << EFUSE_FLASH_TPUW_S)
#define EFUSE_FLASH_TPUW_V  0x0000000FU
#define EFUSE_FLASH_TPUW_S  28

/** EFUSE_RD_REPEAT_DATA3_REG register
 *  Represents rd_repeat_data
 */
#define EFUSE_RD_REPEAT_DATA3_REG (DR_REG_EFUSE_BASE + 0x3c)
/** EFUSE_DIS_DOWNLOAD_MODE : RO; bitpos: [0]; default: 0;
 *  Represents whether all download modes are disabled.
 *  1: Disabled
 *  0: Enabled
 */
#define EFUSE_DIS_DOWNLOAD_MODE    (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_M  (EFUSE_DIS_DOWNLOAD_MODE_V << EFUSE_DIS_DOWNLOAD_MODE_S)
#define EFUSE_DIS_DOWNLOAD_MODE_V  0x00000001U
#define EFUSE_DIS_DOWNLOAD_MODE_S  0
/** EFUSE_DIS_DIRECT_BOOT : RO; bitpos: [1]; default: 0;
 *  Represents whether direct boot mode is disabled.
 *  1: Disabled
 *  0: Enabled
 */
#define EFUSE_DIS_DIRECT_BOOT    (BIT(1))
#define EFUSE_DIS_DIRECT_BOOT_M  (EFUSE_DIS_DIRECT_BOOT_V << EFUSE_DIS_DIRECT_BOOT_S)
#define EFUSE_DIS_DIRECT_BOOT_V  0x00000001U
#define EFUSE_DIS_DIRECT_BOOT_S  1
/** EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT : RO; bitpos: [2]; default: 0;
 *  Represents whether print from USB-Serial-JTAG during ROM boot is disabled.
 *  1: Disabled
 *  0: Enabled
 */
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT    (BIT(2))
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_M  (EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_V << EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_S)
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_V  0x00000001U
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_S  2
/** EFUSE_FLASH_LDO_EFUSE_SEL : RO; bitpos: [3]; default: 0;
 *  Represents whether to select efuse control flash ldo default voltage.
 *  1: efuse
 *  0: strapping
 */
#define EFUSE_FLASH_LDO_EFUSE_SEL    (BIT(3))
#define EFUSE_FLASH_LDO_EFUSE_SEL_M  (EFUSE_FLASH_LDO_EFUSE_SEL_V << EFUSE_FLASH_LDO_EFUSE_SEL_S)
#define EFUSE_FLASH_LDO_EFUSE_SEL_V  0x00000001U
#define EFUSE_FLASH_LDO_EFUSE_SEL_S  3
/** EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE : RO; bitpos: [4]; default: 0;
 *  Represents whether the USB-Serial-JTAG download function is disabled.
 *  1: Disabled
 *  0: Enabled
 */
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE    (BIT(4))
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_M  (EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_V << EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_S)
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_V  0x00000001U
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_S  4
/** EFUSE_ENABLE_SECURITY_DOWNLOAD : RO; bitpos: [5]; default: 0;
 *  Represents whether security download is enabled. Only UART is supported for
 *  download. Reading/writing RAM or registers is not supported (i.e. Stub download is
 *  not supported).
 *  1: Enabled
 *  0: Disabled
 */
#define EFUSE_ENABLE_SECURITY_DOWNLOAD    (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_M  (EFUSE_ENABLE_SECURITY_DOWNLOAD_V << EFUSE_ENABLE_SECURITY_DOWNLOAD_S)
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_V  0x00000001U
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_S  5
/** EFUSE_UART_PRINT_CONTROL : RO; bitpos: [7:6]; default: 0;
 *  Represents the type of UART printing.
 *  0: Force enable printing.
 *  1: Enable printing when GPIO8 is reset at low level.
 *  2: Enable printing when GPIO8 is reset at high level.
 *  3: Force disable printing.
 */
#define EFUSE_UART_PRINT_CONTROL    0x00000003U
#define EFUSE_UART_PRINT_CONTROL_M  (EFUSE_UART_PRINT_CONTROL_V << EFUSE_UART_PRINT_CONTROL_S)
#define EFUSE_UART_PRINT_CONTROL_V  0x00000003U
#define EFUSE_UART_PRINT_CONTROL_S  6
/** EFUSE_FORCE_SEND_RESUME : RO; bitpos: [8]; default: 0;
 *  Represents whether ROM code is forced to send a resume command during SPI boot.
 *  1: Forced
 *  0: Not forced
 */
#define EFUSE_FORCE_SEND_RESUME    (BIT(8))
#define EFUSE_FORCE_SEND_RESUME_M  (EFUSE_FORCE_SEND_RESUME_V << EFUSE_FORCE_SEND_RESUME_S)
#define EFUSE_FORCE_SEND_RESUME_V  0x00000001U
#define EFUSE_FORCE_SEND_RESUME_S  8
/** EFUSE_SECURE_VERSION : RO; bitpos: [24:9]; default: 0;
 *  Represents the security version used by ESP-IDF anti-rollback feature.
 */
#define EFUSE_SECURE_VERSION    0x0000FFFFU
#define EFUSE_SECURE_VERSION_M  (EFUSE_SECURE_VERSION_V << EFUSE_SECURE_VERSION_S)
#define EFUSE_SECURE_VERSION_V  0x0000FFFFU
#define EFUSE_SECURE_VERSION_S  9
/** EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE : RO; bitpos: [25]; default: 0;
 *  Represents whether FAST VERIFY ON WAKE is disabled when Secure Boot is enabled.
 *  1: Disabled
 *  0: Enabled
 */
#define EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE    (BIT(25))
#define EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_M  (EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_V << EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_S)
#define EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_V  0x00000001U
#define EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_S  25
/** EFUSE_HYS_EN_PAD0 : RO; bitpos: [31:26]; default: 0;
 *  Represents whether to enable the hysteresis function of pad 0-5.
 *  0: Disabled
 *  1: Enabled
 */
#define EFUSE_HYS_EN_PAD0    0x0000003FU
#define EFUSE_HYS_EN_PAD0_M  (EFUSE_HYS_EN_PAD0_V << EFUSE_HYS_EN_PAD0_S)
#define EFUSE_HYS_EN_PAD0_V  0x0000003FU
#define EFUSE_HYS_EN_PAD0_S  26

/** EFUSE_RD_REPEAT_DATA4_REG register
 *  Represents rd_repeat_data
 */
#define EFUSE_RD_REPEAT_DATA4_REG (DR_REG_EFUSE_BASE + 0x40)
/** EFUSE_HYS_EN_PAD1 : RO; bitpos: [21:0]; default: 0;
 *  Represents whether to enable the hysteresis function of pad 6-27.
 *  0: Disabled
 *  1: Enabled
 */
#define EFUSE_HYS_EN_PAD1    0x003FFFFFU
#define EFUSE_HYS_EN_PAD1_M  (EFUSE_HYS_EN_PAD1_V << EFUSE_HYS_EN_PAD1_S)
#define EFUSE_HYS_EN_PAD1_V  0x003FFFFFU
#define EFUSE_HYS_EN_PAD1_S  0
/** EFUSE_FLASH_LDO_POWER_SEL : RO; bitpos: [22]; default: 0;
 *  Represents which flash LDO is selected.
 *  0: FLASH LDO 1P8.
 *  1: FLASH LDO 1P2.
 */
#define EFUSE_FLASH_LDO_POWER_SEL    (BIT(22))
#define EFUSE_FLASH_LDO_POWER_SEL_M  (EFUSE_FLASH_LDO_POWER_SEL_V << EFUSE_FLASH_LDO_POWER_SEL_S)
#define EFUSE_FLASH_LDO_POWER_SEL_V  0x00000001U
#define EFUSE_FLASH_LDO_POWER_SEL_S  22

/** EFUSE_RD_MAC_SYS0_REG register
 *  Represents rd_mac_sys
 */
#define EFUSE_RD_MAC_SYS0_REG (DR_REG_EFUSE_BASE + 0x44)
/** EFUSE_MAC_0 : RO; bitpos: [31:0]; default: 0;
 *  Represents MAC address. Low 32-bit.
 */
#define EFUSE_MAC_0    0xFFFFFFFFU
#define EFUSE_MAC_0_M  (EFUSE_MAC_0_V << EFUSE_MAC_0_S)
#define EFUSE_MAC_0_V  0xFFFFFFFFU
#define EFUSE_MAC_0_S  0

/** EFUSE_RD_MAC_SYS1_REG register
 *  Represents rd_mac_sys
 */
#define EFUSE_RD_MAC_SYS1_REG (DR_REG_EFUSE_BASE + 0x48)
/** EFUSE_MAC_1 : RO; bitpos: [15:0]; default: 0;
 *  Represents MAC address. High 16-bit.
 */
#define EFUSE_MAC_1    0x0000FFFFU
#define EFUSE_MAC_1_M  (EFUSE_MAC_1_V << EFUSE_MAC_1_S)
#define EFUSE_MAC_1_V  0x0000FFFFU
#define EFUSE_MAC_1_S  0
/** EFUSE_MAC_EXT : RO; bitpos: [31:16]; default: 0;
 *  Represents the extended bits of MAC address.
 */
#define EFUSE_MAC_EXT    0x0000FFFFU
#define EFUSE_MAC_EXT_M  (EFUSE_MAC_EXT_V << EFUSE_MAC_EXT_S)
#define EFUSE_MAC_EXT_V  0x0000FFFFU
#define EFUSE_MAC_EXT_S  16

/** EFUSE_RD_MAC_SYS2_REG register
 *  Represents rd_mac_sys
 */
#define EFUSE_RD_MAC_SYS2_REG (DR_REG_EFUSE_BASE + 0x4c)
/** EFUSE_MAC_RESERVED_0 : RO; bitpos: [13:0]; default: 0;
 *  Reserved.
 */
#define EFUSE_MAC_RESERVED_0    0x00003FFFU
#define EFUSE_MAC_RESERVED_0_M  (EFUSE_MAC_RESERVED_0_V << EFUSE_MAC_RESERVED_0_S)
#define EFUSE_MAC_RESERVED_0_V  0x00003FFFU
#define EFUSE_MAC_RESERVED_0_S  0
/** EFUSE_MAC_RESERVED_1 : RO; bitpos: [31:14]; default: 0;
 *  Reserved.
 */
#define EFUSE_MAC_RESERVED_1    0x0003FFFFU
#define EFUSE_MAC_RESERVED_1_M  (EFUSE_MAC_RESERVED_1_V << EFUSE_MAC_RESERVED_1_S)
#define EFUSE_MAC_RESERVED_1_V  0x0003FFFFU
#define EFUSE_MAC_RESERVED_1_S  14

/** EFUSE_RD_MAC_SYS3_REG register
 *  Represents rd_mac_sys
 */
#define EFUSE_RD_MAC_SYS3_REG (DR_REG_EFUSE_BASE + 0x50)
/** EFUSE_MAC_RESERVED_2 : RO; bitpos: [3:0]; default: 0;
 *  Reserved.
 */
#define EFUSE_MAC_RESERVED_2    0x0000000FU
#define EFUSE_MAC_RESERVED_2_M  (EFUSE_MAC_RESERVED_2_V << EFUSE_MAC_RESERVED_2_S)
#define EFUSE_MAC_RESERVED_2_V  0x0000000FU
#define EFUSE_MAC_RESERVED_2_S  0
/** EFUSE_PVT_CELL_SELECT : RO; bitpos: [10:4]; default: 0;
 *  Represents the selection of Power glitch monitor PVT cell.
 */
#define EFUSE_PVT_CELL_SELECT    0x0000007FU
#define EFUSE_PVT_CELL_SELECT_M  (EFUSE_PVT_CELL_SELECT_V << EFUSE_PVT_CELL_SELECT_S)
#define EFUSE_PVT_CELL_SELECT_V  0x0000007FU
#define EFUSE_PVT_CELL_SELECT_S  4
/** EFUSE_MAC_RESERVED_3 : RO; bitpos: [17:11]; default: 0;
 *  Reserved.
 */
#define EFUSE_MAC_RESERVED_3    0x0000007FU
#define EFUSE_MAC_RESERVED_3_M  (EFUSE_MAC_RESERVED_3_V << EFUSE_MAC_RESERVED_3_S)
#define EFUSE_MAC_RESERVED_3_V  0x0000007FU
#define EFUSE_MAC_RESERVED_3_S  11
/** EFUSE_SYS_DATA_PART0_0 : RO; bitpos: [31:18]; default: 0;
 *  Represents the first 14-bit of zeroth part of system data.
 */
#define EFUSE_SYS_DATA_PART0_0    0x00003FFFU
#define EFUSE_SYS_DATA_PART0_0_M  (EFUSE_SYS_DATA_PART0_0_V << EFUSE_SYS_DATA_PART0_0_S)
#define EFUSE_SYS_DATA_PART0_0_V  0x00003FFFU
#define EFUSE_SYS_DATA_PART0_0_S  18

/** EFUSE_RD_MAC_SYS4_REG register
 *  Represents rd_mac_sys
 */
#define EFUSE_RD_MAC_SYS4_REG (DR_REG_EFUSE_BASE + 0x54)
/** EFUSE_PVT_LIMIT : RO; bitpos: [20:5]; default: 0;
 *  Represents the threshold of power glitch monitor.
 */
#define EFUSE_PVT_LIMIT    0x0000FFFFU
#define EFUSE_PVT_LIMIT_M  (EFUSE_PVT_LIMIT_V << EFUSE_PVT_LIMIT_S)
#define EFUSE_PVT_LIMIT_V  0x0000FFFFU
#define EFUSE_PVT_LIMIT_S  5
/** EFUSE_PVT_GLITCH_CHARGE_RESET : RO; bitpos: [21]; default: 0;
 *  Represents whether to trigger reset or charge pump when PVT power glitch happened.
 *  1:Trigger charge pump.
 *  0:Trigger reset
 */
#define EFUSE_PVT_GLITCH_CHARGE_RESET    (BIT(21))
#define EFUSE_PVT_GLITCH_CHARGE_RESET_M  (EFUSE_PVT_GLITCH_CHARGE_RESET_V << EFUSE_PVT_GLITCH_CHARGE_RESET_S)
#define EFUSE_PVT_GLITCH_CHARGE_RESET_V  0x00000001U
#define EFUSE_PVT_GLITCH_CHARGE_RESET_S  21
/** EFUSE_PVT_GLITCH_MODE : RO; bitpos: [23:22]; default: 0;
 *  Represents the configuration of glitch mode.
 */
#define EFUSE_PVT_GLITCH_MODE    0x00000003U
#define EFUSE_PVT_GLITCH_MODE_M  (EFUSE_PVT_GLITCH_MODE_V << EFUSE_PVT_GLITCH_MODE_S)
#define EFUSE_PVT_GLITCH_MODE_V  0x00000003U
#define EFUSE_PVT_GLITCH_MODE_S  22
/** EFUSE_PVT_PUMP_LIMIT : RO; bitpos: [31:24]; default: 0;
 *  Represents the configuration voltage monitor limit for charge pump.
 */
#define EFUSE_PVT_PUMP_LIMIT    0x000000FFU
#define EFUSE_PVT_PUMP_LIMIT_M  (EFUSE_PVT_PUMP_LIMIT_V << EFUSE_PVT_PUMP_LIMIT_S)
#define EFUSE_PVT_PUMP_LIMIT_V  0x000000FFU
#define EFUSE_PVT_PUMP_LIMIT_S  24

/** EFUSE_RD_MAC_SYS5_REG register
 *  Represents rd_mac_sys
 */
#define EFUSE_RD_MAC_SYS5_REG (DR_REG_EFUSE_BASE + 0x58)
/** EFUSE_PUMP_DRV : RO; bitpos: [3:0]; default: 0;
 *  Use to configure charge pump voltage gain.
 */
#define EFUSE_PUMP_DRV    0x0000000FU
#define EFUSE_PUMP_DRV_M  (EFUSE_PUMP_DRV_V << EFUSE_PUMP_DRV_S)
#define EFUSE_PUMP_DRV_V  0x0000000FU
#define EFUSE_PUMP_DRV_S  0
/** EFUSE_SYS_DATA_PART0_2 : RO; bitpos: [31:4]; default: 0;
 *  Represents the second 28-bit of zeroth part of system data.
 */
#define EFUSE_SYS_DATA_PART0_2    0x0FFFFFFFU
#define EFUSE_SYS_DATA_PART0_2_M  (EFUSE_SYS_DATA_PART0_2_V << EFUSE_SYS_DATA_PART0_2_S)
#define EFUSE_SYS_DATA_PART0_2_V  0x0FFFFFFFU
#define EFUSE_SYS_DATA_PART0_2_S  4

/** EFUSE_RD_SYS_PART1_DATA0_REG register
 *  Represents rd_sys_part1_data0
 */
#define EFUSE_RD_SYS_PART1_DATA0_REG (DR_REG_EFUSE_BASE + 0x5c)
/** EFUSE_SYS_DATA_PART1_0 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of first part of system data.
 */
#define EFUSE_SYS_DATA_PART1_0    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART1_0_M  (EFUSE_SYS_DATA_PART1_0_V << EFUSE_SYS_DATA_PART1_0_S)
#define EFUSE_SYS_DATA_PART1_0_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART1_0_S  0

/** EFUSE_RD_SYS_PART1_DATA1_REG register
 *  Represents rd_sys_part1_data1
 */
#define EFUSE_RD_SYS_PART1_DATA1_REG (DR_REG_EFUSE_BASE + 0x60)
/** EFUSE_SYS_DATA_PART1_1 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of first part of system data.
 */
#define EFUSE_SYS_DATA_PART1_1    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART1_1_M  (EFUSE_SYS_DATA_PART1_1_V << EFUSE_SYS_DATA_PART1_1_S)
#define EFUSE_SYS_DATA_PART1_1_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART1_1_S  0

/** EFUSE_RD_SYS_PART1_DATA2_REG register
 *  Represents rd_sys_part1_data2
 */
#define EFUSE_RD_SYS_PART1_DATA2_REG (DR_REG_EFUSE_BASE + 0x64)
/** EFUSE_SYS_DATA_PART1_2 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of first part of system data.
 */
#define EFUSE_SYS_DATA_PART1_2    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART1_2_M  (EFUSE_SYS_DATA_PART1_2_V << EFUSE_SYS_DATA_PART1_2_S)
#define EFUSE_SYS_DATA_PART1_2_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART1_2_S  0

/** EFUSE_RD_SYS_PART1_DATA3_REG register
 *  Represents rd_sys_part1_data3
 */
#define EFUSE_RD_SYS_PART1_DATA3_REG (DR_REG_EFUSE_BASE + 0x68)
/** EFUSE_SYS_DATA_PART1_3 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of first part of system data.
 */
#define EFUSE_SYS_DATA_PART1_3    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART1_3_M  (EFUSE_SYS_DATA_PART1_3_V << EFUSE_SYS_DATA_PART1_3_S)
#define EFUSE_SYS_DATA_PART1_3_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART1_3_S  0

/** EFUSE_RD_SYS_PART1_DATA4_REG register
 *  Represents rd_sys_part1_data4
 */
#define EFUSE_RD_SYS_PART1_DATA4_REG (DR_REG_EFUSE_BASE + 0x6c)
/** EFUSE_SYS_DATA_PART1_4 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of first part of system data.
 */
#define EFUSE_SYS_DATA_PART1_4    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART1_4_M  (EFUSE_SYS_DATA_PART1_4_V << EFUSE_SYS_DATA_PART1_4_S)
#define EFUSE_SYS_DATA_PART1_4_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART1_4_S  0

/** EFUSE_RD_SYS_PART1_DATA5_REG register
 *  Represents rd_sys_part1_data5
 */
#define EFUSE_RD_SYS_PART1_DATA5_REG (DR_REG_EFUSE_BASE + 0x70)
/** EFUSE_SYS_DATA_PART1_5 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of first part of system data.
 */
#define EFUSE_SYS_DATA_PART1_5    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART1_5_M  (EFUSE_SYS_DATA_PART1_5_V << EFUSE_SYS_DATA_PART1_5_S)
#define EFUSE_SYS_DATA_PART1_5_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART1_5_S  0

/** EFUSE_RD_SYS_PART1_DATA6_REG register
 *  Represents rd_sys_part1_data6
 */
#define EFUSE_RD_SYS_PART1_DATA6_REG (DR_REG_EFUSE_BASE + 0x74)
/** EFUSE_SYS_DATA_PART1_6 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of first part of system data.
 */
#define EFUSE_SYS_DATA_PART1_6    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART1_6_M  (EFUSE_SYS_DATA_PART1_6_V << EFUSE_SYS_DATA_PART1_6_S)
#define EFUSE_SYS_DATA_PART1_6_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART1_6_S  0

/** EFUSE_RD_SYS_PART1_DATA7_REG register
 *  Represents rd_sys_part1_data7
 */
#define EFUSE_RD_SYS_PART1_DATA7_REG (DR_REG_EFUSE_BASE + 0x78)
/** EFUSE_SYS_DATA_PART1_7 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of first part of system data.
 */
#define EFUSE_SYS_DATA_PART1_7    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART1_7_M  (EFUSE_SYS_DATA_PART1_7_V << EFUSE_SYS_DATA_PART1_7_S)
#define EFUSE_SYS_DATA_PART1_7_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART1_7_S  0

/** EFUSE_RD_USR_DATA0_REG register
 *  Represents rd_usr_data0
 */
#define EFUSE_RD_USR_DATA0_REG (DR_REG_EFUSE_BASE + 0x7c)
/** EFUSE_USR_DATA0 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of block3 (user).
 */
#define EFUSE_USR_DATA0    0xFFFFFFFFU
#define EFUSE_USR_DATA0_M  (EFUSE_USR_DATA0_V << EFUSE_USR_DATA0_S)
#define EFUSE_USR_DATA0_V  0xFFFFFFFFU
#define EFUSE_USR_DATA0_S  0

/** EFUSE_RD_USR_DATA1_REG register
 *  Represents rd_usr_data1
 */
#define EFUSE_RD_USR_DATA1_REG (DR_REG_EFUSE_BASE + 0x80)
/** EFUSE_USR_DATA1 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of block3 (user).
 */
#define EFUSE_USR_DATA1    0xFFFFFFFFU
#define EFUSE_USR_DATA1_M  (EFUSE_USR_DATA1_V << EFUSE_USR_DATA1_S)
#define EFUSE_USR_DATA1_V  0xFFFFFFFFU
#define EFUSE_USR_DATA1_S  0

/** EFUSE_RD_USR_DATA2_REG register
 *  Represents rd_usr_data2
 */
#define EFUSE_RD_USR_DATA2_REG (DR_REG_EFUSE_BASE + 0x84)
/** EFUSE_USR_DATA2 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of block3 (user).
 */
#define EFUSE_USR_DATA2    0xFFFFFFFFU
#define EFUSE_USR_DATA2_M  (EFUSE_USR_DATA2_V << EFUSE_USR_DATA2_S)
#define EFUSE_USR_DATA2_V  0xFFFFFFFFU
#define EFUSE_USR_DATA2_S  0

/** EFUSE_RD_USR_DATA3_REG register
 *  Represents rd_usr_data3
 */
#define EFUSE_RD_USR_DATA3_REG (DR_REG_EFUSE_BASE + 0x88)
/** EFUSE_USR_DATA3 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of block3 (user).
 */
#define EFUSE_USR_DATA3    0xFFFFFFFFU
#define EFUSE_USR_DATA3_M  (EFUSE_USR_DATA3_V << EFUSE_USR_DATA3_S)
#define EFUSE_USR_DATA3_V  0xFFFFFFFFU
#define EFUSE_USR_DATA3_S  0

/** EFUSE_RD_USR_DATA4_REG register
 *  Represents rd_usr_data4
 */
#define EFUSE_RD_USR_DATA4_REG (DR_REG_EFUSE_BASE + 0x8c)
/** EFUSE_USR_DATA4 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of block3 (user).
 */
#define EFUSE_USR_DATA4    0xFFFFFFFFU
#define EFUSE_USR_DATA4_M  (EFUSE_USR_DATA4_V << EFUSE_USR_DATA4_S)
#define EFUSE_USR_DATA4_V  0xFFFFFFFFU
#define EFUSE_USR_DATA4_S  0

/** EFUSE_RD_USR_DATA5_REG register
 *  Represents rd_usr_data5
 */
#define EFUSE_RD_USR_DATA5_REG (DR_REG_EFUSE_BASE + 0x90)
/** EFUSE_USR_DATA5 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of block3 (user).
 */
#define EFUSE_USR_DATA5    0xFFFFFFFFU
#define EFUSE_USR_DATA5_M  (EFUSE_USR_DATA5_V << EFUSE_USR_DATA5_S)
#define EFUSE_USR_DATA5_V  0xFFFFFFFFU
#define EFUSE_USR_DATA5_S  0

/** EFUSE_RD_USR_DATA6_REG register
 *  Represents rd_usr_data6
 */
#define EFUSE_RD_USR_DATA6_REG (DR_REG_EFUSE_BASE + 0x94)
/** EFUSE_USR_DATA6 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of block3 (user).
 */
#define EFUSE_USR_DATA6    0xFFFFFFFFU
#define EFUSE_USR_DATA6_M  (EFUSE_USR_DATA6_V << EFUSE_USR_DATA6_S)
#define EFUSE_USR_DATA6_V  0xFFFFFFFFU
#define EFUSE_USR_DATA6_S  0

/** EFUSE_RD_USR_DATA7_REG register
 *  Represents rd_usr_data7
 */
#define EFUSE_RD_USR_DATA7_REG (DR_REG_EFUSE_BASE + 0x98)
/** EFUSE_USR_DATA7 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of block3 (user).
 */
#define EFUSE_USR_DATA7    0xFFFFFFFFU
#define EFUSE_USR_DATA7_M  (EFUSE_USR_DATA7_V << EFUSE_USR_DATA7_S)
#define EFUSE_USR_DATA7_V  0xFFFFFFFFU
#define EFUSE_USR_DATA7_S  0

/** EFUSE_RD_KEY0_DATA0_REG register
 *  Represents rd_key0_data0
 */
#define EFUSE_RD_KEY0_DATA0_REG (DR_REG_EFUSE_BASE + 0x9c)
/** EFUSE_KEY0_DATA0 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key0.
 */
#define EFUSE_KEY0_DATA0    0xFFFFFFFFU
#define EFUSE_KEY0_DATA0_M  (EFUSE_KEY0_DATA0_V << EFUSE_KEY0_DATA0_S)
#define EFUSE_KEY0_DATA0_V  0xFFFFFFFFU
#define EFUSE_KEY0_DATA0_S  0

/** EFUSE_RD_KEY0_DATA1_REG register
 *  Represents rd_key0_data1
 */
#define EFUSE_RD_KEY0_DATA1_REG (DR_REG_EFUSE_BASE + 0xa0)
/** EFUSE_KEY0_DATA1 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key0.
 */
#define EFUSE_KEY0_DATA1    0xFFFFFFFFU
#define EFUSE_KEY0_DATA1_M  (EFUSE_KEY0_DATA1_V << EFUSE_KEY0_DATA1_S)
#define EFUSE_KEY0_DATA1_V  0xFFFFFFFFU
#define EFUSE_KEY0_DATA1_S  0

/** EFUSE_RD_KEY0_DATA2_REG register
 *  Represents rd_key0_data2
 */
#define EFUSE_RD_KEY0_DATA2_REG (DR_REG_EFUSE_BASE + 0xa4)
/** EFUSE_KEY0_DATA2 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key0.
 */
#define EFUSE_KEY0_DATA2    0xFFFFFFFFU
#define EFUSE_KEY0_DATA2_M  (EFUSE_KEY0_DATA2_V << EFUSE_KEY0_DATA2_S)
#define EFUSE_KEY0_DATA2_V  0xFFFFFFFFU
#define EFUSE_KEY0_DATA2_S  0

/** EFUSE_RD_KEY0_DATA3_REG register
 *  Represents rd_key0_data3
 */
#define EFUSE_RD_KEY0_DATA3_REG (DR_REG_EFUSE_BASE + 0xa8)
/** EFUSE_KEY0_DATA3 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key0.
 */
#define EFUSE_KEY0_DATA3    0xFFFFFFFFU
#define EFUSE_KEY0_DATA3_M  (EFUSE_KEY0_DATA3_V << EFUSE_KEY0_DATA3_S)
#define EFUSE_KEY0_DATA3_V  0xFFFFFFFFU
#define EFUSE_KEY0_DATA3_S  0

/** EFUSE_RD_KEY0_DATA4_REG register
 *  Represents rd_key0_data4
 */
#define EFUSE_RD_KEY0_DATA4_REG (DR_REG_EFUSE_BASE + 0xac)
/** EFUSE_KEY0_DATA4 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key0.
 */
#define EFUSE_KEY0_DATA4    0xFFFFFFFFU
#define EFUSE_KEY0_DATA4_M  (EFUSE_KEY0_DATA4_V << EFUSE_KEY0_DATA4_S)
#define EFUSE_KEY0_DATA4_V  0xFFFFFFFFU
#define EFUSE_KEY0_DATA4_S  0

/** EFUSE_RD_KEY0_DATA5_REG register
 *  Represents rd_key0_data5
 */
#define EFUSE_RD_KEY0_DATA5_REG (DR_REG_EFUSE_BASE + 0xb0)
/** EFUSE_KEY0_DATA5 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key0.
 */
#define EFUSE_KEY0_DATA5    0xFFFFFFFFU
#define EFUSE_KEY0_DATA5_M  (EFUSE_KEY0_DATA5_V << EFUSE_KEY0_DATA5_S)
#define EFUSE_KEY0_DATA5_V  0xFFFFFFFFU
#define EFUSE_KEY0_DATA5_S  0

/** EFUSE_RD_KEY0_DATA6_REG register
 *  Represents rd_key0_data6
 */
#define EFUSE_RD_KEY0_DATA6_REG (DR_REG_EFUSE_BASE + 0xb4)
/** EFUSE_KEY0_DATA6 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key0.
 */
#define EFUSE_KEY0_DATA6    0xFFFFFFFFU
#define EFUSE_KEY0_DATA6_M  (EFUSE_KEY0_DATA6_V << EFUSE_KEY0_DATA6_S)
#define EFUSE_KEY0_DATA6_V  0xFFFFFFFFU
#define EFUSE_KEY0_DATA6_S  0

/** EFUSE_RD_KEY0_DATA7_REG register
 *  Represents rd_key0_data7
 */
#define EFUSE_RD_KEY0_DATA7_REG (DR_REG_EFUSE_BASE + 0xb8)
/** EFUSE_KEY0_DATA7 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key0.
 */
#define EFUSE_KEY0_DATA7    0xFFFFFFFFU
#define EFUSE_KEY0_DATA7_M  (EFUSE_KEY0_DATA7_V << EFUSE_KEY0_DATA7_S)
#define EFUSE_KEY0_DATA7_V  0xFFFFFFFFU
#define EFUSE_KEY0_DATA7_S  0

/** EFUSE_RD_KEY1_DATA0_REG register
 *  Represents rd_key1_data0
 */
#define EFUSE_RD_KEY1_DATA0_REG (DR_REG_EFUSE_BASE + 0xbc)
/** EFUSE_KEY1_DATA0 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key1.
 */
#define EFUSE_KEY1_DATA0    0xFFFFFFFFU
#define EFUSE_KEY1_DATA0_M  (EFUSE_KEY1_DATA0_V << EFUSE_KEY1_DATA0_S)
#define EFUSE_KEY1_DATA0_V  0xFFFFFFFFU
#define EFUSE_KEY1_DATA0_S  0

/** EFUSE_RD_KEY1_DATA1_REG register
 *  Represents rd_key1_data1
 */
#define EFUSE_RD_KEY1_DATA1_REG (DR_REG_EFUSE_BASE + 0xc0)
/** EFUSE_KEY1_DATA1 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key1.
 */
#define EFUSE_KEY1_DATA1    0xFFFFFFFFU
#define EFUSE_KEY1_DATA1_M  (EFUSE_KEY1_DATA1_V << EFUSE_KEY1_DATA1_S)
#define EFUSE_KEY1_DATA1_V  0xFFFFFFFFU
#define EFUSE_KEY1_DATA1_S  0

/** EFUSE_RD_KEY1_DATA2_REG register
 *  Represents rd_key1_data2
 */
#define EFUSE_RD_KEY1_DATA2_REG (DR_REG_EFUSE_BASE + 0xc4)
/** EFUSE_KEY1_DATA2 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key1.
 */
#define EFUSE_KEY1_DATA2    0xFFFFFFFFU
#define EFUSE_KEY1_DATA2_M  (EFUSE_KEY1_DATA2_V << EFUSE_KEY1_DATA2_S)
#define EFUSE_KEY1_DATA2_V  0xFFFFFFFFU
#define EFUSE_KEY1_DATA2_S  0

/** EFUSE_RD_KEY1_DATA3_REG register
 *  Represents rd_key1_data3
 */
#define EFUSE_RD_KEY1_DATA3_REG (DR_REG_EFUSE_BASE + 0xc8)
/** EFUSE_KEY1_DATA3 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key1.
 */
#define EFUSE_KEY1_DATA3    0xFFFFFFFFU
#define EFUSE_KEY1_DATA3_M  (EFUSE_KEY1_DATA3_V << EFUSE_KEY1_DATA3_S)
#define EFUSE_KEY1_DATA3_V  0xFFFFFFFFU
#define EFUSE_KEY1_DATA3_S  0

/** EFUSE_RD_KEY1_DATA4_REG register
 *  Represents rd_key1_data4
 */
#define EFUSE_RD_KEY1_DATA4_REG (DR_REG_EFUSE_BASE + 0xcc)
/** EFUSE_KEY1_DATA4 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key1.
 */
#define EFUSE_KEY1_DATA4    0xFFFFFFFFU
#define EFUSE_KEY1_DATA4_M  (EFUSE_KEY1_DATA4_V << EFUSE_KEY1_DATA4_S)
#define EFUSE_KEY1_DATA4_V  0xFFFFFFFFU
#define EFUSE_KEY1_DATA4_S  0

/** EFUSE_RD_KEY1_DATA5_REG register
 *  Represents rd_key1_data5
 */
#define EFUSE_RD_KEY1_DATA5_REG (DR_REG_EFUSE_BASE + 0xd0)
/** EFUSE_KEY1_DATA5 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key1.
 */
#define EFUSE_KEY1_DATA5    0xFFFFFFFFU
#define EFUSE_KEY1_DATA5_M  (EFUSE_KEY1_DATA5_V << EFUSE_KEY1_DATA5_S)
#define EFUSE_KEY1_DATA5_V  0xFFFFFFFFU
#define EFUSE_KEY1_DATA5_S  0

/** EFUSE_RD_KEY1_DATA6_REG register
 *  Represents rd_key1_data6
 */
#define EFUSE_RD_KEY1_DATA6_REG (DR_REG_EFUSE_BASE + 0xd4)
/** EFUSE_KEY1_DATA6 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key1.
 */
#define EFUSE_KEY1_DATA6    0xFFFFFFFFU
#define EFUSE_KEY1_DATA6_M  (EFUSE_KEY1_DATA6_V << EFUSE_KEY1_DATA6_S)
#define EFUSE_KEY1_DATA6_V  0xFFFFFFFFU
#define EFUSE_KEY1_DATA6_S  0

/** EFUSE_RD_KEY1_DATA7_REG register
 *  Represents rd_key1_data7
 */
#define EFUSE_RD_KEY1_DATA7_REG (DR_REG_EFUSE_BASE + 0xd8)
/** EFUSE_KEY1_DATA7 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key1.
 */
#define EFUSE_KEY1_DATA7    0xFFFFFFFFU
#define EFUSE_KEY1_DATA7_M  (EFUSE_KEY1_DATA7_V << EFUSE_KEY1_DATA7_S)
#define EFUSE_KEY1_DATA7_V  0xFFFFFFFFU
#define EFUSE_KEY1_DATA7_S  0

/** EFUSE_RD_KEY2_DATA0_REG register
 *  Represents rd_key2_data0
 */
#define EFUSE_RD_KEY2_DATA0_REG (DR_REG_EFUSE_BASE + 0xdc)
/** EFUSE_KEY2_DATA0 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key2.
 */
#define EFUSE_KEY2_DATA0    0xFFFFFFFFU
#define EFUSE_KEY2_DATA0_M  (EFUSE_KEY2_DATA0_V << EFUSE_KEY2_DATA0_S)
#define EFUSE_KEY2_DATA0_V  0xFFFFFFFFU
#define EFUSE_KEY2_DATA0_S  0

/** EFUSE_RD_KEY2_DATA1_REG register
 *  Represents rd_key2_data1
 */
#define EFUSE_RD_KEY2_DATA1_REG (DR_REG_EFUSE_BASE + 0xe0)
/** EFUSE_KEY2_DATA1 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key2.
 */
#define EFUSE_KEY2_DATA1    0xFFFFFFFFU
#define EFUSE_KEY2_DATA1_M  (EFUSE_KEY2_DATA1_V << EFUSE_KEY2_DATA1_S)
#define EFUSE_KEY2_DATA1_V  0xFFFFFFFFU
#define EFUSE_KEY2_DATA1_S  0

/** EFUSE_RD_KEY2_DATA2_REG register
 *  Represents rd_key2_data2
 */
#define EFUSE_RD_KEY2_DATA2_REG (DR_REG_EFUSE_BASE + 0xe4)
/** EFUSE_KEY2_DATA2 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key2.
 */
#define EFUSE_KEY2_DATA2    0xFFFFFFFFU
#define EFUSE_KEY2_DATA2_M  (EFUSE_KEY2_DATA2_V << EFUSE_KEY2_DATA2_S)
#define EFUSE_KEY2_DATA2_V  0xFFFFFFFFU
#define EFUSE_KEY2_DATA2_S  0

/** EFUSE_RD_KEY2_DATA3_REG register
 *  Represents rd_key2_data3
 */
#define EFUSE_RD_KEY2_DATA3_REG (DR_REG_EFUSE_BASE + 0xe8)
/** EFUSE_KEY2_DATA3 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key2.
 */
#define EFUSE_KEY2_DATA3    0xFFFFFFFFU
#define EFUSE_KEY2_DATA3_M  (EFUSE_KEY2_DATA3_V << EFUSE_KEY2_DATA3_S)
#define EFUSE_KEY2_DATA3_V  0xFFFFFFFFU
#define EFUSE_KEY2_DATA3_S  0

/** EFUSE_RD_KEY2_DATA4_REG register
 *  Represents rd_key2_data4
 */
#define EFUSE_RD_KEY2_DATA4_REG (DR_REG_EFUSE_BASE + 0xec)
/** EFUSE_KEY2_DATA4 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key2.
 */
#define EFUSE_KEY2_DATA4    0xFFFFFFFFU
#define EFUSE_KEY2_DATA4_M  (EFUSE_KEY2_DATA4_V << EFUSE_KEY2_DATA4_S)
#define EFUSE_KEY2_DATA4_V  0xFFFFFFFFU
#define EFUSE_KEY2_DATA4_S  0

/** EFUSE_RD_KEY2_DATA5_REG register
 *  Represents rd_key2_data5
 */
#define EFUSE_RD_KEY2_DATA5_REG (DR_REG_EFUSE_BASE + 0xf0)
/** EFUSE_KEY2_DATA5 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key2.
 */
#define EFUSE_KEY2_DATA5    0xFFFFFFFFU
#define EFUSE_KEY2_DATA5_M  (EFUSE_KEY2_DATA5_V << EFUSE_KEY2_DATA5_S)
#define EFUSE_KEY2_DATA5_V  0xFFFFFFFFU
#define EFUSE_KEY2_DATA5_S  0

/** EFUSE_RD_KEY2_DATA6_REG register
 *  Represents rd_key2_data6
 */
#define EFUSE_RD_KEY2_DATA6_REG (DR_REG_EFUSE_BASE + 0xf4)
/** EFUSE_KEY2_DATA6 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key2.
 */
#define EFUSE_KEY2_DATA6    0xFFFFFFFFU
#define EFUSE_KEY2_DATA6_M  (EFUSE_KEY2_DATA6_V << EFUSE_KEY2_DATA6_S)
#define EFUSE_KEY2_DATA6_V  0xFFFFFFFFU
#define EFUSE_KEY2_DATA6_S  0

/** EFUSE_RD_KEY2_DATA7_REG register
 *  Represents rd_key2_data7
 */
#define EFUSE_RD_KEY2_DATA7_REG (DR_REG_EFUSE_BASE + 0xf8)
/** EFUSE_KEY2_DATA7 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key2.
 */
#define EFUSE_KEY2_DATA7    0xFFFFFFFFU
#define EFUSE_KEY2_DATA7_M  (EFUSE_KEY2_DATA7_V << EFUSE_KEY2_DATA7_S)
#define EFUSE_KEY2_DATA7_V  0xFFFFFFFFU
#define EFUSE_KEY2_DATA7_S  0

/** EFUSE_RD_KEY3_DATA0_REG register
 *  Represents rd_key3_data0
 */
#define EFUSE_RD_KEY3_DATA0_REG (DR_REG_EFUSE_BASE + 0xfc)
/** EFUSE_KEY3_DATA0 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key3.
 */
#define EFUSE_KEY3_DATA0    0xFFFFFFFFU
#define EFUSE_KEY3_DATA0_M  (EFUSE_KEY3_DATA0_V << EFUSE_KEY3_DATA0_S)
#define EFUSE_KEY3_DATA0_V  0xFFFFFFFFU
#define EFUSE_KEY3_DATA0_S  0

/** EFUSE_RD_KEY3_DATA1_REG register
 *  Represents rd_key3_data1
 */
#define EFUSE_RD_KEY3_DATA1_REG (DR_REG_EFUSE_BASE + 0x100)
/** EFUSE_KEY3_DATA1 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key3.
 */
#define EFUSE_KEY3_DATA1    0xFFFFFFFFU
#define EFUSE_KEY3_DATA1_M  (EFUSE_KEY3_DATA1_V << EFUSE_KEY3_DATA1_S)
#define EFUSE_KEY3_DATA1_V  0xFFFFFFFFU
#define EFUSE_KEY3_DATA1_S  0

/** EFUSE_RD_KEY3_DATA2_REG register
 *  Represents rd_key3_data2
 */
#define EFUSE_RD_KEY3_DATA2_REG (DR_REG_EFUSE_BASE + 0x104)
/** EFUSE_KEY3_DATA2 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key3.
 */
#define EFUSE_KEY3_DATA2    0xFFFFFFFFU
#define EFUSE_KEY3_DATA2_M  (EFUSE_KEY3_DATA2_V << EFUSE_KEY3_DATA2_S)
#define EFUSE_KEY3_DATA2_V  0xFFFFFFFFU
#define EFUSE_KEY3_DATA2_S  0

/** EFUSE_RD_KEY3_DATA3_REG register
 *  Represents rd_key3_data3
 */
#define EFUSE_RD_KEY3_DATA3_REG (DR_REG_EFUSE_BASE + 0x108)
/** EFUSE_KEY3_DATA3 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key3.
 */
#define EFUSE_KEY3_DATA3    0xFFFFFFFFU
#define EFUSE_KEY3_DATA3_M  (EFUSE_KEY3_DATA3_V << EFUSE_KEY3_DATA3_S)
#define EFUSE_KEY3_DATA3_V  0xFFFFFFFFU
#define EFUSE_KEY3_DATA3_S  0

/** EFUSE_RD_KEY3_DATA4_REG register
 *  Represents rd_key3_data4
 */
#define EFUSE_RD_KEY3_DATA4_REG (DR_REG_EFUSE_BASE + 0x10c)
/** EFUSE_KEY3_DATA4 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key3.
 */
#define EFUSE_KEY3_DATA4    0xFFFFFFFFU
#define EFUSE_KEY3_DATA4_M  (EFUSE_KEY3_DATA4_V << EFUSE_KEY3_DATA4_S)
#define EFUSE_KEY3_DATA4_V  0xFFFFFFFFU
#define EFUSE_KEY3_DATA4_S  0

/** EFUSE_RD_KEY3_DATA5_REG register
 *  Represents rd_key3_data5
 */
#define EFUSE_RD_KEY3_DATA5_REG (DR_REG_EFUSE_BASE + 0x110)
/** EFUSE_KEY3_DATA5 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key3.
 */
#define EFUSE_KEY3_DATA5    0xFFFFFFFFU
#define EFUSE_KEY3_DATA5_M  (EFUSE_KEY3_DATA5_V << EFUSE_KEY3_DATA5_S)
#define EFUSE_KEY3_DATA5_V  0xFFFFFFFFU
#define EFUSE_KEY3_DATA5_S  0

/** EFUSE_RD_KEY3_DATA6_REG register
 *  Represents rd_key3_data6
 */
#define EFUSE_RD_KEY3_DATA6_REG (DR_REG_EFUSE_BASE + 0x114)
/** EFUSE_KEY3_DATA6 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key3.
 */
#define EFUSE_KEY3_DATA6    0xFFFFFFFFU
#define EFUSE_KEY3_DATA6_M  (EFUSE_KEY3_DATA6_V << EFUSE_KEY3_DATA6_S)
#define EFUSE_KEY3_DATA6_V  0xFFFFFFFFU
#define EFUSE_KEY3_DATA6_S  0

/** EFUSE_RD_KEY3_DATA7_REG register
 *  Represents rd_key3_data7
 */
#define EFUSE_RD_KEY3_DATA7_REG (DR_REG_EFUSE_BASE + 0x118)
/** EFUSE_KEY3_DATA7 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key3.
 */
#define EFUSE_KEY3_DATA7    0xFFFFFFFFU
#define EFUSE_KEY3_DATA7_M  (EFUSE_KEY3_DATA7_V << EFUSE_KEY3_DATA7_S)
#define EFUSE_KEY3_DATA7_V  0xFFFFFFFFU
#define EFUSE_KEY3_DATA7_S  0

/** EFUSE_RD_KEY4_DATA0_REG register
 *  Represents rd_key4_data0
 */
#define EFUSE_RD_KEY4_DATA0_REG (DR_REG_EFUSE_BASE + 0x11c)
/** EFUSE_KEY4_DATA0 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key4.
 */
#define EFUSE_KEY4_DATA0    0xFFFFFFFFU
#define EFUSE_KEY4_DATA0_M  (EFUSE_KEY4_DATA0_V << EFUSE_KEY4_DATA0_S)
#define EFUSE_KEY4_DATA0_V  0xFFFFFFFFU
#define EFUSE_KEY4_DATA0_S  0

/** EFUSE_RD_KEY4_DATA1_REG register
 *  Represents rd_key4_data1
 */
#define EFUSE_RD_KEY4_DATA1_REG (DR_REG_EFUSE_BASE + 0x120)
/** EFUSE_KEY4_DATA1 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key4.
 */
#define EFUSE_KEY4_DATA1    0xFFFFFFFFU
#define EFUSE_KEY4_DATA1_M  (EFUSE_KEY4_DATA1_V << EFUSE_KEY4_DATA1_S)
#define EFUSE_KEY4_DATA1_V  0xFFFFFFFFU
#define EFUSE_KEY4_DATA1_S  0

/** EFUSE_RD_KEY4_DATA2_REG register
 *  Represents rd_key4_data2
 */
#define EFUSE_RD_KEY4_DATA2_REG (DR_REG_EFUSE_BASE + 0x124)
/** EFUSE_KEY4_DATA2 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key4.
 */
#define EFUSE_KEY4_DATA2    0xFFFFFFFFU
#define EFUSE_KEY4_DATA2_M  (EFUSE_KEY4_DATA2_V << EFUSE_KEY4_DATA2_S)
#define EFUSE_KEY4_DATA2_V  0xFFFFFFFFU
#define EFUSE_KEY4_DATA2_S  0

/** EFUSE_RD_KEY4_DATA3_REG register
 *  Represents rd_key4_data3
 */
#define EFUSE_RD_KEY4_DATA3_REG (DR_REG_EFUSE_BASE + 0x128)
/** EFUSE_KEY4_DATA3 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key4.
 */
#define EFUSE_KEY4_DATA3    0xFFFFFFFFU
#define EFUSE_KEY4_DATA3_M  (EFUSE_KEY4_DATA3_V << EFUSE_KEY4_DATA3_S)
#define EFUSE_KEY4_DATA3_V  0xFFFFFFFFU
#define EFUSE_KEY4_DATA3_S  0

/** EFUSE_RD_KEY4_DATA4_REG register
 *  Represents rd_key4_data4
 */
#define EFUSE_RD_KEY4_DATA4_REG (DR_REG_EFUSE_BASE + 0x12c)
/** EFUSE_KEY4_DATA4 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key4.
 */
#define EFUSE_KEY4_DATA4    0xFFFFFFFFU
#define EFUSE_KEY4_DATA4_M  (EFUSE_KEY4_DATA4_V << EFUSE_KEY4_DATA4_S)
#define EFUSE_KEY4_DATA4_V  0xFFFFFFFFU
#define EFUSE_KEY4_DATA4_S  0

/** EFUSE_RD_KEY4_DATA5_REG register
 *  Represents rd_key4_data5
 */
#define EFUSE_RD_KEY4_DATA5_REG (DR_REG_EFUSE_BASE + 0x130)
/** EFUSE_KEY4_DATA5 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key4.
 */
#define EFUSE_KEY4_DATA5    0xFFFFFFFFU
#define EFUSE_KEY4_DATA5_M  (EFUSE_KEY4_DATA5_V << EFUSE_KEY4_DATA5_S)
#define EFUSE_KEY4_DATA5_V  0xFFFFFFFFU
#define EFUSE_KEY4_DATA5_S  0

/** EFUSE_RD_KEY4_DATA6_REG register
 *  Represents rd_key4_data6
 */
#define EFUSE_RD_KEY4_DATA6_REG (DR_REG_EFUSE_BASE + 0x134)
/** EFUSE_KEY4_DATA6 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key4.
 */
#define EFUSE_KEY4_DATA6    0xFFFFFFFFU
#define EFUSE_KEY4_DATA6_M  (EFUSE_KEY4_DATA6_V << EFUSE_KEY4_DATA6_S)
#define EFUSE_KEY4_DATA6_V  0xFFFFFFFFU
#define EFUSE_KEY4_DATA6_S  0

/** EFUSE_RD_KEY4_DATA7_REG register
 *  Represents rd_key4_data7
 */
#define EFUSE_RD_KEY4_DATA7_REG (DR_REG_EFUSE_BASE + 0x138)
/** EFUSE_KEY4_DATA7 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key4.
 */
#define EFUSE_KEY4_DATA7    0xFFFFFFFFU
#define EFUSE_KEY4_DATA7_M  (EFUSE_KEY4_DATA7_V << EFUSE_KEY4_DATA7_S)
#define EFUSE_KEY4_DATA7_V  0xFFFFFFFFU
#define EFUSE_KEY4_DATA7_S  0

/** EFUSE_RD_KEY5_DATA0_REG register
 *  Represents rd_key5_data0
 */
#define EFUSE_RD_KEY5_DATA0_REG (DR_REG_EFUSE_BASE + 0x13c)
/** EFUSE_KEY5_DATA0 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key5.
 */
#define EFUSE_KEY5_DATA0    0xFFFFFFFFU
#define EFUSE_KEY5_DATA0_M  (EFUSE_KEY5_DATA0_V << EFUSE_KEY5_DATA0_S)
#define EFUSE_KEY5_DATA0_V  0xFFFFFFFFU
#define EFUSE_KEY5_DATA0_S  0

/** EFUSE_RD_KEY5_DATA1_REG register
 *  Represents rd_key5_data1
 */
#define EFUSE_RD_KEY5_DATA1_REG (DR_REG_EFUSE_BASE + 0x140)
/** EFUSE_KEY5_DATA1 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key5.
 */
#define EFUSE_KEY5_DATA1    0xFFFFFFFFU
#define EFUSE_KEY5_DATA1_M  (EFUSE_KEY5_DATA1_V << EFUSE_KEY5_DATA1_S)
#define EFUSE_KEY5_DATA1_V  0xFFFFFFFFU
#define EFUSE_KEY5_DATA1_S  0

/** EFUSE_RD_KEY5_DATA2_REG register
 *  Represents rd_key5_data2
 */
#define EFUSE_RD_KEY5_DATA2_REG (DR_REG_EFUSE_BASE + 0x144)
/** EFUSE_KEY5_DATA2 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key5.
 */
#define EFUSE_KEY5_DATA2    0xFFFFFFFFU
#define EFUSE_KEY5_DATA2_M  (EFUSE_KEY5_DATA2_V << EFUSE_KEY5_DATA2_S)
#define EFUSE_KEY5_DATA2_V  0xFFFFFFFFU
#define EFUSE_KEY5_DATA2_S  0

/** EFUSE_RD_KEY5_DATA3_REG register
 *  Represents rd_key5_data3
 */
#define EFUSE_RD_KEY5_DATA3_REG (DR_REG_EFUSE_BASE + 0x148)
/** EFUSE_KEY5_DATA3 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key5.
 */
#define EFUSE_KEY5_DATA3    0xFFFFFFFFU
#define EFUSE_KEY5_DATA3_M  (EFUSE_KEY5_DATA3_V << EFUSE_KEY5_DATA3_S)
#define EFUSE_KEY5_DATA3_V  0xFFFFFFFFU
#define EFUSE_KEY5_DATA3_S  0

/** EFUSE_RD_KEY5_DATA4_REG register
 *  Represents rd_key5_data4
 */
#define EFUSE_RD_KEY5_DATA4_REG (DR_REG_EFUSE_BASE + 0x14c)
/** EFUSE_KEY5_DATA4 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key5.
 */
#define EFUSE_KEY5_DATA4    0xFFFFFFFFU
#define EFUSE_KEY5_DATA4_M  (EFUSE_KEY5_DATA4_V << EFUSE_KEY5_DATA4_S)
#define EFUSE_KEY5_DATA4_V  0xFFFFFFFFU
#define EFUSE_KEY5_DATA4_S  0

/** EFUSE_RD_KEY5_DATA5_REG register
 *  Represents rd_key5_data5
 */
#define EFUSE_RD_KEY5_DATA5_REG (DR_REG_EFUSE_BASE + 0x150)
/** EFUSE_KEY5_DATA5 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key5.
 */
#define EFUSE_KEY5_DATA5    0xFFFFFFFFU
#define EFUSE_KEY5_DATA5_M  (EFUSE_KEY5_DATA5_V << EFUSE_KEY5_DATA5_S)
#define EFUSE_KEY5_DATA5_V  0xFFFFFFFFU
#define EFUSE_KEY5_DATA5_S  0

/** EFUSE_RD_KEY5_DATA6_REG register
 *  Represents rd_key5_data6
 */
#define EFUSE_RD_KEY5_DATA6_REG (DR_REG_EFUSE_BASE + 0x154)
/** EFUSE_KEY5_DATA6 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key5.
 */
#define EFUSE_KEY5_DATA6    0xFFFFFFFFU
#define EFUSE_KEY5_DATA6_M  (EFUSE_KEY5_DATA6_V << EFUSE_KEY5_DATA6_S)
#define EFUSE_KEY5_DATA6_V  0xFFFFFFFFU
#define EFUSE_KEY5_DATA6_S  0

/** EFUSE_RD_KEY5_DATA7_REG register
 *  Represents rd_key5_data7
 */
#define EFUSE_RD_KEY5_DATA7_REG (DR_REG_EFUSE_BASE + 0x158)
/** EFUSE_KEY5_DATA7 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of key5.
 */
#define EFUSE_KEY5_DATA7    0xFFFFFFFFU
#define EFUSE_KEY5_DATA7_M  (EFUSE_KEY5_DATA7_V << EFUSE_KEY5_DATA7_S)
#define EFUSE_KEY5_DATA7_V  0xFFFFFFFFU
#define EFUSE_KEY5_DATA7_S  0

/** EFUSE_RD_SYS_PART2_DATA0_REG register
 *  Represents rd_sys_part2_data0
 */
#define EFUSE_RD_SYS_PART2_DATA0_REG (DR_REG_EFUSE_BASE + 0x15c)
/** EFUSE_SYS_DATA_PART2_0 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of second part of system data.
 */
#define EFUSE_SYS_DATA_PART2_0    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_0_M  (EFUSE_SYS_DATA_PART2_0_V << EFUSE_SYS_DATA_PART2_0_S)
#define EFUSE_SYS_DATA_PART2_0_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_0_S  0

/** EFUSE_RD_SYS_PART2_DATA1_REG register
 *  Represents rd_sys_part2_data1
 */
#define EFUSE_RD_SYS_PART2_DATA1_REG (DR_REG_EFUSE_BASE + 0x160)
/** EFUSE_SYS_DATA_PART2_1 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of second part of system data.
 */
#define EFUSE_SYS_DATA_PART2_1    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_1_M  (EFUSE_SYS_DATA_PART2_1_V << EFUSE_SYS_DATA_PART2_1_S)
#define EFUSE_SYS_DATA_PART2_1_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_1_S  0

/** EFUSE_RD_SYS_PART2_DATA2_REG register
 *  Represents rd_sys_part2_data2
 */
#define EFUSE_RD_SYS_PART2_DATA2_REG (DR_REG_EFUSE_BASE + 0x164)
/** EFUSE_SYS_DATA_PART2_2 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of second part of system data.
 */
#define EFUSE_SYS_DATA_PART2_2    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_2_M  (EFUSE_SYS_DATA_PART2_2_V << EFUSE_SYS_DATA_PART2_2_S)
#define EFUSE_SYS_DATA_PART2_2_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_2_S  0

/** EFUSE_RD_SYS_PART2_DATA3_REG register
 *  Represents rd_sys_part2_data3
 */
#define EFUSE_RD_SYS_PART2_DATA3_REG (DR_REG_EFUSE_BASE + 0x168)
/** EFUSE_SYS_DATA_PART2_3 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of second part of system data.
 */
#define EFUSE_SYS_DATA_PART2_3    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_3_M  (EFUSE_SYS_DATA_PART2_3_V << EFUSE_SYS_DATA_PART2_3_S)
#define EFUSE_SYS_DATA_PART2_3_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_3_S  0

/** EFUSE_RD_SYS_PART2_DATA4_REG register
 *  Represents rd_sys_part2_data4
 */
#define EFUSE_RD_SYS_PART2_DATA4_REG (DR_REG_EFUSE_BASE + 0x16c)
/** EFUSE_SYS_DATA_PART2_4 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of second part of system data.
 */
#define EFUSE_SYS_DATA_PART2_4    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_4_M  (EFUSE_SYS_DATA_PART2_4_V << EFUSE_SYS_DATA_PART2_4_S)
#define EFUSE_SYS_DATA_PART2_4_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_4_S  0

/** EFUSE_RD_SYS_PART2_DATA5_REG register
 *  Represents rd_sys_part2_data5
 */
#define EFUSE_RD_SYS_PART2_DATA5_REG (DR_REG_EFUSE_BASE + 0x170)
/** EFUSE_SYS_DATA_PART2_5 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of second part of system data.
 */
#define EFUSE_SYS_DATA_PART2_5    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_5_M  (EFUSE_SYS_DATA_PART2_5_V << EFUSE_SYS_DATA_PART2_5_S)
#define EFUSE_SYS_DATA_PART2_5_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_5_S  0

/** EFUSE_RD_SYS_PART2_DATA6_REG register
 *  Represents rd_sys_part2_data6
 */
#define EFUSE_RD_SYS_PART2_DATA6_REG (DR_REG_EFUSE_BASE + 0x174)
/** EFUSE_SYS_DATA_PART2_6 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of second part of system data.
 */
#define EFUSE_SYS_DATA_PART2_6    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_6_M  (EFUSE_SYS_DATA_PART2_6_V << EFUSE_SYS_DATA_PART2_6_S)
#define EFUSE_SYS_DATA_PART2_6_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_6_S  0

/** EFUSE_RD_SYS_PART2_DATA7_REG register
 *  Represents rd_sys_part2_data7
 */
#define EFUSE_RD_SYS_PART2_DATA7_REG (DR_REG_EFUSE_BASE + 0x178)
/** EFUSE_SYS_DATA_PART2_7 : RO; bitpos: [31:0]; default: 0;
 *  Represents the zeroth 32-bit of second part of system data.
 */
#define EFUSE_SYS_DATA_PART2_7    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_7_M  (EFUSE_SYS_DATA_PART2_7_V << EFUSE_SYS_DATA_PART2_7_S)
#define EFUSE_SYS_DATA_PART2_7_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_7_S  0

/** EFUSE_RD_REPEAT_DATA_ERR0_REG register
 *  Represents rd_repeat_data_err
 */
#define EFUSE_RD_REPEAT_DATA_ERR0_REG (DR_REG_EFUSE_BASE + 0x17c)
/** EFUSE_RD_DIS_ERR : RO; bitpos: [6:0]; default: 0;
 *  Represents the programming error of EFUSE_RD_DIS
 */
#define EFUSE_RD_DIS_ERR    0x0000007FU
#define EFUSE_RD_DIS_ERR_M  (EFUSE_RD_DIS_ERR_V << EFUSE_RD_DIS_ERR_S)
#define EFUSE_RD_DIS_ERR_V  0x0000007FU
#define EFUSE_RD_DIS_ERR_S  0
/** EFUSE_PVT_GLITCH_EN_ERR : RO; bitpos: [7]; default: 0;
 *  Represents the programming error of EFUSE_PVT_GLITCH_EN
 */
#define EFUSE_PVT_GLITCH_EN_ERR    (BIT(7))
#define EFUSE_PVT_GLITCH_EN_ERR_M  (EFUSE_PVT_GLITCH_EN_ERR_V << EFUSE_PVT_GLITCH_EN_ERR_S)
#define EFUSE_PVT_GLITCH_EN_ERR_V  0x00000001U
#define EFUSE_PVT_GLITCH_EN_ERR_S  7
/** EFUSE_DIS_ICACHE_ERR : RO; bitpos: [8]; default: 0;
 *  Represents the programming error of EFUSE_DIS_ICACHE
 */
#define EFUSE_DIS_ICACHE_ERR    (BIT(8))
#define EFUSE_DIS_ICACHE_ERR_M  (EFUSE_DIS_ICACHE_ERR_V << EFUSE_DIS_ICACHE_ERR_S)
#define EFUSE_DIS_ICACHE_ERR_V  0x00000001U
#define EFUSE_DIS_ICACHE_ERR_S  8
/** EFUSE_DIS_USB_JTAG_ERR : RO; bitpos: [9]; default: 0;
 *  Represents the programming error of EFUSE_DIS_USB_JTAG
 */
#define EFUSE_DIS_USB_JTAG_ERR    (BIT(9))
#define EFUSE_DIS_USB_JTAG_ERR_M  (EFUSE_DIS_USB_JTAG_ERR_V << EFUSE_DIS_USB_JTAG_ERR_S)
#define EFUSE_DIS_USB_JTAG_ERR_V  0x00000001U
#define EFUSE_DIS_USB_JTAG_ERR_S  9
/** EFUSE_POWERGLITCH_EN_ERR : RO; bitpos: [10]; default: 0;
 *  Represents the programming error of EFUSE_POWERGLITCH_EN
 */
#define EFUSE_POWERGLITCH_EN_ERR    (BIT(10))
#define EFUSE_POWERGLITCH_EN_ERR_M  (EFUSE_POWERGLITCH_EN_ERR_V << EFUSE_POWERGLITCH_EN_ERR_S)
#define EFUSE_POWERGLITCH_EN_ERR_V  0x00000001U
#define EFUSE_POWERGLITCH_EN_ERR_S  10
/** EFUSE_DIS_FORCE_DOWNLOAD_ERR : RO; bitpos: [12]; default: 0;
 *  Represents the programming error of EFUSE_DIS_FORCE_DOWNLOAD
 */
#define EFUSE_DIS_FORCE_DOWNLOAD_ERR    (BIT(12))
#define EFUSE_DIS_FORCE_DOWNLOAD_ERR_M  (EFUSE_DIS_FORCE_DOWNLOAD_ERR_V << EFUSE_DIS_FORCE_DOWNLOAD_ERR_S)
#define EFUSE_DIS_FORCE_DOWNLOAD_ERR_V  0x00000001U
#define EFUSE_DIS_FORCE_DOWNLOAD_ERR_S  12
/** EFUSE_SPI_DOWNLOAD_MSPI_DIS_ERR : RO; bitpos: [13]; default: 0;
 *  Represents the programming error of EFUSE_SPI_DOWNLOAD_MSPI_DIS
 */
#define EFUSE_SPI_DOWNLOAD_MSPI_DIS_ERR    (BIT(13))
#define EFUSE_SPI_DOWNLOAD_MSPI_DIS_ERR_M  (EFUSE_SPI_DOWNLOAD_MSPI_DIS_ERR_V << EFUSE_SPI_DOWNLOAD_MSPI_DIS_ERR_S)
#define EFUSE_SPI_DOWNLOAD_MSPI_DIS_ERR_V  0x00000001U
#define EFUSE_SPI_DOWNLOAD_MSPI_DIS_ERR_S  13
/** EFUSE_DIS_TWAI_ERR : RO; bitpos: [14]; default: 0;
 *  Represents the programming error of EFUSE_DIS_TWAI
 */
#define EFUSE_DIS_TWAI_ERR    (BIT(14))
#define EFUSE_DIS_TWAI_ERR_M  (EFUSE_DIS_TWAI_ERR_V << EFUSE_DIS_TWAI_ERR_S)
#define EFUSE_DIS_TWAI_ERR_V  0x00000001U
#define EFUSE_DIS_TWAI_ERR_S  14
/** EFUSE_JTAG_SEL_ENABLE_ERR : RO; bitpos: [15]; default: 0;
 *  Represents the programming error of EFUSE_JTAG_SEL_ENABLE
 */
#define EFUSE_JTAG_SEL_ENABLE_ERR    (BIT(15))
#define EFUSE_JTAG_SEL_ENABLE_ERR_M  (EFUSE_JTAG_SEL_ENABLE_ERR_V << EFUSE_JTAG_SEL_ENABLE_ERR_S)
#define EFUSE_JTAG_SEL_ENABLE_ERR_V  0x00000001U
#define EFUSE_JTAG_SEL_ENABLE_ERR_S  15
/** EFUSE_SOFT_DIS_JTAG_ERR : RO; bitpos: [18:16]; default: 0;
 *  Represents the programming error of EFUSE_SOFT_DIS_JTAG
 */
#define EFUSE_SOFT_DIS_JTAG_ERR    0x00000007U
#define EFUSE_SOFT_DIS_JTAG_ERR_M  (EFUSE_SOFT_DIS_JTAG_ERR_V << EFUSE_SOFT_DIS_JTAG_ERR_S)
#define EFUSE_SOFT_DIS_JTAG_ERR_V  0x00000007U
#define EFUSE_SOFT_DIS_JTAG_ERR_S  16
/** EFUSE_DIS_PAD_JTAG_ERR : RO; bitpos: [19]; default: 0;
 *  Represents the programming error of EFUSE_DIS_PAD_JTAG
 */
#define EFUSE_DIS_PAD_JTAG_ERR    (BIT(19))
#define EFUSE_DIS_PAD_JTAG_ERR_M  (EFUSE_DIS_PAD_JTAG_ERR_V << EFUSE_DIS_PAD_JTAG_ERR_S)
#define EFUSE_DIS_PAD_JTAG_ERR_V  0x00000001U
#define EFUSE_DIS_PAD_JTAG_ERR_S  19
/** EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR : RO; bitpos: [20]; default: 0;
 *  Represents the programming error of EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT
 */
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR    (BIT(20))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_M  (EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_V << EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_S)
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_V  0x00000001U
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_S  20
/** EFUSE_USB_EXCHG_PINS_ERR : RO; bitpos: [25]; default: 0;
 *  Represents the programming error of EFUSE_USB_EXCHG_PINS
 */
#define EFUSE_USB_EXCHG_PINS_ERR    (BIT(25))
#define EFUSE_USB_EXCHG_PINS_ERR_M  (EFUSE_USB_EXCHG_PINS_ERR_V << EFUSE_USB_EXCHG_PINS_ERR_S)
#define EFUSE_USB_EXCHG_PINS_ERR_V  0x00000001U
#define EFUSE_USB_EXCHG_PINS_ERR_S  25
/** EFUSE_VDD_SPI_AS_GPIO_ERR : RO; bitpos: [26]; default: 0;
 *  Represents the programming error of EFUSE_VDD_SPI_AS_GPIO
 */
#define EFUSE_VDD_SPI_AS_GPIO_ERR    (BIT(26))
#define EFUSE_VDD_SPI_AS_GPIO_ERR_M  (EFUSE_VDD_SPI_AS_GPIO_ERR_V << EFUSE_VDD_SPI_AS_GPIO_ERR_S)
#define EFUSE_VDD_SPI_AS_GPIO_ERR_V  0x00000001U
#define EFUSE_VDD_SPI_AS_GPIO_ERR_S  26
/** EFUSE_ECDSA_CURVE_MODE_ERR : RO; bitpos: [28:27]; default: 0;
 *  Represents the programming error of EFUSE_ECDSA_CURVE_MODE
 */
#define EFUSE_ECDSA_CURVE_MODE_ERR    0x00000003U
#define EFUSE_ECDSA_CURVE_MODE_ERR_M  (EFUSE_ECDSA_CURVE_MODE_ERR_V << EFUSE_ECDSA_CURVE_MODE_ERR_S)
#define EFUSE_ECDSA_CURVE_MODE_ERR_V  0x00000003U
#define EFUSE_ECDSA_CURVE_MODE_ERR_S  27
/** EFUSE_ECC_FORCE_CONST_TIME_ERR : RO; bitpos: [29]; default: 0;
 *  Represents the programming error of EFUSE_ECC_FORCE_CONST_TIME
 */
#define EFUSE_ECC_FORCE_CONST_TIME_ERR    (BIT(29))
#define EFUSE_ECC_FORCE_CONST_TIME_ERR_M  (EFUSE_ECC_FORCE_CONST_TIME_ERR_V << EFUSE_ECC_FORCE_CONST_TIME_ERR_S)
#define EFUSE_ECC_FORCE_CONST_TIME_ERR_V  0x00000001U
#define EFUSE_ECC_FORCE_CONST_TIME_ERR_S  29
/** EFUSE_XTS_DPA_PSEUDO_LEVEL_ERR : RO; bitpos: [31:30]; default: 0;
 *  Represents the programming error of EFUSE_XTS_DPA_PSEUDO_LEVEL
 */
#define EFUSE_XTS_DPA_PSEUDO_LEVEL_ERR    0x00000003U
#define EFUSE_XTS_DPA_PSEUDO_LEVEL_ERR_M  (EFUSE_XTS_DPA_PSEUDO_LEVEL_ERR_V << EFUSE_XTS_DPA_PSEUDO_LEVEL_ERR_S)
#define EFUSE_XTS_DPA_PSEUDO_LEVEL_ERR_V  0x00000003U
#define EFUSE_XTS_DPA_PSEUDO_LEVEL_ERR_S  30

/** EFUSE_RD_REPEAT_DATA_ERR1_REG register
 *  Represents rd_repeat_data_err
 */
#define EFUSE_RD_REPEAT_DATA_ERR1_REG (DR_REG_EFUSE_BASE + 0x180)
/** EFUSE_IO_LDO_ADJUST_ERR : RO; bitpos: [7:0]; default: 0;
 *  Represents the programming error of EFUSE_IO_LDO_ADJUST
 */
#define EFUSE_IO_LDO_ADJUST_ERR    0x000000FFU
#define EFUSE_IO_LDO_ADJUST_ERR_M  (EFUSE_IO_LDO_ADJUST_ERR_V << EFUSE_IO_LDO_ADJUST_ERR_S)
#define EFUSE_IO_LDO_ADJUST_ERR_V  0x000000FFU
#define EFUSE_IO_LDO_ADJUST_ERR_S  0
/** EFUSE_VDD_SPI_LDO_ADJUST_ERR : RO; bitpos: [15:8]; default: 0;
 *  Represents the programming error of EFUSE_VDD_SPI_LDO_ADJUST
 */
#define EFUSE_VDD_SPI_LDO_ADJUST_ERR    0x000000FFU
#define EFUSE_VDD_SPI_LDO_ADJUST_ERR_M  (EFUSE_VDD_SPI_LDO_ADJUST_ERR_V << EFUSE_VDD_SPI_LDO_ADJUST_ERR_S)
#define EFUSE_VDD_SPI_LDO_ADJUST_ERR_V  0x000000FFU
#define EFUSE_VDD_SPI_LDO_ADJUST_ERR_S  8
/** EFUSE_WDT_DELAY_SEL_ERR : RO; bitpos: [17:16]; default: 0;
 *  Represents the programming error of EFUSE_WDT_DELAY_SEL
 */
#define EFUSE_WDT_DELAY_SEL_ERR    0x00000003U
#define EFUSE_WDT_DELAY_SEL_ERR_M  (EFUSE_WDT_DELAY_SEL_ERR_V << EFUSE_WDT_DELAY_SEL_ERR_S)
#define EFUSE_WDT_DELAY_SEL_ERR_V  0x00000003U
#define EFUSE_WDT_DELAY_SEL_ERR_S  16
/** EFUSE_SPI_BOOT_CRYPT_CNT_ERR : RO; bitpos: [20:18]; default: 0;
 *  Represents the programming error of EFUSE_SPI_BOOT_CRYPT_CNT
 */
#define EFUSE_SPI_BOOT_CRYPT_CNT_ERR    0x00000007U
#define EFUSE_SPI_BOOT_CRYPT_CNT_ERR_M  (EFUSE_SPI_BOOT_CRYPT_CNT_ERR_V << EFUSE_SPI_BOOT_CRYPT_CNT_ERR_S)
#define EFUSE_SPI_BOOT_CRYPT_CNT_ERR_V  0x00000007U
#define EFUSE_SPI_BOOT_CRYPT_CNT_ERR_S  18
/** EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR : RO; bitpos: [21]; default: 0;
 *  Represents the programming error of EFUSE_SECURE_BOOT_KEY_REVOKE0
 */
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR    (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR_M  (EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR_V << EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR_S)
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR_V  0x00000001U
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR_S  21
/** EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR : RO; bitpos: [22]; default: 0;
 *  Represents the programming error of EFUSE_SECURE_BOOT_KEY_REVOKE1
 */
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR    (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR_M  (EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR_V << EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR_S)
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR_V  0x00000001U
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR_S  22
/** EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR : RO; bitpos: [23]; default: 0;
 *  Represents the programming error of EFUSE_SECURE_BOOT_KEY_REVOKE2
 */
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR    (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR_M  (EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR_V << EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR_S)
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR_V  0x00000001U
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR_S  23
/** EFUSE_KEY_PURPOSE_0_ERR : RO; bitpos: [27:24]; default: 0;
 *  Represents the programming error of EFUSE_KEY_PURPOSE_0
 */
#define EFUSE_KEY_PURPOSE_0_ERR    0x0000000FU
#define EFUSE_KEY_PURPOSE_0_ERR_M  (EFUSE_KEY_PURPOSE_0_ERR_V << EFUSE_KEY_PURPOSE_0_ERR_S)
#define EFUSE_KEY_PURPOSE_0_ERR_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_0_ERR_S  24
/** EFUSE_KEY_PURPOSE_1_ERR : RO; bitpos: [31:28]; default: 0;
 *  Represents the programming error of EFUSE_KEY_PURPOSE_1
 */
#define EFUSE_KEY_PURPOSE_1_ERR    0x0000000FU
#define EFUSE_KEY_PURPOSE_1_ERR_M  (EFUSE_KEY_PURPOSE_1_ERR_V << EFUSE_KEY_PURPOSE_1_ERR_S)
#define EFUSE_KEY_PURPOSE_1_ERR_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_1_ERR_S  28

/** EFUSE_RD_REPEAT_DATA_ERR2_REG register
 *  Represents rd_repeat_data_err
 */
#define EFUSE_RD_REPEAT_DATA_ERR2_REG (DR_REG_EFUSE_BASE + 0x184)
/** EFUSE_KEY_PURPOSE_2_ERR : RO; bitpos: [3:0]; default: 0;
 *  Represents the programming error of EFUSE_KEY_PURPOSE_2
 */
#define EFUSE_KEY_PURPOSE_2_ERR    0x0000000FU
#define EFUSE_KEY_PURPOSE_2_ERR_M  (EFUSE_KEY_PURPOSE_2_ERR_V << EFUSE_KEY_PURPOSE_2_ERR_S)
#define EFUSE_KEY_PURPOSE_2_ERR_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_2_ERR_S  0
/** EFUSE_KEY_PURPOSE_3_ERR : RO; bitpos: [7:4]; default: 0;
 *  Represents the programming error of EFUSE_KEY_PURPOSE_3
 */
#define EFUSE_KEY_PURPOSE_3_ERR    0x0000000FU
#define EFUSE_KEY_PURPOSE_3_ERR_M  (EFUSE_KEY_PURPOSE_3_ERR_V << EFUSE_KEY_PURPOSE_3_ERR_S)
#define EFUSE_KEY_PURPOSE_3_ERR_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_3_ERR_S  4
/** EFUSE_KEY_PURPOSE_4_ERR : RO; bitpos: [11:8]; default: 0;
 *  Represents the programming error of EFUSE_KEY_PURPOSE_4
 */
#define EFUSE_KEY_PURPOSE_4_ERR    0x0000000FU
#define EFUSE_KEY_PURPOSE_4_ERR_M  (EFUSE_KEY_PURPOSE_4_ERR_V << EFUSE_KEY_PURPOSE_4_ERR_S)
#define EFUSE_KEY_PURPOSE_4_ERR_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_4_ERR_S  8
/** EFUSE_KEY_PURPOSE_5_ERR : RO; bitpos: [15:12]; default: 0;
 *  Represents the programming error of EFUSE_KEY_PURPOSE_5
 */
#define EFUSE_KEY_PURPOSE_5_ERR    0x0000000FU
#define EFUSE_KEY_PURPOSE_5_ERR_M  (EFUSE_KEY_PURPOSE_5_ERR_V << EFUSE_KEY_PURPOSE_5_ERR_S)
#define EFUSE_KEY_PURPOSE_5_ERR_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_5_ERR_S  12
/** EFUSE_SEC_DPA_LEVEL_ERR : RO; bitpos: [17:16]; default: 0;
 *  Represents the programming error of EFUSE_SEC_DPA_LEVEL
 */
#define EFUSE_SEC_DPA_LEVEL_ERR    0x00000003U
#define EFUSE_SEC_DPA_LEVEL_ERR_M  (EFUSE_SEC_DPA_LEVEL_ERR_V << EFUSE_SEC_DPA_LEVEL_ERR_S)
#define EFUSE_SEC_DPA_LEVEL_ERR_V  0x00000003U
#define EFUSE_SEC_DPA_LEVEL_ERR_S  16
/** EFUSE_IO_LDO_1P8_ERR : RO; bitpos: [18]; default: 0;
 *  Represents the programming error of EFUSE_IO_LDO_1P8
 */
#define EFUSE_IO_LDO_1P8_ERR    (BIT(18))
#define EFUSE_IO_LDO_1P8_ERR_M  (EFUSE_IO_LDO_1P8_ERR_V << EFUSE_IO_LDO_1P8_ERR_S)
#define EFUSE_IO_LDO_1P8_ERR_V  0x00000001U
#define EFUSE_IO_LDO_1P8_ERR_S  18
/** EFUSE_CRYPT_DPA_ENABLE_ERR : RO; bitpos: [19]; default: 0;
 *  Represents the programming error of EFUSE_CRYPT_DPA_ENABLE
 */
#define EFUSE_CRYPT_DPA_ENABLE_ERR    (BIT(19))
#define EFUSE_CRYPT_DPA_ENABLE_ERR_M  (EFUSE_CRYPT_DPA_ENABLE_ERR_V << EFUSE_CRYPT_DPA_ENABLE_ERR_S)
#define EFUSE_CRYPT_DPA_ENABLE_ERR_V  0x00000001U
#define EFUSE_CRYPT_DPA_ENABLE_ERR_S  19
/** EFUSE_SECURE_BOOT_EN_ERR : RO; bitpos: [20]; default: 0;
 *  Represents the programming error of EFUSE_SECURE_BOOT_EN
 */
#define EFUSE_SECURE_BOOT_EN_ERR    (BIT(20))
#define EFUSE_SECURE_BOOT_EN_ERR_M  (EFUSE_SECURE_BOOT_EN_ERR_V << EFUSE_SECURE_BOOT_EN_ERR_S)
#define EFUSE_SECURE_BOOT_EN_ERR_V  0x00000001U
#define EFUSE_SECURE_BOOT_EN_ERR_S  20
/** EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR : RO; bitpos: [21]; default: 0;
 *  Represents the programming error of EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE
 */
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR    (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_M  (EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_V << EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_S)
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_V  0x00000001U
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_S  21
/** EFUSE_POWERGLITCH_EN1_ERR : RO; bitpos: [26:22]; default: 0;
 *  Represents the programming error of EFUSE_POWERGLITCH_EN1
 */
#define EFUSE_POWERGLITCH_EN1_ERR    0x0000001FU
#define EFUSE_POWERGLITCH_EN1_ERR_M  (EFUSE_POWERGLITCH_EN1_ERR_V << EFUSE_POWERGLITCH_EN1_ERR_S)
#define EFUSE_POWERGLITCH_EN1_ERR_V  0x0000001FU
#define EFUSE_POWERGLITCH_EN1_ERR_S  22
/** EFUSE_DCDC_CCM_EN_ERR : RO; bitpos: [27]; default: 0;
 *  Represents the programming error of EFUSE_DCDC_CCM_EN
 */
#define EFUSE_DCDC_CCM_EN_ERR    (BIT(27))
#define EFUSE_DCDC_CCM_EN_ERR_M  (EFUSE_DCDC_CCM_EN_ERR_V << EFUSE_DCDC_CCM_EN_ERR_S)
#define EFUSE_DCDC_CCM_EN_ERR_V  0x00000001U
#define EFUSE_DCDC_CCM_EN_ERR_S  27
/** EFUSE_FLASH_TPUW_ERR : RO; bitpos: [31:28]; default: 0;
 *  Represents the programming error of EFUSE_FLASH_TPUW
 */
#define EFUSE_FLASH_TPUW_ERR    0x0000000FU
#define EFUSE_FLASH_TPUW_ERR_M  (EFUSE_FLASH_TPUW_ERR_V << EFUSE_FLASH_TPUW_ERR_S)
#define EFUSE_FLASH_TPUW_ERR_V  0x0000000FU
#define EFUSE_FLASH_TPUW_ERR_S  28

/** EFUSE_RD_REPEAT_DATA_ERR3_REG register
 *  Represents rd_repeat_data_err
 */
#define EFUSE_RD_REPEAT_DATA_ERR3_REG (DR_REG_EFUSE_BASE + 0x188)
/** EFUSE_DIS_DOWNLOAD_MODE_ERR : RO; bitpos: [0]; default: 0;
 *  Represents the programming error of EFUSE_DIS_DOWNLOAD_MODE
 */
#define EFUSE_DIS_DOWNLOAD_MODE_ERR    (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_ERR_M  (EFUSE_DIS_DOWNLOAD_MODE_ERR_V << EFUSE_DIS_DOWNLOAD_MODE_ERR_S)
#define EFUSE_DIS_DOWNLOAD_MODE_ERR_V  0x00000001U
#define EFUSE_DIS_DOWNLOAD_MODE_ERR_S  0
/** EFUSE_DIS_DIRECT_BOOT_ERR : RO; bitpos: [1]; default: 0;
 *  Represents the programming error of EFUSE_DIS_DIRECT_BOOT
 */
#define EFUSE_DIS_DIRECT_BOOT_ERR    (BIT(1))
#define EFUSE_DIS_DIRECT_BOOT_ERR_M  (EFUSE_DIS_DIRECT_BOOT_ERR_V << EFUSE_DIS_DIRECT_BOOT_ERR_S)
#define EFUSE_DIS_DIRECT_BOOT_ERR_V  0x00000001U
#define EFUSE_DIS_DIRECT_BOOT_ERR_S  1
/** EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR : RO; bitpos: [2]; default: 0;
 *  Represents the programming error of EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT
 */
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR    (BIT(2))
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR_M  (EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR_V << EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR_S)
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR_V  0x00000001U
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR_S  2
/** EFUSE_FLASH_LDO_EFUSE_SEL_ERR : RO; bitpos: [3]; default: 0;
 *  Represents the programming error of EFUSE_FLASH_LDO_EFUSE_SEL
 */
#define EFUSE_FLASH_LDO_EFUSE_SEL_ERR    (BIT(3))
#define EFUSE_FLASH_LDO_EFUSE_SEL_ERR_M  (EFUSE_FLASH_LDO_EFUSE_SEL_ERR_V << EFUSE_FLASH_LDO_EFUSE_SEL_ERR_S)
#define EFUSE_FLASH_LDO_EFUSE_SEL_ERR_V  0x00000001U
#define EFUSE_FLASH_LDO_EFUSE_SEL_ERR_S  3
/** EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_ERR : RO; bitpos: [4]; default: 0;
 *  Represents the programming error of EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE
 */
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_ERR    (BIT(4))
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_ERR_M  (EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_ERR_V << EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_ERR_S)
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_ERR_V  0x00000001U
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_ERR_S  4
/** EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR : RO; bitpos: [5]; default: 0;
 *  Represents the programming error of EFUSE_ENABLE_SECURITY_DOWNLOAD
 */
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR    (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_M  (EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_V << EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_S)
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_V  0x00000001U
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_S  5
/** EFUSE_UART_PRINT_CONTROL_ERR : RO; bitpos: [7:6]; default: 0;
 *  Represents the programming error of EFUSE_UART_PRINT_CONTROL
 */
#define EFUSE_UART_PRINT_CONTROL_ERR    0x00000003U
#define EFUSE_UART_PRINT_CONTROL_ERR_M  (EFUSE_UART_PRINT_CONTROL_ERR_V << EFUSE_UART_PRINT_CONTROL_ERR_S)
#define EFUSE_UART_PRINT_CONTROL_ERR_V  0x00000003U
#define EFUSE_UART_PRINT_CONTROL_ERR_S  6
/** EFUSE_FORCE_SEND_RESUME_ERR : RO; bitpos: [8]; default: 0;
 *  Represents the programming error of EFUSE_FORCE_SEND_RESUME
 */
#define EFUSE_FORCE_SEND_RESUME_ERR    (BIT(8))
#define EFUSE_FORCE_SEND_RESUME_ERR_M  (EFUSE_FORCE_SEND_RESUME_ERR_V << EFUSE_FORCE_SEND_RESUME_ERR_S)
#define EFUSE_FORCE_SEND_RESUME_ERR_V  0x00000001U
#define EFUSE_FORCE_SEND_RESUME_ERR_S  8
/** EFUSE_SECURE_VERSION_ERR : RO; bitpos: [24:9]; default: 0;
 *  Represents the programming error of EFUSE_SECURE_VERSION
 */
#define EFUSE_SECURE_VERSION_ERR    0x0000FFFFU
#define EFUSE_SECURE_VERSION_ERR_M  (EFUSE_SECURE_VERSION_ERR_V << EFUSE_SECURE_VERSION_ERR_S)
#define EFUSE_SECURE_VERSION_ERR_V  0x0000FFFFU
#define EFUSE_SECURE_VERSION_ERR_S  9
/** EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_ERR : RO; bitpos: [25]; default: 0;
 *  Represents the programming error of EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE
 */
#define EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_ERR    (BIT(25))
#define EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_ERR_M  (EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_ERR_V << EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_ERR_S)
#define EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_ERR_V  0x00000001U
#define EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_ERR_S  25
/** EFUSE_HYS_EN_PAD0_ERR : RO; bitpos: [31:26]; default: 0;
 *  Represents the programming error of EFUSE_HYS_EN_PAD0
 */
#define EFUSE_HYS_EN_PAD0_ERR    0x0000003FU
#define EFUSE_HYS_EN_PAD0_ERR_M  (EFUSE_HYS_EN_PAD0_ERR_V << EFUSE_HYS_EN_PAD0_ERR_S)
#define EFUSE_HYS_EN_PAD0_ERR_V  0x0000003FU
#define EFUSE_HYS_EN_PAD0_ERR_S  26

/** EFUSE_RD_REPEAT_DATA_ERR4_REG register
 *  Represents rd_repeat_data_err
 */
#define EFUSE_RD_REPEAT_DATA_ERR4_REG (DR_REG_EFUSE_BASE + 0x18c)
/** EFUSE_HYS_EN_PAD1_ERR : RO; bitpos: [21:0]; default: 0;
 *  Represents the programming error of EFUSE_HYS_EN_PAD1
 */
#define EFUSE_HYS_EN_PAD1_ERR    0x003FFFFFU
#define EFUSE_HYS_EN_PAD1_ERR_M  (EFUSE_HYS_EN_PAD1_ERR_V << EFUSE_HYS_EN_PAD1_ERR_S)
#define EFUSE_HYS_EN_PAD1_ERR_V  0x003FFFFFU
#define EFUSE_HYS_EN_PAD1_ERR_S  0
/** EFUSE_FLASH_LDO_POWER_SEL_ERR : RO; bitpos: [22]; default: 0;
 *  Represents the programming error of EFUSE_FLASH_LDO_POWER_SEL
 */
#define EFUSE_FLASH_LDO_POWER_SEL_ERR    (BIT(22))
#define EFUSE_FLASH_LDO_POWER_SEL_ERR_M  (EFUSE_FLASH_LDO_POWER_SEL_ERR_V << EFUSE_FLASH_LDO_POWER_SEL_ERR_S)
#define EFUSE_FLASH_LDO_POWER_SEL_ERR_V  0x00000001U
#define EFUSE_FLASH_LDO_POWER_SEL_ERR_S  22

/** EFUSE_RD_RS_DATA_ERR0_REG register
 *  Represents rd_rs_data_err
 */
#define EFUSE_RD_RS_DATA_ERR0_REG (DR_REG_EFUSE_BASE + 0x190)
/** EFUSE_RD_MAC_SYS_ERR_NUM : RO; bitpos: [2:0]; default: 0;
 *  Represents the error number of registers.
 *  The value of this signal means the number of error bytes in rd_mac_sys
 */
#define EFUSE_RD_MAC_SYS_ERR_NUM    0x00000007U
#define EFUSE_RD_MAC_SYS_ERR_NUM_M  (EFUSE_RD_MAC_SYS_ERR_NUM_V << EFUSE_RD_MAC_SYS_ERR_NUM_S)
#define EFUSE_RD_MAC_SYS_ERR_NUM_V  0x00000007U
#define EFUSE_RD_MAC_SYS_ERR_NUM_S  0
/** EFUSE_RD_MAC_SYS_FAIL : RO; bitpos: [3]; default: 0;
 *  Represents error status of register.
 *  0: Means no failure and that the data of rd_mac_sys is reliable
 *  1: Means that programming rd_mac_sys failed and the number of error bytes is over 6.
 */
#define EFUSE_RD_MAC_SYS_FAIL    (BIT(3))
#define EFUSE_RD_MAC_SYS_FAIL_M  (EFUSE_RD_MAC_SYS_FAIL_V << EFUSE_RD_MAC_SYS_FAIL_S)
#define EFUSE_RD_MAC_SYS_FAIL_V  0x00000001U
#define EFUSE_RD_MAC_SYS_FAIL_S  3
/** EFUSE_RD_SYS_PART1_DATA_ERR_NUM : RO; bitpos: [6:4]; default: 0;
 *  Represents the error number of registers.
 *  The value of this signal means the number of error bytes in rd_sys_part1_data
 */
#define EFUSE_RD_SYS_PART1_DATA_ERR_NUM    0x00000007U
#define EFUSE_RD_SYS_PART1_DATA_ERR_NUM_M  (EFUSE_RD_SYS_PART1_DATA_ERR_NUM_V << EFUSE_RD_SYS_PART1_DATA_ERR_NUM_S)
#define EFUSE_RD_SYS_PART1_DATA_ERR_NUM_V  0x00000007U
#define EFUSE_RD_SYS_PART1_DATA_ERR_NUM_S  4
/** EFUSE_RD_SYS_PART1_DATA_FAIL : RO; bitpos: [7]; default: 0;
 *  Represents error status of register.
 *  0: Means no failure and that the data of rd_sys_part1_data is reliable
 *  1: Means that programming rd_sys_part1_data failed and the number of error bytes is
 *  over 6.
 */
#define EFUSE_RD_SYS_PART1_DATA_FAIL    (BIT(7))
#define EFUSE_RD_SYS_PART1_DATA_FAIL_M  (EFUSE_RD_SYS_PART1_DATA_FAIL_V << EFUSE_RD_SYS_PART1_DATA_FAIL_S)
#define EFUSE_RD_SYS_PART1_DATA_FAIL_V  0x00000001U
#define EFUSE_RD_SYS_PART1_DATA_FAIL_S  7
/** EFUSE_RD_USR_DATA_ERR_NUM : RO; bitpos: [10:8]; default: 0;
 *  Represents the error number of registers.
 *  The value of this signal means the number of error bytes in rd_usr_data
 */
#define EFUSE_RD_USR_DATA_ERR_NUM    0x00000007U
#define EFUSE_RD_USR_DATA_ERR_NUM_M  (EFUSE_RD_USR_DATA_ERR_NUM_V << EFUSE_RD_USR_DATA_ERR_NUM_S)
#define EFUSE_RD_USR_DATA_ERR_NUM_V  0x00000007U
#define EFUSE_RD_USR_DATA_ERR_NUM_S  8
/** EFUSE_RD_USR_DATA_FAIL : RO; bitpos: [11]; default: 0;
 *  Represents error status of register.
 *  0: Means no failure and that the data of rd_usr_data is reliable
 *  1: Means that programming rd_usr_data failed and the number of error bytes is over
 *  6.
 */
#define EFUSE_RD_USR_DATA_FAIL    (BIT(11))
#define EFUSE_RD_USR_DATA_FAIL_M  (EFUSE_RD_USR_DATA_FAIL_V << EFUSE_RD_USR_DATA_FAIL_S)
#define EFUSE_RD_USR_DATA_FAIL_V  0x00000001U
#define EFUSE_RD_USR_DATA_FAIL_S  11
/** EFUSE_RD_KEY0_DATA_ERR_NUM : RO; bitpos: [14:12]; default: 0;
 *  Represents the error number of registers.
 *  The value of this signal means the number of error bytes in rd_key0_data
 */
#define EFUSE_RD_KEY0_DATA_ERR_NUM    0x00000007U
#define EFUSE_RD_KEY0_DATA_ERR_NUM_M  (EFUSE_RD_KEY0_DATA_ERR_NUM_V << EFUSE_RD_KEY0_DATA_ERR_NUM_S)
#define EFUSE_RD_KEY0_DATA_ERR_NUM_V  0x00000007U
#define EFUSE_RD_KEY0_DATA_ERR_NUM_S  12
/** EFUSE_RD_KEY0_DATA_FAIL : RO; bitpos: [15]; default: 0;
 *  Represents error status of register.
 *  0: Means no failure and that the data of rd_key0_data is reliable
 *  1: Means that programming rd_key0_data failed and the number of error bytes is over
 *  6.
 */
#define EFUSE_RD_KEY0_DATA_FAIL    (BIT(15))
#define EFUSE_RD_KEY0_DATA_FAIL_M  (EFUSE_RD_KEY0_DATA_FAIL_V << EFUSE_RD_KEY0_DATA_FAIL_S)
#define EFUSE_RD_KEY0_DATA_FAIL_V  0x00000001U
#define EFUSE_RD_KEY0_DATA_FAIL_S  15
/** EFUSE_RD_KEY1_DATA_ERR_NUM : RO; bitpos: [18:16]; default: 0;
 *  Represents the error number of registers.
 *  The value of this signal means the number of error bytes in rd_key1_data
 */
#define EFUSE_RD_KEY1_DATA_ERR_NUM    0x00000007U
#define EFUSE_RD_KEY1_DATA_ERR_NUM_M  (EFUSE_RD_KEY1_DATA_ERR_NUM_V << EFUSE_RD_KEY1_DATA_ERR_NUM_S)
#define EFUSE_RD_KEY1_DATA_ERR_NUM_V  0x00000007U
#define EFUSE_RD_KEY1_DATA_ERR_NUM_S  16
/** EFUSE_RD_KEY1_DATA_FAIL : RO; bitpos: [19]; default: 0;
 *  Represents error status of register.
 *  0: Means no failure and that the data of rd_key1_data is reliable
 *  1: Means that programming rd_key1_data failed and the number of error bytes is over
 *  6.
 */
#define EFUSE_RD_KEY1_DATA_FAIL    (BIT(19))
#define EFUSE_RD_KEY1_DATA_FAIL_M  (EFUSE_RD_KEY1_DATA_FAIL_V << EFUSE_RD_KEY1_DATA_FAIL_S)
#define EFUSE_RD_KEY1_DATA_FAIL_V  0x00000001U
#define EFUSE_RD_KEY1_DATA_FAIL_S  19
/** EFUSE_RD_KEY2_DATA_ERR_NUM : RO; bitpos: [22:20]; default: 0;
 *  Represents the error number of registers.
 *  The value of this signal means the number of error bytes in rd_key2_data
 */
#define EFUSE_RD_KEY2_DATA_ERR_NUM    0x00000007U
#define EFUSE_RD_KEY2_DATA_ERR_NUM_M  (EFUSE_RD_KEY2_DATA_ERR_NUM_V << EFUSE_RD_KEY2_DATA_ERR_NUM_S)
#define EFUSE_RD_KEY2_DATA_ERR_NUM_V  0x00000007U
#define EFUSE_RD_KEY2_DATA_ERR_NUM_S  20
/** EFUSE_RD_KEY2_DATA_FAIL : RO; bitpos: [23]; default: 0;
 *  Represents error status of register.
 *  0: Means no failure and that the data of rd_key2_data is reliable
 *  1: Means that programming rd_key2_data failed and the number of error bytes is over
 *  6.
 */
#define EFUSE_RD_KEY2_DATA_FAIL    (BIT(23))
#define EFUSE_RD_KEY2_DATA_FAIL_M  (EFUSE_RD_KEY2_DATA_FAIL_V << EFUSE_RD_KEY2_DATA_FAIL_S)
#define EFUSE_RD_KEY2_DATA_FAIL_V  0x00000001U
#define EFUSE_RD_KEY2_DATA_FAIL_S  23
/** EFUSE_RD_KEY3_DATA_ERR_NUM : RO; bitpos: [26:24]; default: 0;
 *  Represents the error number of registers.
 *  The value of this signal means the number of error bytes in rd_key3_data
 */
#define EFUSE_RD_KEY3_DATA_ERR_NUM    0x00000007U
#define EFUSE_RD_KEY3_DATA_ERR_NUM_M  (EFUSE_RD_KEY3_DATA_ERR_NUM_V << EFUSE_RD_KEY3_DATA_ERR_NUM_S)
#define EFUSE_RD_KEY3_DATA_ERR_NUM_V  0x00000007U
#define EFUSE_RD_KEY3_DATA_ERR_NUM_S  24
/** EFUSE_RD_KEY3_DATA_FAIL : RO; bitpos: [27]; default: 0;
 *  Represents error status of register.
 *  0: Means no failure and that the data of rd_key3_data is reliable
 *  1: Means that programming rd_key3_data failed and the number of error bytes is over
 *  6.
 */
#define EFUSE_RD_KEY3_DATA_FAIL    (BIT(27))
#define EFUSE_RD_KEY3_DATA_FAIL_M  (EFUSE_RD_KEY3_DATA_FAIL_V << EFUSE_RD_KEY3_DATA_FAIL_S)
#define EFUSE_RD_KEY3_DATA_FAIL_V  0x00000001U
#define EFUSE_RD_KEY3_DATA_FAIL_S  27
/** EFUSE_RD_KEY4_DATA_ERR_NUM : RO; bitpos: [30:28]; default: 0;
 *  Represents the error number of registers.
 *  The value of this signal means the number of error bytes in rd_key4_data
 */
#define EFUSE_RD_KEY4_DATA_ERR_NUM    0x00000007U
#define EFUSE_RD_KEY4_DATA_ERR_NUM_M  (EFUSE_RD_KEY4_DATA_ERR_NUM_V << EFUSE_RD_KEY4_DATA_ERR_NUM_S)
#define EFUSE_RD_KEY4_DATA_ERR_NUM_V  0x00000007U
#define EFUSE_RD_KEY4_DATA_ERR_NUM_S  28
/** EFUSE_RD_KEY4_DATA_FAIL : RO; bitpos: [31]; default: 0;
 *  Represents error status of register.
 *  0: Means no failure and that the data of rd_key4_data is reliable
 *  1: Means that programming rd_key4_data failed and the number of error bytes is over
 *  6.
 */
#define EFUSE_RD_KEY4_DATA_FAIL    (BIT(31))
#define EFUSE_RD_KEY4_DATA_FAIL_M  (EFUSE_RD_KEY4_DATA_FAIL_V << EFUSE_RD_KEY4_DATA_FAIL_S)
#define EFUSE_RD_KEY4_DATA_FAIL_V  0x00000001U
#define EFUSE_RD_KEY4_DATA_FAIL_S  31

/** EFUSE_RD_RS_DATA_ERR1_REG register
 *  Represents rd_rs_data_err
 */
#define EFUSE_RD_RS_DATA_ERR1_REG (DR_REG_EFUSE_BASE + 0x194)
/** EFUSE_RD_KEY5_DATA_ERR_NUM : RO; bitpos: [2:0]; default: 0;
 *  Represents the error number of registers.
 *  The value of this signal means the number of error bytes in rd_key5_data
 */
#define EFUSE_RD_KEY5_DATA_ERR_NUM    0x00000007U
#define EFUSE_RD_KEY5_DATA_ERR_NUM_M  (EFUSE_RD_KEY5_DATA_ERR_NUM_V << EFUSE_RD_KEY5_DATA_ERR_NUM_S)
#define EFUSE_RD_KEY5_DATA_ERR_NUM_V  0x00000007U
#define EFUSE_RD_KEY5_DATA_ERR_NUM_S  0
/** EFUSE_RD_KEY5_DATA_FAIL : RO; bitpos: [3]; default: 0;
 *  Represents error status of register.
 *  0: Means no failure and that the data of rd_key5_data is reliable
 *  1: Means that programming rd_key5_data failed and the number of error bytes is over
 *  6.
 */
#define EFUSE_RD_KEY5_DATA_FAIL    (BIT(3))
#define EFUSE_RD_KEY5_DATA_FAIL_M  (EFUSE_RD_KEY5_DATA_FAIL_V << EFUSE_RD_KEY5_DATA_FAIL_S)
#define EFUSE_RD_KEY5_DATA_FAIL_V  0x00000001U
#define EFUSE_RD_KEY5_DATA_FAIL_S  3
/** EFUSE_RD_SYS_PART2_DATA_ERR_NUM : RO; bitpos: [6:4]; default: 0;
 *  Represents the error number of registers.
 *  The value of this signal means the number of error bytes in rd_sys_part2_data
 */
#define EFUSE_RD_SYS_PART2_DATA_ERR_NUM    0x00000007U
#define EFUSE_RD_SYS_PART2_DATA_ERR_NUM_M  (EFUSE_RD_SYS_PART2_DATA_ERR_NUM_V << EFUSE_RD_SYS_PART2_DATA_ERR_NUM_S)
#define EFUSE_RD_SYS_PART2_DATA_ERR_NUM_V  0x00000007U
#define EFUSE_RD_SYS_PART2_DATA_ERR_NUM_S  4
/** EFUSE_RD_SYS_PART2_DATA_FAIL : RO; bitpos: [7]; default: 0;
 *  Represents error status of register.
 *  0: Means no failure and that the data of rd_sys_part2_data is reliable
 *  1: Means that programming rd_sys_part2_data failed and the number of error bytes is
 *  over 6.
 */
#define EFUSE_RD_SYS_PART2_DATA_FAIL    (BIT(7))
#define EFUSE_RD_SYS_PART2_DATA_FAIL_M  (EFUSE_RD_SYS_PART2_DATA_FAIL_V << EFUSE_RD_SYS_PART2_DATA_FAIL_S)
#define EFUSE_RD_SYS_PART2_DATA_FAIL_V  0x00000001U
#define EFUSE_RD_SYS_PART2_DATA_FAIL_S  7

/** EFUSE_DATE_REG register
 *  eFuse version register.
 */
#define EFUSE_DATE_REG (DR_REG_EFUSE_BASE + 0x198)
/** EFUSE_DATE : R/W; bitpos: [27:0]; default: 38806112;
 *  Represents eFuse version.
 */
#define EFUSE_DATE    0x0FFFFFFFU
#define EFUSE_DATE_M  (EFUSE_DATE_V << EFUSE_DATE_S)
#define EFUSE_DATE_V  0x0FFFFFFFU
#define EFUSE_DATE_S  0

/** EFUSE_CLK_REG register
 *  eFuse clcok configuration register.
 */
#define EFUSE_CLK_REG (DR_REG_EFUSE_BASE + 0x1c8)
/** EFUSE_MEM_FORCE_PD : R/W; bitpos: [0]; default: 0;
 *  Configures whether to force power down eFuse SRAM.
 *  1: Force
 *  0: No effect
 */
#define EFUSE_MEM_FORCE_PD    (BIT(0))
#define EFUSE_MEM_FORCE_PD_M  (EFUSE_MEM_FORCE_PD_V << EFUSE_MEM_FORCE_PD_S)
#define EFUSE_MEM_FORCE_PD_V  0x00000001U
#define EFUSE_MEM_FORCE_PD_S  0
/** EFUSE_MEM_CLK_FORCE_ON : R/W; bitpos: [1]; default: 1;
 *  Configures whether to force activate clock signal of eFuse SRAM.
 *  1: Force activate
 *  0: No effect
 */
#define EFUSE_MEM_CLK_FORCE_ON    (BIT(1))
#define EFUSE_MEM_CLK_FORCE_ON_M  (EFUSE_MEM_CLK_FORCE_ON_V << EFUSE_MEM_CLK_FORCE_ON_S)
#define EFUSE_MEM_CLK_FORCE_ON_V  0x00000001U
#define EFUSE_MEM_CLK_FORCE_ON_S  1
/** EFUSE_MEM_FORCE_PU : R/W; bitpos: [2]; default: 0;
 *  Configures whether to force power up eFuse SRAM.
 *  1: Force
 *  0: No effect
 */
#define EFUSE_MEM_FORCE_PU    (BIT(2))
#define EFUSE_MEM_FORCE_PU_M  (EFUSE_MEM_FORCE_PU_V << EFUSE_MEM_FORCE_PU_S)
#define EFUSE_MEM_FORCE_PU_V  0x00000001U
#define EFUSE_MEM_FORCE_PU_S  2
/** EFUSE_CLK_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether to force enable eFuse register configuration clock signal.
 *  1: Force
 *  0: The clock is enabled only during the reading and writing of registers
 */
#define EFUSE_CLK_EN    (BIT(16))
#define EFUSE_CLK_EN_M  (EFUSE_CLK_EN_V << EFUSE_CLK_EN_S)
#define EFUSE_CLK_EN_V  0x00000001U
#define EFUSE_CLK_EN_S  16

/** EFUSE_CONF_REG register
 *  eFuse operation mode configuration register
 */
#define EFUSE_CONF_REG (DR_REG_EFUSE_BASE + 0x1cc)
/** EFUSE_OP_CODE : R/W; bitpos: [15:0]; default: 0;
 *  Configures operation command type.
 *  0x5A5A: Program operation command
 *  0x5AA5: Read operation command
 *  Other values: No effect
 */
#define EFUSE_OP_CODE    0x0000FFFFU
#define EFUSE_OP_CODE_M  (EFUSE_OP_CODE_V << EFUSE_OP_CODE_S)
#define EFUSE_OP_CODE_V  0x0000FFFFU
#define EFUSE_OP_CODE_S  0
/** EFUSE_CFG_ECDSA_L_BLK : R/W; bitpos: [19:16]; default: 0;
 *  Configures which block to use for ECDSA key low part output.
 */
#define EFUSE_CFG_ECDSA_L_BLK    0x0000000FU
#define EFUSE_CFG_ECDSA_L_BLK_M  (EFUSE_CFG_ECDSA_L_BLK_V << EFUSE_CFG_ECDSA_L_BLK_S)
#define EFUSE_CFG_ECDSA_L_BLK_V  0x0000000FU
#define EFUSE_CFG_ECDSA_L_BLK_S  16
/** EFUSE_CFG_ECDSA_H_BLK : R/W; bitpos: [23:20]; default: 0;
 *  Configures which block to use for ECDSA key high part output.
 */
#define EFUSE_CFG_ECDSA_H_BLK    0x0000000FU
#define EFUSE_CFG_ECDSA_H_BLK_M  (EFUSE_CFG_ECDSA_H_BLK_V << EFUSE_CFG_ECDSA_H_BLK_S)
#define EFUSE_CFG_ECDSA_H_BLK_V  0x0000000FU
#define EFUSE_CFG_ECDSA_H_BLK_S  20

/** EFUSE_STATUS_REG register
 *  eFuse status register.
 */
#define EFUSE_STATUS_REG (DR_REG_EFUSE_BASE + 0x1d0)
/** EFUSE_STATE : RO; bitpos: [3:0]; default: 0;
 *  Represents the state of the eFuse state machine.
 *  0: Reset state, the initial state after power-up
 *  1: Idle state
 *  Other values: Non-idle state
 */
#define EFUSE_STATE    0x0000000FU
#define EFUSE_STATE_M  (EFUSE_STATE_V << EFUSE_STATE_S)
#define EFUSE_STATE_V  0x0000000FU
#define EFUSE_STATE_S  0
/** EFUSE_BLK0_VALID_BIT_CNT : RO; bitpos: [19:10]; default: 0;
 *  Represents the number of block valid bit.
 */
#define EFUSE_BLK0_VALID_BIT_CNT    0x000003FFU
#define EFUSE_BLK0_VALID_BIT_CNT_M  (EFUSE_BLK0_VALID_BIT_CNT_V << EFUSE_BLK0_VALID_BIT_CNT_S)
#define EFUSE_BLK0_VALID_BIT_CNT_V  0x000003FFU
#define EFUSE_BLK0_VALID_BIT_CNT_S  10
/** EFUSE_CUR_ECDSA_L_BLK : RO; bitpos: [23:20]; default: 0;
 *  Represents which block is used for ECDSA key low part output.
 */
#define EFUSE_CUR_ECDSA_L_BLK    0x0000000FU
#define EFUSE_CUR_ECDSA_L_BLK_M  (EFUSE_CUR_ECDSA_L_BLK_V << EFUSE_CUR_ECDSA_L_BLK_S)
#define EFUSE_CUR_ECDSA_L_BLK_V  0x0000000FU
#define EFUSE_CUR_ECDSA_L_BLK_S  20
/** EFUSE_CUR_ECDSA_H_BLK : RO; bitpos: [27:24]; default: 0;
 *  Represents which block is used for ECDSA key high part output.
 */
#define EFUSE_CUR_ECDSA_H_BLK    0x0000000FU
#define EFUSE_CUR_ECDSA_H_BLK_M  (EFUSE_CUR_ECDSA_H_BLK_V << EFUSE_CUR_ECDSA_H_BLK_S)
#define EFUSE_CUR_ECDSA_H_BLK_V  0x0000000FU
#define EFUSE_CUR_ECDSA_H_BLK_S  24

/** EFUSE_CMD_REG register
 *  eFuse command register.
 */
#define EFUSE_CMD_REG (DR_REG_EFUSE_BASE + 0x1d4)
/** EFUSE_READ_CMD : R/W/SC; bitpos: [0]; default: 0;
 *  Configures whether to send read commands.
 *  1: Send
 *  0: No effect
 */
#define EFUSE_READ_CMD    (BIT(0))
#define EFUSE_READ_CMD_M  (EFUSE_READ_CMD_V << EFUSE_READ_CMD_S)
#define EFUSE_READ_CMD_V  0x00000001U
#define EFUSE_READ_CMD_S  0
/** EFUSE_PGM_CMD : R/W/SC; bitpos: [1]; default: 0;
 *  Configures whether to send programming commands.
 *  1: Send
 *  0: No effect
 */
#define EFUSE_PGM_CMD    (BIT(1))
#define EFUSE_PGM_CMD_M  (EFUSE_PGM_CMD_V << EFUSE_PGM_CMD_S)
#define EFUSE_PGM_CMD_V  0x00000001U
#define EFUSE_PGM_CMD_S  1
/** EFUSE_BLK_NUM : R/W; bitpos: [5:2]; default: 0;
 *  Configures the serial number of the block to be programmed. Value 0-10 corresponds
 *  to block number 0-10, respectively.
 */
#define EFUSE_BLK_NUM    0x0000000FU
#define EFUSE_BLK_NUM_M  (EFUSE_BLK_NUM_V << EFUSE_BLK_NUM_S)
#define EFUSE_BLK_NUM_V  0x0000000FU
#define EFUSE_BLK_NUM_S  2

/** EFUSE_INT_RAW_REG register
 *  eFuse raw interrupt register.
 */
#define EFUSE_INT_RAW_REG (DR_REG_EFUSE_BASE + 0x1d8)
/** EFUSE_READ_DONE_INT_RAW : R/SS/WTC; bitpos: [0]; default: 0;
 *  The raw interrupt status of EFUSE_READ_DONE_INT.
 */
#define EFUSE_READ_DONE_INT_RAW    (BIT(0))
#define EFUSE_READ_DONE_INT_RAW_M  (EFUSE_READ_DONE_INT_RAW_V << EFUSE_READ_DONE_INT_RAW_S)
#define EFUSE_READ_DONE_INT_RAW_V  0x00000001U
#define EFUSE_READ_DONE_INT_RAW_S  0
/** EFUSE_PGM_DONE_INT_RAW : R/SS/WTC; bitpos: [1]; default: 0;
 *  The raw interrupt status of EFUSE_PGM_DONE_INT.
 */
#define EFUSE_PGM_DONE_INT_RAW    (BIT(1))
#define EFUSE_PGM_DONE_INT_RAW_M  (EFUSE_PGM_DONE_INT_RAW_V << EFUSE_PGM_DONE_INT_RAW_S)
#define EFUSE_PGM_DONE_INT_RAW_V  0x00000001U
#define EFUSE_PGM_DONE_INT_RAW_S  1

/** EFUSE_INT_ST_REG register
 *  eFuse interrupt status register.
 */
#define EFUSE_INT_ST_REG (DR_REG_EFUSE_BASE + 0x1dc)
/** EFUSE_READ_DONE_INT_ST : RO; bitpos: [0]; default: 0;
 *  The masked interrupt status of EFUSE_READ_DONE_INT.
 */
#define EFUSE_READ_DONE_INT_ST    (BIT(0))
#define EFUSE_READ_DONE_INT_ST_M  (EFUSE_READ_DONE_INT_ST_V << EFUSE_READ_DONE_INT_ST_S)
#define EFUSE_READ_DONE_INT_ST_V  0x00000001U
#define EFUSE_READ_DONE_INT_ST_S  0
/** EFUSE_PGM_DONE_INT_ST : RO; bitpos: [1]; default: 0;
 *  The masked interrupt status of EFUSE_PGM_DONE_INT.
 */
#define EFUSE_PGM_DONE_INT_ST    (BIT(1))
#define EFUSE_PGM_DONE_INT_ST_M  (EFUSE_PGM_DONE_INT_ST_V << EFUSE_PGM_DONE_INT_ST_S)
#define EFUSE_PGM_DONE_INT_ST_V  0x00000001U
#define EFUSE_PGM_DONE_INT_ST_S  1

/** EFUSE_INT_ENA_REG register
 *  eFuse interrupt enable register.
 */
#define EFUSE_INT_ENA_REG (DR_REG_EFUSE_BASE + 0x1e0)
/** EFUSE_READ_DONE_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  Write 1 to enable EFUSE_READ_DONE_INT.
 */
#define EFUSE_READ_DONE_INT_ENA    (BIT(0))
#define EFUSE_READ_DONE_INT_ENA_M  (EFUSE_READ_DONE_INT_ENA_V << EFUSE_READ_DONE_INT_ENA_S)
#define EFUSE_READ_DONE_INT_ENA_V  0x00000001U
#define EFUSE_READ_DONE_INT_ENA_S  0
/** EFUSE_PGM_DONE_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  Write 1 to enable EFUSE_PGM_DONE_INT.
 */
#define EFUSE_PGM_DONE_INT_ENA    (BIT(1))
#define EFUSE_PGM_DONE_INT_ENA_M  (EFUSE_PGM_DONE_INT_ENA_V << EFUSE_PGM_DONE_INT_ENA_S)
#define EFUSE_PGM_DONE_INT_ENA_V  0x00000001U
#define EFUSE_PGM_DONE_INT_ENA_S  1

/** EFUSE_INT_CLR_REG register
 *  eFuse interrupt clear register.
 */
#define EFUSE_INT_CLR_REG (DR_REG_EFUSE_BASE + 0x1e4)
/** EFUSE_READ_DONE_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Write 1 to clear EFUSE_READ_DONE_INT.
 */
#define EFUSE_READ_DONE_INT_CLR    (BIT(0))
#define EFUSE_READ_DONE_INT_CLR_M  (EFUSE_READ_DONE_INT_CLR_V << EFUSE_READ_DONE_INT_CLR_S)
#define EFUSE_READ_DONE_INT_CLR_V  0x00000001U
#define EFUSE_READ_DONE_INT_CLR_S  0
/** EFUSE_PGM_DONE_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Write 1 to clear EFUSE_PGM_DONE_INT.
 */
#define EFUSE_PGM_DONE_INT_CLR    (BIT(1))
#define EFUSE_PGM_DONE_INT_CLR_M  (EFUSE_PGM_DONE_INT_CLR_V << EFUSE_PGM_DONE_INT_CLR_S)
#define EFUSE_PGM_DONE_INT_CLR_V  0x00000001U
#define EFUSE_PGM_DONE_INT_CLR_S  1

/** EFUSE_DAC_CONF_REG register
 *  Controls the eFuse programming voltage.
 */
#define EFUSE_DAC_CONF_REG (DR_REG_EFUSE_BASE + 0x1e8)
/** EFUSE_DAC_CLK_DIV : R/W; bitpos: [7:0]; default: 19;
 *  Configures the division factor of the rising clock of the programming voltage.
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
 *  Configures clock cycles for programming voltage to rise. Measurement unit: a clock
 *  cycle divided by EFUSE_DAC_CLK_DIV.
 */
#define EFUSE_DAC_NUM    0x000000FFU
#define EFUSE_DAC_NUM_M  (EFUSE_DAC_NUM_V << EFUSE_DAC_NUM_S)
#define EFUSE_DAC_NUM_V  0x000000FFU
#define EFUSE_DAC_NUM_S  9
/** EFUSE_OE_CLR : R/W; bitpos: [17]; default: 0;
 *  Configures whether to reduce the power supply of programming voltage.
 *  0: Not reduce
 *  1: Reduce
 */
#define EFUSE_OE_CLR    (BIT(17))
#define EFUSE_OE_CLR_M  (EFUSE_OE_CLR_V << EFUSE_OE_CLR_S)
#define EFUSE_OE_CLR_V  0x00000001U
#define EFUSE_OE_CLR_S  17

/** EFUSE_RD_TIM_CONF_REG register
 *  Configures read timing parameters.
 */
#define EFUSE_RD_TIM_CONF_REG (DR_REG_EFUSE_BASE + 0x1ec)
/** EFUSE_THR_A : R/W; bitpos: [7:0]; default: 1;
 *  Configures the read hold time. Measurement unit: One cycle of the eFuse core clock.
 */
#define EFUSE_THR_A    0x000000FFU
#define EFUSE_THR_A_M  (EFUSE_THR_A_V << EFUSE_THR_A_S)
#define EFUSE_THR_A_V  0x000000FFU
#define EFUSE_THR_A_S  0
/** EFUSE_TRD : R/W; bitpos: [15:8]; default: 2;
 *  Configures the read time. Measurement unit: One cycle of the eFuse core clock.
 */
#define EFUSE_TRD    0x000000FFU
#define EFUSE_TRD_M  (EFUSE_TRD_V << EFUSE_TRD_S)
#define EFUSE_TRD_V  0x000000FFU
#define EFUSE_TRD_S  8
/** EFUSE_TSUR_A : R/W; bitpos: [23:16]; default: 1;
 *  Configures the read setup time. Measurement unit: One cycle of the eFuse core clock.
 */
#define EFUSE_TSUR_A    0x000000FFU
#define EFUSE_TSUR_A_M  (EFUSE_TSUR_A_V << EFUSE_TSUR_A_S)
#define EFUSE_TSUR_A_V  0x000000FFU
#define EFUSE_TSUR_A_S  16
/** EFUSE_READ_INIT_NUM : R/W; bitpos: [31:24]; default: 18;
 *  Configures the waiting time of reading eFuse memory. Measurement unit: One cycle of
 *  the eFuse core clock.
 */
#define EFUSE_READ_INIT_NUM    0x000000FFU
#define EFUSE_READ_INIT_NUM_M  (EFUSE_READ_INIT_NUM_V << EFUSE_READ_INIT_NUM_S)
#define EFUSE_READ_INIT_NUM_V  0x000000FFU
#define EFUSE_READ_INIT_NUM_S  24

/** EFUSE_WR_TIM_CONF1_REG register
 *  Configurarion register 1 of eFuse programming timing parameters.
 */
#define EFUSE_WR_TIM_CONF1_REG (DR_REG_EFUSE_BASE + 0x1f0)
/** EFUSE_TSUP_A : R/W; bitpos: [7:0]; default: 1;
 *  Configures the programming setup time. Measurement unit: One cycle of the eFuse
 *  core clock.
 */
#define EFUSE_TSUP_A    0x000000FFU
#define EFUSE_TSUP_A_M  (EFUSE_TSUP_A_V << EFUSE_TSUP_A_S)
#define EFUSE_TSUP_A_V  0x000000FFU
#define EFUSE_TSUP_A_S  0
/** EFUSE_PWR_ON_NUM : R/W; bitpos: [23:8]; default: 9831;
 *  Configures the power up time for VDDQ. Measurement unit: One cycle of the eFuse
 *  core clock.
 */
#define EFUSE_PWR_ON_NUM    0x0000FFFFU
#define EFUSE_PWR_ON_NUM_M  (EFUSE_PWR_ON_NUM_V << EFUSE_PWR_ON_NUM_S)
#define EFUSE_PWR_ON_NUM_V  0x0000FFFFU
#define EFUSE_PWR_ON_NUM_S  8
/** EFUSE_THP_A : R/W; bitpos: [31:24]; default: 1;
 *  Configures the programming hold time. Measurement unit: One cycle of the eFuse core
 *  clock.
 */
#define EFUSE_THP_A    0x000000FFU
#define EFUSE_THP_A_M  (EFUSE_THP_A_V << EFUSE_THP_A_S)
#define EFUSE_THP_A_V  0x000000FFU
#define EFUSE_THP_A_S  24

/** EFUSE_WR_TIM_CONF2_REG register
 *  Configurarion register 2 of eFuse programming timing parameters.
 */
#define EFUSE_WR_TIM_CONF2_REG (DR_REG_EFUSE_BASE + 0x1f4)
/** EFUSE_PWR_OFF_NUM : R/W; bitpos: [15:0]; default: 320;
 *  Configures the power outage time for VDDQ. Measurement unit: One cycle of the eFuse
 *  core clock.
 */
#define EFUSE_PWR_OFF_NUM    0x0000FFFFU
#define EFUSE_PWR_OFF_NUM_M  (EFUSE_PWR_OFF_NUM_V << EFUSE_PWR_OFF_NUM_S)
#define EFUSE_PWR_OFF_NUM_V  0x0000FFFFU
#define EFUSE_PWR_OFF_NUM_S  0
/** EFUSE_TPGM : R/W; bitpos: [31:16]; default: 160;
 *  Configures the active programming time. Measurement unit: One cycle of the eFuse
 *  core clock.
 */
#define EFUSE_TPGM    0x0000FFFFU
#define EFUSE_TPGM_M  (EFUSE_TPGM_V << EFUSE_TPGM_S)
#define EFUSE_TPGM_V  0x0000FFFFU
#define EFUSE_TPGM_S  16

/** EFUSE_WR_TIM_CONF0_RS_BYPASS_REG register
 *  Configurarion register0 of eFuse programming time parameters and rs bypass
 *  operation.
 */
#define EFUSE_WR_TIM_CONF0_RS_BYPASS_REG (DR_REG_EFUSE_BASE + 0x1f8)
/** EFUSE_BYPASS_RS_CORRECTION : R/W; bitpos: [0]; default: 0;
 *  Configures whether to bypass the Reed-Solomon (RS) correction step.
 *  0: Not bypass
 *  1: Bypass
 */
#define EFUSE_BYPASS_RS_CORRECTION    (BIT(0))
#define EFUSE_BYPASS_RS_CORRECTION_M  (EFUSE_BYPASS_RS_CORRECTION_V << EFUSE_BYPASS_RS_CORRECTION_S)
#define EFUSE_BYPASS_RS_CORRECTION_V  0x00000001U
#define EFUSE_BYPASS_RS_CORRECTION_S  0
/** EFUSE_BYPASS_RS_BLK_NUM : R/W; bitpos: [11:1]; default: 0;
 *  Configures which block number to bypass the Reed-Solomon (RS) correction step.
 */
#define EFUSE_BYPASS_RS_BLK_NUM    0x000007FFU
#define EFUSE_BYPASS_RS_BLK_NUM_M  (EFUSE_BYPASS_RS_BLK_NUM_V << EFUSE_BYPASS_RS_BLK_NUM_S)
#define EFUSE_BYPASS_RS_BLK_NUM_V  0x000007FFU
#define EFUSE_BYPASS_RS_BLK_NUM_S  1
/** EFUSE_UPDATE : WT; bitpos: [12]; default: 0;
 *  Configures whether to update multi-bit register signals.
 *  1: Update
 *  0: No effect
 */
#define EFUSE_UPDATE    (BIT(12))
#define EFUSE_UPDATE_M  (EFUSE_UPDATE_V << EFUSE_UPDATE_S)
#define EFUSE_UPDATE_V  0x00000001U
#define EFUSE_UPDATE_S  12
/** EFUSE_TPGM_INACTIVE : R/W; bitpos: [20:13]; default: 1;
 *  Configures the inactive programming time. Measurement unit: One cycle of the eFuse
 *  core clock.
 */
#define EFUSE_TPGM_INACTIVE    0x000000FFU
#define EFUSE_TPGM_INACTIVE_M  (EFUSE_TPGM_INACTIVE_V << EFUSE_TPGM_INACTIVE_S)
#define EFUSE_TPGM_INACTIVE_V  0x000000FFU
#define EFUSE_TPGM_INACTIVE_S  13

#ifdef __cplusplus
}
#endif
