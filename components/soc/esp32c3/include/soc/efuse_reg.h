/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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
/** EFUSE_WR_DIS : RO; bitpos: [31:0]; default: 0;
 *  Disable programming of individual eFuses.
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
 *  Set this bit to disable reading from BlOCK4-10.
 */
#define EFUSE_RD_DIS    0x0000007FU
#define EFUSE_RD_DIS_M  (EFUSE_RD_DIS_V << EFUSE_RD_DIS_S)
#define EFUSE_RD_DIS_V  0x0000007FU
#define EFUSE_RD_DIS_S  0
/** EFUSE_DIS_RTC_RAM_BOOT : RO; bitpos: [7]; default: 0;
 *  Set this bit to disable boot from RTC RAM.
 */
#define EFUSE_DIS_RTC_RAM_BOOT    (BIT(7))
#define EFUSE_DIS_RTC_RAM_BOOT_M  (EFUSE_DIS_RTC_RAM_BOOT_V << EFUSE_DIS_RTC_RAM_BOOT_S)
#define EFUSE_DIS_RTC_RAM_BOOT_V  0x00000001U
#define EFUSE_DIS_RTC_RAM_BOOT_S  7
/** EFUSE_DIS_ICACHE : RO; bitpos: [8]; default: 0;
 *  Set this bit to disable Icache.
 */
#define EFUSE_DIS_ICACHE    (BIT(8))
#define EFUSE_DIS_ICACHE_M  (EFUSE_DIS_ICACHE_V << EFUSE_DIS_ICACHE_S)
#define EFUSE_DIS_ICACHE_V  0x00000001U
#define EFUSE_DIS_ICACHE_S  8
/** EFUSE_DIS_USB_JTAG : RO; bitpos: [9]; default: 0;
 *  Set this bit to disable function of usb switch to jtag in module of usb device.
 */
#define EFUSE_DIS_USB_JTAG    (BIT(9))
#define EFUSE_DIS_USB_JTAG_M  (EFUSE_DIS_USB_JTAG_V << EFUSE_DIS_USB_JTAG_S)
#define EFUSE_DIS_USB_JTAG_V  0x00000001U
#define EFUSE_DIS_USB_JTAG_S  9
/** EFUSE_DIS_DOWNLOAD_ICACHE : RO; bitpos: [10]; default: 0;
 *  Set this bit to disable Icache in download mode (boot_mode[3:0] is 0, 1, 2, 3, 6,
 *  7).
 */
#define EFUSE_DIS_DOWNLOAD_ICACHE    (BIT(10))
#define EFUSE_DIS_DOWNLOAD_ICACHE_M  (EFUSE_DIS_DOWNLOAD_ICACHE_V << EFUSE_DIS_DOWNLOAD_ICACHE_S)
#define EFUSE_DIS_DOWNLOAD_ICACHE_V  0x00000001U
#define EFUSE_DIS_DOWNLOAD_ICACHE_S  10
/** EFUSE_DIS_USB_SERIAL_JTAG : RO; bitpos: [11]; default: 0;
 *  Represents whether USB-Serial-JTAG is disabled. 1: Disabled. 0: Enabled
 */
#define EFUSE_DIS_USB_SERIAL_JTAG    (BIT(11))
#define EFUSE_DIS_USB_SERIAL_JTAG_M  (EFUSE_DIS_USB_SERIAL_JTAG_V << EFUSE_DIS_USB_SERIAL_JTAG_S)
#define EFUSE_DIS_USB_SERIAL_JTAG_V  0x00000001U
#define EFUSE_DIS_USB_SERIAL_JTAG_S  11
/** EFUSE_DIS_FORCE_DOWNLOAD : RO; bitpos: [12]; default: 0;
 *  Set this bit to disable the function that forces chip into download mode.
 */
#define EFUSE_DIS_FORCE_DOWNLOAD    (BIT(12))
#define EFUSE_DIS_FORCE_DOWNLOAD_M  (EFUSE_DIS_FORCE_DOWNLOAD_V << EFUSE_DIS_FORCE_DOWNLOAD_S)
#define EFUSE_DIS_FORCE_DOWNLOAD_V  0x00000001U
#define EFUSE_DIS_FORCE_DOWNLOAD_S  12
/** EFUSE_RPT4_RESERVED6 : RO; bitpos: [13]; default: 0;
 *  Reserved (used for four backups method).
 */
#define EFUSE_RPT4_RESERVED6    (BIT(13))
#define EFUSE_RPT4_RESERVED6_M  (EFUSE_RPT4_RESERVED6_V << EFUSE_RPT4_RESERVED6_S)
#define EFUSE_RPT4_RESERVED6_V  0x00000001U
#define EFUSE_RPT4_RESERVED6_S  13
/** EFUSE_DIS_TWAI : RO; bitpos: [14]; default: 0;
 *  Set this bit to disable CAN function.
 */
#define EFUSE_DIS_TWAI    (BIT(14))
#define EFUSE_DIS_TWAI_M  (EFUSE_DIS_TWAI_V << EFUSE_DIS_TWAI_S)
#define EFUSE_DIS_TWAI_V  0x00000001U
#define EFUSE_DIS_TWAI_S  14
/** EFUSE_JTAG_SEL_ENABLE : RO; bitpos: [15]; default: 0;
 *  Set this bit to enable selection between usb_to_jtag and pad_to_jtag through
 *  strapping gpio10 when both reg_dis_usb_jtag and reg_dis_pad_jtag are equal to 0.
 */
#define EFUSE_JTAG_SEL_ENABLE    (BIT(15))
#define EFUSE_JTAG_SEL_ENABLE_M  (EFUSE_JTAG_SEL_ENABLE_V << EFUSE_JTAG_SEL_ENABLE_S)
#define EFUSE_JTAG_SEL_ENABLE_V  0x00000001U
#define EFUSE_JTAG_SEL_ENABLE_S  15
/** EFUSE_SOFT_DIS_JTAG : RO; bitpos: [18:16]; default: 0;
 *  Set these bits to disable JTAG in the soft way (odd number 1 means disable ). JTAG
 *  can be enabled in HMAC module.
 */
#define EFUSE_SOFT_DIS_JTAG    0x00000007U
#define EFUSE_SOFT_DIS_JTAG_M  (EFUSE_SOFT_DIS_JTAG_V << EFUSE_SOFT_DIS_JTAG_S)
#define EFUSE_SOFT_DIS_JTAG_V  0x00000007U
#define EFUSE_SOFT_DIS_JTAG_S  16
/** EFUSE_DIS_PAD_JTAG : RO; bitpos: [19]; default: 0;
 *  Set this bit to disable JTAG in the hard way. JTAG is disabled permanently.
 */
#define EFUSE_DIS_PAD_JTAG    (BIT(19))
#define EFUSE_DIS_PAD_JTAG_M  (EFUSE_DIS_PAD_JTAG_V << EFUSE_DIS_PAD_JTAG_S)
#define EFUSE_DIS_PAD_JTAG_V  0x00000001U
#define EFUSE_DIS_PAD_JTAG_S  19
/** EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT : RO; bitpos: [20]; default: 0;
 *  Set this bit to disable flash encryption when in download boot modes.
 */
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT    (BIT(20))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_M  (EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_V << EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_S)
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_V  0x00000001U
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_S  20
/** EFUSE_USB_DREFH : RO; bitpos: [22:21]; default: 0;
 *  Controls single-end input threshold vrefh, 1.76 V to 2 V with step of 80 mV, stored
 *  in eFuse.
 */
#define EFUSE_USB_DREFH    0x00000003U
#define EFUSE_USB_DREFH_M  (EFUSE_USB_DREFH_V << EFUSE_USB_DREFH_S)
#define EFUSE_USB_DREFH_V  0x00000003U
#define EFUSE_USB_DREFH_S  21
/** EFUSE_USB_DREFL : RO; bitpos: [24:23]; default: 0;
 *  Controls single-end input threshold vrefl, 0.8 V to 1.04 V with step of 80 mV,
 *  stored in eFuse.
 */
#define EFUSE_USB_DREFL    0x00000003U
#define EFUSE_USB_DREFL_M  (EFUSE_USB_DREFL_V << EFUSE_USB_DREFL_S)
#define EFUSE_USB_DREFL_V  0x00000003U
#define EFUSE_USB_DREFL_S  23
/** EFUSE_USB_EXCHG_PINS : RO; bitpos: [25]; default: 0;
 *  Set this bit to exchange USB D+ and D- pins.
 */
#define EFUSE_USB_EXCHG_PINS    (BIT(25))
#define EFUSE_USB_EXCHG_PINS_M  (EFUSE_USB_EXCHG_PINS_V << EFUSE_USB_EXCHG_PINS_S)
#define EFUSE_USB_EXCHG_PINS_V  0x00000001U
#define EFUSE_USB_EXCHG_PINS_S  25
/** EFUSE_VDD_SPI_AS_GPIO : RO; bitpos: [26]; default: 0;
 *  Set this bit to vdd spi pin function as gpio.
 */
#define EFUSE_VDD_SPI_AS_GPIO    (BIT(26))
#define EFUSE_VDD_SPI_AS_GPIO_M  (EFUSE_VDD_SPI_AS_GPIO_V << EFUSE_VDD_SPI_AS_GPIO_S)
#define EFUSE_VDD_SPI_AS_GPIO_V  0x00000001U
#define EFUSE_VDD_SPI_AS_GPIO_S  26
/** EFUSE_BTLC_GPIO_ENABLE : RO; bitpos: [28:27]; default: 0;
 *  Enable btlc gpio.
 */
#define EFUSE_BTLC_GPIO_ENABLE    0x00000003U
#define EFUSE_BTLC_GPIO_ENABLE_M  (EFUSE_BTLC_GPIO_ENABLE_V << EFUSE_BTLC_GPIO_ENABLE_S)
#define EFUSE_BTLC_GPIO_ENABLE_V  0x00000003U
#define EFUSE_BTLC_GPIO_ENABLE_S  27
/** EFUSE_POWERGLITCH_EN : RO; bitpos: [29]; default: 0;
 *  Set this bit to enable power glitch function.
 */
#define EFUSE_POWERGLITCH_EN    (BIT(29))
#define EFUSE_POWERGLITCH_EN_M  (EFUSE_POWERGLITCH_EN_V << EFUSE_POWERGLITCH_EN_S)
#define EFUSE_POWERGLITCH_EN_V  0x00000001U
#define EFUSE_POWERGLITCH_EN_S  29
/** EFUSE_POWER_GLITCH_DSENSE : RO; bitpos: [31:30]; default: 0;
 *  Sample delay configuration of power glitch.
 */
#define EFUSE_POWER_GLITCH_DSENSE    0x00000003U
#define EFUSE_POWER_GLITCH_DSENSE_M  (EFUSE_POWER_GLITCH_DSENSE_V << EFUSE_POWER_GLITCH_DSENSE_S)
#define EFUSE_POWER_GLITCH_DSENSE_V  0x00000003U
#define EFUSE_POWER_GLITCH_DSENSE_S  30

/** EFUSE_RD_REPEAT_DATA1_REG register
 *  BLOCK0 data register 2.
 */
#define EFUSE_RD_REPEAT_DATA1_REG (DR_REG_EFUSE_BASE + 0x34)
/** EFUSE_RPT4_RESERVED2 : RO; bitpos: [15:0]; default: 0;
 *  Reserved (used for four backups method).
 */
#define EFUSE_RPT4_RESERVED2    0x0000FFFFU
#define EFUSE_RPT4_RESERVED2_M  (EFUSE_RPT4_RESERVED2_V << EFUSE_RPT4_RESERVED2_S)
#define EFUSE_RPT4_RESERVED2_V  0x0000FFFFU
#define EFUSE_RPT4_RESERVED2_S  0
/** EFUSE_WDT_DELAY_SEL : RO; bitpos: [17:16]; default: 0;
 *  Selects RTC watchdog timeout threshold, in unit of slow clock cycle. 0: 40000. 1:
 *  80000. 2: 160000. 3:320000.
 */
#define EFUSE_WDT_DELAY_SEL    0x00000003U
#define EFUSE_WDT_DELAY_SEL_M  (EFUSE_WDT_DELAY_SEL_V << EFUSE_WDT_DELAY_SEL_S)
#define EFUSE_WDT_DELAY_SEL_V  0x00000003U
#define EFUSE_WDT_DELAY_SEL_S  16
/** EFUSE_SPI_BOOT_CRYPT_CNT : RO; bitpos: [20:18]; default: 0;
 *  Set this bit to enable SPI boot encrypt/decrypt. Odd number of 1: enable. even
 *  number of 1: disable.
 */
#define EFUSE_SPI_BOOT_CRYPT_CNT    0x00000007U
#define EFUSE_SPI_BOOT_CRYPT_CNT_M  (EFUSE_SPI_BOOT_CRYPT_CNT_V << EFUSE_SPI_BOOT_CRYPT_CNT_S)
#define EFUSE_SPI_BOOT_CRYPT_CNT_V  0x00000007U
#define EFUSE_SPI_BOOT_CRYPT_CNT_S  18
/** EFUSE_SECURE_BOOT_KEY_REVOKE0 : RO; bitpos: [21]; default: 0;
 *  Set this bit to enable revoking first secure boot key.
 */
#define EFUSE_SECURE_BOOT_KEY_REVOKE0    (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_M  (EFUSE_SECURE_BOOT_KEY_REVOKE0_V << EFUSE_SECURE_BOOT_KEY_REVOKE0_S)
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_V  0x00000001U
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_S  21
/** EFUSE_SECURE_BOOT_KEY_REVOKE1 : RO; bitpos: [22]; default: 0;
 *  Set this bit to enable revoking second secure boot key.
 */
#define EFUSE_SECURE_BOOT_KEY_REVOKE1    (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_M  (EFUSE_SECURE_BOOT_KEY_REVOKE1_V << EFUSE_SECURE_BOOT_KEY_REVOKE1_S)
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_V  0x00000001U
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_S  22
/** EFUSE_SECURE_BOOT_KEY_REVOKE2 : RO; bitpos: [23]; default: 0;
 *  Set this bit to enable revoking third secure boot key.
 */
#define EFUSE_SECURE_BOOT_KEY_REVOKE2    (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_M  (EFUSE_SECURE_BOOT_KEY_REVOKE2_V << EFUSE_SECURE_BOOT_KEY_REVOKE2_S)
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_V  0x00000001U
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_S  23
/** EFUSE_KEY_PURPOSE_0 : RO; bitpos: [27:24]; default: 0;
 *  Purpose of Key0.
 */
#define EFUSE_KEY_PURPOSE_0    0x0000000FU
#define EFUSE_KEY_PURPOSE_0_M  (EFUSE_KEY_PURPOSE_0_V << EFUSE_KEY_PURPOSE_0_S)
#define EFUSE_KEY_PURPOSE_0_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_0_S  24
/** EFUSE_KEY_PURPOSE_1 : RO; bitpos: [31:28]; default: 0;
 *  Purpose of Key1.
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
 *  Purpose of Key2.
 */
#define EFUSE_KEY_PURPOSE_2    0x0000000FU
#define EFUSE_KEY_PURPOSE_2_M  (EFUSE_KEY_PURPOSE_2_V << EFUSE_KEY_PURPOSE_2_S)
#define EFUSE_KEY_PURPOSE_2_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_2_S  0
/** EFUSE_KEY_PURPOSE_3 : RO; bitpos: [7:4]; default: 0;
 *  Purpose of Key3.
 */
#define EFUSE_KEY_PURPOSE_3    0x0000000FU
#define EFUSE_KEY_PURPOSE_3_M  (EFUSE_KEY_PURPOSE_3_V << EFUSE_KEY_PURPOSE_3_S)
#define EFUSE_KEY_PURPOSE_3_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_3_S  4
/** EFUSE_KEY_PURPOSE_4 : RO; bitpos: [11:8]; default: 0;
 *  Purpose of Key4.
 */
#define EFUSE_KEY_PURPOSE_4    0x0000000FU
#define EFUSE_KEY_PURPOSE_4_M  (EFUSE_KEY_PURPOSE_4_V << EFUSE_KEY_PURPOSE_4_S)
#define EFUSE_KEY_PURPOSE_4_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_4_S  8
/** EFUSE_KEY_PURPOSE_5 : RO; bitpos: [15:12]; default: 0;
 *  Purpose of Key5.
 */
#define EFUSE_KEY_PURPOSE_5    0x0000000FU
#define EFUSE_KEY_PURPOSE_5_M  (EFUSE_KEY_PURPOSE_5_V << EFUSE_KEY_PURPOSE_5_S)
#define EFUSE_KEY_PURPOSE_5_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_5_S  12
/** EFUSE_RPT4_RESERVED3 : RO; bitpos: [19:16]; default: 0;
 *  Reserved (used for four backups method).
 */
#define EFUSE_RPT4_RESERVED3    0x0000000FU
#define EFUSE_RPT4_RESERVED3_M  (EFUSE_RPT4_RESERVED3_V << EFUSE_RPT4_RESERVED3_S)
#define EFUSE_RPT4_RESERVED3_V  0x0000000FU
#define EFUSE_RPT4_RESERVED3_S  16
/** EFUSE_SECURE_BOOT_EN : RO; bitpos: [20]; default: 0;
 *  Set this bit to enable secure boot.
 */
#define EFUSE_SECURE_BOOT_EN    (BIT(20))
#define EFUSE_SECURE_BOOT_EN_M  (EFUSE_SECURE_BOOT_EN_V << EFUSE_SECURE_BOOT_EN_S)
#define EFUSE_SECURE_BOOT_EN_V  0x00000001U
#define EFUSE_SECURE_BOOT_EN_S  20
/** EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE : RO; bitpos: [21]; default: 0;
 *  Set this bit to enable revoking aggressive secure boot.
 */
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE    (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_M  (EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_V << EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_S)
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_V  0x00000001U
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_S  21
/** EFUSE_RPT4_RESERVED0 : RO; bitpos: [27:22]; default: 0;
 *  Reserved (used for four backups method).
 */
#define EFUSE_RPT4_RESERVED0    0x0000003FU
#define EFUSE_RPT4_RESERVED0_M  (EFUSE_RPT4_RESERVED0_V << EFUSE_RPT4_RESERVED0_S)
#define EFUSE_RPT4_RESERVED0_V  0x0000003FU
#define EFUSE_RPT4_RESERVED0_S  22
/** EFUSE_FLASH_TPUW : RO; bitpos: [31:28]; default: 0;
 *  Configures flash waiting time after power-up, in unit of ms. If the value is less
 *  than 15, the waiting time is the configurable value; Otherwise, the waiting time is
 *  twice the configurable value.
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
 *  Set this bit to disable download mode (boot_mode[3:0] = 0, 1, 2, 3, 6, 7).
 */
#define EFUSE_DIS_DOWNLOAD_MODE    (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_M  (EFUSE_DIS_DOWNLOAD_MODE_V << EFUSE_DIS_DOWNLOAD_MODE_S)
#define EFUSE_DIS_DOWNLOAD_MODE_V  0x00000001U
#define EFUSE_DIS_DOWNLOAD_MODE_S  0
/** EFUSE_DIS_DIRECT_BOOT : RO; bitpos: [1]; default: 0;
 *  Disable direct boot mode
 */
#define EFUSE_DIS_DIRECT_BOOT    (BIT(1))
#define EFUSE_DIS_DIRECT_BOOT_M  (EFUSE_DIS_DIRECT_BOOT_V << EFUSE_DIS_DIRECT_BOOT_S)
#define EFUSE_DIS_DIRECT_BOOT_V  0x00000001U
#define EFUSE_DIS_DIRECT_BOOT_S  1
/** EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT : RO; bitpos: [2]; default: 0;
 *  Represents whether USB printing is disabled or enabled. 1: Disabled. 0: Enabled
 */
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT    (BIT(2))
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_M  (EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_V << EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_S)
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_V  0x00000001U
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_S  2
/** EFUSE_FLASH_ECC_MODE : RO; bitpos: [3]; default: 0;
 *  Set ECC mode in ROM, 0: ROM would Enable Flash ECC 16to18 byte mode. 1:ROM would
 *  use 16to17 byte mode.
 */
#define EFUSE_FLASH_ECC_MODE    (BIT(3))
#define EFUSE_FLASH_ECC_MODE_M  (EFUSE_FLASH_ECC_MODE_V << EFUSE_FLASH_ECC_MODE_S)
#define EFUSE_FLASH_ECC_MODE_V  0x00000001U
#define EFUSE_FLASH_ECC_MODE_S  3
/** EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE : RO; bitpos: [4]; default: 0;
 *  Disable UART download mode through USB-Serial-JTAG
 */
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE    (BIT(4))
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_M  (EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_V << EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_S)
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_V  0x00000001U
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_S  4
/** EFUSE_ENABLE_SECURITY_DOWNLOAD : RO; bitpos: [5]; default: 0;
 *  Set this bit to enable secure UART download mode.
 */
#define EFUSE_ENABLE_SECURITY_DOWNLOAD    (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_M  (EFUSE_ENABLE_SECURITY_DOWNLOAD_V << EFUSE_ENABLE_SECURITY_DOWNLOAD_S)
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_V  0x00000001U
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_S  5
/** EFUSE_UART_PRINT_CONTROL : RO; bitpos: [7:6]; default: 0;
 *  Set the default UARTboot message output mode. 00: Enabled. 01: Enabled when GPIO8
 *  is low at reset. 10: Enabled when GPIO8 is high at reset. 11:disabled.
 */
#define EFUSE_UART_PRINT_CONTROL    0x00000003U
#define EFUSE_UART_PRINT_CONTROL_M  (EFUSE_UART_PRINT_CONTROL_V << EFUSE_UART_PRINT_CONTROL_S)
#define EFUSE_UART_PRINT_CONTROL_V  0x00000003U
#define EFUSE_UART_PRINT_CONTROL_S  6
/** EFUSE_PIN_POWER_SELECTION : RO; bitpos: [8]; default: 0;
 *  GPIO33-GPIO37 power supply selection in ROM code. 0: VDD3P3_CPU. 1: VDD_SPI.
 */
#define EFUSE_PIN_POWER_SELECTION    (BIT(8))
#define EFUSE_PIN_POWER_SELECTION_M  (EFUSE_PIN_POWER_SELECTION_V << EFUSE_PIN_POWER_SELECTION_S)
#define EFUSE_PIN_POWER_SELECTION_V  0x00000001U
#define EFUSE_PIN_POWER_SELECTION_S  8
/** EFUSE_FLASH_TYPE : RO; bitpos: [9]; default: 0;
 *  Set the maximum lines of SPI flash. 0: four lines. 1: eight lines.
 */
#define EFUSE_FLASH_TYPE    (BIT(9))
#define EFUSE_FLASH_TYPE_M  (EFUSE_FLASH_TYPE_V << EFUSE_FLASH_TYPE_S)
#define EFUSE_FLASH_TYPE_V  0x00000001U
#define EFUSE_FLASH_TYPE_S  9
/** EFUSE_FLASH_PAGE_SIZE : RO; bitpos: [11:10]; default: 0;
 *  Set Flash page size.
 */
#define EFUSE_FLASH_PAGE_SIZE    0x00000003U
#define EFUSE_FLASH_PAGE_SIZE_M  (EFUSE_FLASH_PAGE_SIZE_V << EFUSE_FLASH_PAGE_SIZE_S)
#define EFUSE_FLASH_PAGE_SIZE_V  0x00000003U
#define EFUSE_FLASH_PAGE_SIZE_S  10
/** EFUSE_FLASH_ECC_EN : RO; bitpos: [12]; default: 0;
 *  Set 1 to enable ECC for flash boot.
 */
#define EFUSE_FLASH_ECC_EN    (BIT(12))
#define EFUSE_FLASH_ECC_EN_M  (EFUSE_FLASH_ECC_EN_V << EFUSE_FLASH_ECC_EN_S)
#define EFUSE_FLASH_ECC_EN_V  0x00000001U
#define EFUSE_FLASH_ECC_EN_S  12
/** EFUSE_FORCE_SEND_RESUME : RO; bitpos: [13]; default: 0;
 *  Set this bit to force ROM code to send a resume command during SPI boot.
 */
#define EFUSE_FORCE_SEND_RESUME    (BIT(13))
#define EFUSE_FORCE_SEND_RESUME_M  (EFUSE_FORCE_SEND_RESUME_V << EFUSE_FORCE_SEND_RESUME_S)
#define EFUSE_FORCE_SEND_RESUME_V  0x00000001U
#define EFUSE_FORCE_SEND_RESUME_S  13
/** EFUSE_SECURE_VERSION : RO; bitpos: [29:14]; default: 0;
 *  Secure version (used by ESP-IDF anti-rollback feature).
 */
#define EFUSE_SECURE_VERSION    0x0000FFFFU
#define EFUSE_SECURE_VERSION_M  (EFUSE_SECURE_VERSION_V << EFUSE_SECURE_VERSION_S)
#define EFUSE_SECURE_VERSION_V  0x0000FFFFU
#define EFUSE_SECURE_VERSION_S  14
/** EFUSE_RESERVED_0_158 : R; bitpos: [30]; default: 0;
 *  reserved
 */
#define EFUSE_RESERVED_0_158    (BIT(30))
#define EFUSE_RESERVED_0_158_M  (EFUSE_RESERVED_0_158_V << EFUSE_RESERVED_0_158_S)
#define EFUSE_RESERVED_0_158_V  0x00000001U
#define EFUSE_RESERVED_0_158_S  30
/** EFUSE_ERR_RST_ENABLE : R; bitpos: [31]; default: 0;
 *  Use BLOCK0 to check error record registers
 */
#define EFUSE_ERR_RST_ENABLE    (BIT(31))
#define EFUSE_ERR_RST_ENABLE_M  (EFUSE_ERR_RST_ENABLE_V << EFUSE_ERR_RST_ENABLE_S)
#define EFUSE_ERR_RST_ENABLE_V  0x00000001U
#define EFUSE_ERR_RST_ENABLE_S  31

/** EFUSE_RD_REPEAT_DATA4_REG register
 *  BLOCK0 data register 5.
 */
#define EFUSE_RD_REPEAT_DATA4_REG (DR_REG_EFUSE_BASE + 0x40)
/** EFUSE_DISABLE_WAFER_VERSION_MAJOR : R; bitpos: [0]; default: 0;
 *  Disables check of wafer version major
 */
#define EFUSE_DISABLE_WAFER_VERSION_MAJOR    (BIT(0))
#define EFUSE_DISABLE_WAFER_VERSION_MAJOR_M  (EFUSE_DISABLE_WAFER_VERSION_MAJOR_V << EFUSE_DISABLE_WAFER_VERSION_MAJOR_S)
#define EFUSE_DISABLE_WAFER_VERSION_MAJOR_V  0x00000001U
#define EFUSE_DISABLE_WAFER_VERSION_MAJOR_S  0
/** EFUSE_DISABLE_BLK_VERSION_MAJOR : R; bitpos: [1]; default: 0;
 *  Disables check of blk version major
 */
#define EFUSE_DISABLE_BLK_VERSION_MAJOR    (BIT(1))
#define EFUSE_DISABLE_BLK_VERSION_MAJOR_M  (EFUSE_DISABLE_BLK_VERSION_MAJOR_V << EFUSE_DISABLE_BLK_VERSION_MAJOR_S)
#define EFUSE_DISABLE_BLK_VERSION_MAJOR_V  0x00000001U
#define EFUSE_DISABLE_BLK_VERSION_MAJOR_S  1
/** EFUSE_RESERVED_0_162 : R; bitpos: [23:2]; default: 0;
 *  reserved
 */
#define EFUSE_RESERVED_0_162    0x003FFFFFU
#define EFUSE_RESERVED_0_162_M  (EFUSE_RESERVED_0_162_V << EFUSE_RESERVED_0_162_S)
#define EFUSE_RESERVED_0_162_V  0x003FFFFFU
#define EFUSE_RESERVED_0_162_S  2

/** EFUSE_RD_MAC_SPI_SYS_0_REG register
 *  BLOCK1 data register 0.
 */
#define EFUSE_RD_MAC_SPI_SYS_0_REG (DR_REG_EFUSE_BASE + 0x44)
/** EFUSE_MAC_0 : RO; bitpos: [31:0]; default: 0;
 *  Stores the low 32 bits of MAC address.
 */
#define EFUSE_MAC_0    0xFFFFFFFFU
#define EFUSE_MAC_0_M  (EFUSE_MAC_0_V << EFUSE_MAC_0_S)
#define EFUSE_MAC_0_V  0xFFFFFFFFU
#define EFUSE_MAC_0_S  0

/** EFUSE_RD_MAC_SPI_SYS_1_REG register
 *  BLOCK1 data register 1.
 */
#define EFUSE_RD_MAC_SPI_SYS_1_REG (DR_REG_EFUSE_BASE + 0x48)
/** EFUSE_MAC_1 : RO; bitpos: [15:0]; default: 0;
 *  Stores the high 16 bits of MAC address.
 */
#define EFUSE_MAC_1    0x0000FFFFU
#define EFUSE_MAC_1_M  (EFUSE_MAC_1_V << EFUSE_MAC_1_S)
#define EFUSE_MAC_1_V  0x0000FFFFU
#define EFUSE_MAC_1_S  0
/** EFUSE_SPI_PAD_CONFIG_CLK : R; bitpos: [21:16]; default: 0;
 *  SPI PAD CLK
 */
#define EFUSE_SPI_PAD_CONFIG_CLK    0x0000003FU
#define EFUSE_SPI_PAD_CONFIG_CLK_M  (EFUSE_SPI_PAD_CONFIG_CLK_V << EFUSE_SPI_PAD_CONFIG_CLK_S)
#define EFUSE_SPI_PAD_CONFIG_CLK_V  0x0000003FU
#define EFUSE_SPI_PAD_CONFIG_CLK_S  16
/** EFUSE_SPI_PAD_CONFIG_Q : R; bitpos: [27:22]; default: 0;
 *  SPI PAD Q(D1)
 */
#define EFUSE_SPI_PAD_CONFIG_Q    0x0000003FU
#define EFUSE_SPI_PAD_CONFIG_Q_M  (EFUSE_SPI_PAD_CONFIG_Q_V << EFUSE_SPI_PAD_CONFIG_Q_S)
#define EFUSE_SPI_PAD_CONFIG_Q_V  0x0000003FU
#define EFUSE_SPI_PAD_CONFIG_Q_S  22
/** EFUSE_SPI_PAD_CONFIG_D : R; bitpos: [31:28]; default: 0;
 *  SPI PAD D(D0)
 */
#define EFUSE_SPI_PAD_CONFIG_D    0x0000000FU
#define EFUSE_SPI_PAD_CONFIG_D_M  (EFUSE_SPI_PAD_CONFIG_D_V << EFUSE_SPI_PAD_CONFIG_D_S)
#define EFUSE_SPI_PAD_CONFIG_D_V  0x0000000FU
#define EFUSE_SPI_PAD_CONFIG_D_S  28

/** EFUSE_RD_MAC_SPI_SYS_2_REG register
 *  BLOCK1 data register 2.
 */
#define EFUSE_RD_MAC_SPI_SYS_2_REG (DR_REG_EFUSE_BASE + 0x4c)
/** EFUSE_SPI_PAD_CONFIG_D_1 : R; bitpos: [1:0]; default: 0;
 *  SPI PAD D(D0)
 */
#define EFUSE_SPI_PAD_CONFIG_D_1    0x00000003U
#define EFUSE_SPI_PAD_CONFIG_D_1_M  (EFUSE_SPI_PAD_CONFIG_D_1_V << EFUSE_SPI_PAD_CONFIG_D_1_S)
#define EFUSE_SPI_PAD_CONFIG_D_1_V  0x00000003U
#define EFUSE_SPI_PAD_CONFIG_D_1_S  0
/** EFUSE_SPI_PAD_CONFIG_CS : R; bitpos: [7:2]; default: 0;
 *  SPI PAD CS
 */
#define EFUSE_SPI_PAD_CONFIG_CS    0x0000003FU
#define EFUSE_SPI_PAD_CONFIG_CS_M  (EFUSE_SPI_PAD_CONFIG_CS_V << EFUSE_SPI_PAD_CONFIG_CS_S)
#define EFUSE_SPI_PAD_CONFIG_CS_V  0x0000003FU
#define EFUSE_SPI_PAD_CONFIG_CS_S  2
/** EFUSE_SPI_PAD_CONFIG_HD : R; bitpos: [13:8]; default: 0;
 *  SPI PAD HD(D3)
 */
#define EFUSE_SPI_PAD_CONFIG_HD    0x0000003FU
#define EFUSE_SPI_PAD_CONFIG_HD_M  (EFUSE_SPI_PAD_CONFIG_HD_V << EFUSE_SPI_PAD_CONFIG_HD_S)
#define EFUSE_SPI_PAD_CONFIG_HD_V  0x0000003FU
#define EFUSE_SPI_PAD_CONFIG_HD_S  8
/** EFUSE_SPI_PAD_CONFIG_WP : R; bitpos: [19:14]; default: 0;
 *  SPI PAD WP(D2)
 */
#define EFUSE_SPI_PAD_CONFIG_WP    0x0000003FU
#define EFUSE_SPI_PAD_CONFIG_WP_M  (EFUSE_SPI_PAD_CONFIG_WP_V << EFUSE_SPI_PAD_CONFIG_WP_S)
#define EFUSE_SPI_PAD_CONFIG_WP_V  0x0000003FU
#define EFUSE_SPI_PAD_CONFIG_WP_S  14
/** EFUSE_SPI_PAD_CONFIG_DQS : R; bitpos: [25:20]; default: 0;
 *  SPI PAD DQS
 */
#define EFUSE_SPI_PAD_CONFIG_DQS    0x0000003FU
#define EFUSE_SPI_PAD_CONFIG_DQS_M  (EFUSE_SPI_PAD_CONFIG_DQS_V << EFUSE_SPI_PAD_CONFIG_DQS_S)
#define EFUSE_SPI_PAD_CONFIG_DQS_V  0x0000003FU
#define EFUSE_SPI_PAD_CONFIG_DQS_S  20
/** EFUSE_SPI_PAD_CONFIG_D4 : R; bitpos: [31:26]; default: 0;
 *  SPI PAD D4
 */
#define EFUSE_SPI_PAD_CONFIG_D4    0x0000003FU
#define EFUSE_SPI_PAD_CONFIG_D4_M  (EFUSE_SPI_PAD_CONFIG_D4_V << EFUSE_SPI_PAD_CONFIG_D4_S)
#define EFUSE_SPI_PAD_CONFIG_D4_V  0x0000003FU
#define EFUSE_SPI_PAD_CONFIG_D4_S  26

/** EFUSE_RD_MAC_SPI_SYS_3_REG register
 *  BLOCK1 data register 3.
 */
#define EFUSE_RD_MAC_SPI_SYS_3_REG (DR_REG_EFUSE_BASE + 0x50)
/** EFUSE_SPI_PAD_CONFIG_D5 : R; bitpos: [5:0]; default: 0;
 *  SPI PAD D5
 */
#define EFUSE_SPI_PAD_CONFIG_D5    0x0000003FU
#define EFUSE_SPI_PAD_CONFIG_D5_M  (EFUSE_SPI_PAD_CONFIG_D5_V << EFUSE_SPI_PAD_CONFIG_D5_S)
#define EFUSE_SPI_PAD_CONFIG_D5_V  0x0000003FU
#define EFUSE_SPI_PAD_CONFIG_D5_S  0
/** EFUSE_SPI_PAD_CONFIG_D6 : R; bitpos: [11:6]; default: 0;
 *  SPI PAD D6
 */
#define EFUSE_SPI_PAD_CONFIG_D6    0x0000003FU
#define EFUSE_SPI_PAD_CONFIG_D6_M  (EFUSE_SPI_PAD_CONFIG_D6_V << EFUSE_SPI_PAD_CONFIG_D6_S)
#define EFUSE_SPI_PAD_CONFIG_D6_V  0x0000003FU
#define EFUSE_SPI_PAD_CONFIG_D6_S  6
/** EFUSE_SPI_PAD_CONFIG_D7 : R; bitpos: [17:12]; default: 0;
 *  SPI PAD D7
 */
#define EFUSE_SPI_PAD_CONFIG_D7    0x0000003FU
#define EFUSE_SPI_PAD_CONFIG_D7_M  (EFUSE_SPI_PAD_CONFIG_D7_V << EFUSE_SPI_PAD_CONFIG_D7_S)
#define EFUSE_SPI_PAD_CONFIG_D7_V  0x0000003FU
#define EFUSE_SPI_PAD_CONFIG_D7_S  12
/** EFUSE_WAFER_VERSION_MINOR_LO : R; bitpos: [20:18]; default: 0;
 *  WAFER_VERSION_MINOR least significant bits
 */
#define EFUSE_WAFER_VERSION_MINOR_LO    0x00000007U
#define EFUSE_WAFER_VERSION_MINOR_LO_M  (EFUSE_WAFER_VERSION_MINOR_LO_V << EFUSE_WAFER_VERSION_MINOR_LO_S)
#define EFUSE_WAFER_VERSION_MINOR_LO_V  0x00000007U
#define EFUSE_WAFER_VERSION_MINOR_LO_S  18
/** EFUSE_PKG_VERSION : R; bitpos: [23:21]; default: 0;
 *  Package version
 */
#define EFUSE_PKG_VERSION    0x00000007U
#define EFUSE_PKG_VERSION_M  (EFUSE_PKG_VERSION_V << EFUSE_PKG_VERSION_S)
#define EFUSE_PKG_VERSION_V  0x00000007U
#define EFUSE_PKG_VERSION_S  21
/** EFUSE_BLK_VERSION_MINOR : R; bitpos: [26:24]; default: 0;
 *  BLK_VERSION_MINOR
 */
#define EFUSE_BLK_VERSION_MINOR    0x00000007U
#define EFUSE_BLK_VERSION_MINOR_M  (EFUSE_BLK_VERSION_MINOR_V << EFUSE_BLK_VERSION_MINOR_S)
#define EFUSE_BLK_VERSION_MINOR_V  0x00000007U
#define EFUSE_BLK_VERSION_MINOR_S  24
/** EFUSE_FLASH_CAP : R; bitpos: [29:27]; default: 0;
 *  Flash capacity
 */
#define EFUSE_FLASH_CAP    0x00000007U
#define EFUSE_FLASH_CAP_M  (EFUSE_FLASH_CAP_V << EFUSE_FLASH_CAP_S)
#define EFUSE_FLASH_CAP_V  0x00000007U
#define EFUSE_FLASH_CAP_S  27
/** EFUSE_FLASH_TEMP : R; bitpos: [31:30]; default: 0;
 *  Flash temperature
 */
#define EFUSE_FLASH_TEMP    0x00000003U
#define EFUSE_FLASH_TEMP_M  (EFUSE_FLASH_TEMP_V << EFUSE_FLASH_TEMP_S)
#define EFUSE_FLASH_TEMP_V  0x00000003U
#define EFUSE_FLASH_TEMP_S  30

/** EFUSE_RD_MAC_SPI_SYS_4_REG register
 *  BLOCK1 data register 4.
 */
#define EFUSE_RD_MAC_SPI_SYS_4_REG (DR_REG_EFUSE_BASE + 0x54)
/** EFUSE_FLASH_VENDOR : R; bitpos: [2:0]; default: 0;
 *  Flash vendor
 */
#define EFUSE_FLASH_VENDOR    0x00000007U
#define EFUSE_FLASH_VENDOR_M  (EFUSE_FLASH_VENDOR_V << EFUSE_FLASH_VENDOR_S)
#define EFUSE_FLASH_VENDOR_V  0x00000007U
#define EFUSE_FLASH_VENDOR_S  0
/** EFUSE_RESERVED_1_131 : R; bitpos: [6:3]; default: 0;
 *  reserved
 */
#define EFUSE_RESERVED_1_131    0x0000000FU
#define EFUSE_RESERVED_1_131_M  (EFUSE_RESERVED_1_131_V << EFUSE_RESERVED_1_131_S)
#define EFUSE_RESERVED_1_131_V  0x0000000FU
#define EFUSE_RESERVED_1_131_S  3
/** EFUSE_K_RTC_LDO : R; bitpos: [13:7]; default: 0;
 *  BLOCK1 K_RTC_LDO
 */
#define EFUSE_K_RTC_LDO    0x0000007FU
#define EFUSE_K_RTC_LDO_M  (EFUSE_K_RTC_LDO_V << EFUSE_K_RTC_LDO_S)
#define EFUSE_K_RTC_LDO_V  0x0000007FU
#define EFUSE_K_RTC_LDO_S  7
/** EFUSE_K_DIG_LDO : R; bitpos: [20:14]; default: 0;
 *  BLOCK1 K_DIG_LDO
 */
#define EFUSE_K_DIG_LDO    0x0000007FU
#define EFUSE_K_DIG_LDO_M  (EFUSE_K_DIG_LDO_V << EFUSE_K_DIG_LDO_S)
#define EFUSE_K_DIG_LDO_V  0x0000007FU
#define EFUSE_K_DIG_LDO_S  14
/** EFUSE_V_RTC_DBIAS20 : R; bitpos: [28:21]; default: 0;
 *  BLOCK1 voltage of rtc dbias20
 */
#define EFUSE_V_RTC_DBIAS20    0x000000FFU
#define EFUSE_V_RTC_DBIAS20_M  (EFUSE_V_RTC_DBIAS20_V << EFUSE_V_RTC_DBIAS20_S)
#define EFUSE_V_RTC_DBIAS20_V  0x000000FFU
#define EFUSE_V_RTC_DBIAS20_S  21
/** EFUSE_V_DIG_DBIAS20 : R; bitpos: [31:29]; default: 0;
 *  BLOCK1 voltage of digital dbias20
 */
#define EFUSE_V_DIG_DBIAS20    0x00000007U
#define EFUSE_V_DIG_DBIAS20_M  (EFUSE_V_DIG_DBIAS20_V << EFUSE_V_DIG_DBIAS20_S)
#define EFUSE_V_DIG_DBIAS20_V  0x00000007U
#define EFUSE_V_DIG_DBIAS20_S  29

/** EFUSE_RD_MAC_SPI_SYS_5_REG register
 *  BLOCK1 data register 5.
 */
#define EFUSE_RD_MAC_SPI_SYS_5_REG (DR_REG_EFUSE_BASE + 0x58)
/** EFUSE_V_DIG_DBIAS20_1 : R; bitpos: [4:0]; default: 0;
 *  BLOCK1 voltage of digital dbias20
 */
#define EFUSE_V_DIG_DBIAS20_1    0x0000001FU
#define EFUSE_V_DIG_DBIAS20_1_M  (EFUSE_V_DIG_DBIAS20_1_V << EFUSE_V_DIG_DBIAS20_1_S)
#define EFUSE_V_DIG_DBIAS20_1_V  0x0000001FU
#define EFUSE_V_DIG_DBIAS20_1_S  0
/** EFUSE_DIG_DBIAS_HVT : R; bitpos: [9:5]; default: 0;
 *  BLOCK1 digital dbias when hvt
 */
#define EFUSE_DIG_DBIAS_HVT    0x0000001FU
#define EFUSE_DIG_DBIAS_HVT_M  (EFUSE_DIG_DBIAS_HVT_V << EFUSE_DIG_DBIAS_HVT_S)
#define EFUSE_DIG_DBIAS_HVT_V  0x0000001FU
#define EFUSE_DIG_DBIAS_HVT_S  5
/** EFUSE_THRES_HVT : R; bitpos: [19:10]; default: 0;
 *  BLOCK1 pvt threshold when hvt
 */
#define EFUSE_THRES_HVT    0x000003FFU
#define EFUSE_THRES_HVT_M  (EFUSE_THRES_HVT_V << EFUSE_THRES_HVT_S)
#define EFUSE_THRES_HVT_V  0x000003FFU
#define EFUSE_THRES_HVT_S  10
/** EFUSE_RESERVED_1_180 : R; bitpos: [22:20]; default: 0;
 *  reserved
 */
#define EFUSE_RESERVED_1_180    0x00000007U
#define EFUSE_RESERVED_1_180_M  (EFUSE_RESERVED_1_180_V << EFUSE_RESERVED_1_180_S)
#define EFUSE_RESERVED_1_180_V  0x00000007U
#define EFUSE_RESERVED_1_180_S  20
/** EFUSE_WAFER_VERSION_MINOR_HI : R; bitpos: [23]; default: 0;
 *  WAFER_VERSION_MINOR most significant bit
 */
#define EFUSE_WAFER_VERSION_MINOR_HI    (BIT(23))
#define EFUSE_WAFER_VERSION_MINOR_HI_M  (EFUSE_WAFER_VERSION_MINOR_HI_V << EFUSE_WAFER_VERSION_MINOR_HI_S)
#define EFUSE_WAFER_VERSION_MINOR_HI_V  0x00000001U
#define EFUSE_WAFER_VERSION_MINOR_HI_S  23
/** EFUSE_WAFER_VERSION_MAJOR : R; bitpos: [25:24]; default: 0;
 *  WAFER_VERSION_MAJOR
 */
#define EFUSE_WAFER_VERSION_MAJOR    0x00000003U
#define EFUSE_WAFER_VERSION_MAJOR_M  (EFUSE_WAFER_VERSION_MAJOR_V << EFUSE_WAFER_VERSION_MAJOR_S)
#define EFUSE_WAFER_VERSION_MAJOR_V  0x00000003U
#define EFUSE_WAFER_VERSION_MAJOR_S  24
/** EFUSE_RESERVED_1_186 : R; bitpos: [31:26]; default: 0;
 *  reserved
 */
#define EFUSE_RESERVED_1_186    0x0000003FU
#define EFUSE_RESERVED_1_186_M  (EFUSE_RESERVED_1_186_V << EFUSE_RESERVED_1_186_S)
#define EFUSE_RESERVED_1_186_V  0x0000003FU
#define EFUSE_RESERVED_1_186_S  26

/** EFUSE_RD_SYS_PART1_DATA0_REG register
 *  Register 0 of BLOCK2 (system).
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
 *  Register 1 of BLOCK2 (system).
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
 *  Register 2 of BLOCK2 (system).
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
 *  Register 3 of BLOCK2 (system).
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
 *  Register 4 of BLOCK2 (system).
 */
#define EFUSE_RD_SYS_PART1_DATA4_REG (DR_REG_EFUSE_BASE + 0x6c)
/** EFUSE_BLK_VERSION_MAJOR : R; bitpos: [1:0]; default: 0;
 *  BLK_VERSION_MAJOR of BLOCK2
 */
#define EFUSE_BLK_VERSION_MAJOR    0x00000003U
#define EFUSE_BLK_VERSION_MAJOR_M  (EFUSE_BLK_VERSION_MAJOR_V << EFUSE_BLK_VERSION_MAJOR_S)
#define EFUSE_BLK_VERSION_MAJOR_V  0x00000003U
#define EFUSE_BLK_VERSION_MAJOR_S  0
/** EFUSE_RESERVED_2_130 : R; bitpos: [2]; default: 0;
 *  reserved
 */
#define EFUSE_RESERVED_2_130    (BIT(2))
#define EFUSE_RESERVED_2_130_M  (EFUSE_RESERVED_2_130_V << EFUSE_RESERVED_2_130_S)
#define EFUSE_RESERVED_2_130_V  0x00000001U
#define EFUSE_RESERVED_2_130_S  2
/** EFUSE_TEMP_CALIB : R; bitpos: [11:3]; default: 0;
 *  Temperature calibration data
 */
#define EFUSE_TEMP_CALIB    0x000001FFU
#define EFUSE_TEMP_CALIB_M  (EFUSE_TEMP_CALIB_V << EFUSE_TEMP_CALIB_S)
#define EFUSE_TEMP_CALIB_V  0x000001FFU
#define EFUSE_TEMP_CALIB_S  3
/** EFUSE_OCODE : R; bitpos: [19:12]; default: 0;
 *  ADC OCode
 */
#define EFUSE_OCODE    0x000000FFU
#define EFUSE_OCODE_M  (EFUSE_OCODE_V << EFUSE_OCODE_S)
#define EFUSE_OCODE_V  0x000000FFU
#define EFUSE_OCODE_S  12
/** EFUSE_ADC1_INIT_CODE_ATTEN0 : R; bitpos: [29:20]; default: 0;
 *  ADC1 init code at atten0
 */
#define EFUSE_ADC1_INIT_CODE_ATTEN0    0x000003FFU
#define EFUSE_ADC1_INIT_CODE_ATTEN0_M  (EFUSE_ADC1_INIT_CODE_ATTEN0_V << EFUSE_ADC1_INIT_CODE_ATTEN0_S)
#define EFUSE_ADC1_INIT_CODE_ATTEN0_V  0x000003FFU
#define EFUSE_ADC1_INIT_CODE_ATTEN0_S  20
/** EFUSE_ADC1_INIT_CODE_ATTEN1 : R; bitpos: [31:30]; default: 0;
 *  ADC1 init code at atten1
 */
#define EFUSE_ADC1_INIT_CODE_ATTEN1    0x00000003U
#define EFUSE_ADC1_INIT_CODE_ATTEN1_M  (EFUSE_ADC1_INIT_CODE_ATTEN1_V << EFUSE_ADC1_INIT_CODE_ATTEN1_S)
#define EFUSE_ADC1_INIT_CODE_ATTEN1_V  0x00000003U
#define EFUSE_ADC1_INIT_CODE_ATTEN1_S  30

/** EFUSE_RD_SYS_PART1_DATA5_REG register
 *  Register 5 of BLOCK2 (system).
 */
#define EFUSE_RD_SYS_PART1_DATA5_REG (DR_REG_EFUSE_BASE + 0x70)
/** EFUSE_ADC1_INIT_CODE_ATTEN1_1 : R; bitpos: [7:0]; default: 0;
 *  ADC1 init code at atten1
 */
#define EFUSE_ADC1_INIT_CODE_ATTEN1_1    0x000000FFU
#define EFUSE_ADC1_INIT_CODE_ATTEN1_1_M  (EFUSE_ADC1_INIT_CODE_ATTEN1_1_V << EFUSE_ADC1_INIT_CODE_ATTEN1_1_S)
#define EFUSE_ADC1_INIT_CODE_ATTEN1_1_V  0x000000FFU
#define EFUSE_ADC1_INIT_CODE_ATTEN1_1_S  0
/** EFUSE_ADC1_INIT_CODE_ATTEN2 : R; bitpos: [17:8]; default: 0;
 *  ADC1 init code at atten2
 */
#define EFUSE_ADC1_INIT_CODE_ATTEN2    0x000003FFU
#define EFUSE_ADC1_INIT_CODE_ATTEN2_M  (EFUSE_ADC1_INIT_CODE_ATTEN2_V << EFUSE_ADC1_INIT_CODE_ATTEN2_S)
#define EFUSE_ADC1_INIT_CODE_ATTEN2_V  0x000003FFU
#define EFUSE_ADC1_INIT_CODE_ATTEN2_S  8
/** EFUSE_ADC1_INIT_CODE_ATTEN3 : R; bitpos: [27:18]; default: 0;
 *  ADC1 init code at atten3
 */
#define EFUSE_ADC1_INIT_CODE_ATTEN3    0x000003FFU
#define EFUSE_ADC1_INIT_CODE_ATTEN3_M  (EFUSE_ADC1_INIT_CODE_ATTEN3_V << EFUSE_ADC1_INIT_CODE_ATTEN3_S)
#define EFUSE_ADC1_INIT_CODE_ATTEN3_V  0x000003FFU
#define EFUSE_ADC1_INIT_CODE_ATTEN3_S  18
/** EFUSE_ADC1_CAL_VOL_ATTEN0 : R; bitpos: [31:28]; default: 0;
 *  ADC1 calibration voltage at atten0
 */
#define EFUSE_ADC1_CAL_VOL_ATTEN0    0x0000000FU
#define EFUSE_ADC1_CAL_VOL_ATTEN0_M  (EFUSE_ADC1_CAL_VOL_ATTEN0_V << EFUSE_ADC1_CAL_VOL_ATTEN0_S)
#define EFUSE_ADC1_CAL_VOL_ATTEN0_V  0x0000000FU
#define EFUSE_ADC1_CAL_VOL_ATTEN0_S  28

/** EFUSE_RD_SYS_PART1_DATA6_REG register
 *  Register 6 of BLOCK2 (system).
 */
#define EFUSE_RD_SYS_PART1_DATA6_REG (DR_REG_EFUSE_BASE + 0x74)
/** EFUSE_ADC1_CAL_VOL_ATTEN0_1 : R; bitpos: [5:0]; default: 0;
 *  ADC1 calibration voltage at atten0
 */
#define EFUSE_ADC1_CAL_VOL_ATTEN0_1    0x0000003FU
#define EFUSE_ADC1_CAL_VOL_ATTEN0_1_M  (EFUSE_ADC1_CAL_VOL_ATTEN0_1_V << EFUSE_ADC1_CAL_VOL_ATTEN0_1_S)
#define EFUSE_ADC1_CAL_VOL_ATTEN0_1_V  0x0000003FU
#define EFUSE_ADC1_CAL_VOL_ATTEN0_1_S  0
/** EFUSE_ADC1_CAL_VOL_ATTEN1 : R; bitpos: [15:6]; default: 0;
 *  ADC1 calibration voltage at atten1
 */
#define EFUSE_ADC1_CAL_VOL_ATTEN1    0x000003FFU
#define EFUSE_ADC1_CAL_VOL_ATTEN1_M  (EFUSE_ADC1_CAL_VOL_ATTEN1_V << EFUSE_ADC1_CAL_VOL_ATTEN1_S)
#define EFUSE_ADC1_CAL_VOL_ATTEN1_V  0x000003FFU
#define EFUSE_ADC1_CAL_VOL_ATTEN1_S  6
/** EFUSE_ADC1_CAL_VOL_ATTEN2 : R; bitpos: [25:16]; default: 0;
 *  ADC1 calibration voltage at atten2
 */
#define EFUSE_ADC1_CAL_VOL_ATTEN2    0x000003FFU
#define EFUSE_ADC1_CAL_VOL_ATTEN2_M  (EFUSE_ADC1_CAL_VOL_ATTEN2_V << EFUSE_ADC1_CAL_VOL_ATTEN2_S)
#define EFUSE_ADC1_CAL_VOL_ATTEN2_V  0x000003FFU
#define EFUSE_ADC1_CAL_VOL_ATTEN2_S  16
/** EFUSE_ADC1_CAL_VOL_ATTEN3 : R; bitpos: [31:26]; default: 0;
 *  ADC1 calibration voltage at atten3
 */
#define EFUSE_ADC1_CAL_VOL_ATTEN3    0x0000003FU
#define EFUSE_ADC1_CAL_VOL_ATTEN3_M  (EFUSE_ADC1_CAL_VOL_ATTEN3_V << EFUSE_ADC1_CAL_VOL_ATTEN3_S)
#define EFUSE_ADC1_CAL_VOL_ATTEN3_V  0x0000003FU
#define EFUSE_ADC1_CAL_VOL_ATTEN3_S  26

/** EFUSE_RD_SYS_PART1_DATA7_REG register
 *  Register 7 of BLOCK2 (system).
 */
#define EFUSE_RD_SYS_PART1_DATA7_REG (DR_REG_EFUSE_BASE + 0x78)
/** EFUSE_ADC1_CAL_VOL_ATTEN3_1 : R; bitpos: [3:0]; default: 0;
 *  ADC1 calibration voltage at atten3
 */
#define EFUSE_ADC1_CAL_VOL_ATTEN3_1    0x0000000FU
#define EFUSE_ADC1_CAL_VOL_ATTEN3_1_M  (EFUSE_ADC1_CAL_VOL_ATTEN3_1_V << EFUSE_ADC1_CAL_VOL_ATTEN3_1_S)
#define EFUSE_ADC1_CAL_VOL_ATTEN3_1_V  0x0000000FU
#define EFUSE_ADC1_CAL_VOL_ATTEN3_1_S  0
/** EFUSE_RESERVED_2_228 : R; bitpos: [31:4]; default: 0;
 *  reserved
 */
#define EFUSE_RESERVED_2_228    0x0FFFFFFFU
#define EFUSE_RESERVED_2_228_M  (EFUSE_RESERVED_2_228_V << EFUSE_RESERVED_2_228_S)
#define EFUSE_RESERVED_2_228_V  0x0FFFFFFFU
#define EFUSE_RESERVED_2_228_S  4

/** EFUSE_RD_USR_DATA0_REG register
 *  Register 0 of BLOCK3 (user).
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
 *  Register 1 of BLOCK3 (user).
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
 *  Register 2 of BLOCK3 (user).
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
 *  Register 3 of BLOCK3 (user).
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
 *  Register 4 of BLOCK3 (user).
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
 *  Register 5 of BLOCK3 (user).
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
 *  Register 6 of BLOCK3 (user).
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
 *  Custom MAC address
 */
#define EFUSE_CUSTOM_MAC    0x00FFFFFFU
#define EFUSE_CUSTOM_MAC_M  (EFUSE_CUSTOM_MAC_V << EFUSE_CUSTOM_MAC_S)
#define EFUSE_CUSTOM_MAC_V  0x00FFFFFFU
#define EFUSE_CUSTOM_MAC_S  8

/** EFUSE_RD_USR_DATA7_REG register
 *  Register 7 of BLOCK3 (user).
 */
#define EFUSE_RD_USR_DATA7_REG (DR_REG_EFUSE_BASE + 0x98)
/** EFUSE_CUSTOM_MAC_1 : R; bitpos: [23:0]; default: 0;
 *  Custom MAC address
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
 *  Register 0 of BLOCK4 (KEY0).
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
 *  Register 1 of BLOCK4 (KEY0).
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
 *  Register 2 of BLOCK4 (KEY0).
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
 *  Register 3 of BLOCK4 (KEY0).
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
 *  Register 4 of BLOCK4 (KEY0).
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
 *  Register 5 of BLOCK4 (KEY0).
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
 *  Register 6 of BLOCK4 (KEY0).
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
 *  Register 7 of BLOCK4 (KEY0).
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
 *  Register 0 of BLOCK5 (KEY1).
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
 *  Register 1 of BLOCK5 (KEY1).
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
 *  Register 2 of BLOCK5 (KEY1).
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
 *  Register 3 of BLOCK5 (KEY1).
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
 *  Register 4 of BLOCK5 (KEY1).
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
 *  Register 5 of BLOCK5 (KEY1).
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
 *  Register 6 of BLOCK5 (KEY1).
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
 *  Register 7 of BLOCK5 (KEY1).
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
 *  Register 0 of BLOCK6 (KEY2).
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
 *  Register 1 of BLOCK6 (KEY2).
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
 *  Register 2 of BLOCK6 (KEY2).
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
 *  Register 3 of BLOCK6 (KEY2).
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
 *  Register 4 of BLOCK6 (KEY2).
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
 *  Register 5 of BLOCK6 (KEY2).
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
 *  Register 6 of BLOCK6 (KEY2).
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
 *  Register 7 of BLOCK6 (KEY2).
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
 *  Register 0 of BLOCK7 (KEY3).
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
 *  Register 1 of BLOCK7 (KEY3).
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
 *  Register 2 of BLOCK7 (KEY3).
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
 *  Register 3 of BLOCK7 (KEY3).
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
 *  Register 4 of BLOCK7 (KEY3).
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
 *  Register 5 of BLOCK7 (KEY3).
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
 *  Register 6 of BLOCK7 (KEY3).
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
 *  Register 7 of BLOCK7 (KEY3).
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
 *  Register 0 of BLOCK8 (KEY4).
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
 *  Register 1 of BLOCK8 (KEY4).
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
 *  Register 2 of BLOCK8 (KEY4).
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
 *  Register 3 of BLOCK8 (KEY4).
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
 *  Register 4 of BLOCK8 (KEY4).
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
 *  Register 5 of BLOCK8 (KEY4).
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
 *  Register 6 of BLOCK8 (KEY4).
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
 *  Register 7 of BLOCK8 (KEY4).
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
 *  Register 0 of BLOCK9 (KEY5).
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
 *  Register 1 of BLOCK9 (KEY5).
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
 *  Register 2 of BLOCK9 (KEY5).
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
 *  Register 3 of BLOCK9 (KEY5).
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
 *  Register 4 of BLOCK9 (KEY5).
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
 *  Register 5 of BLOCK9 (KEY5).
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
 *  Register 6 of BLOCK9 (KEY5).
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
 *  Register 7 of BLOCK9 (KEY5).
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
 *  Register 0 of BLOCK10 (system).
 */
#define EFUSE_RD_SYS_PART2_DATA0_REG (DR_REG_EFUSE_BASE + 0x15c)
/** EFUSE_SYS_DATA_PART2_0 : RO; bitpos: [31:0]; default: 0;
 *  Stores the 0th 32 bits of the 2nd part of system data.
 */
#define EFUSE_SYS_DATA_PART2_0    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_0_M  (EFUSE_SYS_DATA_PART2_0_V << EFUSE_SYS_DATA_PART2_0_S)
#define EFUSE_SYS_DATA_PART2_0_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_0_S  0

/** EFUSE_RD_SYS_PART2_DATA1_REG register
 *  Register 1 of BLOCK9 (KEY5).
 */
#define EFUSE_RD_SYS_PART2_DATA1_REG (DR_REG_EFUSE_BASE + 0x160)
/** EFUSE_SYS_DATA_PART2_1 : RO; bitpos: [31:0]; default: 0;
 *  Stores the 1st 32 bits of the 2nd part of system data.
 */
#define EFUSE_SYS_DATA_PART2_1    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_1_M  (EFUSE_SYS_DATA_PART2_1_V << EFUSE_SYS_DATA_PART2_1_S)
#define EFUSE_SYS_DATA_PART2_1_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_1_S  0

/** EFUSE_RD_SYS_PART2_DATA2_REG register
 *  Register 2 of BLOCK10 (system).
 */
#define EFUSE_RD_SYS_PART2_DATA2_REG (DR_REG_EFUSE_BASE + 0x164)
/** EFUSE_SYS_DATA_PART2_2 : RO; bitpos: [31:0]; default: 0;
 *  Stores the 2nd 32 bits of the 2nd part of system data.
 */
#define EFUSE_SYS_DATA_PART2_2    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_2_M  (EFUSE_SYS_DATA_PART2_2_V << EFUSE_SYS_DATA_PART2_2_S)
#define EFUSE_SYS_DATA_PART2_2_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_2_S  0

/** EFUSE_RD_SYS_PART2_DATA3_REG register
 *  Register 3 of BLOCK10 (system).
 */
#define EFUSE_RD_SYS_PART2_DATA3_REG (DR_REG_EFUSE_BASE + 0x168)
/** EFUSE_SYS_DATA_PART2_3 : RO; bitpos: [31:0]; default: 0;
 *  Stores the 3rd 32 bits of the 2nd part of system data.
 */
#define EFUSE_SYS_DATA_PART2_3    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_3_M  (EFUSE_SYS_DATA_PART2_3_V << EFUSE_SYS_DATA_PART2_3_S)
#define EFUSE_SYS_DATA_PART2_3_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_3_S  0

/** EFUSE_RD_SYS_PART2_DATA4_REG register
 *  Register 4 of BLOCK10 (system).
 */
#define EFUSE_RD_SYS_PART2_DATA4_REG (DR_REG_EFUSE_BASE + 0x16c)
/** EFUSE_SYS_DATA_PART2_4 : RO; bitpos: [31:0]; default: 0;
 *  Stores the 4th 32 bits of the 2nd part of system data.
 */
#define EFUSE_SYS_DATA_PART2_4    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_4_M  (EFUSE_SYS_DATA_PART2_4_V << EFUSE_SYS_DATA_PART2_4_S)
#define EFUSE_SYS_DATA_PART2_4_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_4_S  0

/** EFUSE_RD_SYS_PART2_DATA5_REG register
 *  Register 5 of BLOCK10 (system).
 */
#define EFUSE_RD_SYS_PART2_DATA5_REG (DR_REG_EFUSE_BASE + 0x170)
/** EFUSE_SYS_DATA_PART2_5 : RO; bitpos: [31:0]; default: 0;
 *  Stores the 5th 32 bits of the 2nd part of system data.
 */
#define EFUSE_SYS_DATA_PART2_5    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_5_M  (EFUSE_SYS_DATA_PART2_5_V << EFUSE_SYS_DATA_PART2_5_S)
#define EFUSE_SYS_DATA_PART2_5_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_5_S  0

/** EFUSE_RD_SYS_PART2_DATA6_REG register
 *  Register 6 of BLOCK10 (system).
 */
#define EFUSE_RD_SYS_PART2_DATA6_REG (DR_REG_EFUSE_BASE + 0x174)
/** EFUSE_SYS_DATA_PART2_6 : RO; bitpos: [31:0]; default: 0;
 *  Stores the 6th 32 bits of the 2nd part of system data.
 */
#define EFUSE_SYS_DATA_PART2_6    0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_6_M  (EFUSE_SYS_DATA_PART2_6_V << EFUSE_SYS_DATA_PART2_6_S)
#define EFUSE_SYS_DATA_PART2_6_V  0xFFFFFFFFU
#define EFUSE_SYS_DATA_PART2_6_S  0

/** EFUSE_RD_SYS_PART2_DATA7_REG register
 *  Register 7 of BLOCK10 (system).
 */
#define EFUSE_RD_SYS_PART2_DATA7_REG (DR_REG_EFUSE_BASE + 0x178)
/** EFUSE_SYS_DATA_PART2_7 : RO; bitpos: [31:0]; default: 0;
 *  Stores the 7th 32 bits of the 2nd part of system data.
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
 *  If any bit in RD_DIS is 1, then it indicates a programming error.
 */
#define EFUSE_RD_DIS_ERR    0x0000007FU
#define EFUSE_RD_DIS_ERR_M  (EFUSE_RD_DIS_ERR_V << EFUSE_RD_DIS_ERR_S)
#define EFUSE_RD_DIS_ERR_V  0x0000007FU
#define EFUSE_RD_DIS_ERR_S  0
/** EFUSE_DIS_RTC_RAM_BOOT_ERR : RO; bitpos: [7]; default: 0;
 *  If DIS_RTC_RAM_BOOT is 1, then it indicates a programming error.
 */
#define EFUSE_DIS_RTC_RAM_BOOT_ERR    (BIT(7))
#define EFUSE_DIS_RTC_RAM_BOOT_ERR_M  (EFUSE_DIS_RTC_RAM_BOOT_ERR_V << EFUSE_DIS_RTC_RAM_BOOT_ERR_S)
#define EFUSE_DIS_RTC_RAM_BOOT_ERR_V  0x00000001U
#define EFUSE_DIS_RTC_RAM_BOOT_ERR_S  7
/** EFUSE_DIS_ICACHE_ERR : RO; bitpos: [8]; default: 0;
 *  If DIS_ICACHE is 1, then it indicates a programming error.
 */
#define EFUSE_DIS_ICACHE_ERR    (BIT(8))
#define EFUSE_DIS_ICACHE_ERR_M  (EFUSE_DIS_ICACHE_ERR_V << EFUSE_DIS_ICACHE_ERR_S)
#define EFUSE_DIS_ICACHE_ERR_V  0x00000001U
#define EFUSE_DIS_ICACHE_ERR_S  8
/** EFUSE_DIS_USB_JTAG_ERR : RO; bitpos: [9]; default: 0;
 *  If DIS_USB_JTAG is 1, then it indicates a programming error.
 */
#define EFUSE_DIS_USB_JTAG_ERR    (BIT(9))
#define EFUSE_DIS_USB_JTAG_ERR_M  (EFUSE_DIS_USB_JTAG_ERR_V << EFUSE_DIS_USB_JTAG_ERR_S)
#define EFUSE_DIS_USB_JTAG_ERR_V  0x00000001U
#define EFUSE_DIS_USB_JTAG_ERR_S  9
/** EFUSE_DIS_DOWNLOAD_ICACHE_ERR : RO; bitpos: [10]; default: 0;
 *  If DIS_DOWNLOAD_ICACHE is 1, then it indicates a programming error.
 */
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR    (BIT(10))
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR_M  (EFUSE_DIS_DOWNLOAD_ICACHE_ERR_V << EFUSE_DIS_DOWNLOAD_ICACHE_ERR_S)
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR_V  0x00000001U
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR_S  10
/** EFUSE_DIS_USB_DEVICE_ERR : RO; bitpos: [11]; default: 0;
 *  If DIS_USB_DEVICE is 1, then it indicates a programming error.
 */
#define EFUSE_DIS_USB_DEVICE_ERR    (BIT(11))
#define EFUSE_DIS_USB_DEVICE_ERR_M  (EFUSE_DIS_USB_DEVICE_ERR_V << EFUSE_DIS_USB_DEVICE_ERR_S)
#define EFUSE_DIS_USB_DEVICE_ERR_V  0x00000001U
#define EFUSE_DIS_USB_DEVICE_ERR_S  11
/** EFUSE_DIS_FORCE_DOWNLOAD_ERR : RO; bitpos: [12]; default: 0;
 *  If DIS_FORCE_DOWNLOAD is 1, then it indicates a programming error.
 */
#define EFUSE_DIS_FORCE_DOWNLOAD_ERR    (BIT(12))
#define EFUSE_DIS_FORCE_DOWNLOAD_ERR_M  (EFUSE_DIS_FORCE_DOWNLOAD_ERR_V << EFUSE_DIS_FORCE_DOWNLOAD_ERR_S)
#define EFUSE_DIS_FORCE_DOWNLOAD_ERR_V  0x00000001U
#define EFUSE_DIS_FORCE_DOWNLOAD_ERR_S  12
/** EFUSE_RPT4_RESERVED6_ERR : RO; bitpos: [13]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED6_ERR    (BIT(13))
#define EFUSE_RPT4_RESERVED6_ERR_M  (EFUSE_RPT4_RESERVED6_ERR_V << EFUSE_RPT4_RESERVED6_ERR_S)
#define EFUSE_RPT4_RESERVED6_ERR_V  0x00000001U
#define EFUSE_RPT4_RESERVED6_ERR_S  13
/** EFUSE_DIS_CAN_ERR : RO; bitpos: [14]; default: 0;
 *  If DIS_CAN is 1, then it indicates a programming error.
 */
#define EFUSE_DIS_CAN_ERR    (BIT(14))
#define EFUSE_DIS_CAN_ERR_M  (EFUSE_DIS_CAN_ERR_V << EFUSE_DIS_CAN_ERR_S)
#define EFUSE_DIS_CAN_ERR_V  0x00000001U
#define EFUSE_DIS_CAN_ERR_S  14
/** EFUSE_JTAG_SEL_ENABLE_ERR : RO; bitpos: [15]; default: 0;
 *  If JTAG_SEL_ENABLE is 1, then it indicates a programming error.
 */
#define EFUSE_JTAG_SEL_ENABLE_ERR    (BIT(15))
#define EFUSE_JTAG_SEL_ENABLE_ERR_M  (EFUSE_JTAG_SEL_ENABLE_ERR_V << EFUSE_JTAG_SEL_ENABLE_ERR_S)
#define EFUSE_JTAG_SEL_ENABLE_ERR_V  0x00000001U
#define EFUSE_JTAG_SEL_ENABLE_ERR_S  15
/** EFUSE_SOFT_DIS_JTAG_ERR : RO; bitpos: [18:16]; default: 0;
 *  If SOFT_DIS_JTAG is 1, then it indicates a programming error.
 */
#define EFUSE_SOFT_DIS_JTAG_ERR    0x00000007U
#define EFUSE_SOFT_DIS_JTAG_ERR_M  (EFUSE_SOFT_DIS_JTAG_ERR_V << EFUSE_SOFT_DIS_JTAG_ERR_S)
#define EFUSE_SOFT_DIS_JTAG_ERR_V  0x00000007U
#define EFUSE_SOFT_DIS_JTAG_ERR_S  16
/** EFUSE_DIS_PAD_JTAG_ERR : RO; bitpos: [19]; default: 0;
 *  If DIS_PAD_JTAG is 1, then it indicates a programming error.
 */
#define EFUSE_DIS_PAD_JTAG_ERR    (BIT(19))
#define EFUSE_DIS_PAD_JTAG_ERR_M  (EFUSE_DIS_PAD_JTAG_ERR_V << EFUSE_DIS_PAD_JTAG_ERR_S)
#define EFUSE_DIS_PAD_JTAG_ERR_V  0x00000001U
#define EFUSE_DIS_PAD_JTAG_ERR_S  19
/** EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR : RO; bitpos: [20]; default: 0;
 *  If DIS_DOWNLOAD_MANUAL_ENCRYPT is 1, then it indicates a programming error.
 */
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR    (BIT(20))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_M  (EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_V << EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_S)
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_V  0x00000001U
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_S  20
/** EFUSE_USB_DREFH_ERR : RO; bitpos: [22:21]; default: 0;
 *  If any bit in USB_DREFH is 1, then it indicates a programming error.
 */
#define EFUSE_USB_DREFH_ERR    0x00000003U
#define EFUSE_USB_DREFH_ERR_M  (EFUSE_USB_DREFH_ERR_V << EFUSE_USB_DREFH_ERR_S)
#define EFUSE_USB_DREFH_ERR_V  0x00000003U
#define EFUSE_USB_DREFH_ERR_S  21
/** EFUSE_USB_DREFL_ERR : RO; bitpos: [24:23]; default: 0;
 *  If any bit in USB_DREFL is 1, then it indicates a programming error.
 */
#define EFUSE_USB_DREFL_ERR    0x00000003U
#define EFUSE_USB_DREFL_ERR_M  (EFUSE_USB_DREFL_ERR_V << EFUSE_USB_DREFL_ERR_S)
#define EFUSE_USB_DREFL_ERR_V  0x00000003U
#define EFUSE_USB_DREFL_ERR_S  23
/** EFUSE_USB_EXCHG_PINS_ERR : RO; bitpos: [25]; default: 0;
 *  If USB_EXCHG_PINS is 1, then it indicates a programming error.
 */
#define EFUSE_USB_EXCHG_PINS_ERR    (BIT(25))
#define EFUSE_USB_EXCHG_PINS_ERR_M  (EFUSE_USB_EXCHG_PINS_ERR_V << EFUSE_USB_EXCHG_PINS_ERR_S)
#define EFUSE_USB_EXCHG_PINS_ERR_V  0x00000001U
#define EFUSE_USB_EXCHG_PINS_ERR_S  25
/** EFUSE_VDD_SPI_AS_GPIO_ERR : RO; bitpos: [26]; default: 0;
 *  If VDD_SPI_AS_GPIO is 1, then it indicates a programming error.
 */
#define EFUSE_VDD_SPI_AS_GPIO_ERR    (BIT(26))
#define EFUSE_VDD_SPI_AS_GPIO_ERR_M  (EFUSE_VDD_SPI_AS_GPIO_ERR_V << EFUSE_VDD_SPI_AS_GPIO_ERR_S)
#define EFUSE_VDD_SPI_AS_GPIO_ERR_V  0x00000001U
#define EFUSE_VDD_SPI_AS_GPIO_ERR_S  26
/** EFUSE_BTLC_GPIO_ENABLE_ERR : RO; bitpos: [28:27]; default: 0;
 *  If any bit in BTLC_GPIO_ENABLE is 1, then it indicates a programming error.
 */
#define EFUSE_BTLC_GPIO_ENABLE_ERR    0x00000003U
#define EFUSE_BTLC_GPIO_ENABLE_ERR_M  (EFUSE_BTLC_GPIO_ENABLE_ERR_V << EFUSE_BTLC_GPIO_ENABLE_ERR_S)
#define EFUSE_BTLC_GPIO_ENABLE_ERR_V  0x00000003U
#define EFUSE_BTLC_GPIO_ENABLE_ERR_S  27
/** EFUSE_POWERGLITCH_EN_ERR : RO; bitpos: [29]; default: 0;
 *  If POWERGLITCH_EN is 1, then it indicates a programming error.
 */
#define EFUSE_POWERGLITCH_EN_ERR    (BIT(29))
#define EFUSE_POWERGLITCH_EN_ERR_M  (EFUSE_POWERGLITCH_EN_ERR_V << EFUSE_POWERGLITCH_EN_ERR_S)
#define EFUSE_POWERGLITCH_EN_ERR_V  0x00000001U
#define EFUSE_POWERGLITCH_EN_ERR_S  29
/** EFUSE_POWER_GLITCH_DSENSE_ERR : RO; bitpos: [31:30]; default: 0;
 *  If any bit in POWER_GLITCH_DSENSE is 1, then it indicates a programming error.
 */
#define EFUSE_POWER_GLITCH_DSENSE_ERR    0x00000003U
#define EFUSE_POWER_GLITCH_DSENSE_ERR_M  (EFUSE_POWER_GLITCH_DSENSE_ERR_V << EFUSE_POWER_GLITCH_DSENSE_ERR_S)
#define EFUSE_POWER_GLITCH_DSENSE_ERR_V  0x00000003U
#define EFUSE_POWER_GLITCH_DSENSE_ERR_S  30

/** EFUSE_RD_REPEAT_ERR1_REG register
 *  Programming error record register 1 of BLOCK0.
 */
#define EFUSE_RD_REPEAT_ERR1_REG (DR_REG_EFUSE_BASE + 0x180)
/** EFUSE_RPT4_RESERVED2_ERR : RO; bitpos: [15:0]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED2_ERR    0x0000FFFFU
#define EFUSE_RPT4_RESERVED2_ERR_M  (EFUSE_RPT4_RESERVED2_ERR_V << EFUSE_RPT4_RESERVED2_ERR_S)
#define EFUSE_RPT4_RESERVED2_ERR_V  0x0000FFFFU
#define EFUSE_RPT4_RESERVED2_ERR_S  0
/** EFUSE_WDT_DELAY_SEL_ERR : RO; bitpos: [17:16]; default: 0;
 *  If any bit in WDT_DELAY_SEL is 1, then it indicates a programming error.
 */
#define EFUSE_WDT_DELAY_SEL_ERR    0x00000003U
#define EFUSE_WDT_DELAY_SEL_ERR_M  (EFUSE_WDT_DELAY_SEL_ERR_V << EFUSE_WDT_DELAY_SEL_ERR_S)
#define EFUSE_WDT_DELAY_SEL_ERR_V  0x00000003U
#define EFUSE_WDT_DELAY_SEL_ERR_S  16
/** EFUSE_SPI_BOOT_CRYPT_CNT_ERR : RO; bitpos: [20:18]; default: 0;
 *  If any bit in SPI_BOOT_CRYPT_CNT is 1, then it indicates a programming error.
 */
#define EFUSE_SPI_BOOT_CRYPT_CNT_ERR    0x00000007U
#define EFUSE_SPI_BOOT_CRYPT_CNT_ERR_M  (EFUSE_SPI_BOOT_CRYPT_CNT_ERR_V << EFUSE_SPI_BOOT_CRYPT_CNT_ERR_S)
#define EFUSE_SPI_BOOT_CRYPT_CNT_ERR_V  0x00000007U
#define EFUSE_SPI_BOOT_CRYPT_CNT_ERR_S  18
/** EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR : RO; bitpos: [21]; default: 0;
 *  If SECURE_BOOT_KEY_REVOKE0 is 1, then it indicates a programming error.
 */
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR    (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR_M  (EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR_V << EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR_S)
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR_V  0x00000001U
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR_S  21
/** EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR : RO; bitpos: [22]; default: 0;
 *  If SECURE_BOOT_KEY_REVOKE1 is 1, then it indicates a programming error.
 */
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR    (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR_M  (EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR_V << EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR_S)
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR_V  0x00000001U
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR_S  22
/** EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR : RO; bitpos: [23]; default: 0;
 *  If SECURE_BOOT_KEY_REVOKE2 is 1, then it indicates a programming error.
 */
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR    (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR_M  (EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR_V << EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR_S)
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR_V  0x00000001U
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR_S  23
/** EFUSE_KEY_PURPOSE_0_ERR : RO; bitpos: [27:24]; default: 0;
 *  If any bit in KEY_PURPOSE_0 is 1, then it indicates a programming error.
 */
#define EFUSE_KEY_PURPOSE_0_ERR    0x0000000FU
#define EFUSE_KEY_PURPOSE_0_ERR_M  (EFUSE_KEY_PURPOSE_0_ERR_V << EFUSE_KEY_PURPOSE_0_ERR_S)
#define EFUSE_KEY_PURPOSE_0_ERR_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_0_ERR_S  24
/** EFUSE_KEY_PURPOSE_1_ERR : RO; bitpos: [31:28]; default: 0;
 *  If any bit in KEY_PURPOSE_1 is 1, then it indicates a programming error.
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
 *  If any bit in KEY_PURPOSE_2 is 1, then it indicates a programming error.
 */
#define EFUSE_KEY_PURPOSE_2_ERR    0x0000000FU
#define EFUSE_KEY_PURPOSE_2_ERR_M  (EFUSE_KEY_PURPOSE_2_ERR_V << EFUSE_KEY_PURPOSE_2_ERR_S)
#define EFUSE_KEY_PURPOSE_2_ERR_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_2_ERR_S  0
/** EFUSE_KEY_PURPOSE_3_ERR : RO; bitpos: [7:4]; default: 0;
 *  If any bit in KEY_PURPOSE_3 is 1, then it indicates a programming error.
 */
#define EFUSE_KEY_PURPOSE_3_ERR    0x0000000FU
#define EFUSE_KEY_PURPOSE_3_ERR_M  (EFUSE_KEY_PURPOSE_3_ERR_V << EFUSE_KEY_PURPOSE_3_ERR_S)
#define EFUSE_KEY_PURPOSE_3_ERR_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_3_ERR_S  4
/** EFUSE_KEY_PURPOSE_4_ERR : RO; bitpos: [11:8]; default: 0;
 *  If any bit in KEY_PURPOSE_4 is 1, then it indicates a programming error.
 */
#define EFUSE_KEY_PURPOSE_4_ERR    0x0000000FU
#define EFUSE_KEY_PURPOSE_4_ERR_M  (EFUSE_KEY_PURPOSE_4_ERR_V << EFUSE_KEY_PURPOSE_4_ERR_S)
#define EFUSE_KEY_PURPOSE_4_ERR_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_4_ERR_S  8
/** EFUSE_KEY_PURPOSE_5_ERR : RO; bitpos: [15:12]; default: 0;
 *  If any bit in KEY_PURPOSE_5 is 1, then it indicates a programming error.
 */
#define EFUSE_KEY_PURPOSE_5_ERR    0x0000000FU
#define EFUSE_KEY_PURPOSE_5_ERR_M  (EFUSE_KEY_PURPOSE_5_ERR_V << EFUSE_KEY_PURPOSE_5_ERR_S)
#define EFUSE_KEY_PURPOSE_5_ERR_V  0x0000000FU
#define EFUSE_KEY_PURPOSE_5_ERR_S  12
/** EFUSE_RPT4_RESERVED3_ERR : RO; bitpos: [19:16]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED3_ERR    0x0000000FU
#define EFUSE_RPT4_RESERVED3_ERR_M  (EFUSE_RPT4_RESERVED3_ERR_V << EFUSE_RPT4_RESERVED3_ERR_S)
#define EFUSE_RPT4_RESERVED3_ERR_V  0x0000000FU
#define EFUSE_RPT4_RESERVED3_ERR_S  16
/** EFUSE_SECURE_BOOT_EN_ERR : RO; bitpos: [20]; default: 0;
 *  If SECURE_BOOT_EN is 1, then it indicates a programming error.
 */
#define EFUSE_SECURE_BOOT_EN_ERR    (BIT(20))
#define EFUSE_SECURE_BOOT_EN_ERR_M  (EFUSE_SECURE_BOOT_EN_ERR_V << EFUSE_SECURE_BOOT_EN_ERR_S)
#define EFUSE_SECURE_BOOT_EN_ERR_V  0x00000001U
#define EFUSE_SECURE_BOOT_EN_ERR_S  20
/** EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR : RO; bitpos: [21]; default: 0;
 *  If SECURE_BOOT_AGGRESSIVE_REVOKE is 1, then it indicates a programming error.
 */
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR    (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_M  (EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_V << EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_S)
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_V  0x00000001U
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_S  21
/** EFUSE_RPT4_RESERVED0_ERR : RO; bitpos: [27:22]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED0_ERR    0x0000003FU
#define EFUSE_RPT4_RESERVED0_ERR_M  (EFUSE_RPT4_RESERVED0_ERR_V << EFUSE_RPT4_RESERVED0_ERR_S)
#define EFUSE_RPT4_RESERVED0_ERR_V  0x0000003FU
#define EFUSE_RPT4_RESERVED0_ERR_S  22
/** EFUSE_FLASH_TPUW_ERR : RO; bitpos: [31:28]; default: 0;
 *  If any bit in FLASH_TPUM is 1, then it indicates a programming error.
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
 *  If DIS_DOWNLOAD_MODE is 1, then it indicates a programming error.
 */
#define EFUSE_DIS_DOWNLOAD_MODE_ERR    (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_ERR_M  (EFUSE_DIS_DOWNLOAD_MODE_ERR_V << EFUSE_DIS_DOWNLOAD_MODE_ERR_S)
#define EFUSE_DIS_DOWNLOAD_MODE_ERR_V  0x00000001U
#define EFUSE_DIS_DOWNLOAD_MODE_ERR_S  0
/** EFUSE_DIS_LEGACY_SPI_BOOT_ERR : RO; bitpos: [1]; default: 0;
 *  If DIS_LEGACY_SPI_BOOT is 1, then it indicates a programming error.
 */
#define EFUSE_DIS_LEGACY_SPI_BOOT_ERR    (BIT(1))
#define EFUSE_DIS_LEGACY_SPI_BOOT_ERR_M  (EFUSE_DIS_LEGACY_SPI_BOOT_ERR_V << EFUSE_DIS_LEGACY_SPI_BOOT_ERR_S)
#define EFUSE_DIS_LEGACY_SPI_BOOT_ERR_V  0x00000001U
#define EFUSE_DIS_LEGACY_SPI_BOOT_ERR_S  1
/** EFUSE_UART_PRINT_CHANNEL_ERR : RO; bitpos: [2]; default: 0;
 *  If UART_PRINT_CHANNEL is 1, then it indicates a programming error.
 */
#define EFUSE_UART_PRINT_CHANNEL_ERR    (BIT(2))
#define EFUSE_UART_PRINT_CHANNEL_ERR_M  (EFUSE_UART_PRINT_CHANNEL_ERR_V << EFUSE_UART_PRINT_CHANNEL_ERR_S)
#define EFUSE_UART_PRINT_CHANNEL_ERR_V  0x00000001U
#define EFUSE_UART_PRINT_CHANNEL_ERR_S  2
/** EFUSE_FLASH_ECC_MODE_ERR : RO; bitpos: [3]; default: 0;
 *  If FLASH_ECC_MODE is 1, then it indicates a programming error.
 */
#define EFUSE_FLASH_ECC_MODE_ERR    (BIT(3))
#define EFUSE_FLASH_ECC_MODE_ERR_M  (EFUSE_FLASH_ECC_MODE_ERR_V << EFUSE_FLASH_ECC_MODE_ERR_S)
#define EFUSE_FLASH_ECC_MODE_ERR_V  0x00000001U
#define EFUSE_FLASH_ECC_MODE_ERR_S  3
/** EFUSE_DIS_USB_DOWNLOAD_MODE_ERR : RO; bitpos: [4]; default: 0;
 *  If DIS_USB_DOWNLOAD_MODE is 1, then it indicates a programming error.
 */
#define EFUSE_DIS_USB_DOWNLOAD_MODE_ERR    (BIT(4))
#define EFUSE_DIS_USB_DOWNLOAD_MODE_ERR_M  (EFUSE_DIS_USB_DOWNLOAD_MODE_ERR_V << EFUSE_DIS_USB_DOWNLOAD_MODE_ERR_S)
#define EFUSE_DIS_USB_DOWNLOAD_MODE_ERR_V  0x00000001U
#define EFUSE_DIS_USB_DOWNLOAD_MODE_ERR_S  4
/** EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR : RO; bitpos: [5]; default: 0;
 *  If ENABLE_SECURITY_DOWNLOAD is 1, then it indicates a programming error.
 */
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR    (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_M  (EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_V << EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_S)
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_V  0x00000001U
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_S  5
/** EFUSE_UART_PRINT_CONTROL_ERR : RO; bitpos: [7:6]; default: 0;
 *  If any bit in UART_PRINT_CONTROL is 1, then it indicates a programming error.
 */
#define EFUSE_UART_PRINT_CONTROL_ERR    0x00000003U
#define EFUSE_UART_PRINT_CONTROL_ERR_M  (EFUSE_UART_PRINT_CONTROL_ERR_V << EFUSE_UART_PRINT_CONTROL_ERR_S)
#define EFUSE_UART_PRINT_CONTROL_ERR_V  0x00000003U
#define EFUSE_UART_PRINT_CONTROL_ERR_S  6
/** EFUSE_PIN_POWER_SELECTION_ERR : RO; bitpos: [8]; default: 0;
 *  If PIN_POWER_SELECTION is 1, then it indicates a programming error.
 */
#define EFUSE_PIN_POWER_SELECTION_ERR    (BIT(8))
#define EFUSE_PIN_POWER_SELECTION_ERR_M  (EFUSE_PIN_POWER_SELECTION_ERR_V << EFUSE_PIN_POWER_SELECTION_ERR_S)
#define EFUSE_PIN_POWER_SELECTION_ERR_V  0x00000001U
#define EFUSE_PIN_POWER_SELECTION_ERR_S  8
/** EFUSE_FLASH_TYPE_ERR : RO; bitpos: [9]; default: 0;
 *  If FLASH_TYPE is 1, then it indicates a programming error.
 */
#define EFUSE_FLASH_TYPE_ERR    (BIT(9))
#define EFUSE_FLASH_TYPE_ERR_M  (EFUSE_FLASH_TYPE_ERR_V << EFUSE_FLASH_TYPE_ERR_S)
#define EFUSE_FLASH_TYPE_ERR_V  0x00000001U
#define EFUSE_FLASH_TYPE_ERR_S  9
/** EFUSE_FLASH_PAGE_SIZE_ERR : RO; bitpos: [11:10]; default: 0;
 *  If any bits in FLASH_PAGE_SIZE is 1, then it indicates a programming error.
 */
#define EFUSE_FLASH_PAGE_SIZE_ERR    0x00000003U
#define EFUSE_FLASH_PAGE_SIZE_ERR_M  (EFUSE_FLASH_PAGE_SIZE_ERR_V << EFUSE_FLASH_PAGE_SIZE_ERR_S)
#define EFUSE_FLASH_PAGE_SIZE_ERR_V  0x00000003U
#define EFUSE_FLASH_PAGE_SIZE_ERR_S  10
/** EFUSE_FLASH_ECC_EN_ERR : RO; bitpos: [12]; default: 0;
 *  If FLASH_ECC_EN_ERR is 1, then it indicates a programming error.
 */
#define EFUSE_FLASH_ECC_EN_ERR    (BIT(12))
#define EFUSE_FLASH_ECC_EN_ERR_M  (EFUSE_FLASH_ECC_EN_ERR_V << EFUSE_FLASH_ECC_EN_ERR_S)
#define EFUSE_FLASH_ECC_EN_ERR_V  0x00000001U
#define EFUSE_FLASH_ECC_EN_ERR_S  12
/** EFUSE_FORCE_SEND_RESUME_ERR : RO; bitpos: [13]; default: 0;
 *  If FORCE_SEND_RESUME is 1, then it indicates a programming error.
 */
#define EFUSE_FORCE_SEND_RESUME_ERR    (BIT(13))
#define EFUSE_FORCE_SEND_RESUME_ERR_M  (EFUSE_FORCE_SEND_RESUME_ERR_V << EFUSE_FORCE_SEND_RESUME_ERR_S)
#define EFUSE_FORCE_SEND_RESUME_ERR_V  0x00000001U
#define EFUSE_FORCE_SEND_RESUME_ERR_S  13
/** EFUSE_SECURE_VERSION_ERR : RO; bitpos: [29:14]; default: 0;
 *  If any bit in SECURE_VERSION is 1, then it indicates a programming error.
 */
#define EFUSE_SECURE_VERSION_ERR    0x0000FFFFU
#define EFUSE_SECURE_VERSION_ERR_M  (EFUSE_SECURE_VERSION_ERR_V << EFUSE_SECURE_VERSION_ERR_S)
#define EFUSE_SECURE_VERSION_ERR_V  0x0000FFFFU
#define EFUSE_SECURE_VERSION_ERR_S  14
/** EFUSE_RPT4_RESERVED1_ERR : RO; bitpos: [31:30]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED1_ERR    0x00000003U
#define EFUSE_RPT4_RESERVED1_ERR_M  (EFUSE_RPT4_RESERVED1_ERR_V << EFUSE_RPT4_RESERVED1_ERR_S)
#define EFUSE_RPT4_RESERVED1_ERR_V  0x00000003U
#define EFUSE_RPT4_RESERVED1_ERR_S  30

/** EFUSE_RD_REPEAT_ERR4_REG register
 *  Programming error record register 4 of BLOCK0.
 */
#define EFUSE_RD_REPEAT_ERR4_REG (DR_REG_EFUSE_BASE + 0x190)
/** EFUSE_RPT4_RESERVED4_ERR : RO; bitpos: [23:0]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED4_ERR    0x00FFFFFFU
#define EFUSE_RPT4_RESERVED4_ERR_M  (EFUSE_RPT4_RESERVED4_ERR_V << EFUSE_RPT4_RESERVED4_ERR_S)
#define EFUSE_RPT4_RESERVED4_ERR_V  0x00FFFFFFU
#define EFUSE_RPT4_RESERVED4_ERR_S  0

/** EFUSE_RD_RS_ERR0_REG register
 *  Programming error record register 0 of BLOCK1-10.
 */
#define EFUSE_RD_RS_ERR0_REG (DR_REG_EFUSE_BASE + 0x1c0)
/** EFUSE_MAC_SPI_8M_ERR_NUM : RO; bitpos: [2:0]; default: 0;
 *  The value of this signal means the number of error bytes.
 */
#define EFUSE_MAC_SPI_8M_ERR_NUM    0x00000007U
#define EFUSE_MAC_SPI_8M_ERR_NUM_M  (EFUSE_MAC_SPI_8M_ERR_NUM_V << EFUSE_MAC_SPI_8M_ERR_NUM_S)
#define EFUSE_MAC_SPI_8M_ERR_NUM_V  0x00000007U
#define EFUSE_MAC_SPI_8M_ERR_NUM_S  0
/** EFUSE_RESERVED_FAIL : RO; bitpos: [3]; default: 0;
 *  0: Means no failure and that the data of MAC_SPI_8M is reliable 1: Means that
 *  programming user data failed and the number of error bytes is over 6.
 */
#define EFUSE_RESERVED_FAIL    (BIT(3))
#define EFUSE_RESERVED_FAIL_M  (EFUSE_RESERVED_FAIL_V << EFUSE_RESERVED_FAIL_S)
#define EFUSE_RESERVED_FAIL_V  0x00000001U
#define EFUSE_RESERVED_FAIL_S  3
/** EFUSE_SYS_PART1_NUM : RO; bitpos: [6:4]; default: 0;
 *  The value of this signal means the number of error bytes.
 */
#define EFUSE_SYS_PART1_NUM    0x00000007U
#define EFUSE_SYS_PART1_NUM_M  (EFUSE_SYS_PART1_NUM_V << EFUSE_SYS_PART1_NUM_S)
#define EFUSE_SYS_PART1_NUM_V  0x00000007U
#define EFUSE_SYS_PART1_NUM_S  4
/** EFUSE_MAC_SPI_8M_FAIL : RO; bitpos: [7]; default: 0;
 *  0: Means no failure and that the data of system part1 is reliable 1: Means that
 *  programming user data failed and the number of error bytes is over 6.
 */
#define EFUSE_MAC_SPI_8M_FAIL    (BIT(7))
#define EFUSE_MAC_SPI_8M_FAIL_M  (EFUSE_MAC_SPI_8M_FAIL_V << EFUSE_MAC_SPI_8M_FAIL_S)
#define EFUSE_MAC_SPI_8M_FAIL_V  0x00000001U
#define EFUSE_MAC_SPI_8M_FAIL_S  7
/** EFUSE_USR_DATA_ERR_NUM : RO; bitpos: [10:8]; default: 0;
 *  The value of this signal means the number of error bytes.
 */
#define EFUSE_USR_DATA_ERR_NUM    0x00000007U
#define EFUSE_USR_DATA_ERR_NUM_M  (EFUSE_USR_DATA_ERR_NUM_V << EFUSE_USR_DATA_ERR_NUM_S)
#define EFUSE_USR_DATA_ERR_NUM_V  0x00000007U
#define EFUSE_USR_DATA_ERR_NUM_S  8
/** EFUSE_SYS_PART1_FAIL : RO; bitpos: [11]; default: 0;
 *  0: Means no failure and that the user data is reliable 1: Means that programming
 *  user data failed and the number of error bytes is over 6.
 */
#define EFUSE_SYS_PART1_FAIL    (BIT(11))
#define EFUSE_SYS_PART1_FAIL_M  (EFUSE_SYS_PART1_FAIL_V << EFUSE_SYS_PART1_FAIL_S)
#define EFUSE_SYS_PART1_FAIL_V  0x00000001U
#define EFUSE_SYS_PART1_FAIL_S  11
/** EFUSE_KEY0_ERR_NUM : RO; bitpos: [14:12]; default: 0;
 *  The value of this signal means the number of error bytes.
 */
#define EFUSE_KEY0_ERR_NUM    0x00000007U
#define EFUSE_KEY0_ERR_NUM_M  (EFUSE_KEY0_ERR_NUM_V << EFUSE_KEY0_ERR_NUM_S)
#define EFUSE_KEY0_ERR_NUM_V  0x00000007U
#define EFUSE_KEY0_ERR_NUM_S  12
/** EFUSE_USR_DATA_FAIL : RO; bitpos: [15]; default: 0;
 *  0: Means no failure and that the data of key0 is reliable 1: Means that programming
 *  key0 failed and the number of error bytes is over 6.
 */
#define EFUSE_USR_DATA_FAIL    (BIT(15))
#define EFUSE_USR_DATA_FAIL_M  (EFUSE_USR_DATA_FAIL_V << EFUSE_USR_DATA_FAIL_S)
#define EFUSE_USR_DATA_FAIL_V  0x00000001U
#define EFUSE_USR_DATA_FAIL_S  15
/** EFUSE_KEY1_ERR_NUM : RO; bitpos: [18:16]; default: 0;
 *  The value of this signal means the number of error bytes.
 */
#define EFUSE_KEY1_ERR_NUM    0x00000007U
#define EFUSE_KEY1_ERR_NUM_M  (EFUSE_KEY1_ERR_NUM_V << EFUSE_KEY1_ERR_NUM_S)
#define EFUSE_KEY1_ERR_NUM_V  0x00000007U
#define EFUSE_KEY1_ERR_NUM_S  16
/** EFUSE_KEY0_FAIL : RO; bitpos: [19]; default: 0;
 *  0: Means no failure and that the data of key1 is reliable 1: Means that programming
 *  key1 failed and the number of error bytes is over 6.
 */
#define EFUSE_KEY0_FAIL    (BIT(19))
#define EFUSE_KEY0_FAIL_M  (EFUSE_KEY0_FAIL_V << EFUSE_KEY0_FAIL_S)
#define EFUSE_KEY0_FAIL_V  0x00000001U
#define EFUSE_KEY0_FAIL_S  19
/** EFUSE_KEY2_ERR_NUM : RO; bitpos: [22:20]; default: 0;
 *  The value of this signal means the number of error bytes.
 */
#define EFUSE_KEY2_ERR_NUM    0x00000007U
#define EFUSE_KEY2_ERR_NUM_M  (EFUSE_KEY2_ERR_NUM_V << EFUSE_KEY2_ERR_NUM_S)
#define EFUSE_KEY2_ERR_NUM_V  0x00000007U
#define EFUSE_KEY2_ERR_NUM_S  20
/** EFUSE_KEY1_FAIL : RO; bitpos: [23]; default: 0;
 *  0: Means no failure and that the data of key2 is reliable 1: Means that programming
 *  key2 failed and the number of error bytes is over 6.
 */
#define EFUSE_KEY1_FAIL    (BIT(23))
#define EFUSE_KEY1_FAIL_M  (EFUSE_KEY1_FAIL_V << EFUSE_KEY1_FAIL_S)
#define EFUSE_KEY1_FAIL_V  0x00000001U
#define EFUSE_KEY1_FAIL_S  23
/** EFUSE_KEY3_ERR_NUM : RO; bitpos: [26:24]; default: 0;
 *  The value of this signal means the number of error bytes.
 */
#define EFUSE_KEY3_ERR_NUM    0x00000007U
#define EFUSE_KEY3_ERR_NUM_M  (EFUSE_KEY3_ERR_NUM_V << EFUSE_KEY3_ERR_NUM_S)
#define EFUSE_KEY3_ERR_NUM_V  0x00000007U
#define EFUSE_KEY3_ERR_NUM_S  24
/** EFUSE_KEY2_FAIL : RO; bitpos: [27]; default: 0;
 *  0: Means no failure and that the data of key3 is reliable 1: Means that programming
 *  key3 failed and the number of error bytes is over 6.
 */
#define EFUSE_KEY2_FAIL    (BIT(27))
#define EFUSE_KEY2_FAIL_M  (EFUSE_KEY2_FAIL_V << EFUSE_KEY2_FAIL_S)
#define EFUSE_KEY2_FAIL_V  0x00000001U
#define EFUSE_KEY2_FAIL_S  27
/** EFUSE_KEY4_ERR_NUM : RO; bitpos: [30:28]; default: 0;
 *  The value of this signal means the number of error bytes.
 */
#define EFUSE_KEY4_ERR_NUM    0x00000007U
#define EFUSE_KEY4_ERR_NUM_M  (EFUSE_KEY4_ERR_NUM_V << EFUSE_KEY4_ERR_NUM_S)
#define EFUSE_KEY4_ERR_NUM_V  0x00000007U
#define EFUSE_KEY4_ERR_NUM_S  28
/** EFUSE_KEY3_FAIL : RO; bitpos: [31]; default: 0;
 *  0: Means no failure and that the data of key4 is reliable 1: Means that programming
 *  key4 failed and the number of error bytes is over 6.
 */
#define EFUSE_KEY3_FAIL    (BIT(31))
#define EFUSE_KEY3_FAIL_M  (EFUSE_KEY3_FAIL_V << EFUSE_KEY3_FAIL_S)
#define EFUSE_KEY3_FAIL_V  0x00000001U
#define EFUSE_KEY3_FAIL_S  31

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
/** EFUSE_KEY4_FAIL : RO; bitpos: [3]; default: 0;
 *  0: Means no failure and that the data of KEY5 is reliable 1: Means that programming
 *  user data failed and the number of error bytes is over 6.
 */
#define EFUSE_KEY4_FAIL    (BIT(3))
#define EFUSE_KEY4_FAIL_M  (EFUSE_KEY4_FAIL_V << EFUSE_KEY4_FAIL_S)
#define EFUSE_KEY4_FAIL_V  0x00000001U
#define EFUSE_KEY4_FAIL_S  3
/** EFUSE_SYS_PART2_ERR_NUM : RO; bitpos: [6:4]; default: 0;
 *  The value of this signal means the number of error bytes.
 */
#define EFUSE_SYS_PART2_ERR_NUM    0x00000007U
#define EFUSE_SYS_PART2_ERR_NUM_M  (EFUSE_SYS_PART2_ERR_NUM_V << EFUSE_SYS_PART2_ERR_NUM_S)
#define EFUSE_SYS_PART2_ERR_NUM_V  0x00000007U
#define EFUSE_SYS_PART2_ERR_NUM_S  4
/** EFUSE_KEY5_FAIL : RO; bitpos: [7]; default: 0;
 *  0: Means no failure and that the data of system part2 is reliable 1: Means that
 *  programming user data failed and the number of error bytes is over 6.
 */
#define EFUSE_KEY5_FAIL    (BIT(7))
#define EFUSE_KEY5_FAIL_M  (EFUSE_KEY5_FAIL_V << EFUSE_KEY5_FAIL_S)
#define EFUSE_KEY5_FAIL_V  0x00000001U
#define EFUSE_KEY5_FAIL_S  7

/** EFUSE_CLK_REG register
 *  eFuse clcok configuration register.
 */
#define EFUSE_CLK_REG (DR_REG_EFUSE_BASE + 0x1c8)
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
 *  eFuse operation mode configuraiton register;
 */
#define EFUSE_CONF_REG (DR_REG_EFUSE_BASE + 0x1cc)
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
/** EFUSE_REPEAT_ERR_CNT : RO; bitpos: [17:10]; default: 0;
 *  Indicates the number of error bits during programming BLOCK0.
 */
#define EFUSE_REPEAT_ERR_CNT    0x000000FFU
#define EFUSE_REPEAT_ERR_CNT_M  (EFUSE_REPEAT_ERR_CNT_V << EFUSE_REPEAT_ERR_CNT_S)
#define EFUSE_REPEAT_ERR_CNT_V  0x000000FFU
#define EFUSE_REPEAT_ERR_CNT_S  10

/** EFUSE_CMD_REG register
 *  eFuse command register.
 */
#define EFUSE_CMD_REG (DR_REG_EFUSE_BASE + 0x1d4)
/** EFUSE_READ_CMD : R/WS/SC; bitpos: [0]; default: 0;
 *  Set this bit to send read command.
 */
#define EFUSE_READ_CMD    (BIT(0))
#define EFUSE_READ_CMD_M  (EFUSE_READ_CMD_V << EFUSE_READ_CMD_S)
#define EFUSE_READ_CMD_V  0x00000001U
#define EFUSE_READ_CMD_S  0
/** EFUSE_PGM_CMD : R/WS/SC; bitpos: [1]; default: 0;
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
/** EFUSE_READ_DONE_INT_RAW : R/WC/SS; bitpos: [0]; default: 0;
 *  The raw bit signal for read_done interrupt.
 */
#define EFUSE_READ_DONE_INT_RAW    (BIT(0))
#define EFUSE_READ_DONE_INT_RAW_M  (EFUSE_READ_DONE_INT_RAW_V << EFUSE_READ_DONE_INT_RAW_S)
#define EFUSE_READ_DONE_INT_RAW_V  0x00000001U
#define EFUSE_READ_DONE_INT_RAW_S  0
/** EFUSE_PGM_DONE_INT_RAW : R/WC/SS; bitpos: [1]; default: 0;
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
/** EFUSE_READ_DONE_INT_CLR : WO; bitpos: [0]; default: 0;
 *  The clear signal for read_done interrupt.
 */
#define EFUSE_READ_DONE_INT_CLR    (BIT(0))
#define EFUSE_READ_DONE_INT_CLR_M  (EFUSE_READ_DONE_INT_CLR_V << EFUSE_READ_DONE_INT_CLR_S)
#define EFUSE_READ_DONE_INT_CLR_V  0x00000001U
#define EFUSE_READ_DONE_INT_CLR_S  0
/** EFUSE_PGM_DONE_INT_CLR : WO; bitpos: [1]; default: 0;
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
#define EFUSE_RD_TIM_CONF_REG (DR_REG_EFUSE_BASE + 0x1ec)
/** EFUSE_READ_INIT_NUM : R/W; bitpos: [31:24]; default: 18;
 *  Configures the initial read time of eFuse.
 */
#define EFUSE_READ_INIT_NUM    0x000000FFU
#define EFUSE_READ_INIT_NUM_M  (EFUSE_READ_INIT_NUM_V << EFUSE_READ_INIT_NUM_S)
#define EFUSE_READ_INIT_NUM_V  0x000000FFU
#define EFUSE_READ_INIT_NUM_S  24

/** EFUSE_WR_TIM_CONF1_REG register
 *  Configurarion register 1 of eFuse programming timing parameters.
 */
#define EFUSE_WR_TIM_CONF1_REG (DR_REG_EFUSE_BASE + 0x1f0)
/** EFUSE_PWR_ON_NUM : R/W; bitpos: [23:8]; default: 10368;
 *  Configures the power up time for VDDQ.
 */
#define EFUSE_PWR_ON_NUM    0x0000FFFFU
#define EFUSE_PWR_ON_NUM_M  (EFUSE_PWR_ON_NUM_V << EFUSE_PWR_ON_NUM_S)
#define EFUSE_PWR_ON_NUM_V  0x0000FFFFU
#define EFUSE_PWR_ON_NUM_S  8

/** EFUSE_WR_TIM_CONF2_REG register
 *  Configurarion register 2 of eFuse programming timing parameters.
 */
#define EFUSE_WR_TIM_CONF2_REG (DR_REG_EFUSE_BASE + 0x1f4)
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
/** EFUSE_DATE : R/W; bitpos: [27:0]; default: 33583616;
 *  Stores eFuse version.
 */
#define EFUSE_DATE    0x0FFFFFFFU
#define EFUSE_DATE_M  (EFUSE_DATE_V << EFUSE_DATE_S)
#define EFUSE_DATE_V  0x0FFFFFFFU
#define EFUSE_DATE_S  0

#ifdef __cplusplus
}
#endif
