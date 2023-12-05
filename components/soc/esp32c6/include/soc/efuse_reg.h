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
/** EFUSE_SWAP_UART_SDIO_EN : RO; bitpos: [7]; default: 0;
 *  Represents whether pad of uart and sdio is swapped or not. 1: swapped. 0: not
 *  swapped.
 */
#define EFUSE_SWAP_UART_SDIO_EN    (BIT(7))
#define EFUSE_SWAP_UART_SDIO_EN_M  (EFUSE_SWAP_UART_SDIO_EN_V << EFUSE_SWAP_UART_SDIO_EN_S)
#define EFUSE_SWAP_UART_SDIO_EN_V  0x00000001U
#define EFUSE_SWAP_UART_SDIO_EN_S  7
/** EFUSE_DIS_ICACHE : RO; bitpos: [8]; default: 0;
 *  Represents whether icache is disabled or enabled. 1: disabled. 0: enabled.
 */
#define EFUSE_DIS_ICACHE    (BIT(8))
#define EFUSE_DIS_ICACHE_M  (EFUSE_DIS_ICACHE_V << EFUSE_DIS_ICACHE_S)
#define EFUSE_DIS_ICACHE_V  0x00000001U
#define EFUSE_DIS_ICACHE_S  8
/** EFUSE_DIS_USB_JTAG : RO; bitpos: [9]; default: 0;
 *  Represents whether the function of usb switch to jtag is disabled or enabled. 1:
 *  disabled. 0: enabled.
 */
#define EFUSE_DIS_USB_JTAG    (BIT(9))
#define EFUSE_DIS_USB_JTAG_M  (EFUSE_DIS_USB_JTAG_V << EFUSE_DIS_USB_JTAG_S)
#define EFUSE_DIS_USB_JTAG_V  0x00000001U
#define EFUSE_DIS_USB_JTAG_S  9
/** EFUSE_DIS_DOWNLOAD_ICACHE : RO; bitpos: [10]; default: 0;
 *  Represents whether icache is disabled or enabled in Download mode. 1: disabled. 0:
 *  enabled.
 */
#define EFUSE_DIS_DOWNLOAD_ICACHE    (BIT(10))
#define EFUSE_DIS_DOWNLOAD_ICACHE_M  (EFUSE_DIS_DOWNLOAD_ICACHE_V << EFUSE_DIS_DOWNLOAD_ICACHE_S)
#define EFUSE_DIS_DOWNLOAD_ICACHE_V  0x00000001U
#define EFUSE_DIS_DOWNLOAD_ICACHE_S  10
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
 *  Represents whether SPI0 controller during boot_mode_download is disabled or
 *  enabled. 1: disabled. 0: enabled.
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
/** EFUSE_USB_DREFH : RO; bitpos: [22:21]; default: 0;
 *  Represents the single-end input threhold vrefh, 1.76 V to 2 V with step of 80 mV.
 */
#define EFUSE_USB_DREFH    0x00000003U
#define EFUSE_USB_DREFH_M  (EFUSE_USB_DREFH_V << EFUSE_USB_DREFH_S)
#define EFUSE_USB_DREFH_V  0x00000003U
#define EFUSE_USB_DREFH_S  21
/** EFUSE_USB_DREFL : RO; bitpos: [24:23]; default: 0;
 *  Represents the single-end input threhold vrefl, 1.76 V to 2 V with step of 80 mV.
 */
#define EFUSE_USB_DREFL    0x00000003U
#define EFUSE_USB_DREFL_M  (EFUSE_USB_DREFL_V << EFUSE_USB_DREFL_S)
#define EFUSE_USB_DREFL_V  0x00000003U
#define EFUSE_USB_DREFL_S  23
/** EFUSE_USB_EXCHG_PINS : RO; bitpos: [25]; default: 0;
 *  Represents whether the D+ and D- pins is exchanged. 1: exchanged. 0: not exchanged.
 */
#define EFUSE_USB_EXCHG_PINS    (BIT(25))
#define EFUSE_USB_EXCHG_PINS_M  (EFUSE_USB_EXCHG_PINS_V << EFUSE_USB_EXCHG_PINS_S)
#define EFUSE_USB_EXCHG_PINS_V  0x00000001U
#define EFUSE_USB_EXCHG_PINS_S  25
/** EFUSE_VDD_SPI_AS_GPIO : RO; bitpos: [26]; default: 0;
 *  Represents whether vdd spi pin is functioned as gpio. 1: functioned. 0: not
 *  functioned.
 */
#define EFUSE_VDD_SPI_AS_GPIO    (BIT(26))
#define EFUSE_VDD_SPI_AS_GPIO_M  (EFUSE_VDD_SPI_AS_GPIO_V << EFUSE_VDD_SPI_AS_GPIO_S)
#define EFUSE_VDD_SPI_AS_GPIO_V  0x00000001U
#define EFUSE_VDD_SPI_AS_GPIO_S  26
/** EFUSE_RPT4_RESERVED0_2 : RO; bitpos: [28:27]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED0_2    0x00000003U
#define EFUSE_RPT4_RESERVED0_2_M  (EFUSE_RPT4_RESERVED0_2_V << EFUSE_RPT4_RESERVED0_2_S)
#define EFUSE_RPT4_RESERVED0_2_V  0x00000003U
#define EFUSE_RPT4_RESERVED0_2_S  27
/** EFUSE_RPT4_RESERVED0_1 : RO; bitpos: [29]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED0_1    (BIT(29))
#define EFUSE_RPT4_RESERVED0_1_M  (EFUSE_RPT4_RESERVED0_1_V << EFUSE_RPT4_RESERVED0_1_S)
#define EFUSE_RPT4_RESERVED0_1_V  0x00000001U
#define EFUSE_RPT4_RESERVED0_1_S  29
/** EFUSE_RPT4_RESERVED0_0 : RO; bitpos: [31:30]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED0_0    0x00000003U
#define EFUSE_RPT4_RESERVED0_0_M  (EFUSE_RPT4_RESERVED0_0_V << EFUSE_RPT4_RESERVED0_0_S)
#define EFUSE_RPT4_RESERVED0_0_V  0x00000003U
#define EFUSE_RPT4_RESERVED0_0_S  30

/** EFUSE_RD_REPEAT_DATA1_REG register
 *  BLOCK0 data register 2.
 */
#define EFUSE_RD_REPEAT_DATA1_REG (DR_REG_EFUSE_BASE + 0x34)
/** EFUSE_RPT4_RESERVED1_0 : RO; bitpos: [15:0]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED1_0    0x0000FFFFU
#define EFUSE_RPT4_RESERVED1_0_M  (EFUSE_RPT4_RESERVED1_0_V << EFUSE_RPT4_RESERVED1_0_S)
#define EFUSE_RPT4_RESERVED1_0_V  0x0000FFFFU
#define EFUSE_RPT4_RESERVED1_0_S  0
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
/** EFUSE_CRYPT_DPA_ENABLE : RO; bitpos: [18]; default: 0;
 *  Represents whether anti-dpa attack is enabled. 1:enabled. 0: disabled.
 */
#define EFUSE_CRYPT_DPA_ENABLE    (BIT(18))
#define EFUSE_CRYPT_DPA_ENABLE_M  (EFUSE_CRYPT_DPA_ENABLE_V << EFUSE_CRYPT_DPA_ENABLE_S)
#define EFUSE_CRYPT_DPA_ENABLE_V  0x00000001U
#define EFUSE_CRYPT_DPA_ENABLE_S  18
/** EFUSE_RPT4_RESERVED2_1 : RO; bitpos: [19]; default: 1;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED2_1    (BIT(19))
#define EFUSE_RPT4_RESERVED2_1_M  (EFUSE_RPT4_RESERVED2_1_V << EFUSE_RPT4_RESERVED2_1_S)
#define EFUSE_RPT4_RESERVED2_1_V  0x00000001U
#define EFUSE_RPT4_RESERVED2_1_S  19
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
/** EFUSE_RPT4_RESERVED2_0 : RO; bitpos: [27:22]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED2_0    0x0000003FU
#define EFUSE_RPT4_RESERVED2_0_M  (EFUSE_RPT4_RESERVED2_0_V << EFUSE_RPT4_RESERVED2_0_S)
#define EFUSE_RPT4_RESERVED2_0_V  0x0000003FU
#define EFUSE_RPT4_RESERVED2_0_S  22
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
/** EFUSE_RPT4_RESERVED3_5 : RO; bitpos: [3]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED3_5    (BIT(3))
#define EFUSE_RPT4_RESERVED3_5_M  (EFUSE_RPT4_RESERVED3_5_V << EFUSE_RPT4_RESERVED3_5_S)
#define EFUSE_RPT4_RESERVED3_5_V  0x00000001U
#define EFUSE_RPT4_RESERVED3_5_S  3
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
/** EFUSE_RPT4_RESERVED3_4 : RO; bitpos: [8]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED3_4    (BIT(8))
#define EFUSE_RPT4_RESERVED3_4_M  (EFUSE_RPT4_RESERVED3_4_V << EFUSE_RPT4_RESERVED3_4_S)
#define EFUSE_RPT4_RESERVED3_4_V  0x00000001U
#define EFUSE_RPT4_RESERVED3_4_S  8
/** EFUSE_RPT4_RESERVED3_3 : RO; bitpos: [9]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED3_3    (BIT(9))
#define EFUSE_RPT4_RESERVED3_3_M  (EFUSE_RPT4_RESERVED3_3_V << EFUSE_RPT4_RESERVED3_3_S)
#define EFUSE_RPT4_RESERVED3_3_V  0x00000001U
#define EFUSE_RPT4_RESERVED3_3_S  9
/** EFUSE_RPT4_RESERVED3_2 : RO; bitpos: [11:10]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED3_2    0x00000003U
#define EFUSE_RPT4_RESERVED3_2_M  (EFUSE_RPT4_RESERVED3_2_V << EFUSE_RPT4_RESERVED3_2_S)
#define EFUSE_RPT4_RESERVED3_2_V  0x00000003U
#define EFUSE_RPT4_RESERVED3_2_S  10
/** EFUSE_RPT4_RESERVED3_1 : RO; bitpos: [12]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED3_1    (BIT(12))
#define EFUSE_RPT4_RESERVED3_1_M  (EFUSE_RPT4_RESERVED3_1_V << EFUSE_RPT4_RESERVED3_1_S)
#define EFUSE_RPT4_RESERVED3_1_V  0x00000001U
#define EFUSE_RPT4_RESERVED3_1_S  12
/** EFUSE_FORCE_SEND_RESUME : RO; bitpos: [13]; default: 0;
 *  Represents whether ROM code is forced to send a resume command during SPI boot. 1:
 *  forced. 0:not forced.
 */
#define EFUSE_FORCE_SEND_RESUME    (BIT(13))
#define EFUSE_FORCE_SEND_RESUME_M  (EFUSE_FORCE_SEND_RESUME_V << EFUSE_FORCE_SEND_RESUME_S)
#define EFUSE_FORCE_SEND_RESUME_V  0x00000001U
#define EFUSE_FORCE_SEND_RESUME_S  13
/** EFUSE_SECURE_VERSION : RO; bitpos: [29:14]; default: 0;
 *  Represents the version used by ESP-IDF anti-rollback feature.
 */
#define EFUSE_SECURE_VERSION    0x0000FFFFU
#define EFUSE_SECURE_VERSION_M  (EFUSE_SECURE_VERSION_V << EFUSE_SECURE_VERSION_S)
#define EFUSE_SECURE_VERSION_V  0x0000FFFFU
#define EFUSE_SECURE_VERSION_S  14
/** EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE : RO; bitpos: [30]; default: 0;
 *  Represents whether FAST VERIFY ON WAKE is disabled or enabled when Secure Boot is
 *  enabled. 1: disabled. 0: enabled.
 */
#define EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE    (BIT(30))
#define EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_M  (EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_V << EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_S)
#define EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_V  0x00000001U
#define EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE_S  30
/** EFUSE_RPT4_RESERVED3_0 : RO; bitpos: [31]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED3_0    (BIT(31))
#define EFUSE_RPT4_RESERVED3_0_M  (EFUSE_RPT4_RESERVED3_0_V << EFUSE_RPT4_RESERVED3_0_S)
#define EFUSE_RPT4_RESERVED3_0_V  0x00000001U
#define EFUSE_RPT4_RESERVED3_0_S  31

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
/** EFUSE_RPT4_RESERVED4_0 : RO; bitpos: [31:24]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED4_0    0x000000FFU
#define EFUSE_RPT4_RESERVED4_0_M  (EFUSE_RPT4_RESERVED4_0_V << EFUSE_RPT4_RESERVED4_0_S)
#define EFUSE_RPT4_RESERVED4_0_V  0x000000FFU
#define EFUSE_RPT4_RESERVED4_0_S  24

/** EFUSE_RD_MAC_SPI_SYS_0_REG register
 *  BLOCK1 data register $n.
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
 *  BLOCK1 data register $n.
 */
#define EFUSE_RD_MAC_SPI_SYS_1_REG (DR_REG_EFUSE_BASE + 0x48)
/** EFUSE_MAC_1 : RO; bitpos: [15:0]; default: 0;
 *  Stores the high 16 bits of MAC address.
 */
#define EFUSE_MAC_1    0x0000FFFFU
#define EFUSE_MAC_1_M  (EFUSE_MAC_1_V << EFUSE_MAC_1_S)
#define EFUSE_MAC_1_V  0x0000FFFFU
#define EFUSE_MAC_1_S  0
/** EFUSE_MAC_EXT : RO; bitpos: [31:16]; default: 0;
 *  Stores the extended bits of MAC address.
 */
#define EFUSE_MAC_EXT    0x0000FFFFU
#define EFUSE_MAC_EXT_M  (EFUSE_MAC_EXT_V << EFUSE_MAC_EXT_S)
#define EFUSE_MAC_EXT_V  0x0000FFFFU
#define EFUSE_MAC_EXT_S  16

/** EFUSE_RD_MAC_SPI_SYS_2_REG register
 *  BLOCK1 data register $n.
 */
#define EFUSE_RD_MAC_SPI_SYS_2_REG (DR_REG_EFUSE_BASE + 0x4c)
/** EFUSE_ACTIVE_HP_DBIAS : RO; bitpos: [4:0]; default: 0;
 *  Stores the active hp dbias.
 */
#define EFUSE_ACTIVE_HP_DBIAS    0x0000001FU
#define EFUSE_ACTIVE_HP_DBIAS_M  (EFUSE_ACTIVE_HP_DBIAS_V << EFUSE_ACTIVE_HP_DBIAS_S)
#define EFUSE_ACTIVE_HP_DBIAS_V  0x0000001FU
#define EFUSE_ACTIVE_HP_DBIAS_S  0
/** EFUSE_ACTIVE_LP_DBIAS : RO; bitpos: [9:5]; default: 0;
 *  Stores the active lp dbias.
 */
#define EFUSE_ACTIVE_LP_DBIAS    0x0000001FU
#define EFUSE_ACTIVE_LP_DBIAS_M  (EFUSE_ACTIVE_LP_DBIAS_V << EFUSE_ACTIVE_LP_DBIAS_S)
#define EFUSE_ACTIVE_LP_DBIAS_V  0x0000001FU
#define EFUSE_ACTIVE_LP_DBIAS_S  5
/** EFUSE_LSLP_HP_DBG : RO; bitpos: [11:10]; default: 0;
 *  Stores the lslp hp dbg.
 */
#define EFUSE_LSLP_HP_DBG    0x00000003U
#define EFUSE_LSLP_HP_DBG_M  (EFUSE_LSLP_HP_DBG_V << EFUSE_LSLP_HP_DBG_S)
#define EFUSE_LSLP_HP_DBG_V  0x00000003U
#define EFUSE_LSLP_HP_DBG_S  10
/** EFUSE_LSLP_HP_DBIAS : RO; bitpos: [15:12]; default: 0;
 *  Stores the lslp hp dbias.
 */
#define EFUSE_LSLP_HP_DBIAS    0x0000000FU
#define EFUSE_LSLP_HP_DBIAS_M  (EFUSE_LSLP_HP_DBIAS_V << EFUSE_LSLP_HP_DBIAS_S)
#define EFUSE_LSLP_HP_DBIAS_V  0x0000000FU
#define EFUSE_LSLP_HP_DBIAS_S  12
/** EFUSE_DSLP_LP_DBG : RO; bitpos: [18:16]; default: 0;
 *  Stores the dslp lp dbg.
 */
#define EFUSE_DSLP_LP_DBG    0x00000007U
#define EFUSE_DSLP_LP_DBG_M  (EFUSE_DSLP_LP_DBG_V << EFUSE_DSLP_LP_DBG_S)
#define EFUSE_DSLP_LP_DBG_V  0x00000007U
#define EFUSE_DSLP_LP_DBG_S  16
/** EFUSE_DSLP_LP_DBIAS : RO; bitpos: [22:19]; default: 0;
 *  Stores the dslp lp dbias.
 */
#define EFUSE_DSLP_LP_DBIAS    0x0000000FU
#define EFUSE_DSLP_LP_DBIAS_M  (EFUSE_DSLP_LP_DBIAS_V << EFUSE_DSLP_LP_DBIAS_S)
#define EFUSE_DSLP_LP_DBIAS_V  0x0000000FU
#define EFUSE_DSLP_LP_DBIAS_S  19
/** EFUSE_DBIAS_VOL_GAP : RO; bitpos: [27:23]; default: 0;
 *  Stores the hp and lp dbias vol gap.
 */
#define EFUSE_DBIAS_VOL_GAP    0x0000001FU
#define EFUSE_DBIAS_VOL_GAP_M  (EFUSE_DBIAS_VOL_GAP_V << EFUSE_DBIAS_VOL_GAP_S)
#define EFUSE_DBIAS_VOL_GAP_V  0x0000001FU
#define EFUSE_DBIAS_VOL_GAP_S  23
/** EFUSE_SPI_PAD_CONF_1 : RO; bitpos: [31:28]; default: 0;
 *  Stores the first part of SPI_PAD_CONF.
 */
#define EFUSE_SPI_PAD_CONF_1    0x0000000FU
#define EFUSE_SPI_PAD_CONF_1_M  (EFUSE_SPI_PAD_CONF_1_V << EFUSE_SPI_PAD_CONF_1_S)
#define EFUSE_SPI_PAD_CONF_1_V  0x0000000FU
#define EFUSE_SPI_PAD_CONF_1_S  28

/** EFUSE_RD_MAC_SPI_SYS_3_REG register
 *  BLOCK1 data register $n.
 */
#define EFUSE_RD_MAC_SPI_SYS_3_REG (DR_REG_EFUSE_BASE + 0x50)
/** EFUSE_SPI_PAD_CONF_2 : RO; bitpos: [17:0]; default: 0;
 *  Stores the second part of SPI_PAD_CONF.
 */
#define EFUSE_SPI_PAD_CONF_2    0x0003FFFFU
#define EFUSE_SPI_PAD_CONF_2_M  (EFUSE_SPI_PAD_CONF_2_V << EFUSE_SPI_PAD_CONF_2_S)
#define EFUSE_SPI_PAD_CONF_2_V  0x0003FFFFU
#define EFUSE_SPI_PAD_CONF_2_S  0
/** EFUSE_WAFER_VERSION_MINOR : R; bitpos: [21:18]; default: 0; */
#define EFUSE_WAFER_VERSION_MINOR    0x0000000FU
#define EFUSE_WAFER_VERSION_MINOR_M  (EFUSE_WAFER_VERSION_MINOR_V << EFUSE_WAFER_VERSION_MINOR_S)
#define EFUSE_WAFER_VERSION_MINOR_V  0x0000000FU
#define EFUSE_WAFER_VERSION_MINOR_S  18
/** EFUSE_WAFER_VERSION_MAJOR : R; bitpos: [23:22]; default: 0; */
#define EFUSE_WAFER_VERSION_MAJOR    0x00000003U
#define EFUSE_WAFER_VERSION_MAJOR_M  (EFUSE_WAFER_VERSION_MAJOR_V << EFUSE_WAFER_VERSION_MAJOR_S)
#define EFUSE_WAFER_VERSION_MAJOR_V  0x00000003U
#define EFUSE_WAFER_VERSION_MAJOR_S  22
/** EFUSE_PKG_VERSION : R; bitpos: [26:24]; default: 0;
 *  Package version
 */
#define EFUSE_PKG_VERSION    0x00000007U
#define EFUSE_PKG_VERSION_M  (EFUSE_PKG_VERSION_V << EFUSE_PKG_VERSION_S)
#define EFUSE_PKG_VERSION_V  0x00000007U
#define EFUSE_PKG_VERSION_S  24
/** EFUSE_BLK_VERSION_MINOR : R; bitpos: [29:27]; default: 0;
 *  BLK_VERSION_MINOR of BLOCK2
 */
#define EFUSE_BLK_VERSION_MINOR    0x00000007U
#define EFUSE_BLK_VERSION_MINOR_M  (EFUSE_BLK_VERSION_MINOR_V << EFUSE_BLK_VERSION_MINOR_S)
#define EFUSE_BLK_VERSION_MINOR_V  0x00000007U
#define EFUSE_BLK_VERSION_MINOR_S  27
/** EFUSE_BLK_VERSION_MAJOR : R; bitpos: [31:30]; default: 0;
 *  BLK_VERSION_MAJOR of BLOCK2
 */
#define EFUSE_BLK_VERSION_MAJOR    0x00000003U
#define EFUSE_BLK_VERSION_MAJOR_M  (EFUSE_BLK_VERSION_MAJOR_V << EFUSE_BLK_VERSION_MAJOR_S)
#define EFUSE_BLK_VERSION_MAJOR_V  0x00000003U
#define EFUSE_BLK_VERSION_MAJOR_S  30

/** EFUSE_RD_MAC_SPI_SYS_4_REG register
 *  BLOCK1 data register $n.
 */
#define EFUSE_RD_MAC_SPI_SYS_4_REG (DR_REG_EFUSE_BASE + 0x54)
/** EFUSE_FLASH_CAP : R; bitpos: [2:0]; default: 0; */
#define EFUSE_FLASH_CAP    0x00000007U
#define EFUSE_FLASH_CAP_M  (EFUSE_FLASH_CAP_V << EFUSE_FLASH_CAP_S)
#define EFUSE_FLASH_CAP_V  0x00000007U
#define EFUSE_FLASH_CAP_S  0
/** EFUSE_FLASH_TEMP : R; bitpos: [4:3]; default: 0; */
#define EFUSE_FLASH_TEMP    0x00000003U
#define EFUSE_FLASH_TEMP_M  (EFUSE_FLASH_TEMP_V << EFUSE_FLASH_TEMP_S)
#define EFUSE_FLASH_TEMP_V  0x00000003U
#define EFUSE_FLASH_TEMP_S  3
/** EFUSE_FLASH_VENDOR : R; bitpos: [7:5]; default: 0; */
#define EFUSE_FLASH_VENDOR    0x00000007U
#define EFUSE_FLASH_VENDOR_M  (EFUSE_FLASH_VENDOR_V << EFUSE_FLASH_VENDOR_S)
#define EFUSE_FLASH_VENDOR_V  0x00000007U
#define EFUSE_FLASH_VENDOR_S  5
/** EFUSE_RESERVED_1_136 : R; bitpos: [31:8]; default: 0;
 *  reserved
 */
#define EFUSE_RESERVED_1_136    0x00FFFFFFU
#define EFUSE_RESERVED_1_136_M  (EFUSE_RESERVED_1_136_V << EFUSE_RESERVED_1_136_S)
#define EFUSE_RESERVED_1_136_V  0x00FFFFFFU
#define EFUSE_RESERVED_1_136_S  8

/** EFUSE_RD_MAC_SPI_SYS_5_REG register
 *  BLOCK1 data register $n.
 */
#define EFUSE_RD_MAC_SPI_SYS_5_REG (DR_REG_EFUSE_BASE + 0x58)
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
/** EFUSE_TEMP_CALIB : R; bitpos: [8:0]; default: 0;
 *  Temperature calibration data
 */
#define EFUSE_TEMP_CALIB    0x000001FFU
#define EFUSE_TEMP_CALIB_M  (EFUSE_TEMP_CALIB_V << EFUSE_TEMP_CALIB_S)
#define EFUSE_TEMP_CALIB_V  0x000001FFU
#define EFUSE_TEMP_CALIB_S  0
/** EFUSE_OCODE : R; bitpos: [16:9]; default: 0;
 *  ADC OCode
 */
#define EFUSE_OCODE    0x000000FFU
#define EFUSE_OCODE_M  (EFUSE_OCODE_V << EFUSE_OCODE_S)
#define EFUSE_OCODE_V  0x000000FFU
#define EFUSE_OCODE_S  9
/** EFUSE_ADC1_INIT_CODE_ATTEN0 : R; bitpos: [26:17]; default: 0;
 *  ADC1 init code at atten0
 */
#define EFUSE_ADC1_INIT_CODE_ATTEN0    0x000003FFU
#define EFUSE_ADC1_INIT_CODE_ATTEN0_M  (EFUSE_ADC1_INIT_CODE_ATTEN0_V << EFUSE_ADC1_INIT_CODE_ATTEN0_S)
#define EFUSE_ADC1_INIT_CODE_ATTEN0_V  0x000003FFU
#define EFUSE_ADC1_INIT_CODE_ATTEN0_S  17
/** EFUSE_ADC1_INIT_CODE_ATTEN1 : R; bitpos: [31:27]; default: 0;
 *  ADC1 init code at atten1
 */
#define EFUSE_ADC1_INIT_CODE_ATTEN1    0x0000001FU
#define EFUSE_ADC1_INIT_CODE_ATTEN1_M  (EFUSE_ADC1_INIT_CODE_ATTEN1_V << EFUSE_ADC1_INIT_CODE_ATTEN1_S)
#define EFUSE_ADC1_INIT_CODE_ATTEN1_V  0x0000001FU
#define EFUSE_ADC1_INIT_CODE_ATTEN1_S  27

/** EFUSE_RD_SYS_PART1_DATA5_REG register
 *  Register $n of BLOCK2 (system).
 */
#define EFUSE_RD_SYS_PART1_DATA5_REG (DR_REG_EFUSE_BASE + 0x70)
/** EFUSE_ADC1_INIT_CODE_ATTEN1_1 : R; bitpos: [4:0]; default: 0;
 *  ADC1 init code at atten1
 */
#define EFUSE_ADC1_INIT_CODE_ATTEN1_1    0x0000001FU
#define EFUSE_ADC1_INIT_CODE_ATTEN1_1_M  (EFUSE_ADC1_INIT_CODE_ATTEN1_1_V << EFUSE_ADC1_INIT_CODE_ATTEN1_1_S)
#define EFUSE_ADC1_INIT_CODE_ATTEN1_1_V  0x0000001FU
#define EFUSE_ADC1_INIT_CODE_ATTEN1_1_S  0
/** EFUSE_ADC1_INIT_CODE_ATTEN2 : R; bitpos: [14:5]; default: 0;
 *  ADC1 init code at atten2
 */
#define EFUSE_ADC1_INIT_CODE_ATTEN2    0x000003FFU
#define EFUSE_ADC1_INIT_CODE_ATTEN2_M  (EFUSE_ADC1_INIT_CODE_ATTEN2_V << EFUSE_ADC1_INIT_CODE_ATTEN2_S)
#define EFUSE_ADC1_INIT_CODE_ATTEN2_V  0x000003FFU
#define EFUSE_ADC1_INIT_CODE_ATTEN2_S  5
/** EFUSE_ADC1_INIT_CODE_ATTEN3 : R; bitpos: [24:15]; default: 0;
 *  ADC1 init code at atten3
 */
#define EFUSE_ADC1_INIT_CODE_ATTEN3    0x000003FFU
#define EFUSE_ADC1_INIT_CODE_ATTEN3_M  (EFUSE_ADC1_INIT_CODE_ATTEN3_V << EFUSE_ADC1_INIT_CODE_ATTEN3_S)
#define EFUSE_ADC1_INIT_CODE_ATTEN3_V  0x000003FFU
#define EFUSE_ADC1_INIT_CODE_ATTEN3_S  15
/** EFUSE_ADC1_CAL_VOL_ATTEN0 : R; bitpos: [31:25]; default: 0;
 *  ADC1 calibration voltage at atten0
 */
#define EFUSE_ADC1_CAL_VOL_ATTEN0    0x0000007FU
#define EFUSE_ADC1_CAL_VOL_ATTEN0_M  (EFUSE_ADC1_CAL_VOL_ATTEN0_V << EFUSE_ADC1_CAL_VOL_ATTEN0_S)
#define EFUSE_ADC1_CAL_VOL_ATTEN0_V  0x0000007FU
#define EFUSE_ADC1_CAL_VOL_ATTEN0_S  25

/** EFUSE_RD_SYS_PART1_DATA6_REG register
 *  Register $n of BLOCK2 (system).
 */
#define EFUSE_RD_SYS_PART1_DATA6_REG (DR_REG_EFUSE_BASE + 0x74)
/** EFUSE_ADC1_CAL_VOL_ATTEN0_1 : R; bitpos: [2:0]; default: 0;
 *  ADC1 calibration voltage at atten0
 */
#define EFUSE_ADC1_CAL_VOL_ATTEN0_1    0x00000007U
#define EFUSE_ADC1_CAL_VOL_ATTEN0_1_M  (EFUSE_ADC1_CAL_VOL_ATTEN0_1_V << EFUSE_ADC1_CAL_VOL_ATTEN0_1_S)
#define EFUSE_ADC1_CAL_VOL_ATTEN0_1_V  0x00000007U
#define EFUSE_ADC1_CAL_VOL_ATTEN0_1_S  0
/** EFUSE_ADC1_CAL_VOL_ATTEN1 : R; bitpos: [12:3]; default: 0;
 *  ADC1 calibration voltage at atten1
 */
#define EFUSE_ADC1_CAL_VOL_ATTEN1    0x000003FFU
#define EFUSE_ADC1_CAL_VOL_ATTEN1_M  (EFUSE_ADC1_CAL_VOL_ATTEN1_V << EFUSE_ADC1_CAL_VOL_ATTEN1_S)
#define EFUSE_ADC1_CAL_VOL_ATTEN1_V  0x000003FFU
#define EFUSE_ADC1_CAL_VOL_ATTEN1_S  3
/** EFUSE_ADC1_CAL_VOL_ATTEN2 : R; bitpos: [22:13]; default: 0;
 *  ADC1 calibration voltage at atten2
 */
#define EFUSE_ADC1_CAL_VOL_ATTEN2    0x000003FFU
#define EFUSE_ADC1_CAL_VOL_ATTEN2_M  (EFUSE_ADC1_CAL_VOL_ATTEN2_V << EFUSE_ADC1_CAL_VOL_ATTEN2_S)
#define EFUSE_ADC1_CAL_VOL_ATTEN2_V  0x000003FFU
#define EFUSE_ADC1_CAL_VOL_ATTEN2_S  13
/** EFUSE_ADC1_CAL_VOL_ATTEN3 : R; bitpos: [31:23]; default: 0;
 *  ADC1 calibration voltage at atten3
 */
#define EFUSE_ADC1_CAL_VOL_ATTEN3    0x000001FFU
#define EFUSE_ADC1_CAL_VOL_ATTEN3_M  (EFUSE_ADC1_CAL_VOL_ATTEN3_V << EFUSE_ADC1_CAL_VOL_ATTEN3_S)
#define EFUSE_ADC1_CAL_VOL_ATTEN3_V  0x000001FFU
#define EFUSE_ADC1_CAL_VOL_ATTEN3_S  23

/** EFUSE_RD_SYS_PART1_DATA7_REG register
 *  Register $n of BLOCK2 (system).
 */
#define EFUSE_RD_SYS_PART1_DATA7_REG (DR_REG_EFUSE_BASE + 0x78)
/** EFUSE_ADC1_CAL_VOL_ATTEN3_1 : R; bitpos: [0]; default: 0;
 *  ADC1 calibration voltage at atten3
 */
#define EFUSE_ADC1_CAL_VOL_ATTEN3_1    (BIT(0))
#define EFUSE_ADC1_CAL_VOL_ATTEN3_1_M  (EFUSE_ADC1_CAL_VOL_ATTEN3_1_V << EFUSE_ADC1_CAL_VOL_ATTEN3_1_S)
#define EFUSE_ADC1_CAL_VOL_ATTEN3_1_V  0x00000001U
#define EFUSE_ADC1_CAL_VOL_ATTEN3_1_S  0
/** EFUSE_ADC1_INIT_CODE_ATTEN0_CH0 : R; bitpos: [4:1]; default: 0;
 *  ADC1 init code at atten0 ch0
 */
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH0    0x0000000FU
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH0_M  (EFUSE_ADC1_INIT_CODE_ATTEN0_CH0_V << EFUSE_ADC1_INIT_CODE_ATTEN0_CH0_S)
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH0_V  0x0000000FU
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH0_S  1
/** EFUSE_ADC1_INIT_CODE_ATTEN0_CH1 : R; bitpos: [8:5]; default: 0;
 *  ADC1 init code at atten0 ch1
 */
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH1    0x0000000FU
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH1_M  (EFUSE_ADC1_INIT_CODE_ATTEN0_CH1_V << EFUSE_ADC1_INIT_CODE_ATTEN0_CH1_S)
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH1_V  0x0000000FU
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH1_S  5
/** EFUSE_ADC1_INIT_CODE_ATTEN0_CH2 : R; bitpos: [12:9]; default: 0;
 *  ADC1 init code at atten0 ch2
 */
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH2    0x0000000FU
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH2_M  (EFUSE_ADC1_INIT_CODE_ATTEN0_CH2_V << EFUSE_ADC1_INIT_CODE_ATTEN0_CH2_S)
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH2_V  0x0000000FU
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH2_S  9
/** EFUSE_ADC1_INIT_CODE_ATTEN0_CH3 : R; bitpos: [16:13]; default: 0;
 *  ADC1 init code at atten0 ch3
 */
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH3    0x0000000FU
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH3_M  (EFUSE_ADC1_INIT_CODE_ATTEN0_CH3_V << EFUSE_ADC1_INIT_CODE_ATTEN0_CH3_S)
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH3_V  0x0000000FU
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH3_S  13
/** EFUSE_ADC1_INIT_CODE_ATTEN0_CH4 : R; bitpos: [20:17]; default: 0;
 *  ADC1 init code at atten0 ch4
 */
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH4    0x0000000FU
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH4_M  (EFUSE_ADC1_INIT_CODE_ATTEN0_CH4_V << EFUSE_ADC1_INIT_CODE_ATTEN0_CH4_S)
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH4_V  0x0000000FU
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH4_S  17
/** EFUSE_ADC1_INIT_CODE_ATTEN0_CH5 : R; bitpos: [24:21]; default: 0;
 *  ADC1 init code at atten0 ch5
 */
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH5    0x0000000FU
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH5_M  (EFUSE_ADC1_INIT_CODE_ATTEN0_CH5_V << EFUSE_ADC1_INIT_CODE_ATTEN0_CH5_S)
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH5_V  0x0000000FU
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH5_S  21
/** EFUSE_ADC1_INIT_CODE_ATTEN0_CH6 : R; bitpos: [28:25]; default: 0;
 *  ADC1 init code at atten0 ch6
 */
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH6    0x0000000FU
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH6_M  (EFUSE_ADC1_INIT_CODE_ATTEN0_CH6_V << EFUSE_ADC1_INIT_CODE_ATTEN0_CH6_S)
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH6_V  0x0000000FU
#define EFUSE_ADC1_INIT_CODE_ATTEN0_CH6_S  25
/** EFUSE_RESERVED_2_253 : R; bitpos: [31:29]; default: 0;
 *  reserved
 */
#define EFUSE_RESERVED_2_253    0x00000007U
#define EFUSE_RESERVED_2_253_M  (EFUSE_RESERVED_2_253_V << EFUSE_RESERVED_2_253_S)
#define EFUSE_RESERVED_2_253_V  0x00000007U
#define EFUSE_RESERVED_2_253_S  29

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
/** EFUSE_SWAP_UART_SDIO_EN_ERR : RO; bitpos: [7]; default: 0;
 *  Indicates a programming error of SWAP_UART_SDIO_EN.
 */
#define EFUSE_SWAP_UART_SDIO_EN_ERR    (BIT(7))
#define EFUSE_SWAP_UART_SDIO_EN_ERR_M  (EFUSE_SWAP_UART_SDIO_EN_ERR_V << EFUSE_SWAP_UART_SDIO_EN_ERR_S)
#define EFUSE_SWAP_UART_SDIO_EN_ERR_V  0x00000001U
#define EFUSE_SWAP_UART_SDIO_EN_ERR_S  7
/** EFUSE_DIS_ICACHE_ERR : RO; bitpos: [8]; default: 0;
 *  Indicates a programming error of DIS_ICACHE.
 */
#define EFUSE_DIS_ICACHE_ERR    (BIT(8))
#define EFUSE_DIS_ICACHE_ERR_M  (EFUSE_DIS_ICACHE_ERR_V << EFUSE_DIS_ICACHE_ERR_S)
#define EFUSE_DIS_ICACHE_ERR_V  0x00000001U
#define EFUSE_DIS_ICACHE_ERR_S  8
/** EFUSE_DIS_USB_JTAG_ERR : RO; bitpos: [9]; default: 0;
 *  Indicates a programming error of DIS_USB_JTAG.
 */
#define EFUSE_DIS_USB_JTAG_ERR    (BIT(9))
#define EFUSE_DIS_USB_JTAG_ERR_M  (EFUSE_DIS_USB_JTAG_ERR_V << EFUSE_DIS_USB_JTAG_ERR_S)
#define EFUSE_DIS_USB_JTAG_ERR_V  0x00000001U
#define EFUSE_DIS_USB_JTAG_ERR_S  9
/** EFUSE_DIS_DOWNLOAD_ICACHE_ERR : RO; bitpos: [10]; default: 0;
 *  Indicates a programming error of DIS_DOWNLOAD_ICACHE.
 */
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR    (BIT(10))
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR_M  (EFUSE_DIS_DOWNLOAD_ICACHE_ERR_V << EFUSE_DIS_DOWNLOAD_ICACHE_ERR_S)
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR_V  0x00000001U
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR_S  10
/** EFUSE_DIS_USB_SERIAL_JTAG_ERR : RO; bitpos: [11]; default: 0;
 *  Indicates a programming error of DIS_USB_DEVICE.
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
 *  Indicates a programming error of DIS_CAN.
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
/** EFUSE_USB_DREFH_ERR : RO; bitpos: [22:21]; default: 0;
 *  Indicates a programming error of USB_DREFH.
 */
#define EFUSE_USB_DREFH_ERR    0x00000003U
#define EFUSE_USB_DREFH_ERR_M  (EFUSE_USB_DREFH_ERR_V << EFUSE_USB_DREFH_ERR_S)
#define EFUSE_USB_DREFH_ERR_V  0x00000003U
#define EFUSE_USB_DREFH_ERR_S  21
/** EFUSE_USB_DREFL_ERR : RO; bitpos: [24:23]; default: 0;
 *  Indicates a programming error of USB_DREFL.
 */
#define EFUSE_USB_DREFL_ERR    0x00000003U
#define EFUSE_USB_DREFL_ERR_M  (EFUSE_USB_DREFL_ERR_V << EFUSE_USB_DREFL_ERR_S)
#define EFUSE_USB_DREFL_ERR_V  0x00000003U
#define EFUSE_USB_DREFL_ERR_S  23
/** EFUSE_USB_EXCHG_PINS_ERR : RO; bitpos: [25]; default: 0;
 *  Indicates a programming error of USB_EXCHG_PINS.
 */
#define EFUSE_USB_EXCHG_PINS_ERR    (BIT(25))
#define EFUSE_USB_EXCHG_PINS_ERR_M  (EFUSE_USB_EXCHG_PINS_ERR_V << EFUSE_USB_EXCHG_PINS_ERR_S)
#define EFUSE_USB_EXCHG_PINS_ERR_V  0x00000001U
#define EFUSE_USB_EXCHG_PINS_ERR_S  25
/** EFUSE_VDD_SPI_AS_GPIO_ERR : RO; bitpos: [26]; default: 0;
 *  Indicates a programming error of VDD_SPI_AS_GPIO.
 */
#define EFUSE_VDD_SPI_AS_GPIO_ERR    (BIT(26))
#define EFUSE_VDD_SPI_AS_GPIO_ERR_M  (EFUSE_VDD_SPI_AS_GPIO_ERR_V << EFUSE_VDD_SPI_AS_GPIO_ERR_S)
#define EFUSE_VDD_SPI_AS_GPIO_ERR_V  0x00000001U
#define EFUSE_VDD_SPI_AS_GPIO_ERR_S  26
/** EFUSE_RPT4_RESERVED0_ERR_2 : RO; bitpos: [28:27]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED0_ERR_2    0x00000003U
#define EFUSE_RPT4_RESERVED0_ERR_2_M  (EFUSE_RPT4_RESERVED0_ERR_2_V << EFUSE_RPT4_RESERVED0_ERR_2_S)
#define EFUSE_RPT4_RESERVED0_ERR_2_V  0x00000003U
#define EFUSE_RPT4_RESERVED0_ERR_2_S  27
/** EFUSE_RPT4_RESERVED0_ERR_1 : RO; bitpos: [29]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED0_ERR_1    (BIT(29))
#define EFUSE_RPT4_RESERVED0_ERR_1_M  (EFUSE_RPT4_RESERVED0_ERR_1_V << EFUSE_RPT4_RESERVED0_ERR_1_S)
#define EFUSE_RPT4_RESERVED0_ERR_1_V  0x00000001U
#define EFUSE_RPT4_RESERVED0_ERR_1_S  29
/** EFUSE_RPT4_RESERVED0_ERR_0 : RO; bitpos: [31:30]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED0_ERR_0    0x00000003U
#define EFUSE_RPT4_RESERVED0_ERR_0_M  (EFUSE_RPT4_RESERVED0_ERR_0_V << EFUSE_RPT4_RESERVED0_ERR_0_S)
#define EFUSE_RPT4_RESERVED0_ERR_0_V  0x00000003U
#define EFUSE_RPT4_RESERVED0_ERR_0_S  30

/** EFUSE_RD_REPEAT_ERR1_REG register
 *  Programming error record register 1 of BLOCK0.
 */
#define EFUSE_RD_REPEAT_ERR1_REG (DR_REG_EFUSE_BASE + 0x180)
/** EFUSE_RPT4_RESERVED1_ERR_0 : RO; bitpos: [15:0]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED1_ERR_0    0x0000FFFFU
#define EFUSE_RPT4_RESERVED1_ERR_0_M  (EFUSE_RPT4_RESERVED1_ERR_0_V << EFUSE_RPT4_RESERVED1_ERR_0_S)
#define EFUSE_RPT4_RESERVED1_ERR_0_V  0x0000FFFFU
#define EFUSE_RPT4_RESERVED1_ERR_0_S  0
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
/** EFUSE_RPT4_RESERVED2_ERR_1 : RO; bitpos: [18]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED2_ERR_1    (BIT(18))
#define EFUSE_RPT4_RESERVED2_ERR_1_M  (EFUSE_RPT4_RESERVED2_ERR_1_V << EFUSE_RPT4_RESERVED2_ERR_1_S)
#define EFUSE_RPT4_RESERVED2_ERR_1_V  0x00000001U
#define EFUSE_RPT4_RESERVED2_ERR_1_S  18
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
/** EFUSE_RPT4_RESERVED2_ERR_0 : RO; bitpos: [27:22]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED2_ERR_0    0x0000003FU
#define EFUSE_RPT4_RESERVED2_ERR_0_M  (EFUSE_RPT4_RESERVED2_ERR_0_V << EFUSE_RPT4_RESERVED2_ERR_0_S)
#define EFUSE_RPT4_RESERVED2_ERR_0_V  0x0000003FU
#define EFUSE_RPT4_RESERVED2_ERR_0_S  22
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
/** EFUSE_USB_PRINT_ERR : RO; bitpos: [2]; default: 0;
 *  Indicates a programming error of UART_PRINT_CHANNEL.
 */
#define EFUSE_USB_PRINT_ERR    (BIT(2))
#define EFUSE_USB_PRINT_ERR_M  (EFUSE_USB_PRINT_ERR_V << EFUSE_USB_PRINT_ERR_S)
#define EFUSE_USB_PRINT_ERR_V  0x00000001U
#define EFUSE_USB_PRINT_ERR_S  2
/** EFUSE_RPT4_RESERVED3_ERR_5 : RO; bitpos: [3]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED3_ERR_5    (BIT(3))
#define EFUSE_RPT4_RESERVED3_ERR_5_M  (EFUSE_RPT4_RESERVED3_ERR_5_V << EFUSE_RPT4_RESERVED3_ERR_5_S)
#define EFUSE_RPT4_RESERVED3_ERR_5_V  0x00000001U
#define EFUSE_RPT4_RESERVED3_ERR_5_S  3
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
/** EFUSE_RPT4_RESERVED3_ERR_4 : RO; bitpos: [8]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED3_ERR_4    (BIT(8))
#define EFUSE_RPT4_RESERVED3_ERR_4_M  (EFUSE_RPT4_RESERVED3_ERR_4_V << EFUSE_RPT4_RESERVED3_ERR_4_S)
#define EFUSE_RPT4_RESERVED3_ERR_4_V  0x00000001U
#define EFUSE_RPT4_RESERVED3_ERR_4_S  8
/** EFUSE_RPT4_RESERVED3_ERR_3 : RO; bitpos: [9]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED3_ERR_3    (BIT(9))
#define EFUSE_RPT4_RESERVED3_ERR_3_M  (EFUSE_RPT4_RESERVED3_ERR_3_V << EFUSE_RPT4_RESERVED3_ERR_3_S)
#define EFUSE_RPT4_RESERVED3_ERR_3_V  0x00000001U
#define EFUSE_RPT4_RESERVED3_ERR_3_S  9
/** EFUSE_RPT4_RESERVED3_ERR_2 : RO; bitpos: [11:10]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED3_ERR_2    0x00000003U
#define EFUSE_RPT4_RESERVED3_ERR_2_M  (EFUSE_RPT4_RESERVED3_ERR_2_V << EFUSE_RPT4_RESERVED3_ERR_2_S)
#define EFUSE_RPT4_RESERVED3_ERR_2_V  0x00000003U
#define EFUSE_RPT4_RESERVED3_ERR_2_S  10
/** EFUSE_RPT4_RESERVED3_ERR_1 : RO; bitpos: [12]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED3_ERR_1    (BIT(12))
#define EFUSE_RPT4_RESERVED3_ERR_1_M  (EFUSE_RPT4_RESERVED3_ERR_1_V << EFUSE_RPT4_RESERVED3_ERR_1_S)
#define EFUSE_RPT4_RESERVED3_ERR_1_V  0x00000001U
#define EFUSE_RPT4_RESERVED3_ERR_1_S  12
/** EFUSE_FORCE_SEND_RESUME_ERR : RO; bitpos: [13]; default: 0;
 *  Indicates a programming error of FORCE_SEND_RESUME.
 */
#define EFUSE_FORCE_SEND_RESUME_ERR    (BIT(13))
#define EFUSE_FORCE_SEND_RESUME_ERR_M  (EFUSE_FORCE_SEND_RESUME_ERR_V << EFUSE_FORCE_SEND_RESUME_ERR_S)
#define EFUSE_FORCE_SEND_RESUME_ERR_V  0x00000001U
#define EFUSE_FORCE_SEND_RESUME_ERR_S  13
/** EFUSE_SECURE_VERSION_ERR : RO; bitpos: [29:14]; default: 0;
 *  Indicates a programming error of SECURE_VERSION.
 */
#define EFUSE_SECURE_VERSION_ERR    0x0000FFFFU
#define EFUSE_SECURE_VERSION_ERR_M  (EFUSE_SECURE_VERSION_ERR_V << EFUSE_SECURE_VERSION_ERR_S)
#define EFUSE_SECURE_VERSION_ERR_V  0x0000FFFFU
#define EFUSE_SECURE_VERSION_ERR_S  14
/** EFUSE_RPT4_RESERVED3_ERR_0 : RO; bitpos: [31:30]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED3_ERR_0    0x00000003U
#define EFUSE_RPT4_RESERVED3_ERR_0_M  (EFUSE_RPT4_RESERVED3_ERR_0_V << EFUSE_RPT4_RESERVED3_ERR_0_S)
#define EFUSE_RPT4_RESERVED3_ERR_0_V  0x00000003U
#define EFUSE_RPT4_RESERVED3_ERR_0_S  30

/** EFUSE_RD_REPEAT_ERR4_REG register
 *  Programming error record register 4 of BLOCK0.
 */
#define EFUSE_RD_REPEAT_ERR4_REG (DR_REG_EFUSE_BASE + 0x190)
/** EFUSE_RPT4_RESERVED4_ERR_1 : RO; bitpos: [23:0]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED4_ERR_1    0x00FFFFFFU
#define EFUSE_RPT4_RESERVED4_ERR_1_M  (EFUSE_RPT4_RESERVED4_ERR_1_V << EFUSE_RPT4_RESERVED4_ERR_1_S)
#define EFUSE_RPT4_RESERVED4_ERR_1_V  0x00FFFFFFU
#define EFUSE_RPT4_RESERVED4_ERR_1_S  0
/** EFUSE_RPT4_RESERVED4_ERR_0 : RO; bitpos: [31:24]; default: 0;
 *  Reserved.
 */
#define EFUSE_RPT4_RESERVED4_ERR_0    0x000000FFU
#define EFUSE_RPT4_RESERVED4_ERR_0_M  (EFUSE_RPT4_RESERVED4_ERR_0_V << EFUSE_RPT4_RESERVED4_ERR_0_S)
#define EFUSE_RPT4_RESERVED4_ERR_0_V  0x000000FFU
#define EFUSE_RPT4_RESERVED4_ERR_0_S  24

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
/** EFUSE_MAC_SPI_8M_FAIL : RO; bitpos: [3]; default: 0;
 *  0: Means no failure and that the data of MAC_SPI_8M is reliable 1: Means that
 *  programming user data failed and the number of error bytes is over 6.
 */
#define EFUSE_MAC_SPI_8M_FAIL    (BIT(3))
#define EFUSE_MAC_SPI_8M_FAIL_M  (EFUSE_MAC_SPI_8M_FAIL_V << EFUSE_MAC_SPI_8M_FAIL_S)
#define EFUSE_MAC_SPI_8M_FAIL_V  0x00000001U
#define EFUSE_MAC_SPI_8M_FAIL_S  3
/** EFUSE_SYS_PART1_NUM : RO; bitpos: [6:4]; default: 0;
 *  The value of this signal means the number of error bytes.
 */
#define EFUSE_SYS_PART1_NUM    0x00000007U
#define EFUSE_SYS_PART1_NUM_M  (EFUSE_SYS_PART1_NUM_V << EFUSE_SYS_PART1_NUM_S)
#define EFUSE_SYS_PART1_NUM_V  0x00000007U
#define EFUSE_SYS_PART1_NUM_S  4
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
/** EFUSE_MEM_CLK_FORCE_ON : R/W; bitpos: [1]; default: 1;
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
 *  eFuse operation mode configuraiton register
 */
#define EFUSE_CONF_REG (DR_REG_EFUSE_BASE + 0x1cc)
/** EFUSE_OP_CODE : R/W; bitpos: [15:0]; default: 0;
 *  0x5A5A:  programming operation command 0x5AA5: read operation command.
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
/** EFUSE_BLK0_VALID_BIT_CNT : RO; bitpos: [19:10]; default: 0;
 *  Indicates the number of block valid bit.
 */
#define EFUSE_BLK0_VALID_BIT_CNT    0x000003FFU
#define EFUSE_BLK0_VALID_BIT_CNT_M  (EFUSE_BLK0_VALID_BIT_CNT_V << EFUSE_BLK0_VALID_BIT_CNT_S)
#define EFUSE_BLK0_VALID_BIT_CNT_V  0x000003FFU
#define EFUSE_BLK0_VALID_BIT_CNT_S  10

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
/** EFUSE_READ_INIT_NUM : R/W; bitpos: [31:24]; default: 18;
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
/** EFUSE_PWR_ON_NUM : R/W; bitpos: [23:8]; default: 12288;
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
/** EFUSE_PWR_OFF_NUM : R/W; bitpos: [15:0]; default: 400;
 *  Configures the power outage time for VDDQ.
 */
#define EFUSE_PWR_OFF_NUM    0x0000FFFFU
#define EFUSE_PWR_OFF_NUM_M  (EFUSE_PWR_OFF_NUM_V << EFUSE_PWR_OFF_NUM_S)
#define EFUSE_PWR_OFF_NUM_V  0x0000FFFFU
#define EFUSE_PWR_OFF_NUM_S  0
/** EFUSE_TPGM : R/W; bitpos: [31:16]; default: 200;
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
/** EFUSE_DATE : R/W; bitpos: [27:0]; default: 35676928;
 *  Stores eFuse version.
 */
#define EFUSE_DATE    0x0FFFFFFFU
#define EFUSE_DATE_M  (EFUSE_DATE_V << EFUSE_DATE_S)
#define EFUSE_DATE_V  0x0FFFFFFFU
#define EFUSE_DATE_S  0

#ifdef __cplusplus
}
#endif
