/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_EFUSE_REG_H_
#define _SOC_EFUSE_REG_H_


#include "soc.h"
#ifdef __cplusplus
extern "C" {
#endif

#define EFUSE_PGM_DATA0_REG          (DR_REG_EFUSE_BASE + 0x0)
/* EFUSE_PGM_DATA_0 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the 0th 32-bit data to be programmed..*/
#define EFUSE_PGM_DATA_0    0xFFFFFFFF
#define EFUSE_PGM_DATA_0_M  ((EFUSE_PGM_DATA_0_V)<<(EFUSE_PGM_DATA_0_S))
#define EFUSE_PGM_DATA_0_V  0xFFFFFFFF
#define EFUSE_PGM_DATA_0_S  0

#define EFUSE_PGM_DATA1_REG          (DR_REG_EFUSE_BASE + 0x4)
/* EFUSE_PGM_DATA_1 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the 1st 32-bit data to be programmed..*/
#define EFUSE_PGM_DATA_1    0xFFFFFFFF
#define EFUSE_PGM_DATA_1_M  ((EFUSE_PGM_DATA_1_V)<<(EFUSE_PGM_DATA_1_S))
#define EFUSE_PGM_DATA_1_V  0xFFFFFFFF
#define EFUSE_PGM_DATA_1_S  0

#define EFUSE_PGM_DATA2_REG          (DR_REG_EFUSE_BASE + 0x8)
/* EFUSE_PGM_DATA_2 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the 2nd 32-bit data to be programmed..*/
#define EFUSE_PGM_DATA_2    0xFFFFFFFF
#define EFUSE_PGM_DATA_2_M  ((EFUSE_PGM_DATA_2_V)<<(EFUSE_PGM_DATA_2_S))
#define EFUSE_PGM_DATA_2_V  0xFFFFFFFF
#define EFUSE_PGM_DATA_2_S  0

#define EFUSE_PGM_DATA3_REG          (DR_REG_EFUSE_BASE + 0xC)
/* EFUSE_PGM_DATA_3 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the 3rd 32-bit data to be programmed..*/
#define EFUSE_PGM_DATA_3    0xFFFFFFFF
#define EFUSE_PGM_DATA_3_M  ((EFUSE_PGM_DATA_3_V)<<(EFUSE_PGM_DATA_3_S))
#define EFUSE_PGM_DATA_3_V  0xFFFFFFFF
#define EFUSE_PGM_DATA_3_S  0

#define EFUSE_PGM_DATA4_REG          (DR_REG_EFUSE_BASE + 0x10)
/* EFUSE_PGM_DATA_4 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the 4th 32-bit data to be programmed..*/
#define EFUSE_PGM_DATA_4    0xFFFFFFFF
#define EFUSE_PGM_DATA_4_M  ((EFUSE_PGM_DATA_4_V)<<(EFUSE_PGM_DATA_4_S))
#define EFUSE_PGM_DATA_4_V  0xFFFFFFFF
#define EFUSE_PGM_DATA_4_S  0

#define EFUSE_PGM_DATA5_REG          (DR_REG_EFUSE_BASE + 0x14)
/* EFUSE_PGM_DATA_5 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the 5th 32-bit data to be programmed..*/
#define EFUSE_PGM_DATA_5    0xFFFFFFFF
#define EFUSE_PGM_DATA_5_M  ((EFUSE_PGM_DATA_5_V)<<(EFUSE_PGM_DATA_5_S))
#define EFUSE_PGM_DATA_5_V  0xFFFFFFFF
#define EFUSE_PGM_DATA_5_S  0

#define EFUSE_PGM_DATA6_REG          (DR_REG_EFUSE_BASE + 0x18)
/* EFUSE_PGM_DATA_6 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the 6th 32-bit data to be programmed..*/
#define EFUSE_PGM_DATA_6    0xFFFFFFFF
#define EFUSE_PGM_DATA_6_M  ((EFUSE_PGM_DATA_6_V)<<(EFUSE_PGM_DATA_6_S))
#define EFUSE_PGM_DATA_6_V  0xFFFFFFFF
#define EFUSE_PGM_DATA_6_S  0

#define EFUSE_PGM_DATA7_REG          (DR_REG_EFUSE_BASE + 0x1C)
/* EFUSE_PGM_DATA_7 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the 7th 32-bit data to be programmed..*/
#define EFUSE_PGM_DATA_7    0xFFFFFFFF
#define EFUSE_PGM_DATA_7_M  ((EFUSE_PGM_DATA_7_V)<<(EFUSE_PGM_DATA_7_S))
#define EFUSE_PGM_DATA_7_V  0xFFFFFFFF
#define EFUSE_PGM_DATA_7_S  0

#define EFUSE_PGM_CHECK_VALUE0_REG          (DR_REG_EFUSE_BASE + 0x20)
/* EFUSE_PGM_RS_DATA_0 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the 0th 32-bit RS code to be programmed..*/
#define EFUSE_PGM_RS_DATA_0    0xFFFFFFFF
#define EFUSE_PGM_RS_DATA_0_M  ((EFUSE_PGM_RS_DATA_0_V)<<(EFUSE_PGM_RS_DATA_0_S))
#define EFUSE_PGM_RS_DATA_0_V  0xFFFFFFFF
#define EFUSE_PGM_RS_DATA_0_S  0

#define EFUSE_PGM_CHECK_VALUE1_REG          (DR_REG_EFUSE_BASE + 0x24)
/* EFUSE_PGM_RS_DATA_1 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the 1st 32-bit RS code to be programmed..*/
#define EFUSE_PGM_RS_DATA_1    0xFFFFFFFF
#define EFUSE_PGM_RS_DATA_1_M  ((EFUSE_PGM_RS_DATA_1_V)<<(EFUSE_PGM_RS_DATA_1_S))
#define EFUSE_PGM_RS_DATA_1_V  0xFFFFFFFF
#define EFUSE_PGM_RS_DATA_1_S  0

#define EFUSE_PGM_CHECK_VALUE2_REG          (DR_REG_EFUSE_BASE + 0x28)
/* EFUSE_PGM_RS_DATA_2 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the 2nd 32-bit RS code to be programmed..*/
#define EFUSE_PGM_RS_DATA_2    0xFFFFFFFF
#define EFUSE_PGM_RS_DATA_2_M  ((EFUSE_PGM_RS_DATA_2_V)<<(EFUSE_PGM_RS_DATA_2_S))
#define EFUSE_PGM_RS_DATA_2_V  0xFFFFFFFF
#define EFUSE_PGM_RS_DATA_2_S  0

#define EFUSE_RD_WR_DIS_REG          (DR_REG_EFUSE_BASE + 0x2C)
/* EFUSE_WR_DIS : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Disable programming of individual eFuses..*/
#define EFUSE_WR_DIS    0xFFFFFFFF
#define EFUSE_WR_DIS_M  ((EFUSE_WR_DIS_V)<<(EFUSE_WR_DIS_S))
#define EFUSE_WR_DIS_V  0xFFFFFFFF
#define EFUSE_WR_DIS_S  0

#define EFUSE_RD_REPEAT_DATA0_REG          (DR_REG_EFUSE_BASE + 0x30)
/* EFUSE_VDD_SPI_DREFH : RO ;bitpos:[31:30] ;default: 2'h0 ; */
/*description: SPI regulator high voltage reference..*/
#define EFUSE_VDD_SPI_DREFH    0x00000003
#define EFUSE_VDD_SPI_DREFH_M  ((EFUSE_VDD_SPI_DREFH_V)<<(EFUSE_VDD_SPI_DREFH_S))
#define EFUSE_VDD_SPI_DREFH_V  0x3
#define EFUSE_VDD_SPI_DREFH_S  30
/* EFUSE_VDD_SPI_MODECURLIM : RO ;bitpos:[29] ;default: 1'b0 ; */
/*description: SPI regulator switches current limit mode..*/
#define EFUSE_VDD_SPI_MODECURLIM    (BIT(29))
#define EFUSE_VDD_SPI_MODECURLIM_M  (BIT(29))
#define EFUSE_VDD_SPI_MODECURLIM_V  0x1
#define EFUSE_VDD_SPI_MODECURLIM_S  29
/* EFUSE_BTLC_GPIO_ENABLE : RO ;bitpos:[28:27] ;default: 2'h0 ; */
/*description: Enable btlc gpio..*/
#define EFUSE_BTLC_GPIO_ENABLE    0x00000003
#define EFUSE_BTLC_GPIO_ENABLE_M  ((EFUSE_BTLC_GPIO_ENABLE_V)<<(EFUSE_BTLC_GPIO_ENABLE_S))
#define EFUSE_BTLC_GPIO_ENABLE_V  0x3
#define EFUSE_BTLC_GPIO_ENABLE_S  27
/* EFUSE_EXT_PHY_ENABLE : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: Set this bit to enable external PHY..*/
#define EFUSE_EXT_PHY_ENABLE    (BIT(26))
#define EFUSE_EXT_PHY_ENABLE_M  (BIT(26))
#define EFUSE_EXT_PHY_ENABLE_V  0x1
#define EFUSE_EXT_PHY_ENABLE_S  26
/* EFUSE_USB_EXCHG_PINS : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: Set this bit to exchange USB D+ and D- pins..*/
#define EFUSE_USB_EXCHG_PINS    (BIT(25))
#define EFUSE_USB_EXCHG_PINS_M  (BIT(25))
#define EFUSE_USB_EXCHG_PINS_V  0x1
#define EFUSE_USB_EXCHG_PINS_S  25
/* EFUSE_USB_DREFL : RO ;bitpos:[24:23] ;default: 2'h0 ; */
/*description: Controls single-end input threshold vrefl, 0.8 V to 1.04 V with step of 80 mV, s
tored in eFuse..*/
#define EFUSE_USB_DREFL    0x00000003
#define EFUSE_USB_DREFL_M  ((EFUSE_USB_DREFL_V)<<(EFUSE_USB_DREFL_S))
#define EFUSE_USB_DREFL_V  0x3
#define EFUSE_USB_DREFL_S  23
/* EFUSE_USB_DREFH : RO ;bitpos:[22:21] ;default: 2'h0 ; */
/*description: Controls single-end input threshold vrefh, 1.76 V to 2 V with step of 80 mV, sto
red in eFuse..*/
#define EFUSE_USB_DREFH    0x00000003
#define EFUSE_USB_DREFH_M  ((EFUSE_USB_DREFH_V)<<(EFUSE_USB_DREFH_S))
#define EFUSE_USB_DREFH_V  0x3
#define EFUSE_USB_DREFH_S  21
/* EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: Set this bit to disable flash encryption when in download boot modes..*/
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT    (BIT(20))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_M  (BIT(20))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_V  0x1
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_S  20
/* EFUSE_DIS_PAD_JTAG : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: Set this bit to disable JTAG in the hard way. JTAG is disabled permanently..*/
#define EFUSE_DIS_PAD_JTAG    (BIT(19))
#define EFUSE_DIS_PAD_JTAG_M  (BIT(19))
#define EFUSE_DIS_PAD_JTAG_V  0x1
#define EFUSE_DIS_PAD_JTAG_S  19
/* EFUSE_SOFT_DIS_JTAG : RO ;bitpos:[18:16] ;default: 3'h0 ; */
/*description: Set these bits to disable JTAG in the soft way (odd number 1 means disable ). JT
AG can be enabled in HMAC module..*/
#define EFUSE_SOFT_DIS_JTAG    0x00000007
#define EFUSE_SOFT_DIS_JTAG_M  ((EFUSE_SOFT_DIS_JTAG_V)<<(EFUSE_SOFT_DIS_JTAG_S))
#define EFUSE_SOFT_DIS_JTAG_V  0x7
#define EFUSE_SOFT_DIS_JTAG_S  16
/* EFUSE_DIS_APP_CPU : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: Disable app cpu..*/
#define EFUSE_DIS_APP_CPU    (BIT(15))
#define EFUSE_DIS_APP_CPU_M  (BIT(15))
#define EFUSE_DIS_APP_CPU_V  0x1
#define EFUSE_DIS_APP_CPU_S  15
/* EFUSE_DIS_CAN : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: Set this bit to disable CAN function..*/
#define EFUSE_DIS_CAN    (BIT(14))
#define EFUSE_DIS_CAN_M  (BIT(14))
#define EFUSE_DIS_CAN_V  0x1
#define EFUSE_DIS_CAN_S  14
/* EFUSE_DIS_USB : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: Set this bit to disable USB function..*/
#define EFUSE_DIS_USB    (BIT(13))
#define EFUSE_DIS_USB_M  (BIT(13))
#define EFUSE_DIS_USB_V  0x1
#define EFUSE_DIS_USB_S  13
/* EFUSE_DIS_FORCE_DOWNLOAD : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to disable the function that forces chip into download mode..*/
#define EFUSE_DIS_FORCE_DOWNLOAD    (BIT(12))
#define EFUSE_DIS_FORCE_DOWNLOAD_M  (BIT(12))
#define EFUSE_DIS_FORCE_DOWNLOAD_V  0x1
#define EFUSE_DIS_FORCE_DOWNLOAD_S  12
/* EFUSE_DIS_DOWNLOAD_DCACHE : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: Set this bit to disable Dcache in download mode ( boot_mode[3:0] is 0, 1, 2, 3,
6, 7)..*/
#define EFUSE_DIS_DOWNLOAD_DCACHE    (BIT(11))
#define EFUSE_DIS_DOWNLOAD_DCACHE_M  (BIT(11))
#define EFUSE_DIS_DOWNLOAD_DCACHE_V  0x1
#define EFUSE_DIS_DOWNLOAD_DCACHE_S  11
/* EFUSE_DIS_DOWNLOAD_ICACHE : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: Set this bit to disable Icache in download mode (boot_mode[3:0] is 0, 1, 2, 3, 6
, 7)..*/
#define EFUSE_DIS_DOWNLOAD_ICACHE    (BIT(10))
#define EFUSE_DIS_DOWNLOAD_ICACHE_M  (BIT(10))
#define EFUSE_DIS_DOWNLOAD_ICACHE_V  0x1
#define EFUSE_DIS_DOWNLOAD_ICACHE_S  10
/* EFUSE_DIS_DCACHE : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this bit to disable Dcache..*/
#define EFUSE_DIS_DCACHE    (BIT(9))
#define EFUSE_DIS_DCACHE_M  (BIT(9))
#define EFUSE_DIS_DCACHE_V  0x1
#define EFUSE_DIS_DCACHE_S  9
/* EFUSE_DIS_ICACHE : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this bit to disable Icache..*/
#define EFUSE_DIS_ICACHE    (BIT(8))
#define EFUSE_DIS_ICACHE_M  (BIT(8))
#define EFUSE_DIS_ICACHE_V  0x1
#define EFUSE_DIS_ICACHE_S  8
/* EFUSE_RPT4_RESERVED5 : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: Reserved */
#define EFUSE_RPT4_RESERVED5    (BIT(7))
#define EFUSE_RPT4_RESERVED5_M  (BIT(7))
#define EFUSE_RPT4_RESERVED5_V  0x1
#define EFUSE_RPT4_RESERVED5_S  7
/* EFUSE_RD_DIS : RO ;bitpos:[6:0] ;default: 7'h0 ; */
/*description: Set this bit to disable reading from BlOCK4-10..*/
#define EFUSE_RD_DIS    0x0000007F
#define EFUSE_RD_DIS_M  ((EFUSE_RD_DIS_V)<<(EFUSE_RD_DIS_S))
#define EFUSE_RD_DIS_V  0x7F
#define EFUSE_RD_DIS_S  0

#define EFUSE_RD_REPEAT_DATA1_REG          (DR_REG_EFUSE_BASE + 0x34)
/* EFUSE_KEY_PURPOSE_1 : RO ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: Purpose of Key1..*/
#define EFUSE_KEY_PURPOSE_1    0x0000000F
#define EFUSE_KEY_PURPOSE_1_M  ((EFUSE_KEY_PURPOSE_1_V)<<(EFUSE_KEY_PURPOSE_1_S))
#define EFUSE_KEY_PURPOSE_1_V  0xF
#define EFUSE_KEY_PURPOSE_1_S  28
/* EFUSE_KEY_PURPOSE_0 : RO ;bitpos:[27:24] ;default: 4'h0 ; */
/*description: Purpose of Key0..*/
#define EFUSE_KEY_PURPOSE_0    0x0000000F
#define EFUSE_KEY_PURPOSE_0_M  ((EFUSE_KEY_PURPOSE_0_V)<<(EFUSE_KEY_PURPOSE_0_S))
#define EFUSE_KEY_PURPOSE_0_V  0xF
#define EFUSE_KEY_PURPOSE_0_S  24
/* EFUSE_SECURE_BOOT_KEY_REVOKE2 : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: Set this bit to enable revoking third secure boot key..*/
#define EFUSE_SECURE_BOOT_KEY_REVOKE2    (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_M  (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_S  23
/* EFUSE_SECURE_BOOT_KEY_REVOKE1 : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this bit to enable revoking second secure boot key..*/
#define EFUSE_SECURE_BOOT_KEY_REVOKE1    (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_M  (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_S  22
/* EFUSE_SECURE_BOOT_KEY_REVOKE0 : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to enable revoking first secure boot key..*/
#define EFUSE_SECURE_BOOT_KEY_REVOKE0    (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_M  (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_S  21
/* EFUSE_SPI_BOOT_CRYPT_CNT : RO ;bitpos:[20:18] ;default: 3'h0 ; */
/*description: Set this bit to enable SPI boot encrypt/decrypt. Odd number of 1: enable. even n
umber of 1: disable..*/
#define EFUSE_SPI_BOOT_CRYPT_CNT    0x00000007
#define EFUSE_SPI_BOOT_CRYPT_CNT_M  ((EFUSE_SPI_BOOT_CRYPT_CNT_V)<<(EFUSE_SPI_BOOT_CRYPT_CNT_S))
#define EFUSE_SPI_BOOT_CRYPT_CNT_V  0x7
#define EFUSE_SPI_BOOT_CRYPT_CNT_S  18
/* EFUSE_WDT_DELAY_SEL : RO ;bitpos:[17:16] ;default: 2'h0 ; */
/*description: Selects RTC watchdog timeout threshold, in unit of slow clock cycle. 0: 40000. 1
: 80000. 2: 160000. 3:320000..*/
#define EFUSE_WDT_DELAY_SEL    0x00000003
#define EFUSE_WDT_DELAY_SEL_M  ((EFUSE_WDT_DELAY_SEL_V)<<(EFUSE_WDT_DELAY_SEL_S))
#define EFUSE_WDT_DELAY_SEL_V  0x3
#define EFUSE_WDT_DELAY_SEL_S  16
/* EFUSE_VDD_SPI_DCAP : RO ;bitpos:[15:14] ;default: 2'h0 ; */
/*description: Prevents SPI regulator from overshoot..*/
#define EFUSE_VDD_SPI_DCAP    0x00000003
#define EFUSE_VDD_SPI_DCAP_M  ((EFUSE_VDD_SPI_DCAP_V)<<(EFUSE_VDD_SPI_DCAP_S))
#define EFUSE_VDD_SPI_DCAP_V  0x3
#define EFUSE_VDD_SPI_DCAP_S  14
/* EFUSE_VDD_SPI_INIT : RO ;bitpos:[13:12] ;default: 2'h0 ; */
/*description: Adds resistor from LDO output to ground. 0: no resistance 1: 6 K 2: 4 K 3: 2 K..*/
#define EFUSE_VDD_SPI_INIT    0x00000003
#define EFUSE_VDD_SPI_INIT_M  ((EFUSE_VDD_SPI_INIT_V)<<(EFUSE_VDD_SPI_INIT_S))
#define EFUSE_VDD_SPI_INIT_V  0x3
#define EFUSE_VDD_SPI_INIT_S  12
/* EFUSE_VDD_SPI_DCURLIM : RO ;bitpos:[11:9]] ;default: 3'h0 ; */
/*description: Tunes the current limit threshold of SPI regulator when tieh=0, about 800 mA/(8+
d)..*/
#define EFUSE_VDD_SPI_DCURLIM    0x00000007
#define EFUSE_VDD_SPI_DCURLIM_M  ((EFUSE_VDD_SPI_DCURLIM_V)<<(EFUSE_VDD_SPI_DCURLIM_S))
#define EFUSE_VDD_SPI_DCURLIM_V  0x7
#define EFUSE_VDD_SPI_DCURLIM_S  9
/* EFUSE_VDD_SPI_ENCURLIM : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set SPI regulator to 1 to enable output current limit..*/
#define EFUSE_VDD_SPI_ENCURLIM    (BIT(8))
#define EFUSE_VDD_SPI_ENCURLIM_M  (BIT(8))
#define EFUSE_VDD_SPI_ENCURLIM_V  0x1
#define EFUSE_VDD_SPI_ENCURLIM_S  8
/* EFUSE_VDD_SPI_EN_INIT : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set SPI regulator to 0 to configure init[1:0]=0..*/
#define EFUSE_VDD_SPI_EN_INIT    (BIT(7))
#define EFUSE_VDD_SPI_EN_INIT_M  (BIT(7))
#define EFUSE_VDD_SPI_EN_INIT_V  0x1
#define EFUSE_VDD_SPI_EN_INIT_S  7
/* EFUSE_VDD_SPI_FORCE : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit and force to use the configuration of eFuse to configure VDD_SPI..*/
#define EFUSE_VDD_SPI_FORCE    (BIT(6))
#define EFUSE_VDD_SPI_FORCE_M  (BIT(6))
#define EFUSE_VDD_SPI_FORCE_V  0x1
#define EFUSE_VDD_SPI_FORCE_S  6
/* EFUSE_VDD_SPI_TIEH : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: SPI regulator output is short connected to VDD3P3_RTC_IO..*/
#define EFUSE_VDD_SPI_TIEH    (BIT(5))
#define EFUSE_VDD_SPI_TIEH_M  (BIT(5))
#define EFUSE_VDD_SPI_TIEH_V  0x1
#define EFUSE_VDD_SPI_TIEH_S  5
/* EFUSE_VDD_SPI_XPD : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: SPI regulator power up signal..*/
#define EFUSE_VDD_SPI_XPD    (BIT(4))
#define EFUSE_VDD_SPI_XPD_M  (BIT(4))
#define EFUSE_VDD_SPI_XPD_V  0x1
#define EFUSE_VDD_SPI_XPD_S  4
/* EFUSE_VDD_SPI_DREFL : RO ;bitpos:[3:2] ;default: 2'h0 ; */
/*description: SPI regulator low voltage reference..*/
#define EFUSE_VDD_SPI_DREFL    0x00000003
#define EFUSE_VDD_SPI_DREFL_M  ((EFUSE_VDD_SPI_DREFL_V)<<(EFUSE_VDD_SPI_DREFL_S))
#define EFUSE_VDD_SPI_DREFL_V  0x3
#define EFUSE_VDD_SPI_DREFL_S  2
/* EFUSE_VDD_SPI_DREFM : RO ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: SPI regulator medium voltage reference..*/
#define EFUSE_VDD_SPI_DREFM    0x00000003
#define EFUSE_VDD_SPI_DREFM_M  ((EFUSE_VDD_SPI_DREFM_V)<<(EFUSE_VDD_SPI_DREFM_S))
#define EFUSE_VDD_SPI_DREFM_V  0x3
#define EFUSE_VDD_SPI_DREFM_S  0

#define EFUSE_RD_REPEAT_DATA2_REG          (DR_REG_EFUSE_BASE + 0x38)
/* EFUSE_FLASH_TPUW : RO ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: Configures flash waiting time after power-up, in unit of ms. If the value is les
s than 15, the waiting time is the configurable value; Otherwise, the waiting ti
me is twice the configurable value..*/
#define EFUSE_FLASH_TPUW    0x0000000F
#define EFUSE_FLASH_TPUW_M  ((EFUSE_FLASH_TPUW_V)<<(EFUSE_FLASH_TPUW_S))
#define EFUSE_FLASH_TPUW_V  0xF
#define EFUSE_FLASH_TPUW_S  28
/* EFUSE_POWER_GLITCH_DSENSE : RO ;bitpos:[27:26] ;default: 2'h0 ; */
/*description: Sample delay configuration of power glitch..*/
#define EFUSE_POWER_GLITCH_DSENSE    0x00000003
#define EFUSE_POWER_GLITCH_DSENSE_M  ((EFUSE_POWER_GLITCH_DSENSE_V)<<(EFUSE_POWER_GLITCH_DSENSE_S))
#define EFUSE_POWER_GLITCH_DSENSE_V  0x3
#define EFUSE_POWER_GLITCH_DSENSE_S  26
/* EFUSE_USB_PHY_SEL : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: This bit is used to switch internal PHY and external PHY for USB OTG and USB Dev
ice. 0: internal PHY is assigned to USB Device while external PHY is assigned to
 USB OTG. 1: internal PHY is assigned to USB OTG while external PHY is assigned
to USB Device..*/
#define EFUSE_USB_PHY_SEL    (BIT(25))
#define EFUSE_USB_PHY_SEL_M  (BIT(25))
#define EFUSE_USB_PHY_SEL_V  0x1
#define EFUSE_USB_PHY_SEL_S  25
/* EFUSE_STRAP_JTAG_SEL : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: Set this bit to enable selection between usb_to_jtag and pad_to_jtag through str
apping gpio10 when both reg_dis_usb_jtag and reg_dis_pad_jtag are equal to 0..*/
#define EFUSE_STRAP_JTAG_SEL    (BIT(24))
#define EFUSE_STRAP_JTAG_SEL_M  (BIT(24))
#define EFUSE_STRAP_JTAG_SEL_V  0x1
#define EFUSE_STRAP_JTAG_SEL_S  24
/* EFUSE_DIS_USB_DEVICE : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: Set this bit to disable usb device..*/
#define EFUSE_DIS_USB_DEVICE    (BIT(23))
#define EFUSE_DIS_USB_DEVICE_M  (BIT(23))
#define EFUSE_DIS_USB_DEVICE_V  0x1
#define EFUSE_DIS_USB_DEVICE_S  23
/* EFUSE_DIS_USB_JTAG : RO ;bitpos:[22] ;default: 6'h0 ; */
/*description: Set this bit to disable function of usb switch to jtag in module of usb device..*/
#define EFUSE_DIS_USB_JTAG    (BIT(22))
#define EFUSE_DIS_USB_JTAG_M  (BIT(22))
#define EFUSE_DIS_USB_JTAG_V  0x1
#define EFUSE_DIS_USB_JTAG_S  22
/* EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to enable revoking aggressive secure boot..*/
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE    (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_M  (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_V  0x1
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_S  21
/* EFUSE_SECURE_BOOT_EN : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: Set this bit to enable secure boot..*/
#define EFUSE_SECURE_BOOT_EN    (BIT(20))
#define EFUSE_SECURE_BOOT_EN_M  (BIT(20))
#define EFUSE_SECURE_BOOT_EN_V  0x1
#define EFUSE_SECURE_BOOT_EN_S  20
/* EFUSE_RPT4_RESERVED0 : RO ;bitpos:[19:16] ;default: 4'h0 ; */
/*description: Reserved (used for four backups method)..*/
#define EFUSE_RPT4_RESERVED0    0x0000000F
#define EFUSE_RPT4_RESERVED0_M  ((EFUSE_RPT4_RESERVED0_V)<<(EFUSE_RPT4_RESERVED0_S))
#define EFUSE_RPT4_RESERVED0_V  0xF
#define EFUSE_RPT4_RESERVED0_S  16
/* EFUSE_KEY_PURPOSE_5 : RO ;bitpos:[15:12] ;default: 4'h0 ; */
/*description: Purpose of Key5..*/
#define EFUSE_KEY_PURPOSE_5    0x0000000F
#define EFUSE_KEY_PURPOSE_5_M  ((EFUSE_KEY_PURPOSE_5_V)<<(EFUSE_KEY_PURPOSE_5_S))
#define EFUSE_KEY_PURPOSE_5_V  0xF
#define EFUSE_KEY_PURPOSE_5_S  12
/* EFUSE_KEY_PURPOSE_4 : RO ;bitpos:[11:8] ;default: 4'h0 ; */
/*description: Purpose of Key4..*/
#define EFUSE_KEY_PURPOSE_4    0x0000000F
#define EFUSE_KEY_PURPOSE_4_M  ((EFUSE_KEY_PURPOSE_4_V)<<(EFUSE_KEY_PURPOSE_4_S))
#define EFUSE_KEY_PURPOSE_4_V  0xF
#define EFUSE_KEY_PURPOSE_4_S  8
/* EFUSE_KEY_PURPOSE_3 : RO ;bitpos:[7:4] ;default: 4'h0 ; */
/*description: Purpose of Key3..*/
#define EFUSE_KEY_PURPOSE_3    0x0000000F
#define EFUSE_KEY_PURPOSE_3_M  ((EFUSE_KEY_PURPOSE_3_V)<<(EFUSE_KEY_PURPOSE_3_S))
#define EFUSE_KEY_PURPOSE_3_V  0xF
#define EFUSE_KEY_PURPOSE_3_S  4
/* EFUSE_KEY_PURPOSE_2 : RO ;bitpos:[3:0] ;default: 4'h0 ; */
/*description: Purpose of Key2..*/
#define EFUSE_KEY_PURPOSE_2    0x0000000F
#define EFUSE_KEY_PURPOSE_2_M  ((EFUSE_KEY_PURPOSE_2_V)<<(EFUSE_KEY_PURPOSE_2_S))
#define EFUSE_KEY_PURPOSE_2_V  0xF
#define EFUSE_KEY_PURPOSE_2_S  0

#define EFUSE_RD_REPEAT_DATA3_REG          (DR_REG_EFUSE_BASE + 0x3C)
/* EFUSE_DIS_USB_OTG_DOWNLOAD_MODE : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description:  Set this bit to disable download through USB-OTG*/
#define EFUSE_DIS_USB_OTG_DOWNLOAD_MODE  (BIT(31))
#define EFUSE_DIS_USB_OTG_DOWNLOAD_MODE_M  (BIT(31))
#define EFUSE_DIS_USB_OTG_DOWNLOAD_MODE_V  0x1
#define EFUSE_DIS_USB_OTG_DOWNLOAD_MODE_S  31
/* EFUSE_RPT4_RESERVED1 : RO ;bitpos:[30] ;default: 1'h0 ; */
/*description: Reserved (used for four backups method).*/
#define EFUSE_RPT4_RESERVED1  (BIT(30))
#define EFUSE_RPT4_RESERVED1_M  (BIT(30))
#define EFUSE_RPT4_RESERVED1_V  0x1
#define EFUSE_RPT4_RESERVED1_S  30
/* EFUSE_SECURE_VERSION : RO ;bitpos:[29:14] ;default: 16'h0 ; */
/*description: Secure version (used by ESP-IDF anti-rollback feature)..*/
#define EFUSE_SECURE_VERSION    0x0000FFFF
#define EFUSE_SECURE_VERSION_M  ((EFUSE_SECURE_VERSION_V)<<(EFUSE_SECURE_VERSION_S))
#define EFUSE_SECURE_VERSION_V  0xFFFF
#define EFUSE_SECURE_VERSION_S  14
/* EFUSE_FORCE_SEND_RESUME : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: Set this bit to force ROM code to send a resume command during SPI boot..*/
#define EFUSE_FORCE_SEND_RESUME    (BIT(13))
#define EFUSE_FORCE_SEND_RESUME_M  (BIT(13))
#define EFUSE_FORCE_SEND_RESUME_V  0x1
#define EFUSE_FORCE_SEND_RESUME_S  13
/* EFUSE_FLASH_ECC_EN : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set 1 to enable ECC for flash boot..*/
#define EFUSE_FLASH_ECC_EN    (BIT(12))
#define EFUSE_FLASH_ECC_EN_M  (BIT(12))
#define EFUSE_FLASH_ECC_EN_V  0x1
#define EFUSE_FLASH_ECC_EN_S  12
/* EFUSE_FLASH_PAGE_SIZE : RO ;bitpos:[11:10] ;default: 2'h0 ; */
/*description: Set Flash page size..*/
#define EFUSE_FLASH_PAGE_SIZE    0x00000003
#define EFUSE_FLASH_PAGE_SIZE_M  ((EFUSE_FLASH_PAGE_SIZE_V)<<(EFUSE_FLASH_PAGE_SIZE_S))
#define EFUSE_FLASH_PAGE_SIZE_V  0x3
#define EFUSE_FLASH_PAGE_SIZE_S  10
/* EFUSE_FLASH_TYPE : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set the maximum lines of SPI flash. 0: four lines. 1: eight lines..*/
#define EFUSE_FLASH_TYPE    (BIT(9))
#define EFUSE_FLASH_TYPE_M  (BIT(9))
#define EFUSE_FLASH_TYPE_V  0x1
#define EFUSE_FLASH_TYPE_S  9
/* EFUSE_PIN_POWER_SELECTION : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: GPIO33-GPIO37 power supply selection in ROM code. 0: VDD3P3_CPU. 1: VDD_SPI..*/
#define EFUSE_PIN_POWER_SELECTION    (BIT(8))
#define EFUSE_PIN_POWER_SELECTION_M  (BIT(8))
#define EFUSE_PIN_POWER_SELECTION_V  0x1
#define EFUSE_PIN_POWER_SELECTION_S  8
/* EFUSE_UART_PRINT_CONTROL : RO ;bitpos:[7:6] ;default: 2'h0 ; */
/*description: Set the default UARTboot message output mode. 00: Enabled. 01: Enabled when GPIO
8 is low at reset. 10: Enabled when GPIO8 is high at reset. 11:disabled..*/
#define EFUSE_UART_PRINT_CONTROL    0x00000003
#define EFUSE_UART_PRINT_CONTROL_M  ((EFUSE_UART_PRINT_CONTROL_V)<<(EFUSE_UART_PRINT_CONTROL_S))
#define EFUSE_UART_PRINT_CONTROL_V  0x3
#define EFUSE_UART_PRINT_CONTROL_S  6
/* EFUSE_ENABLE_SECURITY_DOWNLOAD : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to enable secure UART download mode..*/
#define EFUSE_ENABLE_SECURITY_DOWNLOAD    (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_M  (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_V  0x1
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_S  5
/* EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to disable download through USB-Seial-JTAG.*/
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE    (BIT(4))
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_M  (BIT(4))
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_V  0x1
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_S  4
/* EFUSE_FLASH_ECC_MODE : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set ECC mode in ROM, 0: ROM would Enable Flash ECC 16to18 byte mode. 1:ROM would
 use 16to17 byte mode..*/
#define EFUSE_FLASH_ECC_MODE    (BIT(3))
#define EFUSE_FLASH_ECC_MODE_M  (BIT(3))
#define EFUSE_FLASH_ECC_MODE_V  0x1
#define EFUSE_FLASH_ECC_MODE_S  3
/* EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: Disable USB-Serial-JTAG print during rom boot.*/
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT    (BIT(2))
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_M  (BIT(2))
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_V  0x1
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_S  2
/* EFUSE_DIS_DIRECT_BOOT : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to disable direct boot.*/
#define EFUSE_DIS_DIRECT_BOOT    (BIT(1))
#define EFUSE_DIS_DIRECT_BOOT_M  (BIT(1))
#define EFUSE_DIS_DIRECT_BOOT_V  0x1
#define EFUSE_DIS_DIRECT_BOOT_S  1
/* EFUSE_DIS_DOWNLOAD_MODE : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to disable download mode (boot_mode[3:0] = 0, 1, 2, 3, 6, 7)..*/
#define EFUSE_DIS_DOWNLOAD_MODE    (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_M  (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_V  0x1
#define EFUSE_DIS_DOWNLOAD_MODE_S  0

#define EFUSE_RD_REPEAT_DATA4_REG          (DR_REG_EFUSE_BASE + 0x40)
/* EFUSE_RPT4_RESERVED2 : RO ;bitpos:[23:0] ;default: 24'h0 ; */
/*description: Reserved (used for four backups method)..*/
#define EFUSE_RPT4_RESERVED2    0x00FFFFFF
#define EFUSE_RPT4_RESERVED2_M  ((EFUSE_RPT4_RESERVED2_V)<<(EFUSE_RPT4_RESERVED2_S))
#define EFUSE_RPT4_RESERVED2_V  0xFFFFFF
#define EFUSE_RPT4_RESERVED2_S  0

#define EFUSE_RD_MAC_SPI_SYS_0_REG          (DR_REG_EFUSE_BASE + 0x44)
/* EFUSE_MAC_0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the low 32 bits of MAC address..*/
#define EFUSE_MAC_0    0xFFFFFFFF
#define EFUSE_MAC_0_M  ((EFUSE_MAC_0_V)<<(EFUSE_MAC_0_S))
#define EFUSE_MAC_0_V  0xFFFFFFFF
#define EFUSE_MAC_0_S  0

#define EFUSE_RD_MAC_SPI_SYS_1_REG          (DR_REG_EFUSE_BASE + 0x48)
/* EFUSE_SPI_PAD_CONF_0 : RO ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: Stores the zeroth part of SPI_PAD_CONF..*/
#define EFUSE_SPI_PAD_CONF_0    0x0000FFFF
#define EFUSE_SPI_PAD_CONF_0_M  ((EFUSE_SPI_PAD_CONF_0_V)<<(EFUSE_SPI_PAD_CONF_0_S))
#define EFUSE_SPI_PAD_CONF_0_V  0xFFFF
#define EFUSE_SPI_PAD_CONF_0_S  16
/* EFUSE_MAC_1 : RO ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: Stores the high 16 bits of MAC address..*/
#define EFUSE_MAC_1    0x0000FFFF
#define EFUSE_MAC_1_M  ((EFUSE_MAC_1_V)<<(EFUSE_MAC_1_S))
#define EFUSE_MAC_1_V  0xFFFF
#define EFUSE_MAC_1_S  0

#define EFUSE_RD_MAC_SPI_SYS_2_REG          (DR_REG_EFUSE_BASE + 0x4C)
/* EFUSE_SPI_PAD_CONF_1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first part of SPI_PAD_CONF..*/
#define EFUSE_SPI_PAD_CONF_1    0xFFFFFFFF
#define EFUSE_SPI_PAD_CONF_1_M  ((EFUSE_SPI_PAD_CONF_1_V)<<(EFUSE_SPI_PAD_CONF_1_S))
#define EFUSE_SPI_PAD_CONF_1_V  0xFFFFFFFF
#define EFUSE_SPI_PAD_CONF_1_S  0

#define EFUSE_RD_MAC_SPI_SYS_3_REG          (DR_REG_EFUSE_BASE + 0x50)
/* EFUSE_SYS_DATA_PART0_0 : RO ;bitpos:[31:18] ;default: 14'h0 ; */
/*description: Stores the fist 14 bits of the zeroth part of system data..*/
#define EFUSE_SYS_DATA_PART0_0    0x00003FFF
#define EFUSE_SYS_DATA_PART0_0_M  ((EFUSE_SYS_DATA_PART0_0_V)<<(EFUSE_SYS_DATA_PART0_0_S))
#define EFUSE_SYS_DATA_PART0_0_V  0x3FFF
#define EFUSE_SYS_DATA_PART0_0_S  18
/* EFUSE_SPI_PAD_CONF_2 : RO ;bitpos:[17:0] ;default: 18'h0 ; */
/*description: Stores the second part of SPI_PAD_CONF..*/
#define EFUSE_SPI_PAD_CONF_2    0x0003FFFF
#define EFUSE_SPI_PAD_CONF_2_M  ((EFUSE_SPI_PAD_CONF_2_V)<<(EFUSE_SPI_PAD_CONF_2_S))
#define EFUSE_SPI_PAD_CONF_2_V  0x3FFFF
#define EFUSE_SPI_PAD_CONF_2_S  0

#define EFUSE_RD_MAC_SPI_SYS_4_REG          (DR_REG_EFUSE_BASE + 0x54)
/* EFUSE_SYS_DATA_PART0_1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fist 32 bits of the zeroth part of system data..*/
#define EFUSE_SYS_DATA_PART0_1    0xFFFFFFFF
#define EFUSE_SYS_DATA_PART0_1_M  ((EFUSE_SYS_DATA_PART0_1_V)<<(EFUSE_SYS_DATA_PART0_1_S))
#define EFUSE_SYS_DATA_PART0_1_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART0_1_S  0

#define EFUSE_RD_MAC_SPI_SYS_5_REG          (DR_REG_EFUSE_BASE + 0x58)
/* EFUSE_SYS_DATA_PART0_2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of the zeroth part of system data..*/
#define EFUSE_SYS_DATA_PART0_2    0xFFFFFFFF
#define EFUSE_SYS_DATA_PART0_2_M  ((EFUSE_SYS_DATA_PART0_2_V)<<(EFUSE_SYS_DATA_PART0_2_S))
#define EFUSE_SYS_DATA_PART0_2_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART0_2_S  0

#define EFUSE_RD_SYS_PART1_DATA0_REG          (DR_REG_EFUSE_BASE + 0x5C)
/* EFUSE_SYS_DATA_PART1_0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the zeroth 32 bits of the first part of system data..*/
#define EFUSE_SYS_DATA_PART1_0    0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_0_M  ((EFUSE_SYS_DATA_PART1_0_V)<<(EFUSE_SYS_DATA_PART1_0_S))
#define EFUSE_SYS_DATA_PART1_0_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_0_S  0

#define EFUSE_RD_SYS_PART1_DATA1_REG          (DR_REG_EFUSE_BASE + 0x60)
/* EFUSE_SYS_DATA_PART1_1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first 32 bits of the first part of system data..*/
#define EFUSE_SYS_DATA_PART1_1    0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_1_M  ((EFUSE_SYS_DATA_PART1_1_V)<<(EFUSE_SYS_DATA_PART1_1_S))
#define EFUSE_SYS_DATA_PART1_1_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_1_S  0

#define EFUSE_RD_SYS_PART1_DATA2_REG          (DR_REG_EFUSE_BASE + 0x64)
/* EFUSE_SYS_DATA_PART1_2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of the first part of system data..*/
#define EFUSE_SYS_DATA_PART1_2    0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_2_M  ((EFUSE_SYS_DATA_PART1_2_V)<<(EFUSE_SYS_DATA_PART1_2_S))
#define EFUSE_SYS_DATA_PART1_2_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_2_S  0

#define EFUSE_RD_SYS_PART1_DATA3_REG          (DR_REG_EFUSE_BASE + 0x68)
/* EFUSE_SYS_DATA_PART1_3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the third 32 bits of the first part of system data..*/
#define EFUSE_SYS_DATA_PART1_3    0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_3_M  ((EFUSE_SYS_DATA_PART1_3_V)<<(EFUSE_SYS_DATA_PART1_3_S))
#define EFUSE_SYS_DATA_PART1_3_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_3_S  0

#define EFUSE_RD_SYS_PART1_DATA4_REG          (DR_REG_EFUSE_BASE + 0x6C)
/* EFUSE_SYS_DATA_PART1_4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fourth 32 bits of the first part of system data..*/
#define EFUSE_SYS_DATA_PART1_4    0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_4_M  ((EFUSE_SYS_DATA_PART1_4_V)<<(EFUSE_SYS_DATA_PART1_4_S))
#define EFUSE_SYS_DATA_PART1_4_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_4_S  0

#define EFUSE_RD_SYS_PART1_DATA5_REG          (DR_REG_EFUSE_BASE + 0x70)
/* EFUSE_SYS_DATA_PART1_5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fifth 32 bits of the first part of system data..*/
#define EFUSE_SYS_DATA_PART1_5    0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_5_M  ((EFUSE_SYS_DATA_PART1_5_V)<<(EFUSE_SYS_DATA_PART1_5_S))
#define EFUSE_SYS_DATA_PART1_5_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_5_S  0

#define EFUSE_RD_SYS_PART1_DATA6_REG          (DR_REG_EFUSE_BASE + 0x74)
/* EFUSE_SYS_DATA_PART1_6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the sixth 32 bits of the first part of system data..*/
#define EFUSE_SYS_DATA_PART1_6    0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_6_M  ((EFUSE_SYS_DATA_PART1_6_V)<<(EFUSE_SYS_DATA_PART1_6_S))
#define EFUSE_SYS_DATA_PART1_6_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_6_S  0

#define EFUSE_RD_SYS_PART1_DATA7_REG          (DR_REG_EFUSE_BASE + 0x78)
/* EFUSE_SYS_DATA_PART1_7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the seventh 32 bits of the first part of system data..*/
#define EFUSE_SYS_DATA_PART1_7    0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_7_M  ((EFUSE_SYS_DATA_PART1_7_V)<<(EFUSE_SYS_DATA_PART1_7_S))
#define EFUSE_SYS_DATA_PART1_7_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_7_S  0

#define EFUSE_RD_USR_DATA0_REG          (DR_REG_EFUSE_BASE + 0x7C)
/* EFUSE_USR_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the zeroth 32 bits of BLOCK3 (user)..*/
#define EFUSE_USR_DATA0    0xFFFFFFFF
#define EFUSE_USR_DATA0_M  ((EFUSE_USR_DATA0_V)<<(EFUSE_USR_DATA0_S))
#define EFUSE_USR_DATA0_V  0xFFFFFFFF
#define EFUSE_USR_DATA0_S  0

#define EFUSE_RD_USR_DATA1_REG          (DR_REG_EFUSE_BASE + 0x80)
/* EFUSE_USR_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first 32 bits of BLOCK3 (user)..*/
#define EFUSE_USR_DATA1    0xFFFFFFFF
#define EFUSE_USR_DATA1_M  ((EFUSE_USR_DATA1_V)<<(EFUSE_USR_DATA1_S))
#define EFUSE_USR_DATA1_V  0xFFFFFFFF
#define EFUSE_USR_DATA1_S  0

#define EFUSE_RD_USR_DATA2_REG          (DR_REG_EFUSE_BASE + 0x84)
/* EFUSE_USR_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of BLOCK3 (user)..*/
#define EFUSE_USR_DATA2    0xFFFFFFFF
#define EFUSE_USR_DATA2_M  ((EFUSE_USR_DATA2_V)<<(EFUSE_USR_DATA2_S))
#define EFUSE_USR_DATA2_V  0xFFFFFFFF
#define EFUSE_USR_DATA2_S  0

#define EFUSE_RD_USR_DATA3_REG          (DR_REG_EFUSE_BASE + 0x88)
/* EFUSE_USR_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the third 32 bits of BLOCK3 (user)..*/
#define EFUSE_USR_DATA3    0xFFFFFFFF
#define EFUSE_USR_DATA3_M  ((EFUSE_USR_DATA3_V)<<(EFUSE_USR_DATA3_S))
#define EFUSE_USR_DATA3_V  0xFFFFFFFF
#define EFUSE_USR_DATA3_S  0

#define EFUSE_RD_USR_DATA4_REG          (DR_REG_EFUSE_BASE + 0x8C)
/* EFUSE_USR_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fourth 32 bits of BLOCK3 (user)..*/
#define EFUSE_USR_DATA4    0xFFFFFFFF
#define EFUSE_USR_DATA4_M  ((EFUSE_USR_DATA4_V)<<(EFUSE_USR_DATA4_S))
#define EFUSE_USR_DATA4_V  0xFFFFFFFF
#define EFUSE_USR_DATA4_S  0

#define EFUSE_RD_USR_DATA5_REG          (DR_REG_EFUSE_BASE + 0x90)
/* EFUSE_USR_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fifth 32 bits of BLOCK3 (user)..*/
#define EFUSE_USR_DATA5    0xFFFFFFFF
#define EFUSE_USR_DATA5_M  ((EFUSE_USR_DATA5_V)<<(EFUSE_USR_DATA5_S))
#define EFUSE_USR_DATA5_V  0xFFFFFFFF
#define EFUSE_USR_DATA5_S  0

#define EFUSE_RD_USR_DATA6_REG          (DR_REG_EFUSE_BASE + 0x94)
/* EFUSE_USR_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the sixth 32 bits of BLOCK3 (user)..*/
#define EFUSE_USR_DATA6    0xFFFFFFFF
#define EFUSE_USR_DATA6_M  ((EFUSE_USR_DATA6_V)<<(EFUSE_USR_DATA6_S))
#define EFUSE_USR_DATA6_V  0xFFFFFFFF
#define EFUSE_USR_DATA6_S  0

#define EFUSE_RD_USR_DATA7_REG          (DR_REG_EFUSE_BASE + 0x98)
/* EFUSE_USR_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the seventh 32 bits of BLOCK3 (user)..*/
#define EFUSE_USR_DATA7    0xFFFFFFFF
#define EFUSE_USR_DATA7_M  ((EFUSE_USR_DATA7_V)<<(EFUSE_USR_DATA7_S))
#define EFUSE_USR_DATA7_V  0xFFFFFFFF
#define EFUSE_USR_DATA7_S  0

#define EFUSE_RD_KEY0_DATA0_REG          (DR_REG_EFUSE_BASE + 0x9C)
/* EFUSE_KEY0_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the zeroth 32 bits of KEY0..*/
#define EFUSE_KEY0_DATA0    0xFFFFFFFF
#define EFUSE_KEY0_DATA0_M  ((EFUSE_KEY0_DATA0_V)<<(EFUSE_KEY0_DATA0_S))
#define EFUSE_KEY0_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA0_S  0

#define EFUSE_RD_KEY0_DATA1_REG          (DR_REG_EFUSE_BASE + 0xA0)
/* EFUSE_KEY0_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first 32 bits of KEY0..*/
#define EFUSE_KEY0_DATA1    0xFFFFFFFF
#define EFUSE_KEY0_DATA1_M  ((EFUSE_KEY0_DATA1_V)<<(EFUSE_KEY0_DATA1_S))
#define EFUSE_KEY0_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA1_S  0

#define EFUSE_RD_KEY0_DATA2_REG          (DR_REG_EFUSE_BASE + 0xA4)
/* EFUSE_KEY0_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of KEY0..*/
#define EFUSE_KEY0_DATA2    0xFFFFFFFF
#define EFUSE_KEY0_DATA2_M  ((EFUSE_KEY0_DATA2_V)<<(EFUSE_KEY0_DATA2_S))
#define EFUSE_KEY0_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA2_S  0

#define EFUSE_RD_KEY0_DATA3_REG          (DR_REG_EFUSE_BASE + 0xA8)
/* EFUSE_KEY0_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the third 32 bits of KEY0..*/
#define EFUSE_KEY0_DATA3    0xFFFFFFFF
#define EFUSE_KEY0_DATA3_M  ((EFUSE_KEY0_DATA3_V)<<(EFUSE_KEY0_DATA3_S))
#define EFUSE_KEY0_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA3_S  0

#define EFUSE_RD_KEY0_DATA4_REG          (DR_REG_EFUSE_BASE + 0xAC)
/* EFUSE_KEY0_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fourth 32 bits of KEY0..*/
#define EFUSE_KEY0_DATA4    0xFFFFFFFF
#define EFUSE_KEY0_DATA4_M  ((EFUSE_KEY0_DATA4_V)<<(EFUSE_KEY0_DATA4_S))
#define EFUSE_KEY0_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA4_S  0

#define EFUSE_RD_KEY0_DATA5_REG          (DR_REG_EFUSE_BASE + 0xB0)
/* EFUSE_KEY0_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fifth 32 bits of KEY0..*/
#define EFUSE_KEY0_DATA5    0xFFFFFFFF
#define EFUSE_KEY0_DATA5_M  ((EFUSE_KEY0_DATA5_V)<<(EFUSE_KEY0_DATA5_S))
#define EFUSE_KEY0_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA5_S  0

#define EFUSE_RD_KEY0_DATA6_REG          (DR_REG_EFUSE_BASE + 0xB4)
/* EFUSE_KEY0_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the sixth 32 bits of KEY0..*/
#define EFUSE_KEY0_DATA6    0xFFFFFFFF
#define EFUSE_KEY0_DATA6_M  ((EFUSE_KEY0_DATA6_V)<<(EFUSE_KEY0_DATA6_S))
#define EFUSE_KEY0_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA6_S  0

#define EFUSE_RD_KEY0_DATA7_REG          (DR_REG_EFUSE_BASE + 0xB8)
/* EFUSE_KEY0_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the seventh 32 bits of KEY0..*/
#define EFUSE_KEY0_DATA7    0xFFFFFFFF
#define EFUSE_KEY0_DATA7_M  ((EFUSE_KEY0_DATA7_V)<<(EFUSE_KEY0_DATA7_S))
#define EFUSE_KEY0_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA7_S  0

#define EFUSE_RD_KEY1_DATA0_REG          (DR_REG_EFUSE_BASE + 0xBC)
/* EFUSE_KEY1_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the zeroth 32 bits of KEY1..*/
#define EFUSE_KEY1_DATA0    0xFFFFFFFF
#define EFUSE_KEY1_DATA0_M  ((EFUSE_KEY1_DATA0_V)<<(EFUSE_KEY1_DATA0_S))
#define EFUSE_KEY1_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA0_S  0

#define EFUSE_RD_KEY1_DATA1_REG          (DR_REG_EFUSE_BASE + 0xC0)
/* EFUSE_KEY1_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first 32 bits of KEY1..*/
#define EFUSE_KEY1_DATA1    0xFFFFFFFF
#define EFUSE_KEY1_DATA1_M  ((EFUSE_KEY1_DATA1_V)<<(EFUSE_KEY1_DATA1_S))
#define EFUSE_KEY1_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA1_S  0

#define EFUSE_RD_KEY1_DATA2_REG          (DR_REG_EFUSE_BASE + 0xC4)
/* EFUSE_KEY1_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of KEY1..*/
#define EFUSE_KEY1_DATA2    0xFFFFFFFF
#define EFUSE_KEY1_DATA2_M  ((EFUSE_KEY1_DATA2_V)<<(EFUSE_KEY1_DATA2_S))
#define EFUSE_KEY1_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA2_S  0

#define EFUSE_RD_KEY1_DATA3_REG          (DR_REG_EFUSE_BASE + 0xC8)
/* EFUSE_KEY1_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the third 32 bits of KEY1..*/
#define EFUSE_KEY1_DATA3    0xFFFFFFFF
#define EFUSE_KEY1_DATA3_M  ((EFUSE_KEY1_DATA3_V)<<(EFUSE_KEY1_DATA3_S))
#define EFUSE_KEY1_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA3_S  0

#define EFUSE_RD_KEY1_DATA4_REG          (DR_REG_EFUSE_BASE + 0xCC)
/* EFUSE_KEY1_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fourth 32 bits of KEY1..*/
#define EFUSE_KEY1_DATA4    0xFFFFFFFF
#define EFUSE_KEY1_DATA4_M  ((EFUSE_KEY1_DATA4_V)<<(EFUSE_KEY1_DATA4_S))
#define EFUSE_KEY1_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA4_S  0

#define EFUSE_RD_KEY1_DATA5_REG          (DR_REG_EFUSE_BASE + 0xD0)
/* EFUSE_KEY1_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fifth 32 bits of KEY1..*/
#define EFUSE_KEY1_DATA5    0xFFFFFFFF
#define EFUSE_KEY1_DATA5_M  ((EFUSE_KEY1_DATA5_V)<<(EFUSE_KEY1_DATA5_S))
#define EFUSE_KEY1_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA5_S  0

#define EFUSE_RD_KEY1_DATA6_REG          (DR_REG_EFUSE_BASE + 0xD4)
/* EFUSE_KEY1_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the sixth 32 bits of KEY1..*/
#define EFUSE_KEY1_DATA6    0xFFFFFFFF
#define EFUSE_KEY1_DATA6_M  ((EFUSE_KEY1_DATA6_V)<<(EFUSE_KEY1_DATA6_S))
#define EFUSE_KEY1_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA6_S  0

#define EFUSE_RD_KEY1_DATA7_REG          (DR_REG_EFUSE_BASE + 0xD8)
/* EFUSE_KEY1_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the seventh 32 bits of KEY1..*/
#define EFUSE_KEY1_DATA7    0xFFFFFFFF
#define EFUSE_KEY1_DATA7_M  ((EFUSE_KEY1_DATA7_V)<<(EFUSE_KEY1_DATA7_S))
#define EFUSE_KEY1_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA7_S  0

#define EFUSE_RD_KEY2_DATA0_REG          (DR_REG_EFUSE_BASE + 0xDC)
/* EFUSE_KEY2_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the zeroth 32 bits of KEY2..*/
#define EFUSE_KEY2_DATA0    0xFFFFFFFF
#define EFUSE_KEY2_DATA0_M  ((EFUSE_KEY2_DATA0_V)<<(EFUSE_KEY2_DATA0_S))
#define EFUSE_KEY2_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA0_S  0

#define EFUSE_RD_KEY2_DATA1_REG          (DR_REG_EFUSE_BASE + 0xE0)
/* EFUSE_KEY2_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first 32 bits of KEY2..*/
#define EFUSE_KEY2_DATA1    0xFFFFFFFF
#define EFUSE_KEY2_DATA1_M  ((EFUSE_KEY2_DATA1_V)<<(EFUSE_KEY2_DATA1_S))
#define EFUSE_KEY2_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA1_S  0

#define EFUSE_RD_KEY2_DATA2_REG          (DR_REG_EFUSE_BASE + 0xE4)
/* EFUSE_KEY2_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of KEY2..*/
#define EFUSE_KEY2_DATA2    0xFFFFFFFF
#define EFUSE_KEY2_DATA2_M  ((EFUSE_KEY2_DATA2_V)<<(EFUSE_KEY2_DATA2_S))
#define EFUSE_KEY2_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA2_S  0

#define EFUSE_RD_KEY2_DATA3_REG          (DR_REG_EFUSE_BASE + 0xE8)
/* EFUSE_KEY2_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the third 32 bits of KEY2..*/
#define EFUSE_KEY2_DATA3    0xFFFFFFFF
#define EFUSE_KEY2_DATA3_M  ((EFUSE_KEY2_DATA3_V)<<(EFUSE_KEY2_DATA3_S))
#define EFUSE_KEY2_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA3_S  0

#define EFUSE_RD_KEY2_DATA4_REG          (DR_REG_EFUSE_BASE + 0xEC)
/* EFUSE_KEY2_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fourth 32 bits of KEY2..*/
#define EFUSE_KEY2_DATA4    0xFFFFFFFF
#define EFUSE_KEY2_DATA4_M  ((EFUSE_KEY2_DATA4_V)<<(EFUSE_KEY2_DATA4_S))
#define EFUSE_KEY2_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA4_S  0

#define EFUSE_RD_KEY2_DATA5_REG          (DR_REG_EFUSE_BASE + 0xF0)
/* EFUSE_KEY2_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fifth 32 bits of KEY2..*/
#define EFUSE_KEY2_DATA5    0xFFFFFFFF
#define EFUSE_KEY2_DATA5_M  ((EFUSE_KEY2_DATA5_V)<<(EFUSE_KEY2_DATA5_S))
#define EFUSE_KEY2_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA5_S  0

#define EFUSE_RD_KEY2_DATA6_REG          (DR_REG_EFUSE_BASE + 0xF4)
/* EFUSE_KEY2_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the sixth 32 bits of KEY2..*/
#define EFUSE_KEY2_DATA6    0xFFFFFFFF
#define EFUSE_KEY2_DATA6_M  ((EFUSE_KEY2_DATA6_V)<<(EFUSE_KEY2_DATA6_S))
#define EFUSE_KEY2_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA6_S  0

#define EFUSE_RD_KEY2_DATA7_REG          (DR_REG_EFUSE_BASE + 0xF8)
/* EFUSE_KEY2_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the seventh 32 bits of KEY2..*/
#define EFUSE_KEY2_DATA7    0xFFFFFFFF
#define EFUSE_KEY2_DATA7_M  ((EFUSE_KEY2_DATA7_V)<<(EFUSE_KEY2_DATA7_S))
#define EFUSE_KEY2_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA7_S  0

#define EFUSE_RD_KEY3_DATA0_REG          (DR_REG_EFUSE_BASE + 0xFC)
/* EFUSE_KEY3_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the zeroth 32 bits of KEY3..*/
#define EFUSE_KEY3_DATA0    0xFFFFFFFF
#define EFUSE_KEY3_DATA0_M  ((EFUSE_KEY3_DATA0_V)<<(EFUSE_KEY3_DATA0_S))
#define EFUSE_KEY3_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA0_S  0

#define EFUSE_RD_KEY3_DATA1_REG          (DR_REG_EFUSE_BASE + 0x100)
/* EFUSE_KEY3_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first 32 bits of KEY3..*/
#define EFUSE_KEY3_DATA1    0xFFFFFFFF
#define EFUSE_KEY3_DATA1_M  ((EFUSE_KEY3_DATA1_V)<<(EFUSE_KEY3_DATA1_S))
#define EFUSE_KEY3_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA1_S  0

#define EFUSE_RD_KEY3_DATA2_REG          (DR_REG_EFUSE_BASE + 0x104)
/* EFUSE_KEY3_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of KEY3..*/
#define EFUSE_KEY3_DATA2    0xFFFFFFFF
#define EFUSE_KEY3_DATA2_M  ((EFUSE_KEY3_DATA2_V)<<(EFUSE_KEY3_DATA2_S))
#define EFUSE_KEY3_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA2_S  0

#define EFUSE_RD_KEY3_DATA3_REG          (DR_REG_EFUSE_BASE + 0x108)
/* EFUSE_KEY3_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the third 32 bits of KEY3..*/
#define EFUSE_KEY3_DATA3    0xFFFFFFFF
#define EFUSE_KEY3_DATA3_M  ((EFUSE_KEY3_DATA3_V)<<(EFUSE_KEY3_DATA3_S))
#define EFUSE_KEY3_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA3_S  0

#define EFUSE_RD_KEY3_DATA4_REG          (DR_REG_EFUSE_BASE + 0x10C)
/* EFUSE_KEY3_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fourth 32 bits of KEY3..*/
#define EFUSE_KEY3_DATA4    0xFFFFFFFF
#define EFUSE_KEY3_DATA4_M  ((EFUSE_KEY3_DATA4_V)<<(EFUSE_KEY3_DATA4_S))
#define EFUSE_KEY3_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA4_S  0

#define EFUSE_RD_KEY3_DATA5_REG          (DR_REG_EFUSE_BASE + 0x110)
/* EFUSE_KEY3_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fifth 32 bits of KEY3..*/
#define EFUSE_KEY3_DATA5    0xFFFFFFFF
#define EFUSE_KEY3_DATA5_M  ((EFUSE_KEY3_DATA5_V)<<(EFUSE_KEY3_DATA5_S))
#define EFUSE_KEY3_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA5_S  0

#define EFUSE_RD_KEY3_DATA6_REG          (DR_REG_EFUSE_BASE + 0x114)
/* EFUSE_KEY3_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the sixth 32 bits of KEY3..*/
#define EFUSE_KEY3_DATA6    0xFFFFFFFF
#define EFUSE_KEY3_DATA6_M  ((EFUSE_KEY3_DATA6_V)<<(EFUSE_KEY3_DATA6_S))
#define EFUSE_KEY3_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA6_S  0

#define EFUSE_RD_KEY3_DATA7_REG          (DR_REG_EFUSE_BASE + 0x118)
/* EFUSE_KEY3_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the seventh 32 bits of KEY3..*/
#define EFUSE_KEY3_DATA7    0xFFFFFFFF
#define EFUSE_KEY3_DATA7_M  ((EFUSE_KEY3_DATA7_V)<<(EFUSE_KEY3_DATA7_S))
#define EFUSE_KEY3_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA7_S  0

#define EFUSE_RD_KEY4_DATA0_REG          (DR_REG_EFUSE_BASE + 0x11C)
/* EFUSE_KEY4_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the zeroth 32 bits of KEY4..*/
#define EFUSE_KEY4_DATA0    0xFFFFFFFF
#define EFUSE_KEY4_DATA0_M  ((EFUSE_KEY4_DATA0_V)<<(EFUSE_KEY4_DATA0_S))
#define EFUSE_KEY4_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA0_S  0

#define EFUSE_RD_KEY4_DATA1_REG          (DR_REG_EFUSE_BASE + 0x120)
/* EFUSE_KEY4_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first 32 bits of KEY4..*/
#define EFUSE_KEY4_DATA1    0xFFFFFFFF
#define EFUSE_KEY4_DATA1_M  ((EFUSE_KEY4_DATA1_V)<<(EFUSE_KEY4_DATA1_S))
#define EFUSE_KEY4_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA1_S  0

#define EFUSE_RD_KEY4_DATA2_REG          (DR_REG_EFUSE_BASE + 0x124)
/* EFUSE_KEY4_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of KEY4..*/
#define EFUSE_KEY4_DATA2    0xFFFFFFFF
#define EFUSE_KEY4_DATA2_M  ((EFUSE_KEY4_DATA2_V)<<(EFUSE_KEY4_DATA2_S))
#define EFUSE_KEY4_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA2_S  0

#define EFUSE_RD_KEY4_DATA3_REG          (DR_REG_EFUSE_BASE + 0x128)
/* EFUSE_KEY4_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the third 32 bits of KEY4..*/
#define EFUSE_KEY4_DATA3    0xFFFFFFFF
#define EFUSE_KEY4_DATA3_M  ((EFUSE_KEY4_DATA3_V)<<(EFUSE_KEY4_DATA3_S))
#define EFUSE_KEY4_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA3_S  0

#define EFUSE_RD_KEY4_DATA4_REG          (DR_REG_EFUSE_BASE + 0x12C)
/* EFUSE_KEY4_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fourth 32 bits of KEY4..*/
#define EFUSE_KEY4_DATA4    0xFFFFFFFF
#define EFUSE_KEY4_DATA4_M  ((EFUSE_KEY4_DATA4_V)<<(EFUSE_KEY4_DATA4_S))
#define EFUSE_KEY4_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA4_S  0

#define EFUSE_RD_KEY4_DATA5_REG          (DR_REG_EFUSE_BASE + 0x130)
/* EFUSE_KEY4_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fifth 32 bits of KEY4..*/
#define EFUSE_KEY4_DATA5    0xFFFFFFFF
#define EFUSE_KEY4_DATA5_M  ((EFUSE_KEY4_DATA5_V)<<(EFUSE_KEY4_DATA5_S))
#define EFUSE_KEY4_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA5_S  0

#define EFUSE_RD_KEY4_DATA6_REG          (DR_REG_EFUSE_BASE + 0x134)
/* EFUSE_KEY4_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the sixth 32 bits of KEY4..*/
#define EFUSE_KEY4_DATA6    0xFFFFFFFF
#define EFUSE_KEY4_DATA6_M  ((EFUSE_KEY4_DATA6_V)<<(EFUSE_KEY4_DATA6_S))
#define EFUSE_KEY4_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA6_S  0

#define EFUSE_RD_KEY4_DATA7_REG          (DR_REG_EFUSE_BASE + 0x138)
/* EFUSE_KEY4_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the seventh 32 bits of KEY4..*/
#define EFUSE_KEY4_DATA7    0xFFFFFFFF
#define EFUSE_KEY4_DATA7_M  ((EFUSE_KEY4_DATA7_V)<<(EFUSE_KEY4_DATA7_S))
#define EFUSE_KEY4_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA7_S  0

#define EFUSE_RD_KEY5_DATA0_REG          (DR_REG_EFUSE_BASE + 0x13C)
/* EFUSE_KEY5_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the zeroth 32 bits of KEY5..*/
#define EFUSE_KEY5_DATA0    0xFFFFFFFF
#define EFUSE_KEY5_DATA0_M  ((EFUSE_KEY5_DATA0_V)<<(EFUSE_KEY5_DATA0_S))
#define EFUSE_KEY5_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA0_S  0

#define EFUSE_RD_KEY5_DATA1_REG          (DR_REG_EFUSE_BASE + 0x140)
/* EFUSE_KEY5_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first 32 bits of KEY5..*/
#define EFUSE_KEY5_DATA1    0xFFFFFFFF
#define EFUSE_KEY5_DATA1_M  ((EFUSE_KEY5_DATA1_V)<<(EFUSE_KEY5_DATA1_S))
#define EFUSE_KEY5_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA1_S  0

#define EFUSE_RD_KEY5_DATA2_REG          (DR_REG_EFUSE_BASE + 0x144)
/* EFUSE_KEY5_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of KEY5..*/
#define EFUSE_KEY5_DATA2    0xFFFFFFFF
#define EFUSE_KEY5_DATA2_M  ((EFUSE_KEY5_DATA2_V)<<(EFUSE_KEY5_DATA2_S))
#define EFUSE_KEY5_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA2_S  0

#define EFUSE_RD_KEY5_DATA3_REG          (DR_REG_EFUSE_BASE + 0x148)
/* EFUSE_KEY5_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the third 32 bits of KEY5..*/
#define EFUSE_KEY5_DATA3    0xFFFFFFFF
#define EFUSE_KEY5_DATA3_M  ((EFUSE_KEY5_DATA3_V)<<(EFUSE_KEY5_DATA3_S))
#define EFUSE_KEY5_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA3_S  0

#define EFUSE_RD_KEY5_DATA4_REG          (DR_REG_EFUSE_BASE + 0x14C)
/* EFUSE_KEY5_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fourth 32 bits of KEY5..*/
#define EFUSE_KEY5_DATA4    0xFFFFFFFF
#define EFUSE_KEY5_DATA4_M  ((EFUSE_KEY5_DATA4_V)<<(EFUSE_KEY5_DATA4_S))
#define EFUSE_KEY5_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA4_S  0

#define EFUSE_RD_KEY5_DATA5_REG          (DR_REG_EFUSE_BASE + 0x150)
/* EFUSE_KEY5_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fifth 32 bits of KEY5..*/
#define EFUSE_KEY5_DATA5    0xFFFFFFFF
#define EFUSE_KEY5_DATA5_M  ((EFUSE_KEY5_DATA5_V)<<(EFUSE_KEY5_DATA5_S))
#define EFUSE_KEY5_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA5_S  0

#define EFUSE_RD_KEY5_DATA6_REG          (DR_REG_EFUSE_BASE + 0x154)
/* EFUSE_KEY5_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the sixth 32 bits of KEY5..*/
#define EFUSE_KEY5_DATA6    0xFFFFFFFF
#define EFUSE_KEY5_DATA6_M  ((EFUSE_KEY5_DATA6_V)<<(EFUSE_KEY5_DATA6_S))
#define EFUSE_KEY5_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA6_S  0

#define EFUSE_RD_KEY5_DATA7_REG          (DR_REG_EFUSE_BASE + 0x158)
/* EFUSE_KEY5_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the seventh 32 bits of KEY5..*/
#define EFUSE_KEY5_DATA7    0xFFFFFFFF
#define EFUSE_KEY5_DATA7_M  ((EFUSE_KEY5_DATA7_V)<<(EFUSE_KEY5_DATA7_S))
#define EFUSE_KEY5_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA7_S  0

#define EFUSE_RD_SYS_PART2_DATA0_REG          (DR_REG_EFUSE_BASE + 0x15C)
/* EFUSE_SYS_DATA_PART2_0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the 0th 32 bits of the 2nd part of system data..*/
#define EFUSE_SYS_DATA_PART2_0    0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_0_M  ((EFUSE_SYS_DATA_PART2_0_V)<<(EFUSE_SYS_DATA_PART2_0_S))
#define EFUSE_SYS_DATA_PART2_0_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_0_S  0

#define EFUSE_RD_SYS_PART2_DATA1_REG          (DR_REG_EFUSE_BASE + 0x160)
/* EFUSE_SYS_DATA_PART2_1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the 1st 32 bits of the 2nd part of system data..*/
#define EFUSE_SYS_DATA_PART2_1    0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_1_M  ((EFUSE_SYS_DATA_PART2_1_V)<<(EFUSE_SYS_DATA_PART2_1_S))
#define EFUSE_SYS_DATA_PART2_1_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_1_S  0

#define EFUSE_RD_SYS_PART2_DATA2_REG          (DR_REG_EFUSE_BASE + 0x164)
/* EFUSE_SYS_DATA_PART2_2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the 2nd 32 bits of the 2nd part of system data..*/
#define EFUSE_SYS_DATA_PART2_2    0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_2_M  ((EFUSE_SYS_DATA_PART2_2_V)<<(EFUSE_SYS_DATA_PART2_2_S))
#define EFUSE_SYS_DATA_PART2_2_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_2_S  0

#define EFUSE_RD_SYS_PART2_DATA3_REG          (DR_REG_EFUSE_BASE + 0x168)
/* EFUSE_SYS_DATA_PART2_3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the 3rd 32 bits of the 2nd part of system data..*/
#define EFUSE_SYS_DATA_PART2_3    0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_3_M  ((EFUSE_SYS_DATA_PART2_3_V)<<(EFUSE_SYS_DATA_PART2_3_S))
#define EFUSE_SYS_DATA_PART2_3_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_3_S  0

#define EFUSE_RD_SYS_PART2_DATA4_REG          (DR_REG_EFUSE_BASE + 0x16C)
/* EFUSE_SYS_DATA_PART2_4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the 4th 32 bits of the 2nd part of system data..*/
#define EFUSE_SYS_DATA_PART2_4    0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_4_M  ((EFUSE_SYS_DATA_PART2_4_V)<<(EFUSE_SYS_DATA_PART2_4_S))
#define EFUSE_SYS_DATA_PART2_4_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_4_S  0

#define EFUSE_RD_SYS_PART2_DATA5_REG          (DR_REG_EFUSE_BASE + 0x170)
/* EFUSE_SYS_DATA_PART2_5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the 5th 32 bits of the 2nd part of system data..*/
#define EFUSE_SYS_DATA_PART2_5    0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_5_M  ((EFUSE_SYS_DATA_PART2_5_V)<<(EFUSE_SYS_DATA_PART2_5_S))
#define EFUSE_SYS_DATA_PART2_5_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_5_S  0

#define EFUSE_RD_SYS_PART2_DATA6_REG          (DR_REG_EFUSE_BASE + 0x174)
/* EFUSE_SYS_DATA_PART2_6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the 6th 32 bits of the 2nd part of system data..*/
#define EFUSE_SYS_DATA_PART2_6    0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_6_M  ((EFUSE_SYS_DATA_PART2_6_V)<<(EFUSE_SYS_DATA_PART2_6_S))
#define EFUSE_SYS_DATA_PART2_6_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_6_S  0

#define EFUSE_RD_SYS_PART2_DATA7_REG          (DR_REG_EFUSE_BASE + 0x178)
/* EFUSE_SYS_DATA_PART2_7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the 7th 32 bits of the 2nd part of system data..*/
#define EFUSE_SYS_DATA_PART2_7    0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_7_M  ((EFUSE_SYS_DATA_PART2_7_V)<<(EFUSE_SYS_DATA_PART2_7_S))
#define EFUSE_SYS_DATA_PART2_7_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_7_S  0

#define EFUSE_RD_REPEAT_ERR0_REG          (DR_REG_EFUSE_BASE + 0x17C)
/* EFUSE_VDD_SPI_DREFH_ERR : RO ;bitpos:[31:30] ;default: 2'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_VDD_SPI_DREFH_ERR    0x00000003
#define EFUSE_VDD_SPI_DREFH_ERR_M  ((EFUSE_VDD_SPI_DREFH_ERR_V)<<(EFUSE_VDD_SPI_DREFH_ERR_S))
#define EFUSE_VDD_SPI_DREFH_ERR_V  0x3
#define EFUSE_VDD_SPI_DREFH_ERR_S  30
/* EFUSE_VDD_SPI_MODECURLIM_ERR : RO ;bitpos:[29] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_VDD_SPI_MODECURLIM_ERR    (BIT(29))
#define EFUSE_VDD_SPI_MODECURLIM_ERR_M  (BIT(29))
#define EFUSE_VDD_SPI_MODECURLIM_ERR_V  0x1
#define EFUSE_VDD_SPI_MODECURLIM_ERR_S  29
/* EFUSE_BTLC_GPIO_ENABLE_ERR : RO ;bitpos:[28:27] ;default: 2'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_BTLC_GPIO_ENABLE_ERR    0x00000003
#define EFUSE_BTLC_GPIO_ENABLE_ERR_M  ((EFUSE_BTLC_GPIO_ENABLE_ERR_V)<<(EFUSE_BTLC_GPIO_ENABLE_ERR_S))
#define EFUSE_BTLC_GPIO_ENABLE_ERR_V  0x3
#define EFUSE_BTLC_GPIO_ENABLE_ERR_S  27
/* EFUSE_EXT_PHY_ENABLE_ERR : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_EXT_PHY_ENABLE_ERR    (BIT(26))
#define EFUSE_EXT_PHY_ENABLE_ERR_M  (BIT(26))
#define EFUSE_EXT_PHY_ENABLE_ERR_V  0x1
#define EFUSE_EXT_PHY_ENABLE_ERR_S  26
/* EFUSE_USB_EXCHG_PINS_ERR : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_USB_EXCHG_PINS_ERR    (BIT(25))
#define EFUSE_USB_EXCHG_PINS_ERR_M  (BIT(25))
#define EFUSE_USB_EXCHG_PINS_ERR_V  0x1
#define EFUSE_USB_EXCHG_PINS_ERR_S  25
/* EFUSE_USB_DREFL_ERR : RO ;bitpos:[24:23] ;default: 2'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_USB_DREFL_ERR    0x00000003
#define EFUSE_USB_DREFL_ERR_M  ((EFUSE_USB_DREFL_ERR_V)<<(EFUSE_USB_DREFL_ERR_S))
#define EFUSE_USB_DREFL_ERR_V  0x3
#define EFUSE_USB_DREFL_ERR_S  23
/* EFUSE_USB_DREFH_ERR : RO ;bitpos:[22:21] ;default: 2'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_USB_DREFH_ERR    0x00000003
#define EFUSE_USB_DREFH_ERR_M  ((EFUSE_USB_DREFH_ERR_V)<<(EFUSE_USB_DREFH_ERR_S))
#define EFUSE_USB_DREFH_ERR_V  0x3
#define EFUSE_USB_DREFH_ERR_S  21
/* EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR    (BIT(20))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_M  (BIT(20))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_V  0x1
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_S  20
/* EFUSE_DIS_PAD_JTAG_ERR : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_DIS_PAD_JTAG_ERR    (BIT(19))
#define EFUSE_DIS_PAD_JTAG_ERR_M  (BIT(19))
#define EFUSE_DIS_PAD_JTAG_ERR_V  0x1
#define EFUSE_DIS_PAD_JTAG_ERR_S  19
/* EFUSE_SOFT_DIS_JTAG_ERR : RO ;bitpos:[18:16] ;default: 3'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_SOFT_DIS_JTAG_ERR    0x00000007
#define EFUSE_SOFT_DIS_JTAG_ERR_M  ((EFUSE_SOFT_DIS_JTAG_ERR_V)<<(EFUSE_SOFT_DIS_JTAG_ERR_S))
#define EFUSE_SOFT_DIS_JTAG_ERR_V  0x7
#define EFUSE_SOFT_DIS_JTAG_ERR_S  16
/* EFUSE_DIS_APP_CPU_ERR : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_DIS_APP_CPU_ERR    (BIT(15))
#define EFUSE_DIS_APP_CPU_ERR_M  (BIT(15))
#define EFUSE_DIS_APP_CPU_ERR_V  0x1
#define EFUSE_DIS_APP_CPU_ERR_S  15
/* EFUSE_DIS_CAN_ERR : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_DIS_CAN_ERR    (BIT(14))
#define EFUSE_DIS_CAN_ERR_M  (BIT(14))
#define EFUSE_DIS_CAN_ERR_V  0x1
#define EFUSE_DIS_CAN_ERR_S  14
/* EFUSE_DIS_USB_ERR : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_DIS_USB_ERR    (BIT(13))
#define EFUSE_DIS_USB_ERR_M  (BIT(13))
#define EFUSE_DIS_USB_ERR_V  0x1
#define EFUSE_DIS_USB_ERR_S  13
/* EFUSE_DIS_FORCE_DOWNLOAD_ERR : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_DIS_FORCE_DOWNLOAD_ERR    (BIT(12))
#define EFUSE_DIS_FORCE_DOWNLOAD_ERR_M  (BIT(12))
#define EFUSE_DIS_FORCE_DOWNLOAD_ERR_V  0x1
#define EFUSE_DIS_FORCE_DOWNLOAD_ERR_S  12
/* EFUSE_DIS_DOWNLOAD_DCACHE_ERR : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_DIS_DOWNLOAD_DCACHE_ERR    (BIT(11))
#define EFUSE_DIS_DOWNLOAD_DCACHE_ERR_M  (BIT(11))
#define EFUSE_DIS_DOWNLOAD_DCACHE_ERR_V  0x1
#define EFUSE_DIS_DOWNLOAD_DCACHE_ERR_S  11
/* EFUSE_DIS_DOWNLOAD_ICACHE_ERR : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR    (BIT(10))
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR_M  (BIT(10))
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR_V  0x1
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR_S  10
/* EFUSE_DIS_DCACHE_ERR : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_DIS_DCACHE_ERR    (BIT(9))
#define EFUSE_DIS_DCACHE_ERR_M  (BIT(9))
#define EFUSE_DIS_DCACHE_ERR_V  0x1
#define EFUSE_DIS_DCACHE_ERR_S  9
/* EFUSE_DIS_ICACHE_ERR : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_DIS_ICACHE_ERR    (BIT(8))
#define EFUSE_DIS_ICACHE_ERR_M  (BIT(8))
#define EFUSE_DIS_ICACHE_ERR_V  0x1
#define EFUSE_DIS_ICACHE_ERR_S  8
/* EFUSE_RPT4_RESERVED5_ERR : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: Reserved*/
#define EFUSE_RPT4_RESERVED5_ERR  (BIT(7))
#define EFUSE_RPT4_RESERVED5_ERR_M  (BIT(7))
#define EFUSE_RPT4_RESERVED5_ERR_V  0x1
#define EFUSE_RPT4_RESERVED5_ERR_S  7
/* EFUSE_RD_DIS_ERR : RO ;bitpos:[6:0] ;default: 7'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_RD_DIS_ERR    0x0000007F
#define EFUSE_RD_DIS_ERR_M  ((EFUSE_RD_DIS_ERR_V)<<(EFUSE_RD_DIS_ERR_S))
#define EFUSE_RD_DIS_ERR_V  0x7F
#define EFUSE_RD_DIS_ERR_S  0

#define EFUSE_RD_REPEAT_ERR1_REG          (DR_REG_EFUSE_BASE + 0x180)
/* EFUSE_KEY_PURPOSE_1_ERR : RO ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_KEY_PURPOSE_1_ERR    0x0000000F
#define EFUSE_KEY_PURPOSE_1_ERR_M  ((EFUSE_KEY_PURPOSE_1_ERR_V)<<(EFUSE_KEY_PURPOSE_1_ERR_S))
#define EFUSE_KEY_PURPOSE_1_ERR_V  0xF
#define EFUSE_KEY_PURPOSE_1_ERR_S  28
/* EFUSE_KEY_PURPOSE_0_ERR : RO ;bitpos:[27:24] ;default: 4'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_KEY_PURPOSE_0_ERR    0x0000000F
#define EFUSE_KEY_PURPOSE_0_ERR_M  ((EFUSE_KEY_PURPOSE_0_ERR_V)<<(EFUSE_KEY_PURPOSE_0_ERR_S))
#define EFUSE_KEY_PURPOSE_0_ERR_V  0xF
#define EFUSE_KEY_PURPOSE_0_ERR_S  24
/* EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR    (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR_M  (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR_S  23
/* EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR    (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR_M  (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR_S  22
/* EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR    (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR_M  (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR_S  21
/* EFUSE_SPI_BOOT_CRYPT_CNT_ERR : RO ;bitpos:[20:18] ;default: 3'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_SPI_BOOT_CRYPT_CNT_ERR    0x00000007
#define EFUSE_SPI_BOOT_CRYPT_CNT_ERR_M  ((EFUSE_SPI_BOOT_CRYPT_CNT_ERR_V)<<(EFUSE_SPI_BOOT_CRYPT_CNT_ERR_S))
#define EFUSE_SPI_BOOT_CRYPT_CNT_ERR_V  0x7
#define EFUSE_SPI_BOOT_CRYPT_CNT_ERR_S  18
/* EFUSE_WDT_DELAY_SEL_ERR : RO ;bitpos:[17:16] ;default: 2'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_WDT_DELAY_SEL_ERR    0x00000003
#define EFUSE_WDT_DELAY_SEL_ERR_M  ((EFUSE_WDT_DELAY_SEL_ERR_V)<<(EFUSE_WDT_DELAY_SEL_ERR_S))
#define EFUSE_WDT_DELAY_SEL_ERR_V  0x3
#define EFUSE_WDT_DELAY_SEL_ERR_S  16
/* EFUSE_VDD_SPI_DCAP_ERR : RO ;bitpos:[15:14] ;default: 2'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_VDD_SPI_DCAP_ERR    0x00000003
#define EFUSE_VDD_SPI_DCAP_ERR_M  ((EFUSE_VDD_SPI_DCAP_ERR_V)<<(EFUSE_VDD_SPI_DCAP_ERR_S))
#define EFUSE_VDD_SPI_DCAP_ERR_V  0x3
#define EFUSE_VDD_SPI_DCAP_ERR_S  14
/* EFUSE_VDD_SPI_INIT_ERR : RO ;bitpos:[13:12] ;default: 2'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_VDD_SPI_INIT_ERR    0x00000003
#define EFUSE_VDD_SPI_INIT_ERR_M  ((EFUSE_VDD_SPI_INIT_ERR_V)<<(EFUSE_VDD_SPI_INIT_ERR_S))
#define EFUSE_VDD_SPI_INIT_ERR_V  0x3
#define EFUSE_VDD_SPI_INIT_ERR_S  12
/* EFUSE_VDD_SPI_DCURLIM_ERR : RO ;bitpos:[11:9]] ;default: 3'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_VDD_SPI_DCURLIM_ERR    0x00000007
#define EFUSE_VDD_SPI_DCURLIM_ERR_M  ((EFUSE_VDD_SPI_DCURLIM_ERR_V)<<(EFUSE_VDD_SPI_DCURLIM_ERR_S))
#define EFUSE_VDD_SPI_DCURLIM_ERR_V  0x7
#define EFUSE_VDD_SPI_DCURLIM_ERR_S  9
/* EFUSE_VDD_SPI_ENCURLIM_ERR : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_VDD_SPI_ENCURLIM_ERR    (BIT(8))
#define EFUSE_VDD_SPI_ENCURLIM_ERR_M  (BIT(8))
#define EFUSE_VDD_SPI_ENCURLIM_ERR_V  0x1
#define EFUSE_VDD_SPI_ENCURLIM_ERR_S  8
/* EFUSE_VDD_SPI_EN_INIT_ERR : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_VDD_SPI_EN_INIT_ERR    (BIT(7))
#define EFUSE_VDD_SPI_EN_INIT_ERR_M  (BIT(7))
#define EFUSE_VDD_SPI_EN_INIT_ERR_V  0x1
#define EFUSE_VDD_SPI_EN_INIT_ERR_S  7
/* EFUSE_VDD_SPI_FORCE_ERR : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_VDD_SPI_FORCE_ERR    (BIT(6))
#define EFUSE_VDD_SPI_FORCE_ERR_M  (BIT(6))
#define EFUSE_VDD_SPI_FORCE_ERR_V  0x1
#define EFUSE_VDD_SPI_FORCE_ERR_S  6
/* EFUSE_VDD_SPI_TIEH_ERR : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_VDD_SPI_TIEH_ERR    (BIT(5))
#define EFUSE_VDD_SPI_TIEH_ERR_M  (BIT(5))
#define EFUSE_VDD_SPI_TIEH_ERR_V  0x1
#define EFUSE_VDD_SPI_TIEH_ERR_S  5
/* EFUSE_VDD_SPI_XPD_ERR : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_VDD_SPI_XPD_ERR    (BIT(4))
#define EFUSE_VDD_SPI_XPD_ERR_M  (BIT(4))
#define EFUSE_VDD_SPI_XPD_ERR_V  0x1
#define EFUSE_VDD_SPI_XPD_ERR_S  4
/* EFUSE_VDD_SPI_DREFL_ERR : RO ;bitpos:[3:2] ;default: 2'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_VDD_SPI_DREFL_ERR    0x00000003
#define EFUSE_VDD_SPI_DREFL_ERR_M  ((EFUSE_VDD_SPI_DREFL_ERR_V)<<(EFUSE_VDD_SPI_DREFL_ERR_S))
#define EFUSE_VDD_SPI_DREFL_ERR_V  0x3
#define EFUSE_VDD_SPI_DREFL_ERR_S  2
/* EFUSE_VDD_SPI_DREFM_ERR : RO ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_VDD_SPI_DREFM_ERR    0x00000003
#define EFUSE_VDD_SPI_DREFM_ERR_M  ((EFUSE_VDD_SPI_DREFM_ERR_V)<<(EFUSE_VDD_SPI_DREFM_ERR_S))
#define EFUSE_VDD_SPI_DREFM_ERR_V  0x3
#define EFUSE_VDD_SPI_DREFM_ERR_S  0

#define EFUSE_RD_REPEAT_ERR2_REG          (DR_REG_EFUSE_BASE + 0x184)
/* EFUSE_FLASH_TPUW_ERR : RO ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_FLASH_TPUW_ERR    0x0000000F
#define EFUSE_FLASH_TPUW_ERR_M  ((EFUSE_FLASH_TPUW_ERR_V)<<(EFUSE_FLASH_TPUW_ERR_S))
#define EFUSE_FLASH_TPUW_ERR_V  0xF
#define EFUSE_FLASH_TPUW_ERR_S  28
/* EFUSE_POWER_GLITCH_DSENSE_ERR : RO ;bitpos:[27:26] ;default: 2'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_POWER_GLITCH_DSENSE_ERR    0x00000003
#define EFUSE_POWER_GLITCH_DSENSE_ERR_M  ((EFUSE_POWER_GLITCH_DSENSE_ERR_V)<<(EFUSE_POWER_GLITCH_DSENSE_ERR_S))
#define EFUSE_POWER_GLITCH_DSENSE_ERR_V  0x3
#define EFUSE_POWER_GLITCH_DSENSE_ERR_S  26
/* EFUSE_USB_PHY_SEL_ERR : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_USB_PHY_SEL_ERR    (BIT(25))
#define EFUSE_USB_PHY_SEL_ERR_M  (BIT(25))
#define EFUSE_USB_PHY_SEL_ERR_V  0x1
#define EFUSE_USB_PHY_SEL_ERR_S  25
/* EFUSE_STRAP_JTAG_SEL_ERR : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_STRAP_JTAG_SEL_ERR    (BIT(24))
#define EFUSE_STRAP_JTAG_SEL_ERR_M  (BIT(24))
#define EFUSE_STRAP_JTAG_SEL_ERR_V  0x1
#define EFUSE_STRAP_JTAG_SEL_ERR_S  24
/* EFUSE_DIS_USB_DEVICE_ERR : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_DIS_USB_DEVICE_ERR    (BIT(23))
#define EFUSE_DIS_USB_DEVICE_ERR_M  (BIT(23))
#define EFUSE_DIS_USB_DEVICE_ERR_V  0x1
#define EFUSE_DIS_USB_DEVICE_ERR_S  23
/* EFUSE_DIS_USB_JTAG_ERR : RO ;bitpos:[22] ;default: 6'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_DIS_USB_JTAG_ERR    (BIT(22))
#define EFUSE_DIS_USB_JTAG_ERR_M  (BIT(22))
#define EFUSE_DIS_USB_JTAG_ERR_V  0x1
#define EFUSE_DIS_USB_JTAG_ERR_S  22
/* EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR    (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_M  (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_V  0x1
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_S  21
/* EFUSE_SECURE_BOOT_EN_ERR : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_SECURE_BOOT_EN_ERR    (BIT(20))
#define EFUSE_SECURE_BOOT_EN_ERR_M  (BIT(20))
#define EFUSE_SECURE_BOOT_EN_ERR_V  0x1
#define EFUSE_SECURE_BOOT_EN_ERR_S  20
/* EFUSE_RPT4_RESERVED0_ERR : RO ;bitpos:[19:16] ;default: 4'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_RPT4_RESERVED0_ERR    0x0000000F
#define EFUSE_RPT4_RESERVED0_ERR_M  ((EFUSE_RPT4_RESERVED0_ERR_V)<<(EFUSE_RPT4_RESERVED0_ERR_S))
#define EFUSE_RPT4_RESERVED0_ERR_V  0xF
#define EFUSE_RPT4_RESERVED0_ERR_S  16
/* EFUSE_KEY_PURPOSE_5_ERR : RO ;bitpos:[15:12] ;default: 4'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_KEY_PURPOSE_5_ERR    0x0000000F
#define EFUSE_KEY_PURPOSE_5_ERR_M  ((EFUSE_KEY_PURPOSE_5_ERR_V)<<(EFUSE_KEY_PURPOSE_5_ERR_S))
#define EFUSE_KEY_PURPOSE_5_ERR_V  0xF
#define EFUSE_KEY_PURPOSE_5_ERR_S  12
/* EFUSE_KEY_PURPOSE_4_ERR : RO ;bitpos:[11:8] ;default: 4'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_KEY_PURPOSE_4_ERR    0x0000000F
#define EFUSE_KEY_PURPOSE_4_ERR_M  ((EFUSE_KEY_PURPOSE_4_ERR_V)<<(EFUSE_KEY_PURPOSE_4_ERR_S))
#define EFUSE_KEY_PURPOSE_4_ERR_V  0xF
#define EFUSE_KEY_PURPOSE_4_ERR_S  8
/* EFUSE_KEY_PURPOSE_3_ERR : RO ;bitpos:[7:4] ;default: 4'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_KEY_PURPOSE_3_ERR    0x0000000F
#define EFUSE_KEY_PURPOSE_3_ERR_M  ((EFUSE_KEY_PURPOSE_3_ERR_V)<<(EFUSE_KEY_PURPOSE_3_ERR_S))
#define EFUSE_KEY_PURPOSE_3_ERR_V  0xF
#define EFUSE_KEY_PURPOSE_3_ERR_S  4
/* EFUSE_KEY_PURPOSE_2_ERR : RO ;bitpos:[3:0] ;default: 4'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_KEY_PURPOSE_2_ERR    0x0000000F
#define EFUSE_KEY_PURPOSE_2_ERR_M  ((EFUSE_KEY_PURPOSE_2_ERR_V)<<(EFUSE_KEY_PURPOSE_2_ERR_S))
#define EFUSE_KEY_PURPOSE_2_ERR_V  0xF
#define EFUSE_KEY_PURPOSE_2_ERR_S  0

#define EFUSE_RD_REPEAT_ERR3_REG          (DR_REG_EFUSE_BASE + 0x188)
/* EFUSE_DIS_USB_OTG_DOWNLOAD_MODE_ERR : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description: Set this bit to disable download through USB-OTG.*/
#define EFUSE_DIS_USB_OTG_DOWNLOAD_MODE_ERR  (BIT(31))
#define EFUSE_DIS_USB_OTG_DOWNLOAD_MODE_ERR_M  (BIT(31))
#define EFUSE_DIS_USB_OTG_DOWNLOAD_MODE_ERR_V  0x1
#define EFUSE_DIS_USB_OTG_DOWNLOAD_MODE_ERR_S  31
/* EFUSE_RPT4_RESERVED1_ERR : RO ;bitpos:[30] ;default: 1'h0 ; */
/*description: Reserved.*/
#define EFUSE_RPT4_RESERVED1_ERR  (BIT(30))
#define EFUSE_RPT4_RESERVED1_ERR_M  (BIT(30))
#define EFUSE_RPT4_RESERVED1_ERR_V  0x1
#define EFUSE_RPT4_RESERVED1_ERR_S  30
/* EFUSE_SECURE_VERSION_ERR : RO ;bitpos:[29:14] ;default: 16'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_SECURE_VERSION_ERR    0x0000FFFF
#define EFUSE_SECURE_VERSION_ERR_M  ((EFUSE_SECURE_VERSION_ERR_V)<<(EFUSE_SECURE_VERSION_ERR_S))
#define EFUSE_SECURE_VERSION_ERR_V  0xFFFF
#define EFUSE_SECURE_VERSION_ERR_S  14
/* EFUSE_FORCE_SEND_RESUME_ERR : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_FORCE_SEND_RESUME_ERR    (BIT(13))
#define EFUSE_FORCE_SEND_RESUME_ERR_M  (BIT(13))
#define EFUSE_FORCE_SEND_RESUME_ERR_V  0x1
#define EFUSE_FORCE_SEND_RESUME_ERR_S  13
/* EFUSE_FLASH_ECC_EN_ERR : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_FLASH_ECC_EN_ERR    (BIT(12))
#define EFUSE_FLASH_ECC_EN_ERR_M  (BIT(12))
#define EFUSE_FLASH_ECC_EN_ERR_V  0x1
#define EFUSE_FLASH_ECC_EN_ERR_S  12
/* EFUSE_FLASH_PAGE_SIZE_ERR : RO ;bitpos:[11:10] ;default: 2'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_FLASH_PAGE_SIZE_ERR    0x00000003
#define EFUSE_FLASH_PAGE_SIZE_ERR_M  ((EFUSE_FLASH_PAGE_SIZE_ERR_V)<<(EFUSE_FLASH_PAGE_SIZE_ERR_S))
#define EFUSE_FLASH_PAGE_SIZE_ERR_V  0x3
#define EFUSE_FLASH_PAGE_SIZE_ERR_S  10
/* EFUSE_FLASH_TYPE_ERR : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_FLASH_TYPE_ERR    (BIT(9))
#define EFUSE_FLASH_TYPE_ERR_M  (BIT(9))
#define EFUSE_FLASH_TYPE_ERR_V  0x1
#define EFUSE_FLASH_TYPE_ERR_S  9
/* EFUSE_PIN_POWER_SELECTION_ERR : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_PIN_POWER_SELECTION_ERR    (BIT(8))
#define EFUSE_PIN_POWER_SELECTION_ERR_M  (BIT(8))
#define EFUSE_PIN_POWER_SELECTION_ERR_V  0x1
#define EFUSE_PIN_POWER_SELECTION_ERR_S  8
/* EFUSE_UART_PRINT_CONTROL_ERR : RO ;bitpos:[7:6] ;default: 2'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_UART_PRINT_CONTROL_ERR    0x00000003
#define EFUSE_UART_PRINT_CONTROL_ERR_M  ((EFUSE_UART_PRINT_CONTROL_ERR_V)<<(EFUSE_UART_PRINT_CONTROL_ERR_S))
#define EFUSE_UART_PRINT_CONTROL_ERR_V  0x3
#define EFUSE_UART_PRINT_CONTROL_ERR_S  6
/* EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR    (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_M  (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_V  0x1
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_S  5
/* EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_ERR : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_ERR    (BIT(4))
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_ERR_M  (BIT(4))
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_ERR_V  0x1
#define EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE_ERR_S  4
/* EFUSE_FLASH_ECC_MODE_ERR : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_FLASH_ECC_MODE_ERR    (BIT(3))
#define EFUSE_FLASH_ECC_MODE_ERR_M  (BIT(3))
#define EFUSE_FLASH_ECC_MODE_ERR_V  0x1
#define EFUSE_FLASH_ECC_MODE_ERR_S  3
/* EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR    (BIT(2))
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR_M  (BIT(2))
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR_V  0x1
#define EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR_S  2
/* EFUSE_DIS_DIRECT_BOOT_ERR : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_DIS_DIRECT_BOOT_ERR    (BIT(1))
#define EFUSE_DIS_DIRECT_BOOT_ERR_M  (BIT(1))
#define EFUSE_DIS_DIRECT_BOOT_ERR_V  0x1
#define EFUSE_DIS_DIRECT_BOOT_ERR_S  1
/* EFUSE_DIS_DOWNLOAD_MODE_ERR : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_DIS_DOWNLOAD_MODE_ERR    (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_ERR_M  (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_ERR_V  0x1
#define EFUSE_DIS_DOWNLOAD_MODE_ERR_S  0

#define EFUSE_RD_REPEAT_ERR4_REG          (DR_REG_EFUSE_BASE + 0x18C)
/* EFUSE_RPT4_RESERVED2_ERR : RO ;bitpos:[23:0] ;default: 24'h0 ; */
/*description: If any bits in this filed are 1, then it indicates a programming error..*/
#define EFUSE_RPT4_RESERVED2_ERR    0x00FFFFFF
#define EFUSE_RPT4_RESERVED2_ERR_M  ((EFUSE_RPT4_RESERVED2_ERR_V)<<(EFUSE_RPT4_RESERVED2_ERR_S))
#define EFUSE_RPT4_RESERVED2_ERR_V  0xFFFFFF
#define EFUSE_RPT4_RESERVED2_ERR_S  0

#define EFUSE_RD_RS_ERR0_REG          (DR_REG_EFUSE_BASE + 0x1C0)
/* EFUSE_KEY4_FAIL : RO ;bitpos:[31] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of key4 is reliable 1: Means that programm
ing key4 failed and the number of error bytes is over 6..*/
#define EFUSE_KEY4_FAIL    (BIT(31))
#define EFUSE_KEY4_FAIL_M  (BIT(31))
#define EFUSE_KEY4_FAIL_V  0x1
#define EFUSE_KEY4_FAIL_S  31
/* EFUSE_KEY4_ERR_NUM : RO ;bitpos:[30:28] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes..*/
#define EFUSE_KEY4_ERR_NUM    0x00000007
#define EFUSE_KEY4_ERR_NUM_M  ((EFUSE_KEY4_ERR_NUM_V)<<(EFUSE_KEY4_ERR_NUM_S))
#define EFUSE_KEY4_ERR_NUM_V  0x7
#define EFUSE_KEY4_ERR_NUM_S  28
/* EFUSE_KEY3_FAIL : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of key3 is reliable 1: Means that programm
ing key3 failed and the number of error bytes is over 6..*/
#define EFUSE_KEY3_FAIL    (BIT(27))
#define EFUSE_KEY3_FAIL_M  (BIT(27))
#define EFUSE_KEY3_FAIL_V  0x1
#define EFUSE_KEY3_FAIL_S  27
/* EFUSE_KEY3_ERR_NUM : RO ;bitpos:[26:24] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes..*/
#define EFUSE_KEY3_ERR_NUM    0x00000007
#define EFUSE_KEY3_ERR_NUM_M  ((EFUSE_KEY3_ERR_NUM_V)<<(EFUSE_KEY3_ERR_NUM_S))
#define EFUSE_KEY3_ERR_NUM_V  0x7
#define EFUSE_KEY3_ERR_NUM_S  24
/* EFUSE_KEY2_FAIL : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of key2 is reliable 1: Means that programm
ing key2 failed and the number of error bytes is over 6..*/
#define EFUSE_KEY2_FAIL    (BIT(23))
#define EFUSE_KEY2_FAIL_M  (BIT(23))
#define EFUSE_KEY2_FAIL_V  0x1
#define EFUSE_KEY2_FAIL_S  23
/* EFUSE_KEY2_ERR_NUM : RO ;bitpos:[22:20] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes..*/
#define EFUSE_KEY2_ERR_NUM    0x00000007
#define EFUSE_KEY2_ERR_NUM_M  ((EFUSE_KEY2_ERR_NUM_V)<<(EFUSE_KEY2_ERR_NUM_S))
#define EFUSE_KEY2_ERR_NUM_V  0x7
#define EFUSE_KEY2_ERR_NUM_S  20
/* EFUSE_KEY1_FAIL : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of key1 is reliable 1: Means that programm
ing key1 failed and the number of error bytes is over 6..*/
#define EFUSE_KEY1_FAIL    (BIT(19))
#define EFUSE_KEY1_FAIL_M  (BIT(19))
#define EFUSE_KEY1_FAIL_V  0x1
#define EFUSE_KEY1_FAIL_S  19
/* EFUSE_KEY1_ERR_NUM : RO ;bitpos:[18:16] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes..*/
#define EFUSE_KEY1_ERR_NUM    0x00000007
#define EFUSE_KEY1_ERR_NUM_M  ((EFUSE_KEY1_ERR_NUM_V)<<(EFUSE_KEY1_ERR_NUM_S))
#define EFUSE_KEY1_ERR_NUM_V  0x7
#define EFUSE_KEY1_ERR_NUM_S  16
/* EFUSE_KEY0_FAIL : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of key0 is reliable 1: Means that programm
ing key0 failed and the number of error bytes is over 6..*/
#define EFUSE_KEY0_FAIL    (BIT(15))
#define EFUSE_KEY0_FAIL_M  (BIT(15))
#define EFUSE_KEY0_FAIL_V  0x1
#define EFUSE_KEY0_FAIL_S  15
/* EFUSE_KEY0_ERR_NUM : RO ;bitpos:[14:12] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes..*/
#define EFUSE_KEY0_ERR_NUM    0x00000007
#define EFUSE_KEY0_ERR_NUM_M  ((EFUSE_KEY0_ERR_NUM_V)<<(EFUSE_KEY0_ERR_NUM_S))
#define EFUSE_KEY0_ERR_NUM_V  0x7
#define EFUSE_KEY0_ERR_NUM_S  12
/* EFUSE_USR_DATA_FAIL : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the user data is reliable 1: Means that programming
 user data failed and the number of error bytes is over 6..*/
#define EFUSE_USR_DATA_FAIL    (BIT(11))
#define EFUSE_USR_DATA_FAIL_M  (BIT(11))
#define EFUSE_USR_DATA_FAIL_V  0x1
#define EFUSE_USR_DATA_FAIL_S  11
/* EFUSE_USR_DATA_ERR_NUM : RO ;bitpos:[10:8] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes..*/
#define EFUSE_USR_DATA_ERR_NUM    0x00000007
#define EFUSE_USR_DATA_ERR_NUM_M  ((EFUSE_USR_DATA_ERR_NUM_V)<<(EFUSE_USR_DATA_ERR_NUM_S))
#define EFUSE_USR_DATA_ERR_NUM_V  0x7
#define EFUSE_USR_DATA_ERR_NUM_S  8
/* EFUSE_SYS_PART1_FAIL : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of system part1 is reliable 1: Means that
programming user data failed and the number of error bytes is over 6..*/
#define EFUSE_SYS_PART1_FAIL    (BIT(7))
#define EFUSE_SYS_PART1_FAIL_M  (BIT(7))
#define EFUSE_SYS_PART1_FAIL_V  0x1
#define EFUSE_SYS_PART1_FAIL_S  7
/* EFUSE_SYS_PART1_NUM : RO ;bitpos:[6:4] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes..*/
#define EFUSE_SYS_PART1_NUM    0x00000007
#define EFUSE_SYS_PART1_NUM_M  ((EFUSE_SYS_PART1_NUM_V)<<(EFUSE_SYS_PART1_NUM_S))
#define EFUSE_SYS_PART1_NUM_V  0x7
#define EFUSE_SYS_PART1_NUM_S  4
/* EFUSE_MAC_SPI_8M_FAIL : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of MAC_SPI_8M is reliable 1: Means that pr
ogramming user data failed and the number of error bytes is over 6..*/
#define EFUSE_MAC_SPI_8M_FAIL    (BIT(3))
#define EFUSE_MAC_SPI_8M_FAIL_M  (BIT(3))
#define EFUSE_MAC_SPI_8M_FAIL_V  0x1
#define EFUSE_MAC_SPI_8M_FAIL_S  3
/* EFUSE_MAC_SPI_8M_ERR_NUM : RO ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes..*/
#define EFUSE_MAC_SPI_8M_ERR_NUM    0x00000007
#define EFUSE_MAC_SPI_8M_ERR_NUM_M  ((EFUSE_MAC_SPI_8M_ERR_NUM_V)<<(EFUSE_MAC_SPI_8M_ERR_NUM_S))
#define EFUSE_MAC_SPI_8M_ERR_NUM_V  0x7
#define EFUSE_MAC_SPI_8M_ERR_NUM_S  0

#define EFUSE_RD_RS_ERR1_REG          (DR_REG_EFUSE_BASE + 0x1C4)
/* EFUSE_SYS_PART2_FAIL : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of system part2 is reliable 1: Means that
programming user data failed and the number of error bytes is over 6..*/
#define EFUSE_SYS_PART2_FAIL    (BIT(7))
#define EFUSE_SYS_PART2_FAIL_M  (BIT(7))
#define EFUSE_SYS_PART2_FAIL_V  0x1
#define EFUSE_SYS_PART2_FAIL_S  7
/* EFUSE_SYS_PART2_ERR_NUM : RO ;bitpos:[6:4] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes..*/
#define EFUSE_SYS_PART2_ERR_NUM    0x00000007
#define EFUSE_SYS_PART2_ERR_NUM_M  ((EFUSE_SYS_PART2_ERR_NUM_V)<<(EFUSE_SYS_PART2_ERR_NUM_S))
#define EFUSE_SYS_PART2_ERR_NUM_V  0x7
#define EFUSE_SYS_PART2_ERR_NUM_S  4
/* EFUSE_KEY5_FAIL : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of KEY5 is reliable 1: Means that programm
ing user data failed and the number of error bytes is over 6..*/
#define EFUSE_KEY5_FAIL    (BIT(3))
#define EFUSE_KEY5_FAIL_M  (BIT(3))
#define EFUSE_KEY5_FAIL_V  0x1
#define EFUSE_KEY5_FAIL_S  3
/* EFUSE_KEY5_ERR_NUM : RO ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes..*/
#define EFUSE_KEY5_ERR_NUM    0x00000007
#define EFUSE_KEY5_ERR_NUM_M  ((EFUSE_KEY5_ERR_NUM_V)<<(EFUSE_KEY5_ERR_NUM_S))
#define EFUSE_KEY5_ERR_NUM_V  0x7
#define EFUSE_KEY5_ERR_NUM_S  0

#define EFUSE_CLK_REG          (DR_REG_EFUSE_BASE + 0x1C8)
/* EFUSE_CLK_EN : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: Set this bit and force to enable clock signal of eFuse memory..*/
#define EFUSE_CLK_EN    (BIT(16))
#define EFUSE_CLK_EN_M  (BIT(16))
#define EFUSE_CLK_EN_V  0x1
#define EFUSE_CLK_EN_S  16
/* EFUSE_MEM_FORCE_PU : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to force eFuse SRAM into working mode..*/
#define EFUSE_MEM_FORCE_PU    (BIT(2))
#define EFUSE_MEM_FORCE_PU_M  (BIT(2))
#define EFUSE_MEM_FORCE_PU_V  0x1
#define EFUSE_MEM_FORCE_PU_S  2
/* EFUSE_MEM_CLK_FORCE_ON : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: Set this bit and force to activate clock signal of eFuse SRAM..*/
#define EFUSE_MEM_CLK_FORCE_ON    (BIT(1))
#define EFUSE_MEM_CLK_FORCE_ON_M  (BIT(1))
#define EFUSE_MEM_CLK_FORCE_ON_V  0x1
#define EFUSE_MEM_CLK_FORCE_ON_S  1
/* EFUSE_MEM_FORCE_PD : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to force eFuse SRAM into power-saving mode..*/
#define EFUSE_MEM_FORCE_PD    (BIT(0))
#define EFUSE_MEM_FORCE_PD_M  (BIT(0))
#define EFUSE_MEM_FORCE_PD_V  0x1
#define EFUSE_MEM_FORCE_PD_S  0

#define EFUSE_CONF_REG          (DR_REG_EFUSE_BASE + 0x1CC)
/* EFUSE_OP_CODE : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: 0x5A5A: Operate programming command 0x5AA5: Operate read command..*/
#define EFUSE_OP_CODE    0x0000FFFF
#define EFUSE_OP_CODE_M  ((EFUSE_OP_CODE_V)<<(EFUSE_OP_CODE_S))
#define EFUSE_OP_CODE_V  0xFFFF
#define EFUSE_OP_CODE_S  0

#define EFUSE_WRITE_OP_CODE 0x5a5a
#define EFUSE_READ_OP_CODE  0x5aa5

#define EFUSE_STATUS_REG          (DR_REG_EFUSE_BASE + 0x1D0)
/* EFUSE_REPEAT_ERR_CNT : RO ;bitpos:[17:10] ;default: 8'h0 ; */
/*description: Indicates the number of error bits during programming BLOCK0..*/
#define EFUSE_REPEAT_ERR_CNT    0x000000FF
#define EFUSE_REPEAT_ERR_CNT_M  ((EFUSE_REPEAT_ERR_CNT_V)<<(EFUSE_REPEAT_ERR_CNT_S))
#define EFUSE_REPEAT_ERR_CNT_V  0xFF
#define EFUSE_REPEAT_ERR_CNT_S  10
/* EFUSE_OTP_VDDQ_IS_SW : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The value of OTP_VDDQ_IS_SW..*/
#define EFUSE_OTP_VDDQ_IS_SW    (BIT(9))
#define EFUSE_OTP_VDDQ_IS_SW_M  (BIT(9))
#define EFUSE_OTP_VDDQ_IS_SW_V  0x1
#define EFUSE_OTP_VDDQ_IS_SW_S  9
/* EFUSE_OTP_PGENB_SW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The value of OTP_PGENB_SW..*/
#define EFUSE_OTP_PGENB_SW    (BIT(8))
#define EFUSE_OTP_PGENB_SW_M  (BIT(8))
#define EFUSE_OTP_PGENB_SW_V  0x1
#define EFUSE_OTP_PGENB_SW_S  8
/* EFUSE_OTP_CSB_SW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The value of OTP_CSB_SW..*/
#define EFUSE_OTP_CSB_SW    (BIT(7))
#define EFUSE_OTP_CSB_SW_M  (BIT(7))
#define EFUSE_OTP_CSB_SW_V  0x1
#define EFUSE_OTP_CSB_SW_S  7
/* EFUSE_OTP_STROBE_SW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The value of OTP_STROBE_SW..*/
#define EFUSE_OTP_STROBE_SW    (BIT(6))
#define EFUSE_OTP_STROBE_SW_M  (BIT(6))
#define EFUSE_OTP_STROBE_SW_V  0x1
#define EFUSE_OTP_STROBE_SW_S  6
/* EFUSE_OTP_VDDQ_C_SYNC2 : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The value of OTP_VDDQ_C_SYNC2..*/
#define EFUSE_OTP_VDDQ_C_SYNC2    (BIT(5))
#define EFUSE_OTP_VDDQ_C_SYNC2_M  (BIT(5))
#define EFUSE_OTP_VDDQ_C_SYNC2_V  0x1
#define EFUSE_OTP_VDDQ_C_SYNC2_S  5
/* EFUSE_OTP_LOAD_SW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The value of OTP_LOAD_SW..*/
#define EFUSE_OTP_LOAD_SW    (BIT(4))
#define EFUSE_OTP_LOAD_SW_M  (BIT(4))
#define EFUSE_OTP_LOAD_SW_V  0x1
#define EFUSE_OTP_LOAD_SW_S  4
/* EFUSE_STATE : RO ;bitpos:[3:0] ;default: 4'h0 ; */
/*description: Indicates the state of the eFuse state machine..*/
#define EFUSE_STATE    0x0000000F
#define EFUSE_STATE_M  ((EFUSE_STATE_V)<<(EFUSE_STATE_S))
#define EFUSE_STATE_V  0xF
#define EFUSE_STATE_S  0

#define EFUSE_CMD_REG          (DR_REG_EFUSE_BASE + 0x1D4)
/* EFUSE_BLK_NUM : R/W ;bitpos:[5:2] ;default: 4'h0 ; */
/*description: The serial number of the block to be programmed. Value 0-10 corresponds to block
 number 0-10, respectively..*/
#define EFUSE_BLK_NUM    0x0000000F
#define EFUSE_BLK_NUM_M  ((EFUSE_BLK_NUM_V)<<(EFUSE_BLK_NUM_S))
#define EFUSE_BLK_NUM_V  0xF
#define EFUSE_BLK_NUM_S  2
/* EFUSE_PGM_CMD : R/WS/SC ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to send programming command..*/
#define EFUSE_PGM_CMD    (BIT(1))
#define EFUSE_PGM_CMD_M  (BIT(1))
#define EFUSE_PGM_CMD_V  0x1
#define EFUSE_PGM_CMD_S  1
/* EFUSE_READ_CMD : R/WS/SC ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to send read command..*/
#define EFUSE_READ_CMD    (BIT(0))
#define EFUSE_READ_CMD_M  (BIT(0))
#define EFUSE_READ_CMD_V  0x1
#define EFUSE_READ_CMD_S  0

#define EFUSE_INT_RAW_REG          (DR_REG_EFUSE_BASE + 0x1D8)
/* EFUSE_PGM_DONE_INT_RAW : R/WC/SS ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw bit signal for pgm_done interrupt..*/
#define EFUSE_PGM_DONE_INT_RAW    (BIT(1))
#define EFUSE_PGM_DONE_INT_RAW_M  (BIT(1))
#define EFUSE_PGM_DONE_INT_RAW_V  0x1
#define EFUSE_PGM_DONE_INT_RAW_S  1
/* EFUSE_READ_DONE_INT_RAW : R/WC/SS ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw bit signal for read_done interrupt..*/
#define EFUSE_READ_DONE_INT_RAW    (BIT(0))
#define EFUSE_READ_DONE_INT_RAW_M  (BIT(0))
#define EFUSE_READ_DONE_INT_RAW_V  0x1
#define EFUSE_READ_DONE_INT_RAW_S  0

#define EFUSE_INT_ST_REG          (DR_REG_EFUSE_BASE + 0x1DC)
/* EFUSE_PGM_DONE_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The status signal for pgm_done interrupt..*/
#define EFUSE_PGM_DONE_INT_ST    (BIT(1))
#define EFUSE_PGM_DONE_INT_ST_M  (BIT(1))
#define EFUSE_PGM_DONE_INT_ST_V  0x1
#define EFUSE_PGM_DONE_INT_ST_S  1
/* EFUSE_READ_DONE_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The status signal for read_done interrupt..*/
#define EFUSE_READ_DONE_INT_ST    (BIT(0))
#define EFUSE_READ_DONE_INT_ST_M  (BIT(0))
#define EFUSE_READ_DONE_INT_ST_V  0x1
#define EFUSE_READ_DONE_INT_ST_S  0

#define EFUSE_INT_ENA_REG          (DR_REG_EFUSE_BASE + 0x1E0)
/* EFUSE_PGM_DONE_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The enable signal for pgm_done interrupt..*/
#define EFUSE_PGM_DONE_INT_ENA    (BIT(1))
#define EFUSE_PGM_DONE_INT_ENA_M  (BIT(1))
#define EFUSE_PGM_DONE_INT_ENA_V  0x1
#define EFUSE_PGM_DONE_INT_ENA_S  1
/* EFUSE_READ_DONE_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The enable signal for read_done interrupt..*/
#define EFUSE_READ_DONE_INT_ENA    (BIT(0))
#define EFUSE_READ_DONE_INT_ENA_M  (BIT(0))
#define EFUSE_READ_DONE_INT_ENA_V  0x1
#define EFUSE_READ_DONE_INT_ENA_S  0

#define EFUSE_INT_CLR_REG          (DR_REG_EFUSE_BASE + 0x1E4)
/* EFUSE_PGM_DONE_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The clear signal for pgm_done interrupt..*/
#define EFUSE_PGM_DONE_INT_CLR    (BIT(1))
#define EFUSE_PGM_DONE_INT_CLR_M  (BIT(1))
#define EFUSE_PGM_DONE_INT_CLR_V  0x1
#define EFUSE_PGM_DONE_INT_CLR_S  1
/* EFUSE_READ_DONE_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The clear signal for read_done interrupt..*/
#define EFUSE_READ_DONE_INT_CLR    (BIT(0))
#define EFUSE_READ_DONE_INT_CLR_M  (BIT(0))
#define EFUSE_READ_DONE_INT_CLR_V  0x1
#define EFUSE_READ_DONE_INT_CLR_S  0

#define EFUSE_DAC_CONF_REG          (DR_REG_EFUSE_BASE + 0x1E8)
/* EFUSE_OE_CLR : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: Reduces the power supply of the programming voltage..*/
#define EFUSE_OE_CLR    (BIT(17))
#define EFUSE_OE_CLR_M  (BIT(17))
#define EFUSE_OE_CLR_V  0x1
#define EFUSE_OE_CLR_S  17
/* EFUSE_DAC_NUM : R/W ;bitpos:[16:9] ;default: 8'd255 ; */
/*description: Controls the rising period of the programming voltage..*/
#define EFUSE_DAC_NUM    0x000000FF
#define EFUSE_DAC_NUM_M  ((EFUSE_DAC_NUM_V)<<(EFUSE_DAC_NUM_S))
#define EFUSE_DAC_NUM_V  0xFF
#define EFUSE_DAC_NUM_S  9
/* EFUSE_DAC_CLK_PAD_SEL : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: Don't care..*/
#define EFUSE_DAC_CLK_PAD_SEL    (BIT(8))
#define EFUSE_DAC_CLK_PAD_SEL_M  (BIT(8))
#define EFUSE_DAC_CLK_PAD_SEL_V  0x1
#define EFUSE_DAC_CLK_PAD_SEL_S  8
/* EFUSE_DAC_CLK_DIV : R/W ;bitpos:[7:0] ;default: 8'd28 ; */
/*description: Controls the division factor of the rising clock of the programming voltage..*/
#define EFUSE_DAC_CLK_DIV    0x000000FF
#define EFUSE_DAC_CLK_DIV_M  ((EFUSE_DAC_CLK_DIV_V)<<(EFUSE_DAC_CLK_DIV_S))
#define EFUSE_DAC_CLK_DIV_V  0xFF
#define EFUSE_DAC_CLK_DIV_S  0

#define EFUSE_RD_TIM_CONF_REG          (DR_REG_EFUSE_BASE + 0x1EC)
/* EFUSE_READ_INIT_NUM : R/W ;bitpos:[31:24] ;default: 8'h12 ; */
/*description: Configures the initial read time of eFuse..*/
#define EFUSE_READ_INIT_NUM    0x000000FF
#define EFUSE_READ_INIT_NUM_M  ((EFUSE_READ_INIT_NUM_V)<<(EFUSE_READ_INIT_NUM_S))
#define EFUSE_READ_INIT_NUM_V  0xFF
#define EFUSE_READ_INIT_NUM_S  24

#define EFUSE_WR_TIM_CONF0_REG          (DR_REG_EFUSE_BASE + 0x1F0)

#define EFUSE_WR_TIM_CONF1_REG          (DR_REG_EFUSE_BASE + 0x1F4)
/* EFUSE_PWR_ON_NUM : R/W ;bitpos:[23:8] ;default: 16'h2880 ; */
/*description: Configures the power up time for VDDQ..*/
#define EFUSE_PWR_ON_NUM    0x0000FFFF
#define EFUSE_PWR_ON_NUM_M  ((EFUSE_PWR_ON_NUM_V)<<(EFUSE_PWR_ON_NUM_S))
#define EFUSE_PWR_ON_NUM_V  0xFFFF
#define EFUSE_PWR_ON_NUM_S  8

#define EFUSE_WR_TIM_CONF2_REG          (DR_REG_EFUSE_BASE + 0x1F8)
/* EFUSE_PWR_OFF_NUM : R/W ;bitpos:[15:0] ;default: 16'h190 ; */
/*description: Configures the power outage time for VDDQ..*/
#define EFUSE_PWR_OFF_NUM    0x0000FFFF
#define EFUSE_PWR_OFF_NUM_M  ((EFUSE_PWR_OFF_NUM_V)<<(EFUSE_PWR_OFF_NUM_S))
#define EFUSE_PWR_OFF_NUM_V  0xFFFF
#define EFUSE_PWR_OFF_NUM_S  0

#define EFUSE_DATE_REG          (DR_REG_EFUSE_BASE + 0x1FC)
/* EFUSE_DATE : R/W ;bitpos:[27:0] ;default: 28'h2101180 ; */
/*description: Stores eFuse version..*/
#define EFUSE_DATE    0x0FFFFFFF
#define EFUSE_DATE_M  ((EFUSE_DATE_V)<<(EFUSE_DATE_S))
#define EFUSE_DATE_V  0xFFFFFFF
#define EFUSE_DATE_S  0


#ifdef __cplusplus
}
#endif



#endif /*_SOC_EFUSE_REG_H_ */
