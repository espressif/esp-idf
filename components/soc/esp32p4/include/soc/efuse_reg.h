/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#include "efuse_defs.h"
#ifdef __cplusplus
extern "C" {
#endif

/** EFUSE_PGM_DATA0_REG register
 *  Register 0 that stores data to be programmed.
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
 *  Register 1 that stores data to be programmed.
 */
#define EFUSE_PGM_DATA1_REG (DR_REG_EFUSE_BASE + 0x4)
/** EFUSE_PGM_DATA_1 : R/W; bitpos: [31:0]; default: 0;
 *  Configures the 1st 32-bit data to be programmed.
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
 *  Configures the 2nd 32-bit data to be programmed.
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
 *  Configures the 3rd 32-bit data to be programmed.
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
 *  Configures the 4th 32-bit data to be programmed.
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
 *  Configures the 5th 32-bit data to be programmed.
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
 *  Configures the 6th 32-bit data to be programmed.
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
 *  Configures the 7th 32-bit data to be programmed.
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
 *  Configures the 0th 32-bit RS code to be programmed.
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
 *  Configures the 1st 32-bit RS code to be programmed.
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
 *  Configures the 2nd 32-bit RS code to be programmed.
 */
#define EFUSE_PGM_RS_DATA_2    0xFFFFFFFFU
#define EFUSE_PGM_RS_DATA_2_M  (EFUSE_PGM_RS_DATA_2_V << EFUSE_PGM_RS_DATA_2_S)
#define EFUSE_PGM_RS_DATA_2_V  0xFFFFFFFFU
#define EFUSE_PGM_RS_DATA_2_S  0

/** EFUSE_RD_WR_DIS_REG register
 *  BLOCK0 data register 0.
 */
#define EFUSE_RD_WR_DIS_REG (DR_REG_EFUSE_BASE + 0x2c)
/** EFUSE_WR_DIS : RO; bitpos: [31:0]; default: 0;
 *  Represents whether programming of individual eFuse memory bit is disabled or
 *  enabled. 1: Disabled. 0 Enabled.
 */
#define EFUSE_WR_DIS    0xFFFFFFFFU
#define EFUSE_WR_DIS_M  (EFUSE_WR_DIS_V << EFUSE_WR_DIS_S)
#define EFUSE_WR_DIS_V  0xFFFFFFFFU
#define EFUSE_WR_DIS_S  0

/** EFUSE_RD_REPEAT_DATA0_REG register
 *  BLOCK0 data register 1.
 */
#define EFUSE_RD_REPEAT_DATA0_REG (DR_REG_EFUSE_BASE + 0x30)
/** EFUSE_RD_DIS : RO; bitpos: [6:0]; default: 0;
 *  Represents whether reading of individual eFuse block(block4~block10) is disabled or
 *  enabled. 1: disabled. 0: enabled.
 */
#define EFUSE_RD_DIS    0x0000007FU
#define EFUSE_RD_DIS_M  (EFUSE_RD_DIS_V << EFUSE_RD_DIS_S)
#define EFUSE_RD_DIS_V  0x0000007FU
#define EFUSE_RD_DIS_S  0
/** EFUSE_USB_DEVICE_EXCHG_PINS : RO; bitpos: [7]; default: 0;
 *  Enable usb device exchange pins of D+ and D-.
 */
#define EFUSE_USB_DEVICE_EXCHG_PINS    (BIT(7))
#define EFUSE_USB_DEVICE_EXCHG_PINS_M  (EFUSE_USB_DEVICE_EXCHG_PINS_V << EFUSE_USB_DEVICE_EXCHG_PINS_S)
#define EFUSE_USB_DEVICE_EXCHG_PINS_V  0x00000001U
#define EFUSE_USB_DEVICE_EXCHG_PINS_S  7
/** EFUSE_USB_OTG11_EXCHG_PINS : RO; bitpos: [8]; default: 0;
 *  Enable usb otg11 exchange pins of D+ and D-.
 */
#define EFUSE_USB_OTG11_EXCHG_PINS    (BIT(8))
#define EFUSE_USB_OTG11_EXCHG_PINS_M  (EFUSE_USB_OTG11_EXCHG_PINS_V << EFUSE_USB_OTG11_EXCHG_PINS_S)
#define EFUSE_USB_OTG11_EXCHG_PINS_V  0x00000001U
#define EFUSE_USB_OTG11_EXCHG_PINS_S  8
/** EFUSE_DIS_USB_JTAG : RO; bitpos: [9]; default: 0;
 *  Represents whether the function of usb switch to jtag is disabled or enabled. 1:
 *  disabled. 0: enabled.
 */
#define EFUSE_DIS_USB_JTAG    (BIT(9))
#define EFUSE_DIS_USB_JTAG_M  (EFUSE_DIS_USB_JTAG_V << EFUSE_DIS_USB_JTAG_S)
#define EFUSE_DIS_USB_JTAG_V  0x00000001U
#define EFUSE_DIS_USB_JTAG_S  9
/** EFUSE_POWERGLITCH_EN : RO; bitpos: [10]; default: 0;
 *  Represents whether power glitch function is enabled. 1: enabled. 0: disabled.
 */
#define EFUSE_POWERGLITCH_EN    (BIT(10))
#define EFUSE_POWERGLITCH_EN_M  (EFUSE_POWERGLITCH_EN_V << EFUSE_POWERGLITCH_EN_S)
#define EFUSE_POWERGLITCH_EN_V  0x00000001U
#define EFUSE_POWERGLITCH_EN_S  10
/** EFUSE_DIS_USB_SERIAL_JTAG : RO; bitpos: [11]; default: 0;
 *  Represents whether USB-Serial-JTAG is disabled or enabled. 1: disabled. 0: enabled.
 */
#define EFUSE_DIS_USB_SERIAL_JTAG    (BIT(11))
#define EFUSE_DIS_USB_SERIAL_JTAG_M  (EFUSE_DIS_USB_SERIAL_JTAG_V << EFUSE_DIS_USB_SERIAL_JTAG_S)
#define EFUSE_DIS_USB_SERIAL_JTAG_V  0x00000001U
#define EFUSE_DIS_USB_SERIAL_JTAG_S  11
/** EFUSE_DIS_FORCE_DOWNLOAD : RO; bitpos: [12]; default: 0;
 *  Represents whether the function that forces chip into download mode is disabled or
 *  enabled. 1: disabled. 0: enabled.
 */
#define EFUSE_DIS_FORCE_DOWNLOAD    (BIT(12))
#define EFUSE_DIS_FORCE_DOWNLOAD_M  (EFUSE_DIS_FORCE_DOWNLOAD_V << EFUSE_DIS_FORCE_DOWNLOAD_S)
#define EFUSE_DIS_FORCE_DOWNLOAD_V  0x00000001U
#define EFUSE_DIS_FORCE_DOWNLOAD_S  12
/** EFUSE_SPI_DOWNLOAD_MSPI_DIS : RO; bitpos: [13]; default: 0;
 *  Set this bit to disable accessing MSPI flash/MSPI ram by SYS AXI matrix during
 *  boot_mode_download.
 */
#define EFUSE_SPI_DOWNLOAD_MSPI_DIS    (BIT(13))
#define EFUSE_SPI_DOWNLOAD_MSPI_DIS_M  (EFUSE_SPI_DOWNLOAD_MSPI_DIS_V << EFUSE_SPI_DOWNLOAD_MSPI_DIS_S)
#define EFUSE_SPI_DOWNLOAD_MSPI_DIS_V  0x00000001U
#define EFUSE_SPI_DOWNLOAD_MSPI_DIS_S  13
/** EFUSE_DIS_TWAI : RO; bitpos: [14]; default: 0;
 *  Represents whether TWAI function is disabled or enabled. 1: disabled. 0: enabled.
 */
#define EFUSE_DIS_TWAI    (BIT(14))
#define EFUSE_DIS_TWAI_M  (EFUSE_DIS_TWAI_V << EFUSE_DIS_TWAI_S)
#define EFUSE_DIS_TWAI_V  0x00000001U
#define EFUSE_DIS_TWAI_S  14
/** EFUSE_JTAG_SEL_ENABLE : RO; bitpos: [15]; default: 0;
 *  Represents whether the selection between usb_to_jtag and pad_to_jtag through
 *  strapping gpio15 when both EFUSE_DIS_PAD_JTAG and EFUSE_DIS_USB_JTAG are equal to 0
 *  is enabled or disabled. 1: enabled. 0: disabled.
 */
#define EFUSE_JTAG_SEL_ENABLE    (BIT(15))
#define EFUSE_JTAG_SEL_ENABLE_M  (EFUSE_JTAG_SEL_ENABLE_V << EFUSE_JTAG_SEL_ENABLE_S)
#define EFUSE_JTAG_SEL_ENABLE_V  0x00000001U
#define EFUSE_JTAG_SEL_ENABLE_S  15
/** EFUSE_SOFT_DIS_JTAG : RO; bitpos: [18:16]; default: 0;
 *  Represents whether JTAG is disabled in soft way. Odd number: disabled. Even number:
 *  enabled.
 */
#define EFUSE_SOFT_DIS_JTAG    0x00000007U
#define EFUSE_SOFT_DIS_JTAG_M  (EFUSE_SOFT_DIS_JTAG_V << EFUSE_SOFT_DIS_JTAG_S)
#define EFUSE_SOFT_DIS_JTAG_V  0x00000007U
#define EFUSE_SOFT_DIS_JTAG_S  16
/** EFUSE_DIS_PAD_JTAG : RO; bitpos: [19]; default: 0;
 *  Represents whether JTAG is disabled in the hard way(permanently). 1: disabled. 0:
 *  enabled.
 */
#define EFUSE_DIS_PAD_JTAG    (BIT(19))
#define EFUSE_DIS_PAD_JTAG_M  (EFUSE_DIS_PAD_JTAG_V << EFUSE_DIS_PAD_JTAG_S)
#define EFUSE_DIS_PAD_JTAG_V  0x00000001U
#define EFUSE_DIS_PAD_JTAG_S  19
/** EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT : RO; bitpos: [20]; default: 0;
 *  Represents whether flash encrypt function is disabled or enabled(except in SPI boot
 *  mode). 1: disabled. 0: enabled.
 */
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT    (BIT(20))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_M  (EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_V << EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_S)
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_V  0x00000001U
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_S  20
/** EFUSE_USB_DEVICE_DREFH : RO; bitpos: [22:21]; default: 0;
 *  USB intphy of usb device signle-end input high threshold, 1.76V to 2V. Step by 80mV
 */
#define EFUSE_USB_DEVICE_DREFH    0x00000003U
#define EFUSE_USB_DEVICE_DREFH_M  (EFUSE_USB_DEVICE_DREFH_V << EFUSE_USB_DEVICE_DREFH_S)
#define EFUSE_USB_DEVICE_DREFH_V  0x00000003U
#define EFUSE_USB_DEVICE_DREFH_S  21
/** EFUSE_USB_OTG11_DREFH : RO; bitpos: [24:23]; default: 0;
 *  USB intphy of usb otg11 signle-end input high threshold, 1.76V to 2V. Step by 80mV
 */
#define EFUSE_USB_OTG11_DREFH    0x00000003U
#define EFUSE_USB_OTG11_DREFH_M  (EFUSE_USB_OTG11_DREFH_V << EFUSE_USB_OTG11_DREFH_S)
#define EFUSE_USB_OTG11_DREFH_V  0x00000003U
#define EFUSE_USB_OTG11_DREFH_S  23
/** EFUSE_USB_PHY_SEL : RO; bitpos: [25]; default: 0;
 *  TBD
 */
#define EFUSE_USB_PHY_SEL    (BIT(25))
#define EFUSE_USB_PHY_SEL_M  (EFUSE_USB_PHY_SEL_V << EFUSE_USB_PHY_SEL_S)
#define EFUSE_USB_PHY_SEL_V  0x00000001U
#define EFUSE_USB_PHY_SEL_S  25
/** EFUSE_KM_HUK_GEN_STATE_LOW : RO; bitpos: [31:26]; default: 0;
 *  Set this bit to control validation of HUK generate mode. Odd of 1 is invalid, even
 *  of 1 is valid.
 */
#define EFUSE_KM_HUK_GEN_STATE_LOW    0x0000003FU
#define EFUSE_KM_HUK_GEN_STATE_LOW_M  (EFUSE_KM_HUK_GEN_STATE_LOW_V << EFUSE_KM_HUK_GEN_STATE_LOW_S)
#define EFUSE_KM_HUK_GEN_STATE_LOW_V  0x0000003FU
#define EFUSE_KM_HUK_GEN_STATE_LOW_S  26

/** EFUSE_RD_REPEAT_DATA1_REG register
 *  BLOCK0 data register 2.
 */
#define EFUSE_RD_REPEAT_DATA1_REG (DR_REG_EFUSE_BASE + 0x34)
/** EFUSE_KM_HUK_GEN_STATE_HIGH : RO; bitpos: [2:0]; default: 0;
 *  Set this bit to control validation of HUK generate mode. Odd of 1 is invalid, even
 *  of 1 is valid.
 */
#define EFUSE_KM_HUK_GEN_STATE_HIGH    0x00000007U
#define EFUSE_KM_HUK_GEN_STATE_HIGH_M  (EFUSE_KM_HUK_GEN_STATE_HIGH_V << EFUSE_KM_HUK_GEN_STATE_HIGH_S)
#define EFUSE_KM_HUK_GEN_STATE_HIGH_V  0x00000007U
#define EFUSE_KM_HUK_GEN_STATE_HIGH_S  0
/** EFUSE_KM_RND_SWITCH_CYCLE : RO; bitpos: [4:3]; default: 0;
 *  Set bits to control key manager random number switch cycle. 0: control by register.
 *  1: 8 km clk cycles. 2: 16 km cycles. 3: 32 km cycles.
 */
#define EFUSE_KM_RND_SWITCH_CYCLE    0x00000003U
#define EFUSE_KM_RND_SWITCH_CYCLE_M  (EFUSE_KM_RND_SWITCH_CYCLE_V << EFUSE_KM_RND_SWITCH_CYCLE_S)
#define EFUSE_KM_RND_SWITCH_CYCLE_V  0x00000003U
#define EFUSE_KM_RND_SWITCH_CYCLE_S  3
/** EFUSE_KM_DEPLOY_ONLY_ONCE : RO; bitpos: [8:5]; default: 0;
 *  Set each bit to control whether corresponding key can only be deployed once. 1 is
 *  true, 0 is false. Bit0: ecdsa. Bit1: xts. Bit2: hmac. Bit3: ds.
 */
#define EFUSE_KM_DEPLOY_ONLY_ONCE    0x0000000FU
#define EFUSE_KM_DEPLOY_ONLY_ONCE_M  (EFUSE_KM_DEPLOY_ONLY_ONCE_V << EFUSE_KM_DEPLOY_ONLY_ONCE_S)
#define EFUSE_KM_DEPLOY_ONLY_ONCE_V  0x0000000FU
#define EFUSE_KM_DEPLOY_ONLY_ONCE_S  5
/** EFUSE_FORCE_USE_KEY_MANAGER_KEY : RO; bitpos: [12:9]; default: 0;
 *  Set each bit to control whether corresponding key must come from key manager.. 1 is
 *  true, 0 is false. Bit0: ecdsa. Bit1: xts. Bit2: hmac. Bit3: ds.
 */
#define EFUSE_FORCE_USE_KEY_MANAGER_KEY    0x0000000FU
#define EFUSE_FORCE_USE_KEY_MANAGER_KEY_M  (EFUSE_FORCE_USE_KEY_MANAGER_KEY_V << EFUSE_FORCE_USE_KEY_MANAGER_KEY_S)
#define EFUSE_FORCE_USE_KEY_MANAGER_KEY_V  0x0000000FU
#define EFUSE_FORCE_USE_KEY_MANAGER_KEY_S  9
/** EFUSE_FORCE_DISABLE_SW_INIT_KEY : RO; bitpos: [13]; default: 0;
 *  Set this bit to disable software written init key, and force use efuse_init_key.
 */
#define EFUSE_FORCE_DISABLE_SW_INIT_KEY    (BIT(13))
#define EFUSE_FORCE_DISABLE_SW_INIT_KEY_M  (EFUSE_FORCE_DISABLE_SW_INIT_KEY_V << EFUSE_FORCE_DISABLE_SW_INIT_KEY_S)
#define EFUSE_FORCE_DISABLE_SW_INIT_KEY_V  0x00000001U
#define EFUSE_FORCE_DISABLE_SW_INIT_KEY_S  13
/** EFUSE_XTS_KEY_LENGTH_256 : RO; bitpos: [14]; default: 0;
 *  Set this bit to configure flash encryption use xts-128 key, else use xts-256 key.
 */
#define EFUSE_XTS_KEY_LENGTH_256    (BIT(14))
#define EFUSE_XTS_KEY_LENGTH_256_M  (EFUSE_XTS_KEY_LENGTH_256_V << EFUSE_XTS_KEY_LENGTH_256_S)
#define EFUSE_XTS_KEY_LENGTH_256_V  0x00000001U
#define EFUSE_XTS_KEY_LENGTH_256_S  14
/** EFUSE_RD_RESERVE_0_79 : RW; bitpos: [15]; default: 0;
 *  Reserved, it was created by set_missed_fields_in_regs func
 */
#define EFUSE_RD_RESERVE_0_79    (BIT(15))
#define EFUSE_RD_RESERVE_0_79_M  (EFUSE_RD_RESERVE_0_79_V << EFUSE_RD_RESERVE_0_79_S)
#define EFUSE_RD_RESERVE_0_79_V  0x00000001U
#define EFUSE_RD_RESERVE_0_79_S  15
/** EFUSE_WDT_DELAY_SEL : RO; bitpos: [17:16]; default: 0;
 *  Represents whether RTC watchdog timeout threshold is selected at startup. 1:
 *  selected. 0: not selected.
 */
#define EFUSE_WDT_DELAY_SEL    0x00000003U
#define EFUSE_WDT_DELAY_SEL_M  (EFUSE_WDT_DELAY_SEL_V << EFUSE_WDT_DELAY_SEL_S)
#define EFUSE_WDT_DELAY_SEL_V  0x00000003U
#define EFUSE_WDT_DELAY_SEL_S  16
/** EFUSE_SPI_BOOT_CRYPT_CNT : RO; bitpos: [20:18]; default: 0;
 *  Represents whether SPI boot encrypt/decrypt is disabled or enabled. Odd number of
 *  1: enabled. Even number of 1: disabled.
 */
#define EFUSE_SPI_BOOT_CRYPT_CNT    0x00000007U
#define EFUSE_SPI_BOOT_CRYPT_CNT_M  (EFUSE_SPI_BOOT_CRYPT_CNT_V << EFUSE_SPI_BOOT_CRYPT_CNT_S)
#define EFUSE_SPI_BOOT_CRYPT_CNT_V  0x00000007U
#define EFUSE_SPI_BOOT_CRYPT_CNT_S  18
/** EFUSE_SECURE_BOOT_KEY_REVOKE0 : RO; bitpos: [21]; default: 0;
 *  Represents whether revoking first secure boot key is enabled or disabled. 1:
 *  enabled. 0: disabled.
 */
#define EFUSE_SECURE_BOOT_KEY_REVOKE0    (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_M  (EFUSE_SECURE_BOOT_KEY_REVOKE0_V << EFUSE_SECURE_BOOT_KEY_REVOKE0_S)
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_V  0x00000001U
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_S  21
/** EFUSE_SECURE_BOOT_KEY_REVOKE1 : RO; bitpos: [22]; default: 0;
 *  Represents whether revoking second secure boot key is enabled or disabled. 1:
 *  enabled. 0: disabled.
 */
#define EFUSE_SECURE_BOOT_KEY_REVOKE1    (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_M  (EFUSE_SECURE_BOOT_KEY_REVOKE1_V << EFUSE_SECURE_BOOT_KEY_REVOKE1_S)
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_V  0x00000001U
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_S  22
/** EFUSE_SECURE_BOOT_KEY_REVOKE2 : RO; bitpos: [23]; default: 0;
 *  Represents whether revoking third secure boot key is enabled or disabled. 1:
 *  enabled. 0: disabled.
 */
#define EFUSE_SECURE_BOOT_KEY_REVOKE2    (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_M  (EFUSE_SECURE_BOOT_KEY_REVOKE2_V << EFUSE_SECURE_BOOT_KEY_REVOKE2_S)
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_V  0x00000001U
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_S  23
/** EFUSE_KEY_PURPOSE_0 : RO; bitpos: [27:24]; default: 0;
 *  Represents the purpose of Key0.
 */
#define EFUSE_KEY_PURPOSE_0    0x0000000FU
#define EFUSE_KEY_PURPOSE_0_M  (EFUSE_KEY_PURPOSE_0_V << EFUSE_KEY_PURPOSE_0_S)
#define EFUSE_KEY_PURPOSE_0_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_0_S  24
/** EFUSE_KEY_PURPOSE_1 : RO; bitpos: [31:28]; default: 0;
 *  Represents the purpose of Key1.
 */
#define EFUSE_KEY_PURPOSE_1    0x0000000FU
#define EFUSE_KEY_PURPOSE_1_M  (EFUSE_KEY_PURPOSE_1_V << EFUSE_KEY_PURPOSE_1_S)
#define EFUSE_KEY_PURPOSE_1_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_1_S  28

/** EFUSE_RD_REPEAT_DATA2_REG register
 *  BLOCK0 data register 3.
 */
#define EFUSE_RD_REPEAT_DATA2_REG (DR_REG_EFUSE_BASE + 0x38)
/** EFUSE_KEY_PURPOSE_2 : RO; bitpos: [3:0]; default: 0;
 *  Represents the purpose of Key2.
 */
#define EFUSE_KEY_PURPOSE_2    0x0000000FU
#define EFUSE_KEY_PURPOSE_2_M  (EFUSE_KEY_PURPOSE_2_V << EFUSE_KEY_PURPOSE_2_S)
#define EFUSE_KEY_PURPOSE_2_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_2_S  0
/** EFUSE_KEY_PURPOSE_3 : RO; bitpos: [7:4]; default: 0;
 *  Represents the purpose of Key3.
 */
#define EFUSE_KEY_PURPOSE_3    0x0000000FU
#define EFUSE_KEY_PURPOSE_3_M  (EFUSE_KEY_PURPOSE_3_V << EFUSE_KEY_PURPOSE_3_S)
#define EFUSE_KEY_PURPOSE_3_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_3_S  4
/** EFUSE_KEY_PURPOSE_4 : RO; bitpos: [11:8]; default: 0;
 *  Represents the purpose of Key4.
 */
#define EFUSE_KEY_PURPOSE_4    0x0000000FU
#define EFUSE_KEY_PURPOSE_4_M  (EFUSE_KEY_PURPOSE_4_V << EFUSE_KEY_PURPOSE_4_S)
#define EFUSE_KEY_PURPOSE_4_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_4_S  8
/** EFUSE_KEY_PURPOSE_5 : RO; bitpos: [15:12]; default: 0;
 *  Represents the purpose of Key5.
 */
#define EFUSE_KEY_PURPOSE_5    0x0000000FU
#define EFUSE_KEY_PURPOSE_5_M  (EFUSE_KEY_PURPOSE_5_V << EFUSE_KEY_PURPOSE_5_S)
#define EFUSE_KEY_PURPOSE_5_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_5_S  12
/** EFUSE_SEC_DPA_LEVEL : RO; bitpos: [17:16]; default: 0;
 *  Represents the spa secure level by configuring the clock random divide mode.
 */
#define EFUSE_SEC_DPA_LEVEL    0x00000003U
#define EFUSE_SEC_DPA_LEVEL_M  (EFUSE_SEC_DPA_LEVEL_V << EFUSE_SEC_DPA_LEVEL_S)
#define EFUSE_SEC_DPA_LEVEL_V  0x00000003U
#define EFUSE_SEC_DPA_LEVEL_S  16
/** EFUSE_ECDSA_ENABLE_SOFT_K : RO; bitpos: [18]; default: 0;
 *  Represents whether hardware random number k is forced used in ESDCA. 1: force used.
 *  0: not force used.
 */
#define EFUSE_ECDSA_ENABLE_SOFT_K    (BIT(18))
#define EFUSE_ECDSA_ENABLE_SOFT_K_M  (EFUSE_ECDSA_ENABLE_SOFT_K_V << EFUSE_ECDSA_ENABLE_SOFT_K_S)
#define EFUSE_ECDSA_ENABLE_SOFT_K_V  0x00000001U
#define EFUSE_ECDSA_ENABLE_SOFT_K_S  18
/** EFUSE_CRYPT_DPA_ENABLE : RO; bitpos: [19]; default: 1;
 *  Represents whether anti-dpa attack is enabled. 1:enabled. 0: disabled.
 */
#define EFUSE_CRYPT_DPA_ENABLE    (BIT(19))
#define EFUSE_CRYPT_DPA_ENABLE_M  (EFUSE_CRYPT_DPA_ENABLE_V << EFUSE_CRYPT_DPA_ENABLE_S)
#define EFUSE_CRYPT_DPA_ENABLE_V  0x00000001U
#define EFUSE_CRYPT_DPA_ENABLE_S  19
/** EFUSE_SECURE_BOOT_EN : RO; bitpos: [20]; default: 0;
 *  Represents whether secure boot is enabled or disabled. 1: enabled. 0: disabled.
 */
#define EFUSE_SECURE_BOOT_EN    (BIT(20))
#define EFUSE_SECURE_BOOT_EN_M  (EFUSE_SECURE_BOOT_EN_V << EFUSE_SECURE_BOOT_EN_S)
#define EFUSE_SECURE_BOOT_EN_V  0x00000001U
#define EFUSE_SECURE_BOOT_EN_S  20
/** EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE : RO; bitpos: [21]; default: 0;
 *  Represents whether revoking aggressive secure boot is enabled or disabled. 1:
 *  enabled. 0: disabled.
 */
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE    (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_M  (EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_V << EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_S)
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_V  0x00000001U
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_S  21
/** EFUSE_RD_RESERVE_0_118 : RW; bitpos: [22]; default: 0;
 *  Reserved, it was created by set_missed_fields_in_regs func
 */
#define EFUSE_RD_RESERVE_0_118    (BIT(22))
#define EFUSE_RD_RESERVE_0_118_M  (EFUSE_RD_RESERVE_0_118_V << EFUSE_RD_RESERVE_0_118_S)
#define EFUSE_RD_RESERVE_0_118_V  0x00000001U
#define EFUSE_RD_RESERVE_0_118_S  22
/** EFUSE_FLASH_TYPE : RO; bitpos: [23]; default: 0;
 *  The type of interfaced flash. 0: four data lines, 1: eight data lines.
 */
#define EFUSE_FLASH_TYPE    (BIT(23))
#define EFUSE_FLASH_TYPE_M  (EFUSE_FLASH_TYPE_V << EFUSE_FLASH_TYPE_S)
#define EFUSE_FLASH_TYPE_V  0x00000001U
#define EFUSE_FLASH_TYPE_S  23
/** EFUSE_FLASH_PAGE_SIZE : RO; bitpos: [25:24]; default: 0;
 *  Set flash page size.
 */
#define EFUSE_FLASH_PAGE_SIZE    0x00000003U
#define EFUSE_FLASH_PAGE_SIZE_M  (EFUSE_FLASH_PAGE_SIZE_V << EFUSE_FLASH_PAGE_SIZE_S)
#define EFUSE_FLASH_PAGE_SIZE_V  0x00000003U
#define EFUSE_FLASH_PAGE_SIZE_S  24
/** EFUSE_FLASH_ECC_EN : RO; bitpos: [26]; default: 0;
 *  Set this bit to enable ecc for flash boot.
 */
#define EFUSE_FLASH_ECC_EN    (BIT(26))
#define EFUSE_FLASH_ECC_EN_M  (EFUSE_FLASH_ECC_EN_V << EFUSE_FLASH_ECC_EN_S)
#define EFUSE_FLASH_ECC_EN_V  0x00000001U
#define EFUSE_FLASH_ECC_EN_S  26
/** EFUSE_DIS_USB_OTG_DOWNLOAD_MODE : RO; bitpos: [27]; default: 0;
 *  Set this bit to disable download via USB-OTG.
 */
#define EFUSE_DIS_USB_OTG_DOWNLOAD_MODE    (BIT(27))
#define EFUSE_DIS_USB_OTG_DOWNLOAD_MODE_M  (EFUSE_DIS_USB_OTG_DOWNLOAD_MODE_V << EFUSE_DIS_USB_OTG_DOWNLOAD_MODE_S)
#define EFUSE_DIS_USB_OTG_DOWNLOAD_MODE_V  0x00000001U
#define EFUSE_DIS_USB_OTG_DOWNLOAD_MODE_S  27
/** EFUSE_FLASH_TPUW : RO; bitpos: [31:28]; default: 0;
 *  Represents the flash waiting time after power-up, in unit of ms. When the value
 *  less than 15, the waiting time is the programmed value. Otherwise, the waiting time
 *  is 2 times the programmed value.
 */
#define EFUSE_FLASH_TPUW    0x0000000FU
#define EFUSE_FLASH_TPUW_M  (EFUSE_FLASH_TPUW_V << EFUSE_FLASH_TPUW_S)
#define EFUSE_FLASH_TPUW_V  0x0000000FU
#define EFUSE_FLASH_TPUW_S  28

/** EFUSE_RD_REPEAT_DATA3_REG register
 *  BLOCK0 data register 4.
 */
#define EFUSE_RD_REPEAT_DATA3_REG (DR_REG_EFUSE_BASE + 0x3c)
/** EFUSE_DIS_DOWNLOAD_MODE : RO; bitpos: [0]; default: 0;
 *  Represents whether Download mode is disabled or enabled. 1: disabled. 0: enabled.
 */
#define EFUSE_DIS_DOWNLOAD_MODE    (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_M  (EFUSE_DIS_DOWNLOAD_MODE_V << EFUSE_DIS_DOWNLOAD_MODE_S)
#define EFUSE_DIS_DOWNLOAD_MODE_V  0x00000001U
#define EFUSE_DIS_DOWNLOAD_MODE_S  0
/** EFUSE_DIS_DIRECT_BOOT : RO; bitpos: [1]; default: 0;
 *  Represents whether direct boot mode is disabled or enabled. 1: disabled. 0: enabled.
 */
#define EFUSE_DIS_DIRECT_BOOT    (BIT(1))
#define EFUSE_DIS_DIRECT_BOOT_M  (EFUSE_DIS_DIRECT_BOOT_V << EFUSE_DIS_DIRECT_BOOT_S)
#define EFUSE_DIS_DIRECT_BOOT_V  0x00000001U
#define EFUSE_DIS_DIRECT_BOOT_S  1
/** EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT : RO; bitpos: [2]; default: 0;
 *  Represents whether print from USB-Serial-JTAG is disabled or enabled. 1: disabled.
 *  0: enabled.
 */
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT    (BIT(2))
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_M  (EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_V << EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_S)
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_V  0x00000001U
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_S  2
/** EFUSE_LOCK_KM_KEY : RO; bitpos: [3]; default: 0;
 *  TBD
 */
#define EFUSE_LOCK_KM_KEY    (BIT(3))
#define EFUSE_LOCK_KM_KEY_M  (EFUSE_LOCK_KM_KEY_V << EFUSE_LOCK_KM_KEY_S)
#define EFUSE_LOCK_KM_KEY_V  0x00000001U
#define EFUSE_LOCK_KM_KEY_S  3
/** EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE : RO; bitpos: [4]; default: 0;
 *  Represents whether the USB-Serial-JTAG download function is disabled or enabled. 1:
 *  disabled. 0: enabled.
 */
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE    (BIT(4))
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_M  (EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_V << EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_S)
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_V  0x00000001U
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_S  4
/** EFUSE_ENABLE_SECURITY_DOWNLOAD : RO; bitpos: [5]; default: 0;
 *  Represents whether security download is enabled or disabled. 1: enabled. 0:
 *  disabled.
 */
#define EFUSE_ENABLE_SECURITY_DOWNLOAD    (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_M  (EFUSE_ENABLE_SECURITY_DOWNLOAD_V << EFUSE_ENABLE_SECURITY_DOWNLOAD_S)
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_V  0x00000001U
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_S  5
/** EFUSE_UART_PRINT_CONTROL : RO; bitpos: [7:6]; default: 0;
 *  Represents the type of UART printing. 00: force enable printing. 01: enable
 *  printing when GPIO8 is reset at low level. 10: enable printing when GPIO8 is reset
 *  at high level. 11: force disable printing.
 */
#define EFUSE_UART_PRINT_CONTROL    0x00000003U
#define EFUSE_UART_PRINT_CONTROL_M  (EFUSE_UART_PRINT_CONTROL_V << EFUSE_UART_PRINT_CONTROL_S)
#define EFUSE_UART_PRINT_CONTROL_V  0x00000003U
#define EFUSE_UART_PRINT_CONTROL_S  6
/** EFUSE_FORCE_SEND_RESUME : RO; bitpos: [8]; default: 0;
 *  Represents whether ROM code is forced to send a resume command during SPI boot. 1:
 *  forced. 0:not forced.
 */
#define EFUSE_FORCE_SEND_RESUME    (BIT(8))
#define EFUSE_FORCE_SEND_RESUME_M  (EFUSE_FORCE_SEND_RESUME_V << EFUSE_FORCE_SEND_RESUME_S)
#define EFUSE_FORCE_SEND_RESUME_V  0x00000001U
#define EFUSE_FORCE_SEND_RESUME_S  8
/** EFUSE_SECURE_VERSION : RO; bitpos: [24:9]; default: 0;
 *  Represents the version used by ESP-IDF anti-rollback feature.
 */
#define EFUSE_SECURE_VERSION    0x0000FFFFU
#define EFUSE_SECURE_VERSION_M  (EFUSE_SECURE_VERSION_V << EFUSE_SECURE_VERSION_S)
#define EFUSE_SECURE_VERSION_V  0x0000FFFFU
#define EFUSE_SECURE_VERSION_S  9
/** EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE : RO; bitpos: [25]; default: 0;
 *  Represents whether FAST VERIFY ON WAKE is disabled or enabled when Secure Boot is
 *  enabled. 1: disabled. 0: enabled.
 */
#define EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE    (BIT(25))
#define EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_M  (EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_V << EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_S)
#define EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_V  0x00000001U
#define EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_S  25
/** EFUSE_HYS_EN_PAD : RO; bitpos: [26]; default: 0;
 *  Represents whether the hysteresis function of corresponding PAD is enabled. 1:
 *  enabled. 0:disabled.
 */
#define EFUSE_HYS_EN_PAD    (BIT(26))
#define EFUSE_HYS_EN_PAD_M  (EFUSE_HYS_EN_PAD_V << EFUSE_HYS_EN_PAD_S)
#define EFUSE_HYS_EN_PAD_V  0x00000001U
#define EFUSE_HYS_EN_PAD_S  26
/** EFUSE_DCDC_VSET : RO; bitpos: [31:27]; default: 0;
 *  Set the dcdc voltage default.
 */
#define EFUSE_DCDC_VSET    0x0000001FU
#define EFUSE_DCDC_VSET_M  (EFUSE_DCDC_VSET_V << EFUSE_DCDC_VSET_S)
#define EFUSE_DCDC_VSET_V  0x0000001FU
#define EFUSE_DCDC_VSET_S  27

/** EFUSE_RD_REPEAT_DATA4_REG register
 *  BLOCK0 data register 5.
 */
#define EFUSE_RD_REPEAT_DATA4_REG (DR_REG_EFUSE_BASE + 0x40)
/** EFUSE_PXA0_TIEH_SEL_0 : RO; bitpos: [1:0]; default: 0;
 *  TBD
 */
#define EFUSE_PXA0_TIEH_SEL_0    0x00000003U
#define EFUSE_PXA0_TIEH_SEL_0_M  (EFUSE_PXA0_TIEH_SEL_0_V << EFUSE_PXA0_TIEH_SEL_0_S)
#define EFUSE_PXA0_TIEH_SEL_0_V  0x00000003U
#define EFUSE_PXA0_TIEH_SEL_0_S  0
/** EFUSE_PXA0_TIEH_SEL_1 : RO; bitpos: [3:2]; default: 0;
 *  TBD.
 */
#define EFUSE_PXA0_TIEH_SEL_1    0x00000003U
#define EFUSE_PXA0_TIEH_SEL_1_M  (EFUSE_PXA0_TIEH_SEL_1_V << EFUSE_PXA0_TIEH_SEL_1_S)
#define EFUSE_PXA0_TIEH_SEL_1_V  0x00000003U
#define EFUSE_PXA0_TIEH_SEL_1_S  2
/** EFUSE_PXA0_TIEH_SEL_2 : RO; bitpos: [5:4]; default: 0;
 *  TBD.
 */
#define EFUSE_PXA0_TIEH_SEL_2    0x00000003U
#define EFUSE_PXA0_TIEH_SEL_2_M  (EFUSE_PXA0_TIEH_SEL_2_V << EFUSE_PXA0_TIEH_SEL_2_S)
#define EFUSE_PXA0_TIEH_SEL_2_V  0x00000003U
#define EFUSE_PXA0_TIEH_SEL_2_S  4
/** EFUSE_PXA0_TIEH_SEL_3 : RO; bitpos: [7:6]; default: 0;
 *  TBD.
 */
#define EFUSE_PXA0_TIEH_SEL_3    0x00000003U
#define EFUSE_PXA0_TIEH_SEL_3_M  (EFUSE_PXA0_TIEH_SEL_3_V << EFUSE_PXA0_TIEH_SEL_3_S)
#define EFUSE_PXA0_TIEH_SEL_3_V  0x00000003U
#define EFUSE_PXA0_TIEH_SEL_3_S  6
/** EFUSE_KM_DISABLE_DEPLOY_MODE : RO; bitpos: [11:8]; default: 0;
 *  TBD.
 */
#define EFUSE_KM_DISABLE_DEPLOY_MODE    0x0000000FU
#define EFUSE_KM_DISABLE_DEPLOY_MODE_M  (EFUSE_KM_DISABLE_DEPLOY_MODE_V << EFUSE_KM_DISABLE_DEPLOY_MODE_S)
#define EFUSE_KM_DISABLE_DEPLOY_MODE_V  0x0000000FU
#define EFUSE_KM_DISABLE_DEPLOY_MODE_S  8
/** EFUSE_USB_DEVICE_DREFL : RO; bitpos: [13:12]; default: 0;
 *  Represents the usb device single-end input low threshold, 0.8 V to 1.04 V with step
 *  of 80 mV.
 */
#define EFUSE_USB_DEVICE_DREFL    0x00000003U
#define EFUSE_USB_DEVICE_DREFL_M  (EFUSE_USB_DEVICE_DREFL_V << EFUSE_USB_DEVICE_DREFL_S)
#define EFUSE_USB_DEVICE_DREFL_V  0x00000003U
#define EFUSE_USB_DEVICE_DREFL_S  12
/** EFUSE_USB_OTG11_DREFL : RO; bitpos: [15:14]; default: 0;
 *  Represents the usb otg11 single-end input low threshold, 0.8 V to 1.04 V with step
 *  of 80 mV.
 */
#define EFUSE_USB_OTG11_DREFL    0x00000003U
#define EFUSE_USB_OTG11_DREFL_M  (EFUSE_USB_OTG11_DREFL_V << EFUSE_USB_OTG11_DREFL_S)
#define EFUSE_USB_OTG11_DREFL_V  0x00000003U
#define EFUSE_USB_OTG11_DREFL_S  14
/** EFUSE_RD_RESERVE_0_176 : RW; bitpos: [17:16]; default: 0;
 *  Reserved, it was created by set_missed_fields_in_regs func
 */
#define EFUSE_RD_RESERVE_0_176    0x00000003U
#define EFUSE_RD_RESERVE_0_176_M  (EFUSE_RD_RESERVE_0_176_V << EFUSE_RD_RESERVE_0_176_S)
#define EFUSE_RD_RESERVE_0_176_V  0x00000003U
#define EFUSE_RD_RESERVE_0_176_S  16
/** EFUSE_HP_PWR_SRC_SEL : RO; bitpos: [18]; default: 0;
 *  HP system power source select. 0:LDO. 1: DCDC.
 */
#define EFUSE_HP_PWR_SRC_SEL    (BIT(18))
#define EFUSE_HP_PWR_SRC_SEL_M  (EFUSE_HP_PWR_SRC_SEL_V << EFUSE_HP_PWR_SRC_SEL_S)
#define EFUSE_HP_PWR_SRC_SEL_V  0x00000001U
#define EFUSE_HP_PWR_SRC_SEL_S  18
/** EFUSE_DCDC_VSET_EN : RO; bitpos: [19]; default: 0;
 *  Select dcdc vset use efuse_dcdc_vset.
 */
#define EFUSE_DCDC_VSET_EN    (BIT(19))
#define EFUSE_DCDC_VSET_EN_M  (EFUSE_DCDC_VSET_EN_V << EFUSE_DCDC_VSET_EN_S)
#define EFUSE_DCDC_VSET_EN_V  0x00000001U
#define EFUSE_DCDC_VSET_EN_S  19
/** EFUSE_DIS_WDT : RO; bitpos: [20]; default: 0;
 *  Set this bit to disable watch dog.
 */
#define EFUSE_DIS_WDT    (BIT(20))
#define EFUSE_DIS_WDT_M  (EFUSE_DIS_WDT_V << EFUSE_DIS_WDT_S)
#define EFUSE_DIS_WDT_V  0x00000001U
#define EFUSE_DIS_WDT_S  20
/** EFUSE_DIS_SWD : RO; bitpos: [21]; default: 0;
 *  Set this bit to disable super-watchdog.
 */
#define EFUSE_DIS_SWD    (BIT(21))
#define EFUSE_DIS_SWD_M  (EFUSE_DIS_SWD_V << EFUSE_DIS_SWD_S)
#define EFUSE_DIS_SWD_V  0x00000001U
#define EFUSE_DIS_SWD_S  21
/** EFUSE_RD_RESERVE_0_182 : RW; bitpos: [31:22]; default: 0;
 *  Reserved, it was created by set_missed_fields_in_regs func
 */
#define EFUSE_RD_RESERVE_0_182    0x000003FFU
#define EFUSE_RD_RESERVE_0_182_M  (EFUSE_RD_RESERVE_0_182_V << EFUSE_RD_RESERVE_0_182_S)
#define EFUSE_RD_RESERVE_0_182_V  0x000003FFU
#define EFUSE_RD_RESERVE_0_182_S  22

/** EFUSE_RD_MAC_SYS_0_REG register
 *  BLOCK1 data register $n.
 */
#define EFUSE_RD_MAC_SYS_0_REG (DR_REG_EFUSE_BASE + 0x44)
/** EFUSE_MAC_0 : RO; bitpos: [31:0]; default: 0;
 *  Stores the low 32 bits of MAC address.
 */
#define EFUSE_MAC_0    0xFFFFFFFFU
#define EFUSE_MAC_0_M  (EFUSE_MAC_0_V << EFUSE_MAC_0_S)
#define EFUSE_MAC_0_V  0xFFFFFFFFU
#define EFUSE_MAC_0_S  0

/** EFUSE_RD_MAC_SYS_1_REG register
 *  BLOCK1 data register $n.
 */
#define EFUSE_RD_MAC_SYS_1_REG (DR_REG_EFUSE_BASE + 0x48)
/** EFUSE_MAC_1 : RO; bitpos: [15:0]; default: 0;
 *  Stores the high 16 bits of MAC address.
 */
#define EFUSE_MAC_1    0x0000FFFFU
#define EFUSE_MAC_1_M  (EFUSE_MAC_1_V << EFUSE_MAC_1_S)
#define EFUSE_MAC_1_V  0x0000FFFFU
#define EFUSE_MAC_1_S  0
/** EFUSE_RESERVED_1_16 : RO; bitpos: [31:16]; default: 0;
 *  Stores the extended bits of MAC address.
 */
#define EFUSE_RESERVED_1_16    0x0000FFFFU
#define EFUSE_RESERVED_1_16_M  (EFUSE_RESERVED_1_16_V << EFUSE_RESERVED_1_16_S)
#define EFUSE_RESERVED_1_16_V  0x0000FFFFU
#define EFUSE_RESERVED_1_16_S  16

/** EFUSE_RD_MAC_SYS_2_REG register
 *  BLOCK1 data register $n.
 */
#define EFUSE_RD_MAC_SYS_2_REG (DR_REG_EFUSE_BASE + 0x4c)
/** EFUSE_WAFER_VERSION_MINOR : R; bitpos: [3:0]; default: 0;
 *  Minor chip version
 */
#define EFUSE_WAFER_VERSION_MINOR    0x0000000FU
#define EFUSE_WAFER_VERSION_MINOR_M  (EFUSE_WAFER_VERSION_MINOR_V << EFUSE_WAFER_VERSION_MINOR_S)
#define EFUSE_WAFER_VERSION_MINOR_V  0x0000000FU
#define EFUSE_WAFER_VERSION_MINOR_S  0
/** EFUSE_WAFER_VERSION_MAJOR : R; bitpos: [5:4]; default: 0;
 *  Major chip version
 */
#define EFUSE_WAFER_VERSION_MAJOR    0x00000003U
#define EFUSE_WAFER_VERSION_MAJOR_M  (EFUSE_WAFER_VERSION_MAJOR_V << EFUSE_WAFER_VERSION_MAJOR_S)
#define EFUSE_WAFER_VERSION_MAJOR_V  0x00000003U
#define EFUSE_WAFER_VERSION_MAJOR_S  4
/** EFUSE_DISABLE_WAFER_VERSION_MAJOR : R; bitpos: [6]; default: 0;
 *  Disables check of wafer version major
 */
#define EFUSE_DISABLE_WAFER_VERSION_MAJOR    (BIT(6))
#define EFUSE_DISABLE_WAFER_VERSION_MAJOR_M  (EFUSE_DISABLE_WAFER_VERSION_MAJOR_V << EFUSE_DISABLE_WAFER_VERSION_MAJOR_S)
#define EFUSE_DISABLE_WAFER_VERSION_MAJOR_V  0x00000001U
#define EFUSE_DISABLE_WAFER_VERSION_MAJOR_S  6
/** EFUSE_DISABLE_BLK_VERSION_MAJOR : R; bitpos: [7]; default: 0;
 *  Disables check of blk version major
 */
#define EFUSE_DISABLE_BLK_VERSION_MAJOR    (BIT(7))
#define EFUSE_DISABLE_BLK_VERSION_MAJOR_M  (EFUSE_DISABLE_BLK_VERSION_MAJOR_V << EFUSE_DISABLE_BLK_VERSION_MAJOR_S)
#define EFUSE_DISABLE_BLK_VERSION_MAJOR_V  0x00000001U
#define EFUSE_DISABLE_BLK_VERSION_MAJOR_S  7
/** EFUSE_BLK_VERSION_MINOR : R; bitpos: [10:8]; default: 0;
 *  BLK_VERSION_MINOR of BLOCK2
 */
#define EFUSE_BLK_VERSION_MINOR    0x00000007U
#define EFUSE_BLK_VERSION_MINOR_M  (EFUSE_BLK_VERSION_MINOR_V << EFUSE_BLK_VERSION_MINOR_S)
#define EFUSE_BLK_VERSION_MINOR_V  0x00000007U
#define EFUSE_BLK_VERSION_MINOR_S  8
/** EFUSE_BLK_VERSION_MAJOR : R; bitpos: [12:11]; default: 0;
 *  BLK_VERSION_MAJOR of BLOCK2
 */
#define EFUSE_BLK_VERSION_MAJOR    0x00000003U
#define EFUSE_BLK_VERSION_MAJOR_M  (EFUSE_BLK_VERSION_MAJOR_V << EFUSE_BLK_VERSION_MAJOR_S)
#define EFUSE_BLK_VERSION_MAJOR_V  0x00000003U
#define EFUSE_BLK_VERSION_MAJOR_S  11
/** EFUSE_PSRAM_CAP : R; bitpos: [15:13]; default: 0;
 *  PSRAM capacity
 */
#define EFUSE_PSRAM_CAP    0x00000007U
#define EFUSE_PSRAM_CAP_M  (EFUSE_PSRAM_CAP_V << EFUSE_PSRAM_CAP_S)
#define EFUSE_PSRAM_CAP_V  0x00000007U
#define EFUSE_PSRAM_CAP_S  13
/** EFUSE_TEMP : R; bitpos: [17:16]; default: 0;
 *  Operating temperature of the ESP chip
 */
#define EFUSE_TEMP    0x00000003U
#define EFUSE_TEMP_M  (EFUSE_TEMP_V << EFUSE_TEMP_S)
#define EFUSE_TEMP_V  0x00000003U
#define EFUSE_TEMP_S  16
/** EFUSE_PSRAM_VENDOR : R; bitpos: [19:18]; default: 0;
 *  PSRAM vendor
 */
#define EFUSE_PSRAM_VENDOR    0x00000003U
#define EFUSE_PSRAM_VENDOR_M  (EFUSE_PSRAM_VENDOR_V << EFUSE_PSRAM_VENDOR_S)
#define EFUSE_PSRAM_VENDOR_V  0x00000003U
#define EFUSE_PSRAM_VENDOR_S  18
/** EFUSE_PKG_VERSION : R; bitpos: [22:20]; default: 0;
 *  Package version
 */
#define EFUSE_PKG_VERSION    0x00000007U
#define EFUSE_PKG_VERSION_M  (EFUSE_PKG_VERSION_V << EFUSE_PKG_VERSION_S)
#define EFUSE_PKG_VERSION_V  0x00000007U
#define EFUSE_PKG_VERSION_S  20
/** EFUSE_RESERVED_1_87 : R; bitpos: [31:23]; default: 0;
 *  reserved
 */
#define EFUSE_RESERVED_1_87    0x000001FFU
#define EFUSE_RESERVED_1_87_M  (EFUSE_RESERVED_1_87_V << EFUSE_RESERVED_1_87_S)
#define EFUSE_RESERVED_1_87_V  0x000001FFU
#define EFUSE_RESERVED_1_87_S  23

/** EFUSE_RD_MAC_SYS_3_REG register
 *  BLOCK1 data register $n.
 */
#define EFUSE_RD_MAC_SYS_3_REG (DR_REG_EFUSE_BASE + 0x50)
/** EFUSE_MAC_RESERVED_2 : RO; bitpos: [17:0]; default: 0;
 *  Reserved.
 */
#define EFUSE_MAC_RESERVED_2    0x0003FFFFU
#define EFUSE_MAC_RESERVED_2_M  (EFUSE_MAC_RESERVED_2_V << EFUSE_MAC_RESERVED_2_S)
#define EFUSE_MAC_RESERVED_2_V  0x0003FFFFU
#define EFUSE_MAC_RESERVED_2_S  0
/** EFUSE_SYS_DATA_PART0_0 : RO; bitpos: [31:18]; default: 0;
 *  Stores the first 14 bits of the zeroth part of system data.
 */
#define EFUSE_SYS_DATA_PART0_0    0x00003FFFU
#define EFUSE_SYS_DATA_PART0_0_M  (EFUSE_SYS_DATA_PART0_0_V << EFUSE_SYS_DATA_PART0_0_S)
#define EFUSE_SYS_DATA_PART0_0_V  0x00003FFFU
#define EFUSE_SYS_DATA_PART0_0_S  18

/** EFUSE_RD_MAC_SYS_4_REG register
 *  BLOCK1 data register $n.
 */
#define EFUSE_RD_MAC_SYS_4_REG (DR_REG_EFUSE_BASE + 0x54)
/** EFUSE_SYS_DATA_PART0_1 : RO; bitpos: [31:0]; default: 0;
 *  Stores the first 32 bits of the zeroth part of system data.
 */
#define EFUSE_SYS_DATA_PART0_1    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART0_1_M  (EFUSE_SYS_DATA_PART0_1_V << EFUSE_SYS_DATA_PART0_1_S)
#define EFUSE_SYS_DATA_PART0_1_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART0_1_S  0

/** EFUSE_RD_MAC_SYS_5_REG register
 *  BLOCK1 data register $n.
 */
#define EFUSE_RD_MAC_SYS_5_REG (DR_REG_EFUSE_BASE + 0x58)
/** EFUSE_SYS_DATA_PART0_2 : RO; bitpos: [31:0]; default: 0;
 *  Stores the second 32 bits of the zeroth part of system data.
 */
#define EFUSE_SYS_DATA_PART0_2    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART0_2_M  (EFUSE_SYS_DATA_PART0_2_V << EFUSE_SYS_DATA_PART0_2_S)
#define EFUSE_SYS_DATA_PART0_2_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART0_2_S  0

/** EFUSE_RD_SYS_PART1_DATA0_REG register
 *  Register $n of BLOCK2 (system).
 */
#define EFUSE_RD_SYS_PART1_DATA0_REG (DR_REG_EFUSE_BASE + 0x5c)
/** EFUSE_OPTIONAL_UNIQUE_ID : R; bitpos: [31:0]; default: 0;
 *  Optional unique 128-bit ID
 */
#define EFUSE_OPTIONAL_UNIQUE_ID    0xFFFFFFFFU
#define EFUSE_OPTIONAL_UNIQUE_ID_M  (EFUSE_OPTIONAL_UNIQUE_ID_V << EFUSE_OPTIONAL_UNIQUE_ID_S)
#define EFUSE_OPTIONAL_UNIQUE_ID_V  0xFFFFFFFFU
#define EFUSE_OPTIONAL_UNIQUE_ID_S  0

/** EFUSE_RD_SYS_PART1_DATA1_REG register
 *  Register $n of BLOCK2 (system).
 */
#define EFUSE_RD_SYS_PART1_DATA1_REG (DR_REG_EFUSE_BASE + 0x60)
/** EFUSE_OPTIONAL_UNIQUE_ID_1 : R; bitpos: [31:0]; default: 0;
 *  Optional unique 128-bit ID
 */
#define EFUSE_OPTIONAL_UNIQUE_ID_1    0xFFFFFFFFU
#define EFUSE_OPTIONAL_UNIQUE_ID_1_M  (EFUSE_OPTIONAL_UNIQUE_ID_1_V << EFUSE_OPTIONAL_UNIQUE_ID_1_S)
#define EFUSE_OPTIONAL_UNIQUE_ID_1_V  0xFFFFFFFFU
#define EFUSE_OPTIONAL_UNIQUE_ID_1_S  0

/** EFUSE_RD_SYS_PART1_DATA2_REG register
 *  Register $n of BLOCK2 (system).
 */
#define EFUSE_RD_SYS_PART1_DATA2_REG (DR_REG_EFUSE_BASE + 0x64)
/** EFUSE_OPTIONAL_UNIQUE_ID_2 : R; bitpos: [31:0]; default: 0;
 *  Optional unique 128-bit ID
 */
#define EFUSE_OPTIONAL_UNIQUE_ID_2    0xFFFFFFFFU
#define EFUSE_OPTIONAL_UNIQUE_ID_2_M  (EFUSE_OPTIONAL_UNIQUE_ID_2_V << EFUSE_OPTIONAL_UNIQUE_ID_2_S)
#define EFUSE_OPTIONAL_UNIQUE_ID_2_V  0xFFFFFFFFU
#define EFUSE_OPTIONAL_UNIQUE_ID_2_S  0

/** EFUSE_RD_SYS_PART1_DATA3_REG register
 *  Register $n of BLOCK2 (system).
 */
#define EFUSE_RD_SYS_PART1_DATA3_REG (DR_REG_EFUSE_BASE + 0x68)
/** EFUSE_OPTIONAL_UNIQUE_ID_3 : R; bitpos: [31:0]; default: 0;
 *  Optional unique 128-bit ID
 */
#define EFUSE_OPTIONAL_UNIQUE_ID_3    0xFFFFFFFFU
#define EFUSE_OPTIONAL_UNIQUE_ID_3_M  (EFUSE_OPTIONAL_UNIQUE_ID_3_V << EFUSE_OPTIONAL_UNIQUE_ID_3_S)
#define EFUSE_OPTIONAL_UNIQUE_ID_3_V  0xFFFFFFFFU
#define EFUSE_OPTIONAL_UNIQUE_ID_3_S  0

/** EFUSE_RD_SYS_PART1_DATA4_REG register
 *  Register $n of BLOCK2 (system).
 */
#define EFUSE_RD_SYS_PART1_DATA4_REG (DR_REG_EFUSE_BASE + 0x6c)
/** EFUSE_SYS_DATA_PART1_4 : RO; bitpos: [31:0]; default: 0;
 *  Stores the fourth 32 bits of the first part of system data.
 */
#define EFUSE_SYS_DATA_PART1_4    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART1_4_M  (EFUSE_SYS_DATA_PART1_4_V << EFUSE_SYS_DATA_PART1_4_S)
#define EFUSE_SYS_DATA_PART1_4_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART1_4_S  0

/** EFUSE_RD_SYS_PART1_DATA5_REG register
 *  Register $n of BLOCK2 (system).
 */
#define EFUSE_RD_SYS_PART1_DATA5_REG (DR_REG_EFUSE_BASE + 0x70)
/** EFUSE_SYS_DATA_PART1_5 : RO; bitpos: [31:0]; default: 0;
 *  Stores the fifth 32 bits of the first part of system data.
 */
#define EFUSE_SYS_DATA_PART1_5    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART1_5_M  (EFUSE_SYS_DATA_PART1_5_V << EFUSE_SYS_DATA_PART1_5_S)
#define EFUSE_SYS_DATA_PART1_5_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART1_5_S  0

/** EFUSE_RD_SYS_PART1_DATA6_REG register
 *  Register $n of BLOCK2 (system).
 */
#define EFUSE_RD_SYS_PART1_DATA6_REG (DR_REG_EFUSE_BASE + 0x74)
/** EFUSE_SYS_DATA_PART1_6 : RO; bitpos: [31:0]; default: 0;
 *  Stores the sixth 32 bits of the first part of system data.
 */
#define EFUSE_SYS_DATA_PART1_6    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART1_6_M  (EFUSE_SYS_DATA_PART1_6_V << EFUSE_SYS_DATA_PART1_6_S)
#define EFUSE_SYS_DATA_PART1_6_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART1_6_S  0

/** EFUSE_RD_SYS_PART1_DATA7_REG register
 *  Register $n of BLOCK2 (system).
 */
#define EFUSE_RD_SYS_PART1_DATA7_REG (DR_REG_EFUSE_BASE + 0x78)
/** EFUSE_SYS_DATA_PART1_7 : RO; bitpos: [31:0]; default: 0;
 *  Stores the seventh 32 bits of the first part of system data.
 */
#define EFUSE_SYS_DATA_PART1_7    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART1_7_M  (EFUSE_SYS_DATA_PART1_7_V << EFUSE_SYS_DATA_PART1_7_S)
#define EFUSE_SYS_DATA_PART1_7_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART1_7_S  0

/** EFUSE_RD_USR_DATA0_REG register
 *  Register $n of BLOCK3 (user).
 */
#define EFUSE_RD_USR_DATA0_REG (DR_REG_EFUSE_BASE + 0x7c)
/** EFUSE_USR_DATA0 : RO; bitpos: [31:0]; default: 0;
 *  Stores the zeroth 32 bits of BLOCK3 (user).
 */
#define EFUSE_USR_DATA0    0xFFFFFFFFU
#define EFUSE_USR_DATA0_M  (EFUSE_USR_DATA0_V << EFUSE_USR_DATA0_S)
#define EFUSE_USR_DATA0_V  0xFFFFFFFFU
#define EFUSE_USR_DATA0_S  0

/** EFUSE_RD_USR_DATA1_REG register
 *  Register $n of BLOCK3 (user).
 */
#define EFUSE_RD_USR_DATA1_REG (DR_REG_EFUSE_BASE + 0x80)
/** EFUSE_USR_DATA1 : RO; bitpos: [31:0]; default: 0;
 *  Stores the first 32 bits of BLOCK3 (user).
 */
#define EFUSE_USR_DATA1    0xFFFFFFFFU
#define EFUSE_USR_DATA1_M  (EFUSE_USR_DATA1_V << EFUSE_USR_DATA1_S)
#define EFUSE_USR_DATA1_V  0xFFFFFFFFU
#define EFUSE_USR_DATA1_S  0

/** EFUSE_RD_USR_DATA2_REG register
 *  Register $n of BLOCK3 (user).
 */
#define EFUSE_RD_USR_DATA2_REG (DR_REG_EFUSE_BASE + 0x84)
/** EFUSE_USR_DATA2 : RO; bitpos: [31:0]; default: 0;
 *  Stores the second 32 bits of BLOCK3 (user).
 */
#define EFUSE_USR_DATA2    0xFFFFFFFFU
#define EFUSE_USR_DATA2_M  (EFUSE_USR_DATA2_V << EFUSE_USR_DATA2_S)
#define EFUSE_USR_DATA2_V  0xFFFFFFFFU
#define EFUSE_USR_DATA2_S  0

/** EFUSE_RD_USR_DATA3_REG register
 *  Register $n of BLOCK3 (user).
 */
#define EFUSE_RD_USR_DATA3_REG (DR_REG_EFUSE_BASE + 0x88)
/** EFUSE_USR_DATA3 : RO; bitpos: [31:0]; default: 0;
 *  Stores the third 32 bits of BLOCK3 (user).
 */
#define EFUSE_USR_DATA3    0xFFFFFFFFU
#define EFUSE_USR_DATA3_M  (EFUSE_USR_DATA3_V << EFUSE_USR_DATA3_S)
#define EFUSE_USR_DATA3_V  0xFFFFFFFFU
#define EFUSE_USR_DATA3_S  0

/** EFUSE_RD_USR_DATA4_REG register
 *  Register $n of BLOCK3 (user).
 */
#define EFUSE_RD_USR_DATA4_REG (DR_REG_EFUSE_BASE + 0x8c)
/** EFUSE_USR_DATA4 : RO; bitpos: [31:0]; default: 0;
 *  Stores the fourth 32 bits of BLOCK3 (user).
 */
#define EFUSE_USR_DATA4    0xFFFFFFFFU
#define EFUSE_USR_DATA4_M  (EFUSE_USR_DATA4_V << EFUSE_USR_DATA4_S)
#define EFUSE_USR_DATA4_V  0xFFFFFFFFU
#define EFUSE_USR_DATA4_S  0

/** EFUSE_RD_USR_DATA5_REG register
 *  Register $n of BLOCK3 (user).
 */
#define EFUSE_RD_USR_DATA5_REG (DR_REG_EFUSE_BASE + 0x90)
/** EFUSE_USR_DATA5 : RO; bitpos: [31:0]; default: 0;
 *  Stores the fifth 32 bits of BLOCK3 (user).
 */
#define EFUSE_USR_DATA5    0xFFFFFFFFU
#define EFUSE_USR_DATA5_M  (EFUSE_USR_DATA5_V << EFUSE_USR_DATA5_S)
#define EFUSE_USR_DATA5_V  0xFFFFFFFFU
#define EFUSE_USR_DATA5_S  0

/** EFUSE_RD_USR_DATA6_REG register
 *  Register $n of BLOCK3 (user).
 */
#define EFUSE_RD_USR_DATA6_REG (DR_REG_EFUSE_BASE + 0x94)
/** EFUSE_RESERVED_3_192 : R; bitpos: [7:0]; default: 0;
 *  reserved
 */
#define EFUSE_RESERVED_3_192    0x000000FFU
#define EFUSE_RESERVED_3_192_M  (EFUSE_RESERVED_3_192_V << EFUSE_RESERVED_3_192_S)
#define EFUSE_RESERVED_3_192_V  0x000000FFU
#define EFUSE_RESERVED_3_192_S  0
/** EFUSE_CUSTOM_MAC : R; bitpos: [31:8]; default: 0;
 *  Custom MAC
 */
#define EFUSE_CUSTOM_MAC    0x00FFFFFFU
#define EFUSE_CUSTOM_MAC_M  (EFUSE_CUSTOM_MAC_V << EFUSE_CUSTOM_MAC_S)
#define EFUSE_CUSTOM_MAC_V  0x00FFFFFFU
#define EFUSE_CUSTOM_MAC_S  8

/** EFUSE_RD_USR_DATA7_REG register
 *  Register $n of BLOCK3 (user).
 */
#define EFUSE_RD_USR_DATA7_REG (DR_REG_EFUSE_BASE + 0x98)
/** EFUSE_CUSTOM_MAC_1 : R; bitpos: [23:0]; default: 0;
 *  Custom MAC
 */
#define EFUSE_CUSTOM_MAC_1    0x00FFFFFFU
#define EFUSE_CUSTOM_MAC_1_M  (EFUSE_CUSTOM_MAC_1_V << EFUSE_CUSTOM_MAC_1_S)
#define EFUSE_CUSTOM_MAC_1_V  0x00FFFFFFU
#define EFUSE_CUSTOM_MAC_1_S  0
/** EFUSE_RESERVED_3_248 : R; bitpos: [31:24]; default: 0;
 *  reserved
 */
#define EFUSE_RESERVED_3_248    0x000000FFU
#define EFUSE_RESERVED_3_248_M  (EFUSE_RESERVED_3_248_V << EFUSE_RESERVED_3_248_S)
#define EFUSE_RESERVED_3_248_V  0x000000FFU
#define EFUSE_RESERVED_3_248_S  24

/** EFUSE_RD_KEY0_DATA0_REG register
 *  Register $n of BLOCK4 (KEY0).
 */
#define EFUSE_RD_KEY0_DATA0_REG (DR_REG_EFUSE_BASE + 0x9c)
/** EFUSE_KEY0_DATA0 : RO; bitpos: [31:0]; default: 0;
 *  Stores the zeroth 32 bits of KEY0.
 */
#define EFUSE_KEY0_DATA0    0xFFFFFFFFU
#define EFUSE_KEY0_DATA0_M  (EFUSE_KEY0_DATA0_V << EFUSE_KEY0_DATA0_S)
#define EFUSE_KEY0_DATA0_V  0xFFFFFFFFU
#define EFUSE_KEY0_DATA0_S  0

/** EFUSE_RD_KEY0_DATA1_REG register
 *  Register $n of BLOCK4 (KEY0).
 */
#define EFUSE_RD_KEY0_DATA1_REG (DR_REG_EFUSE_BASE + 0xa0)
/** EFUSE_KEY0_DATA1 : RO; bitpos: [31:0]; default: 0;
 *  Stores the first 32 bits of KEY0.
 */
#define EFUSE_KEY0_DATA1    0xFFFFFFFFU
#define EFUSE_KEY0_DATA1_M  (EFUSE_KEY0_DATA1_V << EFUSE_KEY0_DATA1_S)
#define EFUSE_KEY0_DATA1_V  0xFFFFFFFFU
#define EFUSE_KEY0_DATA1_S  0

/** EFUSE_RD_KEY0_DATA2_REG register
 *  Register $n of BLOCK4 (KEY0).
 */
#define EFUSE_RD_KEY0_DATA2_REG (DR_REG_EFUSE_BASE + 0xa4)
/** EFUSE_KEY0_DATA2 : RO; bitpos: [31:0]; default: 0;
 *  Stores the second 32 bits of KEY0.
 */
#define EFUSE_KEY0_DATA2    0xFFFFFFFFU
#define EFUSE_KEY0_DATA2_M  (EFUSE_KEY0_DATA2_V << EFUSE_KEY0_DATA2_S)
#define EFUSE_KEY0_DATA2_V  0xFFFFFFFFU
#define EFUSE_KEY0_DATA2_S  0

/** EFUSE_RD_KEY0_DATA3_REG register
 *  Register $n of BLOCK4 (KEY0).
 */
#define EFUSE_RD_KEY0_DATA3_REG (DR_REG_EFUSE_BASE + 0xa8)
/** EFUSE_KEY0_DATA3 : RO; bitpos: [31:0]; default: 0;
 *  Stores the third 32 bits of KEY0.
 */
#define EFUSE_KEY0_DATA3    0xFFFFFFFFU
#define EFUSE_KEY0_DATA3_M  (EFUSE_KEY0_DATA3_V << EFUSE_KEY0_DATA3_S)
#define EFUSE_KEY0_DATA3_V  0xFFFFFFFFU
#define EFUSE_KEY0_DATA3_S  0

/** EFUSE_RD_KEY0_DATA4_REG register
 *  Register $n of BLOCK4 (KEY0).
 */
#define EFUSE_RD_KEY0_DATA4_REG (DR_REG_EFUSE_BASE + 0xac)
/** EFUSE_KEY0_DATA4 : RO; bitpos: [31:0]; default: 0;
 *  Stores the fourth 32 bits of KEY0.
 */
#define EFUSE_KEY0_DATA4    0xFFFFFFFFU
#define EFUSE_KEY0_DATA4_M  (EFUSE_KEY0_DATA4_V << EFUSE_KEY0_DATA4_S)
#define EFUSE_KEY0_DATA4_V  0xFFFFFFFFU
#define EFUSE_KEY0_DATA4_S  0

/** EFUSE_RD_KEY0_DATA5_REG register
 *  Register $n of BLOCK4 (KEY0).
 */
#define EFUSE_RD_KEY0_DATA5_REG (DR_REG_EFUSE_BASE + 0xb0)
/** EFUSE_KEY0_DATA5 : RO; bitpos: [31:0]; default: 0;
 *  Stores the fifth 32 bits of KEY0.
 */
#define EFUSE_KEY0_DATA5    0xFFFFFFFFU
#define EFUSE_KEY0_DATA5_M  (EFUSE_KEY0_DATA5_V << EFUSE_KEY0_DATA5_S)
#define EFUSE_KEY0_DATA5_V  0xFFFFFFFFU
#define EFUSE_KEY0_DATA5_S  0

/** EFUSE_RD_KEY0_DATA6_REG register
 *  Register $n of BLOCK4 (KEY0).
 */
#define EFUSE_RD_KEY0_DATA6_REG (DR_REG_EFUSE_BASE + 0xb4)
/** EFUSE_KEY0_DATA6 : RO; bitpos: [31:0]; default: 0;
 *  Stores the sixth 32 bits of KEY0.
 */
#define EFUSE_KEY0_DATA6    0xFFFFFFFFU
#define EFUSE_KEY0_DATA6_M  (EFUSE_KEY0_DATA6_V << EFUSE_KEY0_DATA6_S)
#define EFUSE_KEY0_DATA6_V  0xFFFFFFFFU
#define EFUSE_KEY0_DATA6_S  0

/** EFUSE_RD_KEY0_DATA7_REG register
 *  Register $n of BLOCK4 (KEY0).
 */
#define EFUSE_RD_KEY0_DATA7_REG (DR_REG_EFUSE_BASE + 0xb8)
/** EFUSE_KEY0_DATA7 : RO; bitpos: [31:0]; default: 0;
 *  Stores the seventh 32 bits of KEY0.
 */
#define EFUSE_KEY0_DATA7    0xFFFFFFFFU
#define EFUSE_KEY0_DATA7_M  (EFUSE_KEY0_DATA7_V << EFUSE_KEY0_DATA7_S)
#define EFUSE_KEY0_DATA7_V  0xFFFFFFFFU
#define EFUSE_KEY0_DATA7_S  0

/** EFUSE_RD_KEY1_DATA0_REG register
 *  Register $n of BLOCK5 (KEY1).
 */
#define EFUSE_RD_KEY1_DATA0_REG (DR_REG_EFUSE_BASE + 0xbc)
/** EFUSE_KEY1_DATA0 : RO; bitpos: [31:0]; default: 0;
 *  Stores the zeroth 32 bits of KEY1.
 */
#define EFUSE_KEY1_DATA0    0xFFFFFFFFU
#define EFUSE_KEY1_DATA0_M  (EFUSE_KEY1_DATA0_V << EFUSE_KEY1_DATA0_S)
#define EFUSE_KEY1_DATA0_V  0xFFFFFFFFU
#define EFUSE_KEY1_DATA0_S  0

/** EFUSE_RD_KEY1_DATA1_REG register
 *  Register $n of BLOCK5 (KEY1).
 */
#define EFUSE_RD_KEY1_DATA1_REG (DR_REG_EFUSE_BASE + 0xc0)
/** EFUSE_KEY1_DATA1 : RO; bitpos: [31:0]; default: 0;
 *  Stores the first 32 bits of KEY1.
 */
#define EFUSE_KEY1_DATA1    0xFFFFFFFFU
#define EFUSE_KEY1_DATA1_M  (EFUSE_KEY1_DATA1_V << EFUSE_KEY1_DATA1_S)
#define EFUSE_KEY1_DATA1_V  0xFFFFFFFFU
#define EFUSE_KEY1_DATA1_S  0

/** EFUSE_RD_KEY1_DATA2_REG register
 *  Register $n of BLOCK5 (KEY1).
 */
#define EFUSE_RD_KEY1_DATA2_REG (DR_REG_EFUSE_BASE + 0xc4)
/** EFUSE_KEY1_DATA2 : RO; bitpos: [31:0]; default: 0;
 *  Stores the second 32 bits of KEY1.
 */
#define EFUSE_KEY1_DATA2    0xFFFFFFFFU
#define EFUSE_KEY1_DATA2_M  (EFUSE_KEY1_DATA2_V << EFUSE_KEY1_DATA2_S)
#define EFUSE_KEY1_DATA2_V  0xFFFFFFFFU
#define EFUSE_KEY1_DATA2_S  0

/** EFUSE_RD_KEY1_DATA3_REG register
 *  Register $n of BLOCK5 (KEY1).
 */
#define EFUSE_RD_KEY1_DATA3_REG (DR_REG_EFUSE_BASE + 0xc8)
/** EFUSE_KEY1_DATA3 : RO; bitpos: [31:0]; default: 0;
 *  Stores the third 32 bits of KEY1.
 */
#define EFUSE_KEY1_DATA3    0xFFFFFFFFU
#define EFUSE_KEY1_DATA3_M  (EFUSE_KEY1_DATA3_V << EFUSE_KEY1_DATA3_S)
#define EFUSE_KEY1_DATA3_V  0xFFFFFFFFU
#define EFUSE_KEY1_DATA3_S  0

/** EFUSE_RD_KEY1_DATA4_REG register
 *  Register $n of BLOCK5 (KEY1).
 */
#define EFUSE_RD_KEY1_DATA4_REG (DR_REG_EFUSE_BASE + 0xcc)
/** EFUSE_KEY1_DATA4 : RO; bitpos: [31:0]; default: 0;
 *  Stores the fourth 32 bits of KEY1.
 */
#define EFUSE_KEY1_DATA4    0xFFFFFFFFU
#define EFUSE_KEY1_DATA4_M  (EFUSE_KEY1_DATA4_V << EFUSE_KEY1_DATA4_S)
#define EFUSE_KEY1_DATA4_V  0xFFFFFFFFU
#define EFUSE_KEY1_DATA4_S  0

/** EFUSE_RD_KEY1_DATA5_REG register
 *  Register $n of BLOCK5 (KEY1).
 */
#define EFUSE_RD_KEY1_DATA5_REG (DR_REG_EFUSE_BASE + 0xd0)
/** EFUSE_KEY1_DATA5 : RO; bitpos: [31:0]; default: 0;
 *  Stores the fifth 32 bits of KEY1.
 */
#define EFUSE_KEY1_DATA5    0xFFFFFFFFU
#define EFUSE_KEY1_DATA5_M  (EFUSE_KEY1_DATA5_V << EFUSE_KEY1_DATA5_S)
#define EFUSE_KEY1_DATA5_V  0xFFFFFFFFU
#define EFUSE_KEY1_DATA5_S  0

/** EFUSE_RD_KEY1_DATA6_REG register
 *  Register $n of BLOCK5 (KEY1).
 */
#define EFUSE_RD_KEY1_DATA6_REG (DR_REG_EFUSE_BASE + 0xd4)
/** EFUSE_KEY1_DATA6 : RO; bitpos: [31:0]; default: 0;
 *  Stores the sixth 32 bits of KEY1.
 */
#define EFUSE_KEY1_DATA6    0xFFFFFFFFU
#define EFUSE_KEY1_DATA6_M  (EFUSE_KEY1_DATA6_V << EFUSE_KEY1_DATA6_S)
#define EFUSE_KEY1_DATA6_V  0xFFFFFFFFU
#define EFUSE_KEY1_DATA6_S  0

/** EFUSE_RD_KEY1_DATA7_REG register
 *  Register $n of BLOCK5 (KEY1).
 */
#define EFUSE_RD_KEY1_DATA7_REG (DR_REG_EFUSE_BASE + 0xd8)
/** EFUSE_KEY1_DATA7 : RO; bitpos: [31:0]; default: 0;
 *  Stores the seventh 32 bits of KEY1.
 */
#define EFUSE_KEY1_DATA7    0xFFFFFFFFU
#define EFUSE_KEY1_DATA7_M  (EFUSE_KEY1_DATA7_V << EFUSE_KEY1_DATA7_S)
#define EFUSE_KEY1_DATA7_V  0xFFFFFFFFU
#define EFUSE_KEY1_DATA7_S  0

/** EFUSE_RD_KEY2_DATA0_REG register
 *  Register $n of BLOCK6 (KEY2).
 */
#define EFUSE_RD_KEY2_DATA0_REG (DR_REG_EFUSE_BASE + 0xdc)
/** EFUSE_KEY2_DATA0 : RO; bitpos: [31:0]; default: 0;
 *  Stores the zeroth 32 bits of KEY2.
 */
#define EFUSE_KEY2_DATA0    0xFFFFFFFFU
#define EFUSE_KEY2_DATA0_M  (EFUSE_KEY2_DATA0_V << EFUSE_KEY2_DATA0_S)
#define EFUSE_KEY2_DATA0_V  0xFFFFFFFFU
#define EFUSE_KEY2_DATA0_S  0

/** EFUSE_RD_KEY2_DATA1_REG register
 *  Register $n of BLOCK6 (KEY2).
 */
#define EFUSE_RD_KEY2_DATA1_REG (DR_REG_EFUSE_BASE + 0xe0)
/** EFUSE_KEY2_DATA1 : RO; bitpos: [31:0]; default: 0;
 *  Stores the first 32 bits of KEY2.
 */
#define EFUSE_KEY2_DATA1    0xFFFFFFFFU
#define EFUSE_KEY2_DATA1_M  (EFUSE_KEY2_DATA1_V << EFUSE_KEY2_DATA1_S)
#define EFUSE_KEY2_DATA1_V  0xFFFFFFFFU
#define EFUSE_KEY2_DATA1_S  0

/** EFUSE_RD_KEY2_DATA2_REG register
 *  Register $n of BLOCK6 (KEY2).
 */
#define EFUSE_RD_KEY2_DATA2_REG (DR_REG_EFUSE_BASE + 0xe4)
/** EFUSE_KEY2_DATA2 : RO; bitpos: [31:0]; default: 0;
 *  Stores the second 32 bits of KEY2.
 */
#define EFUSE_KEY2_DATA2    0xFFFFFFFFU
#define EFUSE_KEY2_DATA2_M  (EFUSE_KEY2_DATA2_V << EFUSE_KEY2_DATA2_S)
#define EFUSE_KEY2_DATA2_V  0xFFFFFFFFU
#define EFUSE_KEY2_DATA2_S  0

/** EFUSE_RD_KEY2_DATA3_REG register
 *  Register $n of BLOCK6 (KEY2).
 */
#define EFUSE_RD_KEY2_DATA3_REG (DR_REG_EFUSE_BASE + 0xe8)
/** EFUSE_KEY2_DATA3 : RO; bitpos: [31:0]; default: 0;
 *  Stores the third 32 bits of KEY2.
 */
#define EFUSE_KEY2_DATA3    0xFFFFFFFFU
#define EFUSE_KEY2_DATA3_M  (EFUSE_KEY2_DATA3_V << EFUSE_KEY2_DATA3_S)
#define EFUSE_KEY2_DATA3_V  0xFFFFFFFFU
#define EFUSE_KEY2_DATA3_S  0

/** EFUSE_RD_KEY2_DATA4_REG register
 *  Register $n of BLOCK6 (KEY2).
 */
#define EFUSE_RD_KEY2_DATA4_REG (DR_REG_EFUSE_BASE + 0xec)
/** EFUSE_KEY2_DATA4 : RO; bitpos: [31:0]; default: 0;
 *  Stores the fourth 32 bits of KEY2.
 */
#define EFUSE_KEY2_DATA4    0xFFFFFFFFU
#define EFUSE_KEY2_DATA4_M  (EFUSE_KEY2_DATA4_V << EFUSE_KEY2_DATA4_S)
#define EFUSE_KEY2_DATA4_V  0xFFFFFFFFU
#define EFUSE_KEY2_DATA4_S  0

/** EFUSE_RD_KEY2_DATA5_REG register
 *  Register $n of BLOCK6 (KEY2).
 */
#define EFUSE_RD_KEY2_DATA5_REG (DR_REG_EFUSE_BASE + 0xf0)
/** EFUSE_KEY2_DATA5 : RO; bitpos: [31:0]; default: 0;
 *  Stores the fifth 32 bits of KEY2.
 */
#define EFUSE_KEY2_DATA5    0xFFFFFFFFU
#define EFUSE_KEY2_DATA5_M  (EFUSE_KEY2_DATA5_V << EFUSE_KEY2_DATA5_S)
#define EFUSE_KEY2_DATA5_V  0xFFFFFFFFU
#define EFUSE_KEY2_DATA5_S  0

/** EFUSE_RD_KEY2_DATA6_REG register
 *  Register $n of BLOCK6 (KEY2).
 */
#define EFUSE_RD_KEY2_DATA6_REG (DR_REG_EFUSE_BASE + 0xf4)
/** EFUSE_KEY2_DATA6 : RO; bitpos: [31:0]; default: 0;
 *  Stores the sixth 32 bits of KEY2.
 */
#define EFUSE_KEY2_DATA6    0xFFFFFFFFU
#define EFUSE_KEY2_DATA6_M  (EFUSE_KEY2_DATA6_V << EFUSE_KEY2_DATA6_S)
#define EFUSE_KEY2_DATA6_V  0xFFFFFFFFU
#define EFUSE_KEY2_DATA6_S  0

/** EFUSE_RD_KEY2_DATA7_REG register
 *  Register $n of BLOCK6 (KEY2).
 */
#define EFUSE_RD_KEY2_DATA7_REG (DR_REG_EFUSE_BASE + 0xf8)
/** EFUSE_KEY2_DATA7 : RO; bitpos: [31:0]; default: 0;
 *  Stores the seventh 32 bits of KEY2.
 */
#define EFUSE_KEY2_DATA7    0xFFFFFFFFU
#define EFUSE_KEY2_DATA7_M  (EFUSE_KEY2_DATA7_V << EFUSE_KEY2_DATA7_S)
#define EFUSE_KEY2_DATA7_V  0xFFFFFFFFU
#define EFUSE_KEY2_DATA7_S  0

/** EFUSE_RD_KEY3_DATA0_REG register
 *  Register $n of BLOCK7 (KEY3).
 */
#define EFUSE_RD_KEY3_DATA0_REG (DR_REG_EFUSE_BASE + 0xfc)
/** EFUSE_KEY3_DATA0 : RO; bitpos: [31:0]; default: 0;
 *  Stores the zeroth 32 bits of KEY3.
 */
#define EFUSE_KEY3_DATA0    0xFFFFFFFFU
#define EFUSE_KEY3_DATA0_M  (EFUSE_KEY3_DATA0_V << EFUSE_KEY3_DATA0_S)
#define EFUSE_KEY3_DATA0_V  0xFFFFFFFFU
#define EFUSE_KEY3_DATA0_S  0

/** EFUSE_RD_KEY3_DATA1_REG register
 *  Register $n of BLOCK7 (KEY3).
 */
#define EFUSE_RD_KEY3_DATA1_REG (DR_REG_EFUSE_BASE + 0x100)
/** EFUSE_KEY3_DATA1 : RO; bitpos: [31:0]; default: 0;
 *  Stores the first 32 bits of KEY3.
 */
#define EFUSE_KEY3_DATA1    0xFFFFFFFFU
#define EFUSE_KEY3_DATA1_M  (EFUSE_KEY3_DATA1_V << EFUSE_KEY3_DATA1_S)
#define EFUSE_KEY3_DATA1_V  0xFFFFFFFFU
#define EFUSE_KEY3_DATA1_S  0

/** EFUSE_RD_KEY3_DATA2_REG register
 *  Register $n of BLOCK7 (KEY3).
 */
#define EFUSE_RD_KEY3_DATA2_REG (DR_REG_EFUSE_BASE + 0x104)
/** EFUSE_KEY3_DATA2 : RO; bitpos: [31:0]; default: 0;
 *  Stores the second 32 bits of KEY3.
 */
#define EFUSE_KEY3_DATA2    0xFFFFFFFFU
#define EFUSE_KEY3_DATA2_M  (EFUSE_KEY3_DATA2_V << EFUSE_KEY3_DATA2_S)
#define EFUSE_KEY3_DATA2_V  0xFFFFFFFFU
#define EFUSE_KEY3_DATA2_S  0

/** EFUSE_RD_KEY3_DATA3_REG register
 *  Register $n of BLOCK7 (KEY3).
 */
#define EFUSE_RD_KEY3_DATA3_REG (DR_REG_EFUSE_BASE + 0x108)
/** EFUSE_KEY3_DATA3 : RO; bitpos: [31:0]; default: 0;
 *  Stores the third 32 bits of KEY3.
 */
#define EFUSE_KEY3_DATA3    0xFFFFFFFFU
#define EFUSE_KEY3_DATA3_M  (EFUSE_KEY3_DATA3_V << EFUSE_KEY3_DATA3_S)
#define EFUSE_KEY3_DATA3_V  0xFFFFFFFFU
#define EFUSE_KEY3_DATA3_S  0

/** EFUSE_RD_KEY3_DATA4_REG register
 *  Register $n of BLOCK7 (KEY3).
 */
#define EFUSE_RD_KEY3_DATA4_REG (DR_REG_EFUSE_BASE + 0x10c)
/** EFUSE_KEY3_DATA4 : RO; bitpos: [31:0]; default: 0;
 *  Stores the fourth 32 bits of KEY3.
 */
#define EFUSE_KEY3_DATA4    0xFFFFFFFFU
#define EFUSE_KEY3_DATA4_M  (EFUSE_KEY3_DATA4_V << EFUSE_KEY3_DATA4_S)
#define EFUSE_KEY3_DATA4_V  0xFFFFFFFFU
#define EFUSE_KEY3_DATA4_S  0

/** EFUSE_RD_KEY3_DATA5_REG register
 *  Register $n of BLOCK7 (KEY3).
 */
#define EFUSE_RD_KEY3_DATA5_REG (DR_REG_EFUSE_BASE + 0x110)
/** EFUSE_KEY3_DATA5 : RO; bitpos: [31:0]; default: 0;
 *  Stores the fifth 32 bits of KEY3.
 */
#define EFUSE_KEY3_DATA5    0xFFFFFFFFU
#define EFUSE_KEY3_DATA5_M  (EFUSE_KEY3_DATA5_V << EFUSE_KEY3_DATA5_S)
#define EFUSE_KEY3_DATA5_V  0xFFFFFFFFU
#define EFUSE_KEY3_DATA5_S  0

/** EFUSE_RD_KEY3_DATA6_REG register
 *  Register $n of BLOCK7 (KEY3).
 */
#define EFUSE_RD_KEY3_DATA6_REG (DR_REG_EFUSE_BASE + 0x114)
/** EFUSE_KEY3_DATA6 : RO; bitpos: [31:0]; default: 0;
 *  Stores the sixth 32 bits of KEY3.
 */
#define EFUSE_KEY3_DATA6    0xFFFFFFFFU
#define EFUSE_KEY3_DATA6_M  (EFUSE_KEY3_DATA6_V << EFUSE_KEY3_DATA6_S)
#define EFUSE_KEY3_DATA6_V  0xFFFFFFFFU
#define EFUSE_KEY3_DATA6_S  0

/** EFUSE_RD_KEY3_DATA7_REG register
 *  Register $n of BLOCK7 (KEY3).
 */
#define EFUSE_RD_KEY3_DATA7_REG (DR_REG_EFUSE_BASE + 0x118)
/** EFUSE_KEY3_DATA7 : RO; bitpos: [31:0]; default: 0;
 *  Stores the seventh 32 bits of KEY3.
 */
#define EFUSE_KEY3_DATA7    0xFFFFFFFFU
#define EFUSE_KEY3_DATA7_M  (EFUSE_KEY3_DATA7_V << EFUSE_KEY3_DATA7_S)
#define EFUSE_KEY3_DATA7_V  0xFFFFFFFFU
#define EFUSE_KEY3_DATA7_S  0

/** EFUSE_RD_KEY4_DATA0_REG register
 *  Register $n of BLOCK8 (KEY4).
 */
#define EFUSE_RD_KEY4_DATA0_REG (DR_REG_EFUSE_BASE + 0x11c)
/** EFUSE_KEY4_DATA0 : RO; bitpos: [31:0]; default: 0;
 *  Stores the zeroth 32 bits of KEY4.
 */
#define EFUSE_KEY4_DATA0    0xFFFFFFFFU
#define EFUSE_KEY4_DATA0_M  (EFUSE_KEY4_DATA0_V << EFUSE_KEY4_DATA0_S)
#define EFUSE_KEY4_DATA0_V  0xFFFFFFFFU
#define EFUSE_KEY4_DATA0_S  0

/** EFUSE_RD_KEY4_DATA1_REG register
 *  Register $n of BLOCK8 (KEY4).
 */
#define EFUSE_RD_KEY4_DATA1_REG (DR_REG_EFUSE_BASE + 0x120)
/** EFUSE_KEY4_DATA1 : RO; bitpos: [31:0]; default: 0;
 *  Stores the first 32 bits of KEY4.
 */
#define EFUSE_KEY4_DATA1    0xFFFFFFFFU
#define EFUSE_KEY4_DATA1_M  (EFUSE_KEY4_DATA1_V << EFUSE_KEY4_DATA1_S)
#define EFUSE_KEY4_DATA1_V  0xFFFFFFFFU
#define EFUSE_KEY4_DATA1_S  0

/** EFUSE_RD_KEY4_DATA2_REG register
 *  Register $n of BLOCK8 (KEY4).
 */
#define EFUSE_RD_KEY4_DATA2_REG (DR_REG_EFUSE_BASE + 0x124)
/** EFUSE_KEY4_DATA2 : RO; bitpos: [31:0]; default: 0;
 *  Stores the second 32 bits of KEY4.
 */
#define EFUSE_KEY4_DATA2    0xFFFFFFFFU
#define EFUSE_KEY4_DATA2_M  (EFUSE_KEY4_DATA2_V << EFUSE_KEY4_DATA2_S)
#define EFUSE_KEY4_DATA2_V  0xFFFFFFFFU
#define EFUSE_KEY4_DATA2_S  0

/** EFUSE_RD_KEY4_DATA3_REG register
 *  Register $n of BLOCK8 (KEY4).
 */
#define EFUSE_RD_KEY4_DATA3_REG (DR_REG_EFUSE_BASE + 0x128)
/** EFUSE_KEY4_DATA3 : RO; bitpos: [31:0]; default: 0;
 *  Stores the third 32 bits of KEY4.
 */
#define EFUSE_KEY4_DATA3    0xFFFFFFFFU
#define EFUSE_KEY4_DATA3_M  (EFUSE_KEY4_DATA3_V << EFUSE_KEY4_DATA3_S)
#define EFUSE_KEY4_DATA3_V  0xFFFFFFFFU
#define EFUSE_KEY4_DATA3_S  0

/** EFUSE_RD_KEY4_DATA4_REG register
 *  Register $n of BLOCK8 (KEY4).
 */
#define EFUSE_RD_KEY4_DATA4_REG (DR_REG_EFUSE_BASE + 0x12c)
/** EFUSE_KEY4_DATA4 : RO; bitpos: [31:0]; default: 0;
 *  Stores the fourth 32 bits of KEY4.
 */
#define EFUSE_KEY4_DATA4    0xFFFFFFFFU
#define EFUSE_KEY4_DATA4_M  (EFUSE_KEY4_DATA4_V << EFUSE_KEY4_DATA4_S)
#define EFUSE_KEY4_DATA4_V  0xFFFFFFFFU
#define EFUSE_KEY4_DATA4_S  0

/** EFUSE_RD_KEY4_DATA5_REG register
 *  Register $n of BLOCK8 (KEY4).
 */
#define EFUSE_RD_KEY4_DATA5_REG (DR_REG_EFUSE_BASE + 0x130)
/** EFUSE_KEY4_DATA5 : RO; bitpos: [31:0]; default: 0;
 *  Stores the fifth 32 bits of KEY4.
 */
#define EFUSE_KEY4_DATA5    0xFFFFFFFFU
#define EFUSE_KEY4_DATA5_M  (EFUSE_KEY4_DATA5_V << EFUSE_KEY4_DATA5_S)
#define EFUSE_KEY4_DATA5_V  0xFFFFFFFFU
#define EFUSE_KEY4_DATA5_S  0

/** EFUSE_RD_KEY4_DATA6_REG register
 *  Register $n of BLOCK8 (KEY4).
 */
#define EFUSE_RD_KEY4_DATA6_REG (DR_REG_EFUSE_BASE + 0x134)
/** EFUSE_KEY4_DATA6 : RO; bitpos: [31:0]; default: 0;
 *  Stores the sixth 32 bits of KEY4.
 */
#define EFUSE_KEY4_DATA6    0xFFFFFFFFU
#define EFUSE_KEY4_DATA6_M  (EFUSE_KEY4_DATA6_V << EFUSE_KEY4_DATA6_S)
#define EFUSE_KEY4_DATA6_V  0xFFFFFFFFU
#define EFUSE_KEY4_DATA6_S  0

/** EFUSE_RD_KEY4_DATA7_REG register
 *  Register $n of BLOCK8 (KEY4).
 */
#define EFUSE_RD_KEY4_DATA7_REG (DR_REG_EFUSE_BASE + 0x138)
/** EFUSE_KEY4_DATA7 : RO; bitpos: [31:0]; default: 0;
 *  Stores the seventh 32 bits of KEY4.
 */
#define EFUSE_KEY4_DATA7    0xFFFFFFFFU
#define EFUSE_KEY4_DATA7_M  (EFUSE_KEY4_DATA7_V << EFUSE_KEY4_DATA7_S)
#define EFUSE_KEY4_DATA7_V  0xFFFFFFFFU
#define EFUSE_KEY4_DATA7_S  0

/** EFUSE_RD_KEY5_DATA0_REG register
 *  Register $n of BLOCK9 (KEY5).
 */
#define EFUSE_RD_KEY5_DATA0_REG (DR_REG_EFUSE_BASE + 0x13c)
/** EFUSE_KEY5_DATA0 : RO; bitpos: [31:0]; default: 0;
 *  Stores the zeroth 32 bits of KEY5.
 */
#define EFUSE_KEY5_DATA0    0xFFFFFFFFU
#define EFUSE_KEY5_DATA0_M  (EFUSE_KEY5_DATA0_V << EFUSE_KEY5_DATA0_S)
#define EFUSE_KEY5_DATA0_V  0xFFFFFFFFU
#define EFUSE_KEY5_DATA0_S  0

/** EFUSE_RD_KEY5_DATA1_REG register
 *  Register $n of BLOCK9 (KEY5).
 */
#define EFUSE_RD_KEY5_DATA1_REG (DR_REG_EFUSE_BASE + 0x140)
/** EFUSE_KEY5_DATA1 : RO; bitpos: [31:0]; default: 0;
 *  Stores the first 32 bits of KEY5.
 */
#define EFUSE_KEY5_DATA1    0xFFFFFFFFU
#define EFUSE_KEY5_DATA1_M  (EFUSE_KEY5_DATA1_V << EFUSE_KEY5_DATA1_S)
#define EFUSE_KEY5_DATA1_V  0xFFFFFFFFU
#define EFUSE_KEY5_DATA1_S  0

/** EFUSE_RD_KEY5_DATA2_REG register
 *  Register $n of BLOCK9 (KEY5).
 */
#define EFUSE_RD_KEY5_DATA2_REG (DR_REG_EFUSE_BASE + 0x144)
/** EFUSE_KEY5_DATA2 : RO; bitpos: [31:0]; default: 0;
 *  Stores the second 32 bits of KEY5.
 */
#define EFUSE_KEY5_DATA2    0xFFFFFFFFU
#define EFUSE_KEY5_DATA2_M  (EFUSE_KEY5_DATA2_V << EFUSE_KEY5_DATA2_S)
#define EFUSE_KEY5_DATA2_V  0xFFFFFFFFU
#define EFUSE_KEY5_DATA2_S  0

/** EFUSE_RD_KEY5_DATA3_REG register
 *  Register $n of BLOCK9 (KEY5).
 */
#define EFUSE_RD_KEY5_DATA3_REG (DR_REG_EFUSE_BASE + 0x148)
/** EFUSE_KEY5_DATA3 : RO; bitpos: [31:0]; default: 0;
 *  Stores the third 32 bits of KEY5.
 */
#define EFUSE_KEY5_DATA3    0xFFFFFFFFU
#define EFUSE_KEY5_DATA3_M  (EFUSE_KEY5_DATA3_V << EFUSE_KEY5_DATA3_S)
#define EFUSE_KEY5_DATA3_V  0xFFFFFFFFU
#define EFUSE_KEY5_DATA3_S  0

/** EFUSE_RD_KEY5_DATA4_REG register
 *  Register $n of BLOCK9 (KEY5).
 */
#define EFUSE_RD_KEY5_DATA4_REG (DR_REG_EFUSE_BASE + 0x14c)
/** EFUSE_KEY5_DATA4 : RO; bitpos: [31:0]; default: 0;
 *  Stores the fourth 32 bits of KEY5.
 */
#define EFUSE_KEY5_DATA4    0xFFFFFFFFU
#define EFUSE_KEY5_DATA4_M  (EFUSE_KEY5_DATA4_V << EFUSE_KEY5_DATA4_S)
#define EFUSE_KEY5_DATA4_V  0xFFFFFFFFU
#define EFUSE_KEY5_DATA4_S  0

/** EFUSE_RD_KEY5_DATA5_REG register
 *  Register $n of BLOCK9 (KEY5).
 */
#define EFUSE_RD_KEY5_DATA5_REG (DR_REG_EFUSE_BASE + 0x150)
/** EFUSE_KEY5_DATA5 : RO; bitpos: [31:0]; default: 0;
 *  Stores the fifth 32 bits of KEY5.
 */
#define EFUSE_KEY5_DATA5    0xFFFFFFFFU
#define EFUSE_KEY5_DATA5_M  (EFUSE_KEY5_DATA5_V << EFUSE_KEY5_DATA5_S)
#define EFUSE_KEY5_DATA5_V  0xFFFFFFFFU
#define EFUSE_KEY5_DATA5_S  0

/** EFUSE_RD_KEY5_DATA6_REG register
 *  Register $n of BLOCK9 (KEY5).
 */
#define EFUSE_RD_KEY5_DATA6_REG (DR_REG_EFUSE_BASE + 0x154)
/** EFUSE_KEY5_DATA6 : RO; bitpos: [31:0]; default: 0;
 *  Stores the sixth 32 bits of KEY5.
 */
#define EFUSE_KEY5_DATA6    0xFFFFFFFFU
#define EFUSE_KEY5_DATA6_M  (EFUSE_KEY5_DATA6_V << EFUSE_KEY5_DATA6_S)
#define EFUSE_KEY5_DATA6_V  0xFFFFFFFFU
#define EFUSE_KEY5_DATA6_S  0

/** EFUSE_RD_KEY5_DATA7_REG register
 *  Register $n of BLOCK9 (KEY5).
 */
#define EFUSE_RD_KEY5_DATA7_REG (DR_REG_EFUSE_BASE + 0x158)
/** EFUSE_KEY5_DATA7 : RO; bitpos: [31:0]; default: 0;
 *  Stores the seventh 32 bits of KEY5.
 */
#define EFUSE_KEY5_DATA7    0xFFFFFFFFU
#define EFUSE_KEY5_DATA7_M  (EFUSE_KEY5_DATA7_V << EFUSE_KEY5_DATA7_S)
#define EFUSE_KEY5_DATA7_V  0xFFFFFFFFU
#define EFUSE_KEY5_DATA7_S  0

/** EFUSE_RD_SYS_PART2_DATA0_REG register
 *  Register $n of BLOCK10 (system).
 */
#define EFUSE_RD_SYS_PART2_DATA0_REG (DR_REG_EFUSE_BASE + 0x15c)
/** EFUSE_SYS_DATA_PART2_0 : RO; bitpos: [31:0]; default: 0;
 *  Stores the $nth 32 bits of the 2nd part of system data.
 */
#define EFUSE_SYS_DATA_PART2_0    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_0_M  (EFUSE_SYS_DATA_PART2_0_V << EFUSE_SYS_DATA_PART2_0_S)
#define EFUSE_SYS_DATA_PART2_0_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_0_S  0

/** EFUSE_RD_SYS_PART2_DATA1_REG register
 *  Register $n of BLOCK9 (KEY5).
 */
#define EFUSE_RD_SYS_PART2_DATA1_REG (DR_REG_EFUSE_BASE + 0x160)
/** EFUSE_SYS_DATA_PART2_1 : RO; bitpos: [31:0]; default: 0;
 *  Stores the $nth 32 bits of the 2nd part of system data.
 */
#define EFUSE_SYS_DATA_PART2_1    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_1_M  (EFUSE_SYS_DATA_PART2_1_V << EFUSE_SYS_DATA_PART2_1_S)
#define EFUSE_SYS_DATA_PART2_1_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_1_S  0

/** EFUSE_RD_SYS_PART2_DATA2_REG register
 *  Register $n of BLOCK10 (system).
 */
#define EFUSE_RD_SYS_PART2_DATA2_REG (DR_REG_EFUSE_BASE + 0x164)
/** EFUSE_SYS_DATA_PART2_2 : RO; bitpos: [31:0]; default: 0;
 *  Stores the $nth 32 bits of the 2nd part of system data.
 */
#define EFUSE_SYS_DATA_PART2_2    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_2_M  (EFUSE_SYS_DATA_PART2_2_V << EFUSE_SYS_DATA_PART2_2_S)
#define EFUSE_SYS_DATA_PART2_2_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_2_S  0

/** EFUSE_RD_SYS_PART2_DATA3_REG register
 *  Register $n of BLOCK10 (system).
 */
#define EFUSE_RD_SYS_PART2_DATA3_REG (DR_REG_EFUSE_BASE + 0x168)
/** EFUSE_SYS_DATA_PART2_3 : RO; bitpos: [31:0]; default: 0;
 *  Stores the $nth 32 bits of the 2nd part of system data.
 */
#define EFUSE_SYS_DATA_PART2_3    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_3_M  (EFUSE_SYS_DATA_PART2_3_V << EFUSE_SYS_DATA_PART2_3_S)
#define EFUSE_SYS_DATA_PART2_3_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_3_S  0

/** EFUSE_RD_SYS_PART2_DATA4_REG register
 *  Register $n of BLOCK10 (system).
 */
#define EFUSE_RD_SYS_PART2_DATA4_REG (DR_REG_EFUSE_BASE + 0x16c)
/** EFUSE_SYS_DATA_PART2_4 : RO; bitpos: [31:0]; default: 0;
 *  Stores the $nth 32 bits of the 2nd part of system data.
 */
#define EFUSE_SYS_DATA_PART2_4    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_4_M  (EFUSE_SYS_DATA_PART2_4_V << EFUSE_SYS_DATA_PART2_4_S)
#define EFUSE_SYS_DATA_PART2_4_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_4_S  0

/** EFUSE_RD_SYS_PART2_DATA5_REG register
 *  Register $n of BLOCK10 (system).
 */
#define EFUSE_RD_SYS_PART2_DATA5_REG (DR_REG_EFUSE_BASE + 0x170)
/** EFUSE_SYS_DATA_PART2_5 : RO; bitpos: [31:0]; default: 0;
 *  Stores the $nth 32 bits of the 2nd part of system data.
 */
#define EFUSE_SYS_DATA_PART2_5    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_5_M  (EFUSE_SYS_DATA_PART2_5_V << EFUSE_SYS_DATA_PART2_5_S)
#define EFUSE_SYS_DATA_PART2_5_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_5_S  0

/** EFUSE_RD_SYS_PART2_DATA6_REG register
 *  Register $n of BLOCK10 (system).
 */
#define EFUSE_RD_SYS_PART2_DATA6_REG (DR_REG_EFUSE_BASE + 0x174)
/** EFUSE_SYS_DATA_PART2_6 : RO; bitpos: [31:0]; default: 0;
 *  Stores the $nth 32 bits of the 2nd part of system data.
 */
#define EFUSE_SYS_DATA_PART2_6    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_6_M  (EFUSE_SYS_DATA_PART2_6_V << EFUSE_SYS_DATA_PART2_6_S)
#define EFUSE_SYS_DATA_PART2_6_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_6_S  0

/** EFUSE_RD_SYS_PART2_DATA7_REG register
 *  Register $n of BLOCK10 (system).
 */
#define EFUSE_RD_SYS_PART2_DATA7_REG (DR_REG_EFUSE_BASE + 0x178)
/** EFUSE_SYS_DATA_PART2_7 : RO; bitpos: [31:0]; default: 0;
 *  Stores the $nth 32 bits of the 2nd part of system data.
 */
#define EFUSE_SYS_DATA_PART2_7    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_7_M  (EFUSE_SYS_DATA_PART2_7_V << EFUSE_SYS_DATA_PART2_7_S)
#define EFUSE_SYS_DATA_PART2_7_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_7_S  0

/** EFUSE_RD_REPEAT_ERR0_REG register
 *  Programming error record register 0 of BLOCK0.
 */
#define EFUSE_RD_REPEAT_ERR0_REG (DR_REG_EFUSE_BASE + 0x17c)
/** EFUSE_RD_DIS_ERR : RO; bitpos: [6:0]; default: 0;
 *  Indicates a programming error of RD_DIS.
 */
#define EFUSE_RD_DIS_ERR    0x0000007FU
#define EFUSE_RD_DIS_ERR_M  (EFUSE_RD_DIS_ERR_V << EFUSE_RD_DIS_ERR_S)
#define EFUSE_RD_DIS_ERR_V  0x0000007FU
#define EFUSE_RD_DIS_ERR_S  0
/** EFUSE_DIS_USB_DEVICE_EXCHG_PINS_ERR : RO; bitpos: [7]; default: 0;
 *  Indicates a programming error of DIS_USB_DEVICE_EXCHG_PINS.
 */
#define EFUSE_DIS_USB_DEVICE_EXCHG_PINS_ERR    (BIT(7))
#define EFUSE_DIS_USB_DEVICE_EXCHG_PINS_ERR_M  (EFUSE_DIS_USB_DEVICE_EXCHG_PINS_ERR_V << EFUSE_DIS_USB_DEVICE_EXCHG_PINS_ERR_S)
#define EFUSE_DIS_USB_DEVICE_EXCHG_PINS_ERR_V  0x00000001U
#define EFUSE_DIS_USB_DEVICE_EXCHG_PINS_ERR_S  7
/** EFUSE_DIS_USB_OTG11_EXCHG_PINS_ERR : RO; bitpos: [8]; default: 0;
 *  Indicates a programming error of DIS_USB_OTG11_EXCHG_PINS.
 */
#define EFUSE_DIS_USB_OTG11_EXCHG_PINS_ERR    (BIT(8))
#define EFUSE_DIS_USB_OTG11_EXCHG_PINS_ERR_M  (EFUSE_DIS_USB_OTG11_EXCHG_PINS_ERR_V << EFUSE_DIS_USB_OTG11_EXCHG_PINS_ERR_S)
#define EFUSE_DIS_USB_OTG11_EXCHG_PINS_ERR_V  0x00000001U
#define EFUSE_DIS_USB_OTG11_EXCHG_PINS_ERR_S  8
/** EFUSE_DIS_USB_JTAG_ERR : RO; bitpos: [9]; default: 0;
 *  Indicates a programming error of DIS_USB_JTAG.
 */
#define EFUSE_DIS_USB_JTAG_ERR    (BIT(9))
#define EFUSE_DIS_USB_JTAG_ERR_M  (EFUSE_DIS_USB_JTAG_ERR_V << EFUSE_DIS_USB_JTAG_ERR_S)
#define EFUSE_DIS_USB_JTAG_ERR_V  0x00000001U
#define EFUSE_DIS_USB_JTAG_ERR_S  9
/** EFUSE_POWERGLITCH_EN_ERR : RO; bitpos: [10]; default: 0;
 *  Indicates a programming error of POWERGLITCH_EN.
 */
#define EFUSE_POWERGLITCH_EN_ERR    (BIT(10))
#define EFUSE_POWERGLITCH_EN_ERR_M  (EFUSE_POWERGLITCH_EN_ERR_V << EFUSE_POWERGLITCH_EN_ERR_S)
#define EFUSE_POWERGLITCH_EN_ERR_V  0x00000001U
#define EFUSE_POWERGLITCH_EN_ERR_S  10
/** EFUSE_DIS_USB_SERIAL_JTAG_ERR : RO; bitpos: [11]; default: 0;
 *  Indicates a programming error of DIS_USB_SERIAL_JTAG.
 */
#define EFUSE_DIS_USB_SERIAL_JTAG_ERR    (BIT(11))
#define EFUSE_DIS_USB_SERIAL_JTAG_ERR_M  (EFUSE_DIS_USB_SERIAL_JTAG_ERR_V << EFUSE_DIS_USB_SERIAL_JTAG_ERR_S)
#define EFUSE_DIS_USB_SERIAL_JTAG_ERR_V  0x00000001U
#define EFUSE_DIS_USB_SERIAL_JTAG_ERR_S  11
/** EFUSE_DIS_FORCE_DOWNLOAD_ERR : RO; bitpos: [12]; default: 0;
 *  Indicates a programming error of DIS_FORCE_DOWNLOAD.
 */
#define EFUSE_DIS_FORCE_DOWNLOAD_ERR    (BIT(12))
#define EFUSE_DIS_FORCE_DOWNLOAD_ERR_M  (EFUSE_DIS_FORCE_DOWNLOAD_ERR_V << EFUSE_DIS_FORCE_DOWNLOAD_ERR_S)
#define EFUSE_DIS_FORCE_DOWNLOAD_ERR_V  0x00000001U
#define EFUSE_DIS_FORCE_DOWNLOAD_ERR_S  12
/** EFUSE_SPI_DOWNLOAD_MSPI_DIS_ERR : RO; bitpos: [13]; default: 0;
 *  Indicates a programming error of SPI_DOWNLOAD_MSPI_DIS.
 */
#define EFUSE_SPI_DOWNLOAD_MSPI_DIS_ERR    (BIT(13))
#define EFUSE_SPI_DOWNLOAD_MSPI_DIS_ERR_M  (EFUSE_SPI_DOWNLOAD_MSPI_DIS_ERR_V << EFUSE_SPI_DOWNLOAD_MSPI_DIS_ERR_S)
#define EFUSE_SPI_DOWNLOAD_MSPI_DIS_ERR_V  0x00000001U
#define EFUSE_SPI_DOWNLOAD_MSPI_DIS_ERR_S  13
/** EFUSE_DIS_TWAI_ERR : RO; bitpos: [14]; default: 0;
 *  Indicates a programming error of DIS_TWAI.
 */
#define EFUSE_DIS_TWAI_ERR    (BIT(14))
#define EFUSE_DIS_TWAI_ERR_M  (EFUSE_DIS_TWAI_ERR_V << EFUSE_DIS_TWAI_ERR_S)
#define EFUSE_DIS_TWAI_ERR_V  0x00000001U
#define EFUSE_DIS_TWAI_ERR_S  14
/** EFUSE_JTAG_SEL_ENABLE_ERR : RO; bitpos: [15]; default: 0;
 *  Indicates a programming error of JTAG_SEL_ENABLE.
 */
#define EFUSE_JTAG_SEL_ENABLE_ERR    (BIT(15))
#define EFUSE_JTAG_SEL_ENABLE_ERR_M  (EFUSE_JTAG_SEL_ENABLE_ERR_V << EFUSE_JTAG_SEL_ENABLE_ERR_S)
#define EFUSE_JTAG_SEL_ENABLE_ERR_V  0x00000001U
#define EFUSE_JTAG_SEL_ENABLE_ERR_S  15
/** EFUSE_SOFT_DIS_JTAG_ERR : RO; bitpos: [18:16]; default: 0;
 *  Indicates a programming error of SOFT_DIS_JTAG.
 */
#define EFUSE_SOFT_DIS_JTAG_ERR    0x00000007U
#define EFUSE_SOFT_DIS_JTAG_ERR_M  (EFUSE_SOFT_DIS_JTAG_ERR_V << EFUSE_SOFT_DIS_JTAG_ERR_S)
#define EFUSE_SOFT_DIS_JTAG_ERR_V  0x00000007U
#define EFUSE_SOFT_DIS_JTAG_ERR_S  16
/** EFUSE_DIS_PAD_JTAG_ERR : RO; bitpos: [19]; default: 0;
 *  Indicates a programming error of DIS_PAD_JTAG.
 */
#define EFUSE_DIS_PAD_JTAG_ERR    (BIT(19))
#define EFUSE_DIS_PAD_JTAG_ERR_M  (EFUSE_DIS_PAD_JTAG_ERR_V << EFUSE_DIS_PAD_JTAG_ERR_S)
#define EFUSE_DIS_PAD_JTAG_ERR_V  0x00000001U
#define EFUSE_DIS_PAD_JTAG_ERR_S  19
/** EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR : RO; bitpos: [20]; default: 0;
 *  Indicates a programming error of DIS_DOWNLOAD_MANUAL_ENCRYPT.
 */
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR    (BIT(20))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_M  (EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_V << EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_S)
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_V  0x00000001U
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_S  20
/** EFUSE_USB_DEVICE_DREFH_ERR : RO; bitpos: [22:21]; default: 0;
 *  Indicates a programming error of USB_DEVICE_DREFH.
 */
#define EFUSE_USB_DEVICE_DREFH_ERR    0x00000003U
#define EFUSE_USB_DEVICE_DREFH_ERR_M  (EFUSE_USB_DEVICE_DREFH_ERR_V << EFUSE_USB_DEVICE_DREFH_ERR_S)
#define EFUSE_USB_DEVICE_DREFH_ERR_V  0x00000003U
#define EFUSE_USB_DEVICE_DREFH_ERR_S  21
/** EFUSE_USB_OTG11_DREFH_ERR : RO; bitpos: [24:23]; default: 0;
 *  Indicates a programming error of USB_OTG11_DREFH.
 */
#define EFUSE_USB_OTG11_DREFH_ERR    0x00000003U
#define EFUSE_USB_OTG11_DREFH_ERR_M  (EFUSE_USB_OTG11_DREFH_ERR_V << EFUSE_USB_OTG11_DREFH_ERR_S)
#define EFUSE_USB_OTG11_DREFH_ERR_V  0x00000003U
#define EFUSE_USB_OTG11_DREFH_ERR_S  23
/** EFUSE_USB_PHY_SEL_ERR : RO; bitpos: [25]; default: 0;
 *  Indicates a programming error of USB_PHY_SEL.
 */
#define EFUSE_USB_PHY_SEL_ERR    (BIT(25))
#define EFUSE_USB_PHY_SEL_ERR_M  (EFUSE_USB_PHY_SEL_ERR_V << EFUSE_USB_PHY_SEL_ERR_S)
#define EFUSE_USB_PHY_SEL_ERR_V  0x00000001U
#define EFUSE_USB_PHY_SEL_ERR_S  25
/** EFUSE_HUK_GEN_STATE_LOW_ERR : RO; bitpos: [31:26]; default: 0;
 *  Indicates a programming error of HUK_GEN_STATE_LOW.
 */
#define EFUSE_HUK_GEN_STATE_LOW_ERR    0x0000003FU
#define EFUSE_HUK_GEN_STATE_LOW_ERR_M  (EFUSE_HUK_GEN_STATE_LOW_ERR_V << EFUSE_HUK_GEN_STATE_LOW_ERR_S)
#define EFUSE_HUK_GEN_STATE_LOW_ERR_V  0x0000003FU
#define EFUSE_HUK_GEN_STATE_LOW_ERR_S  26

/** EFUSE_RD_REPEAT_ERR1_REG register
 *  Programming error record register 1 of BLOCK0.
 */
#define EFUSE_RD_REPEAT_ERR1_REG (DR_REG_EFUSE_BASE + 0x180)
/** EFUSE_KM_HUK_GEN_STATE_HIGH_ERR : RO; bitpos: [2:0]; default: 0;
 *  Indicates a programming error of HUK_GEN_STATE_HIGH.
 */
#define EFUSE_KM_HUK_GEN_STATE_HIGH_ERR    0x00000007U
#define EFUSE_KM_HUK_GEN_STATE_HIGH_ERR_M  (EFUSE_KM_HUK_GEN_STATE_HIGH_ERR_V << EFUSE_KM_HUK_GEN_STATE_HIGH_ERR_S)
#define EFUSE_KM_HUK_GEN_STATE_HIGH_ERR_V  0x00000007U
#define EFUSE_KM_HUK_GEN_STATE_HIGH_ERR_S  0
/** EFUSE_KM_RND_SWITCH_CYCLE_ERR : RO; bitpos: [4:3]; default: 0;
 *  Indicates a programming error of KM_RND_SWITCH_CYCLE.
 */
#define EFUSE_KM_RND_SWITCH_CYCLE_ERR    0x00000003U
#define EFUSE_KM_RND_SWITCH_CYCLE_ERR_M  (EFUSE_KM_RND_SWITCH_CYCLE_ERR_V << EFUSE_KM_RND_SWITCH_CYCLE_ERR_S)
#define EFUSE_KM_RND_SWITCH_CYCLE_ERR_V  0x00000003U
#define EFUSE_KM_RND_SWITCH_CYCLE_ERR_S  3
/** EFUSE_KM_DEPLOY_ONLY_ONCE_ERR : RO; bitpos: [8:5]; default: 0;
 *  Indicates a programming error of KM_DEPLOY_ONLY_ONCE.
 */
#define EFUSE_KM_DEPLOY_ONLY_ONCE_ERR    0x0000000FU
#define EFUSE_KM_DEPLOY_ONLY_ONCE_ERR_M  (EFUSE_KM_DEPLOY_ONLY_ONCE_ERR_V << EFUSE_KM_DEPLOY_ONLY_ONCE_ERR_S)
#define EFUSE_KM_DEPLOY_ONLY_ONCE_ERR_V  0x0000000FU
#define EFUSE_KM_DEPLOY_ONLY_ONCE_ERR_S  5
/** EFUSE_FORCE_USE_KEY_MANAGER_KEY_ERR : RO; bitpos: [12:9]; default: 0;
 *  Indicates a programming error of FORCE_USE_KEY_MANAGER_KEY.
 */
#define EFUSE_FORCE_USE_KEY_MANAGER_KEY_ERR    0x0000000FU
#define EFUSE_FORCE_USE_KEY_MANAGER_KEY_ERR_M  (EFUSE_FORCE_USE_KEY_MANAGER_KEY_ERR_V << EFUSE_FORCE_USE_KEY_MANAGER_KEY_ERR_S)
#define EFUSE_FORCE_USE_KEY_MANAGER_KEY_ERR_V  0x0000000FU
#define EFUSE_FORCE_USE_KEY_MANAGER_KEY_ERR_S  9
/** EFUSE_FORCE_DISABLE_SW_INIT_KEY_ERR : RO; bitpos: [13]; default: 0;
 *  Indicates a programming error of FORCE_DISABLE_SW_INIT_KEY.
 */
#define EFUSE_FORCE_DISABLE_SW_INIT_KEY_ERR    (BIT(13))
#define EFUSE_FORCE_DISABLE_SW_INIT_KEY_ERR_M  (EFUSE_FORCE_DISABLE_SW_INIT_KEY_ERR_V << EFUSE_FORCE_DISABLE_SW_INIT_KEY_ERR_S)
#define EFUSE_FORCE_DISABLE_SW_INIT_KEY_ERR_V  0x00000001U
#define EFUSE_FORCE_DISABLE_SW_INIT_KEY_ERR_S  13
/** EFUSE_XTS_KEY_LENGTH_256_ERR : RO; bitpos: [14]; default: 0;
 *  Indicates a programming error of XTS_KEY_LENGTH_256.
 */
#define EFUSE_XTS_KEY_LENGTH_256_ERR    (BIT(14))
#define EFUSE_XTS_KEY_LENGTH_256_ERR_M  (EFUSE_XTS_KEY_LENGTH_256_ERR_V << EFUSE_XTS_KEY_LENGTH_256_ERR_S)
#define EFUSE_XTS_KEY_LENGTH_256_ERR_V  0x00000001U
#define EFUSE_XTS_KEY_LENGTH_256_ERR_S  14
/** EFUSE_WDT_DELAY_SEL_ERR : RO; bitpos: [17:16]; default: 0;
 *  Indicates a programming error of WDT_DELAY_SEL.
 */
#define EFUSE_WDT_DELAY_SEL_ERR    0x00000003U
#define EFUSE_WDT_DELAY_SEL_ERR_M  (EFUSE_WDT_DELAY_SEL_ERR_V << EFUSE_WDT_DELAY_SEL_ERR_S)
#define EFUSE_WDT_DELAY_SEL_ERR_V  0x00000003U
#define EFUSE_WDT_DELAY_SEL_ERR_S  16
/** EFUSE_SPI_BOOT_CRYPT_CNT_ERR : RO; bitpos: [20:18]; default: 0;
 *  Indicates a programming error of SPI_BOOT_CRYPT_CNT.
 */
#define EFUSE_SPI_BOOT_CRYPT_CNT_ERR    0x00000007U
#define EFUSE_SPI_BOOT_CRYPT_CNT_ERR_M  (EFUSE_SPI_BOOT_CRYPT_CNT_ERR_V << EFUSE_SPI_BOOT_CRYPT_CNT_ERR_S)
#define EFUSE_SPI_BOOT_CRYPT_CNT_ERR_V  0x00000007U
#define EFUSE_SPI_BOOT_CRYPT_CNT_ERR_S  18
/** EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR : RO; bitpos: [21]; default: 0;
 *  Indicates a programming error of SECURE_BOOT_KEY_REVOKE0.
 */
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR    (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR_M  (EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR_V << EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR_S)
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR_V  0x00000001U
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR_S  21
/** EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR : RO; bitpos: [22]; default: 0;
 *  Indicates a programming error of SECURE_BOOT_KEY_REVOKE1.
 */
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR    (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR_M  (EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR_V << EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR_S)
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR_V  0x00000001U
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR_S  22
/** EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR : RO; bitpos: [23]; default: 0;
 *  Indicates a programming error of SECURE_BOOT_KEY_REVOKE2.
 */
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR    (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR_M  (EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR_V << EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR_S)
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR_V  0x00000001U
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR_S  23
/** EFUSE_KEY_PURPOSE_0_ERR : RO; bitpos: [27:24]; default: 0;
 *  Indicates a programming error of KEY_PURPOSE_0.
 */
#define EFUSE_KEY_PURPOSE_0_ERR    0x0000000FU
#define EFUSE_KEY_PURPOSE_0_ERR_M  (EFUSE_KEY_PURPOSE_0_ERR_V << EFUSE_KEY_PURPOSE_0_ERR_S)
#define EFUSE_KEY_PURPOSE_0_ERR_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_0_ERR_S  24
/** EFUSE_KEY_PURPOSE_1_ERR : RO; bitpos: [31:28]; default: 0;
 *  Indicates a programming error of KEY_PURPOSE_1.
 */
#define EFUSE_KEY_PURPOSE_1_ERR    0x0000000FU
#define EFUSE_KEY_PURPOSE_1_ERR_M  (EFUSE_KEY_PURPOSE_1_ERR_V << EFUSE_KEY_PURPOSE_1_ERR_S)
#define EFUSE_KEY_PURPOSE_1_ERR_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_1_ERR_S  28

/** EFUSE_RD_REPEAT_ERR2_REG register
 *  Programming error record register 2 of BLOCK0.
 */
#define EFUSE_RD_REPEAT_ERR2_REG (DR_REG_EFUSE_BASE + 0x184)
/** EFUSE_KEY_PURPOSE_2_ERR : RO; bitpos: [3:0]; default: 0;
 *  Indicates a programming error of KEY_PURPOSE_2.
 */
#define EFUSE_KEY_PURPOSE_2_ERR    0x0000000FU
#define EFUSE_KEY_PURPOSE_2_ERR_M  (EFUSE_KEY_PURPOSE_2_ERR_V << EFUSE_KEY_PURPOSE_2_ERR_S)
#define EFUSE_KEY_PURPOSE_2_ERR_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_2_ERR_S  0
/** EFUSE_KEY_PURPOSE_3_ERR : RO; bitpos: [7:4]; default: 0;
 *  Indicates a programming error of KEY_PURPOSE_3.
 */
#define EFUSE_KEY_PURPOSE_3_ERR    0x0000000FU
#define EFUSE_KEY_PURPOSE_3_ERR_M  (EFUSE_KEY_PURPOSE_3_ERR_V << EFUSE_KEY_PURPOSE_3_ERR_S)
#define EFUSE_KEY_PURPOSE_3_ERR_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_3_ERR_S  4
/** EFUSE_KEY_PURPOSE_4_ERR : RO; bitpos: [11:8]; default: 0;
 *  Indicates a programming error of KEY_PURPOSE_4.
 */
#define EFUSE_KEY_PURPOSE_4_ERR    0x0000000FU
#define EFUSE_KEY_PURPOSE_4_ERR_M  (EFUSE_KEY_PURPOSE_4_ERR_V << EFUSE_KEY_PURPOSE_4_ERR_S)
#define EFUSE_KEY_PURPOSE_4_ERR_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_4_ERR_S  8
/** EFUSE_KEY_PURPOSE_5_ERR : RO; bitpos: [15:12]; default: 0;
 *  Indicates a programming error of KEY_PURPOSE_5.
 */
#define EFUSE_KEY_PURPOSE_5_ERR    0x0000000FU
#define EFUSE_KEY_PURPOSE_5_ERR_M  (EFUSE_KEY_PURPOSE_5_ERR_V << EFUSE_KEY_PURPOSE_5_ERR_S)
#define EFUSE_KEY_PURPOSE_5_ERR_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_5_ERR_S  12
/** EFUSE_SEC_DPA_LEVEL_ERR : RO; bitpos: [17:16]; default: 0;
 *  Indicates a programming error of SEC_DPA_LEVEL.
 */
#define EFUSE_SEC_DPA_LEVEL_ERR    0x00000003U
#define EFUSE_SEC_DPA_LEVEL_ERR_M  (EFUSE_SEC_DPA_LEVEL_ERR_V << EFUSE_SEC_DPA_LEVEL_ERR_S)
#define EFUSE_SEC_DPA_LEVEL_ERR_V  0x00000003U
#define EFUSE_SEC_DPA_LEVEL_ERR_S  16
/** EFUSE_ECDSA_ENABLE_SOFT_K_ERR : RO; bitpos: [18]; default: 0;
 *  Indicates a programming error of ECDSA_FORCE_USE_HARDWARE_K.
 */
#define EFUSE_ECDSA_ENABLE_SOFT_K_ERR    (BIT(18))
#define EFUSE_ECDSA_ENABLE_SOFT_K_ERR_M  (EFUSE_ECDSA_ENABLE_SOFT_K_ERR_V << EFUSE_ECDSA_ENABLE_SOFT_K_ERR_S)
#define EFUSE_ECDSA_ENABLE_SOFT_K_ERR_V  0x00000001U
#define EFUSE_ECDSA_ENABLE_SOFT_K_ERR_S  18
/** EFUSE_CRYPT_DPA_ENABLE_ERR : RO; bitpos: [19]; default: 0;
 *  Indicates a programming error of CRYPT_DPA_ENABLE.
 */
#define EFUSE_CRYPT_DPA_ENABLE_ERR    (BIT(19))
#define EFUSE_CRYPT_DPA_ENABLE_ERR_M  (EFUSE_CRYPT_DPA_ENABLE_ERR_V << EFUSE_CRYPT_DPA_ENABLE_ERR_S)
#define EFUSE_CRYPT_DPA_ENABLE_ERR_V  0x00000001U
#define EFUSE_CRYPT_DPA_ENABLE_ERR_S  19
/** EFUSE_SECURE_BOOT_EN_ERR : RO; bitpos: [20]; default: 0;
 *  Indicates a programming error of SECURE_BOOT_EN.
 */
#define EFUSE_SECURE_BOOT_EN_ERR    (BIT(20))
#define EFUSE_SECURE_BOOT_EN_ERR_M  (EFUSE_SECURE_BOOT_EN_ERR_V << EFUSE_SECURE_BOOT_EN_ERR_S)
#define EFUSE_SECURE_BOOT_EN_ERR_V  0x00000001U
#define EFUSE_SECURE_BOOT_EN_ERR_S  20
/** EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR : RO; bitpos: [21]; default: 0;
 *  Indicates a programming error of SECURE_BOOT_AGGRESSIVE_REVOKE.
 */
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR    (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_M  (EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_V << EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_S)
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_V  0x00000001U
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_S  21
/** EFUSE_FLASH_TYPE_ERR : RO; bitpos: [23]; default: 0;
 *  Indicates a programming error of FLASH_TYPE.
 */
#define EFUSE_FLASH_TYPE_ERR    (BIT(23))
#define EFUSE_FLASH_TYPE_ERR_M  (EFUSE_FLASH_TYPE_ERR_V << EFUSE_FLASH_TYPE_ERR_S)
#define EFUSE_FLASH_TYPE_ERR_V  0x00000001U
#define EFUSE_FLASH_TYPE_ERR_S  23
/** EFUSE_FLASH_PAGE_SIZE_ERR : RO; bitpos: [25:24]; default: 0;
 *  Indicates a programming error of FLASH_PAGE_SIZE.
 */
#define EFUSE_FLASH_PAGE_SIZE_ERR    0x00000003U
#define EFUSE_FLASH_PAGE_SIZE_ERR_M  (EFUSE_FLASH_PAGE_SIZE_ERR_V << EFUSE_FLASH_PAGE_SIZE_ERR_S)
#define EFUSE_FLASH_PAGE_SIZE_ERR_V  0x00000003U
#define EFUSE_FLASH_PAGE_SIZE_ERR_S  24
/** EFUSE_FLASH_ECC_EN_ERR : RO; bitpos: [26]; default: 0;
 *  Indicates a programming error of FLASH_ECC_EN.
 */
#define EFUSE_FLASH_ECC_EN_ERR    (BIT(26))
#define EFUSE_FLASH_ECC_EN_ERR_M  (EFUSE_FLASH_ECC_EN_ERR_V << EFUSE_FLASH_ECC_EN_ERR_S)
#define EFUSE_FLASH_ECC_EN_ERR_V  0x00000001U
#define EFUSE_FLASH_ECC_EN_ERR_S  26
/** EFUSE_DIS_USB_OTG_DOWNLOAD_MODE_ERR : RO; bitpos: [27]; default: 0;
 *  Indicates a programming error of DIS_USB_OTG_DOWNLOAD_MODE.
 */
#define EFUSE_DIS_USB_OTG_DOWNLOAD_MODE_ERR    (BIT(27))
#define EFUSE_DIS_USB_OTG_DOWNLOAD_MODE_ERR_M  (EFUSE_DIS_USB_OTG_DOWNLOAD_MODE_ERR_V << EFUSE_DIS_USB_OTG_DOWNLOAD_MODE_ERR_S)
#define EFUSE_DIS_USB_OTG_DOWNLOAD_MODE_ERR_V  0x00000001U
#define EFUSE_DIS_USB_OTG_DOWNLOAD_MODE_ERR_S  27
/** EFUSE_FLASH_TPUW_ERR : RO; bitpos: [31:28]; default: 0;
 *  Indicates a programming error of FLASH_TPUW.
 */
#define EFUSE_FLASH_TPUW_ERR    0x0000000FU
#define EFUSE_FLASH_TPUW_ERR_M  (EFUSE_FLASH_TPUW_ERR_V << EFUSE_FLASH_TPUW_ERR_S)
#define EFUSE_FLASH_TPUW_ERR_V  0x0000000FU
#define EFUSE_FLASH_TPUW_ERR_S  28

/** EFUSE_RD_REPEAT_ERR3_REG register
 *  Programming error record register 3 of BLOCK0.
 */
#define EFUSE_RD_REPEAT_ERR3_REG (DR_REG_EFUSE_BASE + 0x188)
/** EFUSE_DIS_DOWNLOAD_MODE_ERR : RO; bitpos: [0]; default: 0;
 *  Indicates a programming error of DIS_DOWNLOAD_MODE.
 */
#define EFUSE_DIS_DOWNLOAD_MODE_ERR    (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_ERR_M  (EFUSE_DIS_DOWNLOAD_MODE_ERR_V << EFUSE_DIS_DOWNLOAD_MODE_ERR_S)
#define EFUSE_DIS_DOWNLOAD_MODE_ERR_V  0x00000001U
#define EFUSE_DIS_DOWNLOAD_MODE_ERR_S  0
/** EFUSE_DIS_DIRECT_BOOT_ERR : RO; bitpos: [1]; default: 0;
 *  Indicates a programming error of DIS_DIRECT_BOOT.
 */
#define EFUSE_DIS_DIRECT_BOOT_ERR    (BIT(1))
#define EFUSE_DIS_DIRECT_BOOT_ERR_M  (EFUSE_DIS_DIRECT_BOOT_ERR_V << EFUSE_DIS_DIRECT_BOOT_ERR_S)
#define EFUSE_DIS_DIRECT_BOOT_ERR_V  0x00000001U
#define EFUSE_DIS_DIRECT_BOOT_ERR_S  1
/** EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR : RO; bitpos: [2]; default: 0;
 *  Indicates a programming error of DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR.
 */
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR    (BIT(2))
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR_M  (EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR_V << EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR_S)
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR_V  0x00000001U
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR_S  2
/** EFUSE_LOCK_KM_KEY_ERR : RO; bitpos: [3]; default: 0;
 *  TBD
 */
#define EFUSE_LOCK_KM_KEY_ERR    (BIT(3))
#define EFUSE_LOCK_KM_KEY_ERR_M  (EFUSE_LOCK_KM_KEY_ERR_V << EFUSE_LOCK_KM_KEY_ERR_S)
#define EFUSE_LOCK_KM_KEY_ERR_V  0x00000001U
#define EFUSE_LOCK_KM_KEY_ERR_S  3
/** EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_ERR : RO; bitpos: [4]; default: 0;
 *  Indicates a programming error of DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE.
 */
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_ERR    (BIT(4))
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_ERR_M  (EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_ERR_V << EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_ERR_S)
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_ERR_V  0x00000001U
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_ERR_S  4
/** EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR : RO; bitpos: [5]; default: 0;
 *  Indicates a programming error of ENABLE_SECURITY_DOWNLOAD.
 */
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR    (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_M  (EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_V << EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_S)
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_V  0x00000001U
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_S  5
/** EFUSE_UART_PRINT_CONTROL_ERR : RO; bitpos: [7:6]; default: 0;
 *  Indicates a programming error of UART_PRINT_CONTROL.
 */
#define EFUSE_UART_PRINT_CONTROL_ERR    0x00000003U
#define EFUSE_UART_PRINT_CONTROL_ERR_M  (EFUSE_UART_PRINT_CONTROL_ERR_V << EFUSE_UART_PRINT_CONTROL_ERR_S)
#define EFUSE_UART_PRINT_CONTROL_ERR_V  0x00000003U
#define EFUSE_UART_PRINT_CONTROL_ERR_S  6
/** EFUSE_FORCE_SEND_RESUME_ERR : RO; bitpos: [8]; default: 0;
 *  Indicates a programming error of FORCE_SEND_RESUME.
 */
#define EFUSE_FORCE_SEND_RESUME_ERR    (BIT(8))
#define EFUSE_FORCE_SEND_RESUME_ERR_M  (EFUSE_FORCE_SEND_RESUME_ERR_V << EFUSE_FORCE_SEND_RESUME_ERR_S)
#define EFUSE_FORCE_SEND_RESUME_ERR_V  0x00000001U
#define EFUSE_FORCE_SEND_RESUME_ERR_S  8
/** EFUSE_SECURE_VERSION_ERR : RO; bitpos: [24:9]; default: 0;
 *  Indicates a programming error of SECURE VERSION.
 */
#define EFUSE_SECURE_VERSION_ERR    0x0000FFFFU
#define EFUSE_SECURE_VERSION_ERR_M  (EFUSE_SECURE_VERSION_ERR_V << EFUSE_SECURE_VERSION_ERR_S)
#define EFUSE_SECURE_VERSION_ERR_V  0x0000FFFFU
#define EFUSE_SECURE_VERSION_ERR_S  9
/** EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_ERR : RO; bitpos: [25]; default: 0;
 *  Indicates a programming error of SECURE_BOOT_DISABLE_FAST_WAKE.
 */
#define EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_ERR    (BIT(25))
#define EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_ERR_M  (EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_ERR_V << EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_ERR_S)
#define EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_ERR_V  0x00000001U
#define EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_ERR_S  25
/** EFUSE_HYS_EN_PAD_ERR : RO; bitpos: [26]; default: 0;
 *  Indicates a programming error of HYS_EN_PAD.
 */
#define EFUSE_HYS_EN_PAD_ERR    (BIT(26))
#define EFUSE_HYS_EN_PAD_ERR_M  (EFUSE_HYS_EN_PAD_ERR_V << EFUSE_HYS_EN_PAD_ERR_S)
#define EFUSE_HYS_EN_PAD_ERR_V  0x00000001U
#define EFUSE_HYS_EN_PAD_ERR_S  26
/** EFUSE_DCDC_VSET_ERR : RO; bitpos: [31:27]; default: 0;
 *  Indicates a programming error of DCDC_VSET.
 */
#define EFUSE_DCDC_VSET_ERR    0x0000001FU
#define EFUSE_DCDC_VSET_ERR_M  (EFUSE_DCDC_VSET_ERR_V << EFUSE_DCDC_VSET_ERR_S)
#define EFUSE_DCDC_VSET_ERR_V  0x0000001FU
#define EFUSE_DCDC_VSET_ERR_S  27

/** EFUSE_RD_REPEAT_ERR4_REG register
 *  Programming error record register 4 of BLOCK0.
 */
#define EFUSE_RD_REPEAT_ERR4_REG (DR_REG_EFUSE_BASE + 0x18c)
/** EFUSE_PXA0_TIEH_SEL_0_ERR : RO; bitpos: [1:0]; default: 0;
 *  Indicates a programming error of 0PXA_TIEH_SEL_0.
 */
#define EFUSE_PXA0_TIEH_SEL_0_ERR    0x00000003U
#define EFUSE_PXA0_TIEH_SEL_0_ERR_M  (EFUSE_PXA0_TIEH_SEL_0_ERR_V << EFUSE_PXA0_TIEH_SEL_0_ERR_S)
#define EFUSE_PXA0_TIEH_SEL_0_ERR_V  0x00000003U
#define EFUSE_PXA0_TIEH_SEL_0_ERR_S  0
/** EFUSE_PXA0_TIEH_SEL_1_ERR : RO; bitpos: [3:2]; default: 0;
 *  Indicates a programming error of 0PXA_TIEH_SEL_1.
 */
#define EFUSE_PXA0_TIEH_SEL_1_ERR    0x00000003U
#define EFUSE_PXA0_TIEH_SEL_1_ERR_M  (EFUSE_PXA0_TIEH_SEL_1_ERR_V << EFUSE_PXA0_TIEH_SEL_1_ERR_S)
#define EFUSE_PXA0_TIEH_SEL_1_ERR_V  0x00000003U
#define EFUSE_PXA0_TIEH_SEL_1_ERR_S  2
/** EFUSE_PXA0_TIEH_SEL_2_ERR : RO; bitpos: [5:4]; default: 0;
 *  Indicates a programming error of 0PXA_TIEH_SEL_2.
 */
#define EFUSE_PXA0_TIEH_SEL_2_ERR    0x00000003U
#define EFUSE_PXA0_TIEH_SEL_2_ERR_M  (EFUSE_PXA0_TIEH_SEL_2_ERR_V << EFUSE_PXA0_TIEH_SEL_2_ERR_S)
#define EFUSE_PXA0_TIEH_SEL_2_ERR_V  0x00000003U
#define EFUSE_PXA0_TIEH_SEL_2_ERR_S  4
/** EFUSE_PXA0_TIEH_SEL_3_ERR : RO; bitpos: [7:6]; default: 0;
 *  Indicates a programming error of 0PXA_TIEH_SEL_3.
 */
#define EFUSE_PXA0_TIEH_SEL_3_ERR    0x00000003U
#define EFUSE_PXA0_TIEH_SEL_3_ERR_M  (EFUSE_PXA0_TIEH_SEL_3_ERR_V << EFUSE_PXA0_TIEH_SEL_3_ERR_S)
#define EFUSE_PXA0_TIEH_SEL_3_ERR_V  0x00000003U
#define EFUSE_PXA0_TIEH_SEL_3_ERR_S  6
/** EFUSE_KM_DISABLE_DEPLOY_MODE_ERR : RO; bitpos: [11:8]; default: 0;
 *  TBD.
 */
#define EFUSE_KM_DISABLE_DEPLOY_MODE_ERR    0x0000000FU
#define EFUSE_KM_DISABLE_DEPLOY_MODE_ERR_M  (EFUSE_KM_DISABLE_DEPLOY_MODE_ERR_V << EFUSE_KM_DISABLE_DEPLOY_MODE_ERR_S)
#define EFUSE_KM_DISABLE_DEPLOY_MODE_ERR_V  0x0000000FU
#define EFUSE_KM_DISABLE_DEPLOY_MODE_ERR_S  8
/** EFUSE_USB_DEVICE_DREFL_ERR : RO; bitpos: [13:12]; default: 0;
 *  Indicates a programming error of USB_DEVICE_DREFL.
 */
#define EFUSE_USB_DEVICE_DREFL_ERR    0x00000003U
#define EFUSE_USB_DEVICE_DREFL_ERR_M  (EFUSE_USB_DEVICE_DREFL_ERR_V << EFUSE_USB_DEVICE_DREFL_ERR_S)
#define EFUSE_USB_DEVICE_DREFL_ERR_V  0x00000003U
#define EFUSE_USB_DEVICE_DREFL_ERR_S  12
/** EFUSE_USB_OTG11_DREFL_ERR : RO; bitpos: [15:14]; default: 0;
 *  Indicates a programming error of USB_OTG11_DREFL.
 */
#define EFUSE_USB_OTG11_DREFL_ERR    0x00000003U
#define EFUSE_USB_OTG11_DREFL_ERR_M  (EFUSE_USB_OTG11_DREFL_ERR_V << EFUSE_USB_OTG11_DREFL_ERR_S)
#define EFUSE_USB_OTG11_DREFL_ERR_V  0x00000003U
#define EFUSE_USB_OTG11_DREFL_ERR_S  14
/** EFUSE_HP_PWR_SRC_SEL_ERR : RO; bitpos: [18]; default: 0;
 *  Indicates a programming error of HP_PWR_SRC_SEL.
 */
#define EFUSE_HP_PWR_SRC_SEL_ERR    (BIT(18))
#define EFUSE_HP_PWR_SRC_SEL_ERR_M  (EFUSE_HP_PWR_SRC_SEL_ERR_V << EFUSE_HP_PWR_SRC_SEL_ERR_S)
#define EFUSE_HP_PWR_SRC_SEL_ERR_V  0x00000001U
#define EFUSE_HP_PWR_SRC_SEL_ERR_S  18
/** EFUSE_DCDC_VSET_EN_ERR : RO; bitpos: [19]; default: 0;
 *  Indicates a programming error of DCDC_VSET_EN.
 */
#define EFUSE_DCDC_VSET_EN_ERR    (BIT(19))
#define EFUSE_DCDC_VSET_EN_ERR_M  (EFUSE_DCDC_VSET_EN_ERR_V << EFUSE_DCDC_VSET_EN_ERR_S)
#define EFUSE_DCDC_VSET_EN_ERR_V  0x00000001U
#define EFUSE_DCDC_VSET_EN_ERR_S  19
/** EFUSE_DIS_WDT_ERR : RO; bitpos: [20]; default: 0;
 *  Indicates a programming error of DIS_WDT.
 */
#define EFUSE_DIS_WDT_ERR    (BIT(20))
#define EFUSE_DIS_WDT_ERR_M  (EFUSE_DIS_WDT_ERR_V << EFUSE_DIS_WDT_ERR_S)
#define EFUSE_DIS_WDT_ERR_V  0x00000001U
#define EFUSE_DIS_WDT_ERR_S  20
/** EFUSE_DIS_SWD_ERR : RO; bitpos: [21]; default: 0;
 *  Indicates a programming error of DIS_SWD.
 */
#define EFUSE_DIS_SWD_ERR    (BIT(21))
#define EFUSE_DIS_SWD_ERR_M  (EFUSE_DIS_SWD_ERR_V << EFUSE_DIS_SWD_ERR_S)
#define EFUSE_DIS_SWD_ERR_V  0x00000001U
#define EFUSE_DIS_SWD_ERR_S  21

/** EFUSE_RD_RS_ERR0_REG register
 *  Programming error record register 0 of BLOCK1-10.
 */
#define EFUSE_RD_RS_ERR0_REG (DR_REG_EFUSE_BASE + 0x1c0)
/** EFUSE_MAC_SYS_ERR_NUM : RO; bitpos: [2:0]; default: 0;
 *  The value of this signal means the number of error bytes.
 */
#define EFUSE_MAC_SYS_ERR_NUM    0x00000007U
#define EFUSE_MAC_SYS_ERR_NUM_M  (EFUSE_MAC_SYS_ERR_NUM_V << EFUSE_MAC_SYS_ERR_NUM_S)
#define EFUSE_MAC_SYS_ERR_NUM_V  0x00000007U
#define EFUSE_MAC_SYS_ERR_NUM_S  0
/** EFUSE_MAC_SYS_FAIL : RO; bitpos: [3]; default: 0;
 *  0: Means no failure and that the data of MAC_SPI_8M is reliable 1: Means that
 *  programming user data failed and the number of error bytes is over 6.
 */
#define EFUSE_MAC_SYS_FAIL    (BIT(3))
#define EFUSE_MAC_SYS_FAIL_M  (EFUSE_MAC_SYS_FAIL_V << EFUSE_MAC_SYS_FAIL_S)
#define EFUSE_MAC_SYS_FAIL_V  0x00000001U
#define EFUSE_MAC_SYS_FAIL_S  3
/** EFUSE_SYS_PART1_ERR_NUM : RO; bitpos: [6:4]; default: 0;
 *  The value of this signal means the number of error bytes.
 */
#define EFUSE_SYS_PART1_ERR_NUM    0x00000007U
#define EFUSE_SYS_PART1_ERR_NUM_M  (EFUSE_SYS_PART1_ERR_NUM_V << EFUSE_SYS_PART1_ERR_NUM_S)
#define EFUSE_SYS_PART1_ERR_NUM_V  0x00000007U
#define EFUSE_SYS_PART1_ERR_NUM_S  4
/** EFUSE_SYS_PART1_FAIL : RO; bitpos: [7]; default: 0;
 *  0: Means no failure and that the data of system part1 is reliable 1: Means that
 *  programming user data failed and the number of error bytes is over 6.
 */
#define EFUSE_SYS_PART1_FAIL    (BIT(7))
#define EFUSE_SYS_PART1_FAIL_M  (EFUSE_SYS_PART1_FAIL_V << EFUSE_SYS_PART1_FAIL_S)
#define EFUSE_SYS_PART1_FAIL_V  0x00000001U
#define EFUSE_SYS_PART1_FAIL_S  7
/** EFUSE_USR_DATA_ERR_NUM : RO; bitpos: [10:8]; default: 0;
 *  The value of this signal means the number of error bytes.
 */
#define EFUSE_USR_DATA_ERR_NUM    0x00000007U
#define EFUSE_USR_DATA_ERR_NUM_M  (EFUSE_USR_DATA_ERR_NUM_V << EFUSE_USR_DATA_ERR_NUM_S)
#define EFUSE_USR_DATA_ERR_NUM_V  0x00000007U
#define EFUSE_USR_DATA_ERR_NUM_S  8
/** EFUSE_USR_DATA_FAIL : RO; bitpos: [11]; default: 0;
 *  0: Means no failure and that the user data is reliable 1: Means that programming
 *  user data failed and the number of error bytes is over 6.
 */
#define EFUSE_USR_DATA_FAIL    (BIT(11))
#define EFUSE_USR_DATA_FAIL_M  (EFUSE_USR_DATA_FAIL_V << EFUSE_USR_DATA_FAIL_S)
#define EFUSE_USR_DATA_FAIL_V  0x00000001U
#define EFUSE_USR_DATA_FAIL_S  11
/** EFUSE_KEY0_ERR_NUM : RO; bitpos: [14:12]; default: 0;
 *  The value of this signal means the number of error bytes.
 */
#define EFUSE_KEY0_ERR_NUM    0x00000007U
#define EFUSE_KEY0_ERR_NUM_M  (EFUSE_KEY0_ERR_NUM_V << EFUSE_KEY0_ERR_NUM_S)
#define EFUSE_KEY0_ERR_NUM_V  0x00000007U
#define EFUSE_KEY0_ERR_NUM_S  12
/** EFUSE_KEY0_FAIL : RO; bitpos: [15]; default: 0;
 *  0: Means no failure and that the data of key0 is reliable 1: Means that programming
 *  key0 failed and the number of error bytes is over 6.
 */
#define EFUSE_KEY0_FAIL    (BIT(15))
#define EFUSE_KEY0_FAIL_M  (EFUSE_KEY0_FAIL_V << EFUSE_KEY0_FAIL_S)
#define EFUSE_KEY0_FAIL_V  0x00000001U
#define EFUSE_KEY0_FAIL_S  15
/** EFUSE_KEY1_ERR_NUM : RO; bitpos: [18:16]; default: 0;
 *  The value of this signal means the number of error bytes.
 */
#define EFUSE_KEY1_ERR_NUM    0x00000007U
#define EFUSE_KEY1_ERR_NUM_M  (EFUSE_KEY1_ERR_NUM_V << EFUSE_KEY1_ERR_NUM_S)
#define EFUSE_KEY1_ERR_NUM_V  0x00000007U
#define EFUSE_KEY1_ERR_NUM_S  16
/** EFUSE_KEY1_FAIL : RO; bitpos: [19]; default: 0;
 *  0: Means no failure and that the data of key1 is reliable 1: Means that programming
 *  key1 failed and the number of error bytes is over 6.
 */
#define EFUSE_KEY1_FAIL    (BIT(19))
#define EFUSE_KEY1_FAIL_M  (EFUSE_KEY1_FAIL_V << EFUSE_KEY1_FAIL_S)
#define EFUSE_KEY1_FAIL_V  0x00000001U
#define EFUSE_KEY1_FAIL_S  19
/** EFUSE_KEY2_ERR_NUM : RO; bitpos: [22:20]; default: 0;
 *  The value of this signal means the number of error bytes.
 */
#define EFUSE_KEY2_ERR_NUM    0x00000007U
#define EFUSE_KEY2_ERR_NUM_M  (EFUSE_KEY2_ERR_NUM_V << EFUSE_KEY2_ERR_NUM_S)
#define EFUSE_KEY2_ERR_NUM_V  0x00000007U
#define EFUSE_KEY2_ERR_NUM_S  20
/** EFUSE_KEY2_FAIL : RO; bitpos: [23]; default: 0;
 *  0: Means no failure and that the data of key2 is reliable 1: Means that programming
 *  key2 failed and the number of error bytes is over 6.
 */
#define EFUSE_KEY2_FAIL    (BIT(23))
#define EFUSE_KEY2_FAIL_M  (EFUSE_KEY2_FAIL_V << EFUSE_KEY2_FAIL_S)
#define EFUSE_KEY2_FAIL_V  0x00000001U
#define EFUSE_KEY2_FAIL_S  23
/** EFUSE_KEY3_ERR_NUM : RO; bitpos: [26:24]; default: 0;
 *  The value of this signal means the number of error bytes.
 */
#define EFUSE_KEY3_ERR_NUM    0x00000007U
#define EFUSE_KEY3_ERR_NUM_M  (EFUSE_KEY3_ERR_NUM_V << EFUSE_KEY3_ERR_NUM_S)
#define EFUSE_KEY3_ERR_NUM_V  0x00000007U
#define EFUSE_KEY3_ERR_NUM_S  24
/** EFUSE_KEY3_FAIL : RO; bitpos: [27]; default: 0;
 *  0: Means no failure and that the data of key3 is reliable 1: Means that programming
 *  key3 failed and the number of error bytes is over 6.
 */
#define EFUSE_KEY3_FAIL    (BIT(27))
#define EFUSE_KEY3_FAIL_M  (EFUSE_KEY3_FAIL_V << EFUSE_KEY3_FAIL_S)
#define EFUSE_KEY3_FAIL_V  0x00000001U
#define EFUSE_KEY3_FAIL_S  27
/** EFUSE_KEY4_ERR_NUM : RO; bitpos: [30:28]; default: 0;
 *  The value of this signal means the number of error bytes.
 */
#define EFUSE_KEY4_ERR_NUM    0x00000007U
#define EFUSE_KEY4_ERR_NUM_M  (EFUSE_KEY4_ERR_NUM_V << EFUSE_KEY4_ERR_NUM_S)
#define EFUSE_KEY4_ERR_NUM_V  0x00000007U
#define EFUSE_KEY4_ERR_NUM_S  28
/** EFUSE_KEY4_FAIL : RO; bitpos: [31]; default: 0;
 *  0: Means no failure and that the data of key4 is reliable 1: Means that programming
 *  key4 failed and the number of error bytes is over 6.
 */
#define EFUSE_KEY4_FAIL    (BIT(31))
#define EFUSE_KEY4_FAIL_M  (EFUSE_KEY4_FAIL_V << EFUSE_KEY4_FAIL_S)
#define EFUSE_KEY4_FAIL_V  0x00000001U
#define EFUSE_KEY4_FAIL_S  31

/** EFUSE_RD_RS_ERR1_REG register
 *  Programming error record register 1 of BLOCK1-10.
 */
#define EFUSE_RD_RS_ERR1_REG (DR_REG_EFUSE_BASE + 0x1c4)
/** EFUSE_KEY5_ERR_NUM : RO; bitpos: [2:0]; default: 0;
 *  The value of this signal means the number of error bytes.
 */
#define EFUSE_KEY5_ERR_NUM    0x00000007U
#define EFUSE_KEY5_ERR_NUM_M  (EFUSE_KEY5_ERR_NUM_V << EFUSE_KEY5_ERR_NUM_S)
#define EFUSE_KEY5_ERR_NUM_V  0x00000007U
#define EFUSE_KEY5_ERR_NUM_S  0
/** EFUSE_KEY5_FAIL : RO; bitpos: [3]; default: 0;
 *  0: Means no failure and that the data of key5 is reliable 1: Means that programming
 *  key5 failed and the number of error bytes is over 6.
 */
#define EFUSE_KEY5_FAIL    (BIT(3))
#define EFUSE_KEY5_FAIL_M  (EFUSE_KEY5_FAIL_V << EFUSE_KEY5_FAIL_S)
#define EFUSE_KEY5_FAIL_V  0x00000001U
#define EFUSE_KEY5_FAIL_S  3
/** EFUSE_SYS_PART2_ERR_NUM : RO; bitpos: [6:4]; default: 0;
 *  The value of this signal means the number of error bytes.
 */
#define EFUSE_SYS_PART2_ERR_NUM    0x00000007U
#define EFUSE_SYS_PART2_ERR_NUM_M  (EFUSE_SYS_PART2_ERR_NUM_V << EFUSE_SYS_PART2_ERR_NUM_S)
#define EFUSE_SYS_PART2_ERR_NUM_V  0x00000007U
#define EFUSE_SYS_PART2_ERR_NUM_S  4
/** EFUSE_SYS_PART2_FAIL : RO; bitpos: [7]; default: 0;
 *  0: Means no failure and that the data of system part2 is reliable 1: Means that
 *  programming user data failed and the number of error bytes is over 6.
 */
#define EFUSE_SYS_PART2_FAIL    (BIT(7))
#define EFUSE_SYS_PART2_FAIL_M  (EFUSE_SYS_PART2_FAIL_V << EFUSE_SYS_PART2_FAIL_S)
#define EFUSE_SYS_PART2_FAIL_V  0x00000001U
#define EFUSE_SYS_PART2_FAIL_S  7

/** EFUSE_CLK_REG register
 *  eFuse clcok configuration register.
 */
#define EFUSE_CLK_REG (DR_REG_EFUSE_BASE + 0x1c8)
/** EFUSE_MEM_FORCE_PD : R/W; bitpos: [0]; default: 0;
 *  Set this bit to force eFuse SRAM into power-saving mode.
 */
#define EFUSE_MEM_FORCE_PD    (BIT(0))
#define EFUSE_MEM_FORCE_PD_M  (EFUSE_MEM_FORCE_PD_V << EFUSE_MEM_FORCE_PD_S)
#define EFUSE_MEM_FORCE_PD_V  0x00000001U
#define EFUSE_MEM_FORCE_PD_S  0
/** EFUSE_MEM_CLK_FORCE_ON : R/W; bitpos: [1]; default: 0;
 *  Set this bit and force to activate clock signal of eFuse SRAM.
 */
#define EFUSE_MEM_CLK_FORCE_ON    (BIT(1))
#define EFUSE_MEM_CLK_FORCE_ON_M  (EFUSE_MEM_CLK_FORCE_ON_V << EFUSE_MEM_CLK_FORCE_ON_S)
#define EFUSE_MEM_CLK_FORCE_ON_V  0x00000001U
#define EFUSE_MEM_CLK_FORCE_ON_S  1
/** EFUSE_MEM_FORCE_PU : R/W; bitpos: [2]; default: 0;
 *  Set this bit to force eFuse SRAM into working mode.
 */
#define EFUSE_MEM_FORCE_PU    (BIT(2))
#define EFUSE_MEM_FORCE_PU_M  (EFUSE_MEM_FORCE_PU_V << EFUSE_MEM_FORCE_PU_S)
#define EFUSE_MEM_FORCE_PU_V  0x00000001U
#define EFUSE_MEM_FORCE_PU_S  2
/** EFUSE_CLK_EN : R/W; bitpos: [16]; default: 0;
 *  Set this bit to force enable eFuse register configuration clock signal.
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
 *  0x5A5A:  programming operation command 0x5AA5: read operation command.
 */
#define EFUSE_OP_CODE    0x0000FFFFU
#define EFUSE_OP_CODE_M  (EFUSE_OP_CODE_V << EFUSE_OP_CODE_S)
#define EFUSE_OP_CODE_V  0x0000FFFFU
#define EFUSE_OP_CODE_S  0
/** EFUSE_CFG_ECDSA_BLK : R/W; bitpos: [19:16]; default: 0;
 *  Configures which block to use for ECDSA key output.
 */
#define EFUSE_CFG_ECDSA_BLK    0x0000000FU
#define EFUSE_CFG_ECDSA_BLK_M  (EFUSE_CFG_ECDSA_BLK_V << EFUSE_CFG_ECDSA_BLK_S)
#define EFUSE_CFG_ECDSA_BLK_V  0x0000000FU
#define EFUSE_CFG_ECDSA_BLK_S  16

/** EFUSE_STATUS_REG register
 *  eFuse status register.
 */
#define EFUSE_STATUS_REG (DR_REG_EFUSE_BASE + 0x1d0)
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
/** EFUSE_BLK0_VALID_BIT_CNT : RO; bitpos: [19:10]; default: 0;
 *  Indicates the number of block valid bit.
 */
#define EFUSE_BLK0_VALID_BIT_CNT    0x000003FFU
#define EFUSE_BLK0_VALID_BIT_CNT_M  (EFUSE_BLK0_VALID_BIT_CNT_V << EFUSE_BLK0_VALID_BIT_CNT_S)
#define EFUSE_BLK0_VALID_BIT_CNT_V  0x000003FFU
#define EFUSE_BLK0_VALID_BIT_CNT_S  10
/** EFUSE_CUR_ECDSA_BLK : RO; bitpos: [23:20]; default: 0;
 *  Indicates which block is used for ECDSA key output.
 */
#define EFUSE_CUR_ECDSA_BLK    0x0000000FU
#define EFUSE_CUR_ECDSA_BLK_M  (EFUSE_CUR_ECDSA_BLK_V << EFUSE_CUR_ECDSA_BLK_S)
#define EFUSE_CUR_ECDSA_BLK_V  0x0000000FU
#define EFUSE_CUR_ECDSA_BLK_S  20

/** EFUSE_CMD_REG register
 *  eFuse command register.
 */
#define EFUSE_CMD_REG (DR_REG_EFUSE_BASE + 0x1d4)
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
/** EFUSE_BLK_NUM : R/W; bitpos: [5:2]; default: 0;
 *  The serial number of the block to be programmed. Value 0-10 corresponds to block
 *  number 0-10, respectively.
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
 *  The raw bit signal for read_done interrupt.
 */
#define EFUSE_READ_DONE_INT_RAW    (BIT(0))
#define EFUSE_READ_DONE_INT_RAW_M  (EFUSE_READ_DONE_INT_RAW_V << EFUSE_READ_DONE_INT_RAW_S)
#define EFUSE_READ_DONE_INT_RAW_V  0x00000001U
#define EFUSE_READ_DONE_INT_RAW_S  0
/** EFUSE_PGM_DONE_INT_RAW : R/SS/WTC; bitpos: [1]; default: 0;
 *  The raw bit signal for pgm_done interrupt.
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
#define EFUSE_INT_ENA_REG (DR_REG_EFUSE_BASE + 0x1e0)
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
#define EFUSE_INT_CLR_REG (DR_REG_EFUSE_BASE + 0x1e4)
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
#define EFUSE_DAC_CONF_REG (DR_REG_EFUSE_BASE + 0x1e8)
/** EFUSE_DAC_CLK_DIV : R/W; bitpos: [7:0]; default: 23;
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
#define EFUSE_RD_TIM_CONF_REG (DR_REG_EFUSE_BASE + 0x1ec)
/** EFUSE_THR_A : R/W; bitpos: [7:0]; default: 1;
 *  Configures the read hold time.
 */
#define EFUSE_THR_A    0x000000FFU
#define EFUSE_THR_A_M  (EFUSE_THR_A_V << EFUSE_THR_A_S)
#define EFUSE_THR_A_V  0x000000FFU
#define EFUSE_THR_A_S  0
/** EFUSE_TRD : R/W; bitpos: [15:8]; default: 2;
 *  Configures the read time.
 */
#define EFUSE_TRD    0x000000FFU
#define EFUSE_TRD_M  (EFUSE_TRD_V << EFUSE_TRD_S)
#define EFUSE_TRD_V  0x000000FFU
#define EFUSE_TRD_S  8
/** EFUSE_TSUR_A : R/W; bitpos: [23:16]; default: 1;
 *  Configures the read setup time.
 */
#define EFUSE_TSUR_A    0x000000FFU
#define EFUSE_TSUR_A_M  (EFUSE_TSUR_A_V << EFUSE_TSUR_A_S)
#define EFUSE_TSUR_A_V  0x000000FFU
#define EFUSE_TSUR_A_S  16
/** EFUSE_READ_INIT_NUM : R/W; bitpos: [31:24]; default: 15;
 *  Configures the waiting time of reading eFuse memory.
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
 *  Configures the programming setup time.
 */
#define EFUSE_TSUP_A    0x000000FFU
#define EFUSE_TSUP_A_M  (EFUSE_TSUP_A_V << EFUSE_TSUP_A_S)
#define EFUSE_TSUP_A_V  0x000000FFU
#define EFUSE_TSUP_A_S  0
/** EFUSE_PWR_ON_NUM : R/W; bitpos: [23:8]; default: 9831;
 *  Configures the power up time for VDDQ.
 */
#define EFUSE_PWR_ON_NUM    0x0000FFFFU
#define EFUSE_PWR_ON_NUM_M  (EFUSE_PWR_ON_NUM_V << EFUSE_PWR_ON_NUM_S)
#define EFUSE_PWR_ON_NUM_V  0x0000FFFFU
#define EFUSE_PWR_ON_NUM_S  8
/** EFUSE_THP_A : R/W; bitpos: [31:24]; default: 1;
 *  Configures the programming hold time.
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
 *  Configures the power outage time for VDDQ.
 */
#define EFUSE_PWR_OFF_NUM    0x0000FFFFU
#define EFUSE_PWR_OFF_NUM_M  (EFUSE_PWR_OFF_NUM_V << EFUSE_PWR_OFF_NUM_S)
#define EFUSE_PWR_OFF_NUM_V  0x0000FFFFU
#define EFUSE_PWR_OFF_NUM_S  0
/** EFUSE_TPGM : R/W; bitpos: [31:16]; default: 160;
 *  Configures the active programming time.
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
 *  Set this bit to bypass reed solomon correction step.
 */
#define EFUSE_BYPASS_RS_CORRECTION    (BIT(0))
#define EFUSE_BYPASS_RS_CORRECTION_M  (EFUSE_BYPASS_RS_CORRECTION_V << EFUSE_BYPASS_RS_CORRECTION_S)
#define EFUSE_BYPASS_RS_CORRECTION_V  0x00000001U
#define EFUSE_BYPASS_RS_CORRECTION_S  0
/** EFUSE_BYPASS_RS_BLK_NUM : R/W; bitpos: [11:1]; default: 0;
 *  Configures block number of programming twice operation.
 */
#define EFUSE_BYPASS_RS_BLK_NUM    0x000007FFU
#define EFUSE_BYPASS_RS_BLK_NUM_M  (EFUSE_BYPASS_RS_BLK_NUM_V << EFUSE_BYPASS_RS_BLK_NUM_S)
#define EFUSE_BYPASS_RS_BLK_NUM_V  0x000007FFU
#define EFUSE_BYPASS_RS_BLK_NUM_S  1
/** EFUSE_UPDATE : WT; bitpos: [12]; default: 0;
 *  Set this bit to update multi-bit register signals.
 */
#define EFUSE_UPDATE    (BIT(12))
#define EFUSE_UPDATE_M  (EFUSE_UPDATE_V << EFUSE_UPDATE_S)
#define EFUSE_UPDATE_V  0x00000001U
#define EFUSE_UPDATE_S  12
/** EFUSE_TPGM_INACTIVE : R/W; bitpos: [20:13]; default: 1;
 *  Configures the inactive programming time.
 */
#define EFUSE_TPGM_INACTIVE    0x000000FFU
#define EFUSE_TPGM_INACTIVE_M  (EFUSE_TPGM_INACTIVE_V << EFUSE_TPGM_INACTIVE_S)
#define EFUSE_TPGM_INACTIVE_V  0x000000FFU
#define EFUSE_TPGM_INACTIVE_S  13

/** EFUSE_DATE_REG register
 *  eFuse version register.
 */
#define EFUSE_DATE_REG (DR_REG_EFUSE_BASE + 0x1fc)
/** EFUSE_DATE : R/W; bitpos: [27:0]; default: 36720720;
 *  Stores eFuse version.
 */
#define EFUSE_DATE    0x0FFFFFFFU
#define EFUSE_DATE_M  (EFUSE_DATE_V << EFUSE_DATE_S)
#define EFUSE_DATE_V  0x0FFFFFFFU
#define EFUSE_DATE_S  0

/** EFUSE_APB2OTP_WR_DIS_REG register
 *  eFuse apb2otp block0 data register1.
 */
#define EFUSE_APB2OTP_WR_DIS_REG (DR_REG_EFUSE_BASE + 0x800)
/** EFUSE_APB2OTP_BLOCK0_WR_DIS : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 write disable data.
 */
#define EFUSE_APB2OTP_BLOCK0_WR_DIS    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_WR_DIS_M  (EFUSE_APB2OTP_BLOCK0_WR_DIS_V << EFUSE_APB2OTP_BLOCK0_WR_DIS_S)
#define EFUSE_APB2OTP_BLOCK0_WR_DIS_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_WR_DIS_S  0

/** EFUSE_APB2OTP_BLK0_BACKUP1_W1_REG register
 *  eFuse apb2otp block0 data register2.
 */
#define EFUSE_APB2OTP_BLK0_BACKUP1_W1_REG (DR_REG_EFUSE_BASE + 0x804)
/** EFUSE_APB2OTP_BLOCK0_BACKUP1_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup1 word1 data.
 */
#define EFUSE_APB2OTP_BLOCK0_BACKUP1_W1    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP1_W1_M  (EFUSE_APB2OTP_BLOCK0_BACKUP1_W1_V << EFUSE_APB2OTP_BLOCK0_BACKUP1_W1_S)
#define EFUSE_APB2OTP_BLOCK0_BACKUP1_W1_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP1_W1_S  0

/** EFUSE_APB2OTP_BLK0_BACKUP1_W2_REG register
 *  eFuse apb2otp block0 data register3.
 */
#define EFUSE_APB2OTP_BLK0_BACKUP1_W2_REG (DR_REG_EFUSE_BASE + 0x808)
/** EFUSE_APB2OTP_BLOCK0_BACKUP1_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup1 word2 data.
 */
#define EFUSE_APB2OTP_BLOCK0_BACKUP1_W2    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP1_W2_M  (EFUSE_APB2OTP_BLOCK0_BACKUP1_W2_V << EFUSE_APB2OTP_BLOCK0_BACKUP1_W2_S)
#define EFUSE_APB2OTP_BLOCK0_BACKUP1_W2_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP1_W2_S  0

/** EFUSE_APB2OTP_BLK0_BACKUP1_W3_REG register
 *  eFuse apb2otp block0 data register4.
 */
#define EFUSE_APB2OTP_BLK0_BACKUP1_W3_REG (DR_REG_EFUSE_BASE + 0x80c)
/** EFUSE_APB2OTP_BLOCK0_BACKUP1_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup1 word3 data.
 */
#define EFUSE_APB2OTP_BLOCK0_BACKUP1_W3    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP1_W3_M  (EFUSE_APB2OTP_BLOCK0_BACKUP1_W3_V << EFUSE_APB2OTP_BLOCK0_BACKUP1_W3_S)
#define EFUSE_APB2OTP_BLOCK0_BACKUP1_W3_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP1_W3_S  0

/** EFUSE_APB2OTP_BLK0_BACKUP1_W4_REG register
 *  eFuse apb2otp block0 data register5.
 */
#define EFUSE_APB2OTP_BLK0_BACKUP1_W4_REG (DR_REG_EFUSE_BASE + 0x810)
/** EFUSE_APB2OTP_BLOCK0_BACKUP1_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup1 word4 data.
 */
#define EFUSE_APB2OTP_BLOCK0_BACKUP1_W4    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP1_W4_M  (EFUSE_APB2OTP_BLOCK0_BACKUP1_W4_V << EFUSE_APB2OTP_BLOCK0_BACKUP1_W4_S)
#define EFUSE_APB2OTP_BLOCK0_BACKUP1_W4_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP1_W4_S  0

/** EFUSE_APB2OTP_BLK0_BACKUP1_W5_REG register
 *  eFuse apb2otp block0 data register6.
 */
#define EFUSE_APB2OTP_BLK0_BACKUP1_W5_REG (DR_REG_EFUSE_BASE + 0x814)
/** EFUSE_APB2OTP_BLOCK0_BACKUP1_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup1 word5 data.
 */
#define EFUSE_APB2OTP_BLOCK0_BACKUP1_W5    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP1_W5_M  (EFUSE_APB2OTP_BLOCK0_BACKUP1_W5_V << EFUSE_APB2OTP_BLOCK0_BACKUP1_W5_S)
#define EFUSE_APB2OTP_BLOCK0_BACKUP1_W5_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP1_W5_S  0

/** EFUSE_APB2OTP_BLK0_BACKUP2_W1_REG register
 *  eFuse apb2otp block0 data register7.
 */
#define EFUSE_APB2OTP_BLK0_BACKUP2_W1_REG (DR_REG_EFUSE_BASE + 0x818)
/** EFUSE_APB2OTP_BLOCK0_BACKUP2_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup2 word1 data.
 */
#define EFUSE_APB2OTP_BLOCK0_BACKUP2_W1    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP2_W1_M  (EFUSE_APB2OTP_BLOCK0_BACKUP2_W1_V << EFUSE_APB2OTP_BLOCK0_BACKUP2_W1_S)
#define EFUSE_APB2OTP_BLOCK0_BACKUP2_W1_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP2_W1_S  0

/** EFUSE_APB2OTP_BLK0_BACKUP2_W2_REG register
 *  eFuse apb2otp block0 data register8.
 */
#define EFUSE_APB2OTP_BLK0_BACKUP2_W2_REG (DR_REG_EFUSE_BASE + 0x81c)
/** EFUSE_APB2OTP_BLOCK0_BACKUP2_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup2 word2 data.
 */
#define EFUSE_APB2OTP_BLOCK0_BACKUP2_W2    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP2_W2_M  (EFUSE_APB2OTP_BLOCK0_BACKUP2_W2_V << EFUSE_APB2OTP_BLOCK0_BACKUP2_W2_S)
#define EFUSE_APB2OTP_BLOCK0_BACKUP2_W2_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP2_W2_S  0

/** EFUSE_APB2OTP_BLK0_BACKUP2_W3_REG register
 *  eFuse apb2otp block0 data register9.
 */
#define EFUSE_APB2OTP_BLK0_BACKUP2_W3_REG (DR_REG_EFUSE_BASE + 0x820)
/** EFUSE_APB2OTP_BLOCK0_BACKUP2_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup2 word3 data.
 */
#define EFUSE_APB2OTP_BLOCK0_BACKUP2_W3    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP2_W3_M  (EFUSE_APB2OTP_BLOCK0_BACKUP2_W3_V << EFUSE_APB2OTP_BLOCK0_BACKUP2_W3_S)
#define EFUSE_APB2OTP_BLOCK0_BACKUP2_W3_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP2_W3_S  0

/** EFUSE_APB2OTP_BLK0_BACKUP2_W4_REG register
 *  eFuse apb2otp block0 data register10.
 */
#define EFUSE_APB2OTP_BLK0_BACKUP2_W4_REG (DR_REG_EFUSE_BASE + 0x824)
/** EFUSE_APB2OTP_BLOCK0_BACKUP2_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup2 word4 data.
 */
#define EFUSE_APB2OTP_BLOCK0_BACKUP2_W4    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP2_W4_M  (EFUSE_APB2OTP_BLOCK0_BACKUP2_W4_V << EFUSE_APB2OTP_BLOCK0_BACKUP2_W4_S)
#define EFUSE_APB2OTP_BLOCK0_BACKUP2_W4_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP2_W4_S  0

/** EFUSE_APB2OTP_BLK0_BACKUP2_W5_REG register
 *  eFuse apb2otp block0 data register11.
 */
#define EFUSE_APB2OTP_BLK0_BACKUP2_W5_REG (DR_REG_EFUSE_BASE + 0x828)
/** EFUSE_APB2OTP_BLOCK0_BACKUP2_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup2 word5 data.
 */
#define EFUSE_APB2OTP_BLOCK0_BACKUP2_W5    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP2_W5_M  (EFUSE_APB2OTP_BLOCK0_BACKUP2_W5_V << EFUSE_APB2OTP_BLOCK0_BACKUP2_W5_S)
#define EFUSE_APB2OTP_BLOCK0_BACKUP2_W5_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP2_W5_S  0

/** EFUSE_APB2OTP_BLK0_BACKUP3_W1_REG register
 *  eFuse apb2otp block0 data register12.
 */
#define EFUSE_APB2OTP_BLK0_BACKUP3_W1_REG (DR_REG_EFUSE_BASE + 0x82c)
/** EFUSE_APB2OTP_BLOCK0_BACKUP3_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup3 word1 data.
 */
#define EFUSE_APB2OTP_BLOCK0_BACKUP3_W1    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP3_W1_M  (EFUSE_APB2OTP_BLOCK0_BACKUP3_W1_V << EFUSE_APB2OTP_BLOCK0_BACKUP3_W1_S)
#define EFUSE_APB2OTP_BLOCK0_BACKUP3_W1_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP3_W1_S  0

/** EFUSE_APB2OTP_BLK0_BACKUP3_W2_REG register
 *  eFuse apb2otp block0 data register13.
 */
#define EFUSE_APB2OTP_BLK0_BACKUP3_W2_REG (DR_REG_EFUSE_BASE + 0x830)
/** EFUSE_APB2OTP_BLOCK0_BACKUP3_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup3 word2 data.
 */
#define EFUSE_APB2OTP_BLOCK0_BACKUP3_W2    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP3_W2_M  (EFUSE_APB2OTP_BLOCK0_BACKUP3_W2_V << EFUSE_APB2OTP_BLOCK0_BACKUP3_W2_S)
#define EFUSE_APB2OTP_BLOCK0_BACKUP3_W2_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP3_W2_S  0

/** EFUSE_APB2OTP_BLK0_BACKUP3_W3_REG register
 *  eFuse apb2otp block0 data register14.
 */
#define EFUSE_APB2OTP_BLK0_BACKUP3_W3_REG (DR_REG_EFUSE_BASE + 0x834)
/** EFUSE_APB2OTP_BLOCK0_BACKUP3_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup3 word3 data.
 */
#define EFUSE_APB2OTP_BLOCK0_BACKUP3_W3    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP3_W3_M  (EFUSE_APB2OTP_BLOCK0_BACKUP3_W3_V << EFUSE_APB2OTP_BLOCK0_BACKUP3_W3_S)
#define EFUSE_APB2OTP_BLOCK0_BACKUP3_W3_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP3_W3_S  0

/** EFUSE_APB2OTP_BLK0_BACKUP3_W4_REG register
 *  eFuse apb2otp block0 data register15.
 */
#define EFUSE_APB2OTP_BLK0_BACKUP3_W4_REG (DR_REG_EFUSE_BASE + 0x838)
/** EFUSE_APB2OTP_BLOCK0_BACKUP3_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup3 word4 data.
 */
#define EFUSE_APB2OTP_BLOCK0_BACKUP3_W4    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP3_W4_M  (EFUSE_APB2OTP_BLOCK0_BACKUP3_W4_V << EFUSE_APB2OTP_BLOCK0_BACKUP3_W4_S)
#define EFUSE_APB2OTP_BLOCK0_BACKUP3_W4_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP3_W4_S  0

/** EFUSE_APB2OTP_BLK0_BACKUP3_W5_REG register
 *  eFuse apb2otp block0 data register16.
 */
#define EFUSE_APB2OTP_BLK0_BACKUP3_W5_REG (DR_REG_EFUSE_BASE + 0x83c)
/** EFUSE_APB2OTP_BLOCK0_BACKUP3_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup3 word5 data.
 */
#define EFUSE_APB2OTP_BLOCK0_BACKUP3_W5    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP3_W5_M  (EFUSE_APB2OTP_BLOCK0_BACKUP3_W5_V << EFUSE_APB2OTP_BLOCK0_BACKUP3_W5_S)
#define EFUSE_APB2OTP_BLOCK0_BACKUP3_W5_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP3_W5_S  0

/** EFUSE_APB2OTP_BLK0_BACKUP4_W1_REG register
 *  eFuse apb2otp block0 data register17.
 */
#define EFUSE_APB2OTP_BLK0_BACKUP4_W1_REG (DR_REG_EFUSE_BASE + 0x840)
/** EFUSE_APB2OTP_BLOCK0_BACKUP4_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup4 word1 data.
 */
#define EFUSE_APB2OTP_BLOCK0_BACKUP4_W1    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP4_W1_M  (EFUSE_APB2OTP_BLOCK0_BACKUP4_W1_V << EFUSE_APB2OTP_BLOCK0_BACKUP4_W1_S)
#define EFUSE_APB2OTP_BLOCK0_BACKUP4_W1_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP4_W1_S  0

/** EFUSE_APB2OTP_BLK0_BACKUP4_W2_REG register
 *  eFuse apb2otp block0 data register18.
 */
#define EFUSE_APB2OTP_BLK0_BACKUP4_W2_REG (DR_REG_EFUSE_BASE + 0x844)
/** EFUSE_APB2OTP_BLOCK0_BACKUP4_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup4 word2 data.
 */
#define EFUSE_APB2OTP_BLOCK0_BACKUP4_W2    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP4_W2_M  (EFUSE_APB2OTP_BLOCK0_BACKUP4_W2_V << EFUSE_APB2OTP_BLOCK0_BACKUP4_W2_S)
#define EFUSE_APB2OTP_BLOCK0_BACKUP4_W2_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP4_W2_S  0

/** EFUSE_APB2OTP_BLK0_BACKUP4_W3_REG register
 *  eFuse apb2otp block0 data register19.
 */
#define EFUSE_APB2OTP_BLK0_BACKUP4_W3_REG (DR_REG_EFUSE_BASE + 0x848)
/** EFUSE_APB2OTP_BLOCK0_BACKUP4_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup4 word3 data.
 */
#define EFUSE_APB2OTP_BLOCK0_BACKUP4_W3    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP4_W3_M  (EFUSE_APB2OTP_BLOCK0_BACKUP4_W3_V << EFUSE_APB2OTP_BLOCK0_BACKUP4_W3_S)
#define EFUSE_APB2OTP_BLOCK0_BACKUP4_W3_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP4_W3_S  0

/** EFUSE_APB2OTP_BLK0_BACKUP4_W4_REG register
 *  eFuse apb2otp block0 data register20.
 */
#define EFUSE_APB2OTP_BLK0_BACKUP4_W4_REG (DR_REG_EFUSE_BASE + 0x84c)
/** EFUSE_APB2OTP_BLOCK0_BACKUP4_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup4 word4 data.
 */
#define EFUSE_APB2OTP_BLOCK0_BACKUP4_W4    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP4_W4_M  (EFUSE_APB2OTP_BLOCK0_BACKUP4_W4_V << EFUSE_APB2OTP_BLOCK0_BACKUP4_W4_S)
#define EFUSE_APB2OTP_BLOCK0_BACKUP4_W4_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP4_W4_S  0

/** EFUSE_APB2OTP_BLK0_BACKUP4_W5_REG register
 *  eFuse apb2otp block0 data register21.
 */
#define EFUSE_APB2OTP_BLK0_BACKUP4_W5_REG (DR_REG_EFUSE_BASE + 0x850)
/** EFUSE_APB2OTP_BLOCK0_BACKUP4_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block0 backup4 word5 data.
 */
#define EFUSE_APB2OTP_BLOCK0_BACKUP4_W5    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP4_W5_M  (EFUSE_APB2OTP_BLOCK0_BACKUP4_W5_V << EFUSE_APB2OTP_BLOCK0_BACKUP4_W5_S)
#define EFUSE_APB2OTP_BLOCK0_BACKUP4_W5_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK0_BACKUP4_W5_S  0

/** EFUSE_APB2OTP_BLK1_W1_REG register
 *  eFuse apb2otp block1 data register1.
 */
#define EFUSE_APB2OTP_BLK1_W1_REG (DR_REG_EFUSE_BASE + 0x854)
/** EFUSE_APB2OTP_BLOCK1_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block1  word1 data.
 */
#define EFUSE_APB2OTP_BLOCK1_W1    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK1_W1_M  (EFUSE_APB2OTP_BLOCK1_W1_V << EFUSE_APB2OTP_BLOCK1_W1_S)
#define EFUSE_APB2OTP_BLOCK1_W1_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK1_W1_S  0

/** EFUSE_APB2OTP_BLK1_W2_REG register
 *  eFuse apb2otp block1 data register2.
 */
#define EFUSE_APB2OTP_BLK1_W2_REG (DR_REG_EFUSE_BASE + 0x858)
/** EFUSE_APB2OTP_BLOCK1_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block1  word2 data.
 */
#define EFUSE_APB2OTP_BLOCK1_W2    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK1_W2_M  (EFUSE_APB2OTP_BLOCK1_W2_V << EFUSE_APB2OTP_BLOCK1_W2_S)
#define EFUSE_APB2OTP_BLOCK1_W2_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK1_W2_S  0

/** EFUSE_APB2OTP_BLK1_W3_REG register
 *  eFuse apb2otp block1 data register3.
 */
#define EFUSE_APB2OTP_BLK1_W3_REG (DR_REG_EFUSE_BASE + 0x85c)
/** EFUSE_APB2OTP_BLOCK1_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block1  word3 data.
 */
#define EFUSE_APB2OTP_BLOCK1_W3    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK1_W3_M  (EFUSE_APB2OTP_BLOCK1_W3_V << EFUSE_APB2OTP_BLOCK1_W3_S)
#define EFUSE_APB2OTP_BLOCK1_W3_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK1_W3_S  0

/** EFUSE_APB2OTP_BLK1_W4_REG register
 *  eFuse apb2otp block1 data register4.
 */
#define EFUSE_APB2OTP_BLK1_W4_REG (DR_REG_EFUSE_BASE + 0x860)
/** EFUSE_APB2OTP_BLOCK1_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block1  word4 data.
 */
#define EFUSE_APB2OTP_BLOCK1_W4    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK1_W4_M  (EFUSE_APB2OTP_BLOCK1_W4_V << EFUSE_APB2OTP_BLOCK1_W4_S)
#define EFUSE_APB2OTP_BLOCK1_W4_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK1_W4_S  0

/** EFUSE_APB2OTP_BLK1_W5_REG register
 *  eFuse apb2otp block1 data register5.
 */
#define EFUSE_APB2OTP_BLK1_W5_REG (DR_REG_EFUSE_BASE + 0x864)
/** EFUSE_APB2OTP_BLOCK1_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block1  word5 data.
 */
#define EFUSE_APB2OTP_BLOCK1_W5    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK1_W5_M  (EFUSE_APB2OTP_BLOCK1_W5_V << EFUSE_APB2OTP_BLOCK1_W5_S)
#define EFUSE_APB2OTP_BLOCK1_W5_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK1_W5_S  0

/** EFUSE_APB2OTP_BLK1_W6_REG register
 *  eFuse apb2otp block1 data register6.
 */
#define EFUSE_APB2OTP_BLK1_W6_REG (DR_REG_EFUSE_BASE + 0x868)
/** EFUSE_APB2OTP_BLOCK1_W6 : RO; bitpos: [31:0]; default: 0;
 *  Otp block1  word6 data.
 */
#define EFUSE_APB2OTP_BLOCK1_W6    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK1_W6_M  (EFUSE_APB2OTP_BLOCK1_W6_V << EFUSE_APB2OTP_BLOCK1_W6_S)
#define EFUSE_APB2OTP_BLOCK1_W6_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK1_W6_S  0

/** EFUSE_APB2OTP_BLK1_W7_REG register
 *  eFuse apb2otp block1 data register7.
 */
#define EFUSE_APB2OTP_BLK1_W7_REG (DR_REG_EFUSE_BASE + 0x86c)
/** EFUSE_APB2OTP_BLOCK1_W7 : RO; bitpos: [31:0]; default: 0;
 *  Otp block1  word7 data.
 */
#define EFUSE_APB2OTP_BLOCK1_W7    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK1_W7_M  (EFUSE_APB2OTP_BLOCK1_W7_V << EFUSE_APB2OTP_BLOCK1_W7_S)
#define EFUSE_APB2OTP_BLOCK1_W7_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK1_W7_S  0

/** EFUSE_APB2OTP_BLK1_W8_REG register
 *  eFuse apb2otp block1 data register8.
 */
#define EFUSE_APB2OTP_BLK1_W8_REG (DR_REG_EFUSE_BASE + 0x870)
/** EFUSE_APB2OTP_BLOCK1_W8 : RO; bitpos: [31:0]; default: 0;
 *  Otp block1  word8 data.
 */
#define EFUSE_APB2OTP_BLOCK1_W8    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK1_W8_M  (EFUSE_APB2OTP_BLOCK1_W8_V << EFUSE_APB2OTP_BLOCK1_W8_S)
#define EFUSE_APB2OTP_BLOCK1_W8_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK1_W8_S  0

/** EFUSE_APB2OTP_BLK1_W9_REG register
 *  eFuse apb2otp block1 data register9.
 */
#define EFUSE_APB2OTP_BLK1_W9_REG (DR_REG_EFUSE_BASE + 0x874)
/** EFUSE_APB2OTP_BLOCK1_W9 : RO; bitpos: [31:0]; default: 0;
 *  Otp block1  word9 data.
 */
#define EFUSE_APB2OTP_BLOCK1_W9    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK1_W9_M  (EFUSE_APB2OTP_BLOCK1_W9_V << EFUSE_APB2OTP_BLOCK1_W9_S)
#define EFUSE_APB2OTP_BLOCK1_W9_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK1_W9_S  0

/** EFUSE_APB2OTP_BLK2_W1_REG register
 *  eFuse apb2otp block2 data register1.
 */
#define EFUSE_APB2OTP_BLK2_W1_REG (DR_REG_EFUSE_BASE + 0x878)
/** EFUSE_APB2OTP_BLOCK2_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block2 word1 data.
 */
#define EFUSE_APB2OTP_BLOCK2_W1    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK2_W1_M  (EFUSE_APB2OTP_BLOCK2_W1_V << EFUSE_APB2OTP_BLOCK2_W1_S)
#define EFUSE_APB2OTP_BLOCK2_W1_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK2_W1_S  0

/** EFUSE_APB2OTP_BLK2_W2_REG register
 *  eFuse apb2otp block2 data register2.
 */
#define EFUSE_APB2OTP_BLK2_W2_REG (DR_REG_EFUSE_BASE + 0x87c)
/** EFUSE_APB2OTP_BLOCK2_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block2 word2 data.
 */
#define EFUSE_APB2OTP_BLOCK2_W2    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK2_W2_M  (EFUSE_APB2OTP_BLOCK2_W2_V << EFUSE_APB2OTP_BLOCK2_W2_S)
#define EFUSE_APB2OTP_BLOCK2_W2_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK2_W2_S  0

/** EFUSE_APB2OTP_BLK2_W3_REG register
 *  eFuse apb2otp block2 data register3.
 */
#define EFUSE_APB2OTP_BLK2_W3_REG (DR_REG_EFUSE_BASE + 0x880)
/** EFUSE_APB2OTP_BLOCK2_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block2 word3 data.
 */
#define EFUSE_APB2OTP_BLOCK2_W3    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK2_W3_M  (EFUSE_APB2OTP_BLOCK2_W3_V << EFUSE_APB2OTP_BLOCK2_W3_S)
#define EFUSE_APB2OTP_BLOCK2_W3_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK2_W3_S  0

/** EFUSE_APB2OTP_BLK2_W4_REG register
 *  eFuse apb2otp block2 data register4.
 */
#define EFUSE_APB2OTP_BLK2_W4_REG (DR_REG_EFUSE_BASE + 0x884)
/** EFUSE_APB2OTP_BLOCK2_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block2 word4 data.
 */
#define EFUSE_APB2OTP_BLOCK2_W4    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK2_W4_M  (EFUSE_APB2OTP_BLOCK2_W4_V << EFUSE_APB2OTP_BLOCK2_W4_S)
#define EFUSE_APB2OTP_BLOCK2_W4_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK2_W4_S  0

/** EFUSE_APB2OTP_BLK2_W5_REG register
 *  eFuse apb2otp block2 data register5.
 */
#define EFUSE_APB2OTP_BLK2_W5_REG (DR_REG_EFUSE_BASE + 0x888)
/** EFUSE_APB2OTP_BLOCK2_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block2 word5 data.
 */
#define EFUSE_APB2OTP_BLOCK2_W5    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK2_W5_M  (EFUSE_APB2OTP_BLOCK2_W5_V << EFUSE_APB2OTP_BLOCK2_W5_S)
#define EFUSE_APB2OTP_BLOCK2_W5_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK2_W5_S  0

/** EFUSE_APB2OTP_BLK2_W6_REG register
 *  eFuse apb2otp block2 data register6.
 */
#define EFUSE_APB2OTP_BLK2_W6_REG (DR_REG_EFUSE_BASE + 0x88c)
/** EFUSE_APB2OTP_BLOCK2_W6 : RO; bitpos: [31:0]; default: 0;
 *  Otp block2 word6 data.
 */
#define EFUSE_APB2OTP_BLOCK2_W6    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK2_W6_M  (EFUSE_APB2OTP_BLOCK2_W6_V << EFUSE_APB2OTP_BLOCK2_W6_S)
#define EFUSE_APB2OTP_BLOCK2_W6_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK2_W6_S  0

/** EFUSE_APB2OTP_BLK2_W7_REG register
 *  eFuse apb2otp block2 data register7.
 */
#define EFUSE_APB2OTP_BLK2_W7_REG (DR_REG_EFUSE_BASE + 0x890)
/** EFUSE_APB2OTP_BLOCK2_W7 : RO; bitpos: [31:0]; default: 0;
 *  Otp block2 word7 data.
 */
#define EFUSE_APB2OTP_BLOCK2_W7    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK2_W7_M  (EFUSE_APB2OTP_BLOCK2_W7_V << EFUSE_APB2OTP_BLOCK2_W7_S)
#define EFUSE_APB2OTP_BLOCK2_W7_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK2_W7_S  0

/** EFUSE_APB2OTP_BLK2_W8_REG register
 *  eFuse apb2otp block2 data register8.
 */
#define EFUSE_APB2OTP_BLK2_W8_REG (DR_REG_EFUSE_BASE + 0x894)
/** EFUSE_APB2OTP_BLOCK2_W8 : RO; bitpos: [31:0]; default: 0;
 *  Otp block2 word8 data.
 */
#define EFUSE_APB2OTP_BLOCK2_W8    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK2_W8_M  (EFUSE_APB2OTP_BLOCK2_W8_V << EFUSE_APB2OTP_BLOCK2_W8_S)
#define EFUSE_APB2OTP_BLOCK2_W8_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK2_W8_S  0

/** EFUSE_APB2OTP_BLK2_W9_REG register
 *  eFuse apb2otp block2 data register9.
 */
#define EFUSE_APB2OTP_BLK2_W9_REG (DR_REG_EFUSE_BASE + 0x898)
/** EFUSE_APB2OTP_BLOCK2_W9 : RO; bitpos: [31:0]; default: 0;
 *  Otp block2 word9 data.
 */
#define EFUSE_APB2OTP_BLOCK2_W9    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK2_W9_M  (EFUSE_APB2OTP_BLOCK2_W9_V << EFUSE_APB2OTP_BLOCK2_W9_S)
#define EFUSE_APB2OTP_BLOCK2_W9_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK2_W9_S  0

/** EFUSE_APB2OTP_BLK2_W10_REG register
 *  eFuse apb2otp block2 data register10.
 */
#define EFUSE_APB2OTP_BLK2_W10_REG (DR_REG_EFUSE_BASE + 0x89c)
/** EFUSE_APB2OTP_BLOCK2_W10 : RO; bitpos: [31:0]; default: 0;
 *  Otp block2 word10 data.
 */
#define EFUSE_APB2OTP_BLOCK2_W10    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK2_W10_M  (EFUSE_APB2OTP_BLOCK2_W10_V << EFUSE_APB2OTP_BLOCK2_W10_S)
#define EFUSE_APB2OTP_BLOCK2_W10_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK2_W10_S  0

/** EFUSE_APB2OTP_BLK2_W11_REG register
 *  eFuse apb2otp block2 data register11.
 */
#define EFUSE_APB2OTP_BLK2_W11_REG (DR_REG_EFUSE_BASE + 0x8a0)
/** EFUSE_APB2OTP_BLOCK2_W11 : RO; bitpos: [31:0]; default: 0;
 *  Otp block2 word11 data.
 */
#define EFUSE_APB2OTP_BLOCK2_W11    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK2_W11_M  (EFUSE_APB2OTP_BLOCK2_W11_V << EFUSE_APB2OTP_BLOCK2_W11_S)
#define EFUSE_APB2OTP_BLOCK2_W11_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK2_W11_S  0

/** EFUSE_APB2OTP_BLK3_W1_REG register
 *  eFuse apb2otp block3 data register1.
 */
#define EFUSE_APB2OTP_BLK3_W1_REG (DR_REG_EFUSE_BASE + 0x8a4)
/** EFUSE_APB2OTP_BLOCK3_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block3 word1 data.
 */
#define EFUSE_APB2OTP_BLOCK3_W1    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK3_W1_M  (EFUSE_APB2OTP_BLOCK3_W1_V << EFUSE_APB2OTP_BLOCK3_W1_S)
#define EFUSE_APB2OTP_BLOCK3_W1_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK3_W1_S  0

/** EFUSE_APB2OTP_BLK3_W2_REG register
 *  eFuse apb2otp block3 data register2.
 */
#define EFUSE_APB2OTP_BLK3_W2_REG (DR_REG_EFUSE_BASE + 0x8a8)
/** EFUSE_APB2OTP_BLOCK3_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block3 word2 data.
 */
#define EFUSE_APB2OTP_BLOCK3_W2    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK3_W2_M  (EFUSE_APB2OTP_BLOCK3_W2_V << EFUSE_APB2OTP_BLOCK3_W2_S)
#define EFUSE_APB2OTP_BLOCK3_W2_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK3_W2_S  0

/** EFUSE_APB2OTP_BLK3_W3_REG register
 *  eFuse apb2otp block3 data register3.
 */
#define EFUSE_APB2OTP_BLK3_W3_REG (DR_REG_EFUSE_BASE + 0x8ac)
/** EFUSE_APB2OTP_BLOCK3_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block3 word3 data.
 */
#define EFUSE_APB2OTP_BLOCK3_W3    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK3_W3_M  (EFUSE_APB2OTP_BLOCK3_W3_V << EFUSE_APB2OTP_BLOCK3_W3_S)
#define EFUSE_APB2OTP_BLOCK3_W3_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK3_W3_S  0

/** EFUSE_APB2OTP_BLK3_W4_REG register
 *  eFuse apb2otp block3 data register4.
 */
#define EFUSE_APB2OTP_BLK3_W4_REG (DR_REG_EFUSE_BASE + 0x8b0)
/** EFUSE_APB2OTP_BLOCK3_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block3 word4 data.
 */
#define EFUSE_APB2OTP_BLOCK3_W4    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK3_W4_M  (EFUSE_APB2OTP_BLOCK3_W4_V << EFUSE_APB2OTP_BLOCK3_W4_S)
#define EFUSE_APB2OTP_BLOCK3_W4_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK3_W4_S  0

/** EFUSE_APB2OTP_BLK3_W5_REG register
 *  eFuse apb2otp block3 data register5.
 */
#define EFUSE_APB2OTP_BLK3_W5_REG (DR_REG_EFUSE_BASE + 0x8b4)
/** EFUSE_APB2OTP_BLOCK3_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block3 word5 data.
 */
#define EFUSE_APB2OTP_BLOCK3_W5    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK3_W5_M  (EFUSE_APB2OTP_BLOCK3_W5_V << EFUSE_APB2OTP_BLOCK3_W5_S)
#define EFUSE_APB2OTP_BLOCK3_W5_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK3_W5_S  0

/** EFUSE_APB2OTP_BLK3_W6_REG register
 *  eFuse apb2otp block3 data register6.
 */
#define EFUSE_APB2OTP_BLK3_W6_REG (DR_REG_EFUSE_BASE + 0x8b8)
/** EFUSE_APB2OTP_BLOCK3_W6 : RO; bitpos: [31:0]; default: 0;
 *  Otp block3 word6 data.
 */
#define EFUSE_APB2OTP_BLOCK3_W6    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK3_W6_M  (EFUSE_APB2OTP_BLOCK3_W6_V << EFUSE_APB2OTP_BLOCK3_W6_S)
#define EFUSE_APB2OTP_BLOCK3_W6_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK3_W6_S  0

/** EFUSE_APB2OTP_BLK3_W7_REG register
 *  eFuse apb2otp block3 data register7.
 */
#define EFUSE_APB2OTP_BLK3_W7_REG (DR_REG_EFUSE_BASE + 0x8bc)
/** EFUSE_APB2OTP_BLOCK3_W7 : RO; bitpos: [31:0]; default: 0;
 *  Otp block3 word7 data.
 */
#define EFUSE_APB2OTP_BLOCK3_W7    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK3_W7_M  (EFUSE_APB2OTP_BLOCK3_W7_V << EFUSE_APB2OTP_BLOCK3_W7_S)
#define EFUSE_APB2OTP_BLOCK3_W7_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK3_W7_S  0

/** EFUSE_APB2OTP_BLK3_W8_REG register
 *  eFuse apb2otp block3 data register8.
 */
#define EFUSE_APB2OTP_BLK3_W8_REG (DR_REG_EFUSE_BASE + 0x8c0)
/** EFUSE_APB2OTP_BLOCK3_W8 : RO; bitpos: [31:0]; default: 0;
 *  Otp block3 word8 data.
 */
#define EFUSE_APB2OTP_BLOCK3_W8    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK3_W8_M  (EFUSE_APB2OTP_BLOCK3_W8_V << EFUSE_APB2OTP_BLOCK3_W8_S)
#define EFUSE_APB2OTP_BLOCK3_W8_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK3_W8_S  0

/** EFUSE_APB2OTP_BLK3_W9_REG register
 *  eFuse apb2otp block3 data register9.
 */
#define EFUSE_APB2OTP_BLK3_W9_REG (DR_REG_EFUSE_BASE + 0x8c4)
/** EFUSE_APB2OTP_BLOCK3_W9 : RO; bitpos: [31:0]; default: 0;
 *  Otp block3 word9 data.
 */
#define EFUSE_APB2OTP_BLOCK3_W9    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK3_W9_M  (EFUSE_APB2OTP_BLOCK3_W9_V << EFUSE_APB2OTP_BLOCK3_W9_S)
#define EFUSE_APB2OTP_BLOCK3_W9_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK3_W9_S  0

/** EFUSE_APB2OTP_BLK3_W10_REG register
 *  eFuse apb2otp block3 data register10.
 */
#define EFUSE_APB2OTP_BLK3_W10_REG (DR_REG_EFUSE_BASE + 0x8c8)
/** EFUSE_APB2OTP_BLOCK3_W10 : RO; bitpos: [31:0]; default: 0;
 *  Otp block3 word10 data.
 */
#define EFUSE_APB2OTP_BLOCK3_W10    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK3_W10_M  (EFUSE_APB2OTP_BLOCK3_W10_V << EFUSE_APB2OTP_BLOCK3_W10_S)
#define EFUSE_APB2OTP_BLOCK3_W10_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK3_W10_S  0

/** EFUSE_APB2OTP_BLK3_W11_REG register
 *  eFuse apb2otp block3 data register11.
 */
#define EFUSE_APB2OTP_BLK3_W11_REG (DR_REG_EFUSE_BASE + 0x8cc)
/** EFUSE_APB2OTP_BLOCK3_W11 : RO; bitpos: [31:0]; default: 0;
 *  Otp block3 word11 data.
 */
#define EFUSE_APB2OTP_BLOCK3_W11    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK3_W11_M  (EFUSE_APB2OTP_BLOCK3_W11_V << EFUSE_APB2OTP_BLOCK3_W11_S)
#define EFUSE_APB2OTP_BLOCK3_W11_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK3_W11_S  0

/** EFUSE_APB2OTP_BLK4_W1_REG register
 *  eFuse apb2otp block4 data register1.
 */
#define EFUSE_APB2OTP_BLK4_W1_REG (DR_REG_EFUSE_BASE + 0x8d0)
/** EFUSE_APB2OTP_BLOCK4_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block4 word1 data.
 */
#define EFUSE_APB2OTP_BLOCK4_W1    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK4_W1_M  (EFUSE_APB2OTP_BLOCK4_W1_V << EFUSE_APB2OTP_BLOCK4_W1_S)
#define EFUSE_APB2OTP_BLOCK4_W1_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK4_W1_S  0

/** EFUSE_APB2OTP_BLK4_W2_REG register
 *  eFuse apb2otp block4 data register2.
 */
#define EFUSE_APB2OTP_BLK4_W2_REG (DR_REG_EFUSE_BASE + 0x8d4)
/** EFUSE_APB2OTP_BLOCK4_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block4 word2 data.
 */
#define EFUSE_APB2OTP_BLOCK4_W2    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK4_W2_M  (EFUSE_APB2OTP_BLOCK4_W2_V << EFUSE_APB2OTP_BLOCK4_W2_S)
#define EFUSE_APB2OTP_BLOCK4_W2_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK4_W2_S  0

/** EFUSE_APB2OTP_BLK4_W3_REG register
 *  eFuse apb2otp block4 data register3.
 */
#define EFUSE_APB2OTP_BLK4_W3_REG (DR_REG_EFUSE_BASE + 0x8d8)
/** EFUSE_APB2OTP_BLOCK4_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block4 word3 data.
 */
#define EFUSE_APB2OTP_BLOCK4_W3    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK4_W3_M  (EFUSE_APB2OTP_BLOCK4_W3_V << EFUSE_APB2OTP_BLOCK4_W3_S)
#define EFUSE_APB2OTP_BLOCK4_W3_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK4_W3_S  0

/** EFUSE_APB2OTP_BLK4_W4_REG register
 *  eFuse apb2otp block4 data register4.
 */
#define EFUSE_APB2OTP_BLK4_W4_REG (DR_REG_EFUSE_BASE + 0x8dc)
/** EFUSE_APB2OTP_BLOCK4_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block4 word4 data.
 */
#define EFUSE_APB2OTP_BLOCK4_W4    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK4_W4_M  (EFUSE_APB2OTP_BLOCK4_W4_V << EFUSE_APB2OTP_BLOCK4_W4_S)
#define EFUSE_APB2OTP_BLOCK4_W4_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK4_W4_S  0

/** EFUSE_APB2OTP_BLK4_W5_REG register
 *  eFuse apb2otp block4 data register5.
 */
#define EFUSE_APB2OTP_BLK4_W5_REG (DR_REG_EFUSE_BASE + 0x8e0)
/** EFUSE_APB2OTP_BLOCK4_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block4 word5 data.
 */
#define EFUSE_APB2OTP_BLOCK4_W5    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK4_W5_M  (EFUSE_APB2OTP_BLOCK4_W5_V << EFUSE_APB2OTP_BLOCK4_W5_S)
#define EFUSE_APB2OTP_BLOCK4_W5_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK4_W5_S  0

/** EFUSE_APB2OTP_BLK4_W6_REG register
 *  eFuse apb2otp block4 data register6.
 */
#define EFUSE_APB2OTP_BLK4_W6_REG (DR_REG_EFUSE_BASE + 0x8e4)
/** EFUSE_APB2OTP_BLOCK4_W6 : RO; bitpos: [31:0]; default: 0;
 *  Otp block4 word6 data.
 */
#define EFUSE_APB2OTP_BLOCK4_W6    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK4_W6_M  (EFUSE_APB2OTP_BLOCK4_W6_V << EFUSE_APB2OTP_BLOCK4_W6_S)
#define EFUSE_APB2OTP_BLOCK4_W6_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK4_W6_S  0

/** EFUSE_APB2OTP_BLK4_W7_REG register
 *  eFuse apb2otp block4 data register7.
 */
#define EFUSE_APB2OTP_BLK4_W7_REG (DR_REG_EFUSE_BASE + 0x8e8)
/** EFUSE_APB2OTP_BLOCK4_W7 : RO; bitpos: [31:0]; default: 0;
 *  Otp block4 word7 data.
 */
#define EFUSE_APB2OTP_BLOCK4_W7    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK4_W7_M  (EFUSE_APB2OTP_BLOCK4_W7_V << EFUSE_APB2OTP_BLOCK4_W7_S)
#define EFUSE_APB2OTP_BLOCK4_W7_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK4_W7_S  0

/** EFUSE_APB2OTP_BLK4_W8_REG register
 *  eFuse apb2otp block4 data register8.
 */
#define EFUSE_APB2OTP_BLK4_W8_REG (DR_REG_EFUSE_BASE + 0x8ec)
/** EFUSE_APB2OTP_BLOCK4_W8 : RO; bitpos: [31:0]; default: 0;
 *  Otp block4 word8 data.
 */
#define EFUSE_APB2OTP_BLOCK4_W8    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK4_W8_M  (EFUSE_APB2OTP_BLOCK4_W8_V << EFUSE_APB2OTP_BLOCK4_W8_S)
#define EFUSE_APB2OTP_BLOCK4_W8_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK4_W8_S  0

/** EFUSE_APB2OTP_BLK4_W9_REG register
 *  eFuse apb2otp block4 data register9.
 */
#define EFUSE_APB2OTP_BLK4_W9_REG (DR_REG_EFUSE_BASE + 0x8f0)
/** EFUSE_APB2OTP_BLOCK4_W9 : RO; bitpos: [31:0]; default: 0;
 *  Otp block4 word9 data.
 */
#define EFUSE_APB2OTP_BLOCK4_W9    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK4_W9_M  (EFUSE_APB2OTP_BLOCK4_W9_V << EFUSE_APB2OTP_BLOCK4_W9_S)
#define EFUSE_APB2OTP_BLOCK4_W9_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK4_W9_S  0

/** EFUSE_APB2OTP_BLK4_W10_REG register
 *  eFuse apb2otp block4 data registe10.
 */
#define EFUSE_APB2OTP_BLK4_W10_REG (DR_REG_EFUSE_BASE + 0x8f4)
/** EFUSE_APB2OTP_BLOCK4_W10 : RO; bitpos: [31:0]; default: 0;
 *  Otp block4 word10 data.
 */
#define EFUSE_APB2OTP_BLOCK4_W10    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK4_W10_M  (EFUSE_APB2OTP_BLOCK4_W10_V << EFUSE_APB2OTP_BLOCK4_W10_S)
#define EFUSE_APB2OTP_BLOCK4_W10_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK4_W10_S  0

/** EFUSE_APB2OTP_BLK4_W11_REG register
 *  eFuse apb2otp block4 data register11.
 */
#define EFUSE_APB2OTP_BLK4_W11_REG (DR_REG_EFUSE_BASE + 0x8f8)
/** EFUSE_APB2OTP_BLOCK4_W11 : RO; bitpos: [31:0]; default: 0;
 *  Otp block4 word11 data.
 */
#define EFUSE_APB2OTP_BLOCK4_W11    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK4_W11_M  (EFUSE_APB2OTP_BLOCK4_W11_V << EFUSE_APB2OTP_BLOCK4_W11_S)
#define EFUSE_APB2OTP_BLOCK4_W11_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK4_W11_S  0

/** EFUSE_APB2OTP_BLK5_W1_REG register
 *  eFuse apb2otp block5 data register1.
 */
#define EFUSE_APB2OTP_BLK5_W1_REG (DR_REG_EFUSE_BASE + 0x8fc)
/** EFUSE_APB2OTP_BLOCK5_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block5 word1 data.
 */
#define EFUSE_APB2OTP_BLOCK5_W1    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK5_W1_M  (EFUSE_APB2OTP_BLOCK5_W1_V << EFUSE_APB2OTP_BLOCK5_W1_S)
#define EFUSE_APB2OTP_BLOCK5_W1_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK5_W1_S  0

/** EFUSE_APB2OTP_BLK5_W2_REG register
 *  eFuse apb2otp block5 data register2.
 */
#define EFUSE_APB2OTP_BLK5_W2_REG (DR_REG_EFUSE_BASE + 0x900)
/** EFUSE_APB2OTP_BLOCK5_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block5 word2 data.
 */
#define EFUSE_APB2OTP_BLOCK5_W2    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK5_W2_M  (EFUSE_APB2OTP_BLOCK5_W2_V << EFUSE_APB2OTP_BLOCK5_W2_S)
#define EFUSE_APB2OTP_BLOCK5_W2_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK5_W2_S  0

/** EFUSE_APB2OTP_BLK5_W3_REG register
 *  eFuse apb2otp block5 data register3.
 */
#define EFUSE_APB2OTP_BLK5_W3_REG (DR_REG_EFUSE_BASE + 0x904)
/** EFUSE_APB2OTP_BLOCK5_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block5 word3 data.
 */
#define EFUSE_APB2OTP_BLOCK5_W3    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK5_W3_M  (EFUSE_APB2OTP_BLOCK5_W3_V << EFUSE_APB2OTP_BLOCK5_W3_S)
#define EFUSE_APB2OTP_BLOCK5_W3_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK5_W3_S  0

/** EFUSE_APB2OTP_BLK5_W4_REG register
 *  eFuse apb2otp block5 data register4.
 */
#define EFUSE_APB2OTP_BLK5_W4_REG (DR_REG_EFUSE_BASE + 0x908)
/** EFUSE_APB2OTP_BLOCK5_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block5 word4 data.
 */
#define EFUSE_APB2OTP_BLOCK5_W4    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK5_W4_M  (EFUSE_APB2OTP_BLOCK5_W4_V << EFUSE_APB2OTP_BLOCK5_W4_S)
#define EFUSE_APB2OTP_BLOCK5_W4_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK5_W4_S  0

/** EFUSE_APB2OTP_BLK5_W5_REG register
 *  eFuse apb2otp block5 data register5.
 */
#define EFUSE_APB2OTP_BLK5_W5_REG (DR_REG_EFUSE_BASE + 0x90c)
/** EFUSE_APB2OTP_BLOCK5_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block5 word5 data.
 */
#define EFUSE_APB2OTP_BLOCK5_W5    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK5_W5_M  (EFUSE_APB2OTP_BLOCK5_W5_V << EFUSE_APB2OTP_BLOCK5_W5_S)
#define EFUSE_APB2OTP_BLOCK5_W5_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK5_W5_S  0

/** EFUSE_APB2OTP_BLK5_W6_REG register
 *  eFuse apb2otp block5 data register6.
 */
#define EFUSE_APB2OTP_BLK5_W6_REG (DR_REG_EFUSE_BASE + 0x910)
/** EFUSE_APB2OTP_BLOCK5_W6 : RO; bitpos: [31:0]; default: 0;
 *  Otp block5 word6 data.
 */
#define EFUSE_APB2OTP_BLOCK5_W6    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK5_W6_M  (EFUSE_APB2OTP_BLOCK5_W6_V << EFUSE_APB2OTP_BLOCK5_W6_S)
#define EFUSE_APB2OTP_BLOCK5_W6_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK5_W6_S  0

/** EFUSE_APB2OTP_BLK5_W7_REG register
 *  eFuse apb2otp block5 data register7.
 */
#define EFUSE_APB2OTP_BLK5_W7_REG (DR_REG_EFUSE_BASE + 0x914)
/** EFUSE_APB2OTP_BLOCK5_W7 : RO; bitpos: [31:0]; default: 0;
 *  Otp block5 word7 data.
 */
#define EFUSE_APB2OTP_BLOCK5_W7    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK5_W7_M  (EFUSE_APB2OTP_BLOCK5_W7_V << EFUSE_APB2OTP_BLOCK5_W7_S)
#define EFUSE_APB2OTP_BLOCK5_W7_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK5_W7_S  0

/** EFUSE_APB2OTP_BLK5_W8_REG register
 *  eFuse apb2otp block5 data register8.
 */
#define EFUSE_APB2OTP_BLK5_W8_REG (DR_REG_EFUSE_BASE + 0x918)
/** EFUSE_APB2OTP_BLOCK5_W8 : RO; bitpos: [31:0]; default: 0;
 *  Otp block5 word8 data.
 */
#define EFUSE_APB2OTP_BLOCK5_W8    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK5_W8_M  (EFUSE_APB2OTP_BLOCK5_W8_V << EFUSE_APB2OTP_BLOCK5_W8_S)
#define EFUSE_APB2OTP_BLOCK5_W8_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK5_W8_S  0

/** EFUSE_APB2OTP_BLK5_W9_REG register
 *  eFuse apb2otp block5 data register9.
 */
#define EFUSE_APB2OTP_BLK5_W9_REG (DR_REG_EFUSE_BASE + 0x91c)
/** EFUSE_APB2OTP_BLOCK5_W9 : RO; bitpos: [31:0]; default: 0;
 *  Otp block5 word9 data.
 */
#define EFUSE_APB2OTP_BLOCK5_W9    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK5_W9_M  (EFUSE_APB2OTP_BLOCK5_W9_V << EFUSE_APB2OTP_BLOCK5_W9_S)
#define EFUSE_APB2OTP_BLOCK5_W9_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK5_W9_S  0

/** EFUSE_APB2OTP_BLK5_W10_REG register
 *  eFuse apb2otp block5 data register10.
 */
#define EFUSE_APB2OTP_BLK5_W10_REG (DR_REG_EFUSE_BASE + 0x920)
/** EFUSE_APB2OTP_BLOCK5_W10 : RO; bitpos: [31:0]; default: 0;
 *  Otp block5 word10 data.
 */
#define EFUSE_APB2OTP_BLOCK5_W10    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK5_W10_M  (EFUSE_APB2OTP_BLOCK5_W10_V << EFUSE_APB2OTP_BLOCK5_W10_S)
#define EFUSE_APB2OTP_BLOCK5_W10_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK5_W10_S  0

/** EFUSE_APB2OTP_BLK5_W11_REG register
 *  eFuse apb2otp block5 data register11.
 */
#define EFUSE_APB2OTP_BLK5_W11_REG (DR_REG_EFUSE_BASE + 0x924)
/** EFUSE_APB2OTP_BLOCK5_W11 : RO; bitpos: [31:0]; default: 0;
 *  Otp block5 word11 data.
 */
#define EFUSE_APB2OTP_BLOCK5_W11    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK5_W11_M  (EFUSE_APB2OTP_BLOCK5_W11_V << EFUSE_APB2OTP_BLOCK5_W11_S)
#define EFUSE_APB2OTP_BLOCK5_W11_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK5_W11_S  0

/** EFUSE_APB2OTP_BLK6_W1_REG register
 *  eFuse apb2otp block6 data register1.
 */
#define EFUSE_APB2OTP_BLK6_W1_REG (DR_REG_EFUSE_BASE + 0x928)
/** EFUSE_APB2OTP_BLOCK6_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block6 word1 data.
 */
#define EFUSE_APB2OTP_BLOCK6_W1    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK6_W1_M  (EFUSE_APB2OTP_BLOCK6_W1_V << EFUSE_APB2OTP_BLOCK6_W1_S)
#define EFUSE_APB2OTP_BLOCK6_W1_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK6_W1_S  0

/** EFUSE_APB2OTP_BLK6_W2_REG register
 *  eFuse apb2otp block6 data register2.
 */
#define EFUSE_APB2OTP_BLK6_W2_REG (DR_REG_EFUSE_BASE + 0x92c)
/** EFUSE_APB2OTP_BLOCK6_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block6 word2 data.
 */
#define EFUSE_APB2OTP_BLOCK6_W2    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK6_W2_M  (EFUSE_APB2OTP_BLOCK6_W2_V << EFUSE_APB2OTP_BLOCK6_W2_S)
#define EFUSE_APB2OTP_BLOCK6_W2_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK6_W2_S  0

/** EFUSE_APB2OTP_BLK6_W3_REG register
 *  eFuse apb2otp block6 data register3.
 */
#define EFUSE_APB2OTP_BLK6_W3_REG (DR_REG_EFUSE_BASE + 0x930)
/** EFUSE_APB2OTP_BLOCK6_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block6 word3 data.
 */
#define EFUSE_APB2OTP_BLOCK6_W3    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK6_W3_M  (EFUSE_APB2OTP_BLOCK6_W3_V << EFUSE_APB2OTP_BLOCK6_W3_S)
#define EFUSE_APB2OTP_BLOCK6_W3_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK6_W3_S  0

/** EFUSE_APB2OTP_BLK6_W4_REG register
 *  eFuse apb2otp block6 data register4.
 */
#define EFUSE_APB2OTP_BLK6_W4_REG (DR_REG_EFUSE_BASE + 0x934)
/** EFUSE_APB2OTP_BLOCK6_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block6 word4 data.
 */
#define EFUSE_APB2OTP_BLOCK6_W4    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK6_W4_M  (EFUSE_APB2OTP_BLOCK6_W4_V << EFUSE_APB2OTP_BLOCK6_W4_S)
#define EFUSE_APB2OTP_BLOCK6_W4_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK6_W4_S  0

/** EFUSE_APB2OTP_BLK6_W5_REG register
 *  eFuse apb2otp block6 data register5.
 */
#define EFUSE_APB2OTP_BLK6_W5_REG (DR_REG_EFUSE_BASE + 0x938)
/** EFUSE_APB2OTP_BLOCK6_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block6 word5 data.
 */
#define EFUSE_APB2OTP_BLOCK6_W5    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK6_W5_M  (EFUSE_APB2OTP_BLOCK6_W5_V << EFUSE_APB2OTP_BLOCK6_W5_S)
#define EFUSE_APB2OTP_BLOCK6_W5_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK6_W5_S  0

/** EFUSE_APB2OTP_BLK6_W6_REG register
 *  eFuse apb2otp block6 data register6.
 */
#define EFUSE_APB2OTP_BLK6_W6_REG (DR_REG_EFUSE_BASE + 0x93c)
/** EFUSE_APB2OTP_BLOCK6_W6 : RO; bitpos: [31:0]; default: 0;
 *  Otp block6 word6 data.
 */
#define EFUSE_APB2OTP_BLOCK6_W6    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK6_W6_M  (EFUSE_APB2OTP_BLOCK6_W6_V << EFUSE_APB2OTP_BLOCK6_W6_S)
#define EFUSE_APB2OTP_BLOCK6_W6_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK6_W6_S  0

/** EFUSE_APB2OTP_BLK6_W7_REG register
 *  eFuse apb2otp block6 data register7.
 */
#define EFUSE_APB2OTP_BLK6_W7_REG (DR_REG_EFUSE_BASE + 0x940)
/** EFUSE_APB2OTP_BLOCK6_W7 : RO; bitpos: [31:0]; default: 0;
 *  Otp block6 word7 data.
 */
#define EFUSE_APB2OTP_BLOCK6_W7    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK6_W7_M  (EFUSE_APB2OTP_BLOCK6_W7_V << EFUSE_APB2OTP_BLOCK6_W7_S)
#define EFUSE_APB2OTP_BLOCK6_W7_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK6_W7_S  0

/** EFUSE_APB2OTP_BLK6_W8_REG register
 *  eFuse apb2otp block6 data register8.
 */
#define EFUSE_APB2OTP_BLK6_W8_REG (DR_REG_EFUSE_BASE + 0x944)
/** EFUSE_APB2OTP_BLOCK6_W8 : RO; bitpos: [31:0]; default: 0;
 *  Otp block6 word8 data.
 */
#define EFUSE_APB2OTP_BLOCK6_W8    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK6_W8_M  (EFUSE_APB2OTP_BLOCK6_W8_V << EFUSE_APB2OTP_BLOCK6_W8_S)
#define EFUSE_APB2OTP_BLOCK6_W8_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK6_W8_S  0

/** EFUSE_APB2OTP_BLK6_W9_REG register
 *  eFuse apb2otp block6 data register9.
 */
#define EFUSE_APB2OTP_BLK6_W9_REG (DR_REG_EFUSE_BASE + 0x948)
/** EFUSE_APB2OTP_BLOCK6_W9 : RO; bitpos: [31:0]; default: 0;
 *  Otp block6 word9 data.
 */
#define EFUSE_APB2OTP_BLOCK6_W9    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK6_W9_M  (EFUSE_APB2OTP_BLOCK6_W9_V << EFUSE_APB2OTP_BLOCK6_W9_S)
#define EFUSE_APB2OTP_BLOCK6_W9_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK6_W9_S  0

/** EFUSE_APB2OTP_BLK6_W10_REG register
 *  eFuse apb2otp block6 data register10.
 */
#define EFUSE_APB2OTP_BLK6_W10_REG (DR_REG_EFUSE_BASE + 0x94c)
/** EFUSE_APB2OTP_BLOCK6_W10 : RO; bitpos: [31:0]; default: 0;
 *  Otp block6 word10 data.
 */
#define EFUSE_APB2OTP_BLOCK6_W10    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK6_W10_M  (EFUSE_APB2OTP_BLOCK6_W10_V << EFUSE_APB2OTP_BLOCK6_W10_S)
#define EFUSE_APB2OTP_BLOCK6_W10_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK6_W10_S  0

/** EFUSE_APB2OTP_BLK6_W11_REG register
 *  eFuse apb2otp block6 data register11.
 */
#define EFUSE_APB2OTP_BLK6_W11_REG (DR_REG_EFUSE_BASE + 0x950)
/** EFUSE_APB2OTP_BLOCK6_W11 : RO; bitpos: [31:0]; default: 0;
 *  Otp block6 word11 data.
 */
#define EFUSE_APB2OTP_BLOCK6_W11    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK6_W11_M  (EFUSE_APB2OTP_BLOCK6_W11_V << EFUSE_APB2OTP_BLOCK6_W11_S)
#define EFUSE_APB2OTP_BLOCK6_W11_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK6_W11_S  0

/** EFUSE_APB2OTP_BLK7_W1_REG register
 *  eFuse apb2otp block7 data register1.
 */
#define EFUSE_APB2OTP_BLK7_W1_REG (DR_REG_EFUSE_BASE + 0x954)
/** EFUSE_APB2OTP_BLOCK7_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block7 word1 data.
 */
#define EFUSE_APB2OTP_BLOCK7_W1    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK7_W1_M  (EFUSE_APB2OTP_BLOCK7_W1_V << EFUSE_APB2OTP_BLOCK7_W1_S)
#define EFUSE_APB2OTP_BLOCK7_W1_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK7_W1_S  0

/** EFUSE_APB2OTP_BLK7_W2_REG register
 *  eFuse apb2otp block7 data register2.
 */
#define EFUSE_APB2OTP_BLK7_W2_REG (DR_REG_EFUSE_BASE + 0x958)
/** EFUSE_APB2OTP_BLOCK7_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block7 word2 data.
 */
#define EFUSE_APB2OTP_BLOCK7_W2    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK7_W2_M  (EFUSE_APB2OTP_BLOCK7_W2_V << EFUSE_APB2OTP_BLOCK7_W2_S)
#define EFUSE_APB2OTP_BLOCK7_W2_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK7_W2_S  0

/** EFUSE_APB2OTP_BLK7_W3_REG register
 *  eFuse apb2otp block7 data register3.
 */
#define EFUSE_APB2OTP_BLK7_W3_REG (DR_REG_EFUSE_BASE + 0x95c)
/** EFUSE_APB2OTP_BLOCK7_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block7 word3 data.
 */
#define EFUSE_APB2OTP_BLOCK7_W3    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK7_W3_M  (EFUSE_APB2OTP_BLOCK7_W3_V << EFUSE_APB2OTP_BLOCK7_W3_S)
#define EFUSE_APB2OTP_BLOCK7_W3_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK7_W3_S  0

/** EFUSE_APB2OTP_BLK7_W4_REG register
 *  eFuse apb2otp block7 data register4.
 */
#define EFUSE_APB2OTP_BLK7_W4_REG (DR_REG_EFUSE_BASE + 0x960)
/** EFUSE_APB2OTP_BLOCK7_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block7 word4 data.
 */
#define EFUSE_APB2OTP_BLOCK7_W4    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK7_W4_M  (EFUSE_APB2OTP_BLOCK7_W4_V << EFUSE_APB2OTP_BLOCK7_W4_S)
#define EFUSE_APB2OTP_BLOCK7_W4_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK7_W4_S  0

/** EFUSE_APB2OTP_BLK7_W5_REG register
 *  eFuse apb2otp block7 data register5.
 */
#define EFUSE_APB2OTP_BLK7_W5_REG (DR_REG_EFUSE_BASE + 0x964)
/** EFUSE_APB2OTP_BLOCK7_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block7 word5 data.
 */
#define EFUSE_APB2OTP_BLOCK7_W5    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK7_W5_M  (EFUSE_APB2OTP_BLOCK7_W5_V << EFUSE_APB2OTP_BLOCK7_W5_S)
#define EFUSE_APB2OTP_BLOCK7_W5_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK7_W5_S  0

/** EFUSE_APB2OTP_BLK7_W6_REG register
 *  eFuse apb2otp block7 data register6.
 */
#define EFUSE_APB2OTP_BLK7_W6_REG (DR_REG_EFUSE_BASE + 0x968)
/** EFUSE_APB2OTP_BLOCK7_W6 : RO; bitpos: [31:0]; default: 0;
 *  Otp block7 word6 data.
 */
#define EFUSE_APB2OTP_BLOCK7_W6    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK7_W6_M  (EFUSE_APB2OTP_BLOCK7_W6_V << EFUSE_APB2OTP_BLOCK7_W6_S)
#define EFUSE_APB2OTP_BLOCK7_W6_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK7_W6_S  0

/** EFUSE_APB2OTP_BLK7_W7_REG register
 *  eFuse apb2otp block7 data register7.
 */
#define EFUSE_APB2OTP_BLK7_W7_REG (DR_REG_EFUSE_BASE + 0x96c)
/** EFUSE_APB2OTP_BLOCK7_W7 : RO; bitpos: [31:0]; default: 0;
 *  Otp block7 word7 data.
 */
#define EFUSE_APB2OTP_BLOCK7_W7    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK7_W7_M  (EFUSE_APB2OTP_BLOCK7_W7_V << EFUSE_APB2OTP_BLOCK7_W7_S)
#define EFUSE_APB2OTP_BLOCK7_W7_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK7_W7_S  0

/** EFUSE_APB2OTP_BLK7_W8_REG register
 *  eFuse apb2otp block7 data register8.
 */
#define EFUSE_APB2OTP_BLK7_W8_REG (DR_REG_EFUSE_BASE + 0x970)
/** EFUSE_APB2OTP_BLOCK7_W8 : RO; bitpos: [31:0]; default: 0;
 *  Otp block7 word8 data.
 */
#define EFUSE_APB2OTP_BLOCK7_W8    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK7_W8_M  (EFUSE_APB2OTP_BLOCK7_W8_V << EFUSE_APB2OTP_BLOCK7_W8_S)
#define EFUSE_APB2OTP_BLOCK7_W8_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK7_W8_S  0

/** EFUSE_APB2OTP_BLK7_W9_REG register
 *  eFuse apb2otp block7 data register9.
 */
#define EFUSE_APB2OTP_BLK7_W9_REG (DR_REG_EFUSE_BASE + 0x974)
/** EFUSE_APB2OTP_BLOCK7_W9 : RO; bitpos: [31:0]; default: 0;
 *  Otp block7 word9 data.
 */
#define EFUSE_APB2OTP_BLOCK7_W9    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK7_W9_M  (EFUSE_APB2OTP_BLOCK7_W9_V << EFUSE_APB2OTP_BLOCK7_W9_S)
#define EFUSE_APB2OTP_BLOCK7_W9_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK7_W9_S  0

/** EFUSE_APB2OTP_BLK7_W10_REG register
 *  eFuse apb2otp block7 data register10.
 */
#define EFUSE_APB2OTP_BLK7_W10_REG (DR_REG_EFUSE_BASE + 0x978)
/** EFUSE_APB2OTP_BLOCK7_W10 : RO; bitpos: [31:0]; default: 0;
 *  Otp block7 word10 data.
 */
#define EFUSE_APB2OTP_BLOCK7_W10    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK7_W10_M  (EFUSE_APB2OTP_BLOCK7_W10_V << EFUSE_APB2OTP_BLOCK7_W10_S)
#define EFUSE_APB2OTP_BLOCK7_W10_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK7_W10_S  0

/** EFUSE_APB2OTP_BLK7_W11_REG register
 *  eFuse apb2otp block7 data register11.
 */
#define EFUSE_APB2OTP_BLK7_W11_REG (DR_REG_EFUSE_BASE + 0x97c)
/** EFUSE_APB2OTP_BLOCK7_W11 : RO; bitpos: [31:0]; default: 0;
 *  Otp block7 word11 data.
 */
#define EFUSE_APB2OTP_BLOCK7_W11    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK7_W11_M  (EFUSE_APB2OTP_BLOCK7_W11_V << EFUSE_APB2OTP_BLOCK7_W11_S)
#define EFUSE_APB2OTP_BLOCK7_W11_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK7_W11_S  0

/** EFUSE_APB2OTP_BLK8_W1_REG register
 *  eFuse apb2otp block8 data register1.
 */
#define EFUSE_APB2OTP_BLK8_W1_REG (DR_REG_EFUSE_BASE + 0x980)
/** EFUSE_APB2OTP_BLOCK8_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block8 word1 data.
 */
#define EFUSE_APB2OTP_BLOCK8_W1    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK8_W1_M  (EFUSE_APB2OTP_BLOCK8_W1_V << EFUSE_APB2OTP_BLOCK8_W1_S)
#define EFUSE_APB2OTP_BLOCK8_W1_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK8_W1_S  0

/** EFUSE_APB2OTP_BLK8_W2_REG register
 *  eFuse apb2otp block8 data register2.
 */
#define EFUSE_APB2OTP_BLK8_W2_REG (DR_REG_EFUSE_BASE + 0x984)
/** EFUSE_APB2OTP_BLOCK8_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block8 word2 data.
 */
#define EFUSE_APB2OTP_BLOCK8_W2    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK8_W2_M  (EFUSE_APB2OTP_BLOCK8_W2_V << EFUSE_APB2OTP_BLOCK8_W2_S)
#define EFUSE_APB2OTP_BLOCK8_W2_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK8_W2_S  0

/** EFUSE_APB2OTP_BLK8_W3_REG register
 *  eFuse apb2otp block8 data register3.
 */
#define EFUSE_APB2OTP_BLK8_W3_REG (DR_REG_EFUSE_BASE + 0x988)
/** EFUSE_APB2OTP_BLOCK8_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block8 word3 data.
 */
#define EFUSE_APB2OTP_BLOCK8_W3    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK8_W3_M  (EFUSE_APB2OTP_BLOCK8_W3_V << EFUSE_APB2OTP_BLOCK8_W3_S)
#define EFUSE_APB2OTP_BLOCK8_W3_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK8_W3_S  0

/** EFUSE_APB2OTP_BLK8_W4_REG register
 *  eFuse apb2otp block8 data register4.
 */
#define EFUSE_APB2OTP_BLK8_W4_REG (DR_REG_EFUSE_BASE + 0x98c)
/** EFUSE_APB2OTP_BLOCK8_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block8 word4 data.
 */
#define EFUSE_APB2OTP_BLOCK8_W4    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK8_W4_M  (EFUSE_APB2OTP_BLOCK8_W4_V << EFUSE_APB2OTP_BLOCK8_W4_S)
#define EFUSE_APB2OTP_BLOCK8_W4_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK8_W4_S  0

/** EFUSE_APB2OTP_BLK8_W5_REG register
 *  eFuse apb2otp block8 data register5.
 */
#define EFUSE_APB2OTP_BLK8_W5_REG (DR_REG_EFUSE_BASE + 0x990)
/** EFUSE_APB2OTP_BLOCK8_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block8 word5 data.
 */
#define EFUSE_APB2OTP_BLOCK8_W5    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK8_W5_M  (EFUSE_APB2OTP_BLOCK8_W5_V << EFUSE_APB2OTP_BLOCK8_W5_S)
#define EFUSE_APB2OTP_BLOCK8_W5_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK8_W5_S  0

/** EFUSE_APB2OTP_BLK8_W6_REG register
 *  eFuse apb2otp block8 data register6.
 */
#define EFUSE_APB2OTP_BLK8_W6_REG (DR_REG_EFUSE_BASE + 0x994)
/** EFUSE_APB2OTP_BLOCK8_W6 : RO; bitpos: [31:0]; default: 0;
 *  Otp block8 word6 data.
 */
#define EFUSE_APB2OTP_BLOCK8_W6    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK8_W6_M  (EFUSE_APB2OTP_BLOCK8_W6_V << EFUSE_APB2OTP_BLOCK8_W6_S)
#define EFUSE_APB2OTP_BLOCK8_W6_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK8_W6_S  0

/** EFUSE_APB2OTP_BLK8_W7_REG register
 *  eFuse apb2otp block8 data register7.
 */
#define EFUSE_APB2OTP_BLK8_W7_REG (DR_REG_EFUSE_BASE + 0x998)
/** EFUSE_APB2OTP_BLOCK8_W7 : RO; bitpos: [31:0]; default: 0;
 *  Otp block8 word7 data.
 */
#define EFUSE_APB2OTP_BLOCK8_W7    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK8_W7_M  (EFUSE_APB2OTP_BLOCK8_W7_V << EFUSE_APB2OTP_BLOCK8_W7_S)
#define EFUSE_APB2OTP_BLOCK8_W7_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK8_W7_S  0

/** EFUSE_APB2OTP_BLK8_W8_REG register
 *  eFuse apb2otp block8 data register8.
 */
#define EFUSE_APB2OTP_BLK8_W8_REG (DR_REG_EFUSE_BASE + 0x99c)
/** EFUSE_APB2OTP_BLOCK8_W8 : RO; bitpos: [31:0]; default: 0;
 *  Otp block8 word8 data.
 */
#define EFUSE_APB2OTP_BLOCK8_W8    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK8_W8_M  (EFUSE_APB2OTP_BLOCK8_W8_V << EFUSE_APB2OTP_BLOCK8_W8_S)
#define EFUSE_APB2OTP_BLOCK8_W8_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK8_W8_S  0

/** EFUSE_APB2OTP_BLK8_W9_REG register
 *  eFuse apb2otp block8 data register9.
 */
#define EFUSE_APB2OTP_BLK8_W9_REG (DR_REG_EFUSE_BASE + 0x9a0)
/** EFUSE_APB2OTP_BLOCK8_W9 : RO; bitpos: [31:0]; default: 0;
 *  Otp block8 word9 data.
 */
#define EFUSE_APB2OTP_BLOCK8_W9    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK8_W9_M  (EFUSE_APB2OTP_BLOCK8_W9_V << EFUSE_APB2OTP_BLOCK8_W9_S)
#define EFUSE_APB2OTP_BLOCK8_W9_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK8_W9_S  0

/** EFUSE_APB2OTP_BLK8_W10_REG register
 *  eFuse apb2otp block8 data register10.
 */
#define EFUSE_APB2OTP_BLK8_W10_REG (DR_REG_EFUSE_BASE + 0x9a4)
/** EFUSE_APB2OTP_BLOCK8_W10 : RO; bitpos: [31:0]; default: 0;
 *  Otp block8 word10 data.
 */
#define EFUSE_APB2OTP_BLOCK8_W10    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK8_W10_M  (EFUSE_APB2OTP_BLOCK8_W10_V << EFUSE_APB2OTP_BLOCK8_W10_S)
#define EFUSE_APB2OTP_BLOCK8_W10_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK8_W10_S  0

/** EFUSE_APB2OTP_BLK8_W11_REG register
 *  eFuse apb2otp block8 data register11.
 */
#define EFUSE_APB2OTP_BLK8_W11_REG (DR_REG_EFUSE_BASE + 0x9a8)
/** EFUSE_APB2OTP_BLOCK8_W11 : RO; bitpos: [31:0]; default: 0;
 *  Otp block8 word11 data.
 */
#define EFUSE_APB2OTP_BLOCK8_W11    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK8_W11_M  (EFUSE_APB2OTP_BLOCK8_W11_V << EFUSE_APB2OTP_BLOCK8_W11_S)
#define EFUSE_APB2OTP_BLOCK8_W11_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK8_W11_S  0

/** EFUSE_APB2OTP_BLK9_W1_REG register
 *  eFuse apb2otp block9 data register1.
 */
#define EFUSE_APB2OTP_BLK9_W1_REG (DR_REG_EFUSE_BASE + 0x9ac)
/** EFUSE_APB2OTP_BLOCK9_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block9 word1 data.
 */
#define EFUSE_APB2OTP_BLOCK9_W1    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK9_W1_M  (EFUSE_APB2OTP_BLOCK9_W1_V << EFUSE_APB2OTP_BLOCK9_W1_S)
#define EFUSE_APB2OTP_BLOCK9_W1_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK9_W1_S  0

/** EFUSE_APB2OTP_BLK9_W2_REG register
 *  eFuse apb2otp block9 data register2.
 */
#define EFUSE_APB2OTP_BLK9_W2_REG (DR_REG_EFUSE_BASE + 0x9b0)
/** EFUSE_APB2OTP_BLOCK9_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block9 word2 data.
 */
#define EFUSE_APB2OTP_BLOCK9_W2    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK9_W2_M  (EFUSE_APB2OTP_BLOCK9_W2_V << EFUSE_APB2OTP_BLOCK9_W2_S)
#define EFUSE_APB2OTP_BLOCK9_W2_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK9_W2_S  0

/** EFUSE_APB2OTP_BLK9_W3_REG register
 *  eFuse apb2otp block9 data register3.
 */
#define EFUSE_APB2OTP_BLK9_W3_REG (DR_REG_EFUSE_BASE + 0x9b4)
/** EFUSE_APB2OTP_BLOCK9_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block9 word3 data.
 */
#define EFUSE_APB2OTP_BLOCK9_W3    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK9_W3_M  (EFUSE_APB2OTP_BLOCK9_W3_V << EFUSE_APB2OTP_BLOCK9_W3_S)
#define EFUSE_APB2OTP_BLOCK9_W3_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK9_W3_S  0

/** EFUSE_APB2OTP_BLK9_W4_REG register
 *  eFuse apb2otp block9 data register4.
 */
#define EFUSE_APB2OTP_BLK9_W4_REG (DR_REG_EFUSE_BASE + 0x9b8)
/** EFUSE_APB2OTP_BLOCK9_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block9 word4 data.
 */
#define EFUSE_APB2OTP_BLOCK9_W4    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK9_W4_M  (EFUSE_APB2OTP_BLOCK9_W4_V << EFUSE_APB2OTP_BLOCK9_W4_S)
#define EFUSE_APB2OTP_BLOCK9_W4_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK9_W4_S  0

/** EFUSE_APB2OTP_BLK9_W5_REG register
 *  eFuse apb2otp block9 data register5.
 */
#define EFUSE_APB2OTP_BLK9_W5_REG (DR_REG_EFUSE_BASE + 0x9bc)
/** EFUSE_APB2OTP_BLOCK9_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block9 word5 data.
 */
#define EFUSE_APB2OTP_BLOCK9_W5    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK9_W5_M  (EFUSE_APB2OTP_BLOCK9_W5_V << EFUSE_APB2OTP_BLOCK9_W5_S)
#define EFUSE_APB2OTP_BLOCK9_W5_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK9_W5_S  0

/** EFUSE_APB2OTP_BLK9_W6_REG register
 *  eFuse apb2otp block9 data register6.
 */
#define EFUSE_APB2OTP_BLK9_W6_REG (DR_REG_EFUSE_BASE + 0x9c0)
/** EFUSE_APB2OTP_BLOCK9_W6 : RO; bitpos: [31:0]; default: 0;
 *  Otp block9 word6 data.
 */
#define EFUSE_APB2OTP_BLOCK9_W6    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK9_W6_M  (EFUSE_APB2OTP_BLOCK9_W6_V << EFUSE_APB2OTP_BLOCK9_W6_S)
#define EFUSE_APB2OTP_BLOCK9_W6_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK9_W6_S  0

/** EFUSE_APB2OTP_BLK9_W7_REG register
 *  eFuse apb2otp block9 data register7.
 */
#define EFUSE_APB2OTP_BLK9_W7_REG (DR_REG_EFUSE_BASE + 0x9c4)
/** EFUSE_APB2OTP_BLOCK9_W7 : RO; bitpos: [31:0]; default: 0;
 *  Otp block9 word7 data.
 */
#define EFUSE_APB2OTP_BLOCK9_W7    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK9_W7_M  (EFUSE_APB2OTP_BLOCK9_W7_V << EFUSE_APB2OTP_BLOCK9_W7_S)
#define EFUSE_APB2OTP_BLOCK9_W7_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK9_W7_S  0

/** EFUSE_APB2OTP_BLK9_W8_REG register
 *  eFuse apb2otp block9 data register8.
 */
#define EFUSE_APB2OTP_BLK9_W8_REG (DR_REG_EFUSE_BASE + 0x9c8)
/** EFUSE_APB2OTP_BLOCK9_W8 : RO; bitpos: [31:0]; default: 0;
 *  Otp block9 word8 data.
 */
#define EFUSE_APB2OTP_BLOCK9_W8    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK9_W8_M  (EFUSE_APB2OTP_BLOCK9_W8_V << EFUSE_APB2OTP_BLOCK9_W8_S)
#define EFUSE_APB2OTP_BLOCK9_W8_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK9_W8_S  0

/** EFUSE_APB2OTP_BLK9_W9_REG register
 *  eFuse apb2otp block9 data register9.
 */
#define EFUSE_APB2OTP_BLK9_W9_REG (DR_REG_EFUSE_BASE + 0x9cc)
/** EFUSE_APB2OTP_BLOCK9_W9 : RO; bitpos: [31:0]; default: 0;
 *  Otp block9 word9 data.
 */
#define EFUSE_APB2OTP_BLOCK9_W9    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK9_W9_M  (EFUSE_APB2OTP_BLOCK9_W9_V << EFUSE_APB2OTP_BLOCK9_W9_S)
#define EFUSE_APB2OTP_BLOCK9_W9_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK9_W9_S  0

/** EFUSE_APB2OTP_BLK9_W10_REG register
 *  eFuse apb2otp block9 data register10.
 */
#define EFUSE_APB2OTP_BLK9_W10_REG (DR_REG_EFUSE_BASE + 0x9d0)
/** EFUSE_APB2OTP_BLOCK9_W10 : RO; bitpos: [31:0]; default: 0;
 *  Otp block9 word10 data.
 */
#define EFUSE_APB2OTP_BLOCK9_W10    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK9_W10_M  (EFUSE_APB2OTP_BLOCK9_W10_V << EFUSE_APB2OTP_BLOCK9_W10_S)
#define EFUSE_APB2OTP_BLOCK9_W10_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK9_W10_S  0

/** EFUSE_APB2OTP_BLK9_W11_REG register
 *  eFuse apb2otp block9 data register11.
 */
#define EFUSE_APB2OTP_BLK9_W11_REG (DR_REG_EFUSE_BASE + 0x9d4)
/** EFUSE_APB2OTP_BLOCK9_W11 : RO; bitpos: [31:0]; default: 0;
 *  Otp block9 word11 data.
 */
#define EFUSE_APB2OTP_BLOCK9_W11    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK9_W11_M  (EFUSE_APB2OTP_BLOCK9_W11_V << EFUSE_APB2OTP_BLOCK9_W11_S)
#define EFUSE_APB2OTP_BLOCK9_W11_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK9_W11_S  0

/** EFUSE_APB2OTP_BLK10_W1_REG register
 *  eFuse apb2otp block10 data register1.
 */
#define EFUSE_APB2OTP_BLK10_W1_REG (DR_REG_EFUSE_BASE + 0x9d8)
/** EFUSE_APB2OTP_BLOCK10_W1 : RO; bitpos: [31:0]; default: 0;
 *  Otp block10 word1 data.
 */
#define EFUSE_APB2OTP_BLOCK10_W1    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK10_W1_M  (EFUSE_APB2OTP_BLOCK10_W1_V << EFUSE_APB2OTP_BLOCK10_W1_S)
#define EFUSE_APB2OTP_BLOCK10_W1_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK10_W1_S  0

/** EFUSE_APB2OTP_BLK10_W2_REG register
 *  eFuse apb2otp block10 data register2.
 */
#define EFUSE_APB2OTP_BLK10_W2_REG (DR_REG_EFUSE_BASE + 0x9dc)
/** EFUSE_APB2OTP_BLOCK10_W2 : RO; bitpos: [31:0]; default: 0;
 *  Otp block10 word2 data.
 */
#define EFUSE_APB2OTP_BLOCK10_W2    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK10_W2_M  (EFUSE_APB2OTP_BLOCK10_W2_V << EFUSE_APB2OTP_BLOCK10_W2_S)
#define EFUSE_APB2OTP_BLOCK10_W2_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK10_W2_S  0

/** EFUSE_APB2OTP_BLK10_W3_REG register
 *  eFuse apb2otp block10 data register3.
 */
#define EFUSE_APB2OTP_BLK10_W3_REG (DR_REG_EFUSE_BASE + 0x9e0)
/** EFUSE_APB2OTP_BLOCK10_W3 : RO; bitpos: [31:0]; default: 0;
 *  Otp block10 word3 data.
 */
#define EFUSE_APB2OTP_BLOCK10_W3    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK10_W3_M  (EFUSE_APB2OTP_BLOCK10_W3_V << EFUSE_APB2OTP_BLOCK10_W3_S)
#define EFUSE_APB2OTP_BLOCK10_W3_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK10_W3_S  0

/** EFUSE_APB2OTP_BLK10_W4_REG register
 *  eFuse apb2otp block10 data register4.
 */
#define EFUSE_APB2OTP_BLK10_W4_REG (DR_REG_EFUSE_BASE + 0x9e4)
/** EFUSE_APB2OTP_BLOCK10_W4 : RO; bitpos: [31:0]; default: 0;
 *  Otp block10 word4 data.
 */
#define EFUSE_APB2OTP_BLOCK10_W4    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK10_W4_M  (EFUSE_APB2OTP_BLOCK10_W4_V << EFUSE_APB2OTP_BLOCK10_W4_S)
#define EFUSE_APB2OTP_BLOCK10_W4_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK10_W4_S  0

/** EFUSE_APB2OTP_BLK10_W5_REG register
 *  eFuse apb2otp block10 data register5.
 */
#define EFUSE_APB2OTP_BLK10_W5_REG (DR_REG_EFUSE_BASE + 0x9e8)
/** EFUSE_APB2OTP_BLOCK10_W5 : RO; bitpos: [31:0]; default: 0;
 *  Otp block10 word5 data.
 */
#define EFUSE_APB2OTP_BLOCK10_W5    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK10_W5_M  (EFUSE_APB2OTP_BLOCK10_W5_V << EFUSE_APB2OTP_BLOCK10_W5_S)
#define EFUSE_APB2OTP_BLOCK10_W5_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK10_W5_S  0

/** EFUSE_APB2OTP_BLK10_W6_REG register
 *  eFuse apb2otp block10 data register6.
 */
#define EFUSE_APB2OTP_BLK10_W6_REG (DR_REG_EFUSE_BASE + 0x9ec)
/** EFUSE_APB2OTP_BLOCK10_W6 : RO; bitpos: [31:0]; default: 0;
 *  Otp block10 word6 data.
 */
#define EFUSE_APB2OTP_BLOCK10_W6    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK10_W6_M  (EFUSE_APB2OTP_BLOCK10_W6_V << EFUSE_APB2OTP_BLOCK10_W6_S)
#define EFUSE_APB2OTP_BLOCK10_W6_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK10_W6_S  0

/** EFUSE_APB2OTP_BLK10_W7_REG register
 *  eFuse apb2otp block10 data register7.
 */
#define EFUSE_APB2OTP_BLK10_W7_REG (DR_REG_EFUSE_BASE + 0x9f0)
/** EFUSE_APB2OTP_BLOCK10_W7 : RO; bitpos: [31:0]; default: 0;
 *  Otp block10 word7 data.
 */
#define EFUSE_APB2OTP_BLOCK10_W7    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK10_W7_M  (EFUSE_APB2OTP_BLOCK10_W7_V << EFUSE_APB2OTP_BLOCK10_W7_S)
#define EFUSE_APB2OTP_BLOCK10_W7_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK10_W7_S  0

/** EFUSE_APB2OTP_BLK10_W8_REG register
 *  eFuse apb2otp block10 data register8.
 */
#define EFUSE_APB2OTP_BLK10_W8_REG (DR_REG_EFUSE_BASE + 0x9f4)
/** EFUSE_APB2OTP_BLOCK10_W8 : RO; bitpos: [31:0]; default: 0;
 *  Otp block10 word8 data.
 */
#define EFUSE_APB2OTP_BLOCK10_W8    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK10_W8_M  (EFUSE_APB2OTP_BLOCK10_W8_V << EFUSE_APB2OTP_BLOCK10_W8_S)
#define EFUSE_APB2OTP_BLOCK10_W8_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK10_W8_S  0

/** EFUSE_APB2OTP_BLK10_W9_REG register
 *  eFuse apb2otp block10 data register9.
 */
#define EFUSE_APB2OTP_BLK10_W9_REG (DR_REG_EFUSE_BASE + 0x9f8)
/** EFUSE_APB2OTP_BLOCK10_W9 : RO; bitpos: [31:0]; default: 0;
 *  Otp block10 word9 data.
 */
#define EFUSE_APB2OTP_BLOCK10_W9    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK10_W9_M  (EFUSE_APB2OTP_BLOCK10_W9_V << EFUSE_APB2OTP_BLOCK10_W9_S)
#define EFUSE_APB2OTP_BLOCK10_W9_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK10_W9_S  0

/** EFUSE_APB2OTP_BLK10_W10_REG register
 *  eFuse apb2otp block10 data register10.
 */
#define EFUSE_APB2OTP_BLK10_W10_REG (DR_REG_EFUSE_BASE + 0x9fc)
/** EFUSE_APB2OTP_BLOCK19_W10 : RO; bitpos: [31:0]; default: 0;
 *  Otp block10 word10 data.
 */
#define EFUSE_APB2OTP_BLOCK19_W10    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK19_W10_M  (EFUSE_APB2OTP_BLOCK19_W10_V << EFUSE_APB2OTP_BLOCK19_W10_S)
#define EFUSE_APB2OTP_BLOCK19_W10_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK19_W10_S  0

/** EFUSE_APB2OTP_BLK10_W11_REG register
 *  eFuse apb2otp block10 data register11.
 */
#define EFUSE_APB2OTP_BLK10_W11_REG (DR_REG_EFUSE_BASE + 0xa00)
/** EFUSE_APB2OTP_BLOCK10_W11 : RO; bitpos: [31:0]; default: 0;
 *  Otp block10 word11 data.
 */
#define EFUSE_APB2OTP_BLOCK10_W11    0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK10_W11_M  (EFUSE_APB2OTP_BLOCK10_W11_V << EFUSE_APB2OTP_BLOCK10_W11_S)
#define EFUSE_APB2OTP_BLOCK10_W11_V  0xFFFFFFFFU
#define EFUSE_APB2OTP_BLOCK10_W11_S  0

/** EFUSE_APB2OTP_EN_REG register
 *  eFuse apb2otp enable configuration register.
 */
#define EFUSE_APB2OTP_EN_REG (DR_REG_EFUSE_BASE + 0xa08)
/** EFUSE_APB2OTP_APB2OTP_EN : R/W; bitpos: [0]; default: 0;
 *  Apb2otp mode enable signal.
 */
#define EFUSE_APB2OTP_APB2OTP_EN    (BIT(0))
#define EFUSE_APB2OTP_APB2OTP_EN_M  (EFUSE_APB2OTP_APB2OTP_EN_V << EFUSE_APB2OTP_APB2OTP_EN_S)
#define EFUSE_APB2OTP_APB2OTP_EN_V  0x00000001U
#define EFUSE_APB2OTP_APB2OTP_EN_S  0

#ifdef __cplusplus
}
#endif
