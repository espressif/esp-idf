// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _SOC_EFUSE_REG_H_
#define _SOC_EFUSE_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"
#define EFUSE_PGM_DATA0_REG          (DR_REG_EFUSE_BASE + 0x000)
/* EFUSE_WR_DIS : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Set this bit to disable eFuse programming.*/
#define EFUSE_WR_DIS  0xFFFFFFFF
#define EFUSE_WR_DIS_M  ((EFUSE_WR_DIS_V)<<(EFUSE_WR_DIS_S))
#define EFUSE_WR_DIS_V  0xFFFFFFFF
#define EFUSE_WR_DIS_S  0

#define EFUSE_PGM_DATA1_REG          (DR_REG_EFUSE_BASE + 0x004)
/* EFUSE_VDD_SPI_DREFH : R/W ;bitpos:[31:30] ;default: 2'h0 ; */
/*description: SPI regulator high voltage reference.*/
#define EFUSE_VDD_SPI_DREFH  0x00000003
#define EFUSE_VDD_SPI_DREFH_M  ((EFUSE_VDD_SPI_DREFH_V)<<(EFUSE_VDD_SPI_DREFH_S))
#define EFUSE_VDD_SPI_DREFH_V  0x3
#define EFUSE_VDD_SPI_DREFH_S  30
/* EFUSE_VDD_SPI_MODECURLIM : R/W ;bitpos:[29] ;default: 1'h0 ; */
/*description: SPI regulator switches current limit mode.*/
#define EFUSE_VDD_SPI_MODECURLIM  (BIT(29))
#define EFUSE_VDD_SPI_MODECURLIM_M  (BIT(29))
#define EFUSE_VDD_SPI_MODECURLIM_V  0x1
#define EFUSE_VDD_SPI_MODECURLIM_S  29
/* EFUSE_RPT4_RESERVED0 : RO ;bitpos:[28:27] ;default: 2'h0 ; */
/*description: Reserved (used for four backups method).*/
#define EFUSE_RPT4_RESERVED0  0x00000003
#define EFUSE_RPT4_RESERVED0_M  ((EFUSE_RPT4_RESERVED0_V)<<(EFUSE_RPT4_RESERVED0_S))
#define EFUSE_RPT4_RESERVED0_V  0x3
#define EFUSE_RPT4_RESERVED0_S  27
/* EFUSE_USB_FORCE_NOPERSIST : R/W ;bitpos:[26] ;default: 1'h0 ; */
/*description: Force nopersist to 1.*/
#define EFUSE_USB_FORCE_NOPERSIST  (BIT(26))
#define EFUSE_USB_FORCE_NOPERSIST_M  (BIT(26))
#define EFUSE_USB_FORCE_NOPERSIST_V  0x1
#define EFUSE_USB_FORCE_NOPERSIST_S  26
/* EFUSE_USB_EXT_PHY_ENABLE : R/W ;bitpos:[25] ;default: 1'h0 ; */
/*description: Set this bit to enable external PHY.*/
#define EFUSE_USB_EXT_PHY_ENABLE  (BIT(25))
#define EFUSE_USB_EXT_PHY_ENABLE_M  (BIT(25))
#define EFUSE_USB_EXT_PHY_ENABLE_V  0x1
#define EFUSE_USB_EXT_PHY_ENABLE_S  25
/* EFUSE_USB_EXCHG_PINS : R/W ;bitpos:[24] ;default: 1'h0 ; */
/*description: Set this bit to exchange D+ and D- pins.*/
#define EFUSE_USB_EXCHG_PINS  (BIT(24))
#define EFUSE_USB_EXCHG_PINS_M  (BIT(24))
#define EFUSE_USB_EXCHG_PINS_V  0x1
#define EFUSE_USB_EXCHG_PINS_S  24
/* EFUSE_USB_DREFL : R/W ;bitpos:[23:22] ;default: 2'h0 ; */
/*description: Controls single-end input threshold vrefl  0.8 V to 1.04 V with
 step of 80 mV  stored in eFuse.*/
#define EFUSE_USB_DREFL  0x00000003
#define EFUSE_USB_DREFL_M  ((EFUSE_USB_DREFL_V)<<(EFUSE_USB_DREFL_S))
#define EFUSE_USB_DREFL_V  0x3
#define EFUSE_USB_DREFL_S  22
/* EFUSE_USB_DREFH : R/W ;bitpos:[21:20] ;default: 2'h0 ; */
/*description: Controls single-end input threshold vrefh  1.76 V to 2 V with
 step of 80 mV  stored in eFuse.*/
#define EFUSE_USB_DREFH  0x00000003
#define EFUSE_USB_DREFH_M  ((EFUSE_USB_DREFH_V)<<(EFUSE_USB_DREFH_S))
#define EFUSE_USB_DREFH_V  0x3
#define EFUSE_USB_DREFH_S  20
/* EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT : R/W ;bitpos:[19] ;default: 1'h0 ; */
/*description: Set this bit to disable flash encrypt function (except in SPI/HSPI/Legacy_SPI
 boot mode).*/
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT  (BIT(19))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_M  (BIT(19))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_V  0x1
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_S  19
/* EFUSE_HARD_DIS_JTAG : R/W ;bitpos:[18] ;default: 1'h0 ; */
/*description: Set this bit to disable JTAG in the hard way. JTAG is disabled permanently.*/
#define EFUSE_HARD_DIS_JTAG  (BIT(18))
#define EFUSE_HARD_DIS_JTAG_M  (BIT(18))
#define EFUSE_HARD_DIS_JTAG_V  0x1
#define EFUSE_HARD_DIS_JTAG_S  18
/* EFUSE_SOFT_DIS_JTAG : R/W ;bitpos:[17] ;default: 1'h0 ; */
/*description: Set this bit to disable JTAG in the soft way. JTAG can be enabled
 in HMAC module.*/
#define EFUSE_SOFT_DIS_JTAG  (BIT(17))
#define EFUSE_SOFT_DIS_JTAG_M  (BIT(17))
#define EFUSE_SOFT_DIS_JTAG_V  0x1
#define EFUSE_SOFT_DIS_JTAG_S  17
/* EFUSE_DIS_EFUSE_ATE_WR : R/W ;bitpos:[16] ;default: 1'h0 ; */
/*description: Set this bit to disable programming eFuse through ATE mode.*/
#define EFUSE_DIS_EFUSE_ATE_WR  (BIT(16))
#define EFUSE_DIS_EFUSE_ATE_WR_M  (BIT(16))
#define EFUSE_DIS_EFUSE_ATE_WR_V  0x1
#define EFUSE_DIS_EFUSE_ATE_WR_S  16
/* EFUSE_DIS_BOOT_REMAP : R/W ;bitpos:[15] ;default: 1'h0 ; */
/*description: Set this bit to disable boot remap from RAM to ROM.*/
#define EFUSE_DIS_BOOT_REMAP  (BIT(15))
#define EFUSE_DIS_BOOT_REMAP_M  (BIT(15))
#define EFUSE_DIS_BOOT_REMAP_V  0x1
#define EFUSE_DIS_BOOT_REMAP_S  15
/* EFUSE_DIS_CAN : R/W ;bitpos:[14] ;default: 1'h0 ; */
/*description: Set this bit to disable CAN function.*/
#define EFUSE_DIS_CAN  (BIT(14))
#define EFUSE_DIS_CAN_M  (BIT(14))
#define EFUSE_DIS_CAN_V  0x1
#define EFUSE_DIS_CAN_S  14
/* EFUSE_DIS_USB : R/W ;bitpos:[13] ;default: 1'h0 ; */
/*description: Set this bit to disable USB function.*/
#define EFUSE_DIS_USB  (BIT(13))
#define EFUSE_DIS_USB_M  (BIT(13))
#define EFUSE_DIS_USB_V  0x1
#define EFUSE_DIS_USB_S  13
/* EFUSE_DIS_FORCE_DOWNLOAD : R/W ;bitpos:[12] ;default: 1'h0 ; */
/*description: Set this bit to disable the function that forces chip into download mode.*/
#define EFUSE_DIS_FORCE_DOWNLOAD  (BIT(12))
#define EFUSE_DIS_FORCE_DOWNLOAD_M  (BIT(12))
#define EFUSE_DIS_FORCE_DOWNLOAD_V  0x1
#define EFUSE_DIS_FORCE_DOWNLOAD_S  12
/* EFUSE_DIS_DOWNLOAD_DCACHE : R/W ;bitpos:[11] ;default: 1'h0 ; */
/*description: Set this bit to disable Dcache in download mode ( boot_mode[3:0]
 is 0  1  2  3  6  7).*/
#define EFUSE_DIS_DOWNLOAD_DCACHE  (BIT(11))
#define EFUSE_DIS_DOWNLOAD_DCACHE_M  (BIT(11))
#define EFUSE_DIS_DOWNLOAD_DCACHE_V  0x1
#define EFUSE_DIS_DOWNLOAD_DCACHE_S  11
/* EFUSE_DIS_DOWNLOAD_ICACHE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: Set this bit to disable Icache in download mode (boot_mode[3:0]
 is 0  1  2  3  6  7).*/
#define EFUSE_DIS_DOWNLOAD_ICACHE  (BIT(10))
#define EFUSE_DIS_DOWNLOAD_ICACHE_M  (BIT(10))
#define EFUSE_DIS_DOWNLOAD_ICACHE_V  0x1
#define EFUSE_DIS_DOWNLOAD_ICACHE_S  10
/* EFUSE_DIS_DCACHE : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: Set this bit to disable Dcache.*/
#define EFUSE_DIS_DCACHE  (BIT(9))
#define EFUSE_DIS_DCACHE_M  (BIT(9))
#define EFUSE_DIS_DCACHE_V  0x1
#define EFUSE_DIS_DCACHE_S  9
/* EFUSE_DIS_ICACHE : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: Set this bit to disable Icache.*/
#define EFUSE_DIS_ICACHE  (BIT(8))
#define EFUSE_DIS_ICACHE_M  (BIT(8))
#define EFUSE_DIS_ICACHE_V  0x1
#define EFUSE_DIS_ICACHE_S  8
/* EFUSE_DIS_RTC_RAM_BOOT : R/W ;bitpos:[7] ;default: 1'h0 ; */
/*description: Set this bit to disable boot from RTC RAM.*/
#define EFUSE_DIS_RTC_RAM_BOOT  (BIT(7))
#define EFUSE_DIS_RTC_RAM_BOOT_M  (BIT(7))
#define EFUSE_DIS_RTC_RAM_BOOT_V  0x1
#define EFUSE_DIS_RTC_RAM_BOOT_S  7
/* EFUSE_RD_DIS : R/W ;bitpos:[6:0] ;default: 7'h0 ; */
/*description: Set this bit to disable reading from BlOCK4-10.*/
#define EFUSE_RD_DIS  0x0000007F
#define EFUSE_RD_DIS_M  ((EFUSE_RD_DIS_V)<<(EFUSE_RD_DIS_S))
#define EFUSE_RD_DIS_V  0x7F
#define EFUSE_RD_DIS_S  0

#define EFUSE_RD_DIS_KEY0 (1<<0)
#define EFUSE_RD_DIS_KEY1 (1<<1)
#define EFUSE_RD_DIS_KEY2 (1<<2)
#define EFUSE_RD_DIS_KEY3 (1<<3)
#define EFUSE_RD_DIS_KEY4 (1<<4)
#define EFUSE_RD_DIS_KEY5 (1<<5)
#define EFUSE_RD_DIS_SYS_DATA_PART2 (1<<6)

#define EFUSE_PGM_DATA2_REG          (DR_REG_EFUSE_BASE + 0x008)
/* EFUSE_KEY_PURPOSE_1 : R/W ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: Purpose of Key1. Refer to Table KEY_PURPOSE Values.*/
#define EFUSE_KEY_PURPOSE_1  0x0000000F
#define EFUSE_KEY_PURPOSE_1_M  ((EFUSE_KEY_PURPOSE_1_V)<<(EFUSE_KEY_PURPOSE_1_S))
#define EFUSE_KEY_PURPOSE_1_V  0xF
#define EFUSE_KEY_PURPOSE_1_S  28
/* EFUSE_KEY_PURPOSE_0 : R/W ;bitpos:[27:24] ;default: 4'h0 ; */
/*description: Purpose of Key0. Refer to Table KEY_PURPOSE Values.*/
#define EFUSE_KEY_PURPOSE_0  0x0000000F
#define EFUSE_KEY_PURPOSE_0_M  ((EFUSE_KEY_PURPOSE_0_V)<<(EFUSE_KEY_PURPOSE_0_S))
#define EFUSE_KEY_PURPOSE_0_V  0xF
#define EFUSE_KEY_PURPOSE_0_S  24
/* EFUSE_SECURE_BOOT_KEY_REVOKE2 : R/W ;bitpos:[23] ;default: 1'h0 ; */
/*description: Set this bit to enable revoking third secure boot key.*/
#define EFUSE_SECURE_BOOT_KEY_REVOKE2  (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_M  (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_S  23
/* EFUSE_SECURE_BOOT_KEY_REVOKE1 : R/W ;bitpos:[22] ;default: 1'h0 ; */
/*description: Set this bit to enable revoking second secure boot key.*/
#define EFUSE_SECURE_BOOT_KEY_REVOKE1  (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_M  (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_S  22
/* EFUSE_SECURE_BOOT_KEY_REVOKE0 : R/W ;bitpos:[21] ;default: 1'h0 ; */
/*description: Set this bit to enable revoking first secure boot key.*/
#define EFUSE_SECURE_BOOT_KEY_REVOKE0  (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_M  (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_S  21
/* EFUSE_SPI_BOOT_CRYPT_CNT : R/W ;bitpos:[20:18] ;default: 3'h0 ; */
/*description: Set this bit to enable SPI boot encrypt/decrypt. Odd number of 1: enable*/
#define EFUSE_SPI_BOOT_CRYPT_CNT  0x00000007
#define EFUSE_SPI_BOOT_CRYPT_CNT_M  ((EFUSE_SPI_BOOT_CRYPT_CNT_V)<<(EFUSE_SPI_BOOT_CRYPT_CNT_S))
#define EFUSE_SPI_BOOT_CRYPT_CNT_V  0x7
#define EFUSE_SPI_BOOT_CRYPT_CNT_S  18
/* EFUSE_WAT_DELAY_SEL : R/W ;bitpos:[17:16] ;default: 2'h0 ; */
/*description: Selects RTC watchdog timeout threshold.*/
#define EFUSE_WAT_DELAY_SEL  0x00000003
#define EFUSE_WAT_DELAY_SEL_M  ((EFUSE_WAT_DELAY_SEL_V)<<(EFUSE_WAT_DELAY_SEL_S))
#define EFUSE_WAT_DELAY_SEL_V  0x3
#define EFUSE_WAT_DELAY_SEL_S  16
/* EFUSE_VDD_SPI_DCAP : R/W ;bitpos:[15:14] ;default: 2'h0 ; */
/*description: Prevents SPI regulator from overshoot.*/
#define EFUSE_VDD_SPI_DCAP  0x00000003
#define EFUSE_VDD_SPI_DCAP_M  ((EFUSE_VDD_SPI_DCAP_V)<<(EFUSE_VDD_SPI_DCAP_S))
#define EFUSE_VDD_SPI_DCAP_V  0x3
#define EFUSE_VDD_SPI_DCAP_S  14
/* EFUSE_VDD_SPI_INIT : R/W ;bitpos:[13:12] ;default: 2'h0 ; */
/*description: Adds resistor from LDO output to ground. 0: no resistance*/
#define EFUSE_VDD_SPI_INIT  0x00000003
#define EFUSE_VDD_SPI_INIT_M  ((EFUSE_VDD_SPI_INIT_V)<<(EFUSE_VDD_SPI_INIT_S))
#define EFUSE_VDD_SPI_INIT_V  0x3
#define EFUSE_VDD_SPI_INIT_S  12
/* EFUSE_VDD_SPI_DCURLIM : R/W ;bitpos:[11:9] ;default: 3'h0 ; */
/*description: Tunes the current limit threshold of SPI regulator when tieh=0
  about 800 mA/(8+d).*/
#define EFUSE_VDD_SPI_DCURLIM  0x00000007
#define EFUSE_VDD_SPI_DCURLIM_M  ((EFUSE_VDD_SPI_DCURLIM_V)<<(EFUSE_VDD_SPI_DCURLIM_S))
#define EFUSE_VDD_SPI_DCURLIM_V  0x7
#define EFUSE_VDD_SPI_DCURLIM_S  9
/* EFUSE_VDD_SPI_ENCURLIM : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: Set SPI regulator to 1 to enable output current limit.*/
#define EFUSE_VDD_SPI_ENCURLIM  (BIT(8))
#define EFUSE_VDD_SPI_ENCURLIM_M  (BIT(8))
#define EFUSE_VDD_SPI_ENCURLIM_V  0x1
#define EFUSE_VDD_SPI_ENCURLIM_S  8
/* EFUSE_VDD_SPI_EN_INIT : R/W ;bitpos:[7] ;default: 1'h0 ; */
/*description: Set SPI regulator to 0 to configure init[1:0]=0.*/
#define EFUSE_VDD_SPI_EN_INIT  (BIT(7))
#define EFUSE_VDD_SPI_EN_INIT_M  (BIT(7))
#define EFUSE_VDD_SPI_EN_INIT_V  0x1
#define EFUSE_VDD_SPI_EN_INIT_S  7
/* EFUSE_VDD_SPI_FORCE : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: Set this bit and force to use the configuration of eFuse to configure VDD_SPI.*/
#define EFUSE_VDD_SPI_FORCE  (BIT(6))
#define EFUSE_VDD_SPI_FORCE_M  (BIT(6))
#define EFUSE_VDD_SPI_FORCE_V  0x1
#define EFUSE_VDD_SPI_FORCE_S  6
/* EFUSE_VDD_SPI_TIEH : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: SPI regulator output is short connected to VDD3P3_RTC_IO.*/
#define EFUSE_VDD_SPI_TIEH  (BIT(5))
#define EFUSE_VDD_SPI_TIEH_M  (BIT(5))
#define EFUSE_VDD_SPI_TIEH_V  0x1
#define EFUSE_VDD_SPI_TIEH_S  5
/* EFUSE_VDD_SPI_XPD : R/W ;bitpos:[4] ;default: 1'h0 ; */
/*description: SPI regulator power up signal.*/
#define EFUSE_VDD_SPI_XPD  (BIT(4))
#define EFUSE_VDD_SPI_XPD_M  (BIT(4))
#define EFUSE_VDD_SPI_XPD_V  0x1
#define EFUSE_VDD_SPI_XPD_S  4
/* EFUSE_VDD_SPI_DREFL : R/W ;bitpos:[3:2] ;default: 2'h0 ; */
/*description: SPI regulator low voltage reference.*/
#define EFUSE_VDD_SPI_DREFL  0x00000003
#define EFUSE_VDD_SPI_DREFL_M  ((EFUSE_VDD_SPI_DREFL_V)<<(EFUSE_VDD_SPI_DREFL_S))
#define EFUSE_VDD_SPI_DREFL_V  0x3
#define EFUSE_VDD_SPI_DREFL_S  2
/* EFUSE_VDD_SPI_DREFM : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: SPI regulator medium voltage reference.*/
#define EFUSE_VDD_SPI_DREFM  0x00000003
#define EFUSE_VDD_SPI_DREFM_M  ((EFUSE_VDD_SPI_DREFM_V)<<(EFUSE_VDD_SPI_DREFM_S))
#define EFUSE_VDD_SPI_DREFM_V  0x3
#define EFUSE_VDD_SPI_DREFM_S  0

#define EFUSE_PGM_DATA3_REG          (DR_REG_EFUSE_BASE + 0x00c)
/* EFUSE_FLASH_TPUW : R/W ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: Configures flash waiting time after power-up  in unit of ms.
 When the value is 15  the waiting time is 30 ms.*/
#define EFUSE_FLASH_TPUW  0x0000000F
#define EFUSE_FLASH_TPUW_M  ((EFUSE_FLASH_TPUW_V)<<(EFUSE_FLASH_TPUW_S))
#define EFUSE_FLASH_TPUW_V  0xF
#define EFUSE_FLASH_TPUW_S  28
/* EFUSE_RPT4_RESERVED1 : RO ;bitpos:[27:22] ;default: 6'h0 ; */
/*description: Reserved (used for four backups method).*/
#define EFUSE_RPT4_RESERVED1  0x0000003F
#define EFUSE_RPT4_RESERVED1_M  ((EFUSE_RPT4_RESERVED1_V)<<(EFUSE_RPT4_RESERVED1_S))
#define EFUSE_RPT4_RESERVED1_V  0x3F
#define EFUSE_RPT4_RESERVED1_S  22
/* EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE : R/W ;bitpos:[21] ;default: 1'h0 ; */
/*description: Set this bit to enable revoking aggressive secure boot.*/
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE  (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_M  (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_V  0x1
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_S  21
/* EFUSE_SECURE_BOOT_EN : R/W ;bitpos:[20] ;default: 1'h0 ; */
/*description: Set this bit to enable secure boot.*/
#define EFUSE_SECURE_BOOT_EN  (BIT(20))
#define EFUSE_SECURE_BOOT_EN_M  (BIT(20))
#define EFUSE_SECURE_BOOT_EN_V  0x1
#define EFUSE_SECURE_BOOT_EN_S  20
/* EFUSE_KEY_PURPOSE_6 : R/W ;bitpos:[19:16] ;default: 4'h0 ; */
/*description: Purpose of Key6. Refer to Table KEY_PURPOSE Values.*/
#define EFUSE_KEY_PURPOSE_6  0x0000000F
#define EFUSE_KEY_PURPOSE_6_M  ((EFUSE_KEY_PURPOSE_6_V)<<(EFUSE_KEY_PURPOSE_6_S))
#define EFUSE_KEY_PURPOSE_6_V  0xF
#define EFUSE_KEY_PURPOSE_6_S  16
/* EFUSE_KEY_PURPOSE_5 : R/W ;bitpos:[15:12] ;default: 4'h0 ; */
/*description: Purpose of Key5. Refer to Table KEY_PURPOSE Values.*/
#define EFUSE_KEY_PURPOSE_5  0x0000000F
#define EFUSE_KEY_PURPOSE_5_M  ((EFUSE_KEY_PURPOSE_5_V)<<(EFUSE_KEY_PURPOSE_5_S))
#define EFUSE_KEY_PURPOSE_5_V  0xF
#define EFUSE_KEY_PURPOSE_5_S  12
/* EFUSE_KEY_PURPOSE_4 : R/W ;bitpos:[11:8] ;default: 4'h0 ; */
/*description: Purpose of Key4. Refer to Table KEY_PURPOSE Values.*/
#define EFUSE_KEY_PURPOSE_4  0x0000000F
#define EFUSE_KEY_PURPOSE_4_M  ((EFUSE_KEY_PURPOSE_4_V)<<(EFUSE_KEY_PURPOSE_4_S))
#define EFUSE_KEY_PURPOSE_4_V  0xF
#define EFUSE_KEY_PURPOSE_4_S  8
/* EFUSE_KEY_PURPOSE_3 : R/W ;bitpos:[7:4] ;default: 4'h0 ; */
/*description: Purpose of Key3. Refer to Table KEY_PURPOSE Values.*/
#define EFUSE_KEY_PURPOSE_3  0x0000000F
#define EFUSE_KEY_PURPOSE_3_M  ((EFUSE_KEY_PURPOSE_3_V)<<(EFUSE_KEY_PURPOSE_3_S))
#define EFUSE_KEY_PURPOSE_3_V  0xF
#define EFUSE_KEY_PURPOSE_3_S  4
/* EFUSE_KEY_PURPOSE_2 : R/W ;bitpos:[3:0] ;default: 4'h0 ; */
/*description: Purpose of Key2. Refer to Table KEY_PURPOSE Values.*/
#define EFUSE_KEY_PURPOSE_2  0x0000000F
#define EFUSE_KEY_PURPOSE_2_M  ((EFUSE_KEY_PURPOSE_2_V)<<(EFUSE_KEY_PURPOSE_2_S))
#define EFUSE_KEY_PURPOSE_2_V  0xF
#define EFUSE_KEY_PURPOSE_2_S  0

#define EFUSE_PGM_DATA4_REG          (DR_REG_EFUSE_BASE + 0x010)
/* EFUSE_RPT4_RESERVED2 : RO ;bitpos:[31:27] ;default: 5'h0 ; */
/*description: Reserved (used for four backups method).*/
#define EFUSE_RPT4_RESERVED2  0x0000001F
#define EFUSE_RPT4_RESERVED2_M  ((EFUSE_RPT4_RESERVED2_V)<<(EFUSE_RPT4_RESERVED2_S))
#define EFUSE_RPT4_RESERVED2_V  0x1F
#define EFUSE_RPT4_RESERVED2_S  27
/* EFUSE_SECURE_VERSION : R/W ;bitpos:[26:11] ;default: 16'h0 ; */
/*description: IDF secure version.*/
#define EFUSE_SECURE_VERSION  0x0000FFFF
#define EFUSE_SECURE_VERSION_M  ((EFUSE_SECURE_VERSION_V)<<(EFUSE_SECURE_VERSION_S))
#define EFUSE_SECURE_VERSION_V  0xFFFF
#define EFUSE_SECURE_VERSION_S  11
/* EFUSE_FORCE_SEND_RESUME : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: Set this bit to force ROM code to send a resume command during SPI boot.*/
#define EFUSE_FORCE_SEND_RESUME  (BIT(10))
#define EFUSE_FORCE_SEND_RESUME_M  (BIT(10))
#define EFUSE_FORCE_SEND_RESUME_V  0x1
#define EFUSE_FORCE_SEND_RESUME_S  10
/* EFUSE_FLASH_TYPE : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: The type of the interfaced flash. 0: four data lines  1: eight data lines.*/
#define EFUSE_FLASH_TYPE  (BIT(9))
#define EFUSE_FLASH_TYPE_M  (BIT(9))
#define EFUSE_FLASH_TYPE_V  0x1
#define EFUSE_FLASH_TYPE_S  9
/* EFUSE_PIN_POWER_SELECTION : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: GPIO33-GPIO37 power supply selection in ROM code. 0: VDD3P3_CPU*/
#define EFUSE_PIN_POWER_SELECTION  (BIT(8))
#define EFUSE_PIN_POWER_SELECTION_M  (BIT(8))
#define EFUSE_PIN_POWER_SELECTION_V  0x1
#define EFUSE_PIN_POWER_SELECTION_S  8
/* EFUSE_UART_PRINT_CONTROL : R/W ;bitpos:[7:6] ;default: 2'h0 ; */
/*description: The type of UART print control.00: Forces to print.01: Controlled
 by GPIO46  print at low level.10: Controlled by GPIO46  print at high level.11: Forces to disable print.*/
#define EFUSE_UART_PRINT_CONTROL  0x00000003
#define EFUSE_UART_PRINT_CONTROL_M  ((EFUSE_UART_PRINT_CONTROL_V)<<(EFUSE_UART_PRINT_CONTROL_S))
#define EFUSE_UART_PRINT_CONTROL_V  0x3
#define EFUSE_UART_PRINT_CONTROL_S  6
/* EFUSE_ENABLE_SECURITY_DOWNLOAD : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: Set this bit to enable security download mode.*/
#define EFUSE_ENABLE_SECURITY_DOWNLOAD  (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_M  (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_V  0x1
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_S  5
/* EFUSE_DIS_USB_DOWNLOAD_MODE : R/W ;bitpos:[4] ;default: 1'h0 ; */
/*description: Set this bit to disable download through USB.*/
#define EFUSE_DIS_USB_DOWNLOAD_MODE  (BIT(4))
#define EFUSE_DIS_USB_DOWNLOAD_MODE_M  (BIT(4))
#define EFUSE_DIS_USB_DOWNLOAD_MODE_V  0x1
#define EFUSE_DIS_USB_DOWNLOAD_MODE_S  4
/* EFUSE_RPT4_RESERVED3 : R/W ;bitpos:[3] ;default: 1'h0 ; */
/*description: Set this bit to disable tiny basic console in ROM.*/
#define EFUSE_RPT4_RESERVED3  (BIT(3))
#define EFUSE_RPT4_RESERVED3_M  (BIT(3))
#define EFUSE_RPT4_RESERVED3_V  0x1
#define EFUSE_RPT4_RESERVED3_S  3
/* EFUSE_UART_PRINT_CHANNEL : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: Selectes UART print channel. 0: UART0*/
#define EFUSE_UART_PRINT_CHANNEL  (BIT(2))
#define EFUSE_UART_PRINT_CHANNEL_M  (BIT(2))
#define EFUSE_UART_PRINT_CHANNEL_V  0x1
#define EFUSE_UART_PRINT_CHANNEL_S  2
/* EFUSE_DIS_LEGACY_SPI_BOOT : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: Set this bit to disable Legacy SPI boot mode (boot_mode[3:0] = 4).*/
#define EFUSE_DIS_LEGACY_SPI_BOOT  (BIT(1))
#define EFUSE_DIS_LEGACY_SPI_BOOT_M  (BIT(1))
#define EFUSE_DIS_LEGACY_SPI_BOOT_V  0x1
#define EFUSE_DIS_LEGACY_SPI_BOOT_S  1
/* EFUSE_DIS_DOWNLOAD_MODE : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: Set this bit to disable download mode (boot_mode[3:0] = 0  1  2  3  6  7).*/
#define EFUSE_DIS_DOWNLOAD_MODE  (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_M  (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_V  0x1
#define EFUSE_DIS_DOWNLOAD_MODE_S  0

#define EFUSE_PGM_DATA5_REG          (DR_REG_EFUSE_BASE + 0x014)
/* EFUSE_RPT1_RESERVED0 : RO ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: Reserved.*/
#define EFUSE_RPT1_RESERVED0  0x000000FF
#define EFUSE_RPT1_RESERVED0_M  ((EFUSE_RPT1_RESERVED0_V)<<(EFUSE_RPT1_RESERVED0_S))
#define EFUSE_RPT1_RESERVED0_V  0xFF
#define EFUSE_RPT1_RESERVED0_S  24
/* EFUSE_RPT4_RESERVED4 : RO ;bitpos:[23:0] ;default: 24'h0 ; */
/*description: Reserved (used for four backups method).*/
#define EFUSE_RPT4_RESERVED4  0x00FFFFFF
#define EFUSE_RPT4_RESERVED4_M  ((EFUSE_RPT4_RESERVED4_V)<<(EFUSE_RPT4_RESERVED4_S))
#define EFUSE_RPT4_RESERVED4_V  0xFFFFFF
#define EFUSE_RPT4_RESERVED4_S  0

#define EFUSE_PGM_DATA6_REG          (DR_REG_EFUSE_BASE + 0x018)
/* EFUSE_PGM_DATA_6 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the sixth 32-bit data to be programmed.*/
#define EFUSE_PGM_DATA_6  0xFFFFFFFF
#define EFUSE_PGM_DATA_6_M  ((EFUSE_PGM_DATA_6_V)<<(EFUSE_PGM_DATA_6_S))
#define EFUSE_PGM_DATA_6_V  0xFFFFFFFF
#define EFUSE_PGM_DATA_6_S  0

#define EFUSE_PGM_DATA7_REG          (DR_REG_EFUSE_BASE + 0x01c)
/* EFUSE_PGM_DATA_7 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The value of pgm data 7.*/
#define EFUSE_PGM_DATA_7  0xFFFFFFFF
#define EFUSE_PGM_DATA_7_M  ((EFUSE_PGM_DATA_7_V)<<(EFUSE_PGM_DATA_7_S))
#define EFUSE_PGM_DATA_7_V  0xFFFFFFFF
#define EFUSE_PGM_DATA_7_S  0

#define EFUSE_PGM_CHECK_VALUE0_REG          (DR_REG_EFUSE_BASE + 0x020)
/* EFUSE_PGM_RS_DATA_0 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the 0th 32-bit RS code to be programmed.*/
#define EFUSE_PGM_RS_DATA_0  0xFFFFFFFF
#define EFUSE_PGM_RS_DATA_0_M  ((EFUSE_PGM_RS_DATA_0_V)<<(EFUSE_PGM_RS_DATA_0_S))
#define EFUSE_PGM_RS_DATA_0_V  0xFFFFFFFF
#define EFUSE_PGM_RS_DATA_0_S  0

#define EFUSE_PGM_CHECK_VALUE1_REG          (DR_REG_EFUSE_BASE + 0x024)
/* EFUSE_PGM_RS_DATA_1 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the first 32-bit RS code to be programmed.*/
#define EFUSE_PGM_RS_DATA_1  0xFFFFFFFF
#define EFUSE_PGM_RS_DATA_1_M  ((EFUSE_PGM_RS_DATA_1_V)<<(EFUSE_PGM_RS_DATA_1_S))
#define EFUSE_PGM_RS_DATA_1_V  0xFFFFFFFF
#define EFUSE_PGM_RS_DATA_1_S  0

#define EFUSE_PGM_CHECK_VALUE2_REG          (DR_REG_EFUSE_BASE + 0x028)
/* EFUSE_PGM_RS_DATA_2 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The content of the second 32-bit RS code to be programmed.*/
#define EFUSE_PGM_RS_DATA_2  0xFFFFFFFF
#define EFUSE_PGM_RS_DATA_2_M  ((EFUSE_PGM_RS_DATA_2_V)<<(EFUSE_PGM_RS_DATA_2_S))
#define EFUSE_PGM_RS_DATA_2_V  0xFFFFFFFF
#define EFUSE_PGM_RS_DATA_2_S  0

#define EFUSE_RD_WR_DIS_REG          (DR_REG_EFUSE_BASE + 0x02c)
/* EFUSE_WR_DIS : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The value of WR_DIS.*/
#define EFUSE_WR_DIS  0xFFFFFFFF
#define EFUSE_WR_DIS_M  ((EFUSE_WR_DIS_V)<<(EFUSE_WR_DIS_S))
#define EFUSE_WR_DIS_V  0xFFFFFFFF
#define EFUSE_WR_DIS_S  0

#define EFUSE_WR_DIS_RD_DIS                        (1<<0)
#define EFUSE_WR_DIS_DIS_RTC_RAM_BOOT              (1<<1)
#define EFUSE_WR_DIS_GROUP_1                       (1<<2)
#define EFUSE_WR_DIS_GROUP_2                       (1<<3)
#define EFUSE_WR_DIS_SPI_BOOT_CRYPT_CNT            (1<<4)
#define EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE0       (1<<5)
#define EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE1       (1<<6)
#define EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE2       (1<<7)
#define EFUSE_WR_DIS_KEY0_PURPOSE                  (1<<8)
#define EFUSE_WR_DIS_KEY1_PURPOSE                  (1<<9)
#define EFUSE_WR_DIS_KEY2_PURPOSE                  (1<<10)
#define EFUSE_WR_DIS_KEY3_PURPOSE                  (1<<11)
#define EFUSE_WR_DIS_KEY4_PURPOSE                  (1<<12)
#define EFUSE_WR_DIS_KEY5_PURPOSE                  (1<<13)
#define EFUSE_WR_DIS_SECURE_BOOT_EN                (1<<15)
#define EFUSE_WR_DIS_SECURE_BOOT_AGGRESSIVE_REVOKE (1<<16)
#define EFUSE_WR_DIS_GROUP_3                       (1<<18)
#define EFUSE_WR_DIS_BLK1                          (1<<20)
#define EFUSE_WR_DIS_SYS_DATA_PART1                (1<<21)
#define EFUSE_WR_DIS_USER_DATA                     (1<<22)
#define EFUSE_WR_DIS_KEY0                          (1<<23)
#define EFUSE_WR_DIS_KEY1                          (1<<24)
#define EFUSE_WR_DIS_KEY2                          (1<<25)
#define EFUSE_WR_DIS_KEY3                          (1<<26)
#define EFUSE_WR_DIS_KEY4                          (1<<27)
#define EFUSE_WR_DIS_KEY5                          (1<<28)
#define EFUSE_WR_DIS_SYS_DATA_PART2                (1<<29)
#define EFUSE_WR_DIS_USB_EXCHG_PINS                (1<<30)


#define EFUSE_RD_REPEAT_DATA0_REG          (DR_REG_EFUSE_BASE + 0x030)
/* EFUSE_VDD_SPI_DREFH : RO ;bitpos:[31:30] ;default: 2'h0 ; */
/*description: The value of VDD_SPI_DREFH.*/
#define EFUSE_VDD_SPI_DREFH  0x00000003
#define EFUSE_VDD_SPI_DREFH_M  ((EFUSE_VDD_SPI_DREFH_V)<<(EFUSE_VDD_SPI_DREFH_S))
#define EFUSE_VDD_SPI_DREFH_V  0x3
#define EFUSE_VDD_SPI_DREFH_S  30
/* EFUSE_VDD_SPI_MODECURLIM : RO ;bitpos:[29] ;default: 1'h0 ; */
/*description: The value of VDD_SPI_MODECURLIM.*/
#define EFUSE_VDD_SPI_MODECURLIM  (BIT(29))
#define EFUSE_VDD_SPI_MODECURLIM_M  (BIT(29))
#define EFUSE_VDD_SPI_MODECURLIM_V  0x1
#define EFUSE_VDD_SPI_MODECURLIM_S  29
/* EFUSE_RPT4_RESERVED0 : RO ;bitpos:[28:27] ;default: 2'h0 ; */
/*description: Reserved.*/
#define EFUSE_RPT4_RESERVED0  0x00000003
#define EFUSE_RPT4_RESERVED0_M  ((EFUSE_RPT4_RESERVED0_V)<<(EFUSE_RPT4_RESERVED0_S))
#define EFUSE_RPT4_RESERVED0_V  0x3
#define EFUSE_RPT4_RESERVED0_S  27
/* EFUSE_USB_FORCE_NOPERSIST : RO ;bitpos:[26] ;default: 1'h0 ; */
/*description: The value of usb_force_nopersist*/
#define EFUSE_USB_FORCE_NOPERSIST  (BIT(26))
#define EFUSE_USB_FORCE_NOPERSIST_M  (BIT(26))
#define EFUSE_USB_FORCE_NOPERSIST_V  0x1
#define EFUSE_USB_FORCE_NOPERSIST_S  26
/* EFUSE_USB_EXT_PHY_ENABLE : RO ;bitpos:[25] ;default: 1'h0 ; */
/*description: The value of EXT_PHY_ENABLE.*/
#define EFUSE_USB_EXT_PHY_ENABLE  (BIT(25))
#define EFUSE_USB_EXT_PHY_ENABLE_M  (BIT(25))
#define EFUSE_USB_EXT_PHY_ENABLE_V  0x1
#define EFUSE_USB_EXT_PHY_ENABLE_S  25
/* EFUSE_USB_EXCHG_PINS : RO ;bitpos:[24] ;default: 1'h0 ; */
/*description: Reserved.*/
#define EFUSE_USB_EXCHG_PINS  (BIT(24))
#define EFUSE_USB_EXCHG_PINS_M  (BIT(24))
#define EFUSE_USB_EXCHG_PINS_V  0x1
#define EFUSE_USB_EXCHG_PINS_S  24
/* EFUSE_USB_DREFL : RO ;bitpos:[23:22] ;default: 2'h0 ; */
/*description: The value of USB_DREFL.*/
#define EFUSE_USB_DREFL  0x00000003
#define EFUSE_USB_DREFL_M  ((EFUSE_USB_DREFL_V)<<(EFUSE_USB_DREFL_S))
#define EFUSE_USB_DREFL_V  0x3
#define EFUSE_USB_DREFL_S  22
/* EFUSE_USB_DREFH : RO ;bitpos:[21:20] ;default: 2'h0 ; */
/*description: The value of USB_DREFH.*/
#define EFUSE_USB_DREFH  0x00000003
#define EFUSE_USB_DREFH_M  ((EFUSE_USB_DREFH_V)<<(EFUSE_USB_DREFH_S))
#define EFUSE_USB_DREFH_V  0x3
#define EFUSE_USB_DREFH_S  20
/* EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT : RO ;bitpos:[19] ;default: 1'h0 ; */
/*description: The value of DIS_DOWNLOAD_MANUAL_ENCRYPT.*/
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT  (BIT(19))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_M  (BIT(19))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_V  0x1
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_S  19
/* EFUSE_HARD_DIS_JTAG : RO ;bitpos:[18] ;default: 1'h0 ; */
/*description: The value of HARD_DIS_JTAG.*/
#define EFUSE_HARD_DIS_JTAG  (BIT(18))
#define EFUSE_HARD_DIS_JTAG_M  (BIT(18))
#define EFUSE_HARD_DIS_JTAG_V  0x1
#define EFUSE_HARD_DIS_JTAG_S  18
/* EFUSE_SOFT_DIS_JTAG : RO ;bitpos:[17] ;default: 1'h0 ; */
/*description: The value of SOFT_DIS_JTAG.*/
#define EFUSE_SOFT_DIS_JTAG  (BIT(17))
#define EFUSE_SOFT_DIS_JTAG_M  (BIT(17))
#define EFUSE_SOFT_DIS_JTAG_V  0x1
#define EFUSE_SOFT_DIS_JTAG_S  17
/* EFUSE_DIS_EFUSE_ATE_WR : RO ;bitpos:[16] ;default: 1'h0 ; */
/*description: The value of DIS_EFUSE_ATE_WR.*/
#define EFUSE_DIS_EFUSE_ATE_WR  (BIT(16))
#define EFUSE_DIS_EFUSE_ATE_WR_M  (BIT(16))
#define EFUSE_DIS_EFUSE_ATE_WR_V  0x1
#define EFUSE_DIS_EFUSE_ATE_WR_S  16
/* EFUSE_DIS_BOOT_REMAP : RO ;bitpos:[15] ;default: 1'h0 ; */
/*description: The value of DIS_BOOT_REMAP.*/
#define EFUSE_DIS_BOOT_REMAP  (BIT(15))
#define EFUSE_DIS_BOOT_REMAP_M  (BIT(15))
#define EFUSE_DIS_BOOT_REMAP_V  0x1
#define EFUSE_DIS_BOOT_REMAP_S  15
/* EFUSE_DIS_CAN : RO ;bitpos:[14] ;default: 1'h0 ; */
/*description: The value of DIS_CAN.*/
#define EFUSE_DIS_CAN  (BIT(14))
#define EFUSE_DIS_CAN_M  (BIT(14))
#define EFUSE_DIS_CAN_V  0x1
#define EFUSE_DIS_CAN_S  14
/* EFUSE_DIS_USB : RO ;bitpos:[13] ;default: 1'h0 ; */
/*description: The value of DIS_USB.*/
#define EFUSE_DIS_USB  (BIT(13))
#define EFUSE_DIS_USB_M  (BIT(13))
#define EFUSE_DIS_USB_V  0x1
#define EFUSE_DIS_USB_S  13
/* EFUSE_DIS_FORCE_DOWNLOAD : RO ;bitpos:[12] ;default: 1'h0 ; */
/*description: The value of DIS_FORCE_DOWNLOAD.*/
#define EFUSE_DIS_FORCE_DOWNLOAD  (BIT(12))
#define EFUSE_DIS_FORCE_DOWNLOAD_M  (BIT(12))
#define EFUSE_DIS_FORCE_DOWNLOAD_V  0x1
#define EFUSE_DIS_FORCE_DOWNLOAD_S  12
/* EFUSE_DIS_DOWNLOAD_DCACHE : RO ;bitpos:[11] ;default: 1'h0 ; */
/*description: The value of DIS_DOWNLOAD_DCACHE.*/
#define EFUSE_DIS_DOWNLOAD_DCACHE  (BIT(11))
#define EFUSE_DIS_DOWNLOAD_DCACHE_M  (BIT(11))
#define EFUSE_DIS_DOWNLOAD_DCACHE_V  0x1
#define EFUSE_DIS_DOWNLOAD_DCACHE_S  11
/* EFUSE_DIS_DOWNLOAD_ICACHE : RO ;bitpos:[10] ;default: 1'h0 ; */
/*description: The value of DIS_DOWNLOAD_ICACHE.*/
#define EFUSE_DIS_DOWNLOAD_ICACHE  (BIT(10))
#define EFUSE_DIS_DOWNLOAD_ICACHE_M  (BIT(10))
#define EFUSE_DIS_DOWNLOAD_ICACHE_V  0x1
#define EFUSE_DIS_DOWNLOAD_ICACHE_S  10
/* EFUSE_DIS_DCACHE : RO ;bitpos:[9] ;default: 1'h0 ; */
/*description: The value of DIS_DCACHE.*/
#define EFUSE_DIS_DCACHE  (BIT(9))
#define EFUSE_DIS_DCACHE_M  (BIT(9))
#define EFUSE_DIS_DCACHE_V  0x1
#define EFUSE_DIS_DCACHE_S  9
/* EFUSE_DIS_ICACHE : RO ;bitpos:[8] ;default: 1'h0 ; */
/*description: The value of DIS_ICACHE.*/
#define EFUSE_DIS_ICACHE  (BIT(8))
#define EFUSE_DIS_ICACHE_M  (BIT(8))
#define EFUSE_DIS_ICACHE_V  0x1
#define EFUSE_DIS_ICACHE_S  8
/* EFUSE_DIS_RTC_RAM_BOOT : RO ;bitpos:[7] ;default: 1'h0 ; */
/*description: The value of DIS_RTC_RAM_BOOT.*/
#define EFUSE_DIS_RTC_RAM_BOOT  (BIT(7))
#define EFUSE_DIS_RTC_RAM_BOOT_M  (BIT(7))
#define EFUSE_DIS_RTC_RAM_BOOT_V  0x1
#define EFUSE_DIS_RTC_RAM_BOOT_S  7
/* EFUSE_RD_DIS : RO ;bitpos:[6:0] ;default: 7'h0 ; */
/*description: The value of RD_DIS.*/
#define EFUSE_RD_DIS  0x0000007F
#define EFUSE_RD_DIS_M  ((EFUSE_RD_DIS_V)<<(EFUSE_RD_DIS_S))
#define EFUSE_RD_DIS_V  0x7F
#define EFUSE_RD_DIS_S  0

#define EFUSE_RD_REPEAT_DATA1_REG          (DR_REG_EFUSE_BASE + 0x034)
/* EFUSE_KEY_PURPOSE_1 : RO ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: The value of KEY_PURPOSE_1.*/
#define EFUSE_KEY_PURPOSE_1  0x0000000F
#define EFUSE_KEY_PURPOSE_1_M  ((EFUSE_KEY_PURPOSE_1_V)<<(EFUSE_KEY_PURPOSE_1_S))
#define EFUSE_KEY_PURPOSE_1_V  0xF
#define EFUSE_KEY_PURPOSE_1_S  28
/* EFUSE_KEY_PURPOSE_0 : RO ;bitpos:[27:24] ;default: 4'h0 ; */
/*description: The value of KEY_PURPOSE_0.*/
#define EFUSE_KEY_PURPOSE_0  0x0000000F
#define EFUSE_KEY_PURPOSE_0_M  ((EFUSE_KEY_PURPOSE_0_V)<<(EFUSE_KEY_PURPOSE_0_S))
#define EFUSE_KEY_PURPOSE_0_V  0xF
#define EFUSE_KEY_PURPOSE_0_S  24
/* EFUSE_SECURE_BOOT_KEY_REVOKE2 : RO ;bitpos:[23] ;default: 1'h0 ; */
/*description: The value of SECURE_BOOT_KEY_REVOKE2.*/
#define EFUSE_SECURE_BOOT_KEY_REVOKE2  (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_M  (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_S  23
/* EFUSE_SECURE_BOOT_KEY_REVOKE1 : RO ;bitpos:[22] ;default: 1'h0 ; */
/*description: The value of SECURE_BOOT_KEY_REVOKE1.*/
#define EFUSE_SECURE_BOOT_KEY_REVOKE1  (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_M  (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_S  22
/* EFUSE_SECURE_BOOT_KEY_REVOKE0 : RO ;bitpos:[21] ;default: 1'h0 ; */
/*description: The value of SECURE_BOOT_KEY_REVOKE0.*/
#define EFUSE_SECURE_BOOT_KEY_REVOKE0  (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_M  (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_S  21
/* EFUSE_SPI_BOOT_CRYPT_CNT : RO ;bitpos:[20:18] ;default: 3'h0 ; */
/*description: The value of SPI_BOOT_CRYPT_CNT.*/
#define EFUSE_SPI_BOOT_CRYPT_CNT  0x00000007
#define EFUSE_SPI_BOOT_CRYPT_CNT_M  ((EFUSE_SPI_BOOT_CRYPT_CNT_V)<<(EFUSE_SPI_BOOT_CRYPT_CNT_S))
#define EFUSE_SPI_BOOT_CRYPT_CNT_V  0x7
#define EFUSE_SPI_BOOT_CRYPT_CNT_S  18
/* EFUSE_WDT_DELAY_SEL : RO ;bitpos:[17:16] ;default: 2'h0 ; */
/*description: The value of WDT_DELAY_SEL.*/
#define EFUSE_WDT_DELAY_SEL  0x00000003
#define EFUSE_WDT_DELAY_SEL_M  ((EFUSE_WDT_DELAY_SEL_V)<<(EFUSE_WDT_DELAY_SEL_S))
#define EFUSE_WDT_DELAY_SEL_V  0x3
#define EFUSE_WDT_DELAY_SEL_S  16
/* EFUSE_VDD_SPI_DCAP : RO ;bitpos:[15:14] ;default: 2'h0 ; */
/*description: The value of REG_VDD_SPI_DCAP.*/
#define EFUSE_VDD_SPI_DCAP  0x00000003
#define EFUSE_VDD_SPI_DCAP_M  ((EFUSE_VDD_SPI_DCAP_V)<<(EFUSE_VDD_SPI_DCAP_S))
#define EFUSE_VDD_SPI_DCAP_V  0x3
#define EFUSE_VDD_SPI_DCAP_S  14
/* EFUSE_VDD_SPI_INIT : RO ;bitpos:[13:12] ;default: 2'h0 ; */
/*description: The value of VDD_SPI_INIT.*/
#define EFUSE_VDD_SPI_INIT  0x00000003
#define EFUSE_VDD_SPI_INIT_M  ((EFUSE_VDD_SPI_INIT_V)<<(EFUSE_VDD_SPI_INIT_S))
#define EFUSE_VDD_SPI_INIT_V  0x3
#define EFUSE_VDD_SPI_INIT_S  12
/* EFUSE_VDD_SPI_DCURLIM : RO ;bitpos:[11:9] ;default: 3'h0 ; */
/*description: The value of VDD_SPI_DCURLIM.*/
#define EFUSE_VDD_SPI_DCURLIM  0x00000007
#define EFUSE_VDD_SPI_DCURLIM_M  ((EFUSE_VDD_SPI_DCURLIM_V)<<(EFUSE_VDD_SPI_DCURLIM_S))
#define EFUSE_VDD_SPI_DCURLIM_V  0x7
#define EFUSE_VDD_SPI_DCURLIM_S  9
/* EFUSE_VDD_SPI_ENCURLIM : RO ;bitpos:[8] ;default: 1'h0 ; */
/*description: The value of VDD_SPI_ENCURLIM.*/
#define EFUSE_VDD_SPI_ENCURLIM  (BIT(8))
#define EFUSE_VDD_SPI_ENCURLIM_M  (BIT(8))
#define EFUSE_VDD_SPI_ENCURLIM_V  0x1
#define EFUSE_VDD_SPI_ENCURLIM_S  8
/* EFUSE_VDD_SPI_EN_INIT : RO ;bitpos:[7] ;default: 1'h0 ; */
/*description: The value of VDD_SPI_EN_INIT.*/
#define EFUSE_VDD_SPI_EN_INIT  (BIT(7))
#define EFUSE_VDD_SPI_EN_INIT_M  (BIT(7))
#define EFUSE_VDD_SPI_EN_INIT_V  0x1
#define EFUSE_VDD_SPI_EN_INIT_S  7
/* EFUSE_VDD_SPI_FORCE : RO ;bitpos:[6] ;default: 1'h0 ; */
/*description: The value of VDD_SPI_FORCE.*/
#define EFUSE_VDD_SPI_FORCE  (BIT(6))
#define EFUSE_VDD_SPI_FORCE_M  (BIT(6))
#define EFUSE_VDD_SPI_FORCE_V  0x1
#define EFUSE_VDD_SPI_FORCE_S  6
/* EFUSE_VDD_SPI_TIEH : RO ;bitpos:[5] ;default: 1'h0 ; */
/*description: The value of VDD_SPI_TIEH.*/
#define EFUSE_VDD_SPI_TIEH  (BIT(5))
#define EFUSE_VDD_SPI_TIEH_M  (BIT(5))
#define EFUSE_VDD_SPI_TIEH_V  0x1
#define EFUSE_VDD_SPI_TIEH_S  5
/* EFUSE_VDD_SPI_XPD : RO ;bitpos:[4] ;default: 1'h0 ; */
/*description: The value of VDD_SPI_XPD.*/
#define EFUSE_VDD_SPI_XPD  (BIT(4))
#define EFUSE_VDD_SPI_XPD_M  (BIT(4))
#define EFUSE_VDD_SPI_XPD_V  0x1
#define EFUSE_VDD_SPI_XPD_S  4
/* EFUSE_VDD_SPI_DREFL : RO ;bitpos:[3:2] ;default: 2'h0 ; */
/*description: The value of VDD_SPI_DREFL.*/
#define EFUSE_VDD_SPI_DREFL  0x00000003
#define EFUSE_VDD_SPI_DREFL_M  ((EFUSE_VDD_SPI_DREFL_V)<<(EFUSE_VDD_SPI_DREFL_S))
#define EFUSE_VDD_SPI_DREFL_V  0x3
#define EFUSE_VDD_SPI_DREFL_S  2
/* EFUSE_VDD_SPI_DREFM : RO ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: The value of VDD_SPI_DREFM.*/
#define EFUSE_VDD_SPI_DREFM  0x00000003
#define EFUSE_VDD_SPI_DREFM_M  ((EFUSE_VDD_SPI_DREFM_V)<<(EFUSE_VDD_SPI_DREFM_S))
#define EFUSE_VDD_SPI_DREFM_V  0x3
#define EFUSE_VDD_SPI_DREFM_S  0

#define EFUSE_RD_REPEAT_DATA2_REG          (DR_REG_EFUSE_BASE + 0x038)
/* EFUSE_FLASH_TPUW : RO ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: The value of FLASH_TPUW.*/
#define EFUSE_FLASH_TPUW  0x0000000F
#define EFUSE_FLASH_TPUW_M  ((EFUSE_FLASH_TPUW_V)<<(EFUSE_FLASH_TPUW_S))
#define EFUSE_FLASH_TPUW_V  0xF
#define EFUSE_FLASH_TPUW_S  28
/* EFUSE_RPT4_RESERVED1 : RO ;bitpos:[27:22] ;default: 6'h0 ; */
/*description: Reserved.*/
#define EFUSE_RPT4_RESERVED1  0x0000003F
#define EFUSE_RPT4_RESERVED1_M  ((EFUSE_RPT4_RESERVED1_V)<<(EFUSE_RPT4_RESERVED1_S))
#define EFUSE_RPT4_RESERVED1_V  0x3F
#define EFUSE_RPT4_RESERVED1_S  22
/* EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE : RO ;bitpos:[21] ;default: 1'h0 ; */
/*description: The value of SECURE_BOOT_AGGRESSIVE_REVOKE.*/
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE  (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_M  (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_V  0x1
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_S  21
/* EFUSE_SECURE_BOOT_EN : RO ;bitpos:[20] ;default: 1'h0 ; */
/*description: The value of SECURE_BOOT_EN.*/
#define EFUSE_SECURE_BOOT_EN  (BIT(20))
#define EFUSE_SECURE_BOOT_EN_M  (BIT(20))
#define EFUSE_SECURE_BOOT_EN_V  0x1
#define EFUSE_SECURE_BOOT_EN_S  20
/* EFUSE_KEY_PURPOSE_6 : RO ;bitpos:[19:16] ;default: 4'h0 ; */
/*description: The value of KEY_PURPOSE_6.*/
#define EFUSE_KEY_PURPOSE_6  0x0000000F
#define EFUSE_KEY_PURPOSE_6_M  ((EFUSE_KEY_PURPOSE_6_V)<<(EFUSE_KEY_PURPOSE_6_S))
#define EFUSE_KEY_PURPOSE_6_V  0xF
#define EFUSE_KEY_PURPOSE_6_S  16
/* EFUSE_KEY_PURPOSE_5 : RO ;bitpos:[15:12] ;default: 4'h0 ; */
/*description: The value of KEY_PURPOSE_5.*/
#define EFUSE_KEY_PURPOSE_5  0x0000000F
#define EFUSE_KEY_PURPOSE_5_M  ((EFUSE_KEY_PURPOSE_5_V)<<(EFUSE_KEY_PURPOSE_5_S))
#define EFUSE_KEY_PURPOSE_5_V  0xF
#define EFUSE_KEY_PURPOSE_5_S  12
/* EFUSE_KEY_PURPOSE_4 : RO ;bitpos:[11:8] ;default: 4'h0 ; */
/*description: The value of KEY_PURPOSE_4.*/
#define EFUSE_KEY_PURPOSE_4  0x0000000F
#define EFUSE_KEY_PURPOSE_4_M  ((EFUSE_KEY_PURPOSE_4_V)<<(EFUSE_KEY_PURPOSE_4_S))
#define EFUSE_KEY_PURPOSE_4_V  0xF
#define EFUSE_KEY_PURPOSE_4_S  8
/* EFUSE_KEY_PURPOSE_3 : RO ;bitpos:[7:4] ;default: 4'h0 ; */
/*description: The value of KEY_PURPOSE_3.*/
#define EFUSE_KEY_PURPOSE_3  0x0000000F
#define EFUSE_KEY_PURPOSE_3_M  ((EFUSE_KEY_PURPOSE_3_V)<<(EFUSE_KEY_PURPOSE_3_S))
#define EFUSE_KEY_PURPOSE_3_V  0xF
#define EFUSE_KEY_PURPOSE_3_S  4
/* EFUSE_KEY_PURPOSE_2 : RO ;bitpos:[3:0] ;default: 4'h0 ; */
/*description: The value of KEY_PURPOSE_2.*/
#define EFUSE_KEY_PURPOSE_2  0x0000000F
#define EFUSE_KEY_PURPOSE_2_M  ((EFUSE_KEY_PURPOSE_2_V)<<(EFUSE_KEY_PURPOSE_2_S))
#define EFUSE_KEY_PURPOSE_2_V  0xF
#define EFUSE_KEY_PURPOSE_2_S  0

#define EFUSE_RD_REPEAT_DATA3_REG          (DR_REG_EFUSE_BASE + 0x03c)
/* EFUSE_RPT4_RESERVED2 : RO ;bitpos:[31:27] ;default: 5'h0 ; */
/*description: Reserved.*/
#define EFUSE_RPT4_RESERVED2  0x0000001F
#define EFUSE_RPT4_RESERVED2_M  ((EFUSE_RPT4_RESERVED2_V)<<(EFUSE_RPT4_RESERVED2_S))
#define EFUSE_RPT4_RESERVED2_V  0x1F
#define EFUSE_RPT4_RESERVED2_S  27
/* EFUSE_SECURE_VERSION : RO ;bitpos:[26:11] ;default: 16'h0 ; */
/*description: The value of SECURE_VERSION.*/
#define EFUSE_SECURE_VERSION  0x0000FFFF
#define EFUSE_SECURE_VERSION_M  ((EFUSE_SECURE_VERSION_V)<<(EFUSE_SECURE_VERSION_S))
#define EFUSE_SECURE_VERSION_V  0xFFFF
#define EFUSE_SECURE_VERSION_S  11
/* EFUSE_FORCE_SEND_RESUME : RO ;bitpos:[10] ;default: 1'h0 ; */
/*description: The value of FORCE_SEND_RESUME.*/
#define EFUSE_FORCE_SEND_RESUME  (BIT(10))
#define EFUSE_FORCE_SEND_RESUME_M  (BIT(10))
#define EFUSE_FORCE_SEND_RESUME_V  0x1
#define EFUSE_FORCE_SEND_RESUME_S  10
/* EFUSE_FLASH_TYPE : RO ;bitpos:[9] ;default: 1'h0 ; */
/*description: The value of FLASH_TYPE.*/
#define EFUSE_FLASH_TYPE  (BIT(9))
#define EFUSE_FLASH_TYPE_M  (BIT(9))
#define EFUSE_FLASH_TYPE_V  0x1
#define EFUSE_FLASH_TYPE_S  9
/* EFUSE_PIN_POWER_SELECTION : RO ;bitpos:[8] ;default: 1'h0 ; */
/*description: The value of PIN_POWER_SELECTION.*/
#define EFUSE_PIN_POWER_SELECTION  (BIT(8))
#define EFUSE_PIN_POWER_SELECTION_M  (BIT(8))
#define EFUSE_PIN_POWER_SELECTION_V  0x1
#define EFUSE_PIN_POWER_SELECTION_S  8
/* EFUSE_UART_PRINT_CONTROL : RO ;bitpos:[7:6] ;default: 2'h0 ; */
/*description: The value of UART_PRINT_CONTROL.*/
#define EFUSE_UART_PRINT_CONTROL  0x00000003
#define EFUSE_UART_PRINT_CONTROL_M  ((EFUSE_UART_PRINT_CONTROL_V)<<(EFUSE_UART_PRINT_CONTROL_S))
#define EFUSE_UART_PRINT_CONTROL_V  0x3
#define EFUSE_UART_PRINT_CONTROL_S  6
/* EFUSE_ENABLE_SECURITY_DOWNLOAD : RO ;bitpos:[5] ;default: 1'h0 ; */
/*description: The value of ENABLE_SECURITY_DOWNLOAD.*/
#define EFUSE_ENABLE_SECURITY_DOWNLOAD  (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_M  (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_V  0x1
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_S  5
/* EFUSE_DIS_USB_DOWNLOAD_MODE : RO ;bitpos:[4] ;default: 1'h0 ; */
/*description: The value of DIS_USB_DOWNLOAD_MODE.*/
#define EFUSE_DIS_USB_DOWNLOAD_MODE  (BIT(4))
#define EFUSE_DIS_USB_DOWNLOAD_MODE_M  (BIT(4))
#define EFUSE_DIS_USB_DOWNLOAD_MODE_V  0x1
#define EFUSE_DIS_USB_DOWNLOAD_MODE_S  4
/* EFUSE_RPT4_RESERVED3 : RO ;bitpos:[3] ;default: 1'h0 ; */
/*description: The value of RPT4_RESERVED4.*/
#define EFUSE_RPT4_RESERVED3  (BIT(3))
#define EFUSE_RPT4_RESERVED3_M  (BIT(3))
#define EFUSE_RPT4_RESERVED3_V  0x1
#define EFUSE_RPT4_RESERVED3_S  3
/* EFUSE_UART_PRINT_CHANNEL : RO ;bitpos:[2] ;default: 1'h0 ; */
/*description: The value of UART_PRINT_CHANNEL.*/
#define EFUSE_UART_PRINT_CHANNEL  (BIT(2))
#define EFUSE_UART_PRINT_CHANNEL_M  (BIT(2))
#define EFUSE_UART_PRINT_CHANNEL_V  0x1
#define EFUSE_UART_PRINT_CHANNEL_S  2
/* EFUSE_DIS_LEGACY_SPI_BOOT : RO ;bitpos:[1] ;default: 1'h0 ; */
/*description: The value of DIS_LEGACY_SPI_BOOT.*/
#define EFUSE_DIS_LEGACY_SPI_BOOT  (BIT(1))
#define EFUSE_DIS_LEGACY_SPI_BOOT_M  (BIT(1))
#define EFUSE_DIS_LEGACY_SPI_BOOT_V  0x1
#define EFUSE_DIS_LEGACY_SPI_BOOT_S  1
/* EFUSE_DIS_DOWNLOAD_MODE : RO ;bitpos:[0] ;default: 1'h0 ; */
/*description: The value of DIS_DOWNLOAD_MODE.*/
#define EFUSE_DIS_DOWNLOAD_MODE  (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_M  (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_V  0x1
#define EFUSE_DIS_DOWNLOAD_MODE_S  0

#define EFUSE_RD_REPEAT_DATA4_REG          (DR_REG_EFUSE_BASE + 0x040)
/* EFUSE_RPT1_RESERVED0 : RO ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: Reserved.*/
#define EFUSE_RPT1_RESERVED0  0x000000FF
#define EFUSE_RPT1_RESERVED0_M  ((EFUSE_RPT1_RESERVED0_V)<<(EFUSE_RPT1_RESERVED0_S))
#define EFUSE_RPT1_RESERVED0_V  0xFF
#define EFUSE_RPT1_RESERVED0_S  24
/* EFUSE_RPT4_RESERVED4 : RO ;bitpos:[23:0] ;default: 24'h0 ; */
/*description: Reserved ( four backup method ).*/
#define EFUSE_RPT4_RESERVED4  0x00FFFFFF
#define EFUSE_RPT4_RESERVED4_M  ((EFUSE_RPT4_RESERVED4_V)<<(EFUSE_RPT4_RESERVED4_S))
#define EFUSE_RPT4_RESERVED4_V  0xFFFFFF
#define EFUSE_RPT4_RESERVED4_S  0

#define EFUSE_RD_MAC_SPI_SYS_0_REG          (DR_REG_EFUSE_BASE + 0x044)
/* EFUSE_MAC_0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the low 32 bits of MAC address.*/
#define EFUSE_MAC_0  0xFFFFFFFF
#define EFUSE_MAC_0_M  ((EFUSE_MAC_0_V)<<(EFUSE_MAC_0_S))
#define EFUSE_MAC_0_V  0xFFFFFFFF
#define EFUSE_MAC_0_S  0

#define EFUSE_RD_MAC_SPI_SYS_1_REG          (DR_REG_EFUSE_BASE + 0x048)
/* EFUSE_SPI_PAD_CONF_0 : RO ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: Stores the zeroth part of SPI_PAD_CONF.*/
#define EFUSE_SPI_PAD_CONF_0  0x0000FFFF
#define EFUSE_SPI_PAD_CONF_0_M  ((EFUSE_SPI_PAD_CONF_0_V)<<(EFUSE_SPI_PAD_CONF_0_S))
#define EFUSE_SPI_PAD_CONF_0_V  0xFFFF
#define EFUSE_SPI_PAD_CONF_0_S  16
/* EFUSE_MAC_1 : RO ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: Stores the high 16 bits of MAC address.*/
#define EFUSE_MAC_1  0x0000FFFF
#define EFUSE_MAC_1_M  ((EFUSE_MAC_1_V)<<(EFUSE_MAC_1_S))
#define EFUSE_MAC_1_V  0xFFFF
#define EFUSE_MAC_1_S  0

#define EFUSE_RD_MAC_SPI_SYS_2_REG          (DR_REG_EFUSE_BASE + 0x04c)
/* EFUSE_SPI_PAD_CONF_1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first part of SPI_PAD_CONF.*/
#define EFUSE_SPI_PAD_CONF_1  0xFFFFFFFF
#define EFUSE_SPI_PAD_CONF_1_M  ((EFUSE_SPI_PAD_CONF_1_V)<<(EFUSE_SPI_PAD_CONF_1_S))
#define EFUSE_SPI_PAD_CONF_1_V  0xFFFFFFFF
#define EFUSE_SPI_PAD_CONF_1_S  0

#define EFUSE_RD_MAC_SPI_SYS_3_REG          (DR_REG_EFUSE_BASE + 0x050)
/* EFUSE_SYS_DATA_PART0_0 : RO ;bitpos:[31:25] ;default: 7'h0 ; */
/*description: Stores the fist 7 bits of the zeroth part of system data.*/
#define EFUSE_SYS_DATA_PART0_0  0x0000007F
#define EFUSE_SYS_DATA_PART0_0_M  ((EFUSE_SYS_DATA_PART0_0_V)<<(EFUSE_SYS_DATA_PART0_0_S))
#define EFUSE_SYS_DATA_PART0_0_V  0x7F
#define EFUSE_SYS_DATA_PART0_0_S  25
/* EFUSE_PKG_VERSION : RO ;bitpos:[24:21] ;default: 4'h0 ; */
/*description: Package version 0:ESP32-S2, 1:ESP32-S2FH16, 2:ESP32-S2FH32 */
#define EFUSE_PKG_VERSION  0x0000000F
#define EFUSE_PKG_VERSION_M  ((EFUSE_PKG_VERSION_V)<<(EFUSE_PKG_VERSION_S))
#define EFUSE_PKG_VERSION_V  0xF
#define EFUSE_PKG_VERSION_S  21
/* EFUSE_WAFER_VERSION : RO ;bitpos:[20:18] ;default: 3'h0 ; */
/*description: WAFER version 0:A */
#define EFUSE_WAFER_VERSION  0x00000007
#define EFUSE_WAFER_VERSION_M  ((EFUSE_WAFER_VERSION_V)<<(EFUSE_WAFER_VERSION_S))
#define EFUSE_WAFER_VERSION_V  0x7
#define EFUSE_WAFER_VERSION_S  18
/* EFUSE_SPI_PAD_CONF_2 : RO ;bitpos:[17:0] ;default: 18'h0 ; */
/*description: Stores the second part of SPI_PAD_CONF.*/
#define EFUSE_SPI_PAD_CONF_2  0x0003FFFF
#define EFUSE_SPI_PAD_CONF_2_M  ((EFUSE_SPI_PAD_CONF_2_V)<<(EFUSE_SPI_PAD_CONF_2_S))
#define EFUSE_SPI_PAD_CONF_2_V  0x3FFFF
#define EFUSE_SPI_PAD_CONF_2_S  0

#define EFUSE_RD_MAC_SPI_SYS_4_REG          (DR_REG_EFUSE_BASE + 0x054)
/* EFUSE_SYS_DATA_PART0_1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fist 32 bits of the zeroth part of system data.*/
#define EFUSE_SYS_DATA_PART0_1  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART0_1_M  ((EFUSE_SYS_DATA_PART0_1_V)<<(EFUSE_SYS_DATA_PART0_1_S))
#define EFUSE_SYS_DATA_PART0_1_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART0_1_S  0

#define EFUSE_RD_MAC_SPI_SYS_5_REG          (DR_REG_EFUSE_BASE + 0x058)
/* EFUSE_SYS_DATA_PART0_2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of the zeroth part of system data.*/
#define EFUSE_SYS_DATA_PART0_2  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART0_2_M  ((EFUSE_SYS_DATA_PART0_2_V)<<(EFUSE_SYS_DATA_PART0_2_S))
#define EFUSE_SYS_DATA_PART0_2_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART0_2_S  0

#define EFUSE_RD_SYS_PART1_DATA0_REG          (DR_REG_EFUSE_BASE + 0x05c)
/* EFUSE_SYS_DATA_PART1_0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the zeroth 32 bits of the first part of system data.*/
#define EFUSE_SYS_DATA_PART1_0  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_0_M  ((EFUSE_SYS_DATA_PART1_0_V)<<(EFUSE_SYS_DATA_PART1_0_S))
#define EFUSE_SYS_DATA_PART1_0_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_0_S  0

#define EFUSE_RD_SYS_PART1_DATA1_REG          (DR_REG_EFUSE_BASE + 0x060)
/* EFUSE_SYS_DATA_PART1_1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first 32 bits of the first part of system data.*/
#define EFUSE_SYS_DATA_PART1_1  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_1_M  ((EFUSE_SYS_DATA_PART1_1_V)<<(EFUSE_SYS_DATA_PART1_1_S))
#define EFUSE_SYS_DATA_PART1_1_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_1_S  0

#define EFUSE_RD_SYS_PART1_DATA2_REG          (DR_REG_EFUSE_BASE + 0x064)
/* EFUSE_SYS_DATA_PART1_2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of the first part of system data.*/
#define EFUSE_SYS_DATA_PART1_2  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_2_M  ((EFUSE_SYS_DATA_PART1_2_V)<<(EFUSE_SYS_DATA_PART1_2_S))
#define EFUSE_SYS_DATA_PART1_2_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_2_S  0

#define EFUSE_RD_SYS_PART1_DATA3_REG          (DR_REG_EFUSE_BASE + 0x068)
/* EFUSE_SYS_DATA_PART1_3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the third 32 bits of the first part of system data.*/
#define EFUSE_SYS_DATA_PART1_3  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_3_M  ((EFUSE_SYS_DATA_PART1_3_V)<<(EFUSE_SYS_DATA_PART1_3_S))
#define EFUSE_SYS_DATA_PART1_3_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_3_S  0

#define EFUSE_RD_SYS_PART1_DATA4_REG          (DR_REG_EFUSE_BASE + 0x06c)
/* EFUSE_SYS_DATA_PART1_4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fourth 32 bits of the first part of system data.*/
#define EFUSE_SYS_DATA_PART1_4  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_4_M  ((EFUSE_SYS_DATA_PART1_4_V)<<(EFUSE_SYS_DATA_PART1_4_S))
#define EFUSE_SYS_DATA_PART1_4_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_4_S  0

#define EFUSE_RD_SYS_PART1_DATA5_REG          (DR_REG_EFUSE_BASE + 0x070)
/* EFUSE_SYS_DATA_PART1_5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fifth 32 bits of the first part of system data.*/
#define EFUSE_SYS_DATA_PART1_5  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_5_M  ((EFUSE_SYS_DATA_PART1_5_V)<<(EFUSE_SYS_DATA_PART1_5_S))
#define EFUSE_SYS_DATA_PART1_5_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_5_S  0

#define EFUSE_RD_SYS_PART1_DATA6_REG          (DR_REG_EFUSE_BASE + 0x074)
/* EFUSE_SYS_DATA_PART1_6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the sixth 32 bits of the first part of system data.*/
#define EFUSE_SYS_DATA_PART1_6  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_6_M  ((EFUSE_SYS_DATA_PART1_6_V)<<(EFUSE_SYS_DATA_PART1_6_S))
#define EFUSE_SYS_DATA_PART1_6_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_6_S  0

#define EFUSE_RD_SYS_PART1_DATA7_REG          (DR_REG_EFUSE_BASE + 0x078)
/* EFUSE_SYS_DATA_PART1_7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the seventh 32 bits of the first part of system data.*/
#define EFUSE_SYS_DATA_PART1_7  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_7_M  ((EFUSE_SYS_DATA_PART1_7_V)<<(EFUSE_SYS_DATA_PART1_7_S))
#define EFUSE_SYS_DATA_PART1_7_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART1_7_S  0

#define EFUSE_RD_USR_DATA0_REG          (DR_REG_EFUSE_BASE + 0x07c)
/* EFUSE_USR_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the zeroth 32 bits of BLOCK3 (user).*/
#define EFUSE_USR_DATA0  0xFFFFFFFF
#define EFUSE_USR_DATA0_M  ((EFUSE_USR_DATA0_V)<<(EFUSE_USR_DATA0_S))
#define EFUSE_USR_DATA0_V  0xFFFFFFFF
#define EFUSE_USR_DATA0_S  0

#define EFUSE_RD_USR_DATA1_REG          (DR_REG_EFUSE_BASE + 0x080)
/* EFUSE_USR_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first 32 bits of BLOCK3 (user).*/
#define EFUSE_USR_DATA1  0xFFFFFFFF
#define EFUSE_USR_DATA1_M  ((EFUSE_USR_DATA1_V)<<(EFUSE_USR_DATA1_S))
#define EFUSE_USR_DATA1_V  0xFFFFFFFF
#define EFUSE_USR_DATA1_S  0

#define EFUSE_RD_USR_DATA2_REG          (DR_REG_EFUSE_BASE + 0x084)
/* EFUSE_USR_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of BLOCK3 (user).*/
#define EFUSE_USR_DATA2  0xFFFFFFFF
#define EFUSE_USR_DATA2_M  ((EFUSE_USR_DATA2_V)<<(EFUSE_USR_DATA2_S))
#define EFUSE_USR_DATA2_V  0xFFFFFFFF
#define EFUSE_USR_DATA2_S  0

#define EFUSE_RD_USR_DATA3_REG          (DR_REG_EFUSE_BASE + 0x088)
/* EFUSE_USR_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the third 32 bits of BLOCK3 (user).*/
#define EFUSE_USR_DATA3  0xFFFFFFFF
#define EFUSE_USR_DATA3_M  ((EFUSE_USR_DATA3_V)<<(EFUSE_USR_DATA3_S))
#define EFUSE_USR_DATA3_V  0xFFFFFFFF
#define EFUSE_USR_DATA3_S  0

#define EFUSE_RD_USR_DATA4_REG          (DR_REG_EFUSE_BASE + 0x08c)
/* EFUSE_USR_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fourth 32 bits of BLOCK3 (user).*/
#define EFUSE_USR_DATA4  0xFFFFFFFF
#define EFUSE_USR_DATA4_M  ((EFUSE_USR_DATA4_V)<<(EFUSE_USR_DATA4_S))
#define EFUSE_USR_DATA4_V  0xFFFFFFFF
#define EFUSE_USR_DATA4_S  0

#define EFUSE_RD_USR_DATA5_REG          (DR_REG_EFUSE_BASE + 0x090)
/* EFUSE_USR_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fifth 32 bits of BLOCK3 (user).*/
#define EFUSE_USR_DATA5  0xFFFFFFFF
#define EFUSE_USR_DATA5_M  ((EFUSE_USR_DATA5_V)<<(EFUSE_USR_DATA5_S))
#define EFUSE_USR_DATA5_V  0xFFFFFFFF
#define EFUSE_USR_DATA5_S  0

#define EFUSE_RD_USR_DATA6_REG          (DR_REG_EFUSE_BASE + 0x094)
/* EFUSE_USR_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the sixth 32 bits of BLOCK3 (user).*/
#define EFUSE_USR_DATA6  0xFFFFFFFF
#define EFUSE_USR_DATA6_M  ((EFUSE_USR_DATA6_V)<<(EFUSE_USR_DATA6_S))
#define EFUSE_USR_DATA6_V  0xFFFFFFFF
#define EFUSE_USR_DATA6_S  0

#define EFUSE_RD_USR_DATA7_REG          (DR_REG_EFUSE_BASE + 0x098)
/* EFUSE_USR_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the seventh 32 bits of BLOCK3 (user).*/
#define EFUSE_USR_DATA7  0xFFFFFFFF
#define EFUSE_USR_DATA7_M  ((EFUSE_USR_DATA7_V)<<(EFUSE_USR_DATA7_S))
#define EFUSE_USR_DATA7_V  0xFFFFFFFF
#define EFUSE_USR_DATA7_S  0

#define EFUSE_RD_KEY0_DATA0_REG          (DR_REG_EFUSE_BASE + 0x09c)
/* EFUSE_KEY0_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the zeroth 32 bits of KEY0.*/
#define EFUSE_KEY0_DATA0  0xFFFFFFFF
#define EFUSE_KEY0_DATA0_M  ((EFUSE_KEY0_DATA0_V)<<(EFUSE_KEY0_DATA0_S))
#define EFUSE_KEY0_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA0_S  0

#define EFUSE_RD_KEY0_DATA1_REG          (DR_REG_EFUSE_BASE + 0x0a0)
/* EFUSE_KEY0_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first 32 bits of KEY0.*/
#define EFUSE_KEY0_DATA1  0xFFFFFFFF
#define EFUSE_KEY0_DATA1_M  ((EFUSE_KEY0_DATA1_V)<<(EFUSE_KEY0_DATA1_S))
#define EFUSE_KEY0_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA1_S  0

#define EFUSE_RD_KEY0_DATA2_REG          (DR_REG_EFUSE_BASE + 0x0a4)
/* EFUSE_KEY0_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of KEY0.*/
#define EFUSE_KEY0_DATA2  0xFFFFFFFF
#define EFUSE_KEY0_DATA2_M  ((EFUSE_KEY0_DATA2_V)<<(EFUSE_KEY0_DATA2_S))
#define EFUSE_KEY0_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA2_S  0

#define EFUSE_RD_KEY0_DATA3_REG          (DR_REG_EFUSE_BASE + 0x0a8)
/* EFUSE_KEY0_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the third 32 bits of KEY0.*/
#define EFUSE_KEY0_DATA3  0xFFFFFFFF
#define EFUSE_KEY0_DATA3_M  ((EFUSE_KEY0_DATA3_V)<<(EFUSE_KEY0_DATA3_S))
#define EFUSE_KEY0_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA3_S  0

#define EFUSE_RD_KEY0_DATA4_REG          (DR_REG_EFUSE_BASE + 0x0ac)
/* EFUSE_KEY0_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fourth 32 bits of KEY0.*/
#define EFUSE_KEY0_DATA4  0xFFFFFFFF
#define EFUSE_KEY0_DATA4_M  ((EFUSE_KEY0_DATA4_V)<<(EFUSE_KEY0_DATA4_S))
#define EFUSE_KEY0_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA4_S  0

#define EFUSE_RD_KEY0_DATA5_REG          (DR_REG_EFUSE_BASE + 0x0b0)
/* EFUSE_KEY0_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fifth 32 bits of KEY0.*/
#define EFUSE_KEY0_DATA5  0xFFFFFFFF
#define EFUSE_KEY0_DATA5_M  ((EFUSE_KEY0_DATA5_V)<<(EFUSE_KEY0_DATA5_S))
#define EFUSE_KEY0_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA5_S  0

#define EFUSE_RD_KEY0_DATA6_REG          (DR_REG_EFUSE_BASE + 0x0b4)
/* EFUSE_KEY0_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the sixth 32 bits of KEY0.*/
#define EFUSE_KEY0_DATA6  0xFFFFFFFF
#define EFUSE_KEY0_DATA6_M  ((EFUSE_KEY0_DATA6_V)<<(EFUSE_KEY0_DATA6_S))
#define EFUSE_KEY0_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA6_S  0

#define EFUSE_RD_KEY0_DATA7_REG          (DR_REG_EFUSE_BASE + 0x0b8)
/* EFUSE_KEY0_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the seventh 32 bits of KEY0.*/
#define EFUSE_KEY0_DATA7  0xFFFFFFFF
#define EFUSE_KEY0_DATA7_M  ((EFUSE_KEY0_DATA7_V)<<(EFUSE_KEY0_DATA7_S))
#define EFUSE_KEY0_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY0_DATA7_S  0

#define EFUSE_RD_KEY1_DATA0_REG          (DR_REG_EFUSE_BASE + 0x0bc)
/* EFUSE_KEY1_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the zeroth 32 bits of KEY1.*/
#define EFUSE_KEY1_DATA0  0xFFFFFFFF
#define EFUSE_KEY1_DATA0_M  ((EFUSE_KEY1_DATA0_V)<<(EFUSE_KEY1_DATA0_S))
#define EFUSE_KEY1_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA0_S  0

#define EFUSE_RD_KEY1_DATA1_REG          (DR_REG_EFUSE_BASE + 0x0c0)
/* EFUSE_KEY1_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first 32 bits of KEY1.*/
#define EFUSE_KEY1_DATA1  0xFFFFFFFF
#define EFUSE_KEY1_DATA1_M  ((EFUSE_KEY1_DATA1_V)<<(EFUSE_KEY1_DATA1_S))
#define EFUSE_KEY1_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA1_S  0

#define EFUSE_RD_KEY1_DATA2_REG          (DR_REG_EFUSE_BASE + 0x0c4)
/* EFUSE_KEY1_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of KEY1.*/
#define EFUSE_KEY1_DATA2  0xFFFFFFFF
#define EFUSE_KEY1_DATA2_M  ((EFUSE_KEY1_DATA2_V)<<(EFUSE_KEY1_DATA2_S))
#define EFUSE_KEY1_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA2_S  0

#define EFUSE_RD_KEY1_DATA3_REG          (DR_REG_EFUSE_BASE + 0x0c8)
/* EFUSE_KEY1_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the third 32 bits of KEY1.*/
#define EFUSE_KEY1_DATA3  0xFFFFFFFF
#define EFUSE_KEY1_DATA3_M  ((EFUSE_KEY1_DATA3_V)<<(EFUSE_KEY1_DATA3_S))
#define EFUSE_KEY1_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA3_S  0

#define EFUSE_RD_KEY1_DATA4_REG          (DR_REG_EFUSE_BASE + 0x0cc)
/* EFUSE_KEY1_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fourth 32 bits of KEY1.*/
#define EFUSE_KEY1_DATA4  0xFFFFFFFF
#define EFUSE_KEY1_DATA4_M  ((EFUSE_KEY1_DATA4_V)<<(EFUSE_KEY1_DATA4_S))
#define EFUSE_KEY1_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA4_S  0

#define EFUSE_RD_KEY1_DATA5_REG          (DR_REG_EFUSE_BASE + 0x0d0)
/* EFUSE_KEY1_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fifth 32 bits of KEY1.*/
#define EFUSE_KEY1_DATA5  0xFFFFFFFF
#define EFUSE_KEY1_DATA5_M  ((EFUSE_KEY1_DATA5_V)<<(EFUSE_KEY1_DATA5_S))
#define EFUSE_KEY1_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA5_S  0

#define EFUSE_RD_KEY1_DATA6_REG          (DR_REG_EFUSE_BASE + 0x0d4)
/* EFUSE_KEY1_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the sixth 32 bits of KEY1.*/
#define EFUSE_KEY1_DATA6  0xFFFFFFFF
#define EFUSE_KEY1_DATA6_M  ((EFUSE_KEY1_DATA6_V)<<(EFUSE_KEY1_DATA6_S))
#define EFUSE_KEY1_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA6_S  0

#define EFUSE_RD_KEY1_DATA7_REG          (DR_REG_EFUSE_BASE + 0x0d8)
/* EFUSE_KEY1_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the seventh 32 bits of KEY1.*/
#define EFUSE_KEY1_DATA7  0xFFFFFFFF
#define EFUSE_KEY1_DATA7_M  ((EFUSE_KEY1_DATA7_V)<<(EFUSE_KEY1_DATA7_S))
#define EFUSE_KEY1_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY1_DATA7_S  0

#define EFUSE_RD_KEY2_DATA0_REG          (DR_REG_EFUSE_BASE + 0x0dc)
/* EFUSE_KEY2_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the zeroth 32 bits of KEY2.*/
#define EFUSE_KEY2_DATA0  0xFFFFFFFF
#define EFUSE_KEY2_DATA0_M  ((EFUSE_KEY2_DATA0_V)<<(EFUSE_KEY2_DATA0_S))
#define EFUSE_KEY2_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA0_S  0

#define EFUSE_RD_KEY2_DATA1_REG          (DR_REG_EFUSE_BASE + 0x0e0)
/* EFUSE_KEY2_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first 32 bits of KEY2.*/
#define EFUSE_KEY2_DATA1  0xFFFFFFFF
#define EFUSE_KEY2_DATA1_M  ((EFUSE_KEY2_DATA1_V)<<(EFUSE_KEY2_DATA1_S))
#define EFUSE_KEY2_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA1_S  0

#define EFUSE_RD_KEY2_DATA2_REG          (DR_REG_EFUSE_BASE + 0x0e4)
/* EFUSE_KEY2_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of KEY2.*/
#define EFUSE_KEY2_DATA2  0xFFFFFFFF
#define EFUSE_KEY2_DATA2_M  ((EFUSE_KEY2_DATA2_V)<<(EFUSE_KEY2_DATA2_S))
#define EFUSE_KEY2_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA2_S  0

#define EFUSE_RD_KEY2_DATA3_REG          (DR_REG_EFUSE_BASE + 0x0e8)
/* EFUSE_KEY2_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the third 32 bits of KEY2.*/
#define EFUSE_KEY2_DATA3  0xFFFFFFFF
#define EFUSE_KEY2_DATA3_M  ((EFUSE_KEY2_DATA3_V)<<(EFUSE_KEY2_DATA3_S))
#define EFUSE_KEY2_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA3_S  0

#define EFUSE_RD_KEY2_DATA4_REG          (DR_REG_EFUSE_BASE + 0x0ec)
/* EFUSE_KEY2_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fourth 32 bits of KEY2.*/
#define EFUSE_KEY2_DATA4  0xFFFFFFFF
#define EFUSE_KEY2_DATA4_M  ((EFUSE_KEY2_DATA4_V)<<(EFUSE_KEY2_DATA4_S))
#define EFUSE_KEY2_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA4_S  0

#define EFUSE_RD_KEY2_DATA5_REG          (DR_REG_EFUSE_BASE + 0x0f0)
/* EFUSE_KEY2_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fifth 32 bits of KEY2.*/
#define EFUSE_KEY2_DATA5  0xFFFFFFFF
#define EFUSE_KEY2_DATA5_M  ((EFUSE_KEY2_DATA5_V)<<(EFUSE_KEY2_DATA5_S))
#define EFUSE_KEY2_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA5_S  0

#define EFUSE_RD_KEY2_DATA6_REG          (DR_REG_EFUSE_BASE + 0x0f4)
/* EFUSE_KEY2_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the sixth 32 bits of KEY2.*/
#define EFUSE_KEY2_DATA6  0xFFFFFFFF
#define EFUSE_KEY2_DATA6_M  ((EFUSE_KEY2_DATA6_V)<<(EFUSE_KEY2_DATA6_S))
#define EFUSE_KEY2_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA6_S  0

#define EFUSE_RD_KEY2_DATA7_REG          (DR_REG_EFUSE_BASE + 0x0f8)
/* EFUSE_KEY2_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the seventh 32 bits of KEY2.*/
#define EFUSE_KEY2_DATA7  0xFFFFFFFF
#define EFUSE_KEY2_DATA7_M  ((EFUSE_KEY2_DATA7_V)<<(EFUSE_KEY2_DATA7_S))
#define EFUSE_KEY2_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY2_DATA7_S  0

#define EFUSE_RD_KEY3_DATA0_REG          (DR_REG_EFUSE_BASE + 0x0fc)
/* EFUSE_KEY3_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the zeroth 32 bits of KEY3.*/
#define EFUSE_KEY3_DATA0  0xFFFFFFFF
#define EFUSE_KEY3_DATA0_M  ((EFUSE_KEY3_DATA0_V)<<(EFUSE_KEY3_DATA0_S))
#define EFUSE_KEY3_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA0_S  0

#define EFUSE_RD_KEY3_DATA1_REG          (DR_REG_EFUSE_BASE + 0x100)
/* EFUSE_KEY3_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first 32 bits of KEY3.*/
#define EFUSE_KEY3_DATA1  0xFFFFFFFF
#define EFUSE_KEY3_DATA1_M  ((EFUSE_KEY3_DATA1_V)<<(EFUSE_KEY3_DATA1_S))
#define EFUSE_KEY3_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA1_S  0

#define EFUSE_RD_KEY3_DATA2_REG          (DR_REG_EFUSE_BASE + 0x104)
/* EFUSE_KEY3_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of KEY3.*/
#define EFUSE_KEY3_DATA2  0xFFFFFFFF
#define EFUSE_KEY3_DATA2_M  ((EFUSE_KEY3_DATA2_V)<<(EFUSE_KEY3_DATA2_S))
#define EFUSE_KEY3_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA2_S  0

#define EFUSE_RD_KEY3_DATA3_REG          (DR_REG_EFUSE_BASE + 0x108)
/* EFUSE_KEY3_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the third 32 bits of KEY3.*/
#define EFUSE_KEY3_DATA3  0xFFFFFFFF
#define EFUSE_KEY3_DATA3_M  ((EFUSE_KEY3_DATA3_V)<<(EFUSE_KEY3_DATA3_S))
#define EFUSE_KEY3_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA3_S  0

#define EFUSE_RD_KEY3_DATA4_REG          (DR_REG_EFUSE_BASE + 0x10c)
/* EFUSE_KEY3_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fourth 32 bits of KEY3.*/
#define EFUSE_KEY3_DATA4  0xFFFFFFFF
#define EFUSE_KEY3_DATA4_M  ((EFUSE_KEY3_DATA4_V)<<(EFUSE_KEY3_DATA4_S))
#define EFUSE_KEY3_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA4_S  0

#define EFUSE_RD_KEY3_DATA5_REG          (DR_REG_EFUSE_BASE + 0x110)
/* EFUSE_KEY3_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fifth 32 bits of KEY3.*/
#define EFUSE_KEY3_DATA5  0xFFFFFFFF
#define EFUSE_KEY3_DATA5_M  ((EFUSE_KEY3_DATA5_V)<<(EFUSE_KEY3_DATA5_S))
#define EFUSE_KEY3_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA5_S  0

#define EFUSE_RD_KEY3_DATA6_REG          (DR_REG_EFUSE_BASE + 0x114)
/* EFUSE_KEY3_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the sixth 32 bits of KEY3.*/
#define EFUSE_KEY3_DATA6  0xFFFFFFFF
#define EFUSE_KEY3_DATA6_M  ((EFUSE_KEY3_DATA6_V)<<(EFUSE_KEY3_DATA6_S))
#define EFUSE_KEY3_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA6_S  0

#define EFUSE_RD_KEY3_DATA7_REG          (DR_REG_EFUSE_BASE + 0x118)
/* EFUSE_KEY3_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the seventh 32 bits of KEY3.*/
#define EFUSE_KEY3_DATA7  0xFFFFFFFF
#define EFUSE_KEY3_DATA7_M  ((EFUSE_KEY3_DATA7_V)<<(EFUSE_KEY3_DATA7_S))
#define EFUSE_KEY3_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY3_DATA7_S  0

#define EFUSE_RD_KEY4_DATA0_REG          (DR_REG_EFUSE_BASE + 0x11c)
/* EFUSE_KEY4_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the zeroth 32 bits of KEY4.*/
#define EFUSE_KEY4_DATA0  0xFFFFFFFF
#define EFUSE_KEY4_DATA0_M  ((EFUSE_KEY4_DATA0_V)<<(EFUSE_KEY4_DATA0_S))
#define EFUSE_KEY4_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA0_S  0

#define EFUSE_RD_KEY4_DATA1_REG          (DR_REG_EFUSE_BASE + 0x120)
/* EFUSE_KEY4_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first 32 bits of KEY4.*/
#define EFUSE_KEY4_DATA1  0xFFFFFFFF
#define EFUSE_KEY4_DATA1_M  ((EFUSE_KEY4_DATA1_V)<<(EFUSE_KEY4_DATA1_S))
#define EFUSE_KEY4_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA1_S  0

#define EFUSE_RD_KEY4_DATA2_REG          (DR_REG_EFUSE_BASE + 0x124)
/* EFUSE_KEY4_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of KEY4.*/
#define EFUSE_KEY4_DATA2  0xFFFFFFFF
#define EFUSE_KEY4_DATA2_M  ((EFUSE_KEY4_DATA2_V)<<(EFUSE_KEY4_DATA2_S))
#define EFUSE_KEY4_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA2_S  0

#define EFUSE_RD_KEY4_DATA3_REG          (DR_REG_EFUSE_BASE + 0x128)
/* EFUSE_KEY4_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the third 32 bits of KEY4.*/
#define EFUSE_KEY4_DATA3  0xFFFFFFFF
#define EFUSE_KEY4_DATA3_M  ((EFUSE_KEY4_DATA3_V)<<(EFUSE_KEY4_DATA3_S))
#define EFUSE_KEY4_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA3_S  0

#define EFUSE_RD_KEY4_DATA4_REG          (DR_REG_EFUSE_BASE + 0x12c)
/* EFUSE_KEY4_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fourth 32 bits of KEY4.*/
#define EFUSE_KEY4_DATA4  0xFFFFFFFF
#define EFUSE_KEY4_DATA4_M  ((EFUSE_KEY4_DATA4_V)<<(EFUSE_KEY4_DATA4_S))
#define EFUSE_KEY4_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA4_S  0

#define EFUSE_RD_KEY4_DATA5_REG          (DR_REG_EFUSE_BASE + 0x130)
/* EFUSE_KEY4_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fifth 32 bits of KEY4.*/
#define EFUSE_KEY4_DATA5  0xFFFFFFFF
#define EFUSE_KEY4_DATA5_M  ((EFUSE_KEY4_DATA5_V)<<(EFUSE_KEY4_DATA5_S))
#define EFUSE_KEY4_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA5_S  0

#define EFUSE_RD_KEY4_DATA6_REG          (DR_REG_EFUSE_BASE + 0x134)
/* EFUSE_KEY4_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the sixth 32 bits of KEY4.*/
#define EFUSE_KEY4_DATA6  0xFFFFFFFF
#define EFUSE_KEY4_DATA6_M  ((EFUSE_KEY4_DATA6_V)<<(EFUSE_KEY4_DATA6_S))
#define EFUSE_KEY4_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA6_S  0

#define EFUSE_RD_KEY4_DATA7_REG          (DR_REG_EFUSE_BASE + 0x138)
/* EFUSE_KEY4_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the seventh 32 bits of KEY4.*/
#define EFUSE_KEY4_DATA7  0xFFFFFFFF
#define EFUSE_KEY4_DATA7_M  ((EFUSE_KEY4_DATA7_V)<<(EFUSE_KEY4_DATA7_S))
#define EFUSE_KEY4_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY4_DATA7_S  0

#define EFUSE_RD_KEY5_DATA0_REG          (DR_REG_EFUSE_BASE + 0x13c)
/* EFUSE_KEY5_DATA0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the zeroth 32 bits of KEY5.*/
#define EFUSE_KEY5_DATA0  0xFFFFFFFF
#define EFUSE_KEY5_DATA0_M  ((EFUSE_KEY5_DATA0_V)<<(EFUSE_KEY5_DATA0_S))
#define EFUSE_KEY5_DATA0_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA0_S  0

#define EFUSE_RD_KEY5_DATA1_REG          (DR_REG_EFUSE_BASE + 0x140)
/* EFUSE_KEY5_DATA1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the first 32 bits of KEY5.*/
#define EFUSE_KEY5_DATA1  0xFFFFFFFF
#define EFUSE_KEY5_DATA1_M  ((EFUSE_KEY5_DATA1_V)<<(EFUSE_KEY5_DATA1_S))
#define EFUSE_KEY5_DATA1_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA1_S  0

#define EFUSE_RD_KEY5_DATA2_REG          (DR_REG_EFUSE_BASE + 0x144)
/* EFUSE_KEY5_DATA2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the second 32 bits of KEY5.*/
#define EFUSE_KEY5_DATA2  0xFFFFFFFF
#define EFUSE_KEY5_DATA2_M  ((EFUSE_KEY5_DATA2_V)<<(EFUSE_KEY5_DATA2_S))
#define EFUSE_KEY5_DATA2_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA2_S  0

#define EFUSE_RD_KEY5_DATA3_REG          (DR_REG_EFUSE_BASE + 0x148)
/* EFUSE_KEY5_DATA3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the third 32 bits of KEY5.*/
#define EFUSE_KEY5_DATA3  0xFFFFFFFF
#define EFUSE_KEY5_DATA3_M  ((EFUSE_KEY5_DATA3_V)<<(EFUSE_KEY5_DATA3_S))
#define EFUSE_KEY5_DATA3_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA3_S  0

#define EFUSE_RD_KEY5_DATA4_REG          (DR_REG_EFUSE_BASE + 0x14c)
/* EFUSE_KEY5_DATA4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fourth 32 bits of KEY5.*/
#define EFUSE_KEY5_DATA4  0xFFFFFFFF
#define EFUSE_KEY5_DATA4_M  ((EFUSE_KEY5_DATA4_V)<<(EFUSE_KEY5_DATA4_S))
#define EFUSE_KEY5_DATA4_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA4_S  0

#define EFUSE_RD_KEY5_DATA5_REG          (DR_REG_EFUSE_BASE + 0x150)
/* EFUSE_KEY5_DATA5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the fifth 32 bits of KEY5.*/
#define EFUSE_KEY5_DATA5  0xFFFFFFFF
#define EFUSE_KEY5_DATA5_M  ((EFUSE_KEY5_DATA5_V)<<(EFUSE_KEY5_DATA5_S))
#define EFUSE_KEY5_DATA5_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA5_S  0

#define EFUSE_RD_KEY5_DATA6_REG          (DR_REG_EFUSE_BASE + 0x154)
/* EFUSE_KEY5_DATA6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the sixth 32 bits of KEY5.*/
#define EFUSE_KEY5_DATA6  0xFFFFFFFF
#define EFUSE_KEY5_DATA6_M  ((EFUSE_KEY5_DATA6_V)<<(EFUSE_KEY5_DATA6_S))
#define EFUSE_KEY5_DATA6_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA6_S  0

#define EFUSE_RD_KEY5_DATA7_REG          (DR_REG_EFUSE_BASE + 0x158)
/* EFUSE_KEY5_DATA7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the seventh 32 bits of KEY5.*/
#define EFUSE_KEY5_DATA7  0xFFFFFFFF
#define EFUSE_KEY5_DATA7_M  ((EFUSE_KEY5_DATA7_V)<<(EFUSE_KEY5_DATA7_S))
#define EFUSE_KEY5_DATA7_V  0xFFFFFFFF
#define EFUSE_KEY5_DATA7_S  0

#define EFUSE_RD_SYS_PART2_DATA0_REG          (DR_REG_EFUSE_BASE + 0x15c)
/* EFUSE_SYS_DATA_PART2_0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the $nth 32 bits of the 2nd part of system data.*/
#define EFUSE_SYS_DATA_PART2_0  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_0_M  ((EFUSE_SYS_DATA_PART2_0_V)<<(EFUSE_SYS_DATA_PART2_0_S))
#define EFUSE_SYS_DATA_PART2_0_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_0_S  0

#define EFUSE_RD_SYS_PART2_DATA1_REG          (DR_REG_EFUSE_BASE + 0x160)
/* EFUSE_SYS_DATA_PART2_1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the $nth 32 bits of the 2nd part of system data.*/
#define EFUSE_SYS_DATA_PART2_1  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_1_M  ((EFUSE_SYS_DATA_PART2_1_V)<<(EFUSE_SYS_DATA_PART2_1_S))
#define EFUSE_SYS_DATA_PART2_1_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_1_S  0

#define EFUSE_RD_SYS_PART2_DATA2_REG          (DR_REG_EFUSE_BASE + 0x164)
/* EFUSE_SYS_DATA_PART2_2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the $nth 32 bits of the 2nd part of system data.*/
#define EFUSE_SYS_DATA_PART2_2  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_2_M  ((EFUSE_SYS_DATA_PART2_2_V)<<(EFUSE_SYS_DATA_PART2_2_S))
#define EFUSE_SYS_DATA_PART2_2_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_2_S  0

#define EFUSE_RD_SYS_PART2_DATA3_REG          (DR_REG_EFUSE_BASE + 0x168)
/* EFUSE_SYS_DATA_PART2_3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the $nth 32 bits of the 2nd part of system data.*/
#define EFUSE_SYS_DATA_PART2_3  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_3_M  ((EFUSE_SYS_DATA_PART2_3_V)<<(EFUSE_SYS_DATA_PART2_3_S))
#define EFUSE_SYS_DATA_PART2_3_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_3_S  0

#define EFUSE_RD_SYS_PART2_DATA4_REG          (DR_REG_EFUSE_BASE + 0x16c)
/* EFUSE_SYS_DATA_PART2_4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the $nth 32 bits of the 2nd part of system data.*/
#define EFUSE_SYS_DATA_PART2_4  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_4_M  ((EFUSE_SYS_DATA_PART2_4_V)<<(EFUSE_SYS_DATA_PART2_4_S))
#define EFUSE_SYS_DATA_PART2_4_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_4_S  0

#define EFUSE_RD_SYS_PART2_DATA5_REG          (DR_REG_EFUSE_BASE + 0x170)
/* EFUSE_SYS_DATA_PART2_5 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the $nth 32 bits of the 2nd part of system data.*/
#define EFUSE_SYS_DATA_PART2_5  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_5_M  ((EFUSE_SYS_DATA_PART2_5_V)<<(EFUSE_SYS_DATA_PART2_5_S))
#define EFUSE_SYS_DATA_PART2_5_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_5_S  0

#define EFUSE_RD_SYS_PART2_DATA6_REG          (DR_REG_EFUSE_BASE + 0x174)
/* EFUSE_SYS_DATA_PART2_6 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the $nth 32 bits of the 2nd part of system data.*/
#define EFUSE_SYS_DATA_PART2_6  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_6_M  ((EFUSE_SYS_DATA_PART2_6_V)<<(EFUSE_SYS_DATA_PART2_6_S))
#define EFUSE_SYS_DATA_PART2_6_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_6_S  0

#define EFUSE_RD_SYS_PART2_DATA7_REG          (DR_REG_EFUSE_BASE + 0x178)
/* EFUSE_SYS_DATA_PART2_7 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: Stores the $nth 32 bits of the 2nd part of system data.*/
#define EFUSE_SYS_DATA_PART2_7  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_7_M  ((EFUSE_SYS_DATA_PART2_7_V)<<(EFUSE_SYS_DATA_PART2_7_S))
#define EFUSE_SYS_DATA_PART2_7_V  0xFFFFFFFF
#define EFUSE_SYS_DATA_PART2_7_S  0

#define EFUSE_RD_REPEAT_ERR0_REG          (DR_REG_EFUSE_BASE + 0x17c)
/* EFUSE_VDD_SPI_DREFH_ERR : RO ;bitpos:[31:30] ;default: 2'h0 ; */
/*description: If any bit in VDD_SPI_DREFH is 1  then it indicates a programming error.*/
#define EFUSE_VDD_SPI_DREFH_ERR  0x00000003
#define EFUSE_VDD_SPI_DREFH_ERR_M  ((EFUSE_VDD_SPI_DREFH_ERR_V)<<(EFUSE_VDD_SPI_DREFH_ERR_S))
#define EFUSE_VDD_SPI_DREFH_ERR_V  0x3
#define EFUSE_VDD_SPI_DREFH_ERR_S  30
/* EFUSE_VDD_SPI_MODECURLIM_ERR : RO ;bitpos:[29] ;default: 1'h0 ; */
/*description: If VDD_SPI_MODECURLIM is 1  then it indicates a programming error.*/
#define EFUSE_VDD_SPI_MODECURLIM_ERR  (BIT(29))
#define EFUSE_VDD_SPI_MODECURLIM_ERR_M  (BIT(29))
#define EFUSE_VDD_SPI_MODECURLIM_ERR_V  0x1
#define EFUSE_VDD_SPI_MODECURLIM_ERR_S  29
/* EFUSE_RPT4_RESERVED0_ERR : RO ;bitpos:[28:27] ;default: 2'h0 ; */
/*description: Reserved.*/
#define EFUSE_RPT4_RESERVED0_ERR  0x00000003
#define EFUSE_RPT4_RESERVED0_ERR_M  ((EFUSE_RPT4_RESERVED0_ERR_V)<<(EFUSE_RPT4_RESERVED0_ERR_S))
#define EFUSE_RPT4_RESERVED0_ERR_V  0x3
#define EFUSE_RPT4_RESERVED0_ERR_S  27
/* EFUSE_USB_FORCE_NOPERSIST_ERR : RO ;bitpos:[26] ;default: 1'h0 ; */
/*description: Record error infomation of the burning result of usb_force_nopersist.*/
#define EFUSE_USB_FORCE_NOPERSIST_ERR  (BIT(26))
#define EFUSE_USB_FORCE_NOPERSIST_ERR_M  (BIT(26))
#define EFUSE_USB_FORCE_NOPERSIST_ERR_V  0x1
#define EFUSE_USB_FORCE_NOPERSIST_ERR_S  26
/* EFUSE_EXT_PHY_ENABLE_ERR : RO ;bitpos:[25] ;default: 1'h0 ; */
/*description: If EXT_PHY_ENABLE is 1  then it indicates a programming error.*/
#define EFUSE_EXT_PHY_ENABLE_ERR  (BIT(25))
#define EFUSE_EXT_PHY_ENABLE_ERR_M  (BIT(25))
#define EFUSE_EXT_PHY_ENABLE_ERR_V  0x1
#define EFUSE_EXT_PHY_ENABLE_ERR_S  25
/* EFUSE_USB_EXCHG_PINS_ERR : RO ;bitpos:[24] ;default: 1'h0 ; */
/*description: If USB_EXCHG_PINS is 1  then it indicates a programming error.*/
#define EFUSE_USB_EXCHG_PINS_ERR  (BIT(24))
#define EFUSE_USB_EXCHG_PINS_ERR_M  (BIT(24))
#define EFUSE_USB_EXCHG_PINS_ERR_V  0x1
#define EFUSE_USB_EXCHG_PINS_ERR_S  24
/* EFUSE_USB_DREFL_ERR : RO ;bitpos:[23:22] ;default: 2'h0 ; */
/*description: If any bit in USB_DREFL is 1  then it indicates a programming error.*/
#define EFUSE_USB_DREFL_ERR  0x00000003
#define EFUSE_USB_DREFL_ERR_M  ((EFUSE_USB_DREFL_ERR_V)<<(EFUSE_USB_DREFL_ERR_S))
#define EFUSE_USB_DREFL_ERR_V  0x3
#define EFUSE_USB_DREFL_ERR_S  22
/* EFUSE_USB_DREFH_ERR : RO ;bitpos:[21:20] ;default: 2'h0 ; */
/*description: If any bit in USB_DREFH is 1  then it indicates a programming error.*/
#define EFUSE_USB_DREFH_ERR  0x00000003
#define EFUSE_USB_DREFH_ERR_M  ((EFUSE_USB_DREFH_ERR_V)<<(EFUSE_USB_DREFH_ERR_S))
#define EFUSE_USB_DREFH_ERR_V  0x3
#define EFUSE_USB_DREFH_ERR_S  20
/* EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR : RO ;bitpos:[19] ;default: 1'h0 ; */
/*description: If DIS_DOWNLOAD_MANUAL_ENCRYPT is 1  then it indicates a programming error.*/
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR  (BIT(19))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_M  (BIT(19))
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_V  0x1
#define EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_ERR_S  19
/* EFUSE_HARD_DIS_JTAG_ERR : RO ;bitpos:[18] ;default: 1'h0 ; */
/*description: If HARD_DIS_JTAG is 1  then it indicates a programming error.*/
#define EFUSE_HARD_DIS_JTAG_ERR  (BIT(18))
#define EFUSE_HARD_DIS_JTAG_ERR_M  (BIT(18))
#define EFUSE_HARD_DIS_JTAG_ERR_V  0x1
#define EFUSE_HARD_DIS_JTAG_ERR_S  18
/* EFUSE_SOFT_DIS_JTAG_ERR : RO ;bitpos:[17] ;default: 1'h0 ; */
/*description: If SOFT_DIS_JTAG is 1  then it indicates a programming error.*/
#define EFUSE_SOFT_DIS_JTAG_ERR  (BIT(17))
#define EFUSE_SOFT_DIS_JTAG_ERR_M  (BIT(17))
#define EFUSE_SOFT_DIS_JTAG_ERR_V  0x1
#define EFUSE_SOFT_DIS_JTAG_ERR_S  17
/* EFUSE_DIS_EFUSE_ATE_WR_ERR : RO ;bitpos:[16] ;default: 1'h0 ; */
/*description: If DIS_EFUSE_ATE_WR is 1  then it indicates a programming error.*/
#define EFUSE_DIS_EFUSE_ATE_WR_ERR  (BIT(16))
#define EFUSE_DIS_EFUSE_ATE_WR_ERR_M  (BIT(16))
#define EFUSE_DIS_EFUSE_ATE_WR_ERR_V  0x1
#define EFUSE_DIS_EFUSE_ATE_WR_ERR_S  16
/* EFUSE_DIS_BOOT_REMAP_ERR : RO ;bitpos:[15] ;default: 1'h0 ; */
/*description: If DIS_BOOT_REMAP is 1  then it indicates a programming error.*/
#define EFUSE_DIS_BOOT_REMAP_ERR  (BIT(15))
#define EFUSE_DIS_BOOT_REMAP_ERR_M  (BIT(15))
#define EFUSE_DIS_BOOT_REMAP_ERR_V  0x1
#define EFUSE_DIS_BOOT_REMAP_ERR_S  15
/* EFUSE_DIS_CAN_ERR : RO ;bitpos:[14] ;default: 1'h0 ; */
/*description: If DIS_CAN is 1  then it indicates a programming error.*/
#define EFUSE_DIS_CAN_ERR  (BIT(14))
#define EFUSE_DIS_CAN_ERR_M  (BIT(14))
#define EFUSE_DIS_CAN_ERR_V  0x1
#define EFUSE_DIS_CAN_ERR_S  14
/* EFUSE_DIS_USB_ERR : RO ;bitpos:[13] ;default: 1'h0 ; */
/*description: If DIS_USB is 1  then it indicates a programming error.*/
#define EFUSE_DIS_USB_ERR  (BIT(13))
#define EFUSE_DIS_USB_ERR_M  (BIT(13))
#define EFUSE_DIS_USB_ERR_V  0x1
#define EFUSE_DIS_USB_ERR_S  13
/* EFUSE_DIS_FORCE_DOWNLOAD_ERR : RO ;bitpos:[12] ;default: 1'h0 ; */
/*description: If DIS_FORCE_DOWNLOAD is 1  then it indicates a programming error.*/
#define EFUSE_DIS_FORCE_DOWNLOAD_ERR  (BIT(12))
#define EFUSE_DIS_FORCE_DOWNLOAD_ERR_M  (BIT(12))
#define EFUSE_DIS_FORCE_DOWNLOAD_ERR_V  0x1
#define EFUSE_DIS_FORCE_DOWNLOAD_ERR_S  12
/* EFUSE_DIS_DOWNLOAD_DCACHE_ERR : RO ;bitpos:[11] ;default: 1'h0 ; */
/*description: If DIS_DOWNLOAD_DCACHE is 1  then it indicates a programming error.*/
#define EFUSE_DIS_DOWNLOAD_DCACHE_ERR  (BIT(11))
#define EFUSE_DIS_DOWNLOAD_DCACHE_ERR_M  (BIT(11))
#define EFUSE_DIS_DOWNLOAD_DCACHE_ERR_V  0x1
#define EFUSE_DIS_DOWNLOAD_DCACHE_ERR_S  11
/* EFUSE_DIS_DOWNLOAD_ICACHE_ERR : RO ;bitpos:[10] ;default: 1'h0 ; */
/*description: If DIS_DOWNLOAD_ICACHE is 1  then it indicates a programming error.*/
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR  (BIT(10))
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR_M  (BIT(10))
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR_V  0x1
#define EFUSE_DIS_DOWNLOAD_ICACHE_ERR_S  10
/* EFUSE_DIS_DCACHE_ERR : RO ;bitpos:[9] ;default: 1'h0 ; */
/*description: If DIS_DCACHE is 1  then it indicates a programming error.*/
#define EFUSE_DIS_DCACHE_ERR  (BIT(9))
#define EFUSE_DIS_DCACHE_ERR_M  (BIT(9))
#define EFUSE_DIS_DCACHE_ERR_V  0x1
#define EFUSE_DIS_DCACHE_ERR_S  9
/* EFUSE_DIS_ICACHE_ERR : RO ;bitpos:[8] ;default: 1'h0 ; */
/*description: If DIS_ICACHE is 1  then it indicates a programming error.*/
#define EFUSE_DIS_ICACHE_ERR  (BIT(8))
#define EFUSE_DIS_ICACHE_ERR_M  (BIT(8))
#define EFUSE_DIS_ICACHE_ERR_V  0x1
#define EFUSE_DIS_ICACHE_ERR_S  8
/* EFUSE_DIS_RTC_RAM_BOOT_ERR : RO ;bitpos:[7] ;default: 1'h0 ; */
/*description: If DIS_RTC_RAM_BOOT is 1  then it indicates a programming error.*/
#define EFUSE_DIS_RTC_RAM_BOOT_ERR  (BIT(7))
#define EFUSE_DIS_RTC_RAM_BOOT_ERR_M  (BIT(7))
#define EFUSE_DIS_RTC_RAM_BOOT_ERR_V  0x1
#define EFUSE_DIS_RTC_RAM_BOOT_ERR_S  7
/* EFUSE_RD_DIS_ERR : RO ;bitpos:[6:0] ;default: 7'h0 ; */
/*description: If any bit in RD_DIS is 1  then it indicates a programming error.*/
#define EFUSE_RD_DIS_ERR  0x0000007F
#define EFUSE_RD_DIS_ERR_M  ((EFUSE_RD_DIS_ERR_V)<<(EFUSE_RD_DIS_ERR_S))
#define EFUSE_RD_DIS_ERR_V  0x7F
#define EFUSE_RD_DIS_ERR_S  0

#define EFUSE_RD_REPEAT_ERR1_REG          (DR_REG_EFUSE_BASE + 0x180)
/* EFUSE_KEY_PURPOSE_1_ERR : RO ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: If any bit in KEY_PURPOSE_1 is 1  then it indicates a programming error.*/
#define EFUSE_KEY_PURPOSE_1_ERR  0x0000000F
#define EFUSE_KEY_PURPOSE_1_ERR_M  ((EFUSE_KEY_PURPOSE_1_ERR_V)<<(EFUSE_KEY_PURPOSE_1_ERR_S))
#define EFUSE_KEY_PURPOSE_1_ERR_V  0xF
#define EFUSE_KEY_PURPOSE_1_ERR_S  28
/* EFUSE_KEY_PURPOSE_0_ERR : RO ;bitpos:[27:24] ;default: 4'h0 ; */
/*description: If any bit in KEY_PURPOSE_0 is 1  then it indicates a programming error.*/
#define EFUSE_KEY_PURPOSE_0_ERR  0x0000000F
#define EFUSE_KEY_PURPOSE_0_ERR_M  ((EFUSE_KEY_PURPOSE_0_ERR_V)<<(EFUSE_KEY_PURPOSE_0_ERR_S))
#define EFUSE_KEY_PURPOSE_0_ERR_V  0xF
#define EFUSE_KEY_PURPOSE_0_ERR_S  24
/* EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR : RO ;bitpos:[23] ;default: 1'h0 ; */
/*description: If SECURE_BOOT_KEY_REVOKE2 is 1  then it indicates a programming error.*/
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR  (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR_M  (BIT(23))
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE2_ERR_S  23
/* EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR : RO ;bitpos:[22] ;default: 1'h0 ; */
/*description: If SECURE_BOOT_KEY_REVOKE1 is 1  then it indicates a programming error.*/
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR  (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR_M  (BIT(22))
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE1_ERR_S  22
/* EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR : RO ;bitpos:[21] ;default: 1'h0 ; */
/*description: If SECURE_BOOT_KEY_REVOKE0 is 1  then it indicates a programming error.*/
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR  (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR_M  (BIT(21))
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR_V  0x1
#define EFUSE_SECURE_BOOT_KEY_REVOKE0_ERR_S  21
/* EFUSE_SPI_BOOT_CRYPT_CNT_ERR : RO ;bitpos:[20:18] ;default: 3'h0 ; */
/*description: If any bit in SPI_BOOT_CRYPT_CNT is 1  then it indicates a programming error.*/
#define EFUSE_SPI_BOOT_CRYPT_CNT_ERR  0x00000007
#define EFUSE_SPI_BOOT_CRYPT_CNT_ERR_M  ((EFUSE_SPI_BOOT_CRYPT_CNT_ERR_V)<<(EFUSE_SPI_BOOT_CRYPT_CNT_ERR_S))
#define EFUSE_SPI_BOOT_CRYPT_CNT_ERR_V  0x7
#define EFUSE_SPI_BOOT_CRYPT_CNT_ERR_S  18
/* EFUSE_WDT_DELAY_SEL_ERR : RO ;bitpos:[17:16] ;default: 2'h0 ; */
/*description: If any bit in WDT_DELAY_SEL is 1  then it indicates a programming error.*/
#define EFUSE_WDT_DELAY_SEL_ERR  0x00000003
#define EFUSE_WDT_DELAY_SEL_ERR_M  ((EFUSE_WDT_DELAY_SEL_ERR_V)<<(EFUSE_WDT_DELAY_SEL_ERR_S))
#define EFUSE_WDT_DELAY_SEL_ERR_V  0x3
#define EFUSE_WDT_DELAY_SEL_ERR_S  16
/* EFUSE_VDD_SPI_DCAP_ERR : RO ;bitpos:[15:14] ;default: 2'h0 ; */
/*description: If any bit in VDD_SPI_DCAP is 1  then it indicates a programming error.*/
#define EFUSE_VDD_SPI_DCAP_ERR  0x00000003
#define EFUSE_VDD_SPI_DCAP_ERR_M  ((EFUSE_VDD_SPI_DCAP_ERR_V)<<(EFUSE_VDD_SPI_DCAP_ERR_S))
#define EFUSE_VDD_SPI_DCAP_ERR_V  0x3
#define EFUSE_VDD_SPI_DCAP_ERR_S  14
/* EFUSE_VDD_SPI_INIT_ERR : RO ;bitpos:[13:12] ;default: 2'h0 ; */
/*description: If any bit in VDD_SPI_INIT is 1  then it indicates a programming error.*/
#define EFUSE_VDD_SPI_INIT_ERR  0x00000003
#define EFUSE_VDD_SPI_INIT_ERR_M  ((EFUSE_VDD_SPI_INIT_ERR_V)<<(EFUSE_VDD_SPI_INIT_ERR_S))
#define EFUSE_VDD_SPI_INIT_ERR_V  0x3
#define EFUSE_VDD_SPI_INIT_ERR_S  12
/* EFUSE_VDD_SPI_DCURLIM_ERR : RO ;bitpos:[11:9] ;default: 3'h0 ; */
/*description: If any bit in VDD_SPI_DCURLIM is 1  then it indicates a programming error.*/
#define EFUSE_VDD_SPI_DCURLIM_ERR  0x00000007
#define EFUSE_VDD_SPI_DCURLIM_ERR_M  ((EFUSE_VDD_SPI_DCURLIM_ERR_V)<<(EFUSE_VDD_SPI_DCURLIM_ERR_S))
#define EFUSE_VDD_SPI_DCURLIM_ERR_V  0x7
#define EFUSE_VDD_SPI_DCURLIM_ERR_S  9
/* EFUSE_VDD_SPI_ENCURLIM_ERR : RO ;bitpos:[8] ;default: 1'h0 ; */
/*description: If VDD_SPI_ENCURLIM is 1  then it indicates a programming error.*/
#define EFUSE_VDD_SPI_ENCURLIM_ERR  (BIT(8))
#define EFUSE_VDD_SPI_ENCURLIM_ERR_M  (BIT(8))
#define EFUSE_VDD_SPI_ENCURLIM_ERR_V  0x1
#define EFUSE_VDD_SPI_ENCURLIM_ERR_S  8
/* EFUSE_VDD_SPI_EN_INIT_ERR : RO ;bitpos:[7] ;default: 1'h0 ; */
/*description: If VDD_SPI_EN_INIT is 1  then it indicates a programming error.*/
#define EFUSE_VDD_SPI_EN_INIT_ERR  (BIT(7))
#define EFUSE_VDD_SPI_EN_INIT_ERR_M  (BIT(7))
#define EFUSE_VDD_SPI_EN_INIT_ERR_V  0x1
#define EFUSE_VDD_SPI_EN_INIT_ERR_S  7
/* EFUSE_VDD_SPI_FORCE_ERR : RO ;bitpos:[6] ;default: 1'h0 ; */
/*description: If VDD_SPI_FORCE is 1  then it indicates a programming error.*/
#define EFUSE_VDD_SPI_FORCE_ERR  (BIT(6))
#define EFUSE_VDD_SPI_FORCE_ERR_M  (BIT(6))
#define EFUSE_VDD_SPI_FORCE_ERR_V  0x1
#define EFUSE_VDD_SPI_FORCE_ERR_S  6
/* EFUSE_VDD_SPI_TIEH_ERR : RO ;bitpos:[5] ;default: 1'h0 ; */
/*description: If VDD_SPI_TIEH is 1  then it indicates a programming error.*/
#define EFUSE_VDD_SPI_TIEH_ERR  (BIT(5))
#define EFUSE_VDD_SPI_TIEH_ERR_M  (BIT(5))
#define EFUSE_VDD_SPI_TIEH_ERR_V  0x1
#define EFUSE_VDD_SPI_TIEH_ERR_S  5
/* EFUSE_VDD_SPI_XPD_ERR : RO ;bitpos:[4] ;default: 1'h0 ; */
/*description: If VDD_SPI_XPD is 1  then it indicates a programming error.*/
#define EFUSE_VDD_SPI_XPD_ERR  (BIT(4))
#define EFUSE_VDD_SPI_XPD_ERR_M  (BIT(4))
#define EFUSE_VDD_SPI_XPD_ERR_V  0x1
#define EFUSE_VDD_SPI_XPD_ERR_S  4
/* EFUSE_VDD_SPI_DREFL_ERR : RO ;bitpos:[3:2] ;default: 2'h0 ; */
/*description: If any bit in VDD_SPI_DREFL is 1  then it indicates a programming error.*/
#define EFUSE_VDD_SPI_DREFL_ERR  0x00000003
#define EFUSE_VDD_SPI_DREFL_ERR_M  ((EFUSE_VDD_SPI_DREFL_ERR_V)<<(EFUSE_VDD_SPI_DREFL_ERR_S))
#define EFUSE_VDD_SPI_DREFL_ERR_V  0x3
#define EFUSE_VDD_SPI_DREFL_ERR_S  2
/* EFUSE_VDD_SPI_DREFM_ERR : RO ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: If any bit in VDD_SPI_DREFM is 1  then it indicates a programming error.*/
#define EFUSE_VDD_SPI_DREFM_ERR  0x00000003
#define EFUSE_VDD_SPI_DREFM_ERR_M  ((EFUSE_VDD_SPI_DREFM_ERR_V)<<(EFUSE_VDD_SPI_DREFM_ERR_S))
#define EFUSE_VDD_SPI_DREFM_ERR_V  0x3
#define EFUSE_VDD_SPI_DREFM_ERR_S  0

#define EFUSE_RD_REPEAT_ERR2_REG          (DR_REG_EFUSE_BASE + 0x184)
/* EFUSE_FLASH_TPUW_ERR : RO ;bitpos:[31:28] ;default: 4'h0 ; */
/*description: If any bit in FLASH_TPUM is 1  then it indicates a programming error.*/
#define EFUSE_FLASH_TPUW_ERR  0x0000000F
#define EFUSE_FLASH_TPUW_ERR_M  ((EFUSE_FLASH_TPUW_ERR_V)<<(EFUSE_FLASH_TPUW_ERR_S))
#define EFUSE_FLASH_TPUW_ERR_V  0xF
#define EFUSE_FLASH_TPUW_ERR_S  28
/* EFUSE_RPT4_RESERVED1_ERR : RO ;bitpos:[27:22] ;default: 6'h0 ; */
/*description: Reserved.*/
#define EFUSE_RPT4_RESERVED1_ERR  0x0000003F
#define EFUSE_RPT4_RESERVED1_ERR_M  ((EFUSE_RPT4_RESERVED1_ERR_V)<<(EFUSE_RPT4_RESERVED1_ERR_S))
#define EFUSE_RPT4_RESERVED1_ERR_V  0x3F
#define EFUSE_RPT4_RESERVED1_ERR_S  22
/* EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR : RO ;bitpos:[21] ;default: 1'h0 ; */
/*description: If SECURE_BOOT_AGGRESSIVE_REVOKE is 1  then it indicates a programming error.*/
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR  (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_M  (BIT(21))
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_V  0x1
#define EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE_ERR_S  21
/* EFUSE_SECURE_BOOT_EN_ERR : RO ;bitpos:[20] ;default: 1'h0 ; */
/*description: If SECURE_BOOT_EN is 1  then it indicates a programming error.*/
#define EFUSE_SECURE_BOOT_EN_ERR  (BIT(20))
#define EFUSE_SECURE_BOOT_EN_ERR_M  (BIT(20))
#define EFUSE_SECURE_BOOT_EN_ERR_V  0x1
#define EFUSE_SECURE_BOOT_EN_ERR_S  20
/* EFUSE_KEY_PURPOSE_6_ERR : RO ;bitpos:[19:16] ;default: 4'h0 ; */
/*description: If any bit in KEY_PURPOSE_6 is 1  then it indicates a programming error.*/
#define EFUSE_KEY_PURPOSE_6_ERR  0x0000000F
#define EFUSE_KEY_PURPOSE_6_ERR_M  ((EFUSE_KEY_PURPOSE_6_ERR_V)<<(EFUSE_KEY_PURPOSE_6_ERR_S))
#define EFUSE_KEY_PURPOSE_6_ERR_V  0xF
#define EFUSE_KEY_PURPOSE_6_ERR_S  16
/* EFUSE_KEY_PURPOSE_5_ERR : RO ;bitpos:[15:12] ;default: 4'h0 ; */
/*description: If any bit in KEY_PURPOSE_5 is 1  then it indicates a programming error.*/
#define EFUSE_KEY_PURPOSE_5_ERR  0x0000000F
#define EFUSE_KEY_PURPOSE_5_ERR_M  ((EFUSE_KEY_PURPOSE_5_ERR_V)<<(EFUSE_KEY_PURPOSE_5_ERR_S))
#define EFUSE_KEY_PURPOSE_5_ERR_V  0xF
#define EFUSE_KEY_PURPOSE_5_ERR_S  12
/* EFUSE_KEY_PURPOSE_4_ERR : RO ;bitpos:[11:8] ;default: 4'h0 ; */
/*description: If any bit in KEY_PURPOSE_4 is 1  then it indicates a programming error.*/
#define EFUSE_KEY_PURPOSE_4_ERR  0x0000000F
#define EFUSE_KEY_PURPOSE_4_ERR_M  ((EFUSE_KEY_PURPOSE_4_ERR_V)<<(EFUSE_KEY_PURPOSE_4_ERR_S))
#define EFUSE_KEY_PURPOSE_4_ERR_V  0xF
#define EFUSE_KEY_PURPOSE_4_ERR_S  8
/* EFUSE_KEY_PURPOSE_3_ERR : RO ;bitpos:[7:4] ;default: 4'h0 ; */
/*description: If any bit in KEY_PURPOSE_3 is 1  then it indicates a programming error.*/
#define EFUSE_KEY_PURPOSE_3_ERR  0x0000000F
#define EFUSE_KEY_PURPOSE_3_ERR_M  ((EFUSE_KEY_PURPOSE_3_ERR_V)<<(EFUSE_KEY_PURPOSE_3_ERR_S))
#define EFUSE_KEY_PURPOSE_3_ERR_V  0xF
#define EFUSE_KEY_PURPOSE_3_ERR_S  4
/* EFUSE_KEY_PURPOSE_2_ERR : RO ;bitpos:[3:0] ;default: 4'h0 ; */
/*description: If any bit in KEY_PURPOSE_2 is 1  then it indicates a programming error.*/
#define EFUSE_KEY_PURPOSE_2_ERR  0x0000000F
#define EFUSE_KEY_PURPOSE_2_ERR_M  ((EFUSE_KEY_PURPOSE_2_ERR_V)<<(EFUSE_KEY_PURPOSE_2_ERR_S))
#define EFUSE_KEY_PURPOSE_2_ERR_V  0xF
#define EFUSE_KEY_PURPOSE_2_ERR_S  0

#define EFUSE_RD_REPEAT_ERR3_REG          (DR_REG_EFUSE_BASE + 0x188)
/* EFUSE_RPT4_RESERVED2_ERR : RO ;bitpos:[31:27] ;default: 5'h0 ; */
/*description: Reserved.*/
#define EFUSE_RPT4_RESERVED2_ERR  0x0000001F
#define EFUSE_RPT4_RESERVED2_ERR_M  ((EFUSE_RPT4_RESERVED2_ERR_V)<<(EFUSE_RPT4_RESERVED2_ERR_S))
#define EFUSE_RPT4_RESERVED2_ERR_V  0x1F
#define EFUSE_RPT4_RESERVED2_ERR_S  27
/* EFUSE_SECURE_VERSION_ERR : RO ;bitpos:[26:11] ;default: 16'h0 ; */
/*description: If any bit in SECURE_VERSION is 1  then it indicates a programming error.*/
#define EFUSE_SECURE_VERSION_ERR  0x0000FFFF
#define EFUSE_SECURE_VERSION_ERR_M  ((EFUSE_SECURE_VERSION_ERR_V)<<(EFUSE_SECURE_VERSION_ERR_S))
#define EFUSE_SECURE_VERSION_ERR_V  0xFFFF
#define EFUSE_SECURE_VERSION_ERR_S  11
/* EFUSE_FORCE_SEND_RESUME_ERR : RO ;bitpos:[10] ;default: 1'h0 ; */
/*description: If FORCE_SEND_RESUME is 1  then it indicates a programming error.*/
#define EFUSE_FORCE_SEND_RESUME_ERR  (BIT(10))
#define EFUSE_FORCE_SEND_RESUME_ERR_M  (BIT(10))
#define EFUSE_FORCE_SEND_RESUME_ERR_V  0x1
#define EFUSE_FORCE_SEND_RESUME_ERR_S  10
/* EFUSE_FLASH_TYPE_ERR : RO ;bitpos:[9] ;default: 1'h0 ; */
/*description: If FLASH_TYPE is 1  then it indicates a programming error.*/
#define EFUSE_FLASH_TYPE_ERR  (BIT(9))
#define EFUSE_FLASH_TYPE_ERR_M  (BIT(9))
#define EFUSE_FLASH_TYPE_ERR_V  0x1
#define EFUSE_FLASH_TYPE_ERR_S  9
/* EFUSE_PIN_POWER_SELECTION_ERR : RO ;bitpos:[8] ;default: 1'h0 ; */
/*description: If PIN_POWER_SELECTION is 1  then it indicates a programming error.*/
#define EFUSE_PIN_POWER_SELECTION_ERR  (BIT(8))
#define EFUSE_PIN_POWER_SELECTION_ERR_M  (BIT(8))
#define EFUSE_PIN_POWER_SELECTION_ERR_V  0x1
#define EFUSE_PIN_POWER_SELECTION_ERR_S  8
/* EFUSE_UART_PRINT_CONTROL_ERR : RO ;bitpos:[7:6] ;default: 2'h0 ; */
/*description: If any bit in UART_PRINT_CONTROL is 1  then it indicates a programming error.*/
#define EFUSE_UART_PRINT_CONTROL_ERR  0x00000003
#define EFUSE_UART_PRINT_CONTROL_ERR_M  ((EFUSE_UART_PRINT_CONTROL_ERR_V)<<(EFUSE_UART_PRINT_CONTROL_ERR_S))
#define EFUSE_UART_PRINT_CONTROL_ERR_V  0x3
#define EFUSE_UART_PRINT_CONTROL_ERR_S  6
/* EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR : RO ;bitpos:[5] ;default: 1'h0 ; */
/*description: If ENABLE_SECURITY_DOWNLOAD is 1  then it indicates a programming error.*/
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR  (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_M  (BIT(5))
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_V  0x1
#define EFUSE_ENABLE_SECURITY_DOWNLOAD_ERR_S  5
/* EFUSE_DIS_USB_DOWNLOAD_MODE_ERR : RO ;bitpos:[4] ;default: 1'h0 ; */
/*description: If DIS_USB_DOWNLOAD_MODE is 1  then it indicates a programming error.*/
#define EFUSE_DIS_USB_DOWNLOAD_MODE_ERR  (BIT(4))
#define EFUSE_DIS_USB_DOWNLOAD_MODE_ERR_M  (BIT(4))
#define EFUSE_DIS_USB_DOWNLOAD_MODE_ERR_V  0x1
#define EFUSE_DIS_USB_DOWNLOAD_MODE_ERR_S  4
/* EFUSE_RPT4_RESERVED3_ERR : RO ;bitpos:[3] ;default: 1'h0 ; */
/*description: If RPT4_RESERVED3 is 1  then it indicates a programming error.*/
#define EFUSE_RPT4_RESERVED3_ERR  (BIT(3))
#define EFUSE_RPT4_RESERVED3_ERR_M  (BIT(3))
#define EFUSE_RPT4_RESERVED3_ERR_V  0x1
#define EFUSE_RPT4_RESERVED3_ERR_S  3
/* EFUSE_UART_PRINT_CHANNEL_ERR : RO ;bitpos:[2] ;default: 1'h0 ; */
/*description: If UART_PRINT_CHANNEL is 1  then it indicates a programming error.*/
#define EFUSE_UART_PRINT_CHANNEL_ERR  (BIT(2))
#define EFUSE_UART_PRINT_CHANNEL_ERR_M  (BIT(2))
#define EFUSE_UART_PRINT_CHANNEL_ERR_V  0x1
#define EFUSE_UART_PRINT_CHANNEL_ERR_S  2
/* EFUSE_DIS_LEGACY_SPI_BOOT_ERR : RO ;bitpos:[1] ;default: 1'h0 ; */
/*description: If DIS_LEGACY_SPI_BOOT is 1  then it indicates a programming error.*/
#define EFUSE_DIS_LEGACY_SPI_BOOT_ERR  (BIT(1))
#define EFUSE_DIS_LEGACY_SPI_BOOT_ERR_M  (BIT(1))
#define EFUSE_DIS_LEGACY_SPI_BOOT_ERR_V  0x1
#define EFUSE_DIS_LEGACY_SPI_BOOT_ERR_S  1
/* EFUSE_DIS_DOWNLOAD_MODE_ERR : RO ;bitpos:[0] ;default: 1'h0 ; */
/*description: If DIS_DOWNLOAD_MODE is 1  then it indicates a programming error.*/
#define EFUSE_DIS_DOWNLOAD_MODE_ERR  (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_ERR_M  (BIT(0))
#define EFUSE_DIS_DOWNLOAD_MODE_ERR_V  0x1
#define EFUSE_DIS_DOWNLOAD_MODE_ERR_S  0

#define EFUSE_RD_REPEAT_ERR4_REG          (DR_REG_EFUSE_BASE + 0x190)
/* EFUSE_RPT1_RESERVED0_ERR : RO ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: Reserved.*/
#define EFUSE_RPT1_RESERVED0_ERR  0x000000FF
#define EFUSE_RPT1_RESERVED0_ERR_M  ((EFUSE_RPT1_RESERVED0_ERR_V)<<(EFUSE_RPT1_RESERVED0_ERR_S))
#define EFUSE_RPT1_RESERVED0_ERR_V  0xFF
#define EFUSE_RPT1_RESERVED0_ERR_S  24
/* EFUSE_RPT4_RESERVED4_ERR : RO ;bitpos:[23:0] ;default: 24'h0 ; */
/*description: Reserved ( four backup method ).*/
#define EFUSE_RPT4_RESERVED4_ERR  0x00FFFFFF
#define EFUSE_RPT4_RESERVED4_ERR_M  ((EFUSE_RPT4_RESERVED4_ERR_V)<<(EFUSE_RPT4_RESERVED4_ERR_S))
#define EFUSE_RPT4_RESERVED4_ERR_V  0xFFFFFF
#define EFUSE_RPT4_RESERVED4_ERR_S  0

#define EFUSE_RD_RS_ERR0_REG          (DR_REG_EFUSE_BASE + 0x1c0)
/* EFUSE_KEY4_FAIL : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description: 0: Means no failure and that the data of key$n is reliable*/
#define EFUSE_KEY4_FAIL  (BIT(31))
#define EFUSE_KEY4_FAIL_M  (BIT(31))
#define EFUSE_KEY4_FAIL_V  0x1
#define EFUSE_KEY4_FAIL_S  31
/* EFUSE_KEY4_ERR_NUM : RO ;bitpos:[30:28] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_KEY4_ERR_NUM  0x00000007
#define EFUSE_KEY4_ERR_NUM_M  ((EFUSE_KEY4_ERR_NUM_V)<<(EFUSE_KEY4_ERR_NUM_S))
#define EFUSE_KEY4_ERR_NUM_V  0x7
#define EFUSE_KEY4_ERR_NUM_S  28
/* EFUSE_KEY3_FAIL : RO ;bitpos:[27] ;default: 1'h0 ; */
/*description: 0: Means no failure and that the data of key$n is reliable*/
#define EFUSE_KEY3_FAIL  (BIT(27))
#define EFUSE_KEY3_FAIL_M  (BIT(27))
#define EFUSE_KEY3_FAIL_V  0x1
#define EFUSE_KEY3_FAIL_S  27
/* EFUSE_KEY3_ERR_NUM : RO ;bitpos:[26:24] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_KEY3_ERR_NUM  0x00000007
#define EFUSE_KEY3_ERR_NUM_M  ((EFUSE_KEY3_ERR_NUM_V)<<(EFUSE_KEY3_ERR_NUM_S))
#define EFUSE_KEY3_ERR_NUM_V  0x7
#define EFUSE_KEY3_ERR_NUM_S  24
/* EFUSE_KEY2_FAIL : RO ;bitpos:[23] ;default: 1'h0 ; */
/*description: 0: Means no failure and that the data of key$n is reliable*/
#define EFUSE_KEY2_FAIL  (BIT(23))
#define EFUSE_KEY2_FAIL_M  (BIT(23))
#define EFUSE_KEY2_FAIL_V  0x1
#define EFUSE_KEY2_FAIL_S  23
/* EFUSE_KEY2_ERR_NUM : RO ;bitpos:[22:20] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_KEY2_ERR_NUM  0x00000007
#define EFUSE_KEY2_ERR_NUM_M  ((EFUSE_KEY2_ERR_NUM_V)<<(EFUSE_KEY2_ERR_NUM_S))
#define EFUSE_KEY2_ERR_NUM_V  0x7
#define EFUSE_KEY2_ERR_NUM_S  20
/* EFUSE_KEY1_FAIL : RO ;bitpos:[19] ;default: 1'h0 ; */
/*description: 0: Means no failure and that the data of key$n is reliable*/
#define EFUSE_KEY1_FAIL  (BIT(19))
#define EFUSE_KEY1_FAIL_M  (BIT(19))
#define EFUSE_KEY1_FAIL_V  0x1
#define EFUSE_KEY1_FAIL_S  19
/* EFUSE_KEY1_ERR_NUM : RO ;bitpos:[18:16] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_KEY1_ERR_NUM  0x00000007
#define EFUSE_KEY1_ERR_NUM_M  ((EFUSE_KEY1_ERR_NUM_V)<<(EFUSE_KEY1_ERR_NUM_S))
#define EFUSE_KEY1_ERR_NUM_V  0x7
#define EFUSE_KEY1_ERR_NUM_S  16
/* EFUSE_KEY0_FAIL : RO ;bitpos:[15] ;default: 1'h0 ; */
/*description: 0: Means no failure and that the data of key$n is reliable*/
#define EFUSE_KEY0_FAIL  (BIT(15))
#define EFUSE_KEY0_FAIL_M  (BIT(15))
#define EFUSE_KEY0_FAIL_V  0x1
#define EFUSE_KEY0_FAIL_S  15
/* EFUSE_KEY0_ERR_NUM : RO ;bitpos:[14:12] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_KEY0_ERR_NUM  0x00000007
#define EFUSE_KEY0_ERR_NUM_M  ((EFUSE_KEY0_ERR_NUM_V)<<(EFUSE_KEY0_ERR_NUM_S))
#define EFUSE_KEY0_ERR_NUM_V  0x7
#define EFUSE_KEY0_ERR_NUM_S  12
/* EFUSE_USR_DATA_FAIL : RO ;bitpos:[11] ;default: 1'h0 ; */
/*description: 0: Means no failure and that the user data is reliable*/
#define EFUSE_USR_DATA_FAIL  (BIT(11))
#define EFUSE_USR_DATA_FAIL_M  (BIT(11))
#define EFUSE_USR_DATA_FAIL_V  0x1
#define EFUSE_USR_DATA_FAIL_S  11
/* EFUSE_USR_DATA_ERR_NUM : RO ;bitpos:[10:8] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_USR_DATA_ERR_NUM  0x00000007
#define EFUSE_USR_DATA_ERR_NUM_M  ((EFUSE_USR_DATA_ERR_NUM_V)<<(EFUSE_USR_DATA_ERR_NUM_S))
#define EFUSE_USR_DATA_ERR_NUM_V  0x7
#define EFUSE_USR_DATA_ERR_NUM_S  8
/* EFUSE_SYS_PART1_FAIL : RO ;bitpos:[7] ;default: 1'h0 ; */
/*description: 0: Means no failure and that the data of system part1 is reliable*/
#define EFUSE_SYS_PART1_FAIL  (BIT(7))
#define EFUSE_SYS_PART1_FAIL_M  (BIT(7))
#define EFUSE_SYS_PART1_FAIL_V  0x1
#define EFUSE_SYS_PART1_FAIL_S  7
/* EFUSE_SYS_PART1_NUM : RO ;bitpos:[6:4] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_SYS_PART1_NUM  0x00000007
#define EFUSE_SYS_PART1_NUM_M  ((EFUSE_SYS_PART1_NUM_V)<<(EFUSE_SYS_PART1_NUM_S))
#define EFUSE_SYS_PART1_NUM_V  0x7
#define EFUSE_SYS_PART1_NUM_S  4
/* EFUSE_MAC_SPI_8M_FAIL : RO ;bitpos:[3] ;default: 1'h0 ; */
/*description: 0: Means no failure and that the data of MAC_SPI_8M is reliable*/
#define EFUSE_MAC_SPI_8M_FAIL  (BIT(3))
#define EFUSE_MAC_SPI_8M_FAIL_M  (BIT(3))
#define EFUSE_MAC_SPI_8M_FAIL_V  0x1
#define EFUSE_MAC_SPI_8M_FAIL_S  3
/* EFUSE_MAC_SPI_8M_ERR_NUM : RO ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_MAC_SPI_8M_ERR_NUM  0x00000007
#define EFUSE_MAC_SPI_8M_ERR_NUM_M  ((EFUSE_MAC_SPI_8M_ERR_NUM_V)<<(EFUSE_MAC_SPI_8M_ERR_NUM_S))
#define EFUSE_MAC_SPI_8M_ERR_NUM_V  0x7
#define EFUSE_MAC_SPI_8M_ERR_NUM_S  0

#define EFUSE_RD_RS_ERR1_REG          (DR_REG_EFUSE_BASE + 0x1c4)
/* EFUSE_SYS_PART2_FAIL : RO ;bitpos:[7] ;default: 1'h0 ; */
/*description: 0: Means no failure and that the data of system part2 is reliable*/
#define EFUSE_SYS_PART2_FAIL  (BIT(7))
#define EFUSE_SYS_PART2_FAIL_M  (BIT(7))
#define EFUSE_SYS_PART2_FAIL_V  0x1
#define EFUSE_SYS_PART2_FAIL_S  7
/* EFUSE_SYS_PART2_ERR_NUM : RO ;bitpos:[6:4] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_SYS_PART2_ERR_NUM  0x00000007
#define EFUSE_SYS_PART2_ERR_NUM_M  ((EFUSE_SYS_PART2_ERR_NUM_V)<<(EFUSE_SYS_PART2_ERR_NUM_S))
#define EFUSE_SYS_PART2_ERR_NUM_V  0x7
#define EFUSE_SYS_PART2_ERR_NUM_S  4
/* EFUSE_KEY5_FAIL : RO ;bitpos:[3] ;default: 1'h0 ; */
/*description: 0: Means no failure and that the data of KEY5 is reliable*/
#define EFUSE_KEY5_FAIL  (BIT(3))
#define EFUSE_KEY5_FAIL_M  (BIT(3))
#define EFUSE_KEY5_FAIL_V  0x1
#define EFUSE_KEY5_FAIL_S  3
/* EFUSE_KEY5_ERR_NUM : RO ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_KEY5_ERR_NUM  0x00000007
#define EFUSE_KEY5_ERR_NUM_M  ((EFUSE_KEY5_ERR_NUM_V)<<(EFUSE_KEY5_ERR_NUM_S))
#define EFUSE_KEY5_ERR_NUM_V  0x7
#define EFUSE_KEY5_ERR_NUM_S  0

#define EFUSE_CLK_REG          (DR_REG_EFUSE_BASE + 0x1c8)
/* EFUSE_CLK_EN : R/W ;bitpos:[16] ;default: 1'h0 ; */
/*description: Set this bit and force to enable clock signal of eFuse memory.*/
#define EFUSE_CLK_EN  (BIT(16))
#define EFUSE_CLK_EN_M  (BIT(16))
#define EFUSE_CLK_EN_V  0x1
#define EFUSE_CLK_EN_S  16
/* EFUSE_MEM_FORCE_PU : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: Set this bit to force eFuse SRAM into working mode.*/
#define EFUSE_MEM_FORCE_PU  (BIT(2))
#define EFUSE_MEM_FORCE_PU_M  (BIT(2))
#define EFUSE_MEM_FORCE_PU_V  0x1
#define EFUSE_MEM_FORCE_PU_S  2
/* EFUSE_MEM_CLK_FORCE_ON : R/W ;bitpos:[1] ;default: 1'h1 ; */
/*description: Set this bit and force to activate clock signal of eFuse SRAM.*/
#define EFUSE_MEM_CLK_FORCE_ON  (BIT(1))
#define EFUSE_MEM_CLK_FORCE_ON_M  (BIT(1))
#define EFUSE_MEM_CLK_FORCE_ON_V  0x1
#define EFUSE_MEM_CLK_FORCE_ON_S  1
/* EFUSE_MEM_FORCE_PD : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: Set this bit to force eFuse SRAM into power-saving mode.*/
#define EFUSE_MEM_FORCE_PD  (BIT(0))
#define EFUSE_MEM_FORCE_PD_M  (BIT(0))
#define EFUSE_MEM_FORCE_PD_V  0x1
#define EFUSE_MEM_FORCE_PD_S  0

#define EFUSE_CONF_REG          (DR_REG_EFUSE_BASE + 0x1cc)
/* EFUSE_OP_CODE : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: 0x5A5A: Operate programming command*/
#define EFUSE_OP_CODE  0x0000FFFF
#define EFUSE_OP_CODE_M  ((EFUSE_OP_CODE_V)<<(EFUSE_OP_CODE_S))
#define EFUSE_OP_CODE_V  0xFFFF
#define EFUSE_OP_CODE_S  0

#define EFUSE_STATUS_REG          (DR_REG_EFUSE_BASE + 0x1d0)
/* EFUSE_REPEAT_ERR_CNT : RO ;bitpos:[17:10] ;default: 8'h0 ; */
/*description: Indicates the number of error bits during programming BLOCK0.*/
#define EFUSE_REPEAT_ERR_CNT  0x000000FF
#define EFUSE_REPEAT_ERR_CNT_M  ((EFUSE_REPEAT_ERR_CNT_V)<<(EFUSE_REPEAT_ERR_CNT_S))
#define EFUSE_REPEAT_ERR_CNT_V  0xFF
#define EFUSE_REPEAT_ERR_CNT_S  10
/* EFUSE_OTP_VDDQ_IS_SW : RO ;bitpos:[9] ;default: 1'h0 ; */
/*description: The value of OTP_VDDQ_IS_SW.*/
#define EFUSE_OTP_VDDQ_IS_SW  (BIT(9))
#define EFUSE_OTP_VDDQ_IS_SW_M  (BIT(9))
#define EFUSE_OTP_VDDQ_IS_SW_V  0x1
#define EFUSE_OTP_VDDQ_IS_SW_S  9
/* EFUSE_OTP_PGENB_SW : RO ;bitpos:[8] ;default: 1'h0 ; */
/*description: The value of OTP_PGENB_SW.*/
#define EFUSE_OTP_PGENB_SW  (BIT(8))
#define EFUSE_OTP_PGENB_SW_M  (BIT(8))
#define EFUSE_OTP_PGENB_SW_V  0x1
#define EFUSE_OTP_PGENB_SW_S  8
/* EFUSE_OTP_CSB_SW : RO ;bitpos:[7] ;default: 1'h0 ; */
/*description: The value of OTP_CSB_SW.*/
#define EFUSE_OTP_CSB_SW  (BIT(7))
#define EFUSE_OTP_CSB_SW_M  (BIT(7))
#define EFUSE_OTP_CSB_SW_V  0x1
#define EFUSE_OTP_CSB_SW_S  7
/* EFUSE_OTP_STROBE_SW : RO ;bitpos:[6] ;default: 1'h0 ; */
/*description: The value of OTP_STROBE_SW.*/
#define EFUSE_OTP_STROBE_SW  (BIT(6))
#define EFUSE_OTP_STROBE_SW_M  (BIT(6))
#define EFUSE_OTP_STROBE_SW_V  0x1
#define EFUSE_OTP_STROBE_SW_S  6
/* EFUSE_OTP_VDDQ_C_SYNC2 : RO ;bitpos:[5] ;default: 1'h0 ; */
/*description: The value of OTP_VDDQ_C_SYNC2.*/
#define EFUSE_OTP_VDDQ_C_SYNC2  (BIT(5))
#define EFUSE_OTP_VDDQ_C_SYNC2_M  (BIT(5))
#define EFUSE_OTP_VDDQ_C_SYNC2_V  0x1
#define EFUSE_OTP_VDDQ_C_SYNC2_S  5
/* EFUSE_OTP_LOAD_SW : RO ;bitpos:[4] ;default: 1'h0 ; */
/*description: The value of OTP_LOAD_SW.*/
#define EFUSE_OTP_LOAD_SW  (BIT(4))
#define EFUSE_OTP_LOAD_SW_M  (BIT(4))
#define EFUSE_OTP_LOAD_SW_V  0x1
#define EFUSE_OTP_LOAD_SW_S  4
/* EFUSE_STATE : RO ;bitpos:[3:0] ;default: 4'h0 ; */
/*description: Indicates the state of the eFuse state machine.*/
#define EFUSE_STATE  0x0000000F
#define EFUSE_STATE_M  ((EFUSE_STATE_V)<<(EFUSE_STATE_S))
#define EFUSE_STATE_V  0xF
#define EFUSE_STATE_S  0

#define EFUSE_CMD_REG          (DR_REG_EFUSE_BASE + 0x1d4)
/* EFUSE_BLK_NUM : R/W ;bitpos:[5:2] ;default: 4'h0 ; */
/*description: The serial number of the block to be programmed. Value 0-10 corresponds
 to block number 0-10  respectively.*/
#define EFUSE_BLK_NUM  0x0000000F
#define EFUSE_BLK_NUM_M  ((EFUSE_BLK_NUM_V)<<(EFUSE_BLK_NUM_S))
#define EFUSE_BLK_NUM_V  0xF
#define EFUSE_BLK_NUM_S  2
/* EFUSE_PGM_CMD : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: Set this bit to send programming command.*/
#define EFUSE_PGM_CMD  (BIT(1))
#define EFUSE_PGM_CMD_M  (BIT(1))
#define EFUSE_PGM_CMD_V  0x1
#define EFUSE_PGM_CMD_S  1
/* EFUSE_READ_CMD : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: Set this bit to send read command.*/
#define EFUSE_READ_CMD  (BIT(0))
#define EFUSE_READ_CMD_M  (BIT(0))
#define EFUSE_READ_CMD_V  0x1
#define EFUSE_READ_CMD_S  0

#define EFUSE_INT_RAW_REG          (DR_REG_EFUSE_BASE + 0x1d8)
/* EFUSE_PGM_DONE_INT_RAW : RO ;bitpos:[1] ;default: 1'h0 ; */
/*description: The raw bit signal for pgm_done interrupt.*/
#define EFUSE_PGM_DONE_INT_RAW  (BIT(1))
#define EFUSE_PGM_DONE_INT_RAW_M  (BIT(1))
#define EFUSE_PGM_DONE_INT_RAW_V  0x1
#define EFUSE_PGM_DONE_INT_RAW_S  1
/* EFUSE_READ_DONE_INT_RAW : RO ;bitpos:[0] ;default: 1'h0 ; */
/*description: The raw bit signal for read_done interrupt.*/
#define EFUSE_READ_DONE_INT_RAW  (BIT(0))
#define EFUSE_READ_DONE_INT_RAW_M  (BIT(0))
#define EFUSE_READ_DONE_INT_RAW_V  0x1
#define EFUSE_READ_DONE_INT_RAW_S  0

#define EFUSE_INT_ST_REG          (DR_REG_EFUSE_BASE + 0x1dc)
/* EFUSE_PGM_DONE_INT_ST : RO ;bitpos:[1] ;default: 1'h0 ; */
/*description: The status signal for pgm_done interrupt.*/
#define EFUSE_PGM_DONE_INT_ST  (BIT(1))
#define EFUSE_PGM_DONE_INT_ST_M  (BIT(1))
#define EFUSE_PGM_DONE_INT_ST_V  0x1
#define EFUSE_PGM_DONE_INT_ST_S  1
/* EFUSE_READ_DONE_INT_ST : RO ;bitpos:[0] ;default: 1'h0 ; */
/*description: The status signal for read_done interrupt.*/
#define EFUSE_READ_DONE_INT_ST  (BIT(0))
#define EFUSE_READ_DONE_INT_ST_M  (BIT(0))
#define EFUSE_READ_DONE_INT_ST_V  0x1
#define EFUSE_READ_DONE_INT_ST_S  0

#define EFUSE_INT_ENA_REG          (DR_REG_EFUSE_BASE + 0x1e0)
/* EFUSE_PGM_DONE_INT_ENA : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: The enable signal for pgm_done interrupt.*/
#define EFUSE_PGM_DONE_INT_ENA  (BIT(1))
#define EFUSE_PGM_DONE_INT_ENA_M  (BIT(1))
#define EFUSE_PGM_DONE_INT_ENA_V  0x1
#define EFUSE_PGM_DONE_INT_ENA_S  1
/* EFUSE_READ_DONE_INT_ENA : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: The enable signal for read_done interrupt.*/
#define EFUSE_READ_DONE_INT_ENA  (BIT(0))
#define EFUSE_READ_DONE_INT_ENA_M  (BIT(0))
#define EFUSE_READ_DONE_INT_ENA_V  0x1
#define EFUSE_READ_DONE_INT_ENA_S  0

#define EFUSE_INT_CLR_REG          (DR_REG_EFUSE_BASE + 0x1e4)
/* EFUSE_PGM_DONE_INT_CLR : WO ;bitpos:[1] ;default: 1'h0 ; */
/*description: The clear signal for pgm_done interrupt.*/
#define EFUSE_PGM_DONE_INT_CLR  (BIT(1))
#define EFUSE_PGM_DONE_INT_CLR_M  (BIT(1))
#define EFUSE_PGM_DONE_INT_CLR_V  0x1
#define EFUSE_PGM_DONE_INT_CLR_S  1
/* EFUSE_READ_DONE_INT_CLR : WO ;bitpos:[0] ;default: 1'h0 ; */
/*description: The clear signal for read_done interrupt.*/
#define EFUSE_READ_DONE_INT_CLR  (BIT(0))
#define EFUSE_READ_DONE_INT_CLR_M  (BIT(0))
#define EFUSE_READ_DONE_INT_CLR_V  0x1
#define EFUSE_READ_DONE_INT_CLR_S  0

#define EFUSE_DAC_CONF_REG          (DR_REG_EFUSE_BASE + 0x1e8)
/* EFUSE_OE_CLR : R/W ;bitpos:[17] ;default: 1'h0 ; */
/*description: Reduces the power supply of the programming voltage.*/
#define EFUSE_OE_CLR  (BIT(17))
#define EFUSE_OE_CLR_M  (BIT(17))
#define EFUSE_OE_CLR_V  0x1
#define EFUSE_OE_CLR_S  17
/* EFUSE_DAC_NUM : R/W ;bitpos:[16:9] ;default: 8'd255 ; */
/*description: Controls the rising period of the programming voltage.*/
#define EFUSE_DAC_NUM  0x000000FF
#define EFUSE_DAC_NUM_M  ((EFUSE_DAC_NUM_V)<<(EFUSE_DAC_NUM_S))
#define EFUSE_DAC_NUM_V  0xFF
#define EFUSE_DAC_NUM_S  9
/* EFUSE_DAC_CLK_PAD_SEL : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: Don't care.*/
#define EFUSE_DAC_CLK_PAD_SEL  (BIT(8))
#define EFUSE_DAC_CLK_PAD_SEL_M  (BIT(8))
#define EFUSE_DAC_CLK_PAD_SEL_V  0x1
#define EFUSE_DAC_CLK_PAD_SEL_S  8
/* EFUSE_DAC_CLK_DIV : R/W ;bitpos:[7:0] ;default: 8'd28 ; */
/*description: Controls the division factor of the rising clock of the programming voltage.*/
#define EFUSE_DAC_CLK_DIV  0x000000FF
#define EFUSE_DAC_CLK_DIV_M  ((EFUSE_DAC_CLK_DIV_V)<<(EFUSE_DAC_CLK_DIV_S))
#define EFUSE_DAC_CLK_DIV_V  0xFF
#define EFUSE_DAC_CLK_DIV_S  0

#define EFUSE_RD_TIM_CONF_REG          (DR_REG_EFUSE_BASE + 0x1ec)
/* EFUSE_READ_INIT_NUM : R/W ;bitpos:[31:24] ;default: 8'h12 ; */
/*description: Configures the initial read time of eFuse.*/
#define EFUSE_READ_INIT_NUM  0x000000FF
#define EFUSE_READ_INIT_NUM_M  ((EFUSE_READ_INIT_NUM_V)<<(EFUSE_READ_INIT_NUM_S))
#define EFUSE_READ_INIT_NUM_V  0xFF
#define EFUSE_READ_INIT_NUM_S  24
/* EFUSE_TSUR_A : R/W ;bitpos:[23:16] ;default: 8'h1 ; */
/*description: Configures the setup time of read operation.*/
#define EFUSE_TSUR_A  0x000000FF
#define EFUSE_TSUR_A_M  ((EFUSE_TSUR_A_V)<<(EFUSE_TSUR_A_S))
#define EFUSE_TSUR_A_V  0xFF
#define EFUSE_TSUR_A_S  16
/* EFUSE_TRD : R/W ;bitpos:[15:8] ;default: 8'h1 ; */
/*description: Configures the length of pulse of read operation.*/
#define EFUSE_TRD  0x000000FF
#define EFUSE_TRD_M  ((EFUSE_TRD_V)<<(EFUSE_TRD_S))
#define EFUSE_TRD_V  0xFF
#define EFUSE_TRD_S  8
/* EFUSE_THR_A : R/W ;bitpos:[7:0] ;default: 8'h1 ; */
/*description: Configures the hold time of read operation.*/
#define EFUSE_THR_A  0x000000FF
#define EFUSE_THR_A_M  ((EFUSE_THR_A_V)<<(EFUSE_THR_A_S))
#define EFUSE_THR_A_V  0xFF
#define EFUSE_THR_A_S  0

#define EFUSE_WR_TIM_CONF0_REG          (DR_REG_EFUSE_BASE + 0x1F0)
/* EFUSE_TPGM : R/W ;bitpos:[31:16] ;default: 16'hc8 ; */
/*description: Configures the length of pulse during programming 1 to eFuse.*/
#define EFUSE_TPGM  0x0000FFFF
#define EFUSE_TPGM_M  ((EFUSE_TPGM_V)<<(EFUSE_TPGM_S))
#define EFUSE_TPGM_V  0xFFFF
#define EFUSE_TPGM_S  16
/* EFUSE_TPGM_INACTIVE : R/W ;bitpos:[15:8] ;default: 8'h1 ; */
/*description: Configures the length of pulse during programming 0 to eFuse.*/
#define EFUSE_TPGM_INACTIVE  0x000000FF
#define EFUSE_TPGM_INACTIVE_M  ((EFUSE_TPGM_INACTIVE_V)<<(EFUSE_TPGM_INACTIVE_S))
#define EFUSE_TPGM_INACTIVE_V  0xFF
#define EFUSE_TPGM_INACTIVE_S  8
/* EFUSE_THP_A : R/W ;bitpos:[7:0] ;default: 8'h1 ; */
/*description: Configures the hold time of programming operation.*/
#define EFUSE_THP_A  0x000000FF
#define EFUSE_THP_A_M  ((EFUSE_THP_A_V)<<(EFUSE_THP_A_S))
#define EFUSE_THP_A_V  0xFF
#define EFUSE_THP_A_S  0

#define EFUSE_WR_TIM_CONF1_REG          (DR_REG_EFUSE_BASE + 0x1F4)
/* EFUSE_PWR_ON_NUM : R/W ;bitpos:[23:8] ;default: 16'h2880 ; */
/*description: Configures the power up time for VDDQ.*/
#define EFUSE_PWR_ON_NUM  0x0000FFFF
#define EFUSE_PWR_ON_NUM_M  ((EFUSE_PWR_ON_NUM_V)<<(EFUSE_PWR_ON_NUM_S))
#define EFUSE_PWR_ON_NUM_V  0xFFFF
#define EFUSE_PWR_ON_NUM_S  8
/* EFUSE_TSUP_A : R/W ;bitpos:[7:0] ;default: 8'h1 ; */
/*description: Configures the setup time of programming operation.*/
#define EFUSE_TSUP_A  0x000000FF
#define EFUSE_TSUP_A_M  ((EFUSE_TSUP_A_V)<<(EFUSE_TSUP_A_S))
#define EFUSE_TSUP_A_V  0xFF
#define EFUSE_TSUP_A_S  0

#define EFUSE_WR_TIM_CONF2_REG          (DR_REG_EFUSE_BASE + 0x1F8)
/* EFUSE_PWR_OFF_NUM : R/W ;bitpos:[15:0] ;default: 16'h190 ; */
/*description: Configures the power outage time for VDDQ.*/
#define EFUSE_PWR_OFF_NUM  0x0000FFFF
#define EFUSE_PWR_OFF_NUM_M  ((EFUSE_PWR_OFF_NUM_V)<<(EFUSE_PWR_OFF_NUM_S))
#define EFUSE_PWR_OFF_NUM_V  0xFFFF
#define EFUSE_PWR_OFF_NUM_S  0

#define EFUSE_DATE_REG          (DR_REG_EFUSE_BASE + 0x1FC)
/* EFUSE_DATE : R/W ;bitpos:[31:0] ;default: 32'h19081100 ; */
/*description: Stores eFuse version.*/
#define EFUSE_DATE  0xFFFFFFFF
#define EFUSE_DATE_M  ((EFUSE_DATE_V)<<(EFUSE_DATE_S))
#define EFUSE_DATE_V  0xFFFFFFFF
#define EFUSE_DATE_S  0

#ifdef __cplusplus
}
#endif



#endif /*_SOC_EFUSE_REG_H_ */


